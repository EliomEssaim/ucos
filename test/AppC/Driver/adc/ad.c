/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   ad.c
    ����:     ����
    �ļ�˵��: ���ļ�ʵ��A/Dģ�����ɼ��������ɼ�ֵ���ݲ�ͬ���㹫ʽ���õ���Ӧ���
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
#include "../../include/Interface.h"
#include "../../OH/CMCC_1_Ext.h"
#include "../../OH/YKPP.h"
#include "../../AC/Actask.h"
#include "ad.h"

#define UpLoad   1 //���У�CountPowOut�Ĳ���
#define DownLoad 0 //���У�CountPowOut�Ĳ���
#define LOWER_VALUE  0x80 //���ڼ�ⷶΧʱ�ķ���ֵ
#define HIGHER_VALUE 0x7F //���ڼ�ⷶΧʱ�ķ���ֵ
#define ADC_AVAILABLE_BIT 0x03ff //A/D����Ϊ10bit�����Բɼ������ȡ���10��λ

extern SETTING_PARAM_SET_ST g_stSettingParamSet;

//���и澯��Ϣ
extern ALARM_ITEM_UN g_uAlarmItems;
//���ĸ澯״̬
extern ALARM_STATUS_UN g_uCenterStatus;
//ʵʱ��������������Ҫ���籣��
extern RC_PARAM_SET_ST g_stRcParamSet;
extern DEV_INFO_SET_ST  g_stDevInfoSet;

//������չ����
extern YKPP_PARAM_SET_ST g_stYkppParamSet;

extern UCHAR MasterSlaveJudge(void); //�������жϱ�����������Ƶ����������Ƶ�ӻ�

char cDetectDlInPwr = 0;   //

void AdcInit(void);
void AdcExe(void);
/*
UINT32 CountPowOut(UINT32 uiOldValue, UINT32 uiUpOrDown);
UINT32 CountVSWR(UINT32 uiOldVSWR, UINT32 uiDownOutput);
INT32 CountTem(UINT32 uiOldTem);
INT32 CountBigPwr(UINT32 uiOldPwr);
INT32 CountSmallPwr(UINT32 uiOldPwr);
INT32 CountBigPwr3G(UINT32 uiOldPwr);
INT32 CountSmallPwr3G(UINT32 uiOldPwr);
void PutIntoRam(UINT32* uiValueTemp);
*/
//======================================================================
//���ܣ�	��ɶ�����A/D�ɼ����ĳ�ʼ��
//����дʱ�䣺2006��3��14��
//======================================================================
void AdcInit(void)
{
    //ARM�ڲ���A/D��س�ʼ��
    APMC_PCER |= 0x00018000;  //��ADC_0��ADC_1���ⲿʱ��
    MA_OUTW(ADC_IDR_0, 0x01);  //�ر�ADC_0���ж�ʹ��
    MA_OUTW(ADC_IDR_1, 0x01);  //�ر�ADC_1���ж�ʹ��
    MA_OUTWM(ADC_MR_0, ADC_PRESCAL, ADC_PRESCAL); //���ò�������Ϊ10bit��ȡ��Ӳ������������
                                                //����ʱ��ΪMCK/((63+1)*2)
    MA_OUTWM(ADC_MR_1, ADC_PRESCAL, ADC_PRESCAL); //���ò�������Ϊ10bit��ȡ��Ӳ������������
                                                //����ʱ��ΪMCK/((63+1)*2)
    PioOpen( &g_stPioB, ADC_SE|ADC_EN1|ADC_EN2, PIO_OUTPUT); //ͬʱ��������PIO���ƺ�����Ϊ���״̬

}

//======================================================================
//���ܣ�	 ���ADC_CHSR�ĵ���λ
//����޸ģ� 2006��7��5��
//������     ʹ����ADCģ�������ͨ������disable״̬����Ϊͬʱ�ж��ͨ��
//           enable�Ļ�������ɼ����̱��жϴ�ϣ��ɼ��ͻ�ֹͣ
//======================================================================
void ClearAdcCHSR(void)
{
    MA_OUTW(ADC_CHDR_0, 0x0F);
    MA_OUTW(ADC_CHDR_1, 0x0F);
}

#ifdef YK_POI
#include "./poi_conversion.c"
#else
#include "./gen_conversion.c"
#endif


//======================================================================
//���ܣ�	  ���A/D�ɼ���ģ�����ĸ澯�ж�
//����޸ģ� 2006��7��20��
//������     ���ļ���ֻ�趨ʱִ�б��������������A/D�ɼ��ͽ�����㹤��
//======================================================================
void AdcExe(void)
{
    UINT32 i, uiWAdcValue[24]; //������ʱ��Ųɼ����Ľ��

 //   AdcInit(); //��ɶ�����A/D�ɼ����ĳ�ʼ��
  //*********************************
  // �˴������ⲿģ�⿪�ص��л�����
    SELECT_A;
    DISSELECT_U30;
    SELECT_U6U7;
  //*********************************
    MA_OUTW(ADC_CHER_0, 0x01);       //��ѡ��0·���ɼ�
    for(i=0;i<5000;i++);  //����Ӳ����·�в���ģ����ӿ������л�ѡ����Ҫ�ɼ���ģ��������
                          //���л�ʱ���������Դ����������ʱ�ȴ��ź��ȶ�
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D�ɼ���0��ʼ����
    while(MA_INWM(ADC_SR_0, 0x01) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[0]=MA_INWM(ADC_CDR0_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x01);       //��ѡ��0·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    while(MA_INWM(ADC_SR_1, 0x01) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[1]=MA_INWM(ADC_CDR0_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_0, 0x02);       //ѡ��1·���ɼ�
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D�ɼ���0��ʼ����
    while(MA_INWM(ADC_SR_0, 0x02) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[2]=MA_INWM(ADC_CDR1_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x02);       //ѡ��1·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    while(MA_INWM(ADC_SR_1, 0x02) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[3]=MA_INWM(ADC_CDR1_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_0, 0x04);       //ѡ��2·���ɼ�
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D�ɼ���0��ʼ����
    while(MA_INWM(ADC_SR_0, 0x04) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[4]=MA_INWM(ADC_CDR2_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x04);       //ѡ��2·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    while(MA_INWM(ADC_SR_1, 0x04) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[5]=MA_INWM(ADC_CDR2_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_0, 0x08);       //ѡ��3·���ɼ�
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D�ɼ���0��ʼ����
    while(MA_INWM(ADC_SR_0, 0x08) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[6]=MA_INWM(ADC_CDR3_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x08);       //ѡ��3·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    while(MA_INWM(ADC_SR_1, 0x08) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[7]=MA_INWM(ADC_CDR3_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
  //*********************************
  // �˴������ⲿģ�⿪�ص��л�����
    SELECT_B;
    for(i=0;i<5000;i++);  //����Ӳ����·�в���ģ����ӿ������л�ѡ����Ҫ�ɼ���ģ��������
                          //���л�ʱ���������Դ����������ʱ�ȴ��ź��ȶ�
  //*********************************
    MA_OUTW(ADC_CHER_0, 0x01);       //��ѡ��0·���ɼ�
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D�ɼ���0��ʼ����
    while(MA_INWM(ADC_SR_0, 0x01) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[8]=MA_INWM(ADC_CDR0_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x01);       //��ѡ��0·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    while(MA_INWM(ADC_SR_1, 0x01) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[9]=MA_INWM(ADC_CDR0_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_0, 0x02);       //ѡ��1·���ɼ�
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D�ɼ���0��ʼ����
    while(MA_INWM(ADC_SR_0, 0x02) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[10]=MA_INWM(ADC_CDR1_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x02);       //ѡ��1·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    while(MA_INWM(ADC_SR_1, 0x02) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[11]=MA_INWM(ADC_CDR1_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_0, 0x04);       //ѡ��2·���ɼ�
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D�ɼ���0��ʼ����
    while(MA_INWM(ADC_SR_0, 0x04) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[12]=MA_INWM(ADC_CDR2_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x04);       //ѡ��2·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    while(MA_INWM(ADC_SR_1, 0x04) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[13]=MA_INWM(ADC_CDR2_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_0, 0x08);       //ѡ��3·���ɼ�
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D�ɼ���0��ʼ����
    while(MA_INWM(ADC_SR_0, 0x08) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[14]=MA_INWM(ADC_CDR3_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x08);       //ѡ��3·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    while(MA_INWM(ADC_SR_1, 0x08) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[15]=MA_INWM(ADC_CDR3_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------

#ifndef YK_POI
  //*********************************
  // �˴������ⲿģ�⿪�ص��л�����
    SELECT_A;
    DISSELECT_U6U7;
    SELECT_U30;
    for(i=0;i<5000;i++);  //����Ӳ����·�в���ģ����ӿ������л�ѡ����Ҫ�ɼ���ģ��������
                          //���л�ʱ���������Դ����������ʱ�ȴ��ź��ȶ�
  //*********************************
    MA_OUTW(ADC_CHER_1, 0x01);       //��ѡ��0·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    
    while(MA_INWM(ADC_SR_1, 0x01) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[16]=MA_INWM(ADC_CDR0_1, ADC_AVAILABLE_BIT);
        
    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_1, 0x02);       //ѡ��1·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    
    while(MA_INWM(ADC_SR_1, 0x02) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[17]=MA_INWM(ADC_CDR1_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_1, 0x04);       //ѡ��2·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    
    while(MA_INWM(ADC_SR_1, 0x04) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[18]=MA_INWM(ADC_CDR2_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_1, 0x08);       //ѡ��3·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����

    while(MA_INWM(ADC_SR_1, 0x08) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[19]=MA_INWM(ADC_CDR3_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();
        //--------------------
  //*********************************
  // �˴������ⲿģ�⿪�ص��л�����
    SELECT_B;
    for(i=0;i<5000;i++);  //����Ӳ����·�в���ģ����ӿ������л�ѡ����Ҫ�ɼ���ģ��������
                          //���л�ʱ���������Դ����������ʱ�ȴ��ź��ȶ�
  //*********************************
    MA_OUTW(ADC_CHER_1, 0x01);       //��ѡ��0·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    
    while(MA_INWM(ADC_SR_1, 0x01) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[20]=MA_INWM(ADC_CDR0_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_1, 0x02);       //ѡ��1·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    
    while(MA_INWM(ADC_SR_1, 0x02) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[21]=MA_INWM(ADC_CDR1_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_1, 0x04);       //ѡ��2·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����
    
    while(MA_INWM(ADC_SR_1, 0x04) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[22]=MA_INWM(ADC_CDR2_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_1, 0x08);       //ѡ��3·���ɼ�
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D�ɼ���1��ʼ����

    while(MA_INWM(ADC_SR_1, 0x08) == 0); //�ȴ��ɼ���ϵı�־
    uiWAdcValue[23]=MA_INWM(ADC_CDR3_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();
        //--------------------
#endif

/*********** TEST **********
    uiWAdcValue[1] = 512;
    uiWAdcValue[2] = 640;
    uiWAdcValue[3] = 512;
    uiWAdcValue[4] = 512;
    uiWAdcValue[5] = 512;
    uiWAdcValue[6] = 512;
    uiWAdcValue[7] = 512;
    uiWAdcValue[8] = 512;
    uiWAdcValue[9] = 512;
    uiWAdcValue[10] = 512;
    uiWAdcValue[11] = 512;
    uiWAdcValue[12] = 512;
    uiWAdcValue[13] = 512;
    uiWAdcValue[14] = 512;
    uiWAdcValue[15] = 512;
    uiWAdcValue[16] = 512;
    uiWAdcValue[17] = 512;
    uiWAdcValue[18] = 512;
    uiWAdcValue[19] = 512;
    uiWAdcValue[20] = 512;
    uiWAdcValue[21] = 512;
    uiWAdcValue[22] = 512;
    uiWAdcValue[23] = 512;
    
/***************************/
    PutIntoRam(uiWAdcValue); //���ݲɼ���Ϻ�ĸ�ģ��������,�Լ��澯״̬�ж�
}


