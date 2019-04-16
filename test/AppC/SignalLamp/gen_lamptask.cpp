/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   gen_lamptask.cpp
    ����:     ����
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  2006/09/28
       ����:  ����
       ����:  �������
    2. ����:  2006/11/29
       ����:  �½�
       ����:  �޸�����MCM-40��
              �Ѹ���ָʾ�Ƴ�ʼ״̬��Ϊ��
    3. ����:  2006/11/29
       ����:  �½�
       ����:  �޸�����MCM-41��
              ��Ӳ���ܽŵĺ궨���Ƴ�����Ӧλ��
---------------------------------------------------------------------------*/
//MCM-41_20061129_zhangjie_begin
#include "MyBoard.h"
//MCM-41_20061129_zhangjie_end
//**************************ȫ�ֱ�������**************************//

//**************************ȫ�ֱ�������**************************//

//���и澯��Ϣ
extern ALARM_ITEM_UN g_uAlarmItems;
//���ĸ澯״̬
extern ALARM_STATUS_UN g_uCenterStatus;

SIGNAL_LAMP_ST  g_stPowerLamp;     //������Դ�澯ָʾ��
SIGNAL_LAMP_ST  g_stDlInPwrLamp;   //���빦�ʸ澯ָʾ��
SIGNAL_LAMP_ST  g_stUlOutPwrLamp;  //����������ʸ澯ָʾ��
SIGNAL_LAMP_ST  g_stDlOutPwrLamp;  //����������ʸ澯ָʾ��
SIGNAL_LAMP_ST  g_stUlSwrLamp;     //����פ���ȸ澯ָʾ��
SIGNAL_LAMP_ST  g_stDlSwrLamp;     //����פ���ȸ澯ָʾ��
SIGNAL_LAMP_ST  g_stReportLamp;    //�ϱ�״ָ̬ʾ��

/******************************************************************
  Function:    SetReportLampStatus
  Description: �޸��ϱ�״ָ̬ʾ�Ƶĵ�ǰ״̬������������ִ��
  Others:      ��վ�ϱ����޸��ϱ���Ѳ���ϱ���
               1��ʾ����0��ʾ��
/******************************************************************/
void SetReportLampStatus(UCHAR ucLampSta)
{
    g_stReportLamp.ucLampStatus = ucLampSta;
}

/******************************************************************
  Function:    UpdateAllLamp
  Description: ���ݸ��澯�������źŵƵ�״̬
  Others:      
/******************************************************************/
void UpdateAllLamp(void)
{
    /**************************** û����˸״̬���źŵ� ****************************/
    g_stPowerLamp.ucLampStatus = 0;    //������Դ�澯ָʾ��
    if((MYALARM.stPwrLost.ucLocalStatus == ALARM) ||
       (MYALARM.stPwrFault.ucLocalStatus == ALARM))
    {
        g_stPowerLamp.ucLampStatus = 1;
    }
/*
    g_stUlSwrLamp.ucLampStatus = 0;    //����פ���ȸ澯ָʾ��
    if(MYALARM.stUlSwr.ucLocalStatus == ALARM)
    {
        g_stUlSwrLamp.ucLampStatus = 1;
    }
*/
    g_stDlSwrLamp.ucLampStatus = 0;    //����פ���ȸ澯ָʾ��
    if(MYALARM.stDlSwr.ucLocalStatus == ALARM)
    {
        g_stDlSwrLamp.ucLampStatus = 1;
    }

    g_stUlOutPwrLamp.ucLampStatus = 0;    //����������ʸ澯ָʾ��
    if(MYALARM.stUlOutOverPwr.ucLocalStatus == ALARM)
    {
        g_stUlOutPwrLamp.ucLampStatus = 1;   //�����ʸ澯
    }

    /**************************** ����˸״̬���źŵ� ****************************/
    g_stDlOutPwrLamp.ucLampStatus = 0;    //����������ʸ澯ָʾ��
    if(MYALARM.stDlOutOverPwr.ucLocalStatus == ALARM)
    {
        g_stDlOutPwrLamp.ucLampStatus = 1;   //�����ʸ澯
    }
    if(MYALARM.stDlOutUnderPwr.ucLocalStatus == ALARM)
    {
        g_stDlOutPwrLamp.ucLampStatus = 2;   //Ƿ���ʸ澯
    }

    g_stDlInPwrLamp.ucLampStatus = 0;    //���빦�ʸ澯ָʾ��
    if(MYALARM.stDlInOverPwr.ucLocalStatus == ALARM)
    {
        g_stDlInPwrLamp.ucLampStatus = 1;   //�����ʸ澯
    }
    if(MYALARM.stDlInUnderPwr.ucLocalStatus == ALARM)
    {
        g_stDlInPwrLamp.ucLampStatus = 2;   //Ƿ���ʸ澯
    }

}

/******************************************************************
  Function:    ControlOneLamp
  Description: ���ݸ��źŵ�״̬�����ƵƵ��������˸
  Others:      
/******************************************************************/
void ControlOneLamp(void* AbcdTemp)
{
    SIGNAL_LAMP_ST *g_stTempL = (SIGNAL_LAMP_ST *)AbcdTemp;

    if(g_stTempL->ucIsItFlicker == 1)  //����õ�֧����˸
    {
        if(g_stTempL->ucLampStatus != 2)   //�Ƶ�����״̬ 0�� 1�� 2��˸
        {
            g_stTempL->ucNowStatus = g_stTempL->ucLampStatus;    //�Ƶĵ�ǰ״̬
        }
        else
        {
            if(g_stTempL->ucNowStatus != 0)    //�Ƶĵ�ǰ״̬
            {
                g_stTempL->ucNowStatus = 0;    //�Ƶĵ�ǰ״̬
            }
            else g_stTempL->ucNowStatus = 1;    //�Ƶĵ�ǰ״̬
        }
    }
    else  //����õƲ�֧����˸
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
  Description: ���ݸ��źŵ�״̬�����ƵƵ��������˸
  Others:      
/******************************************************************/
void ControlAllLamp(void)
{
    //###################### ������Դ�澯ָʾ�� ######################
    ControlOneLamp(& g_stPowerLamp);

    //###################### ���빦�ʸ澯ָʾ�� ######################
    ControlOneLamp(& g_stDlInPwrLamp);

    //###################### ����������ʸ澯ָʾ�� ######################
    ControlOneLamp(& g_stUlOutPwrLamp);

    //###################### ����������ʸ澯ָʾ�� ######################
    ControlOneLamp(& g_stDlOutPwrLamp);

    //###################### ����פ���ȸ澯ָʾ�� ######################
    ControlOneLamp(& g_stUlSwrLamp);

    //###################### ����פ���ȸ澯ָʾ�� ######################
    ControlOneLamp(& g_stDlSwrLamp);

    //###################### �ϱ�״ָ̬ʾ�� ######################
    ControlOneLamp(& g_stReportLamp);
}

//======================================================================
//���ܣ�        ָʾ������ĳ�ʼ��
//����޸�ʱ�䣺2006��7��17��
//������Ӧ��������δ����ǰ����
//======================================================================
void LampTaskInit(void)
{
    //��������
    //g_pstACQue = OSQCreate(g_apvACQue, AC_QUE_SIZE);

    //������ָʾ�ƵĹܽ���PIO���ƣ�������Ϊ���״̬
    PioOpen( PIO_STATE, ALL_SIGNAL_LAMP, PIO_OUTPUT);

    /************************ �������źŵƳ�ʼ��Ӳ������ ************************/
    g_stPowerLamp.ulPioNum = POWER_LAMP;     //������Դ�澯ָʾ��
    g_stDlInPwrLamp.ulPioNum = DL_IN_PWR_LAMP;   //���빦�ʸ澯ָʾ��
    g_stUlOutPwrLamp.ulPioNum = UL_OUT_PWR_LAMP;  //����������ʸ澯ָʾ��
    g_stDlOutPwrLamp.ulPioNum = DL_OUT_PWR_LAMP;  //����������ʸ澯ָʾ��
    g_stUlSwrLamp.ulPioNum = UL_SWR_LAMP;     //����פ���ȸ澯ָʾ��
    g_stDlSwrLamp.ulPioNum = DL_SWR_LAMP;     //����פ���ȸ澯ָʾ��
    g_stReportLamp.ulPioNum = REPORT_LAMP;    //�ϱ�״ָ̬ʾ��

    /*************** ��ʼ�������źŵ��Ƿ�֧����˸���� 1֧�� 0��֧�� ****************/
    g_stPowerLamp.ucIsItFlicker = NOT_FLICKER;   //������Դ�澯ָʾ��
    g_stDlInPwrLamp.ucIsItFlicker = FLICKER;   //���빦�ʸ澯ָʾ��
    g_stUlOutPwrLamp.ucIsItFlicker = NOT_FLICKER;  //����������ʸ澯ָʾ��
    g_stDlOutPwrLamp.ucIsItFlicker = FLICKER;  //����������ʸ澯ָʾ��
    g_stUlSwrLamp.ucIsItFlicker = NOT_FLICKER;     //����פ���ȸ澯ָʾ��
    g_stDlSwrLamp.ucIsItFlicker = NOT_FLICKER;     //����פ���ȸ澯ָʾ��
    g_stReportLamp.ucIsItFlicker = NOT_FLICKER;    //�ϱ�״ָ̬ʾ��

    //MCM-40_20061129_zhangjie_begin
    /*************** ��ʼ�������źŵ��� ****************/
    g_stPowerLamp.ucLampStatus = 0;   //������Դ�澯ָʾ��
    g_stDlInPwrLamp.ucLampStatus = 0;   //���빦�ʸ澯ָʾ��
    g_stUlOutPwrLamp.ucLampStatus = 0;  //����������ʸ澯ָʾ��
    g_stDlOutPwrLamp.ucLampStatus = 0;  //����������ʸ澯ָʾ��
    g_stUlSwrLamp.ucLampStatus = 0;     //����פ���ȸ澯ָʾ��
    g_stDlSwrLamp.ucLampStatus = 0;     //����פ���ȸ澯ָʾ��
    g_stReportLamp.ucLampStatus = 0;    //�ϱ�״ָ̬ʾ��
    //MCM-40_20061129_zhangjie_end
}
