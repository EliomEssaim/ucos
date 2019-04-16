/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   Gen_actask.cpp
    作者:     林玮
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  
       作者:  
       描述:  代码编写完成
    2. 日期:  2006/10/30
       作者:  林玮
       描述:  修改问题MCM-9，
              增加"电源掉电告警对上行输出欠功率告警的屏蔽"。 
    3. 日期:  2006/11/01
       作者:  章杰
       描述:  修改问题MCM-11，把原代码中涉及机型判断的地方全部改为对
              机型配置表中某项或者多项值的判断。
    4. 日期:  2006/11/06
       作者:  林雨
       描述:  修改问题MCM-17，将所有带通道号的通道1的参数都映射对应的
              到不带通道号的参数上，即当设备是多通道时，查询通道1的参
              数实际访问的是对应的不带通道号的参数的地址。
    5. 日期:  2006/11/29
       作者:  林玮
       描述:  修改问题MCM-42，
              增加通道二的下行输出和驻波处理
---------------------------------------------------------------------------*/
#include <string.h>
//****************************变量申明****************************//
extern ULONG g_ulACCHSendMessageNum;
extern DEV_TYPE_TABLE_ST g_stDevTypeTable;
//****************************函数申明****************************//
extern "C" void SetAtt(void);
extern "C" void SetRfSw(void);

USHORT usOldBatValue;

/*********************************************************
  Function:    ACAllJudgment
  Description: 结合告警使能，对实时采集结果与本地告警状态进行
               比较，由采集总次数和状态变化次数决定是否更改本
               地告警状态
  Others: 位置告警和外部告警是立刻更新本地告警状态的
*********************************************************/
void ACAllJudgment(void)
{
    //------------- 电源掉电告警 -------------
    NewAlarmJudgment(&(MYALARM.stPwrLost), MYENABLE.ucPwrLost);
    //if(NewAlarmJudgment(&(MYALARM.stPwrLost), MYENABLE.ucPwrLost) == 1)
    //{
    //    if(MYALARM.stPwrLost.ucSupportedBit == 1)
    //    {
    //        ucNeedToSend = 1; //表示需要发送屏蔽信息给射频的另一端
    //        ucSendAlarmStatus |= MYALARM.stPwrLost.ucLocalStatus; //本地告警状态
    //    }
    //}

    //------------- 电源故障告警 -------------
    NewAlarmJudgment(&(MYALARM.stPwrFault), MYENABLE.ucPwrFault);
    //if(NewAlarmJudgment(&(MYALARM.stPwrFault), MYENABLE.ucPwrFault) == 1)
    //{
    //    if(MYALARM.stPwrFault.ucSupportedBit == 1)
    //    {
    //        ucNeedToSend = 1; //表示需要发送屏蔽信息给射频的另一端
    //        ucSendAlarmStatus |= MYALARM.stPwrFault.ucLocalStatus; //本地告警状态
    //    }
    //}

    //------------- 太阳能电池低电压告警 -------------
    NewAlarmJudgment(&(MYALARM.stSolarBatLowPwr), MYENABLE.ucSolarBatLowPwr);
    //if(NewAlarmJudgment(&(MYALARM.stSolarBatLowPwr), MYENABLE.ucSolarBatLowPwr) == 1)
    //{
    //    if(MYALARM.stSolarBatLowPwr.ucSupportedBit == 1)
    //    {
    //        ucNeedToSend = 1; //表示需要发送屏蔽信息给射频的另一端
    //        ucSendAlarmStatus |= MYALARM.stSolarBatLowPwr.ucLocalStatus; //本地告警状态
    //    }
    //}

   //------------- 监控模块电池故障告警 -------------
    //NewAlarmJudgment(&(MYALARM.stBatFalut), MYENABLE.ucBatFalut);
    //删除一行代码，这里只操作电池故障告警的使能，不判断告警
    MYALARM.stBatFalut.ucEffectiveBit = MYENABLE.ucBatFalut;

    //------------- 位置告警 立刻更新本地，参考使能 -------------
    //MYALARM.stPosition.ucRcStatus &= MYALARM.stPosition.ucSupportedBit; //是否支持该项
    MYALARM.stPosition.ucLocalStatus =   //本地告警状态
            MYALARM.stPosition.ucRcStatus & MYENABLE.ucPosition;
    MYALARM.stPosition.ucEffectiveBit = MYENABLE.ucPosition; //是否有效=告警使能

    //------------- 功放过温告警 -------------
    NewAlarmJudgment(&(MYALARM.stPaOverheat), MYENABLE.ucPaOverheat);

    //------------- 信源变化告警 -------------
    NewAlarmJudgment(&(MYALARM.stSrcSigChanged), MYENABLE.ucSrcSigChanged);

    //------------- 其它模块告警 -------------
    NewAlarmJudgment(&(MYALARM.stOtherModule), MYENABLE.ucOtherModule);

    //------------- 本振失锁告警 -------------
    NewAlarmJudgment(&(MYALARM.stOscUnlocked), MYENABLE.ucOscUnlocked);

    //------------- 上行低噪放故障告警 -------------
    NewAlarmJudgment(&(MYALARM.stUlLna), MYENABLE.ucUlLna);

    //------------- 下行低噪放故障告警 -------------
    NewAlarmJudgment(&(MYALARM.stDlLna), MYENABLE.ucDlLna);

    //------------- 上行功放告警 -------------
    NewAlarmJudgment(&(MYALARM.stUlPa), MYENABLE.ucUlPa);

    //------------- 下行功放告警 -------------
    NewAlarmJudgment(&(MYALARM.stDlPa), MYENABLE.ucDlPa);

    //------------- 光收发模块故障告警 -------------
    NewAlarmJudgment(&(MYALARM.stOptModule), MYENABLE.ucOptModule);

    //------------- 主从监控链路告警 -------------
    NewAlarmJudgment(&(MYALARM.stMsLink), MYENABLE.ucMsLink);

    //------------- 下行输入过功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stDlInOverPwr), MYENABLE.ucDlInOverPwr);

    //------------- 下行输入欠功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stDlInUnderPwr), MYENABLE.ucDlInUnderPwr);

    //------------- 下行输出过功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stDlOutOverPwr), MYENABLE.ucDlOutOverPwr);

    //MCM-42_20061129_linwei_begin
    //------------- 下行输出过功率告警(通道二) -------------
    NewAlarmJudgment(&(MYALARM.stDlOutOverPwr2), MYENABLE.ucDlOutOverPwr2);

    //------------- 下行输出欠功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stDlOutUnderPwr), MYENABLE.ucDlOutUnderPwr);

    //------------- 下行输出欠功率告警(通道二) -------------
    NewAlarmJudgment(&(MYALARM.stDlOutUnderPwr2), MYENABLE.ucDlOutUnderPwr2);

    //------------- 下行驻波比告警 -------------
    NewAlarmJudgment(&(MYALARM.stDlSwr), MYENABLE.ucDlSwr);

    //------------- 下行驻波比告警(通道二) -------------
    NewAlarmJudgment(&(MYALARM.stDlSwr2), MYENABLE.ucDlSwr2);
    //MCM-42_20061129_linwei_end

    //------------- 上行输出过功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stUlOutOverPwr), MYENABLE.ucUlOutOverPwr);

    //------------- 上行输出欠功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stUlOutUnderPwr), MYENABLE.ucUlOutUnderPwr);

    //------------- 上行驻波比告警 -------------
    NewAlarmJudgment(&(MYALARM.stUlSwr), MYENABLE.ucUlSwr);

    //------------- 外部告警1 立刻更新本地，参考使能 -------------
    //MYALARM.stExt1.ucRcStatus &= MYALARM.stExt1.ucSupportedBit; //是否支持该项
    MYALARM.stExt1.ucLocalStatus =   //本地告警状态
            MYALARM.stExt1.ucRcStatus & MYENABLE.ucExt1;
    MYALARM.stExt1.ucEffectiveBit = MYENABLE.ucExt1; //是否有效=告警使能
    
    //------------- 外部告警2 立刻更新本地，参考使能 -------------
    //MYALARM.stExt2.ucRcStatus &= MYALARM.stExt2.ucSupportedBit; //是否支持该项
    MYALARM.stExt2.ucLocalStatus =   //本地告警状态
            MYALARM.stExt2.ucRcStatus & MYENABLE.ucExt2;
    MYALARM.stExt2.ucEffectiveBit = MYENABLE.ucExt2; //是否有效=告警使能
    
    //------------- 外部告警3 立刻更新本地，参考使能 -------------
    //MYALARM.stExt3.ucRcStatus &= MYALARM.stExt3.ucSupportedBit; //是否支持该项
    MYALARM.stExt3.ucLocalStatus =   //本地告警状态
            MYALARM.stExt3.ucRcStatus & MYENABLE.ucExt3;
    MYALARM.stExt3.ucEffectiveBit = MYENABLE.ucExt3; //是否有效=告警使能
    
    //------------- 外部告警4 立刻更新本地，参考使能 -------------
    //MYALARM.stExt4.ucRcStatus &= MYALARM.stExt4.ucSupportedBit; //是否支持该项
    MYALARM.stExt4.ucLocalStatus =   //本地告警状态
            MYALARM.stExt4.ucRcStatus & MYENABLE.ucExt4;
    MYALARM.stExt4.ucEffectiveBit = MYENABLE.ucExt4; //是否有效=告警使能
    
    //------------- 门禁告警 立刻更新本地，参考使能 -------------
    //MYALARM.stDoor.ucRcStatus &= MYALARM.stDoor.ucSupportedBit; //是否支持该项
    MYALARM.stDoor.ucLocalStatus =   //本地告警状态
            MYALARM.stDoor.ucRcStatus & MYENABLE.ucDoor;
    MYALARM.stDoor.ucEffectiveBit = MYENABLE.ucDoor; //是否有效=告警使能
    
    //------------- 自激告警 -------------
    NewAlarmJudgment(&(MYALARM.stSelfOsc), MYENABLE.ucSelfOsc);

    //------------- 3G:GPRS登录失败告警 -------------
    NewAlarmJudgment(&(MYALARM.stGprsLoginFailed), MYENABLE.ucGprsLoginFailed);

    //------------- 3G:自激关机保护告警 -------------
    //NewAlarmJudgment(&(MYALARM.stSelfOscShutdown), MYENABLE.ucSelfOscShutdown);

    //------------- 3G:自激降增益保护告警 -------------
    //NewAlarmJudgment(&(MYALARM.stSelfOscReduceAtt), MYENABLE.ucSelfOscReduceAtt);


    //--------- 以下两个为林玮添加 ---------
    NewAlarmJudgment(&(MYALARM.stUlSelfOsc), 1);//MYENABLE.ucUlSelfOsc);//上行自激告警
    MYALARM.stUlSelfOsc.ucEffectiveBit = 0; //该告警为自定义，不参加告警判断，因此将有效标志清零
    NewAlarmJudgment(&(MYALARM.stDlSelfOsc), 1);//MYENABLE.ucDlSelfOsc);//下行自激告警
    MYALARM.stDlSelfOsc.ucEffectiveBit = 0; //该告警为自定义，不参加告警判断，因此将有效标志清零
    //--------- 以上两个为林玮添加 --------- 

    //------------------------------以下为太阳能控制器新增参数-----------------------------------

    NewAlarmJudgment(&(MYALARM.stSecStBatBlowout),      MYENABLE.ucSecStBatBlowout);
    NewAlarmJudgment(&(MYALARM.stSecSelfResumeBlowout), MYENABLE.ucSecSelfResumeBlowout);
    NewAlarmJudgment(&(MYALARM.stSecOutputOverCur),     MYENABLE.ucSecOutputOverCur);
    NewAlarmJudgment(&(MYALARM.stSecCircuitFault),      MYENABLE.ucSecCircuitFault);
    NewAlarmJudgment(&(MYALARM.stSecVol24fault),        MYENABLE.ucSecVol24fault);
    NewAlarmJudgment(&(MYALARM.stSecVol12Fault),        MYENABLE.ucSecVol12Fault);
    NewAlarmJudgment(&(MYALARM.stSecVol5Fault),         MYENABLE.ucSecVol5Fault);
    NewAlarmJudgment(&(MYALARM.stSecUseUnderVol),       MYENABLE.ucSecUseUnderVol);
    NewAlarmJudgment(&(MYALARM.stSecOverVol),           MYENABLE.ucSecOverVol);
    NewAlarmJudgment(&(MYALARM.stSecOverDischarging),   MYENABLE.ucSecOverDischarging);
    NewAlarmJudgment(&(MYALARM.stSecOverDischarged),    MYENABLE.ucSecOverDischarged);
    NewAlarmJudgment(&(MYALARM.stSecStBatFault),        MYENABLE.ucSecStBatFault);
    NewAlarmJudgment(&(MYALARM.stSecSBFault),           MYENABLE.ucSecSBFault);
    NewAlarmJudgment(&(MYALARM.stSecDoor),              MYENABLE.ucSecDoor);
    NewAlarmJudgment(&(MYALARM.stSecAleak),             MYENABLE.ucSecAleak);
    
    //------------------------------以上为太阳能控制器新增参数-----------------------------------

}

/*********************************************************
  Function:    ACAllShield
  Description: 处理特殊屏蔽
*********************************************************/
void ACAllShield(void)
{

    //当发生电源掉电告警时应屏蔽电源故障告警
    if(MYALARM.stPwrLost.ucLocalStatus == ALARM)               //电源掉电
    {
        MYALARM.stPwrFault.ucLocalStatus =
            g_uCenterStatus.stAlarmStatusStruct.ucPwrFault;
        MYALARM.stPwrFault.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stPwrFault.ucTotalCount = 0;    //采集次数清零
        MYALARM.stPwrFault.ucChangedCount = 0;  //告警状态变化次数清零
    }

    //当发生电源告警、电源故障告警、太阳能电池低电压告警或关断射频开关时，
    //则以下各项告警状态与监控中心的状态一致，且有效标志清零
    if((MYALARM.stPwrLost.ucLocalStatus == ALARM)               //电源掉电
            || (MYALARM.stPwrFault.ucLocalStatus == ALARM)      //电池故障
            || (MYALARM.stSolarBatLowPwr.ucLocalStatus == ALARM)//太阳能电池故障
            || (g_stSettingParamSet.ucRfSw == 0))   //射频开关有效且被关闭
    {
        //产生电源告警或关闭射频信号时，如果原先的状态是0，则需要向主机或从机发送新状态
        if(ucSendAlarmStatus == 0)
        {
            ucNeedToSend = 1;
            ucSendAlarmStatus = 1;
        }
        //----------- 下行输入过功率告警 被屏蔽 -----------
        MYALARM.stDlInOverPwr.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlInOverPwr; //告警中心状态
        MYALARM.stDlInOverPwr.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stDlInOverPwr.ucTotalCount = 0;    //采集次数清零
        MYALARM.stDlInOverPwr.ucChangedCount = 0;  //告警状态变化次数清零
        //----------- 下行输入欠功率告警 被屏蔽 -----------
        MYALARM.stDlInUnderPwr.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlInUnderPwr; //告警中心状态
        MYALARM.stDlInUnderPwr.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stDlInUnderPwr.ucTotalCount = 0;    //采集次数清零
        MYALARM.stDlInUnderPwr.ucChangedCount = 0;  //告警状态变化次数清零
        //----------- 下行输出欠功率告警 被屏蔽 -----------
        MYALARM.stDlOutUnderPwr.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlOutUnderPwr; //告警中心状态
        MYALARM.stDlOutUnderPwr.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stDlOutUnderPwr.ucTotalCount = 0;    //采集次数清零
        MYALARM.stDlOutUnderPwr.ucChangedCount = 0;  //告警状态变化次数清零

        //MCM-42_20061129_linwei_begin
        //----------- 下行输出欠功率告警(通道二) 被屏蔽 -----------
        MYALARM.stDlOutUnderPwr2.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlOutUnderPwr2; //告警中心状态
        MYALARM.stDlOutUnderPwr2.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stDlOutUnderPwr2.ucTotalCount = 0;    //采集次数清零
        MYALARM.stDlOutUnderPwr2.ucChangedCount = 0;  //告警状态变化次数清零
        //MCM-42_20061129_linwei_end
        
        //----------- 上行输出过功率告警 被屏蔽 -----------
        MYALARM.stUlOutOverPwr.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucUlOutOverPwr; //告警中心状态
        MYALARM.stUlOutOverPwr.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stUlOutOverPwr.ucTotalCount = 0;    //采集次数清零
        MYALARM.stUlOutOverPwr.ucChangedCount = 0;  //告警状态变化次数清零

        //MCM-9_20061030_linwei_begin
        //----------- 上行输出欠功率告警 被屏蔽 -----------
        MYALARM.stUlOutUnderPwr.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucUlOutOverPwr; //告警中心状态
        MYALARM.stUlOutUnderPwr.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stUlOutUnderPwr.ucTotalCount = 0;    //采集次数清零
        MYALARM.stUlOutUnderPwr.ucChangedCount = 0;  //告警状态变化次数清零
        //MCM-9_20061030_linwei_end
        
        //----------- 下行功放故障告警 被屏蔽 -----------
        MYALARM.stDlPa.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlPa; //告警中心状态
        MYALARM.stDlPa.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stDlPa.ucTotalCount = 0;    //采集次数清零
        MYALARM.stDlPa.ucChangedCount = 0;  //告警状态变化次数清零
        //----------- 上行功放故障告警 被屏蔽 -----------
        MYALARM.stUlPa.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucUlPa; //告警中心状态
        MYALARM.stUlPa.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stUlPa.ucTotalCount = 0;    //采集次数清零
        MYALARM.stUlPa.ucChangedCount = 0;  //告警状态变化次数清零
        //----------- 下行低噪放故障告警 被屏蔽 -----------
        MYALARM.stDlLna.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlLna; //告警中心状态
        MYALARM.stDlLna.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stDlLna.ucTotalCount = 0;    //采集次数清零
        MYALARM.stDlLna.ucChangedCount = 0;  //告警状态变化次数清零
        //----------- 上行低噪放故障告警 被屏蔽 -----------
        MYALARM.stUlLna.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucUlLna; //告警中心状态
        MYALARM.stUlLna.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stUlLna.ucTotalCount = 0;    //采集次数清零
        MYALARM.stUlLna.ucChangedCount = 0;  //告警状态变化次数清零
        //----------- 功放过温告警 被屏蔽 -----------
        MYALARM.stPaOverheat.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucPaOverheat; //告警中心状态
        MYALARM.stPaOverheat.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stPaOverheat.ucTotalCount = 0;    //采集次数清零
        MYALARM.stPaOverheat.ucChangedCount = 0;  //告警状态变化次数清零
        //----------- 其它模块告警 被屏蔽 -----------
        MYALARM.stOtherModule.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucOtherModule; //告警中心状态
        MYALARM.stOtherModule.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stOtherModule.ucTotalCount = 0;    //采集次数清零
        MYALARM.stOtherModule.ucChangedCount = 0;  //告警状态变化次数清零
        //----------- 本振失锁告警 被屏蔽 -----------
        MYALARM.stOscUnlocked.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucOscUnlocked; //告警中心状态
        MYALARM.stOscUnlocked.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stOscUnlocked.ucTotalCount = 0;    //采集次数清零
        MYALARM.stOscUnlocked.ucChangedCount = 0;  //告警状态变化次数清零
        //----------- 下行驻波告警 被屏蔽 -----------
        MYALARM.stDlSwr.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlSwr; //告警中心状态
        MYALARM.stDlSwr.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stDlSwr.ucTotalCount = 0;    //采集次数清零
        MYALARM.stDlSwr.ucChangedCount = 0;  //告警状态变化次数清零

        //MCM-42_20061129_linwei_begin
        //----------- 下行驻波告警(通道二) 被屏蔽 -----------
        MYALARM.stDlSwr2.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlSwr2; //告警中心状态
        MYALARM.stDlSwr2.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stDlSwr2.ucTotalCount = 0;    //采集次数清零
        MYALARM.stDlSwr2.ucChangedCount = 0;  //告警状态变化次数清零
        //MCM-42_20061129_linwei_end
        
        //----------- 光收发故障告警 被屏蔽 -----------
        MYALARM.stOptModule.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucOptModule; //告警中心状态
        MYALARM.stOptModule.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stOptModule.ucTotalCount = 0;    //采集次数清零
        MYALARM.stOptModule.ucChangedCount = 0;  //告警状态变化次数清零
    }
    else
    {
        //没有电源告警或开射频信号时，如果原先的状态是1，则需要向主机或从机发送新状态
        if(ucSendAlarmStatus == 1)
        {
            ucNeedToSend = 1;
            ucSendAlarmStatus = 0;
        }
    }
    
    //同时产生“下行输入功率”和下行输出功率欠功率告警，应只上报下行输入欠功率告警，
    //    屏蔽下行输出欠功率告警（查询下行输出欠功率告警显示状态为正常）
    if(MYALARM.stDlInUnderPwr.ucLocalStatus == ALARM)
    {
        MYALARM.stDlOutUnderPwr.ucLocalStatus = NOT_ALARM; //查询为不告警
        MYALARM.stDlOutUnderPwr1.ucLocalStatus = NOT_ALARM; //查询为不告警（下输出欠 通道1）
        MYALARM.stDlOutUnderPwr2.ucLocalStatus = NOT_ALARM; //查询为不告警（下输出欠 通道2）
        MYALARM.stDlOutUnderPwr.ucEffectiveBit = 0;  //是否有效清零
        MYALARM.stDlOutUnderPwr1.ucEffectiveBit = 0;  //是否有效清零（下输出欠 通道1）
        MYALARM.stDlOutUnderPwr2.ucEffectiveBit = 0;  //是否有效清零（下输出欠 通道2）
    }
    
    //同时产生“下行低噪放故障告警”和下行输出功率欠功率告警，应只上报下行低噪放故障
    //    告警，屏蔽下行输出欠功率告警（查询下行输出欠功率告警显示状态为正常）
    if(MYALARM.stDlLna.ucLocalStatus == ALARM)
    {
        MYALARM.stDlOutUnderPwr.ucLocalStatus = NOT_ALARM; //查询为不告警
        MYALARM.stDlOutUnderPwr1.ucLocalStatus = NOT_ALARM; //查询为不告警（下输出欠 通道1）
        MYALARM.stDlOutUnderPwr2.ucLocalStatus = NOT_ALARM; //查询为不告警（下输出欠 通道2）
        MYALARM.stDlOutUnderPwr.ucEffectiveBit = 0;  //是否有效清零
        MYALARM.stDlOutUnderPwr1.ucEffectiveBit = 0;  //是否有效清零（下输出欠 通道1）
        MYALARM.stDlOutUnderPwr2.ucEffectiveBit = 0;  //是否有效清零（下输出欠 通道2）
    }
    
    //同时产生“下行功放故障告警”和下行输出功率欠功率告警，应只上报下行功放故障告警，
    //    屏蔽下行输出欠功率告警（查询下行输出欠功率告警显示状态为正常）
    if(MYALARM.stDlPa.ucLocalStatus == ALARM)
    {
        MYALARM.stDlOutUnderPwr.ucLocalStatus = NOT_ALARM; //查询为不告警
        MYALARM.stDlOutUnderPwr1.ucLocalStatus = NOT_ALARM; //查询为不告警（下输出欠 通道1）
        MYALARM.stDlOutUnderPwr2.ucLocalStatus = NOT_ALARM; //查询为不告警（下输出欠 通道2）
        MYALARM.stDlOutUnderPwr.ucEffectiveBit = 0;  //是否有效清零
        MYALARM.stDlOutUnderPwr1.ucEffectiveBit = 0;  //是否有效清零（下输出欠 通道1）
        MYALARM.stDlOutUnderPwr2.ucEffectiveBit = 0;  //是否有效清零（下输出欠 通道2）
    }
    
    //同时产生“本振失锁告警”和下行输出功率欠功率告警，应只上报本振告警，屏蔽下行输出
    //    欠功率告警（查询下行输出欠功率告警显示状态为正常）
    if(MYALARM.stOscUnlocked.ucLocalStatus == ALARM)
    {
        MYALARM.stDlOutUnderPwr.ucLocalStatus = NOT_ALARM; //查询为不告警
        MYALARM.stDlOutUnderPwr1.ucLocalStatus = NOT_ALARM; //查询为不告警（下输出欠 通道1）
        MYALARM.stDlOutUnderPwr2.ucLocalStatus = NOT_ALARM; //查询为不告警（下输出欠 通道2）
        MYALARM.stDlOutUnderPwr.ucEffectiveBit = 0;  //是否有效清零
        MYALARM.stDlOutUnderPwr1.ucEffectiveBit = 0;  //是否有效清零（下输出欠 通道1）
        MYALARM.stDlOutUnderPwr2.ucEffectiveBit = 0;  //是否有效清零（下输出欠 通道2）
    }
    
    //当光纤近端机产生电源告警时，应屏蔽远端机的：下行输出欠功率告警
    if(ucDlOutUnderShield == 1)
    {
        MYALARM.stDlOutUnderPwr.ucLocalStatus = NOT_ALARM; //查询为不告警
        MYALARM.stDlOutUnderPwr.ucEffectiveBit = 0;  //是否有效清零
        MYALARM.stDlOutUnderPwr.ucTotalCount = 0;    //采集次数清零
        MYALARM.stDlOutUnderPwr.ucChangedCount = 0;  //告警状态变化次数清零

        MYALARM.stDlOutUnderPwr1.ucLocalStatus = NOT_ALARM; //查询为不告警（下输出欠 通道1）
        MYALARM.stDlOutUnderPwr1.ucEffectiveBit = 0;  //是否有效清零（下输出欠 通道1）
        MYALARM.stDlOutUnderPwr1.ucTotalCount = 0;    //采集次数清零
        MYALARM.stDlOutUnderPwr1.ucChangedCount = 0;  //告警状态变化次数清零

        MYALARM.stDlOutUnderPwr2.ucLocalStatus = NOT_ALARM; //查询为不告警（下输出欠 通道2）
        MYALARM.stDlOutUnderPwr2.ucEffectiveBit = 0;  //是否有效清零（下输出欠 通道2）
        MYALARM.stDlOutUnderPwr2.ucTotalCount = 0;    //采集次数清零
        MYALARM.stDlOutUnderPwr2.ucChangedCount = 0;  //告警状态变化次数清零
    }
    
    //当光纤远端机产生电源告警时，应屏蔽近端机的：光收发故障告警、上行输出欠功率告警
    if(ucOptShield == 1)
    {
        MYALARM.stOptModule.ucLocalStatus = NOT_ALARM; //查询为不告警
            //g_uCenterStatus.stAlarmStatusStruct.ucOptModule; //告警中心状态
        MYALARM.stOptModule.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stOptModule.ucTotalCount = 0;    //采集次数清零
        MYALARM.stOptModule.ucChangedCount = 0;  //告警状态变化次数清零
    }
//暂时不做，因为原先数据集中没有上行输出欠功率告警
    if(ucUlOutUnderShield == 1) 
    {
        MYALARM.stUlOutUnderPwr.ucLocalStatus = NOT_ALARM; //查询为不告警
            //g_uCenterStatus.stAlarmStatusStruct.ucUlOutUnderPwr; //告警中心状态
        MYALARM.stUlOutUnderPwr.ucEffectiveBit = 0;  //是否有效也清零
        MYALARM.stUlOutUnderPwr.ucTotalCount = 0;    //采集次数清零
        MYALARM.stUlOutUnderPwr.ucChangedCount = 0;  //告警状态变化次数清零
    }

}

/*************************************************
  Function:     CheckPwrLostStatus
  Description:  判断是否发生了电源掉电，如果刚刚发生电源掉电
  Calls:
  Called By:    ACTask
  Input:        
  Output:     
*************************************************/
void CheckPwrLostStatus(void)
{
    if(MYALARM.stPwrLost.ucRcStatus == NOT_ALARM) //本轮采集结果中电源掉电是非告警状态时
    {
        ucDetectBatFlag = 0;  //启动监控模块电池故障判断的标志，0表示未开始判断
    }
    else if(ucDetectBatFlag == 0) //启动监控模块电池故障判断的标志，0表示未开始判断
    {
        if(ucPwrLostOldStatus == NOT_ALARM) //前一轮电源掉电的状态如果是不告警时
        {
            ucDetectBatFlag = 1; //启动监控模块电池故障判断的标志，1表示已开始判断
            usPickBattTimes = 0; //启动判断模块电池故障判断后，采集的总次数--清零
        }
    }
    //ucPwrLostOldStatus; 前一轮电源掉电的状态，每一轮采集并处理后都更新该值
    ucPwrLostOldStatus = MYALARM.stPwrLost.ucRcStatus;
}

/*****************************************************
  Function:     ModuleBatJudge
  Description:  模块电池故障告警判断
  Calls:
  Called By:    ACTask
  Input:        ucDetectBatFlag, usPickBattTimes
  Other:        以下功能暂时不做：市电供电时，监控模块
                后备电池异常，需上报此告警。
******************************************************/
void ModuleBatJudge() //模块电池故障告警判断
{

//************* test ***********
    //g_stPrivateParam.usBattJudgeStartVoltage = 1000;
    //g_stPrivateParam.usBattJudgeStopVoltage  = 950;
/******************************/
    //如果模块电池电压数值变化太大，则丢弃本次数据
    if((g_stRcParamSet.usModuleBatValue < usOldBatValue) &&
           ((usOldBatValue - g_stRcParamSet.usModuleBatValue) > 40))
    {
        return;
    }
    else
    {
        usOldBatValue = g_stRcParamSet.usModuleBatValue;
    }

    if(ucDetectBatFlag == 1) //启动监控模块电池故障判断的标志
    {
        if(usPickBattTimes == 0)
        {
            if(g_stRcParamSet.usModuleBatValue < g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStartVoltage)
            {   //如果"监控模块电池电压"值小于电池满时电压时
                ucDetectBatFlag = 0;  //启动监控模块电池故障判断的标志，0表示未开始判断
            }
            else
            {
                usBeginBatValue = g_stRcParamSet.usModuleBatValue;  //启动监控模块电池故障判断时的模块电池电压值 100倍
                usPickBattTimes = 1;
            }
        }
        else
        {
            usPickBattTimes = usPickBattTimes + 1;
            
            if(g_stRcParamSet.usModuleBatValue < g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStopVoltage)
            {
                //如果电池已经接近没电了
                if(((g_stDevTypeTable.ucOpticalType == OPTICAL_NO) && (usPickBattTimes < ONE_HOUR_PICKTIMES)) ||
                   ((g_stDevTypeTable.ucOpticalType != OPTICAL_NO) && (usPickBattTimes < TWENTY_MINUTE_PICKTIMES)))
                {   //如果本机不是光纤机，停电后不到1小时，电池就没电了  或者
                    //    本机是非光纤机，停电后不到20分钟电池就没电了
                    MYALARM.stBatFalut.ucLocalStatus = ALARM;
                }
                else if(((g_stDevTypeTable.ucOpticalType == OPTICAL_NO) && (usPickBattTimes >= ONE_HOUR_PICKTIMES)) ||
                        ((g_stDevTypeTable.ucOpticalType != OPTICAL_NO) && (usPickBattTimes >= TWENTY_MINUTE_PICKTIMES)))
                {
                    MYALARM.stBatFalut.ucLocalStatus = NOT_ALARM;
                }
                
                ucDetectBatFlag = 0; //关闭监控模块电池故障判断的标志
            }
        }
    }
}

/*************************************************
  Function:      ACTask
  Description:   本函数初始化CH任务
  Calls:
  Called By:
  Input:          
  Output:     
  Return:         
  Others:
*************************************************/
void ACTask(void *)
{
    UCHAR j, i=0;
    UCHAR ucMasterDlInPow;  //主机上一次发送给从机的下行输入功率值
    UCHAR ucTempDlInput;  //传来的A端下行输入
    UCHAR ucBeginDownFlag=0;  //是否开始自激降增益的标志    
                              //0表示未开始降
                              //1表示已开始降，由下行自激告警引起
                              //2表示已开始降，由上行自激告警引起
                              //3表示已开始降，上、下行都自激告警引起
/************ test ************
    TestEnable();
    TestSupport();
    MYALARM.stDlSelfOsc.ucLocalStatus = NOT_ALARM;
    MYALARM.stUlSelfOsc.ucLocalStatus = NOT_ALARM;
/*******************************/

    while(1)
    {
        if(i==0)
        {
            PioWrite(&g_stPioB, PB21, LED_ON);
            i = 1;
        }
        else
        {
            PioWrite(&g_stPioB, PB21, LED_OFF);
            i = 0;
        }

        Dac0Exe(127);
        
        //判断下行输入数值是否变化，有变化的话需向从机发送新的下行输入值
        if(g_stDevTypeTable.ucRFRoleType == RF_MASTER) //本机是射频主机时，需向从机传递下行输入值
        {
            //如果上次发给从机的是“高于检测范围”
            if( ucMasterDlInPow == (0x30 + HIGHER_FLAG) )
            {   //如果当前下行输入不是“高于检测范围”
                if(g_stRcParamSet.ucDlInPwrFlag == LOWER_FLAG)
                {
                    ucNeedToSend = 1;
                    ucMasterDlInPow = 0x30 + LOWER_FLAG;
                }
                else if(g_stRcParamSet.ucDlInPwrFlag != HIGHER_FLAG)
                {
                    ucNeedToSend = 1; //是否需要向射频主或从机发送屏蔽信息的标志
                    ucMasterDlInPow = g_stRcParamSet.cDlInPwr; //更新该值
                }
            }
            //如果上次发给从机的是“低于检测范围”
            else if( ucMasterDlInPow == (0x30 + LOWER_FLAG) )
            {   //如果当前下行输入是“高于检测范围”
                if(g_stRcParamSet.ucDlInPwrFlag == HIGHER_FLAG)
                {
                    ucNeedToSend = 1;
                    ucMasterDlInPow = 0x30 + LOWER_FLAG;
                }
                else if(g_stRcParamSet.ucDlInPwrFlag != LOWER_FLAG)
                {
                    ucNeedToSend = 1; //是否需要向射频主或从机发送屏蔽信息的标志
                    ucMasterDlInPow = g_stRcParamSet.cDlInPwr; //更新该值
                }
            }
            //如果上次发给从机的是正常数值的话
            else
            {
                if(g_stRcParamSet.ucDlInPwrFlag != NORMAL_FLAG)
                {
                    ucNeedToSend = 1;
                    ucMasterDlInPow = 0x30 + g_stRcParamSet.ucDlInPwrFlag;
                }
                else if(ucMasterDlInPow != (UCHAR)(g_stRcParamSet.cDlInPwr))
                {
                    ucNeedToSend = 1; //是否需要向射频主或从机发送屏蔽信息的标志
                    //ucMasterDlInPow —— 主机上一次发送给从机的下行输入功率值
                    ucMasterDlInPow = g_stRcParamSet.cDlInPwr; //更新该值
                }
            }
        }

        GetAlarmInit(); //开关量采集的初始化

        GetAlarmData(); //开关量采集，完成开关量采集结果赋值
        CheckPwrLostStatus(); //判断是否发生了电源掉电，如果刚刚发生电源掉电

        AdcExe();       //完成A/D采集、模拟量的告警判断
        ModuleBatJudge(); //模块电池故障告警判断

        //------------------------------------
        ACAllJudgment();  //考虑使能后，进行告警状态判断

        //告警使能被关闭，应把本地告警状态和中心告警状态均设为正常
        ULONG i = 0;
        for(i = 0; i < MAX_ALARM_ITEMS; i++)
        {
            if(g_uAlarmEnable.aucAlarmEnableArray[i] == 0)
            {
                g_uAlarmItems.astAlarmItemArray[i].ucLocalStatus = 0;
                g_uCenterStatus.aucAlarmStatusArray[i] = 0;
                g_uAlarmItems.astAlarmItemArray[i].ucTotalCount = 0;
                g_uAlarmItems.astAlarmItemArray[i].ucChangedCount = 0;
            }
        }
        
        ACAllShield();  //处理特殊屏蔽

        //通道1的所有告警映射到不带通道的ID上,只需要拷贝本地告警状态和是否有效标志
        MYALARM.stDlInOverPwr1.ucLocalStatus = MYALARM.stDlInOverPwr.ucLocalStatus;
        MYALARM.stDlInOverPwr1.ucEffectiveBit = MYALARM.stDlInOverPwr.ucEffectiveBit;
        
        MYALARM.stDlInUnderPwr1.ucLocalStatus = MYALARM.stDlInUnderPwr.ucLocalStatus;
        MYALARM.stDlInUnderPwr1.ucEffectiveBit = MYALARM.stDlInUnderPwr.ucEffectiveBit;
        
        MYALARM.stDlOutOverPwr1.ucLocalStatus = MYALARM.stDlOutOverPwr.ucLocalStatus;
        MYALARM.stDlOutOverPwr1.ucEffectiveBit = MYALARM.stDlOutOverPwr.ucEffectiveBit;
        
        MYALARM.stDlOutUnderPwr1.ucLocalStatus = MYALARM.stDlOutUnderPwr.ucLocalStatus;
        MYALARM.stDlOutUnderPwr1.ucEffectiveBit = MYALARM.stDlOutUnderPwr.ucEffectiveBit;
        
        MYALARM.stUlOutOverPwr1.ucLocalStatus = MYALARM.stUlOutOverPwr.ucLocalStatus;
        MYALARM.stUlOutOverPwr1.ucEffectiveBit = MYALARM.stUlOutOverPwr.ucEffectiveBit;
        
        MYALARM.stDlSwr1.ucLocalStatus = MYALARM.stDlSwr.ucLocalStatus;
        MYALARM.stDlSwr1.ucEffectiveBit = MYALARM.stDlSwr.ucEffectiveBit;
        
        MYALARM.stUlOutUnderPwr1.ucLocalStatus = MYALARM.stUlOutUnderPwr.ucLocalStatus;
        MYALARM.stUlOutUnderPwr1.ucEffectiveBit = MYALARM.stUlOutUnderPwr.ucEffectiveBit;
        
        MYALARM.stUlSwr1.ucLocalStatus = MYALARM.stUlSwr.ucLocalStatus;
        MYALARM.stUlSwr1.ucEffectiveBit = MYALARM.stUlSwr.ucEffectiveBit;
        
        
        //----------- 自主从机之间数据传递的实现 -----------    
        if(ucNeedToSend == 1) //表示需要发送屏蔽信息给射频的另一端
        {
            //发送告警变化通知给OH
            MESSAGE_ST *pstMessage = (MESSAGE_ST *)MyMalloc(g_pstMemPool256);
            if(pstMessage == 0)
            {
                return; //没有办法
            }

            pstMessage->ulMessType = ACCH_MESS_SEND_REQ;//
            pstMessage->usDataLen  = 3;                  //数据长度            
            
            *(pstMessage->aucData) = g_stDevTypeTable.ucRFRoleType;  //射频主从机状态
            *((pstMessage->aucData) + 1) = ucSendAlarmStatus; //屏蔽状况，1为屏蔽
            *((pstMessage->aucData) + 2) = ucMasterDlInPow; //主机的下行输入功率值
            UCHAR ucTempRet;
            if(g_stDevTypeTable.ucRFRoleType == RF_MASTER)
            {
                //消息队列满则释放内存，如果向消息队列中发送成功，则把计数加1
                ucTempRet = OSQPost(g_pstCHSthQue, pstMessage);
                if(ucTempRet == OS_Q_FULL)
                {
                    MyFree((void*)pstMessage);
                }
                else if(ucTempRet == OS_NO_ERR)
                {
                    g_ulACCHSendMessageNum++;
                }
            }
            else
            {
                ucTempRet = OSQPost(g_pstCHNthQue, pstMessage);
                if(ucTempRet == OS_Q_FULL)
                {
                    MyFree((void*)pstMessage);
                }
                else if(ucTempRet == OS_NO_ERR)
                {
                    g_ulACCHSendMessageNum++;
                }
            }
            ucNeedToSend = 0; //屏蔽信息已经发送，所以清零，下次循环不再发送
        }
        //----------- 以上完成自主从机之间数据传递的实现 ----------- 

        //----------- 自激降增益等告警的实现 -----------
        if((ucBeginDownFlag & 0x02) != 0)  //已经开始自激降增益的标志，由上行自激告警引起
        {
            if(MYALARM.stUlSelfOsc.ucLocalStatus == NOT_ALARM)
            {
                ucBeginDownFlag &= 0xfd;
            }
            else if(MYALARM.stUlSelfOsc.ucRcStatus == ALARM) //如果上行还是自激
            {   //判断是否还可以降增益，不能降的话就关射频
                if(g_stSettingParamSet.ucUlAtt < 30) //如果上行衰耗1<30
                {
                    g_stSettingParamSet.ucUlAtt += 2; //步进为2，每次降低2dB增益
                    SetAtt(); //重设衰耗
                }
                else if(g_stSettingParamSet.ucUlAtt == 30) //如果上行衰耗1=30
                {
                    g_stSettingParamSet.ucUlAtt = 31; //最大只能衰耗31
                    SetAtt(); //重设衰耗
                }
                else //上行衰耗1==31，则不能再降增益，需关射频
                {
                    g_stSettingParamSet.ucRfSw = 0;  //射频信号开关状态
                    SetRfSw(); //关射频
                    MYALARM.stSelfOscShutdown.ucEffectiveBit = MYENABLE.ucSelfOscShutdown;
                    MYALARM.stSelfOscShutdown.ucLocalStatus = ALARM & MYENABLE.ucSelfOscShutdown; //3G：自激关机保护告警
                }
            }
        }
        else if(MYALARM.stUlSelfOsc.ucLocalStatus == ALARM) //如果上行自激告警产生
        {
            ucBeginDownFlag |= 0x02;
            //↓↓↓↓ 如果衰耗为31时出现自激，此处不加的话会连续上报两条告警 ↓↓↓↓
            if(g_stSettingParamSet.ucUlAtt == 31) //如果上行衰耗1==31
            {
                g_stSettingParamSet.ucRfSw = 0;  //射频信号开关状态
                SetRfSw(); //关射频
                MYALARM.stSelfOscShutdown.ucEffectiveBit = MYENABLE.ucSelfOscShutdown;
                MYALARM.stSelfOscShutdown.ucLocalStatus = ALARM & MYENABLE.ucSelfOscShutdown; //3G：自激关机保护告警
            }
            //↑↑↑↑ 如果衰耗为31时出现自激，此处不加的话会连续上报两条告警 ↑↑↑↑
        }

        if((ucBeginDownFlag & 0x01) != 0)  //已经开始自激降增益的标志，由下行自激告警引起
        {
            if(MYALARM.stDlSelfOsc.ucLocalStatus == NOT_ALARM)
            {
                ucBeginDownFlag &= 0xfe;
            }
            else if(MYALARM.stDlSelfOsc.ucRcStatus == ALARM) //如果下行还是自激
            {   //判断是否还可以降增益，不能降的话就关射频
                if(g_stSettingParamSet.ucDlAtt < 30) //如果下行衰耗1<30
                {
                    g_stSettingParamSet.ucDlAtt += 2; //步进为2，每次降低2dB增益
                    SetAtt(); //重设衰耗
                }
                else if(g_stSettingParamSet.ucDlAtt == 30) //如果下行衰耗1=30
                {
                    g_stSettingParamSet.ucDlAtt = 31; //最大只能衰耗31
                    SetAtt(); //重设衰耗
                }
                else //上行衰耗1==31，则不能再降增益，需关射频
                {
                    g_stSettingParamSet.ucRfSw = 0;  //射频信号开关状态
                    SetRfSw(); //关射频
                    MYALARM.stSelfOscShutdown.ucEffectiveBit = MYENABLE.ucSelfOscShutdown;
                    MYALARM.stSelfOscShutdown.ucLocalStatus = ALARM & MYENABLE.ucSelfOscShutdown; //3G：自激关机保护告警
                }
            }
        }
        else if(MYALARM.stDlSelfOsc.ucLocalStatus == ALARM) //如果下行自激告警产生
        {
            ucBeginDownFlag |= 0x01;
            //↓↓↓↓ 如果衰耗为31时出现自激，此处不加的话会连续上报两条告警 ↓↓↓↓
            if(g_stSettingParamSet.ucDlAtt == 31) //如果下行衰耗1==31
            {
                g_stSettingParamSet.ucRfSw = 0;  //射频信号开关状态
                SetRfSw(); //关射频
                MYALARM.stSelfOscShutdown.ucEffectiveBit = MYENABLE.ucSelfOscShutdown;
                MYALARM.stSelfOscShutdown.ucLocalStatus = ALARM & MYENABLE.ucSelfOscShutdown; //3G：自激关机保护告警
            }
            //↑↑↑↑ 如果衰耗为31时出现自激，此处不加的话会连续上报两条告警 ↑↑↑↑
        }

        if(ucBeginDownFlag != 0)
        {
            MYALARM.stSelfOscReduceAtt.ucEffectiveBit = MYENABLE.ucSelfOscReduceAtt;
            MYALARM.stSelfOscReduceAtt.ucLocalStatus = MYENABLE.ucSelfOscReduceAtt; //3G:自激降增益保护告警
        }

        //----------- 以上完成自激降增益等告警的实现 -----------

        CheckAlarmStatus();
        
        /*********** 以下为监控模块电池故障告警的判断 ************
         if((MYALARM.stPwrLost.ucTotalCount == 1) &&       //如电源掉电的总采集次数为1
                 (MYALARM.stPwrLost.ucRcStatus == ALARM))  //且电源掉电当前是掉电状态
         {
             //USHORT usBeginBatValue; 启动监控模块电池故障判断时的模块电池电压值 100倍
     	       usBeginBatValue = g_stRcParamSet.usModuleBatValue  //监控模块电池电压
     	       usBeginBatValue = 1;  //启动监控模块电池故障判断的标志，1表示已开始判断
         }
        /************ 以上为监控模块电池故障告警的判断 ************/
         
        //MyDelay(990);//1990);  //延时1.99秒
        for(j=0;j<199;j++) //每次循环都等待10ms读取队列
        {
            void *pvPrmv = OSQPend(g_pstACQue, 10/MS_PER_TICK, &g_ucErr); //DELAY 10MS
            if(pvPrmv == 0)
            {
                continue;
            }
            if(*((ULONG*)pvPrmv) == ACCH_MESS_RECV_IND)
            {
                if(*(((MESSAGE_ST *)pvPrmv)->aucData) == RF_MASTER)
                {                               //如果是射频主机发来的屏蔽信息
                    if(g_stDevTypeTable.ucRFRoleType == RF_SLAVE) //且本机是射频从机时
                    {//当光纤近端机产生电源告警或关闭射频信号时，应屏蔽远端机的：下行输出欠功率告警。
                        if(*(((MESSAGE_ST *)pvPrmv)->aucData + 1) == 1)
                        {
                            ucDlOutUnderShield = 1;
                        }
                        else
                        {
                            ucDlOutUnderShield = 0;
                        }
                      //以上得到电源掉电告警信息，以下取A端下行输入值
                        ucTempDlInput = *(((MESSAGE_ST *)pvPrmv)->aucData + 2);
                        if(ucTempDlInput == (0x30 + LOWER_FLAG))
                        {  //传来的A端下行输入低于检测范围时
                            g_stRcParamSet.ucDlInPwrFlag = LOWER_FLAG; //下行输入功率电平超出范围的标志
                            g_stRcParamSet.uc3GDlInPwrFlag = LOWER_FLAG;
                        }
                        else if(ucTempDlInput == (0x30 + HIGHER_FLAG))
                        {  //传来的A端下行输入高于检测范围时
                            g_stRcParamSet.ucDlInPwrFlag = HIGHER_FLAG; //下行输入功率电平超出范围的标志
                            g_stRcParamSet.uc3GDlInPwrFlag = HIGHER_FLAG;
                        }
                        else
                        {  //A端传来的下行输入值正常时
                            g_stRcParamSet.ucDlInPwrFlag = NORMAL_FLAG; //下行实际增益超出范围的标志
                            g_stRcParamSet.uc3GDlInPwrFlag = NORMAL_FLAG;
                            g_stRcParamSet.cDlInPwr = (CHAR)ucTempDlInput; //下行输入功率电平
                            g_stRcParamSet.sDlInPwr = g_stRcParamSet.cDlInPwr * 10;
                        }
                    }
                }
                else
                {                                //如果是射频从机发来的屏蔽信息
                    if(g_stDevTypeTable.ucRFRoleType == RF_MASTER) //且本机是射频主机时
                    {//当光纤远端机产生电源告警或关闭射频信号时，应屏蔽近端机的：光收发故障告警、上行输出欠功率告警
                        if(*(((MESSAGE_ST *)pvPrmv)->aucData + 1) == 1)
                        {
                            ucOptShield = 1;
                            ucUlOutUnderShield = 1;
                        }
                        else
                        {
                            ucOptShield = 0;
                            ucUlOutUnderShield = 0;
                        }
                    }
                }
            }
            MyFree(pvPrmv);
        }//for(j=0;j<199;j++)

    }//while(1)
}
