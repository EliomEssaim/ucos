
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

    //------------- 电源故障告警 -------------
    NewAlarmJudgment(&(MYALARM.stPwrFault), MYENABLE.ucPwrFault);

    //------------- 门禁告警 立刻更新本地，参考使能 -------------
    MYALARM.stDoor.ucLocalStatus =   //本地告警状态
            MYALARM.stDoor.ucRcStatus & MYENABLE.ucDoor;
    MYALARM.stDoor.ucEffectiveBit = MYENABLE.ucDoor; //是否有效=告警使能

    //------------- CDMA800输入过功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stCdma800InOverPwr), MYENABLE.ucCdma800InOverPwr);

    //------------- CDMA800输入欠功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stCdma800InUnderPwr), MYENABLE.ucCdma800InUnderPwr);

    //------------- 移动GSM输入过功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stMbGsmInOverPwr), MYENABLE.ucMbGsmInOverPwr);

    //------------- 移动GSM输入欠功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stMbGsmInUnderPwr), MYENABLE.ucMbGsmInUnderPwr);

    //------------- 联通GSM输入过功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stUcGsmInOverPwr), MYENABLE.ucUcGsmInOverPwr);

    //------------- 联通GSM输入欠功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stUcGsmInUnderPwr), MYENABLE.ucUcGsmInUnderPwr);

    //------------- 移动DCS输入过功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stMbDcsInOverPwr), MYENABLE.ucMbDcsInOverPwr);

    //------------- 移动DCS输入欠功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stMbDcsInUnderPwr), MYENABLE.ucMbDcsInUnderPwr);

    //------------- 联通DCS输入过功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stUcDcsInOverPwr), MYENABLE.ucUcDcsInOverPwr);

    //------------- 联通DCS输入欠功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stUcDcsInUnderPwr), MYENABLE.ucUcDcsInUnderPwr);

    //------------- 3G1-FDD输入过功率告警 -------------
    NewAlarmJudgment(&(MYALARM.st3G1FDDInOverPwr), MYENABLE.uc3G1FDDInOverPwr);

    //------------- 3G1-FDD输入欠功率告警 -------------
    NewAlarmJudgment(&(MYALARM.st3G1FDDInUnderPwr), MYENABLE.uc3G1FDDInUnderPwr);

    //------------- 3G2-FDD输入过功率告警 -------------
    NewAlarmJudgment(&(MYALARM.st3G2FDDInOverPwr), MYENABLE.uc3G2FDDInOverPwr);

    //------------- 3G2-FDD输入欠功率告警 -------------
    NewAlarmJudgment(&(MYALARM.st3G2FDDInUnderPwr), MYENABLE.uc3G2FDDInUnderPwr);

    //------------- 3G3-TDD输入过功率告警 -------------
    NewAlarmJudgment(&(MYALARM.st3G3TDDInOverPwr), MYENABLE.uc3G3TDDInOverPwr);

    //------------- 3G3-TDD输入欠功率告警 -------------
    NewAlarmJudgment(&(MYALARM.st3G3TDDInUnderPwr), MYENABLE.uc3G3TDDInUnderPwr);

    //------------- 集群系统输入过功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stTrunkInOverPwr), MYENABLE.ucTrunkInOverPwr);

    //------------- 集群系统输入欠功率告警 -------------
    NewAlarmJudgment(&(MYALARM.stTrunkInUnderPwr), MYENABLE.ucTrunkInUnderPwr);

    //------------- 下行输出过功率告警（通道1） -------------
    NewAlarmJudgment(&(MYALARM.stDlOutOverPwr1), MYENABLE.ucDlOutOverPwr1);

    //------------- 下行输出欠功率告警（通道1） -------------
    NewAlarmJudgment(&(MYALARM.stDlOutUnderPwr1), MYENABLE.ucDlOutUnderPwr1);

    //------------- 下行输出过功率告警（通道2） -------------
    NewAlarmJudgment(&(MYALARM.stDlOutOverPwr2), MYENABLE.ucDlOutOverPwr2);

    //------------- 下行输出欠功率告警（通道2） -------------
    NewAlarmJudgment(&(MYALARM.stDlOutUnderPwr2), MYENABLE.ucDlOutUnderPwr2);

    //------------- 下行驻波比1告警 -------------
    NewAlarmJudgment(&(MYALARM.stDlSwr1), MYENABLE.ucDlSwr1);

    //------------- 下行驻波比2告警 -------------
    NewAlarmJudgment(&(MYALARM.stDlSwr2), MYENABLE.ucDlSwr2);

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

        GetAlarmInit(); //开关量采集的初始化

        GetAlarmData(); //开关量采集，完成开关量采集结果赋值

        AdcExe();       //完成A/D采集、模拟量的告警判断

        //------------------------------------
        ACAllJudgment();  //考虑使能后，进行告警状态判断
        
        CheckAlarmStatus();

        MyDelay(990);//1990);  //延时1.99秒
        
    }//while(1)
}
