
//********************************************
//               管脚连接情况               **
//********************************************
#define POI_PWRLOST_LAMP   PB0 //电源掉电告警指示灯
#define POI_PWRFAULT_LAMP  PB1 //电源故障告警指示灯
#define POI_DOOR_LAMP      PB2 //门禁告警指示灯

#define POI_OUTPWR1_LAMP   PB0 //输出功率1告警指示灯
#define POI_OUTPWR2_LAMP   PB1 //输出功率2告警指示灯
#define DL_SWR1_LAMP       PB2 //下行驻波比1告警指示灯
#define DL_SWR2_LAMP       PB3 //下行驻波比2告警指示灯
#define CDMA800_INPWR_LAMP PB4 //CDMA800输入功率告警指示灯

#define MBGSM_INPWR_LAMP   PB0 //移动GSM输入功率告警指示灯
#define UCGSM_INPWR_LAMP   PB1 //联通GSM输入功率告警指示灯
#define MBDCS_INPWR_LAMP   PB2 //移动DCS输入功率告警指示灯
#define UCDCS_INPWR_LAMP   PB3 //联通DCS输入功率告警指示灯
#define X3G1FDD_INPWR_LAMP  PB4 //3G1-FDD输入功率告警指示灯

#define X3G2FDD_INPWR_LAMP  PB0 //3G2-FDD输入功率告警指示灯
#define X3G3TDD_INPWR_LAMP  PB1 //3G3-TDD输入功率告警指示灯
#define TRUNK_INPWR_LAMP   PB2 //集群系统输入功率告警指示灯

#define POI_CS1  PB6   //片选1  PIOB口
#define POI_CS2  PB7   //片选2  PIOB口
#define POI_OUTEN PB5  //输出使能  PIOB口

/*******************************/
//       与管脚相关宏定义      //
/*******************************/
#define ALL_SIGNAL_LAMP PB0|PB1|PB2|PB3|PB4  //初始化时使用
#define POI_ALL_CTRL    POI_CS1|POI_CS2|POI_OUTEN|ALL_SIGNAL_LAMP

#define PIO_STATE &g_stPioB  //定义在发送数据时往PIOB口送

//将数据发送使能拉高，使SIGNAL_LAMP处于就绪状态
#define SET_EN PioWrite( PIO_STATE, POI_OUTEN, PIO_SET_OUT)
//将数据发送使能拉低，将SIGNAL_LAMP数据发送出去
#define CLR_EN PioWrite( PIO_STATE, POI_OUTEN, PIO_CLEAR_OUT)


//**************************全局变量定义**************************//

//**************************全局变量申明**************************//

//所有告警信息
extern ALARM_ITEM_UN g_uAlarmItems;
//中心告警状态
extern ALARM_STATUS_UN g_uCenterStatus;

POI_SIGNAL_LAMP_ST  g_stPoiPwrLostLamp;    //电源掉电告警指示灯
POI_SIGNAL_LAMP_ST  g_stPoiPwrFaultLamp;   //电源故障告警指示灯
POI_SIGNAL_LAMP_ST  g_stPoiDoorLamp;       //门禁告警指示灯
POI_SIGNAL_LAMP_ST  g_stPoiOutPwr1Lamp;    //输出功率1告警指示灯
POI_SIGNAL_LAMP_ST  g_stPoiOutPwr2Lamp;    //输出功率2告警指示灯
POI_SIGNAL_LAMP_ST  g_stDlSwr1Lamp;        //下行驻波比1告警指示灯
POI_SIGNAL_LAMP_ST  g_stDlSwr2Lamp;        //下行驻波比2告警指示灯

POI_SIGNAL_LAMP_ST  g_stCdma800InPwrLamp;  //CDMA800输入功率告警指示灯
POI_SIGNAL_LAMP_ST  g_stMbGsmInPwrLamp;    //移动GSM输入功率告警指示灯
POI_SIGNAL_LAMP_ST  g_stUcGsmInPwrLamp;    //联通GSM输入功率告警指示灯
POI_SIGNAL_LAMP_ST  g_stMbDcsInPwrLamp;    //移动DCS输入功率告警指示灯
POI_SIGNAL_LAMP_ST  g_stUcDcsInPwrLamp;    //联通DCS输入功率告警指示灯
POI_SIGNAL_LAMP_ST  g_st3G1FDDInPwrLamp;   //3G1-FDD输入功率告警指示灯
POI_SIGNAL_LAMP_ST  g_st3G2FDDInPwrLamp;   //3G2-FDD输入功率告警指示灯
POI_SIGNAL_LAMP_ST  g_st3G3TDDInPwrLamp;   //3G3-TDD输入功率告警指示灯
POI_SIGNAL_LAMP_ST  g_stTrunkInPwrLamp;    //集群系统输入功率告警指示灯

/******************************************************************/
UCHAR ucCpldAttOutStatus[4];
/******************************************************************/


/******************************************************************
  Function:    UpdateAllLamp
  Description: 根据各告警量更新信号灯的状态
  Others:      
/******************************************************************/
void UpdateAllLamp(void)
{
    /**************************** 没有闪烁状态的信号灯 ****************************/
    g_stPoiPwrLostLamp.ucLampStatus = 0;    //电源掉电告警指示灯
    if(MYALARM.stPwrLost.ucLocalStatus == ALARM)
    {
        g_stPoiPwrLostLamp.ucLampStatus = 1;
    }

    g_stPoiPwrFaultLamp.ucLampStatus = 0;   //电源故障告警指示灯
    if(MYALARM.stPwrFault.ucLocalStatus == ALARM)
    {
        g_stPoiPwrFaultLamp.ucLampStatus = 1;
    }

    g_stPoiDoorLamp.ucLampStatus = 0;    //门禁告警指示灯
    if(MYALARM.stDoor.ucLocalStatus == ALARM)
    {
        g_stPoiDoorLamp.ucLampStatus = 1;
    }

    g_stDlSwr1Lamp.ucLampStatus = 0;    //下行驻波比1告警指示灯
    if(MYALARM.stDlSwr1.ucLocalStatus == ALARM)
    {
        g_stDlSwr1Lamp.ucLampStatus = 1;
    }
    
    g_stDlSwr2Lamp.ucLampStatus = 0;    //下行驻波比2告警指示灯
    if(MYALARM.stDlSwr2.ucLocalStatus == ALARM)
    {
        g_stDlSwr2Lamp.ucLampStatus = 1;
    }

    /**************************** 有闪烁状态的信号灯 ****************************/
    g_stPoiOutPwr1Lamp.ucLampStatus = 0;    //输出功率1告警指示灯
    if(MYALARM.stDlOutOverPwr1.ucLocalStatus == ALARM)
    {
        g_stPoiOutPwr1Lamp.ucLampStatus = 1;   //过功率告警
    }
    if(MYALARM.stDlOutUnderPwr1.ucLocalStatus == ALARM)
    {
        g_stPoiOutPwr1Lamp.ucLampStatus = 2;   //欠功率告警
    }
        /*********************************/
    g_stPoiOutPwr2Lamp.ucLampStatus = 0;    //输出功率2告警指示灯
    if(MYALARM.stDlOutOverPwr2.ucLocalStatus == ALARM)
    {
        g_stPoiOutPwr2Lamp.ucLampStatus = 1;   //过功率告警
    }
    if(MYALARM.stDlOutUnderPwr2.ucLocalStatus == ALARM)
    {
        g_stPoiOutPwr2Lamp.ucLampStatus = 2;   //欠功率告警
    }

        /*********************************/
    g_stCdma800InPwrLamp.ucLampStatus = 0;    //CDMA800输入功率告警指示灯
    if(MYALARM.stCdma800InOverPwr.ucLocalStatus == ALARM)
    {
        g_stCdma800InPwrLamp.ucLampStatus = 1;   //过功率告警
    }
    if(MYALARM.stCdma800InUnderPwr.ucLocalStatus == ALARM)
    {
        g_stCdma800InPwrLamp.ucLampStatus = 2;   //欠功率告警
    }
        /*********************************/
    g_stMbGsmInPwrLamp.ucLampStatus = 0;    //移动GSM输入功率告警指示灯
    if(MYALARM.stMbGsmInOverPwr.ucLocalStatus == ALARM)
    {
        g_stMbGsmInPwrLamp.ucLampStatus = 1;   //过功率告警
    }
    if(MYALARM.stMbGsmInUnderPwr.ucLocalStatus == ALARM)
    {
        g_stMbGsmInPwrLamp.ucLampStatus = 2;   //欠功率告警
    }
        /*********************************/
    g_stUcGsmInPwrLamp.ucLampStatus = 0;    //联通GSM输入功率告警指示灯
    if(MYALARM.stUcGsmInOverPwr.ucLocalStatus == ALARM)
    {
        g_stUcGsmInPwrLamp.ucLampStatus = 1;   //过功率告警
    }
    if(MYALARM.stUcGsmInUnderPwr.ucLocalStatus == ALARM)
    {
        g_stUcGsmInPwrLamp.ucLampStatus = 2;   //欠功率告警
    }
        /*********************************/
    g_stMbDcsInPwrLamp.ucLampStatus = 0;    //移动DCS输入功率告警指示灯
    if(MYALARM.stMbDcsInOverPwr.ucLocalStatus == ALARM)
    {
        g_stMbDcsInPwrLamp.ucLampStatus = 1;   //过功率告警
    }
    if(MYALARM.stMbDcsInUnderPwr.ucLocalStatus == ALARM)
    {
        g_stMbDcsInPwrLamp.ucLampStatus = 2;   //欠功率告警
    }
        /*********************************/
    g_stUcDcsInPwrLamp.ucLampStatus = 0;    //联通DCS输入功率告警指示灯
    if(MYALARM.stUcDcsInOverPwr.ucLocalStatus == ALARM)
    {
        g_stUcDcsInPwrLamp.ucLampStatus = 1;   //过功率告警
    }
    if(MYALARM.stUcDcsInUnderPwr.ucLocalStatus == ALARM)
    {
        g_stUcDcsInPwrLamp.ucLampStatus = 2;   //欠功率告警
    }
        /*********************************/
    g_st3G1FDDInPwrLamp.ucLampStatus = 0;    //3G1-FDD输入功率告警指示灯
    if(MYALARM.st3G1FDDInOverPwr.ucLocalStatus == ALARM)
    {
        g_st3G1FDDInPwrLamp.ucLampStatus = 1;   //过功率告警
    }
    if(MYALARM.st3G1FDDInUnderPwr.ucLocalStatus == ALARM)
    {
        g_st3G1FDDInPwrLamp.ucLampStatus = 2;   //欠功率告警
    }
        /*********************************/
    g_st3G2FDDInPwrLamp.ucLampStatus = 0;    //3G1-FDD输入功率告警指示灯
    if(MYALARM.st3G2FDDInOverPwr.ucLocalStatus == ALARM)
    {
        g_st3G2FDDInPwrLamp.ucLampStatus = 1;   //过功率告警
    }
    if(MYALARM.st3G2FDDInUnderPwr.ucLocalStatus == ALARM)
    {
        g_st3G2FDDInPwrLamp.ucLampStatus = 2;   //欠功率告警
    }
        /*********************************/
    g_st3G3TDDInPwrLamp.ucLampStatus = 0;    //3G3-TDD输入功率告警指示灯
    if(MYALARM.st3G3TDDInOverPwr.ucLocalStatus == ALARM)
    {
        g_st3G3TDDInPwrLamp.ucLampStatus = 1;   //过功率告警
    }
    if(MYALARM.st3G3TDDInUnderPwr.ucLocalStatus == ALARM)
    {
        g_st3G3TDDInPwrLamp.ucLampStatus = 2;   //欠功率告警
    }
        /*********************************/
    g_stTrunkInPwrLamp.ucLampStatus = 0;    //集群系统输入功率告警指示灯
    if(MYALARM.stTrunkInOverPwr.ucLocalStatus == ALARM)
    {
        g_stTrunkInPwrLamp.ucLampStatus = 1;   //过功率告警
    }
    if(MYALARM.stTrunkInUnderPwr.ucLocalStatus == ALARM)
    {
        g_stTrunkInPwrLamp.ucLampStatus = 2;   //欠功率告警
    }
        /*********************************/

}

/******************************************************************
  Function:    ControlOneLamp
  Description: 根据各信号灯状态，控制灯的亮灭或闪烁
  Others:      
/******************************************************************/
void ControlOneLamp(void* AbcdTemp)
{
    ULONG ulTempLampSta;
    POI_SIGNAL_LAMP_ST *g_stTempL = (POI_SIGNAL_LAMP_ST *)AbcdTemp;

    SET_EN;

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
 
    switch (g_stTempL->ucGroupNum)   //CPLD的开关量输出，选择第几组
    {
        case 0:
            DataToPio(PIO_STATE, 0, POI_CS1|POI_CS2);
            break;
        case 1:
            DataToPio(PIO_STATE, POI_CS1, POI_CS1|POI_CS2);
            break;
        case 2:
            DataToPio(PIO_STATE, POI_CS2, POI_CS1|POI_CS2);
            break;
        case 3:
            DataToPio(PIO_STATE, POI_CS1|POI_CS2, POI_CS1|POI_CS2);
            break;
        default:
            break;
    }

  //读原先本组告警灯状态，然后更新本灯状态
  ulTempLampSta = ucCpldAttOutStatus[g_stTempL->ucGroupNum];
    if(g_stTempL->ucNowStatus == 1)
    {
        ulTempLampSta |= g_stTempL->ucPioNum;  //在不改变其他灯的情况下，修改本灯状态
    }
  else
    {
        ulTempLampSta &= ~(g_stTempL->ucPioNum);  //在不改变其他灯的情况下，修改本灯状态
    }
    DataToPio(PIO_STATE, ulTempLampSta, ALL_SIGNAL_LAMP);  //告警信号灯状态输出
    ucCpldAttOutStatus[g_stTempL->ucGroupNum] = ulTempLampSta;
    
    ulTempLampSta = 16;
    while(ulTempLampSta--);  //延时若干微秒
    CLR_EN;
    ulTempLampSta = 16;
    while(ulTempLampSta--);  //延时若干微秒
    SET_EN;
}

/******************************************************************
  Function:    ControlAllLamp
  Description: 根据各信号灯状态，控制灯的亮灭或闪烁
  Others:      
/******************************************************************/
void ControlAllLamp(void)
{

    //###################### 电源掉电告警指示灯 ######################
    ControlOneLamp(& g_stPoiPwrLostLamp);

    //###################### 电源故障告警指示灯 ######################
    ControlOneLamp(& g_stPoiPwrFaultLamp);

    //###################### 门禁告警指示灯 ######################
    ControlOneLamp(& g_stPoiDoorLamp);

    //###################### 下行驻波比1告警指示灯 ######################
    ControlOneLamp(& g_stDlSwr1Lamp);

    //###################### 下行驻波比2告警指示灯 ######################
    ControlOneLamp(& g_stDlSwr2Lamp);

    //###################### 输出功率1告警指示灯 ######################
    ControlOneLamp(& g_stPoiOutPwr1Lamp);

    //###################### 输出功率2告警指示灯 ######################
    ControlOneLamp(& g_stPoiOutPwr2Lamp);

    //###################### CDMA800输入功率告警指示灯 ######################
    ControlOneLamp(& g_stCdma800InPwrLamp);

    //###################### 移动GSM输入功率告警指示灯 ######################
    ControlOneLamp(& g_stMbGsmInPwrLamp);

    //###################### 联通GSM输入功率告警指示灯 ######################
    ControlOneLamp(& g_stUcGsmInPwrLamp);

    //###################### 移动DCS输入功率告警指示灯 ######################
    ControlOneLamp(& g_stMbDcsInPwrLamp);

    //###################### 联通DCS输入功率告警指示灯 ######################
    ControlOneLamp(& g_stUcDcsInPwrLamp);

    //###################### 3G1-FDD输入功率告警指示灯 ######################
    ControlOneLamp(& g_st3G1FDDInPwrLamp);

    //###################### 3G2-FDD输入功率告警指示灯 ######################
    ControlOneLamp(& g_st3G2FDDInPwrLamp);

    //###################### 3G3-TDD输入功率告警指示灯 ######################
    ControlOneLamp(& g_st3G3TDDInPwrLamp);

    //###################### 集群系统输入功率告警指示灯 ######################
    ControlOneLamp(& g_stTrunkInPwrLamp);

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
    PioOpen( &g_stPioA, POI_ALL_CTRL, PIO_OUTPUT);

    //将所有告警灯的初始状态都设置为全"灭"
  ucCpldAttOutStatus[0] = 0;
  ucCpldAttOutStatus[1] = 0;
  ucCpldAttOutStatus[2] = 0;
  ucCpldAttOutStatus[3] = 0;
  
    /************************ 给所有信号灯初始化硬件连接 ************************/
    g_stPoiPwrLostLamp.ucPioNum =  POI_PWRLOST_LAMP   ;   //电源掉电告警指示灯
    g_stPoiPwrFaultLamp.ucPioNum = POI_PWRFAULT_LAMP  ;   //电源故障告警指示灯
    g_stPoiDoorLamp.ucPioNum =     POI_DOOR_LAMP      ;   //门禁告警指示灯 
                                                      
    g_stPoiOutPwr1Lamp.ucPioNum =  POI_OUTPWR1_LAMP   ;    //输出功率1告警指示灯
    g_stPoiOutPwr2Lamp.ucPioNum =  POI_OUTPWR2_LAMP   ;    //输出功率2告警指示灯
    g_stDlSwr1Lamp.ucPioNum =      DL_SWR1_LAMP       ;    //下行驻波比1告警指示灯
    g_stDlSwr2Lamp.ucPioNum =      DL_SWR2_LAMP       ;    //下行驻波比2告警指示灯
    g_stCdma800InPwrLamp.ucPioNum= CDMA800_INPWR_LAMP ;    //CDMA800输入功率告警指示灯
                                                      
    g_stMbGsmInPwrLamp.ucPioNum =  MBGSM_INPWR_LAMP   ;    //移动GSM输入功率告警指示灯
    g_stUcGsmInPwrLamp.ucPioNum =  UCGSM_INPWR_LAMP   ;    //联通GSM输入功率告警指示灯
    g_stMbDcsInPwrLamp.ucPioNum =  MBDCS_INPWR_LAMP   ;    //移动DCS输入功率告警指示灯
    g_stUcDcsInPwrLamp.ucPioNum =  UCDCS_INPWR_LAMP   ;    //联通DCS输入功率告警指示灯
    g_st3G1FDDInPwrLamp.ucPioNum = X3G1FDD_INPWR_LAMP  ;    //3G1-FDD输入功率告警指示灯
                                                      
    g_st3G2FDDInPwrLamp.ucPioNum = X3G2FDD_INPWR_LAMP  ;    //3G2-FDD输入功率告警指示灯
    g_st3G3TDDInPwrLamp.ucPioNum = X3G3TDD_INPWR_LAMP  ;    //3G3-TDD输入功率告警指示灯
    g_stTrunkInPwrLamp.ucPioNum =  TRUNK_INPWR_LAMP   ;    //集群系统输入功率告警指示灯
    
    /************************ 给所有信号灯初始化硬件连接 ************************/
    g_stPoiPwrLostLamp.ucGroupNum =    3;   //电源掉电告警指示灯
    g_stPoiPwrFaultLamp.ucGroupNum =   3;   //电源故障告警指示灯
    g_stPoiDoorLamp.ucGroupNum =       3;   //门禁告警指示灯 
                                                      
    g_stPoiOutPwr1Lamp.ucGroupNum =    0;    //输出功率1告警指示灯
    g_stPoiOutPwr2Lamp.ucGroupNum =    0;    //输出功率2告警指示灯
    g_stDlSwr1Lamp.ucGroupNum =        0;    //下行驻波比1告警指示灯
    g_stDlSwr2Lamp.ucGroupNum =        0;    //下行驻波比2告警指示灯
    g_stCdma800InPwrLamp.ucGroupNum=   0;    //CDMA800输入功率告警指示灯
                                                      
    g_stMbGsmInPwrLamp.ucGroupNum =    1;    //移动GSM输入功率告警指示灯
    g_stUcGsmInPwrLamp.ucGroupNum =    1;    //联通GSM输入功率告警指示灯
    g_stMbDcsInPwrLamp.ucGroupNum =    1;    //移动DCS输入功率告警指示灯
    g_stUcDcsInPwrLamp.ucGroupNum =    1;    //联通DCS输入功率告警指示灯
    g_st3G1FDDInPwrLamp.ucGroupNum =   1;    //3G1-FDD输入功率告警指示灯
                                                      
    g_st3G2FDDInPwrLamp.ucGroupNum =   2;    //3G2-FDD输入功率告警指示灯
    g_st3G3TDDInPwrLamp.ucGroupNum =   2;    //3G3-TDD输入功率告警指示灯
    g_stTrunkInPwrLamp.ucGroupNum =    2;    //集群系统输入功率告警指示灯

    /*************** 初始化所有信号灯是否支持闪烁功能 1支持 0不支持 ****************/
    g_stPoiPwrLostLamp.ucIsItFlicker =    0;   //电源掉电告警指示灯
    g_stPoiPwrFaultLamp.ucIsItFlicker =   0;   //电源故障告警指示灯
    g_stPoiDoorLamp.ucIsItFlicker =       0;   //门禁告警指示灯 
                                                      
    g_stPoiOutPwr1Lamp.ucIsItFlicker =    1;    //输出功率1告警指示灯
    g_stPoiOutPwr2Lamp.ucIsItFlicker =    1;    //输出功率2告警指示灯
    g_stDlSwr1Lamp.ucIsItFlicker =        0;    //下行驻波比1告警指示灯
    g_stDlSwr2Lamp.ucIsItFlicker =        0;    //下行驻波比2告警指示灯
    g_stCdma800InPwrLamp.ucIsItFlicker=   1;    //CDMA800输入功率告警指示灯
                                                      
    g_stMbGsmInPwrLamp.ucIsItFlicker =    1;    //移动GSM输入功率告警指示灯
    g_stUcGsmInPwrLamp.ucIsItFlicker =    1;    //联通GSM输入功率告警指示灯
    g_stMbDcsInPwrLamp.ucIsItFlicker =    1;    //移动DCS输入功率告警指示灯
    g_stUcDcsInPwrLamp.ucIsItFlicker =    1;    //联通DCS输入功率告警指示灯
    g_st3G1FDDInPwrLamp.ucIsItFlicker =   1;    //3G1-FDD输入功率告警指示灯
                                                      
    g_st3G2FDDInPwrLamp.ucIsItFlicker =   1;    //3G2-FDD输入功率告警指示灯
    g_st3G3TDDInPwrLamp.ucIsItFlicker =   1;    //3G3-TDD输入功率告警指示灯
    g_stTrunkInPwrLamp.ucIsItFlicker =    1;    //集群系统输入功率告警指示灯

}
