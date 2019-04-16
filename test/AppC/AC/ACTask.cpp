/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   ACTask.cpp
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
       ����:  �޸�����MCM-41��
              �Ƴ����ڵ��ڼ�ⷶΧ�ĺ궨��
---------------------------------------------------------------------------*/
//****************************����ͷ�ļ�**************************//
#include "../Util/Util.h"
#include "../AppMain.h"
#include "../OH/CMCC_1_Ext.h"
#include "../OH/YKPP.h"
#include "../MM/MemMgmt.h"
#include "../periph/usart/Usart.h"
#include "../include/Interface.h"
#include "Actask.h"

//*****************************�궨��*****************************//
//#define MYALARM   g_uAlarmItems.stAlarmItemStruct
//#define MYENABLE  g_uAlarmEnable.stAlarmEnableStruct
//#define ALARM  1
//#define NOT_ALARM  0
//#define ALARM_TIMES 36
//#define NOTALARM_TIMES 81
//#define MAX_PICK_TIMES 90

#define ONE_HOUR_PICKTIMES 1800 //1Сʱһ�����ٴβɼ�
#define TWENTY_MINUTE_PICKTIMES 600 //20����һ�����ٴβɼ�

//**************************ȫ�ֱ�������**************************//

//**************************ȫ�ֱ�������**************************//
extern SETTING_PARAM_SET_ST g_stSettingParamSet;
//�澯ʹ��
extern ALARM_ENABLE_UN g_uAlarmEnable;
//���и澯��Ϣ
extern ALARM_ITEM_UN g_uAlarmItems;
//���ĸ澯״̬
extern ALARM_STATUS_UN g_uCenterStatus;
//ʵʱ��������������Ҫ���籣��
extern RC_PARAM_SET_ST g_stRcParamSet;

//������չ����
extern YKPP_PARAM_SET_ST g_stYkppParamSet;
//********* test ************
extern DEV_INFO_SET_ST  g_stDevInfoSet;
//***************************

//********* ��Ƶ���ӻ��������λ��� **********
UCHAR ucDlOutUnderShield = 0; //�������Ƿ���ʸ澯 �Ƿ���Ƶ�������εı�־��Ϊ1ʱ����
UCHAR ucOptShield = 0;        //���շ����ϸ澯 �Ƿ���Ƶ�ӻ����εı�־��Ϊ1ʱ����
UCHAR ucUlOutUnderShield = 0; //�������Ƿ���ʸ澯 �Ƿ���Ƶ�ӻ����εı�־��Ϊ1ʱ����

UCHAR ucNeedToSend = 0;  //�Ƿ���Ҫ����Ƶ����ӻ�����������Ϣ�ı�־��Ϊ1ʱ��ʾ��Ҫ����
UCHAR ucSendAlarmStatus = 0; //����������Ϣʱ������״̬��1Ϊ���Σ�0Ϊ������
/********************************************/

//*********** ���ģ���ع��ϸ澯�ж�ʹ�õ�ȫ�ֱ��� *************
UCHAR ucPwrLostOldStatus;  //ǰһ�ֵ�Դ�����״̬��ÿһ�ֲɼ�������󶼸��¸�ֵ
UCHAR ucDetectBatFlag = 0; //�������ģ���ع����жϵı�־��1��ʾ�ѿ�ʼ�ж�
USHORT usBeginBatValue;    //�������ģ���ع����ж�ʱ��ģ���ص�ѹֵ 100��
USHORT usPickBattTimes;    //�����ж�ģ���ع����жϺ󣬲ɼ����ܴ���
/***********************************************************/

//****************************��������****************************//
extern "C" void AdcExe(void); //���A/D�ɼ���ģ�����ĸ澯�ж�
extern "C" void AdcInit(void); //��ɶ�����A/D�ɼ����ĳ�ʼ��
extern "C" void GetAlarmInit(void); //�������ɼ��ĳ�ʼ��
extern "C" void GetAlarmData(void); //�������ɼ�����ɿ������ɼ������ֵ
extern "C" void DacInit(void);
extern "C" void Dac0Exe(USHORT usDataOut); //���D/A_0���
extern "C" void SetAtt(void);
extern "C" void SetRfSw(void);
void CheckAlarmStatus();

void AlarmJudgment(void* AbcTemp, UCHAR ucTheEnable);
UCHAR NewAlarmJudgment(void* AbcTemp, UCHAR ucTheEnable);
void ACAllJudgment(void);
void ACAllShield(void);
extern "C" UCHAR MasterSlaveJudge(void); //�������жϱ�����������Ƶ����������Ƶ�ӻ�

/*************************************************
  Function:    TestEnable
  Description: ���Ը澯�����ã������л򲿷ָ澯ʹ�ܴ�
  Input:       ��
  Others:      ��
*************************************************/
/*
void TestEnable(void)
{
    MYENABLE.ucPwrLost = 0;           //��Դ����澯
    MYENABLE.ucPwrFault = 0;          //��Դ���ϸ澯                
    MYENABLE.ucSolarBatLowPwr = 0;    //̫���ܵ�ص͵�ѹ�澯        
    MYENABLE.ucBatFalut = 1;          //���ģ���ع��ϸ澯        
    MYENABLE.ucPosition = 1;          //λ�ø澯                    
    MYENABLE.ucPaOverheat = 1;        //���Ź��¸澯                
    MYENABLE.ucSrcSigChanged = 1;     //��Դ�仯�澯                
    MYENABLE.ucOtherModule = 1;       //����ģ��澯                
    MYENABLE.ucOscUnlocked = 1;       //����ʧ���澯                
    MYENABLE.ucUlLna = 1;             //���е���Ź��ϸ澯          
    MYENABLE.ucDlLna = 1;             //���е���Ź��ϸ澯          
    MYENABLE.ucUlPa = 1;              //���й��Ÿ澯                
    MYENABLE.ucDlPa = 1;              //���й��Ÿ澯                
    MYENABLE.ucOptModule = 1;         //���շ�ģ����ϸ澯          
    MYENABLE.ucMsLink = 1;            //���Ӽ����·�澯            
    MYENABLE.ucDlInOverPwr = 1;       //������������ʸ澯          
    MYENABLE.ucDlInUnderPwr = 1;      //��������Ƿ���ʸ澯          
    MYENABLE.ucDlOutOverPwr = 1;      //������������ʸ澯          
    MYENABLE.ucDlOutUnderPwr = 1;     //�������Ƿ���ʸ澯          
    MYENABLE.ucDlSwr = 1;             //����פ���ȸ澯              
    MYENABLE.ucUlOutOverPwr = 1;      //������������ʸ澯          
    MYENABLE.ucExt1 = 1;              //�ⲿ�澯1��8                
    MYENABLE.ucExt2 = 1;                                            
    MYENABLE.ucExt3 = 1;                                            
    MYENABLE.ucExt4 = 1;
    MYENABLE.ucExt5 = 1;              //�ⲿ�澯1��8                
    MYENABLE.ucExt6 = 1;                                            
    MYENABLE.ucExt7 = 1;                                            
    MYENABLE.ucExt8 = 1;
    MYENABLE.ucDoor = 1;              //�Ž��澯                    
    MYENABLE.ucSelfOsc = 1;           //�Լ��澯                    
    MYENABLE.ucGprsLoginFailed = 1;   //3G:GPRS��¼ʧ�ܸ澯       

    MYENABLE.ucDlInOverPwr1 = 1;      //������������ʸ澯��ͨ��1�� 
    MYENABLE.ucDlInUnderPwr1 = 1;     //��������Ƿ���ʸ澯��ͨ��1�� 
    MYENABLE.ucDlOutOverPwr1 = 1;     //������������ʸ澯��ͨ��1�� 
    MYENABLE.ucDlOutUnderPwr1 = 1;    //�������Ƿ���ʸ澯��ͨ��1�� 
    MYENABLE.ucUlOutOverPwr1 = 1;     //������������ʸ澯��ͨ��1�� 
    MYENABLE.ucDlSwr1 = 1;            //����פ���ȸ澯��ͨ��1��     

    MYENABLE.ucDlInOverPwr2 = 1;      //������������ʸ澯��ͨ��2�� 
    MYENABLE.ucDlInUnderPwr2 = 1;     //��������Ƿ���ʸ澯��ͨ��2�� 
    MYENABLE.ucDlOutOverPwr2 = 1;     //������������ʸ澯��ͨ��2�� 
    MYENABLE.ucDlOutUnderPwr2 = 1;    //�������Ƿ���ʸ澯��ͨ��2�� 
    MYENABLE.ucUlOutOverPwr2 = 1;     //������������ʸ澯��ͨ��2�� 
    MYENABLE.ucDlSwr2 = 1;            //����פ���ȸ澯��ͨ��2��     

    MYENABLE.ucSelfOscShutdown = 1;   //3G:�Լ��ػ������澯    
    MYENABLE.ucSelfOscReduceAtt = 1;  //3G:�Լ������汣���澯
}
*/
/*************************************************
  Function:    TestSupport
  Description: ���Ը澯�����ã������л򲿷ָ澯������Ϊ֧��
  Input:       ��
  Others:      ��
*************************************************/
/*
void TestSupport(void)
{
    MYALARM.stPwrLost.ucSupportedBit = 1;           //��Դ����澯
    MYALARM.stPwrFault.ucSupportedBit = 1;          //��Դ���ϸ澯                
    MYALARM.stSolarBatLowPwr.ucSupportedBit = 1;    //̫���ܵ�ص͵�ѹ�澯        
    MYALARM.stBatFalut.ucSupportedBit = 1;          //���ģ���ع��ϸ澯        
    MYALARM.stPosition.ucSupportedBit = 1;          //λ�ø澯                    
    MYALARM.stPaOverheat.ucSupportedBit = 1;        //���Ź��¸澯                
    MYALARM.stSrcSigChanged.ucSupportedBit = 1;     //��Դ�仯�澯                
    MYALARM.stOtherModule.ucSupportedBit = 1;       //����ģ��澯                
    MYALARM.stOscUnlocked.ucSupportedBit = 1;       //����ʧ���澯                
    MYALARM.stUlLna.ucSupportedBit = 1;             //���е���Ź��ϸ澯          
    MYALARM.stDlLna.ucSupportedBit = 1;             //���е���Ź��ϸ澯          
    MYALARM.stUlPa.ucSupportedBit = 1;              //���й��Ÿ澯                
    MYALARM.stDlPa.ucSupportedBit = 1;              //���й��Ÿ澯                
    MYALARM.stOptModule.ucSupportedBit = 1;         //���շ�ģ����ϸ澯          
    MYALARM.stMsLink.ucSupportedBit = 1;            //���Ӽ����·�澯            
    MYALARM.stDlInOverPwr.ucSupportedBit = 1;       //������������ʸ澯          
    MYALARM.stDlInUnderPwr.ucSupportedBit = 1;      //��������Ƿ���ʸ澯          
    MYALARM.stDlOutOverPwr.ucSupportedBit = 1;      //������������ʸ澯          
    MYALARM.stDlOutUnderPwr.ucSupportedBit = 1;     //�������Ƿ���ʸ澯          
    MYALARM.stDlSwr.ucSupportedBit = 1;             //����פ���ȸ澯              
    MYALARM.stUlOutOverPwr.ucSupportedBit = 1;      //������������ʸ澯          
    MYALARM.stExt1.ucSupportedBit = 1;              //�ⲿ�澯1��8                
    MYALARM.stExt2.ucSupportedBit = 1;                                            
    MYALARM.stExt3.ucSupportedBit = 1;                                            
    MYALARM.stExt4.ucSupportedBit = 1;
    MYALARM.stExt5.ucSupportedBit = 1;              //�ⲿ�澯1��8                
    MYALARM.stExt6.ucSupportedBit = 1;                                            
    MYALARM.stExt7.ucSupportedBit = 1;                                            
    MYALARM.stExt8.ucSupportedBit = 1;
    MYALARM.stDoor.ucSupportedBit = 1;              //�Ž��澯                    
    MYALARM.stSelfOsc.ucSupportedBit = 1;           //�Լ��澯                    
    MYALARM.stGprsLoginFailed.ucSupportedBit = 1;   //3G:GPRS��¼ʧ�ܸ澯       

    MYALARM.stDlInOverPwr1.ucSupportedBit = 1;      //������������ʸ澯��ͨ��1�� 
    MYALARM.stDlInUnderPwr1.ucSupportedBit = 1;     //��������Ƿ���ʸ澯��ͨ��1�� 
    MYALARM.stDlOutOverPwr1.ucSupportedBit = 1;     //������������ʸ澯��ͨ��1�� 
    MYALARM.stDlOutUnderPwr1.ucSupportedBit = 1;    //�������Ƿ���ʸ澯��ͨ��1�� 
    MYALARM.stUlOutOverPwr1.ucSupportedBit = 1;     //������������ʸ澯��ͨ��1�� 
    MYALARM.stDlSwr1.ucSupportedBit = 1;            //����פ���ȸ澯��ͨ��1��     

    MYALARM.stDlInOverPwr2.ucSupportedBit = 1;      //������������ʸ澯��ͨ��2�� 
    MYALARM.stDlInUnderPwr2.ucSupportedBit = 1;     //��������Ƿ���ʸ澯��ͨ��2�� 
    MYALARM.stDlOutOverPwr2.ucSupportedBit = 1;     //������������ʸ澯��ͨ��2�� 
    MYALARM.stDlOutUnderPwr2.ucSupportedBit = 1;    //�������Ƿ���ʸ澯��ͨ��2�� 
    MYALARM.stUlOutOverPwr2.ucSupportedBit = 1;     //������������ʸ澯��ͨ��2�� 
    MYALARM.stDlSwr2.ucSupportedBit = 1;            //����פ���ȸ澯��ͨ��2��     

    MYALARM.stSelfOscShutdown.ucSupportedBit = 1;   //3G:�Լ��ػ������澯    
    MYALARM.stSelfOscReduceAtt.ucSupportedBit = 1;  //3G:�Լ������汣���澯  
    //--------- ��������Ϊ������� ---------  
    MYALARM.stUlSelfOsc.ucSupportedBit = 0;         //�����Լ��澯
    MYALARM.stDlSelfOsc.ucSupportedBit = 0;         //�����Լ��澯
    //--------- ��������Ϊ������� --------- 
}
*/
/*************************************************
  Function:    NewAlarmJudgment
  Description: ���ACAllJudgment�����ľ��幦��ʵ��
  Input:       AbcTemp -- �澯��Ľṹ�׵�ַ
           ucTheEnable -- �ø澯��ĸ澯ʹ��
  Others:      
*************************************************/
/*
void AlarmJudgment(void* AbcTemp, UCHAR ucTheEnable)
{
    ALARM_ITEM_INFO_ST *TempAlarm = (ALARM_ITEM_INFO_ST *)AbcTemp;

    //���������������������������� �ɼ��Ĵ���+1 ��������������������������������
    TempAlarm->ucTotalCount += 1;
    //���������������� ���ݸ澯ʹ�ܱ�־�����¸ø澯���Ƿ���Ч��־ ����������������
//    TempAlarm->ucRcStatus &= TempAlarm->ucSupportedBit; //�Ƿ�֧�ָ���
    TempAlarm->ucEffectiveBit = ucTheEnable;
    TempAlarm->ucRcStatus &= ucTheEnable;
    //���������������������� �жϱ仯�Ĵ��������������ϱ� ������������������������
    if(TempAlarm->ucRcStatus != TempAlarm->ucLocalStatus)
    {
        TempAlarm->ucChangedCount += 1; //�仯�Ĵ���+1
    }
    
    if(TempAlarm->ucLocalStatus == NOT_ALARM) //���ԭ����û�澯��
    {
        if(TempAlarm->ucChangedCount >= ALARM_TIMES)  //ALARM_TIMES = 36
        {
            TempAlarm->ucLocalStatus = ALARM; //�ı䱾��״̬Ϊ�澯
            TempAlarm->ucTotalCount = 0;  //�ɼ���������
            TempAlarm->ucChangedCount = 0;  //�澯״̬�仯��������
        }
        else if((MAX_PICK_TIMES + (TempAlarm->ucChangedCount)
                     - (TempAlarm->ucTotalCount)) < ALARM_TIMES)
        {//��� 90+�ѱ仯����-�Ѳɼ�����<36 �Ļ�����ʾ���ᷢ���澯�ϱ����ɽ�������90�βɼ�
            TempAlarm->ucTotalCount = 0;  //�ɼ���������
            TempAlarm->ucChangedCount = 0;  //�澯״̬�仯��������
        }
    }
    else  //���ԭ�����Ѿ��澯��
    {
        if(TempAlarm->ucChangedCount >= NOTALARM_TIMES)  //NOTALARM_TIMES = 81
        {
            TempAlarm->ucLocalStatus = NOT_ALARM; //�ı䱾��״̬Ϊ����
            TempAlarm->ucTotalCount = 0;  //�ɼ���������
            TempAlarm->ucChangedCount = 0;  //�澯״̬�仯��������
        }
        else if((MAX_PICK_TIMES + (TempAlarm->ucChangedCount)
                     - (TempAlarm->ucTotalCount)) < NOTALARM_TIMES)
        {//��� 90+�ѱ仯����-�Ѳɼ�����<81 �Ļ�����ʾ���ᷢ���ָ��ϱ����ɽ�������90�βɼ�
            TempAlarm->ucTotalCount = 0;  //�ɼ���������
            TempAlarm->ucChangedCount = 0;  //�澯״̬�仯��������
        }
    }
    //�������������������������� �ж��Ƿ�ɼ��������� ����������������������������
    if(TempAlarm->ucTotalCount == MAX_PICK_TIMES)  //����Ѿ��ɼ�90��
    {
        TempAlarm->ucTotalCount = 0;  //�ɼ���������
        TempAlarm->ucChangedCount = 0;  //�澯״̬�仯��������
    }
}
*/
/*************************************************
  Function:    NewNewAlarmJudgment
  Description: ���ACAllJudgment�����ľ��幦��ʵ��
  Input:   AbcTemp     -- �澯��Ľṹ�׵�ַ
           ucTheEnable -- �ø澯��ĸ澯ʹ��
  Return:      �����Ƿ񱾵ظ澯״̬�иı䣬�иı�ʱ����1���޸ı�ʱ����0
  Others:      �޸��˸澯�����ļ�����ʼʱ���Լ��ܲ�������
*************************************************/
UCHAR NewAlarmJudgment(void* AbcTemp, UCHAR ucTheEnable)
{
    UCHAR ucTempRcStatus, ucTempReturn;
    
    ucTempReturn = 0;
    
    ALARM_ITEM_INFO_ST *TempAlarm = (ALARM_ITEM_INFO_ST *)AbcTemp;

    //���������֧�֣����ñ���״̬Ϊ���澯
    if(TempAlarm->ucSupportedBit == NOT_SUPPORTED)
    {
        TempAlarm->ucLocalStatus = NOT_ALARM;
        return 0;
    }

    //���������������� ���ݸ澯ʹ�ܱ�־�����¸ø澯���Ƿ���Ч��־ ��������������
    TempAlarm->ucEffectiveBit = ucTheEnable;
    ucTempRcStatus = TempAlarm->ucRcStatus & ucTheEnable;
    //TempAlarm->ucRcStatus &= ucTheEnable;

    //����������澯����Ҫ���к����ļ����ж�
    if(TempAlarm->ucIsInstant)
    {
        if(TempAlarm->ucLocalStatus != TempAlarm->ucRcStatus)
        {
            TempAlarm->ucLocalStatus = TempAlarm->ucRcStatus;
            return 1; //״̬��ת
        }
        else
        {
            return 0;
        }
    }
    
    //���������������������� �жϱ仯�Ĵ��������������ϱ� ������������������������
    if(ucTempRcStatus != TempAlarm->ucLocalStatus)
    {
        TempAlarm->ucChangedCount += 1;  //�仯�Ĵ���+1
        TempAlarm->ucTotalCount += 1; //�ɼ����ܴ���+1
    }
    else if(TempAlarm->ucTotalCount != 0)  //����ɼ����ܴ���������0�����Ѿ�������״̬�仯
    {
        TempAlarm->ucTotalCount += 1; //�ɼ����ܴ���+1
    }

    //������������ ����ܲɼ�������������ʼ�ж��Ƿ�Ҫ���±��ظ澯״̬ ��������������
    if(TempAlarm->ucTotalCount >= g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucTotal)  //����Ѿ��ɼ�90��
    {
        if(TempAlarm->ucLocalStatus == NOT_ALARM) //���ԭ����û�澯��
        {
            if(TempAlarm->ucChangedCount >= g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucOccurLimit)  //ALARM_TIMES = 36
            {
                TempAlarm->ucLocalStatus = ALARM; //�ı䱾��״̬Ϊ�澯
                ucTempReturn = 1;
            }
        }
        else  //���ԭ�����Ѿ��澯��
        {
            if(TempAlarm->ucChangedCount >= g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucEliminateLimit)  //NOTALARM_TIMES = 81
            {
                TempAlarm->ucLocalStatus = NOT_ALARM; //�ı䱾��״̬Ϊ����
                ucTempReturn = 1;
            }
        }
        TempAlarm->ucTotalCount = 0;  //�ɼ���������
        TempAlarm->ucChangedCount = 0;  //�澯״̬�仯��������    
    }
    
    return ucTempReturn;
}

//������Ӧ��������δ����ǰ����
void ACTaskInit()
{
    //��������
    g_pstACQue = OSQCreate(g_apvACQue, AC_QUE_SIZE);

    AdcInit();      //��ɶ�����A/D�ɼ����ĳ�ʼ��

    DacInit();      //��ɶ�����D/A�ĳ�ʼ��
}

//#define YK_POI

#ifdef YK_POI
#include "./poi_actask.cpp"
#else
#include "./gen_actask.cpp"
#endif
