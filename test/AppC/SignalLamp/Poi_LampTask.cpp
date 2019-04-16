
//********************************************
//               �ܽ��������               **
//********************************************
#define POI_PWRLOST_LAMP   PB0 //��Դ����澯ָʾ��
#define POI_PWRFAULT_LAMP  PB1 //��Դ���ϸ澯ָʾ��
#define POI_DOOR_LAMP      PB2 //�Ž��澯ָʾ��

#define POI_OUTPWR1_LAMP   PB0 //�������1�澯ָʾ��
#define POI_OUTPWR2_LAMP   PB1 //�������2�澯ָʾ��
#define DL_SWR1_LAMP       PB2 //����פ����1�澯ָʾ��
#define DL_SWR2_LAMP       PB3 //����פ����2�澯ָʾ��
#define CDMA800_INPWR_LAMP PB4 //CDMA800���빦�ʸ澯ָʾ��

#define MBGSM_INPWR_LAMP   PB0 //�ƶ�GSM���빦�ʸ澯ָʾ��
#define UCGSM_INPWR_LAMP   PB1 //��ͨGSM���빦�ʸ澯ָʾ��
#define MBDCS_INPWR_LAMP   PB2 //�ƶ�DCS���빦�ʸ澯ָʾ��
#define UCDCS_INPWR_LAMP   PB3 //��ͨDCS���빦�ʸ澯ָʾ��
#define X3G1FDD_INPWR_LAMP  PB4 //3G1-FDD���빦�ʸ澯ָʾ��

#define X3G2FDD_INPWR_LAMP  PB0 //3G2-FDD���빦�ʸ澯ָʾ��
#define X3G3TDD_INPWR_LAMP  PB1 //3G3-TDD���빦�ʸ澯ָʾ��
#define TRUNK_INPWR_LAMP   PB2 //��Ⱥϵͳ���빦�ʸ澯ָʾ��

#define POI_CS1  PB6   //Ƭѡ1  PIOB��
#define POI_CS2  PB7   //Ƭѡ2  PIOB��
#define POI_OUTEN PB5  //���ʹ��  PIOB��

/*******************************/
//       ��ܽ���غ궨��      //
/*******************************/
#define ALL_SIGNAL_LAMP PB0|PB1|PB2|PB3|PB4  //��ʼ��ʱʹ��
#define POI_ALL_CTRL    POI_CS1|POI_CS2|POI_OUTEN|ALL_SIGNAL_LAMP

#define PIO_STATE &g_stPioB  //�����ڷ�������ʱ��PIOB����

//�����ݷ���ʹ�����ߣ�ʹSIGNAL_LAMP���ھ���״̬
#define SET_EN PioWrite( PIO_STATE, POI_OUTEN, PIO_SET_OUT)
//�����ݷ���ʹ�����ͣ���SIGNAL_LAMP���ݷ��ͳ�ȥ
#define CLR_EN PioWrite( PIO_STATE, POI_OUTEN, PIO_CLEAR_OUT)


//**************************ȫ�ֱ�������**************************//

//**************************ȫ�ֱ�������**************************//

//���и澯��Ϣ
extern ALARM_ITEM_UN g_uAlarmItems;
//���ĸ澯״̬
extern ALARM_STATUS_UN g_uCenterStatus;

POI_SIGNAL_LAMP_ST  g_stPoiPwrLostLamp;    //��Դ����澯ָʾ��
POI_SIGNAL_LAMP_ST  g_stPoiPwrFaultLamp;   //��Դ���ϸ澯ָʾ��
POI_SIGNAL_LAMP_ST  g_stPoiDoorLamp;       //�Ž��澯ָʾ��
POI_SIGNAL_LAMP_ST  g_stPoiOutPwr1Lamp;    //�������1�澯ָʾ��
POI_SIGNAL_LAMP_ST  g_stPoiOutPwr2Lamp;    //�������2�澯ָʾ��
POI_SIGNAL_LAMP_ST  g_stDlSwr1Lamp;        //����פ����1�澯ָʾ��
POI_SIGNAL_LAMP_ST  g_stDlSwr2Lamp;        //����פ����2�澯ָʾ��

POI_SIGNAL_LAMP_ST  g_stCdma800InPwrLamp;  //CDMA800���빦�ʸ澯ָʾ��
POI_SIGNAL_LAMP_ST  g_stMbGsmInPwrLamp;    //�ƶ�GSM���빦�ʸ澯ָʾ��
POI_SIGNAL_LAMP_ST  g_stUcGsmInPwrLamp;    //��ͨGSM���빦�ʸ澯ָʾ��
POI_SIGNAL_LAMP_ST  g_stMbDcsInPwrLamp;    //�ƶ�DCS���빦�ʸ澯ָʾ��
POI_SIGNAL_LAMP_ST  g_stUcDcsInPwrLamp;    //��ͨDCS���빦�ʸ澯ָʾ��
POI_SIGNAL_LAMP_ST  g_st3G1FDDInPwrLamp;   //3G1-FDD���빦�ʸ澯ָʾ��
POI_SIGNAL_LAMP_ST  g_st3G2FDDInPwrLamp;   //3G2-FDD���빦�ʸ澯ָʾ��
POI_SIGNAL_LAMP_ST  g_st3G3TDDInPwrLamp;   //3G3-TDD���빦�ʸ澯ָʾ��
POI_SIGNAL_LAMP_ST  g_stTrunkInPwrLamp;    //��Ⱥϵͳ���빦�ʸ澯ָʾ��

/******************************************************************/
UCHAR ucCpldAttOutStatus[4];
/******************************************************************/


/******************************************************************
  Function:    UpdateAllLamp
  Description: ���ݸ��澯�������źŵƵ�״̬
  Others:      
/******************************************************************/
void UpdateAllLamp(void)
{
    /**************************** û����˸״̬���źŵ� ****************************/
    g_stPoiPwrLostLamp.ucLampStatus = 0;    //��Դ����澯ָʾ��
    if(MYALARM.stPwrLost.ucLocalStatus == ALARM)
    {
        g_stPoiPwrLostLamp.ucLampStatus = 1;
    }

    g_stPoiPwrFaultLamp.ucLampStatus = 0;   //��Դ���ϸ澯ָʾ��
    if(MYALARM.stPwrFault.ucLocalStatus == ALARM)
    {
        g_stPoiPwrFaultLamp.ucLampStatus = 1;
    }

    g_stPoiDoorLamp.ucLampStatus = 0;    //�Ž��澯ָʾ��
    if(MYALARM.stDoor.ucLocalStatus == ALARM)
    {
        g_stPoiDoorLamp.ucLampStatus = 1;
    }

    g_stDlSwr1Lamp.ucLampStatus = 0;    //����פ����1�澯ָʾ��
    if(MYALARM.stDlSwr1.ucLocalStatus == ALARM)
    {
        g_stDlSwr1Lamp.ucLampStatus = 1;
    }
    
    g_stDlSwr2Lamp.ucLampStatus = 0;    //����פ����2�澯ָʾ��
    if(MYALARM.stDlSwr2.ucLocalStatus == ALARM)
    {
        g_stDlSwr2Lamp.ucLampStatus = 1;
    }

    /**************************** ����˸״̬���źŵ� ****************************/
    g_stPoiOutPwr1Lamp.ucLampStatus = 0;    //�������1�澯ָʾ��
    if(MYALARM.stDlOutOverPwr1.ucLocalStatus == ALARM)
    {
        g_stPoiOutPwr1Lamp.ucLampStatus = 1;   //�����ʸ澯
    }
    if(MYALARM.stDlOutUnderPwr1.ucLocalStatus == ALARM)
    {
        g_stPoiOutPwr1Lamp.ucLampStatus = 2;   //Ƿ���ʸ澯
    }
        /*********************************/
    g_stPoiOutPwr2Lamp.ucLampStatus = 0;    //�������2�澯ָʾ��
    if(MYALARM.stDlOutOverPwr2.ucLocalStatus == ALARM)
    {
        g_stPoiOutPwr2Lamp.ucLampStatus = 1;   //�����ʸ澯
    }
    if(MYALARM.stDlOutUnderPwr2.ucLocalStatus == ALARM)
    {
        g_stPoiOutPwr2Lamp.ucLampStatus = 2;   //Ƿ���ʸ澯
    }

        /*********************************/
    g_stCdma800InPwrLamp.ucLampStatus = 0;    //CDMA800���빦�ʸ澯ָʾ��
    if(MYALARM.stCdma800InOverPwr.ucLocalStatus == ALARM)
    {
        g_stCdma800InPwrLamp.ucLampStatus = 1;   //�����ʸ澯
    }
    if(MYALARM.stCdma800InUnderPwr.ucLocalStatus == ALARM)
    {
        g_stCdma800InPwrLamp.ucLampStatus = 2;   //Ƿ���ʸ澯
    }
        /*********************************/
    g_stMbGsmInPwrLamp.ucLampStatus = 0;    //�ƶ�GSM���빦�ʸ澯ָʾ��
    if(MYALARM.stMbGsmInOverPwr.ucLocalStatus == ALARM)
    {
        g_stMbGsmInPwrLamp.ucLampStatus = 1;   //�����ʸ澯
    }
    if(MYALARM.stMbGsmInUnderPwr.ucLocalStatus == ALARM)
    {
        g_stMbGsmInPwrLamp.ucLampStatus = 2;   //Ƿ���ʸ澯
    }
        /*********************************/
    g_stUcGsmInPwrLamp.ucLampStatus = 0;    //��ͨGSM���빦�ʸ澯ָʾ��
    if(MYALARM.stUcGsmInOverPwr.ucLocalStatus == ALARM)
    {
        g_stUcGsmInPwrLamp.ucLampStatus = 1;   //�����ʸ澯
    }
    if(MYALARM.stUcGsmInUnderPwr.ucLocalStatus == ALARM)
    {
        g_stUcGsmInPwrLamp.ucLampStatus = 2;   //Ƿ���ʸ澯
    }
        /*********************************/
    g_stMbDcsInPwrLamp.ucLampStatus = 0;    //�ƶ�DCS���빦�ʸ澯ָʾ��
    if(MYALARM.stMbDcsInOverPwr.ucLocalStatus == ALARM)
    {
        g_stMbDcsInPwrLamp.ucLampStatus = 1;   //�����ʸ澯
    }
    if(MYALARM.stMbDcsInUnderPwr.ucLocalStatus == ALARM)
    {
        g_stMbDcsInPwrLamp.ucLampStatus = 2;   //Ƿ���ʸ澯
    }
        /*********************************/
    g_stUcDcsInPwrLamp.ucLampStatus = 0;    //��ͨDCS���빦�ʸ澯ָʾ��
    if(MYALARM.stUcDcsInOverPwr.ucLocalStatus == ALARM)
    {
        g_stUcDcsInPwrLamp.ucLampStatus = 1;   //�����ʸ澯
    }
    if(MYALARM.stUcDcsInUnderPwr.ucLocalStatus == ALARM)
    {
        g_stUcDcsInPwrLamp.ucLampStatus = 2;   //Ƿ���ʸ澯
    }
        /*********************************/
    g_st3G1FDDInPwrLamp.ucLampStatus = 0;    //3G1-FDD���빦�ʸ澯ָʾ��
    if(MYALARM.st3G1FDDInOverPwr.ucLocalStatus == ALARM)
    {
        g_st3G1FDDInPwrLamp.ucLampStatus = 1;   //�����ʸ澯
    }
    if(MYALARM.st3G1FDDInUnderPwr.ucLocalStatus == ALARM)
    {
        g_st3G1FDDInPwrLamp.ucLampStatus = 2;   //Ƿ���ʸ澯
    }
        /*********************************/
    g_st3G2FDDInPwrLamp.ucLampStatus = 0;    //3G1-FDD���빦�ʸ澯ָʾ��
    if(MYALARM.st3G2FDDInOverPwr.ucLocalStatus == ALARM)
    {
        g_st3G2FDDInPwrLamp.ucLampStatus = 1;   //�����ʸ澯
    }
    if(MYALARM.st3G2FDDInUnderPwr.ucLocalStatus == ALARM)
    {
        g_st3G2FDDInPwrLamp.ucLampStatus = 2;   //Ƿ���ʸ澯
    }
        /*********************************/
    g_st3G3TDDInPwrLamp.ucLampStatus = 0;    //3G3-TDD���빦�ʸ澯ָʾ��
    if(MYALARM.st3G3TDDInOverPwr.ucLocalStatus == ALARM)
    {
        g_st3G3TDDInPwrLamp.ucLampStatus = 1;   //�����ʸ澯
    }
    if(MYALARM.st3G3TDDInUnderPwr.ucLocalStatus == ALARM)
    {
        g_st3G3TDDInPwrLamp.ucLampStatus = 2;   //Ƿ���ʸ澯
    }
        /*********************************/
    g_stTrunkInPwrLamp.ucLampStatus = 0;    //��Ⱥϵͳ���빦�ʸ澯ָʾ��
    if(MYALARM.stTrunkInOverPwr.ucLocalStatus == ALARM)
    {
        g_stTrunkInPwrLamp.ucLampStatus = 1;   //�����ʸ澯
    }
    if(MYALARM.stTrunkInUnderPwr.ucLocalStatus == ALARM)
    {
        g_stTrunkInPwrLamp.ucLampStatus = 2;   //Ƿ���ʸ澯
    }
        /*********************************/

}

/******************************************************************
  Function:    ControlOneLamp
  Description: ���ݸ��źŵ�״̬�����ƵƵ��������˸
  Others:      
/******************************************************************/
void ControlOneLamp(void* AbcdTemp)
{
    ULONG ulTempLampSta;
    POI_SIGNAL_LAMP_ST *g_stTempL = (POI_SIGNAL_LAMP_ST *)AbcdTemp;

    SET_EN;

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
 
    switch (g_stTempL->ucGroupNum)   //CPLD�Ŀ����������ѡ��ڼ���
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

  //��ԭ�ȱ���澯��״̬��Ȼ����±���״̬
  ulTempLampSta = ucCpldAttOutStatus[g_stTempL->ucGroupNum];
    if(g_stTempL->ucNowStatus == 1)
    {
        ulTempLampSta |= g_stTempL->ucPioNum;  //�ڲ��ı������Ƶ�����£��޸ı���״̬
    }
  else
    {
        ulTempLampSta &= ~(g_stTempL->ucPioNum);  //�ڲ��ı������Ƶ�����£��޸ı���״̬
    }
    DataToPio(PIO_STATE, ulTempLampSta, ALL_SIGNAL_LAMP);  //�澯�źŵ�״̬���
    ucCpldAttOutStatus[g_stTempL->ucGroupNum] = ulTempLampSta;
    
    ulTempLampSta = 16;
    while(ulTempLampSta--);  //��ʱ����΢��
    CLR_EN;
    ulTempLampSta = 16;
    while(ulTempLampSta--);  //��ʱ����΢��
    SET_EN;
}

/******************************************************************
  Function:    ControlAllLamp
  Description: ���ݸ��źŵ�״̬�����ƵƵ��������˸
  Others:      
/******************************************************************/
void ControlAllLamp(void)
{

    //###################### ��Դ����澯ָʾ�� ######################
    ControlOneLamp(& g_stPoiPwrLostLamp);

    //###################### ��Դ���ϸ澯ָʾ�� ######################
    ControlOneLamp(& g_stPoiPwrFaultLamp);

    //###################### �Ž��澯ָʾ�� ######################
    ControlOneLamp(& g_stPoiDoorLamp);

    //###################### ����פ����1�澯ָʾ�� ######################
    ControlOneLamp(& g_stDlSwr1Lamp);

    //###################### ����פ����2�澯ָʾ�� ######################
    ControlOneLamp(& g_stDlSwr2Lamp);

    //###################### �������1�澯ָʾ�� ######################
    ControlOneLamp(& g_stPoiOutPwr1Lamp);

    //###################### �������2�澯ָʾ�� ######################
    ControlOneLamp(& g_stPoiOutPwr2Lamp);

    //###################### CDMA800���빦�ʸ澯ָʾ�� ######################
    ControlOneLamp(& g_stCdma800InPwrLamp);

    //###################### �ƶ�GSM���빦�ʸ澯ָʾ�� ######################
    ControlOneLamp(& g_stMbGsmInPwrLamp);

    //###################### ��ͨGSM���빦�ʸ澯ָʾ�� ######################
    ControlOneLamp(& g_stUcGsmInPwrLamp);

    //###################### �ƶ�DCS���빦�ʸ澯ָʾ�� ######################
    ControlOneLamp(& g_stMbDcsInPwrLamp);

    //###################### ��ͨDCS���빦�ʸ澯ָʾ�� ######################
    ControlOneLamp(& g_stUcDcsInPwrLamp);

    //###################### 3G1-FDD���빦�ʸ澯ָʾ�� ######################
    ControlOneLamp(& g_st3G1FDDInPwrLamp);

    //###################### 3G2-FDD���빦�ʸ澯ָʾ�� ######################
    ControlOneLamp(& g_st3G2FDDInPwrLamp);

    //###################### 3G3-TDD���빦�ʸ澯ָʾ�� ######################
    ControlOneLamp(& g_st3G3TDDInPwrLamp);

    //###################### ��Ⱥϵͳ���빦�ʸ澯ָʾ�� ######################
    ControlOneLamp(& g_stTrunkInPwrLamp);

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
    PioOpen( &g_stPioA, POI_ALL_CTRL, PIO_OUTPUT);

    //�����и澯�Ƶĳ�ʼ״̬������Ϊȫ"��"
  ucCpldAttOutStatus[0] = 0;
  ucCpldAttOutStatus[1] = 0;
  ucCpldAttOutStatus[2] = 0;
  ucCpldAttOutStatus[3] = 0;
  
    /************************ �������źŵƳ�ʼ��Ӳ������ ************************/
    g_stPoiPwrLostLamp.ucPioNum =  POI_PWRLOST_LAMP   ;   //��Դ����澯ָʾ��
    g_stPoiPwrFaultLamp.ucPioNum = POI_PWRFAULT_LAMP  ;   //��Դ���ϸ澯ָʾ��
    g_stPoiDoorLamp.ucPioNum =     POI_DOOR_LAMP      ;   //�Ž��澯ָʾ�� 
                                                      
    g_stPoiOutPwr1Lamp.ucPioNum =  POI_OUTPWR1_LAMP   ;    //�������1�澯ָʾ��
    g_stPoiOutPwr2Lamp.ucPioNum =  POI_OUTPWR2_LAMP   ;    //�������2�澯ָʾ��
    g_stDlSwr1Lamp.ucPioNum =      DL_SWR1_LAMP       ;    //����פ����1�澯ָʾ��
    g_stDlSwr2Lamp.ucPioNum =      DL_SWR2_LAMP       ;    //����פ����2�澯ָʾ��
    g_stCdma800InPwrLamp.ucPioNum= CDMA800_INPWR_LAMP ;    //CDMA800���빦�ʸ澯ָʾ��
                                                      
    g_stMbGsmInPwrLamp.ucPioNum =  MBGSM_INPWR_LAMP   ;    //�ƶ�GSM���빦�ʸ澯ָʾ��
    g_stUcGsmInPwrLamp.ucPioNum =  UCGSM_INPWR_LAMP   ;    //��ͨGSM���빦�ʸ澯ָʾ��
    g_stMbDcsInPwrLamp.ucPioNum =  MBDCS_INPWR_LAMP   ;    //�ƶ�DCS���빦�ʸ澯ָʾ��
    g_stUcDcsInPwrLamp.ucPioNum =  UCDCS_INPWR_LAMP   ;    //��ͨDCS���빦�ʸ澯ָʾ��
    g_st3G1FDDInPwrLamp.ucPioNum = X3G1FDD_INPWR_LAMP  ;    //3G1-FDD���빦�ʸ澯ָʾ��
                                                      
    g_st3G2FDDInPwrLamp.ucPioNum = X3G2FDD_INPWR_LAMP  ;    //3G2-FDD���빦�ʸ澯ָʾ��
    g_st3G3TDDInPwrLamp.ucPioNum = X3G3TDD_INPWR_LAMP  ;    //3G3-TDD���빦�ʸ澯ָʾ��
    g_stTrunkInPwrLamp.ucPioNum =  TRUNK_INPWR_LAMP   ;    //��Ⱥϵͳ���빦�ʸ澯ָʾ��
    
    /************************ �������źŵƳ�ʼ��Ӳ������ ************************/
    g_stPoiPwrLostLamp.ucGroupNum =    3;   //��Դ����澯ָʾ��
    g_stPoiPwrFaultLamp.ucGroupNum =   3;   //��Դ���ϸ澯ָʾ��
    g_stPoiDoorLamp.ucGroupNum =       3;   //�Ž��澯ָʾ�� 
                                                      
    g_stPoiOutPwr1Lamp.ucGroupNum =    0;    //�������1�澯ָʾ��
    g_stPoiOutPwr2Lamp.ucGroupNum =    0;    //�������2�澯ָʾ��
    g_stDlSwr1Lamp.ucGroupNum =        0;    //����פ����1�澯ָʾ��
    g_stDlSwr2Lamp.ucGroupNum =        0;    //����פ����2�澯ָʾ��
    g_stCdma800InPwrLamp.ucGroupNum=   0;    //CDMA800���빦�ʸ澯ָʾ��
                                                      
    g_stMbGsmInPwrLamp.ucGroupNum =    1;    //�ƶ�GSM���빦�ʸ澯ָʾ��
    g_stUcGsmInPwrLamp.ucGroupNum =    1;    //��ͨGSM���빦�ʸ澯ָʾ��
    g_stMbDcsInPwrLamp.ucGroupNum =    1;    //�ƶ�DCS���빦�ʸ澯ָʾ��
    g_stUcDcsInPwrLamp.ucGroupNum =    1;    //��ͨDCS���빦�ʸ澯ָʾ��
    g_st3G1FDDInPwrLamp.ucGroupNum =   1;    //3G1-FDD���빦�ʸ澯ָʾ��
                                                      
    g_st3G2FDDInPwrLamp.ucGroupNum =   2;    //3G2-FDD���빦�ʸ澯ָʾ��
    g_st3G3TDDInPwrLamp.ucGroupNum =   2;    //3G3-TDD���빦�ʸ澯ָʾ��
    g_stTrunkInPwrLamp.ucGroupNum =    2;    //��Ⱥϵͳ���빦�ʸ澯ָʾ��

    /*************** ��ʼ�������źŵ��Ƿ�֧����˸���� 1֧�� 0��֧�� ****************/
    g_stPoiPwrLostLamp.ucIsItFlicker =    0;   //��Դ����澯ָʾ��
    g_stPoiPwrFaultLamp.ucIsItFlicker =   0;   //��Դ���ϸ澯ָʾ��
    g_stPoiDoorLamp.ucIsItFlicker =       0;   //�Ž��澯ָʾ�� 
                                                      
    g_stPoiOutPwr1Lamp.ucIsItFlicker =    1;    //�������1�澯ָʾ��
    g_stPoiOutPwr2Lamp.ucIsItFlicker =    1;    //�������2�澯ָʾ��
    g_stDlSwr1Lamp.ucIsItFlicker =        0;    //����פ����1�澯ָʾ��
    g_stDlSwr2Lamp.ucIsItFlicker =        0;    //����פ����2�澯ָʾ��
    g_stCdma800InPwrLamp.ucIsItFlicker=   1;    //CDMA800���빦�ʸ澯ָʾ��
                                                      
    g_stMbGsmInPwrLamp.ucIsItFlicker =    1;    //�ƶ�GSM���빦�ʸ澯ָʾ��
    g_stUcGsmInPwrLamp.ucIsItFlicker =    1;    //��ͨGSM���빦�ʸ澯ָʾ��
    g_stMbDcsInPwrLamp.ucIsItFlicker =    1;    //�ƶ�DCS���빦�ʸ澯ָʾ��
    g_stUcDcsInPwrLamp.ucIsItFlicker =    1;    //��ͨDCS���빦�ʸ澯ָʾ��
    g_st3G1FDDInPwrLamp.ucIsItFlicker =   1;    //3G1-FDD���빦�ʸ澯ָʾ��
                                                      
    g_st3G2FDDInPwrLamp.ucIsItFlicker =   1;    //3G2-FDD���빦�ʸ澯ָʾ��
    g_st3G3TDDInPwrLamp.ucIsItFlicker =   1;    //3G3-TDD���빦�ʸ澯ָʾ��
    g_stTrunkInPwrLamp.ucIsItFlicker =    1;    //��Ⱥϵͳ���빦�ʸ澯ָʾ��

}
