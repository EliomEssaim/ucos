/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   Gen_actask.cpp
    ����:     ����
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  
       ����:  
       ����:  �����д���
    2. ����:  2006/10/30
       ����:  ����
       ����:  �޸�����MCM-9��
              ����"��Դ����澯���������Ƿ���ʸ澯������"�� 
    3. ����:  2006/11/01
       ����:  �½�
       ����:  �޸�����MCM-11����ԭ�������漰�����жϵĵط�ȫ����Ϊ��
              �������ñ���ĳ����߶���ֵ���жϡ�
    4. ����:  2006/11/06
       ����:  ����
       ����:  �޸�����MCM-17�������д�ͨ���ŵ�ͨ��1�Ĳ�����ӳ���Ӧ��
              ������ͨ���ŵĲ����ϣ������豸�Ƕ�ͨ��ʱ����ѯͨ��1�Ĳ�
              ��ʵ�ʷ��ʵ��Ƕ�Ӧ�Ĳ���ͨ���ŵĲ����ĵ�ַ��
    5. ����:  2006/11/29
       ����:  ����
       ����:  �޸�����MCM-42��
              ����ͨ���������������פ������
---------------------------------------------------------------------------*/
#include <string.h>
//****************************��������****************************//
extern ULONG g_ulACCHSendMessageNum;
extern DEV_TYPE_TABLE_ST g_stDevTypeTable;
//****************************��������****************************//
extern "C" void SetAtt(void);
extern "C" void SetRfSw(void);

USHORT usOldBatValue;

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
    //if(NewAlarmJudgment(&(MYALARM.stPwrLost), MYENABLE.ucPwrLost) == 1)
    //{
    //    if(MYALARM.stPwrLost.ucSupportedBit == 1)
    //    {
    //        ucNeedToSend = 1; //��ʾ��Ҫ����������Ϣ����Ƶ����һ��
    //        ucSendAlarmStatus |= MYALARM.stPwrLost.ucLocalStatus; //���ظ澯״̬
    //    }
    //}

    //------------- ��Դ���ϸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stPwrFault), MYENABLE.ucPwrFault);
    //if(NewAlarmJudgment(&(MYALARM.stPwrFault), MYENABLE.ucPwrFault) == 1)
    //{
    //    if(MYALARM.stPwrFault.ucSupportedBit == 1)
    //    {
    //        ucNeedToSend = 1; //��ʾ��Ҫ����������Ϣ����Ƶ����һ��
    //        ucSendAlarmStatus |= MYALARM.stPwrFault.ucLocalStatus; //���ظ澯״̬
    //    }
    //}

    //------------- ̫���ܵ�ص͵�ѹ�澯 -------------
    NewAlarmJudgment(&(MYALARM.stSolarBatLowPwr), MYENABLE.ucSolarBatLowPwr);
    //if(NewAlarmJudgment(&(MYALARM.stSolarBatLowPwr), MYENABLE.ucSolarBatLowPwr) == 1)
    //{
    //    if(MYALARM.stSolarBatLowPwr.ucSupportedBit == 1)
    //    {
    //        ucNeedToSend = 1; //��ʾ��Ҫ����������Ϣ����Ƶ����һ��
    //        ucSendAlarmStatus |= MYALARM.stSolarBatLowPwr.ucLocalStatus; //���ظ澯״̬
    //    }
    //}

   //------------- ���ģ���ع��ϸ澯 -------------
    //NewAlarmJudgment(&(MYALARM.stBatFalut), MYENABLE.ucBatFalut);
    //ɾ��һ�д��룬����ֻ������ع��ϸ澯��ʹ�ܣ����жϸ澯
    MYALARM.stBatFalut.ucEffectiveBit = MYENABLE.ucBatFalut;

    //------------- λ�ø澯 ���̸��±��أ��ο�ʹ�� -------------
    //MYALARM.stPosition.ucRcStatus &= MYALARM.stPosition.ucSupportedBit; //�Ƿ�֧�ָ���
    MYALARM.stPosition.ucLocalStatus =   //���ظ澯״̬
            MYALARM.stPosition.ucRcStatus & MYENABLE.ucPosition;
    MYALARM.stPosition.ucEffectiveBit = MYENABLE.ucPosition; //�Ƿ���Ч=�澯ʹ��

    //------------- ���Ź��¸澯 -------------
    NewAlarmJudgment(&(MYALARM.stPaOverheat), MYENABLE.ucPaOverheat);

    //------------- ��Դ�仯�澯 -------------
    NewAlarmJudgment(&(MYALARM.stSrcSigChanged), MYENABLE.ucSrcSigChanged);

    //------------- ����ģ��澯 -------------
    NewAlarmJudgment(&(MYALARM.stOtherModule), MYENABLE.ucOtherModule);

    //------------- ����ʧ���澯 -------------
    NewAlarmJudgment(&(MYALARM.stOscUnlocked), MYENABLE.ucOscUnlocked);

    //------------- ���е���Ź��ϸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stUlLna), MYENABLE.ucUlLna);

    //------------- ���е���Ź��ϸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stDlLna), MYENABLE.ucDlLna);

    //------------- ���й��Ÿ澯 -------------
    NewAlarmJudgment(&(MYALARM.stUlPa), MYENABLE.ucUlPa);

    //------------- ���й��Ÿ澯 -------------
    NewAlarmJudgment(&(MYALARM.stDlPa), MYENABLE.ucDlPa);

    //------------- ���շ�ģ����ϸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stOptModule), MYENABLE.ucOptModule);

    //------------- ���Ӽ����·�澯 -------------
    NewAlarmJudgment(&(MYALARM.stMsLink), MYENABLE.ucMsLink);

    //------------- ������������ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stDlInOverPwr), MYENABLE.ucDlInOverPwr);

    //------------- ��������Ƿ���ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stDlInUnderPwr), MYENABLE.ucDlInUnderPwr);

    //------------- ������������ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stDlOutOverPwr), MYENABLE.ucDlOutOverPwr);

    //MCM-42_20061129_linwei_begin
    //------------- ������������ʸ澯(ͨ����) -------------
    NewAlarmJudgment(&(MYALARM.stDlOutOverPwr2), MYENABLE.ucDlOutOverPwr2);

    //------------- �������Ƿ���ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stDlOutUnderPwr), MYENABLE.ucDlOutUnderPwr);

    //------------- �������Ƿ���ʸ澯(ͨ����) -------------
    NewAlarmJudgment(&(MYALARM.stDlOutUnderPwr2), MYENABLE.ucDlOutUnderPwr2);

    //------------- ����פ���ȸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stDlSwr), MYENABLE.ucDlSwr);

    //------------- ����פ���ȸ澯(ͨ����) -------------
    NewAlarmJudgment(&(MYALARM.stDlSwr2), MYENABLE.ucDlSwr2);
    //MCM-42_20061129_linwei_end

    //------------- ������������ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stUlOutOverPwr), MYENABLE.ucUlOutOverPwr);

    //------------- �������Ƿ���ʸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stUlOutUnderPwr), MYENABLE.ucUlOutUnderPwr);

    //------------- ����פ���ȸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stUlSwr), MYENABLE.ucUlSwr);

    //------------- �ⲿ�澯1 ���̸��±��أ��ο�ʹ�� -------------
    //MYALARM.stExt1.ucRcStatus &= MYALARM.stExt1.ucSupportedBit; //�Ƿ�֧�ָ���
    MYALARM.stExt1.ucLocalStatus =   //���ظ澯״̬
            MYALARM.stExt1.ucRcStatus & MYENABLE.ucExt1;
    MYALARM.stExt1.ucEffectiveBit = MYENABLE.ucExt1; //�Ƿ���Ч=�澯ʹ��
    
    //------------- �ⲿ�澯2 ���̸��±��أ��ο�ʹ�� -------------
    //MYALARM.stExt2.ucRcStatus &= MYALARM.stExt2.ucSupportedBit; //�Ƿ�֧�ָ���
    MYALARM.stExt2.ucLocalStatus =   //���ظ澯״̬
            MYALARM.stExt2.ucRcStatus & MYENABLE.ucExt2;
    MYALARM.stExt2.ucEffectiveBit = MYENABLE.ucExt2; //�Ƿ���Ч=�澯ʹ��
    
    //------------- �ⲿ�澯3 ���̸��±��أ��ο�ʹ�� -------------
    //MYALARM.stExt3.ucRcStatus &= MYALARM.stExt3.ucSupportedBit; //�Ƿ�֧�ָ���
    MYALARM.stExt3.ucLocalStatus =   //���ظ澯״̬
            MYALARM.stExt3.ucRcStatus & MYENABLE.ucExt3;
    MYALARM.stExt3.ucEffectiveBit = MYENABLE.ucExt3; //�Ƿ���Ч=�澯ʹ��
    
    //------------- �ⲿ�澯4 ���̸��±��أ��ο�ʹ�� -------------
    //MYALARM.stExt4.ucRcStatus &= MYALARM.stExt4.ucSupportedBit; //�Ƿ�֧�ָ���
    MYALARM.stExt4.ucLocalStatus =   //���ظ澯״̬
            MYALARM.stExt4.ucRcStatus & MYENABLE.ucExt4;
    MYALARM.stExt4.ucEffectiveBit = MYENABLE.ucExt4; //�Ƿ���Ч=�澯ʹ��
    
    //------------- �Ž��澯 ���̸��±��أ��ο�ʹ�� -------------
    //MYALARM.stDoor.ucRcStatus &= MYALARM.stDoor.ucSupportedBit; //�Ƿ�֧�ָ���
    MYALARM.stDoor.ucLocalStatus =   //���ظ澯״̬
            MYALARM.stDoor.ucRcStatus & MYENABLE.ucDoor;
    MYALARM.stDoor.ucEffectiveBit = MYENABLE.ucDoor; //�Ƿ���Ч=�澯ʹ��
    
    //------------- �Լ��澯 -------------
    NewAlarmJudgment(&(MYALARM.stSelfOsc), MYENABLE.ucSelfOsc);

    //------------- 3G:GPRS��¼ʧ�ܸ澯 -------------
    NewAlarmJudgment(&(MYALARM.stGprsLoginFailed), MYENABLE.ucGprsLoginFailed);

    //------------- 3G:�Լ��ػ������澯 -------------
    //NewAlarmJudgment(&(MYALARM.stSelfOscShutdown), MYENABLE.ucSelfOscShutdown);

    //------------- 3G:�Լ������汣���澯 -------------
    //NewAlarmJudgment(&(MYALARM.stSelfOscReduceAtt), MYENABLE.ucSelfOscReduceAtt);


    //--------- ��������Ϊ������� ---------
    NewAlarmJudgment(&(MYALARM.stUlSelfOsc), 1);//MYENABLE.ucUlSelfOsc);//�����Լ��澯
    MYALARM.stUlSelfOsc.ucEffectiveBit = 0; //�ø澯Ϊ�Զ��壬���μӸ澯�жϣ���˽���Ч��־����
    NewAlarmJudgment(&(MYALARM.stDlSelfOsc), 1);//MYENABLE.ucDlSelfOsc);//�����Լ��澯
    MYALARM.stDlSelfOsc.ucEffectiveBit = 0; //�ø澯Ϊ�Զ��壬���μӸ澯�жϣ���˽���Ч��־����
    //--------- ��������Ϊ������� --------- 

    //------------------------------����Ϊ̫���ܿ�������������-----------------------------------

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
    
    //------------------------------����Ϊ̫���ܿ�������������-----------------------------------

}

/*********************************************************
  Function:    ACAllShield
  Description: ������������
*********************************************************/
void ACAllShield(void)
{

    //��������Դ����澯ʱӦ���ε�Դ���ϸ澯
    if(MYALARM.stPwrLost.ucLocalStatus == ALARM)               //��Դ����
    {
        MYALARM.stPwrFault.ucLocalStatus =
            g_uCenterStatus.stAlarmStatusStruct.ucPwrFault;
        MYALARM.stPwrFault.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stPwrFault.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stPwrFault.ucChangedCount = 0;  //�澯״̬�仯��������
    }

    //��������Դ�澯����Դ���ϸ澯��̫���ܵ�ص͵�ѹ�澯��ض���Ƶ����ʱ��
    //�����¸���澯״̬�������ĵ�״̬һ�£�����Ч��־����
    if((MYALARM.stPwrLost.ucLocalStatus == ALARM)               //��Դ����
            || (MYALARM.stPwrFault.ucLocalStatus == ALARM)      //��ع���
            || (MYALARM.stSolarBatLowPwr.ucLocalStatus == ALARM)//̫���ܵ�ع���
            || (g_stSettingParamSet.ucRfSw == 0))   //��Ƶ������Ч�ұ��ر�
    {
        //������Դ�澯��ر���Ƶ�ź�ʱ�����ԭ�ȵ�״̬��0������Ҫ��������ӻ�������״̬
        if(ucSendAlarmStatus == 0)
        {
            ucNeedToSend = 1;
            ucSendAlarmStatus = 1;
        }
        //----------- ������������ʸ澯 ������ -----------
        MYALARM.stDlInOverPwr.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlInOverPwr; //�澯����״̬
        MYALARM.stDlInOverPwr.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stDlInOverPwr.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stDlInOverPwr.ucChangedCount = 0;  //�澯״̬�仯��������
        //----------- ��������Ƿ���ʸ澯 ������ -----------
        MYALARM.stDlInUnderPwr.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlInUnderPwr; //�澯����״̬
        MYALARM.stDlInUnderPwr.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stDlInUnderPwr.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stDlInUnderPwr.ucChangedCount = 0;  //�澯״̬�仯��������
        //----------- �������Ƿ���ʸ澯 ������ -----------
        MYALARM.stDlOutUnderPwr.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlOutUnderPwr; //�澯����״̬
        MYALARM.stDlOutUnderPwr.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stDlOutUnderPwr.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stDlOutUnderPwr.ucChangedCount = 0;  //�澯״̬�仯��������

        //MCM-42_20061129_linwei_begin
        //----------- �������Ƿ���ʸ澯(ͨ����) ������ -----------
        MYALARM.stDlOutUnderPwr2.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlOutUnderPwr2; //�澯����״̬
        MYALARM.stDlOutUnderPwr2.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stDlOutUnderPwr2.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stDlOutUnderPwr2.ucChangedCount = 0;  //�澯״̬�仯��������
        //MCM-42_20061129_linwei_end
        
        //----------- ������������ʸ澯 ������ -----------
        MYALARM.stUlOutOverPwr.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucUlOutOverPwr; //�澯����״̬
        MYALARM.stUlOutOverPwr.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stUlOutOverPwr.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stUlOutOverPwr.ucChangedCount = 0;  //�澯״̬�仯��������

        //MCM-9_20061030_linwei_begin
        //----------- �������Ƿ���ʸ澯 ������ -----------
        MYALARM.stUlOutUnderPwr.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucUlOutOverPwr; //�澯����״̬
        MYALARM.stUlOutUnderPwr.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stUlOutUnderPwr.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stUlOutUnderPwr.ucChangedCount = 0;  //�澯״̬�仯��������
        //MCM-9_20061030_linwei_end
        
        //----------- ���й��Ź��ϸ澯 ������ -----------
        MYALARM.stDlPa.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlPa; //�澯����״̬
        MYALARM.stDlPa.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stDlPa.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stDlPa.ucChangedCount = 0;  //�澯״̬�仯��������
        //----------- ���й��Ź��ϸ澯 ������ -----------
        MYALARM.stUlPa.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucUlPa; //�澯����״̬
        MYALARM.stUlPa.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stUlPa.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stUlPa.ucChangedCount = 0;  //�澯״̬�仯��������
        //----------- ���е���Ź��ϸ澯 ������ -----------
        MYALARM.stDlLna.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlLna; //�澯����״̬
        MYALARM.stDlLna.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stDlLna.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stDlLna.ucChangedCount = 0;  //�澯״̬�仯��������
        //----------- ���е���Ź��ϸ澯 ������ -----------
        MYALARM.stUlLna.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucUlLna; //�澯����״̬
        MYALARM.stUlLna.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stUlLna.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stUlLna.ucChangedCount = 0;  //�澯״̬�仯��������
        //----------- ���Ź��¸澯 ������ -----------
        MYALARM.stPaOverheat.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucPaOverheat; //�澯����״̬
        MYALARM.stPaOverheat.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stPaOverheat.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stPaOverheat.ucChangedCount = 0;  //�澯״̬�仯��������
        //----------- ����ģ��澯 ������ -----------
        MYALARM.stOtherModule.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucOtherModule; //�澯����״̬
        MYALARM.stOtherModule.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stOtherModule.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stOtherModule.ucChangedCount = 0;  //�澯״̬�仯��������
        //----------- ����ʧ���澯 ������ -----------
        MYALARM.stOscUnlocked.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucOscUnlocked; //�澯����״̬
        MYALARM.stOscUnlocked.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stOscUnlocked.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stOscUnlocked.ucChangedCount = 0;  //�澯״̬�仯��������
        //----------- ����פ���澯 ������ -----------
        MYALARM.stDlSwr.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlSwr; //�澯����״̬
        MYALARM.stDlSwr.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stDlSwr.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stDlSwr.ucChangedCount = 0;  //�澯״̬�仯��������

        //MCM-42_20061129_linwei_begin
        //----------- ����פ���澯(ͨ����) ������ -----------
        MYALARM.stDlSwr2.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucDlSwr2; //�澯����״̬
        MYALARM.stDlSwr2.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stDlSwr2.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stDlSwr2.ucChangedCount = 0;  //�澯״̬�仯��������
        //MCM-42_20061129_linwei_end
        
        //----------- ���շ����ϸ澯 ������ -----------
        MYALARM.stOptModule.ucLocalStatus = 
            g_uCenterStatus.stAlarmStatusStruct.ucOptModule; //�澯����״̬
        MYALARM.stOptModule.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stOptModule.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stOptModule.ucChangedCount = 0;  //�澯״̬�仯��������
    }
    else
    {
        //û�е�Դ�澯����Ƶ�ź�ʱ�����ԭ�ȵ�״̬��1������Ҫ��������ӻ�������״̬
        if(ucSendAlarmStatus == 1)
        {
            ucNeedToSend = 1;
            ucSendAlarmStatus = 0;
        }
    }
    
    //ͬʱ�������������빦�ʡ��������������Ƿ���ʸ澯��Ӧֻ�ϱ���������Ƿ���ʸ澯��
    //    �����������Ƿ���ʸ澯����ѯ�������Ƿ���ʸ澯��ʾ״̬Ϊ������
    if(MYALARM.stDlInUnderPwr.ucLocalStatus == ALARM)
    {
        MYALARM.stDlOutUnderPwr.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯
        MYALARM.stDlOutUnderPwr1.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯�������Ƿ ͨ��1��
        MYALARM.stDlOutUnderPwr2.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯�������Ƿ ͨ��2��
        MYALARM.stDlOutUnderPwr.ucEffectiveBit = 0;  //�Ƿ���Ч����
        MYALARM.stDlOutUnderPwr1.ucEffectiveBit = 0;  //�Ƿ���Ч���㣨�����Ƿ ͨ��1��
        MYALARM.stDlOutUnderPwr2.ucEffectiveBit = 0;  //�Ƿ���Ч���㣨�����Ƿ ͨ��2��
    }
    
    //ͬʱ���������е���Ź��ϸ澯���������������Ƿ���ʸ澯��Ӧֻ�ϱ����е���Ź���
    //    �澯�������������Ƿ���ʸ澯����ѯ�������Ƿ���ʸ澯��ʾ״̬Ϊ������
    if(MYALARM.stDlLna.ucLocalStatus == ALARM)
    {
        MYALARM.stDlOutUnderPwr.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯
        MYALARM.stDlOutUnderPwr1.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯�������Ƿ ͨ��1��
        MYALARM.stDlOutUnderPwr2.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯�������Ƿ ͨ��2��
        MYALARM.stDlOutUnderPwr.ucEffectiveBit = 0;  //�Ƿ���Ч����
        MYALARM.stDlOutUnderPwr1.ucEffectiveBit = 0;  //�Ƿ���Ч���㣨�����Ƿ ͨ��1��
        MYALARM.stDlOutUnderPwr2.ucEffectiveBit = 0;  //�Ƿ���Ч���㣨�����Ƿ ͨ��2��
    }
    
    //ͬʱ���������й��Ź��ϸ澯���������������Ƿ���ʸ澯��Ӧֻ�ϱ����й��Ź��ϸ澯��
    //    �����������Ƿ���ʸ澯����ѯ�������Ƿ���ʸ澯��ʾ״̬Ϊ������
    if(MYALARM.stDlPa.ucLocalStatus == ALARM)
    {
        MYALARM.stDlOutUnderPwr.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯
        MYALARM.stDlOutUnderPwr1.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯�������Ƿ ͨ��1��
        MYALARM.stDlOutUnderPwr2.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯�������Ƿ ͨ��2��
        MYALARM.stDlOutUnderPwr.ucEffectiveBit = 0;  //�Ƿ���Ч����
        MYALARM.stDlOutUnderPwr1.ucEffectiveBit = 0;  //�Ƿ���Ч���㣨�����Ƿ ͨ��1��
        MYALARM.stDlOutUnderPwr2.ucEffectiveBit = 0;  //�Ƿ���Ч���㣨�����Ƿ ͨ��2��
    }
    
    //ͬʱ����������ʧ���澯���������������Ƿ���ʸ澯��Ӧֻ�ϱ�����澯�������������
    //    Ƿ���ʸ澯����ѯ�������Ƿ���ʸ澯��ʾ״̬Ϊ������
    if(MYALARM.stOscUnlocked.ucLocalStatus == ALARM)
    {
        MYALARM.stDlOutUnderPwr.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯
        MYALARM.stDlOutUnderPwr1.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯�������Ƿ ͨ��1��
        MYALARM.stDlOutUnderPwr2.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯�������Ƿ ͨ��2��
        MYALARM.stDlOutUnderPwr.ucEffectiveBit = 0;  //�Ƿ���Ч����
        MYALARM.stDlOutUnderPwr1.ucEffectiveBit = 0;  //�Ƿ���Ч���㣨�����Ƿ ͨ��1��
        MYALARM.stDlOutUnderPwr2.ucEffectiveBit = 0;  //�Ƿ���Ч���㣨�����Ƿ ͨ��2��
    }
    
    //�����˽��˻�������Դ�澯ʱ��Ӧ����Զ�˻��ģ��������Ƿ���ʸ澯
    if(ucDlOutUnderShield == 1)
    {
        MYALARM.stDlOutUnderPwr.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯
        MYALARM.stDlOutUnderPwr.ucEffectiveBit = 0;  //�Ƿ���Ч����
        MYALARM.stDlOutUnderPwr.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stDlOutUnderPwr.ucChangedCount = 0;  //�澯״̬�仯��������

        MYALARM.stDlOutUnderPwr1.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯�������Ƿ ͨ��1��
        MYALARM.stDlOutUnderPwr1.ucEffectiveBit = 0;  //�Ƿ���Ч���㣨�����Ƿ ͨ��1��
        MYALARM.stDlOutUnderPwr1.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stDlOutUnderPwr1.ucChangedCount = 0;  //�澯״̬�仯��������

        MYALARM.stDlOutUnderPwr2.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯�������Ƿ ͨ��2��
        MYALARM.stDlOutUnderPwr2.ucEffectiveBit = 0;  //�Ƿ���Ч���㣨�����Ƿ ͨ��2��
        MYALARM.stDlOutUnderPwr2.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stDlOutUnderPwr2.ucChangedCount = 0;  //�澯״̬�仯��������
    }
    
    //������Զ�˻�������Դ�澯ʱ��Ӧ���ν��˻��ģ����շ����ϸ澯���������Ƿ���ʸ澯
    if(ucOptShield == 1)
    {
        MYALARM.stOptModule.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯
            //g_uCenterStatus.stAlarmStatusStruct.ucOptModule; //�澯����״̬
        MYALARM.stOptModule.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stOptModule.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stOptModule.ucChangedCount = 0;  //�澯״̬�仯��������
    }
//��ʱ��������Ϊԭ�����ݼ���û���������Ƿ���ʸ澯
    if(ucUlOutUnderShield == 1) 
    {
        MYALARM.stUlOutUnderPwr.ucLocalStatus = NOT_ALARM; //��ѯΪ���澯
            //g_uCenterStatus.stAlarmStatusStruct.ucUlOutUnderPwr; //�澯����״̬
        MYALARM.stUlOutUnderPwr.ucEffectiveBit = 0;  //�Ƿ���ЧҲ����
        MYALARM.stUlOutUnderPwr.ucTotalCount = 0;    //�ɼ���������
        MYALARM.stUlOutUnderPwr.ucChangedCount = 0;  //�澯״̬�仯��������
    }

}

/*************************************************
  Function:     CheckPwrLostStatus
  Description:  �ж��Ƿ����˵�Դ���磬����ոշ�����Դ����
  Calls:
  Called By:    ACTask
  Input:        
  Output:     
*************************************************/
void CheckPwrLostStatus(void)
{
    if(MYALARM.stPwrLost.ucRcStatus == NOT_ALARM) //���ֲɼ�����е�Դ�����ǷǸ澯״̬ʱ
    {
        ucDetectBatFlag = 0;  //�������ģ���ع����жϵı�־��0��ʾδ��ʼ�ж�
    }
    else if(ucDetectBatFlag == 0) //�������ģ���ع����жϵı�־��0��ʾδ��ʼ�ж�
    {
        if(ucPwrLostOldStatus == NOT_ALARM) //ǰһ�ֵ�Դ�����״̬����ǲ��澯ʱ
        {
            ucDetectBatFlag = 1; //�������ģ���ع����жϵı�־��1��ʾ�ѿ�ʼ�ж�
            usPickBattTimes = 0; //�����ж�ģ���ع����жϺ󣬲ɼ����ܴ���--����
        }
    }
    //ucPwrLostOldStatus; ǰһ�ֵ�Դ�����״̬��ÿһ�ֲɼ�������󶼸��¸�ֵ
    ucPwrLostOldStatus = MYALARM.stPwrLost.ucRcStatus;
}

/*****************************************************
  Function:     ModuleBatJudge
  Description:  ģ���ع��ϸ澯�ж�
  Calls:
  Called By:    ACTask
  Input:        ucDetectBatFlag, usPickBattTimes
  Other:        ���¹�����ʱ�������е繩��ʱ�����ģ��
                �󱸵���쳣�����ϱ��˸澯��
******************************************************/
void ModuleBatJudge() //ģ���ع��ϸ澯�ж�
{

//************* test ***********
    //g_stPrivateParam.usBattJudgeStartVoltage = 1000;
    //g_stPrivateParam.usBattJudgeStopVoltage  = 950;
/******************************/
    //���ģ���ص�ѹ��ֵ�仯̫��������������
    if((g_stRcParamSet.usModuleBatValue < usOldBatValue) &&
           ((usOldBatValue - g_stRcParamSet.usModuleBatValue) > 40))
    {
        return;
    }
    else
    {
        usOldBatValue = g_stRcParamSet.usModuleBatValue;
    }

    if(ucDetectBatFlag == 1) //�������ģ���ع����жϵı�־
    {
        if(usPickBattTimes == 0)
        {
            if(g_stRcParamSet.usModuleBatValue < g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStartVoltage)
            {   //���"���ģ���ص�ѹ"ֵС�ڵ����ʱ��ѹʱ
                ucDetectBatFlag = 0;  //�������ģ���ع����жϵı�־��0��ʾδ��ʼ�ж�
            }
            else
            {
                usBeginBatValue = g_stRcParamSet.usModuleBatValue;  //�������ģ���ع����ж�ʱ��ģ���ص�ѹֵ 100��
                usPickBattTimes = 1;
            }
        }
        else
        {
            usPickBattTimes = usPickBattTimes + 1;
            
            if(g_stRcParamSet.usModuleBatValue < g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStopVoltage)
            {
                //�������Ѿ��ӽ�û����
                if(((g_stDevTypeTable.ucOpticalType == OPTICAL_NO) && (usPickBattTimes < ONE_HOUR_PICKTIMES)) ||
                   ((g_stDevTypeTable.ucOpticalType != OPTICAL_NO) && (usPickBattTimes < TWENTY_MINUTE_PICKTIMES)))
                {   //����������ǹ��˻���ͣ��󲻵�1Сʱ����ؾ�û����  ����
                    //    �����Ƿǹ��˻���ͣ��󲻵�20���ӵ�ؾ�û����
                    MYALARM.stBatFalut.ucLocalStatus = ALARM;
                }
                else if(((g_stDevTypeTable.ucOpticalType == OPTICAL_NO) && (usPickBattTimes >= ONE_HOUR_PICKTIMES)) ||
                        ((g_stDevTypeTable.ucOpticalType != OPTICAL_NO) && (usPickBattTimes >= TWENTY_MINUTE_PICKTIMES)))
                {
                    MYALARM.stBatFalut.ucLocalStatus = NOT_ALARM;
                }
                
                ucDetectBatFlag = 0; //�رռ��ģ���ع����жϵı�־
            }
        }
    }
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
    UCHAR ucMasterDlInPow;  //������һ�η��͸��ӻ����������빦��ֵ
    UCHAR ucTempDlInput;  //������A����������
    UCHAR ucBeginDownFlag=0;  //�Ƿ�ʼ�Լ�������ı�־    
                              //0��ʾδ��ʼ��
                              //1��ʾ�ѿ�ʼ�����������Լ��澯����
                              //2��ʾ�ѿ�ʼ�����������Լ��澯����
                              //3��ʾ�ѿ�ʼ�����ϡ����ж��Լ��澯����
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
        
        //�ж�����������ֵ�Ƿ�仯���б仯�Ļ�����ӻ������µ���������ֵ
        if(g_stDevTypeTable.ucRFRoleType == RF_MASTER) //��������Ƶ����ʱ������ӻ�������������ֵ
        {
            //����ϴη����ӻ����ǡ����ڼ�ⷶΧ��
            if( ucMasterDlInPow == (0x30 + HIGHER_FLAG) )
            {   //�����ǰ�������벻�ǡ����ڼ�ⷶΧ��
                if(g_stRcParamSet.ucDlInPwrFlag == LOWER_FLAG)
                {
                    ucNeedToSend = 1;
                    ucMasterDlInPow = 0x30 + LOWER_FLAG;
                }
                else if(g_stRcParamSet.ucDlInPwrFlag != HIGHER_FLAG)
                {
                    ucNeedToSend = 1; //�Ƿ���Ҫ����Ƶ����ӻ�����������Ϣ�ı�־
                    ucMasterDlInPow = g_stRcParamSet.cDlInPwr; //���¸�ֵ
                }
            }
            //����ϴη����ӻ����ǡ����ڼ�ⷶΧ��
            else if( ucMasterDlInPow == (0x30 + LOWER_FLAG) )
            {   //�����ǰ���������ǡ����ڼ�ⷶΧ��
                if(g_stRcParamSet.ucDlInPwrFlag == HIGHER_FLAG)
                {
                    ucNeedToSend = 1;
                    ucMasterDlInPow = 0x30 + LOWER_FLAG;
                }
                else if(g_stRcParamSet.ucDlInPwrFlag != LOWER_FLAG)
                {
                    ucNeedToSend = 1; //�Ƿ���Ҫ����Ƶ����ӻ�����������Ϣ�ı�־
                    ucMasterDlInPow = g_stRcParamSet.cDlInPwr; //���¸�ֵ
                }
            }
            //����ϴη����ӻ�����������ֵ�Ļ�
            else
            {
                if(g_stRcParamSet.ucDlInPwrFlag != NORMAL_FLAG)
                {
                    ucNeedToSend = 1;
                    ucMasterDlInPow = 0x30 + g_stRcParamSet.ucDlInPwrFlag;
                }
                else if(ucMasterDlInPow != (UCHAR)(g_stRcParamSet.cDlInPwr))
                {
                    ucNeedToSend = 1; //�Ƿ���Ҫ����Ƶ����ӻ�����������Ϣ�ı�־
                    //ucMasterDlInPow ���� ������һ�η��͸��ӻ����������빦��ֵ
                    ucMasterDlInPow = g_stRcParamSet.cDlInPwr; //���¸�ֵ
                }
            }
        }

        GetAlarmInit(); //�������ɼ��ĳ�ʼ��

        GetAlarmData(); //�������ɼ�����ɿ������ɼ������ֵ
        CheckPwrLostStatus(); //�ж��Ƿ����˵�Դ���磬����ոշ�����Դ����

        AdcExe();       //���A/D�ɼ���ģ�����ĸ澯�ж�
        ModuleBatJudge(); //ģ���ع��ϸ澯�ж�

        //------------------------------------
        ACAllJudgment();  //����ʹ�ܺ󣬽��и澯״̬�ж�

        //�澯ʹ�ܱ��رգ�Ӧ�ѱ��ظ澯״̬�����ĸ澯״̬����Ϊ����
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
        
        ACAllShield();  //������������

        //ͨ��1�����и澯ӳ�䵽����ͨ����ID��,ֻ��Ҫ�������ظ澯״̬���Ƿ���Ч��־
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
        
        
        //----------- �����ӻ�֮�����ݴ��ݵ�ʵ�� -----------    
        if(ucNeedToSend == 1) //��ʾ��Ҫ����������Ϣ����Ƶ����һ��
        {
            //���͸澯�仯֪ͨ��OH
            MESSAGE_ST *pstMessage = (MESSAGE_ST *)MyMalloc(g_pstMemPool256);
            if(pstMessage == 0)
            {
                return; //û�а취
            }

            pstMessage->ulMessType = ACCH_MESS_SEND_REQ;//
            pstMessage->usDataLen  = 3;                  //���ݳ���            
            
            *(pstMessage->aucData) = g_stDevTypeTable.ucRFRoleType;  //��Ƶ���ӻ�״̬
            *((pstMessage->aucData) + 1) = ucSendAlarmStatus; //����״����1Ϊ����
            *((pstMessage->aucData) + 2) = ucMasterDlInPow; //�������������빦��ֵ
            UCHAR ucTempRet;
            if(g_stDevTypeTable.ucRFRoleType == RF_MASTER)
            {
                //��Ϣ���������ͷ��ڴ棬�������Ϣ�����з��ͳɹ�����Ѽ�����1
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
            ucNeedToSend = 0; //������Ϣ�Ѿ����ͣ��������㣬�´�ѭ�����ٷ���
        }
        //----------- ������������ӻ�֮�����ݴ��ݵ�ʵ�� ----------- 

        //----------- �Լ�������ȸ澯��ʵ�� -----------
        if((ucBeginDownFlag & 0x02) != 0)  //�Ѿ���ʼ�Լ�������ı�־���������Լ��澯����
        {
            if(MYALARM.stUlSelfOsc.ucLocalStatus == NOT_ALARM)
            {
                ucBeginDownFlag &= 0xfd;
            }
            else if(MYALARM.stUlSelfOsc.ucRcStatus == ALARM) //������л����Լ�
            {   //�ж��Ƿ񻹿��Խ����棬���ܽ��Ļ��͹���Ƶ
                if(g_stSettingParamSet.ucUlAtt < 30) //�������˥��1<30
                {
                    g_stSettingParamSet.ucUlAtt += 2; //����Ϊ2��ÿ�ν���2dB����
                    SetAtt(); //����˥��
                }
                else if(g_stSettingParamSet.ucUlAtt == 30) //�������˥��1=30
                {
                    g_stSettingParamSet.ucUlAtt = 31; //���ֻ��˥��31
                    SetAtt(); //����˥��
                }
                else //����˥��1==31�������ٽ����棬�����Ƶ
                {
                    g_stSettingParamSet.ucRfSw = 0;  //��Ƶ�źſ���״̬
                    SetRfSw(); //����Ƶ
                    MYALARM.stSelfOscShutdown.ucEffectiveBit = MYENABLE.ucSelfOscShutdown;
                    MYALARM.stSelfOscShutdown.ucLocalStatus = ALARM & MYENABLE.ucSelfOscShutdown; //3G���Լ��ػ������澯
                }
            }
        }
        else if(MYALARM.stUlSelfOsc.ucLocalStatus == ALARM) //��������Լ��澯����
        {
            ucBeginDownFlag |= 0x02;
            //�������� ���˥��Ϊ31ʱ�����Լ����˴����ӵĻ��������ϱ������澯 ��������
            if(g_stSettingParamSet.ucUlAtt == 31) //�������˥��1==31
            {
                g_stSettingParamSet.ucRfSw = 0;  //��Ƶ�źſ���״̬
                SetRfSw(); //����Ƶ
                MYALARM.stSelfOscShutdown.ucEffectiveBit = MYENABLE.ucSelfOscShutdown;
                MYALARM.stSelfOscShutdown.ucLocalStatus = ALARM & MYENABLE.ucSelfOscShutdown; //3G���Լ��ػ������澯
            }
            //�������� ���˥��Ϊ31ʱ�����Լ����˴����ӵĻ��������ϱ������澯 ��������
        }

        if((ucBeginDownFlag & 0x01) != 0)  //�Ѿ���ʼ�Լ�������ı�־���������Լ��澯����
        {
            if(MYALARM.stDlSelfOsc.ucLocalStatus == NOT_ALARM)
            {
                ucBeginDownFlag &= 0xfe;
            }
            else if(MYALARM.stDlSelfOsc.ucRcStatus == ALARM) //������л����Լ�
            {   //�ж��Ƿ񻹿��Խ����棬���ܽ��Ļ��͹���Ƶ
                if(g_stSettingParamSet.ucDlAtt < 30) //�������˥��1<30
                {
                    g_stSettingParamSet.ucDlAtt += 2; //����Ϊ2��ÿ�ν���2dB����
                    SetAtt(); //����˥��
                }
                else if(g_stSettingParamSet.ucDlAtt == 30) //�������˥��1=30
                {
                    g_stSettingParamSet.ucDlAtt = 31; //���ֻ��˥��31
                    SetAtt(); //����˥��
                }
                else //����˥��1==31�������ٽ����棬�����Ƶ
                {
                    g_stSettingParamSet.ucRfSw = 0;  //��Ƶ�źſ���״̬
                    SetRfSw(); //����Ƶ
                    MYALARM.stSelfOscShutdown.ucEffectiveBit = MYENABLE.ucSelfOscShutdown;
                    MYALARM.stSelfOscShutdown.ucLocalStatus = ALARM & MYENABLE.ucSelfOscShutdown; //3G���Լ��ػ������澯
                }
            }
        }
        else if(MYALARM.stDlSelfOsc.ucLocalStatus == ALARM) //��������Լ��澯����
        {
            ucBeginDownFlag |= 0x01;
            //�������� ���˥��Ϊ31ʱ�����Լ����˴����ӵĻ��������ϱ������澯 ��������
            if(g_stSettingParamSet.ucDlAtt == 31) //�������˥��1==31
            {
                g_stSettingParamSet.ucRfSw = 0;  //��Ƶ�źſ���״̬
                SetRfSw(); //����Ƶ
                MYALARM.stSelfOscShutdown.ucEffectiveBit = MYENABLE.ucSelfOscShutdown;
                MYALARM.stSelfOscShutdown.ucLocalStatus = ALARM & MYENABLE.ucSelfOscShutdown; //3G���Լ��ػ������澯
            }
            //�������� ���˥��Ϊ31ʱ�����Լ����˴����ӵĻ��������ϱ������澯 ��������
        }

        if(ucBeginDownFlag != 0)
        {
            MYALARM.stSelfOscReduceAtt.ucEffectiveBit = MYENABLE.ucSelfOscReduceAtt;
            MYALARM.stSelfOscReduceAtt.ucLocalStatus = MYENABLE.ucSelfOscReduceAtt; //3G:�Լ������汣���澯
        }

        //----------- ��������Լ�������ȸ澯��ʵ�� -----------

        CheckAlarmStatus();
        
        /*********** ����Ϊ���ģ���ع��ϸ澯���ж� ************
         if((MYALARM.stPwrLost.ucTotalCount == 1) &&       //���Դ������ܲɼ�����Ϊ1
                 (MYALARM.stPwrLost.ucRcStatus == ALARM))  //�ҵ�Դ���統ǰ�ǵ���״̬
         {
             //USHORT usBeginBatValue; �������ģ���ع����ж�ʱ��ģ���ص�ѹֵ 100��
     	       usBeginBatValue = g_stRcParamSet.usModuleBatValue  //���ģ���ص�ѹ
     	       usBeginBatValue = 1;  //�������ģ���ع����жϵı�־��1��ʾ�ѿ�ʼ�ж�
         }
        /************ ����Ϊ���ģ���ع��ϸ澯���ж� ************/
         
        //MyDelay(990);//1990);  //��ʱ1.99��
        for(j=0;j<199;j++) //ÿ��ѭ�����ȴ�10ms��ȡ����
        {
            void *pvPrmv = OSQPend(g_pstACQue, 10/MS_PER_TICK, &g_ucErr); //DELAY 10MS
            if(pvPrmv == 0)
            {
                continue;
            }
            if(*((ULONG*)pvPrmv) == ACCH_MESS_RECV_IND)
            {
                if(*(((MESSAGE_ST *)pvPrmv)->aucData) == RF_MASTER)
                {                               //�������Ƶ����������������Ϣ
                    if(g_stDevTypeTable.ucRFRoleType == RF_SLAVE) //�ұ�������Ƶ�ӻ�ʱ
                    {//�����˽��˻�������Դ�澯��ر���Ƶ�ź�ʱ��Ӧ����Զ�˻��ģ��������Ƿ���ʸ澯��
                        if(*(((MESSAGE_ST *)pvPrmv)->aucData + 1) == 1)
                        {
                            ucDlOutUnderShield = 1;
                        }
                        else
                        {
                            ucDlOutUnderShield = 0;
                        }
                      //���ϵõ���Դ����澯��Ϣ������ȡA����������ֵ
                        ucTempDlInput = *(((MESSAGE_ST *)pvPrmv)->aucData + 2);
                        if(ucTempDlInput == (0x30 + LOWER_FLAG))
                        {  //������A������������ڼ�ⷶΧʱ
                            g_stRcParamSet.ucDlInPwrFlag = LOWER_FLAG; //�������빦�ʵ�ƽ������Χ�ı�־
                            g_stRcParamSet.uc3GDlInPwrFlag = LOWER_FLAG;
                        }
                        else if(ucTempDlInput == (0x30 + HIGHER_FLAG))
                        {  //������A������������ڼ�ⷶΧʱ
                            g_stRcParamSet.ucDlInPwrFlag = HIGHER_FLAG; //�������빦�ʵ�ƽ������Χ�ı�־
                            g_stRcParamSet.uc3GDlInPwrFlag = HIGHER_FLAG;
                        }
                        else
                        {  //A�˴�������������ֵ����ʱ
                            g_stRcParamSet.ucDlInPwrFlag = NORMAL_FLAG; //����ʵ�����泬����Χ�ı�־
                            g_stRcParamSet.uc3GDlInPwrFlag = NORMAL_FLAG;
                            g_stRcParamSet.cDlInPwr = (CHAR)ucTempDlInput; //�������빦�ʵ�ƽ
                            g_stRcParamSet.sDlInPwr = g_stRcParamSet.cDlInPwr * 10;
                        }
                    }
                }
                else
                {                                //�������Ƶ�ӻ�������������Ϣ
                    if(g_stDevTypeTable.ucRFRoleType == RF_MASTER) //�ұ�������Ƶ����ʱ
                    {//������Զ�˻�������Դ�澯��ر���Ƶ�ź�ʱ��Ӧ���ν��˻��ģ����շ����ϸ澯���������Ƿ���ʸ澯
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
