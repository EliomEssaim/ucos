/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   gen_lamptask.cpp
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
       描述:  修改问题MCM-40，
              把各个指示灯初始状态设为灭
    3. 日期:  2006/11/29
       作者:  章杰
       描述:  修改问题MCM-41，
              灯硬件管脚的宏定义移出到相应位置
---------------------------------------------------------------------------*/
//MCM-41_20061129_zhangjie_begin
#include "MyBoard.h"
//MCM-41_20061129_zhangjie_end
//**************************全局变量定义**************************//

//**************************全局变量申明**************************//

//所有告警信息
extern ALARM_ITEM_UN g_uAlarmItems;
//中心告警状态
extern ALARM_STATUS_UN g_uCenterStatus;

SIGNAL_LAMP_ST  g_stPowerLamp;     //整机电源告警指示灯
SIGNAL_LAMP_ST  g_stDlInPwrLamp;   //输入功率告警指示灯
SIGNAL_LAMP_ST  g_stUlOutPwrLamp;  //上行输出功率告警指示灯
SIGNAL_LAMP_ST  g_stDlOutPwrLamp;  //下行输出功率告警指示灯
SIGNAL_LAMP_ST  g_stUlSwrLamp;     //上行驻波比告警指示灯
SIGNAL_LAMP_ST  g_stDlSwrLamp;     //下行驻波比告警指示灯
SIGNAL_LAMP_ST  g_stReportLamp;    //上报状态指示灯

/******************************************************************
  Function:    SetReportLampStatus
  Description: 修改上报状态指示灯的当前状态，由其他任务执行
  Others:      开站上报、修复上报、巡检上报等
               1表示亮，0表示灭
/******************************************************************/
void SetReportLampStatus(UCHAR ucLampSta)
{
    g_stReportLamp.ucLampStatus = ucLampSta;
}

/******************************************************************
  Function:    UpdateAllLamp
  Description: 根据各告警量更新信号灯的状态
  Others:      
/******************************************************************/
void UpdateAllLamp(void)
{
    /**************************** 没有闪烁状态的信号灯 ****************************/
    g_stPowerLamp.ucLampStatus = 0;    //整机电源告警指示灯
    if((MYALARM.stPwrLost.ucLocalStatus == ALARM) ||
       (MYALARM.stPwrFault.ucLocalStatus == ALARM))
    {
        g_stPowerLamp.ucLampStatus = 1;
    }
/*
    g_stUlSwrLamp.ucLampStatus = 0;    //上行驻波比告警指示灯
    if(MYALARM.stUlSwr.ucLocalStatus == ALARM)
    {
        g_stUlSwrLamp.ucLampStatus = 1;
    }
*/
    g_stDlSwrLamp.ucLampStatus = 0;    //下行驻波比告警指示灯
    if(MYALARM.stDlSwr.ucLocalStatus == ALARM)
    {
        g_stDlSwrLamp.ucLampStatus = 1;
    }

    g_stUlOutPwrLamp.ucLampStatus = 0;    //上行输出功率告警指示灯
    if(MYALARM.stUlOutOverPwr.ucLocalStatus == ALARM)
    {
        g_stUlOutPwrLamp.ucLampStatus = 1;   //过功率告警
    }

    /**************************** 有闪烁状态的信号灯 ****************************/
    g_stDlOutPwrLamp.ucLampStatus = 0;    //下行输出功率告警指示灯
    if(MYALARM.stDlOutOverPwr.ucLocalStatus == ALARM)
    {
        g_stDlOutPwrLamp.ucLampStatus = 1;   //过功率告警
    }
    if(MYALARM.stDlOutUnderPwr.ucLocalStatus == ALARM)
    {
        g_stDlOutPwrLamp.ucLampStatus = 2;   //欠功率告警
    }

    g_stDlInPwrLamp.ucLampStatus = 0;    //输入功率告警指示灯
    if(MYALARM.stDlInOverPwr.ucLocalStatus == ALARM)
    {
        g_stDlInPwrLamp.ucLampStatus = 1;   //过功率告警
    }
    if(MYALARM.stDlInUnderPwr.ucLocalStatus == ALARM)
    {
        g_stDlInPwrLamp.ucLampStatus = 2;   //欠功率告警
    }

}

/******************************************************************
  Function:    ControlOneLamp
  Description: 根据各信号灯状态，控制灯的亮灭或闪烁
  Others:      
/******************************************************************/
void ControlOneLamp(void* AbcdTemp)
{
    SIGNAL_LAMP_ST *g_stTempL = (SIGNAL_LAMP_ST *)AbcdTemp;

    if(g_stTempL->ucIsItFlicker == 1)  //如果该灯支持闪烁
    {
        if(g_stTempL->ucLampStatus != 2)   //灯的亮灭状态 0灭 1亮 2闪烁
        {
            g_stTempL->ucNowStatus = g_stTempL->ucLampStatus;    //灯的当前状态
        }
        else
        {
            if(g_stTempL->ucNowStatus != 0)    //灯的当前状态
            {
                g_stTempL->ucNowStatus = 0;    //灯的当前状态
            }
            else g_stTempL->ucNowStatus = 1;    //灯的当前状态
        }
    }
    else  //如果该灯不支持闪烁
    {
        g_stTempL->ucNowStatus = g_stTempL->ucLampStatus;
    }

    if(g_stTempL->ucNowStatus == 1)
    {
        PioWrite( PIO_STATE, g_stTempL->ulPioNum, PIO_CLEAR_OUT);
    }
    else
    {
        PioWrite( PIO_STATE, g_stTempL->ulPioNum, PIO_SET_OUT);
    }
}

/******************************************************************
  Function:    ControlAllLamp
  Description: 根据各信号灯状态，控制灯的亮灭或闪烁
  Others:      
/******************************************************************/
void ControlAllLamp(void)
{
    //###################### 整机电源告警指示灯 ######################
    ControlOneLamp(& g_stPowerLamp);

    //###################### 输入功率告警指示灯 ######################
    ControlOneLamp(& g_stDlInPwrLamp);

    //###################### 上行输出功率告警指示灯 ######################
    ControlOneLamp(& g_stUlOutPwrLamp);

    //###################### 下行输出功率告警指示灯 ######################
    ControlOneLamp(& g_stDlOutPwrLamp);

    //###################### 上行驻波比告警指示灯 ######################
    ControlOneLamp(& g_stUlSwrLamp);

    //###################### 下行驻波比告警指示灯 ######################
    ControlOneLamp(& g_stDlSwrLamp);

    //###################### 上报状态指示灯 ######################
    ControlOneLamp(& g_stReportLamp);
}

//======================================================================
//功能：        指示灯任务的初始化
//最后修改时间：2006年7月17日
//本函数应该在任务未运行前调用
//======================================================================
void LampTaskInit(void)
{
    //创建队列
    //g_pstACQue = OSQCreate(g_apvACQue, AC_QUE_SIZE);

    //将所有指示灯的管脚受PIO控制，并设置为输出状态
    PioOpen( PIO_STATE, ALL_SIGNAL_LAMP, PIO_OUTPUT);

    /************************ 给所有信号灯初始化硬件连接 ************************/
    g_stPowerLamp.ulPioNum = POWER_LAMP;     //整机电源告警指示灯
    g_stDlInPwrLamp.ulPioNum = DL_IN_PWR_LAMP;   //输入功率告警指示灯
    g_stUlOutPwrLamp.ulPioNum = UL_OUT_PWR_LAMP;  //上行输出功率告警指示灯
    g_stDlOutPwrLamp.ulPioNum = DL_OUT_PWR_LAMP;  //下行输出功率告警指示灯
    g_stUlSwrLamp.ulPioNum = UL_SWR_LAMP;     //上行驻波比告警指示灯
    g_stDlSwrLamp.ulPioNum = DL_SWR_LAMP;     //下行驻波比告警指示灯
    g_stReportLamp.ulPioNum = REPORT_LAMP;    //上报状态指示灯

    /*************** 初始化所有信号灯是否支持闪烁功能 1支持 0不支持 ****************/
    g_stPowerLamp.ucIsItFlicker = NOT_FLICKER;   //整机电源告警指示灯
    g_stDlInPwrLamp.ucIsItFlicker = FLICKER;   //输入功率告警指示灯
    g_stUlOutPwrLamp.ucIsItFlicker = NOT_FLICKER;  //上行输出功率告警指示灯
    g_stDlOutPwrLamp.ucIsItFlicker = FLICKER;  //下行输出功率告警指示灯
    g_stUlSwrLamp.ucIsItFlicker = NOT_FLICKER;     //上行驻波比告警指示灯
    g_stDlSwrLamp.ucIsItFlicker = NOT_FLICKER;     //下行驻波比告警指示灯
    g_stReportLamp.ucIsItFlicker = NOT_FLICKER;    //上报状态指示灯

    //MCM-40_20061129_zhangjie_begin
    /*************** 初始化所有信号灯灭 ****************/
    g_stPowerLamp.ucLampStatus = 0;   //整机电源告警指示灯
    g_stDlInPwrLamp.ucLampStatus = 0;   //输入功率告警指示灯
    g_stUlOutPwrLamp.ucLampStatus = 0;  //上行输出功率告警指示灯
    g_stDlOutPwrLamp.ucLampStatus = 0;  //下行输出功率告警指示灯
    g_stUlSwrLamp.ucLampStatus = 0;     //上行驻波比告警指示灯
    g_stDlSwrLamp.ucLampStatus = 0;     //下行驻波比告警指示灯
    g_stReportLamp.ucLampStatus = 0;    //上报状态指示灯
    //MCM-40_20061129_zhangjie_end
}
