
/*********************************************************
  Function:    ACAllJudgment
  Description: ��ϸ澯ʹ�ܣ���ʵʱ�ɼ�����뱾�ظ澯״̬����
               �Ƚϣ��ɲɼ��ܴ�����״̬�仯���������Ƿ���ı�
               �ظ澯״̬
  Others: λ�ø澯���ⲿ�澯�����̸��±��ظ澯״̬��
*********************************************************/
void ACAllJudgment(void)
{
    //------------- ��Դ����澯 -------------
    NewAlarmJudgment(&(MYALARM.stPwrLost), MYENABLE.ucPwrLost);

    //------------- ��Դ���ϸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stPwrFault), MYENABLE.ucPwrFault);

    //------------- �Ž��澯 ���̸��±��أ��ο�ʹ�� -------------
    MYALARM.stDoor.ucLocalStatus =   //���ظ澯״̬
            MYALARM.stDoor.ucRcStatus & MYENABLE.ucDoor;
    MYALARM.stDoor.ucEffectiveBit = MYENABLE.ucDoor; //�Ƿ���Ч=�澯ʹ��

    //------------- CDMA800��������ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stCdma800InOverPwr), MYENABLE.ucCdma800InOverPwr);

    //------------- CDMA800����Ƿ���ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stCdma800InUnderPwr), MYENABLE.ucCdma800InUnderPwr);

    //------------- �ƶ�GSM��������ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stMbGsmInOverPwr), MYENABLE.ucMbGsmInOverPwr);

    //------------- �ƶ�GSM����Ƿ���ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stMbGsmInUnderPwr), MYENABLE.ucMbGsmInUnderPwr);

    //------------- ��ͨGSM��������ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stUcGsmInOverPwr), MYENABLE.ucUcGsmInOverPwr);

    //------------- ��ͨGSM����Ƿ���ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stUcGsmInUnderPwr), MYENABLE.ucUcGsmInUnderPwr);

    //------------- �ƶ�DCS��������ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stMbDcsInOverPwr), MYENABLE.ucMbDcsInOverPwr);

    //------------- �ƶ�DCS����Ƿ���ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stMbDcsInUnderPwr), MYENABLE.ucMbDcsInUnderPwr);

    //------------- ��ͨDCS��������ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stUcDcsInOverPwr), MYENABLE.ucUcDcsInOverPwr);

    //------------- ��ͨDCS����Ƿ���ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stUcDcsInUnderPwr), MYENABLE.ucUcDcsInUnderPwr);

    //------------- 3G1-FDD��������ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.st3G1FDDInOverPwr), MYENABLE.uc3G1FDDInOverPwr);

    //------------- 3G1-FDD����Ƿ���ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.st3G1FDDInUnderPwr), MYENABLE.uc3G1FDDInUnderPwr);

    //------------- 3G2-FDD��������ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.st3G2FDDInOverPwr), MYENABLE.uc3G2FDDInOverPwr);

    //------------- 3G2-FDD����Ƿ���ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.st3G2FDDInUnderPwr), MYENABLE.uc3G2FDDInUnderPwr);

    //------------- 3G3-TDD��������ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.st3G3TDDInOverPwr), MYENABLE.uc3G3TDDInOverPwr);

    //------------- 3G3-TDD����Ƿ���ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.st3G3TDDInUnderPwr), MYENABLE.uc3G3TDDInUnderPwr);

    //------------- ��Ⱥϵͳ��������ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stTrunkInOverPwr), MYENABLE.ucTrunkInOverPwr);

    //------------- ��Ⱥϵͳ����Ƿ���ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stTrunkInUnderPwr), MYENABLE.ucTrunkInUnderPwr);

    //------------- ������������ʸ澯��ͨ��1�� -------------
    NewAlarmJudgment(&(MYALARM.stDlOutOverPwr1), MYENABLE.ucDlOutOverPwr1);

    //------------- �������Ƿ���ʸ澯��ͨ��1�� -------------
    NewAlarmJudgment(&(MYALARM.stDlOutUnderPwr1), MYENABLE.ucDlOutUnderPwr1);

    //------------- ������������ʸ澯��ͨ��2�� -------------
    NewAlarmJudgment(&(MYALARM.stDlOutOverPwr2), MYENABLE.ucDlOutOverPwr2);

    //------------- �������Ƿ���ʸ澯��ͨ��2�� -------------
    NewAlarmJudgment(&(MYALARM.stDlOutUnderPwr2), MYENABLE.ucDlOutUnderPwr2);

    //------------- ����פ����1�澯 -------------
    NewAlarmJudgment(&(MYALARM.stDlSwr1), MYENABLE.ucDlSwr1);

    //------------- ����פ����2�澯 -------------
    NewAlarmJudgment(&(MYALARM.stDlSwr2), MYENABLE.ucDlSwr2);

}

/*************************************************
  Function:      ACTask
  Description:   ��������ʼ��CH����
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

        GetAlarmInit(); //�������ɼ��ĳ�ʼ��

        GetAlarmData(); //�������ɼ�����ɿ������ɼ������ֵ

        AdcExe();       //���A/D�ɼ���ģ�����ĸ澯�ж�

        //------------------------------------
        ACAllJudgment();  //����ʹ�ܺ󣬽��и澯״̬�ж�
        
        CheckAlarmStatus();

        MyDelay(990);//1990);  //��ʱ1.99��
        
    }//while(1)
}
