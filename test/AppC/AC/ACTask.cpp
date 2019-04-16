/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   ACTask.cpp
    作者:     林玮
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  2006/09/28
       作者:  林玮
       描述:  编码完成
    2. 日期:  2006/11/29
       作者:  章杰
       描述:  修改问题MCM-41，
              移出高于低于检测范围的宏定义
---------------------------------------------------------------------------*/
//****************************包含头文件**************************//
#include "../Util/Util.h"
#include "../AppMain.h"
#include "../OH/CMCC_1_Ext.h"
#include "../OH/YKPP.h"
#include "../MM/MemMgmt.h"
#include "../periph/usart/Usart.h"
#include "../include/Interface.h"
#include "Actask.h"

//*****************************宏定义*****************************//
//#define MYALARM   g_uAlarmItems.stAlarmItemStruct
//#define MYENABLE  g_uAlarmEnable.stAlarmEnableStruct
//#define ALARM  1
//#define NOT_ALARM  0
//#define ALARM_TIMES 36
//#define NOTALARM_TIMES 81
//#define MAX_PICK_TIMES 90

#define ONE_HOUR_PICKTIMES 1800 //1小时一共多少次采集
#define TWENTY_MINUTE_PICKTIMES 600 //20分钟一共多少次采集

//**************************全局变量定义**************************//

//**************************全局变量申明**************************//
extern SETTING_PARAM_SET_ST g_stSettingParamSet;
//告警使能
extern ALARM_ENABLE_UN g_uAlarmEnable;
//所有告警信息
extern ALARM_ITEM_UN g_uAlarmItems;
//中心告警状态
extern ALARM_STATUS_UN g_uCenterStatus;
//实时采样参数，不需要掉电保存
extern RC_PARAM_SET_ST g_stRcParamSet;

//厂家扩展参数
extern YKPP_PARAM_SET_ST g_stYkppParamSet;
//********* test ************
extern DEV_INFO_SET_ST  g_stDevInfoSet;
//***************************

//********* 射频主从机互相屏蔽机制 **********
UCHAR ucDlOutUnderShield = 0; //下行输出欠功率告警 是否被射频主机屏蔽的标志，为1时屏蔽
UCHAR ucOptShield = 0;        //光收发故障告警 是否被射频从机屏蔽的标志，为1时屏蔽
UCHAR ucUlOutUnderShield = 0; //上行输出欠功率告警 是否被射频从机屏蔽的标志，为1时屏蔽

UCHAR ucNeedToSend = 0;  //是否需要向射频主或从机发送屏蔽信息的标志，为1时表示需要发送
UCHAR ucSendAlarmStatus = 0; //发送屏蔽信息时的屏蔽状态，1为屏蔽，0为不屏蔽
/********************************************/

//*********** 监控模块电池故障告警判断使用的全局变量 *************
UCHAR ucPwrLostOldStatus;  //前一轮电源掉电的状态，每一轮采集并处理后都更新该值
UCHAR ucDetectBatFlag = 0; //启动监控模块电池故障判断的标志，1表示已开始判断
USHORT usBeginBatValue;    //启动监控模块电池故障判断时的模块电池电压值 100倍
USHORT usPickBattTimes;    //启动判断模块电池故障判断后，采集的总次数
/***********************************************************/

//****************************函数申明****************************//
extern "C" void AdcExe(void); //完成A/D采集、模拟量的告警判断
extern "C" void AdcInit(void); //完成对两个A/D采集器的初始化
extern "C" void GetAlarmInit(void); //开关量采集的初始化
extern "C" void GetAlarmData(void); //开关量采集，完成开关量采集结果赋值
extern "C" void DacInit(void);
extern "C" void Dac0Exe(USHORT usDataOut); //完成D/A_0输出
extern "C" void SetAtt(void);
extern "C" void SetRfSw(void);
void CheckAlarmStatus();

void AlarmJudgment(void* AbcTemp, UCHAR ucTheEnable);
UCHAR NewAlarmJudgment(void* AbcTemp, UCHAR ucTheEnable);
void ACAllJudgment(void);
void ACAllShield(void);
extern "C" UCHAR MasterSlaveJudge(void); //本函数判断本机型属于射频主机还是射频从机

/*************************************************
  Function:    TestEnable
  Description: 测试告警产生用，将所有或部分告警使能打开
  Input:       无
  Others:      无
*************************************************/
/*
void TestEnable(void)
{
    MYENABLE.ucPwrLost = 0;           //电源掉电告警
    MYENABLE.ucPwrFault = 0;          //电源故障告警                
    MYENABLE.ucSolarBatLowPwr = 0;    //太阳能电池低电压告警        
    MYENABLE.ucBatFalut = 1;          //监控模块电池故障告警        
    MYENABLE.ucPosition = 1;          //位置告警                    
    MYENABLE.ucPaOverheat = 1;        //功放过温告警                
    MYENABLE.ucSrcSigChanged = 1;     //信源变化告警                
    MYENABLE.ucOtherModule = 1;       //其它模块告警                
    MYENABLE.ucOscUnlocked = 1;       //本振失锁告警                
    MYENABLE.ucUlLna = 1;             //上行低噪放故障告警          
    MYENABLE.ucDlLna = 1;             //下行低噪放故障告警          
    MYENABLE.ucUlPa = 1;              //上行功放告警                
    MYENABLE.ucDlPa = 1;              //下行功放告警                
    MYENABLE.ucOptModule = 1;         //光收发模块故障告警          
    MYENABLE.ucMsLink = 1;            //主从监控链路告警            
    MYENABLE.ucDlInOverPwr = 1;       //下行输入过功率告警          
    MYENABLE.ucDlInUnderPwr = 1;      //下行输入欠功率告警          
    MYENABLE.ucDlOutOverPwr = 1;      //下行输出过功率告警          
    MYENABLE.ucDlOutUnderPwr = 1;     //下行输出欠功率告警          
    MYENABLE.ucDlSwr = 1;             //下行驻波比告警              
    MYENABLE.ucUlOutOverPwr = 1;      //上行输出过功率告警          
    MYENABLE.ucExt1 = 1;              //外部告警1～8                
    MYENABLE.ucExt2 = 1;                                            
    MYENABLE.ucExt3 = 1;                                            
    MYENABLE.ucExt4 = 1;
    MYENABLE.ucExt5 = 1;              //外部告警1～8                
    MYENABLE.ucExt6 = 1;                                            
    MYENABLE.ucExt7 = 1;                                            
    MYENABLE.ucExt8 = 1;
    MYENABLE.ucDoor = 1;              //门禁告警                    
    MYENABLE.ucSelfOsc = 1;           //自激告警                    
    MYENABLE.ucGprsLoginFailed = 1;   //3G:GPRS登录失败告警       

    MYENABLE.ucDlInOverPwr1 = 1;      //下行输入过功率告警（通道1） 
    MYENABLE.ucDlInUnderPwr1 = 1;     //下行输入欠功率告警（通道1） 
    MYENABLE.ucDlOutOverPwr1 = 1;     //下行输出过功率告警（通道1） 
    MYENABLE.ucDlOutUnderPwr1 = 1;    //下行输出欠功率告警（通道1） 
    MYENABLE.ucUlOutOverPwr1 = 1;     //上行输出过功率告警（通道1） 
    MYENABLE.ucDlSwr1 = 1;            //下行驻波比告警（通道1）     

    MYENABLE.ucDlInOverPwr2 = 1;      //下行输入过功率告警（通道2） 
    MYENABLE.ucDlInUnderPwr2 = 1;     //下行输入欠功率告警（通道2） 
    MYENABLE.ucDlOutOverPwr2 = 1;     //下行输出过功率告警（通道2） 
    MYENABLE.ucDlOutUnderPwr2 = 1;    //下行输出欠功率告警（通道2） 
    MYENABLE.ucUlOutOverPwr2 = 1;     //上行输出过功率告警（通道2） 
    MYENABLE.ucDlSwr2 = 1;            //下行驻波比告警（通道2）     

    MYENABLE.ucSelfOscShutdown = 1;   //3G:自激关机保护告警    
    MYENABLE.ucSelfOscReduceAtt = 1;  //3G:自激降增益保护告警
}
*/
/*************************************************
  Function:    TestSupport
  Description: 测试告警产生用，将所有或部分告警都设置为支持
  Input:       无
  Others:      无
*************************************************/
/*
void TestSupport(void)
{
    MYALARM.stPwrLost.ucSupportedBit = 1;           //电源掉电告警
    MYALARM.stPwrFault.ucSupportedBit = 1;          //电源故障告警                
    MYALARM.stSolarBatLowPwr.ucSupportedBit = 1;    //太阳能电池低电压告警        
    MYALARM.stBatFalut.ucSupportedBit = 1;          //监控模块电池故障告警        
    MYALARM.stPosition.ucSupportedBit = 1;          //位置告警                    
    MYALARM.stPaOverheat.ucSupportedBit = 1;        //功放过温告警                
    MYALARM.stSrcSigChanged.ucSupportedBit = 1;     //信源变化告警                
    MYALARM.stOtherModule.ucSupportedBit = 1;       //其它模块告警                
    MYALARM.stOscUnlocked.ucSupportedBit = 1;       //本振失锁告警                
    MYALARM.stUlLna.ucSupportedBit = 1;             //上行低噪放故障告警          
    MYALARM.stDlLna.ucSupportedBit = 1;             //下行低噪放故障告警          
    MYALARM.stUlPa.ucSupportedBit = 1;              //上行功放告警                
    MYALARM.stDlPa.ucSupportedBit = 1;              //下行功放告警                
    MYALARM.stOptModule.ucSupportedBit = 1;         //光收发模块故障告警          
    MYALARM.stMsLink.ucSupportedBit = 1;            //主从监控链路告警            
    MYALARM.stDlInOverPwr.ucSupportedBit = 1;       //下行输入过功率告警          
    MYALARM.stDlInUnderPwr.ucSupportedBit = 1;      //下行输入欠功率告警          
    MYALARM.stDlOutOverPwr.ucSupportedBit = 1;      //下行输出过功率告警          
    MYALARM.stDlOutUnderPwr.ucSupportedBit = 1;     //下行输出欠功率告警          
    MYALARM.stDlSwr.ucSupportedBit = 1;             //下行驻波比告警              
    MYALARM.stUlOutOverPwr.ucSupportedBit = 1;      //上行输出过功率告警          
    MYALARM.stExt1.ucSupportedBit = 1;              //外部告警1～8                
    MYALARM.stExt2.ucSupportedBit = 1;                                            
    MYALARM.stExt3.ucSupportedBit = 1;                                            
    MYALARM.stExt4.ucSupportedBit = 1;
    MYALARM.stExt5.ucSupportedBit = 1;              //外部告警1～8                
    MYALARM.stExt6.ucSupportedBit = 1;                                            
    MYALARM.stExt7.ucSupportedBit = 1;                                            
    MYALARM.stExt8.ucSupportedBit = 1;
    MYALARM.stDoor.ucSupportedBit = 1;              //门禁告警                    
    MYALARM.stSelfOsc.ucSupportedBit = 1;           //自激告警                    
    MYALARM.stGprsLoginFailed.ucSupportedBit = 1;   //3G:GPRS登录失败告警       

    MYALARM.stDlInOverPwr1.ucSupportedBit = 1;      //下行输入过功率告警（通道1） 
    MYALARM.stDlInUnderPwr1.ucSupportedBit = 1;     //下行输入欠功率告警（通道1） 
    MYALARM.stDlOutOverPwr1.ucSupportedBit = 1;     //下行输出过功率告警（通道1） 
    MYALARM.stDlOutUnderPwr1.ucSupportedBit = 1;    //下行输出欠功率告警（通道1） 
    MYALARM.stUlOutOverPwr1.ucSupportedBit = 1;     //上行输出过功率告警（通道1） 
    MYALARM.stDlSwr1.ucSupportedBit = 1;            //下行驻波比告警（通道1）     

    MYALARM.stDlInOverPwr2.ucSupportedBit = 1;      //下行输入过功率告警（通道2） 
    MYALARM.stDlInUnderPwr2.ucSupportedBit = 1;     //下行输入欠功率告警（通道2） 
    MYALARM.stDlOutOverPwr2.ucSupportedBit = 1;     //下行输出过功率告警（通道2） 
    MYALARM.stDlOutUnderPwr2.ucSupportedBit = 1;    //下行输出欠功率告警（通道2） 
    MYALARM.stUlOutOverPwr2.ucSupportedBit = 1;     //上行输出过功率告警（通道2） 
    MYALARM.stDlSwr2.ucSupportedBit = 1;            //下行驻波比告警（通道2）     

    MYALARM.stSelfOscShutdown.ucSupportedBit = 1;   //3G:自激关机保护告警    
    MYALARM.stSelfOscReduceAtt.ucSupportedBit = 1;  //3G:自激降增益保护告警  
    //--------- 以下两个为林玮添加 ---------  
    MYALARM.stUlSelfOsc.ucSupportedBit = 0;         //上行自激告警
    MYALARM.stDlSelfOsc.ucSupportedBit = 0;         //下行自激告警
    //--------- 以上两个为林玮添加 --------- 
}
*/
/*************************************************
  Function:    NewAlarmJudgment
  Description: 完成ACAllJudgment函数的具体功能实现
  Input:       AbcTemp -- 告警项的结构首地址
           ucTheEnable -- 该告警项的告警使能
  Others:      
*************************************************/
/*
void AlarmJudgment(void* AbcTemp, UCHAR ucTheEnable)
{
    ALARM_ITEM_INFO_ST *TempAlarm = (ALARM_ITEM_INFO_ST *)AbcTemp;

    //—————————————— 采集的次数+1 ————————————————
    TempAlarm->ucTotalCount += 1;
    //———————— 根据告警使能标志，更新该告警项是否有效标志 ————————
//    TempAlarm->ucRcStatus &= TempAlarm->ucSupportedBit; //是否支持该项
    TempAlarm->ucEffectiveBit = ucTheEnable;
    TempAlarm->ucRcStatus &= ucTheEnable;
    //——————————— 判断变化的次数够不够产生上报 ————————————
    if(TempAlarm->ucRcStatus != TempAlarm->ucLocalStatus)
    {
        TempAlarm->ucChangedCount += 1; //变化的次数+1
    }
    
    if(TempAlarm->ucLocalStatus == NOT_ALARM) //如果原先是没告警的
    {
        if(TempAlarm->ucChangedCount >= ALARM_TIMES)  //ALARM_TIMES = 36
        {
            TempAlarm->ucLocalStatus = ALARM; //改变本地状态为告警
            TempAlarm->ucTotalCount = 0;  //采集次数清零
            TempAlarm->ucChangedCount = 0;  //告警状态变化次数清零
        }
        else if((MAX_PICK_TIMES + (TempAlarm->ucChangedCount)
                     - (TempAlarm->ucTotalCount)) < ALARM_TIMES)
        {//如果 90+已变化次数-已采集次数<36 的话，表示不会发生告警上报，可结束本次90次采集
            TempAlarm->ucTotalCount = 0;  //采集次数清零
            TempAlarm->ucChangedCount = 0;  //告警状态变化次数清零
        }
    }
    else  //如果原先是已经告警的
    {
        if(TempAlarm->ucChangedCount >= NOTALARM_TIMES)  //NOTALARM_TIMES = 81
        {
            TempAlarm->ucLocalStatus = NOT_ALARM; //改变本地状态为正常
            TempAlarm->ucTotalCount = 0;  //采集次数清零
            TempAlarm->ucChangedCount = 0;  //告警状态变化次数清零
        }
        else if((MAX_PICK_TIMES + (TempAlarm->ucChangedCount)
                     - (TempAlarm->ucTotalCount)) < NOTALARM_TIMES)
        {//如果 90+已变化次数-已采集次数<81 的话，表示不会发生恢复上报，可结束本次90次采集
            TempAlarm->ucTotalCount = 0;  //采集次数清零
            TempAlarm->ucChangedCount = 0;  //告警状态变化次数清零
        }
    }
    //————————————— 判断是否采集次数已满 ——————————————
    if(TempAlarm->ucTotalCount == MAX_PICK_TIMES)  //如果已经采集90次
    {
        TempAlarm->ucTotalCount = 0;  //采集次数清零
        TempAlarm->ucChangedCount = 0;  //告警状态变化次数清零
    }
}
*/
/*************************************************
  Function:    NewNewAlarmJudgment
  Description: 完成ACAllJudgment函数的具体功能实现
  Input:   AbcTemp     -- 告警项的结构首地址
           ucTheEnable -- 该告警项的告警使能
  Return:      返回是否本地告警状态有改变，有改变时返回1，无改变时返回0
  Others:      修改了告警产生的计数起始时刻以及总采样周期
*************************************************/
UCHAR NewAlarmJudgment(void* AbcTemp, UCHAR ucTheEnable)
{
    UCHAR ucTempRcStatus, ucTempReturn;
    
    ucTempReturn = 0;
    
    ALARM_ITEM_INFO_ST *TempAlarm = (ALARM_ITEM_INFO_ST *)AbcTemp;

    //如果参量不支持，则置本地状态为不告警
    if(TempAlarm->ucSupportedBit == NOT_SUPPORTED)
    {
        TempAlarm->ucLocalStatus = NOT_ALARM;
        return 0;
    }

    //———————— 根据告警使能标志，更新该告警项是否有效标志 ———————
    TempAlarm->ucEffectiveBit = ucTheEnable;
    ucTempRcStatus = TempAlarm->ucRcStatus & ucTheEnable;
    //TempAlarm->ucRcStatus &= ucTheEnable;

    //如果是立即告警则不需要进行后续的计数判断
    if(TempAlarm->ucIsInstant)
    {
        if(TempAlarm->ucLocalStatus != TempAlarm->ucRcStatus)
        {
            TempAlarm->ucLocalStatus = TempAlarm->ucRcStatus;
            return 1; //状态翻转
        }
        else
        {
            return 0;
        }
    }
    
    //——————————— 判断变化的次数够不够产生上报 ————————————
    if(ucTempRcStatus != TempAlarm->ucLocalStatus)
    {
        TempAlarm->ucChangedCount += 1;  //变化的次数+1
        TempAlarm->ucTotalCount += 1; //采集的总次数+1
    }
    else if(TempAlarm->ucTotalCount != 0)  //如果采集的总次数不等于0，及已经产生过状态变化
    {
        TempAlarm->ucTotalCount += 1; //采集的总次数+1
    }

    //—————— 如果总采集次数已满，则开始判断是否要更新本地告警状态 ———————
    if(TempAlarm->ucTotalCount >= g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucTotal)  //如果已经采集90次
    {
        if(TempAlarm->ucLocalStatus == NOT_ALARM) //如果原先是没告警的
        {
            if(TempAlarm->ucChangedCount >= g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucOccurLimit)  //ALARM_TIMES = 36
            {
                TempAlarm->ucLocalStatus = ALARM; //改变本地状态为告警
                ucTempReturn = 1;
            }
        }
        else  //如果原先是已经告警的
        {
            if(TempAlarm->ucChangedCount >= g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucEliminateLimit)  //NOTALARM_TIMES = 81
            {
                TempAlarm->ucLocalStatus = NOT_ALARM; //改变本地状态为正常
                ucTempReturn = 1;
            }
        }
        TempAlarm->ucTotalCount = 0;  //采集次数清零
        TempAlarm->ucChangedCount = 0;  //告警状态变化次数清零    
    }
    
    return ucTempReturn;
}

//本函数应该在任务未运行前调用
void ACTaskInit()
{
    //创建队列
    g_pstACQue = OSQCreate(g_apvACQue, AC_QUE_SIZE);

    AdcInit();      //完成对两个A/D采集器的初始化

    DacInit();      //完成对两个D/A的初始化
}

//#define YK_POI

#ifdef YK_POI
#include "./poi_actask.cpp"
#else
#include "./gen_actask.cpp"
#endif
