/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   setfreq.c
    ����:     ����
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  
       ����:  ����
       ����:  �������
    2. ����:  2006/11/01
       ����:  �½�
       ����:  �޸�����MCM-11����ԭ�������漰�����жϵĵط�ȫ����Ϊ��
              �������ñ���ĳ����߶���ֵ���жϡ�
    3. ����:  2006/11/07
       ����:  �½�
       ����:  �޸�����MCM-19���޸ı��󣬰�devtype��ΪucFreqArea 
---------------------------------------------------------------------------*/
/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
CPLD��·�й���˥���������Ĳ��裺
      1����ATT_CS1��ATT_CS2��Ƭѡĳһ�����Ż�����
      2����˥��ֵ��ATT_1��ATT_16�ͳ���CPLD
      3����ATT_OUTEN���ͣ���CPLD����������ݷ��ͳ�ȥ
      4���ʵ���ʱ�������Ҫ��ʱ�Ļ�������ATT_OUTEN����

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "string.h"
#include "../../include/UserType.h"
#include "../../include/at91m55800.h"
#include "../../include/MyBoard.h"
#include "../../periph/pio/pio.h"
#include "../../OH/CMCC_1_Ext.h"
#include "../../periph/flash/Flash.h"
#include "../../periph/usart/Usart.h"

//#include "setfreq.h"

//********************************************
//               �ܽ��������                **
//********************************************
#define PIO_STATE &g_stPioA  //�����ڷ�������ʱ��PIOA����
#define FLASH0BASE  g_stNorFlash0.pfwBaseAddr  //������д����

#define FRE_CK PA7  //ѡƵ����ʱ��ʱ�ӽ� PIOA��
#define FRE_DA PA8  //ѡƵ����ʱ�������� PIOA��
#define SET_FRE_CK  PioWrite( PIO_STATE, FRE_CK, PIO_SET_OUT)  //FRE_CK�á�1��
  //#define SET_FRE_CK MA_OUTWM( PIO_SODR_A, FRE_CK, FRE_CK);
#define CLR_FRE_CK  PioWrite( PIO_STATE, FRE_CK, PIO_CLEAR_OUT)  //FRE_CK�塰0��
  //#define CLR_FRE_CK MA_OUTWM( PIO_CODR_A, FRE_CK, FRE_CK);
#define SET_FRE_DA  PioWrite( PIO_STATE, FRE_DA, PIO_SET_OUT)  //FRE_DA�á�1��
  //#define SET_FRE_DA MA_OUTWM( PIO_SODR_A, FRE_DA, FRE_DA);
#define CLR_FRE_DA  PioWrite( PIO_STATE, FRE_DA, PIO_CLEAR_OUT)  //FRE_DA�塰0��
  //#define CLR_FRE_DA MA_OUTWM( PIO_CODR_A, FRE_DA, FRE_DA);

#define FRE_CE PA2  //ѡƵ����ʱ��ʹ�ܽ�  PIOA��
#define FRE_S0 PA3  //ѡƵ����ʱ��Ƭѡ��0��  PIOA��
#define FRE_S1 PA4  //ѡƵ����ʱ��Ƭѡ��1��  PIOA��
#define FRE_S2 PA5  //ѡƵ����ʱ��Ƭѡ��2��  PIOA��
#define FRE_S3 PA6  //ѡƵ����ʱ��Ƭѡ��3��  PIOA��
#define SET_FRE_CE  PioWrite( PIO_STATE, FRE_CE, PIO_SET_OUT)  //FRE_CE�á�1��
  //#define SET_FRE_CE MA_OUTWM( PIO_SODR_A, FRE_CE, FRE_CE);
#define CLR_FRE_CE  PioWrite( PIO_STATE, FRE_CE, PIO_CLEAR_OUT)  //FRE_CE�塰0��
  //#define CLR_FRE_CE MA_OUTWM( PIO_CODR_A, FRE_CE, FRE_CE);
#define MOVE_NUMBER 3 //��ΪFRE_S0��PA3��Ҫѡ�е�1·�ͱ��뽫0x01����3λ
#define FRE_ALL_CTRL  FRE_CK|FRE_DA|FRE_CE|FRE_S0|FRE_S1|FRE_S2|FRE_S3


#define BYTE_BITLEN 8//�ֽڳ���
//���в��õͱ������в��ø߱���

#define UPLINK  -1//����
#define DOWNLINK  1//����

#define SW_H 1//SW bitΪH
#define SW_L 0//SW bitΪL
#define UP_LT 1//����
#define DOWN_LT 0//����

#define MB15E07SL_CS 1
#define MB15E07SL_LDS 0
#define MB15E07SL_FC 1

#define  PROG_REF_DIV_C 1//�ɱ�̲ο���Ƶ������λ(high Level)
#define  PROG_DIV_C 0//�ɱ�̷�Ƶ������λ(Low Level)

typedef enum EN_PLL{MB1502=0, MB1507=1, MB15E07SL=2} T_PLL_EN;//оƬ����
typedef struct ST_DATA
{
    USHORT BITLEN;//���ݳ���
    USHORT LIMIT[2];//ȡֵ����(0=����,1=����)
}T_DATA_ST;
typedef struct ST_PLL//���治ͬоƬ��������Ϣ
{
    USHORT PROG_REF_BITLEN;//�ο���Ƶ����������λ����

    T_DATA_ST PROG_REF_COUNT;//�ο�������(Rֵ)

    USHORT SW_PRES[2];//SWλH/Lʱ�ķ�Ƶ��������(1502�е�P�� 1507E1507SL�е�M)

    T_DATA_ST SWAL_COUNT;//SWALLOW COUNTER DIVIDE RATIO(Aֵ)

    T_DATA_ST PROG_COUNT;//��Ƶ������(Nֵ)
}T_PLL_ST;
T_PLL_ST  ARR_PLL[4]={
    {16, {14, {8, 16383}}, {128, 64},  {7,{0, 127}}, {11, {16, 2047}}},
    {16, {14, {8, 16383}}, {256, 128}, {8,{0, 255}}, {11, {16, 2047}}},
    {19, {14, {3, 16383}}, {64, 32},   {7,{0, 127}}, {11, {3, 2047}}},
    {19, {14, {3, 16383}}, {64, 32},   {7,{0, 127}}, {11, {3, 2047}}}
};

typedef enum EN_FREQ_RANGE_TYPE{
    GSM900,
    EGSM900,
    GSM1800,//GSM1800(DCS)
    CDMA800,
    CDMA1900,
    GSM900_S,
    GSM900_256,   //�ο���Ƶ��Ϊ256����������Ƶ��900��1800
    WCDMA
}T_FREQ_RANGE_TYPE_EN;//ֱ��վƵ�η���

typedef struct ST_FREQ_RANGE_TYPE//����ֱ��վƵ�η�����Ϣ
{
    ULONG INTER_FREQ;//��ƵƵ��(��λKHz)
    ULONG Fosc;//�ο��������Ƶ��
    USHORT R;//�ο���������ƵƵ��,R=[8��16383]�������һ���µ�Ƶ�Σ���ֵ���ܹ�����������ã�����ͨ�������ҵ�һ������������
           //�����������MB1502Ϊ��A=[0,127]��A<N, N=[16, 2047]
    T_PLL_EN Chip;//ʹ�õ�оƬ
    UCHAR SW;//SW,��ֵ������Prescaler,��Pֵ
    UCHAR REF_LEN;//�ο���Ƶ�ȳ���
    ULONG REF_DATA;//�ο���Ƶ��
    UCHAR SET_LEN;//��������ֵ����
}T_FREQ_RANGE_TYPE_ST;

T_FREQ_RANGE_TYPE_ST  ARR_FREQ_RANGE_TYPE[9] = {
    {71000, 12800, 64, MB1502, SW_H, 16, 0x008101, 19},     //GSM900
    {71000, 12800, 64, MB1502, SW_H, 16, 0x008101, 19},     //EGSM900
    {140000, 12800, 256, MB1507, SW_H, 16, 0x008041, 20},   //GSM1800
    {70020, 12800, 1280, MB1502, SW_H, 16, 0x008051, 19},   //CDMA800
    {115000, 12800, 256, MB1507, SW_H, 16, 0x008041, 20},   //CDMA1900
    {90000, 12800, 256, MB15E07SL, SW_H, 19, 0x04020D, 19},  //GSM900_S
    {140000, 12800, 256, MB1502, SW_H, 16, 0x008041, 19},   //GSM900_256 �ο���Ƶ��Ϊ256����������Ƶ��900��1800
    {160000, 12800, 64, MB15E07SL, SW_H, 19, 0x04080D, 19}, //WCDMA
    {90000, 12800, 256, MB15E07SL, SW_H, 19, 0x04020D, 19}  //GSM900_S
};


//********************************************
//                ȫ�ֱ���                  **
//********************************************
void FreqDataOutput(ULONG ulFreqRate,ULONG ulFreqData,
        UCHAR RATE_LENGTH,UCHAR DATA_LENGTH,UCHAR NUMBER);
ULONG ShiftULONG(ULONG ulValue, UCHAR uiCount, UCHAR isLeft);
ULONG ReverseInt(ULONG ulSrc, UCHAR uiLen);
UCHAR EncodeDivOneBitData(
    T_FREQ_RANGE_TYPE_EN frtHndlFreqType,
    ULONG ulFreq,
    signed char cUpDown,//������
    ULONG* pulSeriData
);
ULONG CountFreq(USHORT usTempFreqNum, UCHAR ucUpOrDown);
UCHAR FreqToArea(USHORT usTempFreqNum);
void SetFreqInit(void);
void SetFreq(void);


extern DEV_INFO_SET_ST  g_stDevInfoSet;
extern SETTING_PARAM_SET_ST g_stSettingParamSet;
extern DEV_TYPE_TABLE_ST g_stDevTypeTable;

//********************************************
//                ����ʵ��                  **
//********************************************
//---------------------------------------------------------------------------
/*********************************************************
 * ��������ΪC51��Ƶ�4�ֽڱ�����λ����
 * ���룺4�ֽڱ���
 * �����4�ֽڱ���
 * ����ֵ��
 * ˵����
 * ���ߣ�
*********************************************************/
ULONG ShiftULONG(ULONG ulValue, UCHAR uiCount, UCHAR isLeft)
{
    USHORT usH, usL;//��Ÿ��ֽں͵��ֽ�
    UCHAR i=1;
    int iTemp=0;

    USHORT ausValue[2] = {0};//0��ŵ��ֽڡ�1��Ÿ��ֽ�

    memcpy(ausValue, &ulValue, sizeof(ulValue));

    usL = ausValue[0];
    usH = ausValue[1];

    for (; i<=uiCount; i++)
    {
        if (isLeft)
        {
            usH <<= 1;
            iTemp = (usL&0x8000) ? 1:0;
            usH += iTemp;
            usL <<= 1;
        }
        else
        {
            usL >>= 1;
            iTemp = (usH&0x0001) ? 0x8000:0;
            usL += iTemp;
            usH >>= 1;
        }
    }

    return (ULONG)usH*0x10000 + usL;
}
//---------------------------------------------------------------------------
/*********************************************************
 * ����������ָ�����ȵ�bit���еߵ�ת��
 * ���룺���������ݣ��ߵ�bit����
 * �����
 * ����ֵ�����صߵ������������ȴ��ڱ���������λ������0
 * ˵����
 * ���ߣ�
 * ������ (0x00FF0000, 32)->0x0000FF00��(0x000008201, 16)->0x00008041
*********************************************************/
ULONG ReverseInt(ULONG ulSrc, UCHAR uiLen)
{
    ULONG ulSrcBak = ulSrc;
    ULONG uiRsl = 0;
    UCHAR i = 0;

    if (uiLen > sizeof(ulSrc)*BYTE_BITLEN) return 0;

    for (i=0; i<uiLen; i++)
    {
        if ((ulSrcBak & 0x00000001)==1)
        {
            uiRsl += ShiftULONG(1, uiLen-i-1, TRUE);
        }
        ulSrcBak = ShiftULONG(ulSrcBak, 1, FALSE);
    }
    return uiRsl;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*********************************************************
 * ����������һ���ɱ�̷�Ƶ������ֵSerial data
 * ˵��:
 * ���룺�����Ƶ�Σ�����Ƶ��
 * ������ɱ�̷�Ƶ���������ݣ�
 * ���أ������0=�ɹ�������=�������
 * ����:
*********************************************************/
UCHAR EncodeDivOneBitData(
    T_FREQ_RANGE_TYPE_EN frtHndlFreqType,
    ULONG ulFreq,
    signed char cUpDown,//������
    ULONG* pulSeriData
)
{
    UCHAR bErr = 0;
    ULONG ulP = 0;
    USHORT usN = 0;  //UINT32 uiN = 0;
    UCHAR ucA = 0;
    ULONG ulFosc = 0;
    ULONG ulFvco = 0;
    USHORT usR = 0;  //UINT32 usR = 0;
    ULONG ulTemp = 0;

    T_FREQ_RANGE_TYPE_ST* pfrtType = &(ARR_FREQ_RANGE_TYPE[frtHndlFreqType]);
    T_PLL_ST* pmbPLL = &(ARR_PLL[pfrtType->Chip]);

    ulP = pmbPLL->SW_PRES[pfrtType->SW];
    usR = pfrtType->R;
    ulFosc = pfrtType->Fosc;
    if(frtHndlFreqType == WCDMA)
    {
        ulFvco = ulFreq - pfrtType->INTER_FREQ;
    }
    else
    {
        ulFvco = ulFreq + cUpDown * pfrtType->INTER_FREQ;
    }

    //���R����
    if ((usR > pmbPLL->PROG_REF_COUNT.LIMIT[UP_LT]) || (usR < pmbPLL->PROG_REF_COUNT.LIMIT[DOWN_LT]))
    {
        bErr = 1;
        return bErr;
    }


    if (ulFvco * usR % ulFosc != 0)
    {
        bErr = 2;
        return bErr;
    };
    ulTemp = ulFvco * usR / ulFosc;

    usN = ulTemp / ulP;
    ucA = ulTemp % ulP;

    if (ucA>=usN)
    {
        bErr = 3;
        return bErr;
    }

    //���A�����Ƿ���ȷ
    if ((ucA > pmbPLL->SWAL_COUNT.LIMIT[UP_LT]) || (ucA < pmbPLL->SWAL_COUNT.LIMIT[DOWN_LT]))
    {
        bErr = 4;
        return bErr;
    }
    //���N�����Ƿ���ȷ
    if ((usN > pmbPLL->PROG_COUNT.LIMIT[UP_LT]) || (usN < pmbPLL->PROG_COUNT.LIMIT[DOWN_LT]))
    {
        bErr = 5;
        return bErr;
    }

    //���㴮������
    *pulSeriData = ShiftULONG(usN, pmbPLL->SWAL_COUNT.BITLEN+1, TRUE) +
                   ShiftULONG(ucA, 1, TRUE) +
                   PROG_DIV_C;
    *pulSeriData = ReverseInt(*pulSeriData, pmbPLL->SWAL_COUNT.BITLEN+pmbPLL->PROG_COUNT.BITLEN+1);

    return bErr;
}


//*********************************************
//  ���ŵ���ͨ������õ���Ӧ��Ƶ��ֵ(��λKHz)
//         usTempFreqNum --- �ŵ���  
//  ucUpOrDown=0 ����    ucUpOrDown=1 ����
//  ���أ�Ƶ��ֵ(��λKHz)
//---------------------------------------------
ULONG CountFreq(USHORT usTempFreqNum, UCHAR ucUpOrDown)
{
    ULONG ulTempReturn;
    
    if(usTempFreqNum <= 124)
    {  //fup(n)=890+0.2n(MHz) 0��n��124   fdown(n)=fup(n)+45(MHz)
        ulTempReturn = (ULONG)(usTempFreqNum)*200 + 890000 + 45000*((ULONG)(ucUpOrDown));
    }
    else if((usTempFreqNum >= 975) && (usTempFreqNum <= 1023))
    {  //fup(n)=890+0.2(n-1024)(MHz) 975��n��1023  fdown(n)=fup(n)+45(MHz)
        ulTempReturn = 890000 - (1024-(ULONG)(usTempFreqNum))*200 + 45000*((ULONG)(ucUpOrDown));
    }
    else if((usTempFreqNum >= 512) && (usTempFreqNum <= 885))
    {  //fup(n)=1710.2+0.2(n-512)(MHz) 975��n��1023  fdown(n)=fup(n)+95(MHz)
        ulTempReturn = ((ULONG)(usTempFreqNum)-512)*200 + 1710200 + 95000*((ULONG)(ucUpOrDown));
    }
    else if((usTempFreqNum >= 9600) && (usTempFreqNum <= 10837))
    {
        //fup(n)=1922.4+0.2(n-10562)(MHz) 10562��n��10837  fdown(n)=fup(n)+190(MHz)
        ulTempReturn = 1922400 + ((ULONG)(usTempFreqNum)-10562)*200 + 190000*((ULONG)(ucUpOrDown));
    }
    else ulTempReturn = 0;
    return ulTempReturn;
}

//*********************************************
//        ���ŵ��ŵõ���Ӧ��Ƶ�ʷ�Χ
//           usTempFreqNum --- �ŵ���  
//        ���أ�Ƶ�ʷ�Χ
//---------------------------------------------
UCHAR FreqToArea(USHORT usTempFreqNum)
{
    if(usTempFreqNum <= 124)
    {
        return GSM900;
    }
    else if((usTempFreqNum >= 975) && (usTempFreqNum <= 1023))
    {
        return EGSM900;
    }
    else if((usTempFreqNum >= 512) && (usTempFreqNum <= 885))
    {
        return GSM1800;
    }
    else if((usTempFreqNum >= 9600) && (usTempFreqNum <= 10837))
    {
        return WCDMA;
    }
    else return 0;
}

//======================================================================
//���ܣ�	ѡƵ�����õĳ�ʼ��
//����дʱ�䣺2006��3��14��
//======================================================================
void SetFreqInit(void)
{
    PioOpen( PIO_STATE, FRE_ALL_CTRL, PIO_OUTPUT); //��������ATT�ܽ�Ϊ���״̬������PIO����
}

//*************************************************************
//������;��ִ�м���õķ�Ƶ�Ⱥ�Ƶ������ֵ
//���������ulFreqRate -- ��Ƶ��   RATE_LENGTH -- ��Ƶ����Ч����λ����
//			ulFreqData -- Ƶ������ֵ  DATA_LENGTH -- Ƶ������ֵ��Ч����λ����
//			NUMBER -- ͨ����
//�������أ���
//-------------------------------------------------------------
void FreqDataOutput(ULONG ulFreqRate,ULONG ulFreqData,UCHAR RATE_LENGTH,UCHAR DATA_LENGTH,UCHAR NUMBER)
{
    UCHAR i,j;
    USHORT TEMP_HL;
    USHORT Value[2] = {0, 0};//0��ŵ��ֽڡ�1��Ÿ��ֽ�

/********* test *********
        i = 0xff;
        UsartWrite(&g_stUsart0, &i, 1);
        while(g_stUsart0.pstData->pucTxIn!=g_stUsart0.pstData->pucTxOut);
/************************/

    memcpy(Value, &ulFreqRate, sizeof(ulFreqRate));

    TEMP_HL = Value[0];
    
    CLR_FRE_CE;
    CLR_FRE_CK;  //FRE_CK=0;
    j = 250;
    while(j--);  //��ʱ����ʱ�䣬�������ȶ�
    j = 250;
    while(j--);  //��ʱ����ʱ�䣬�������ȶ�

    for(i=0;i<16;i++)  //���ͷ�Ƶ��
    {
        if((TEMP_HL & 0x00000001) == 0)
        {
            CLR_FRE_DA;  //FRE_DA=0;
        }
        else SET_FRE_DA;  //FRE_DA=1;
        TEMP_HL >>= 1;
        SET_FRE_CK;  //FRE_CK=1;
        j = 65;
        while(j--);  //��ʱ����ʱ�䣬�������ȶ�
        CLR_FRE_CK;  //FRE_CK=0;
        j = 60;
        while(j--);  //��ʱ����ʱ�䣬�������ȶ�
    }
    TEMP_HL = Value[1];
    for(i=0;i<(RATE_LENGTH-16);i++)  //���ͷ�Ƶ��
    {
        if((TEMP_HL & 0x00000001) == 0)
        {
            CLR_FRE_DA;  //FRE_DA=0;
        }
        else SET_FRE_DA;  //FRE_DA=1;
        TEMP_HL >>= 1;
        SET_FRE_CK;  //FRE_CK=1;
        j = 65;
        while(j--);  //��ʱ����ʱ�䣬�������ȶ�
        CLR_FRE_CK;  //FRE_CK=0;
        j = 60;
        while(j--);  //��ʱ����ʱ�䣬�������ȶ�
    }
    TEMP_HL = ((NUMBER - 1) << MOVE_NUMBER);
    DataToPio(PIO_STATE, TEMP_HL, FRE_S0|FRE_S1|FRE_S2|FRE_S3); //Ƭѡ���ݷ���
    j = 250;
    while(j--);  //��ʱ����ʱ�䣬�������ȶ�
    SET_FRE_CE;
    j = 100;
    while(j--);  //��ʱ����ʱ�䣬�������ȶ�
    CLR_FRE_CE;
    j = 250;
    while(j--);  //��ʱ����ʱ�䣬�������ȶ�
    j = 250;
    while(j--);  //��ʱ����ʱ�䣬�������ȶ�
    j = 250;
    while(j--);  //��ʱ����ʱ�䣬�������ȶ�

 //------------- ������ɷ�Ƶ�����ã�һ��ִ���ŵ����� ---------------
 
    memcpy(Value, &ulFreqData, sizeof(ulFreqData));

    TEMP_HL = Value[0];

    for(i=0;i<16;i++)  //���ͷ�Ƶ��
    {
        if((TEMP_HL & 0x00000001) == 0)
        {
            CLR_FRE_DA;  //FRE_DA=0;
        }
        else SET_FRE_DA;  //FRE_DA=1;
        TEMP_HL >>= 1;
        SET_FRE_CK;  //FRE_CK=1;
        j = 65;
        while(j--);  //��ʱ����ʱ�䣬�������ȶ�
        CLR_FRE_CK;  //FRE_CK=0;
        j = 60;
        while(j--);  //��ʱ����ʱ�䣬�������ȶ�
    }
    TEMP_HL = Value[1];
    for(i=0;i<(DATA_LENGTH - 16);i++)  //���ͷ�Ƶ��
    {
        if((TEMP_HL & 0x00000001) == 0)
        {
            CLR_FRE_DA;  //FRE_DA=0;
        }
        else SET_FRE_DA;  //FRE_DA=1;
        TEMP_HL >>= 1;
        SET_FRE_CK;  //FRE_CK=1;
        j = 65;
        while(j--);  //��ʱ����ʱ�䣬�������ȶ�
        CLR_FRE_CK;  //FRE_CK=0;
        j = 60;
        while(j--);  //��ʱ����ʱ�䣬�������ȶ�
    }
    TEMP_HL = ((NUMBER - 1) << MOVE_NUMBER);
    DataToPio(PIO_STATE, TEMP_HL, FRE_S0|FRE_S1|FRE_S2|FRE_S3); //Ƭѡ���ݷ���
    j = 250;
    while(j--);  //��ʱ����ʱ�䣬�������ȶ�
    SET_FRE_CE;
    j = 100;
    while(j--);  //��ʱ����ʱ�䣬�������ȶ�
    CLR_FRE_CE;
    j = 250;
    while(j--);  //��ʱ����ʱ�䣬�������ȶ�
    j = 250;
    while(j--);  //��ʱ����ʱ�䣬�������ȶ�
    j = 250;
    while(j--);  //��ʱ����ʱ�䣬�������ȶ�

}

/*********************************************************
EGSM900ϵͳ�����ŵ���  fu1(n)=890+0.2n (MHz) ��0��n��124
                       fu2(n)= (MHz) ��975��n��1023
�����ŵ���             fd(n)=fu(n)+45 (MHz)��0��n��124
GSM1800ϵͳ�����ŵ���fu(n)=1710.2+0.2(n-512) (MHz)��512��n��885
�����ŵ���           fd(n)=fu(n)+95 (MHz)��512��n��885
*********************************************************/

//*************************************************************
//������;�����÷�Ƶ�Ⱥ��ŵ��ţ������ⲿƵ������
//���������devtype -- �豸����
//��    ע���ú�����Ӧ�ò���ã�����¼��setfreq.hͷ�ļ���
//*************************************************************
void SetFreq(void)  //devtype--�豸����  
{
    UCHAR ucFreqRateLength, ucFreqDataLength, ucFreqArea;
        //ucFreqRateLength--��Ƶ����Чλ��  ucFreqDataLength--Ƶ������ֵ��Чλ��
    USHORT usFreqNum;
    ULONG ulFreqRate, ulFreqData;  //ulFreqRate--��Ƶ�� ulFreqData--Ƶ������ֵ

    switch (g_stDevTypeTable.ucSetFreqType)
    {
        //@@@@@@@@@@@@@@@@@ ����� @@@@@@@@@@@@@@@@@
    case FREQTYPE_WIDEBAND:
        //                                   ��ȡ����Ƶ���±ߴ��ŵ��ţ�ͨ��1-�±ߴ�-���� 1��
        usFreqNum = g_stSettingParamSet.usWorkLbChNum1;
        ucFreqArea=FreqToArea(usFreqNum);  //��ʱ���Ƶ������Ƶ����Ϣ��
        //******** 2006-07-18�����޸ĵģ�����2G��3G ********
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)
        {
            ucFreqArea = GSM900_S;
        }
//������%����*��������#��#%��#��%����#��������%����*��������#��#%��#��������%
        ulFreqRate = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_DATA;
        ucFreqRateLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_LEN;
        ucFreqDataLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].SET_LEN;
//������%����*��������#��#%��#��%����#��������%����*��������#��#%��#��������%
        
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)+12000,-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,1);

        //                                       ����Ƶ���±ߴ��ŵ��ţ�ͨ��1-�±ߴ�-���� 9��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)+12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,9);
        //----------------------
        //                                       ����Ƶ���ϱߴ��ŵ��ţ�ͨ��1-�ϱߴ�-���� 2��
        usFreqNum = g_stSettingParamSet.usWorkUbChNum1;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)-12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,2);

        //                                       ����Ƶ���ϱߴ��ŵ��ţ�ͨ��1-�ϱߴ�-���� 10��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)-12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,10);
        //------------------------------------------------
        //                                       ����Ƶ���±ߴ��ŵ��ţ�ͨ��2-�±ߴ�-���� 3��
        usFreqNum = g_stSettingParamSet.usWorkLbChNum2;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)+12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,3);

        //                                       ����Ƶ���±ߴ��ŵ��ţ�ͨ��2-�±ߴ�-���� 11��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)+12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,11);
        //----------------------
        //                                       ����Ƶ���ϱߴ��ŵ��ţ�ͨ��2-�ϱߴ�-���� 4��
        usFreqNum = g_stSettingParamSet.usWorkUbChNum2;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)-12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,4);

        //                                       ����Ƶ���ϱߴ��ŵ��ţ�ͨ��2-�ϱߴ�-���� 12��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)-12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,12);
        //------------------------------------------------
        //                                       ����Ƶ���±ߴ��ŵ��ţ�ͨ��3-�±ߴ�-���� 5��
        usFreqNum = g_stSettingParamSet.usWorkLbChNum3;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)+12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,5);

        //                                      ����Ƶ���±ߴ��ŵ��ţ�ͨ��3-�±ߴ�-���� 13��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)+12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,13);
        //----------------------
        //                                       ����Ƶ���ϱߴ��ŵ��ţ�ͨ��3-�ϱߴ�-���� 6��
        usFreqNum = g_stSettingParamSet.usWorkUbChNum3;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)-12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,6);

        //                                       ����Ƶ���ϱߴ��ŵ��ţ�ͨ��3-�ϱߴ�-���� 14��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)-12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,14);
        //------------------------------------------------
        //                                       ����Ƶ���±ߴ��ŵ��ţ�ͨ��4-�±ߴ�-���� 7��
        usFreqNum = g_stSettingParamSet.usWorkLbChNum4;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)+12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,7);

        //                                       ����Ƶ���±ߴ��ŵ��ţ�ͨ��4-�±ߴ�-���� 15��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)+12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,15);
        //----------------------
        //                                       ����Ƶ���ϱߴ��ŵ��ţ�ͨ��4-�ϱߴ�-���� 8��
        usFreqNum = g_stSettingParamSet.usWorkUbChNum4;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)-12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,8);

        //                                       ����Ƶ���ϱߴ��ŵ��ţ�ͨ��4-�ϱߴ�-���� 16��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)-12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,16);
        
        break;
        //@@@@@@@@@@@@@@@@@ ����� @@@@@@@@@@@@@@@@@
        
        //@@@@@@@@@@@@@@@@@ ѡƵ�� @@@@@@@@@@@@@@@@@
    case FREQTYPE_FRESELT: 

        //I2cReadMore(&usFreqNum,&mSetPar.ChaNum1,2); //�����ŵ���1��ͨ��1-���� 1��
        usFreqNum = g_stSettingParamSet.usWorkChNum1;

//������%����*��������#��#%��#��%����#��������%����*��������#��#%��#��������%
        ucFreqArea=FreqToArea(usFreqNum);  //��ʱ���Ƶ������Ƶ����Ϣ��
        ulFreqRate = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_DATA;
        ucFreqRateLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_LEN;
        ucFreqDataLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].SET_LEN;
//������%����*��������#��#%��#��%����#��������%����*��������#��#%��#��������%

        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,1);

        //                                            �����ŵ���1��ͨ��1-���� 9��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,9);
        //--------------------------
        //                                            �����ŵ���2��ͨ��2-���� 2��
        usFreqNum = g_stSettingParamSet.usWorkChNum2;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,2);

        //                                            �����ŵ���2��ͨ��2-���� 10��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,10);
        //--------------------------
        //                                            �����ŵ���3��ͨ��3-���� 3��
        usFreqNum = g_stSettingParamSet.usWorkChNum3;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,3);

        //                                            �����ŵ���3��ͨ��3-���� 11��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,11);
        //--------------------------
        //                                             �����ŵ���4��ͨ��4-���� 4��
        usFreqNum = g_stSettingParamSet.usWorkChNum4;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,4);

        //                                            �����ŵ���4��ͨ��4-���� 12��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,12);
        //--------------------------
        //                                            �����ŵ���5��ͨ��5-���� 5��
        usFreqNum = g_stSettingParamSet.usWorkChNum5;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,5);

        //                                            �����ŵ���5��ͨ��5-���� 13��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,13);
        //--------------------------
        //                                             �����ŵ���6��ͨ��6-���� 6��
        usFreqNum = g_stSettingParamSet.usWorkChNum6;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,6);

        //                                            �����ŵ���6��ͨ��6-���� 14��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,14);
        //--------------------------
        //                                             �����ŵ���7��ͨ��7-���� 7��
        usFreqNum = g_stSettingParamSet.usWorkChNum7;
        //MCM-19_20061107_zhangjie_begin
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        //MCM-19_20061107_zhangjie_end
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,7);

        //                                            �����ŵ���7��ͨ��7-���� 15��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,15);
        //--------------------------
        //                                             �����ŵ���8��ͨ��8-���� 8��
        usFreqNum = g_stSettingParamSet.usWorkChNum8;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,8);

        //                                             �����ŵ���8��ͨ��8-���� 16��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,16);
        break;
        //@@@@@@@@@@@@@@@@@ ѡƵ�� @@@@@@@@@@@@@@@@@
        
        //@@@@@@@@@@@@@@@@@ ��Ƶ�� @@@@@@@@@@@@@@@@@
    case FREQTYPE_FRESHIFT:    
        //                                            �����ŵ���1������  1��
        usFreqNum = g_stSettingParamSet.usWorkChNum1;

//������%����*��������#��#%��#��%����#��������%����*��������#��#%��#��������%
        ucFreqArea=FreqToArea(usFreqNum);  //��ʱ���Ƶ������Ƶ����Ϣ��
        //******** 2006-07-18�����޸ĵģ�����2G��3G ********
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)
        {
            ucFreqArea = GSM900_256;
        }
        ulFreqRate = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_DATA;
        ucFreqRateLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_LEN;
        ucFreqDataLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].SET_LEN;
//������%����*��������#��#%��#��%����#��������%����*��������#��#%��#��������%

        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,1);

        //                                            �����ŵ���1������  9��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,9);
        //--------------------------
        //                                             �����ŵ���2������  2��
        usFreqNum = g_stSettingParamSet.usWorkChNum2;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,2);

        //                                            �����ŵ���2������  10��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,10);
        //--------------------------
        //                                             �����ŵ���3������  5/3��
        usFreqNum = g_stSettingParamSet.usWorkChNum3;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,5);

        //                                            �����ŵ���3������  13/11��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,13);
        //--------------------------
        //                                             �����ŵ���4������  6/4��
        usFreqNum = g_stSettingParamSet.usWorkChNum4;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,6);

        //                                            �����ŵ���4������  14/12��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,14);
        
        //--------------------------------------------------------
        //                                                ��Ƶ�ŵ���1������  3/5��
        usFreqNum = g_stSettingParamSet.usShfChNum1;

//������%����*��������#��#%��#��%����#��������%����*��������#��#%��#��������%
        ucFreqArea=FreqToArea(usFreqNum);  //��ʱ���Ƶ������Ƶ����Ϣ��
        ulFreqRate = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_DATA;
        ucFreqRateLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_LEN;
        ucFreqDataLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].SET_LEN;
//������%����*��������#��#%��#��%����#��������%����*��������#��#%��#��������%

        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,3);

        //                                                ��Ƶ�ŵ���1������  11/13��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,11);
        //--------------------------
        //                                                  ��Ƶ�ŵ���2������  4/6��
        usFreqNum = g_stSettingParamSet.usShfChNum2;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,4);

        //                                                ��Ƶ�ŵ���2������  12/14��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,12);
        //--------------------------
        //                                                  ��Ƶ�ŵ���3������  7��
        usFreqNum = g_stSettingParamSet.usShfChNum3;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,7);

        //                                                 ��Ƶ�ŵ���3������  15��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,15);
        //--------------------------
        //                                                  ��Ƶ�ŵ���4������  8��
        usFreqNum = g_stSettingParamSet.usShfChNum4;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,8);

        //                                                ��Ƶ�ŵ���4������  16��
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,16);
        
        break;
        //@@@@@@@@@@@@@@@@@ ��Ƶ�� @@@@@@@@@@@@@@@@@

        //@@@@@@@@@@@@@@@ ��Ƶ����� @@@@@@@@@@@@@@@
    case FREQTYPE_FRESHIFT_WIDEBAND:
    
        //                                             ����Ƶ�����±ߴ���ͨ��1��������  1��
        usFreqNum = g_stSettingParamSet.usWorkLbChNum1;

//������%����*��������#��#%��#��%����#��������%����*��������#��#%��#��������%
        ucFreqArea=FreqToArea(usFreqNum);  //��ʱ���Ƶ������Ƶ����Ϣ��
        //******** 2006-07-18�����޸ĵģ�����2G��3G ********
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)
        {
            ucFreqArea = GSM900_S;
        }
        ulFreqRate = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_DATA;
        ucFreqRateLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_LEN;
        ucFreqDataLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].SET_LEN;
//������%����*��������#��#%��#��%����#��������%����*��������#��#%��#��������%
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)+12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,1);

        //                                              ����Ƶ�����±ߴ���ͨ��1��������  9��
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)+12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,9);
        //--------------------------
        //                                              ����Ƶ�����ϱߴ���ͨ��1��������  2��
        usFreqNum = g_stSettingParamSet.usWorkUbChNum1;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)-12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,2);

        //                                              ����Ƶ�����ϱߴ���ͨ��1��������  10��
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)-12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,10);
        
        //----------------------------------------------------
        //                                              ����Ƶ�����±ߴ���ͨ��2��������  3��
        usFreqNum = g_stSettingParamSet.usWorkLbChNum2;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)-12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,3);

        //                                              ����Ƶ�����±ߴ���ͨ��2��������  11��
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)-12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,11);
        //--------------------------
        //                                              ����Ƶ�����ϱߴ���ͨ��2��������  4��
        usFreqNum = g_stSettingParamSet.usWorkUbChNum2;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)-12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,4);

        //                                              ����Ƶ�����ϱߴ���ͨ��2��������  12��
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)-12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,12);
        
        //����������������������������������������������������������������������������������
        //                                              ��ƵƵ�����±ߴ���ͨ��1��������  5��
        usFreqNum = g_stSettingParamSet.usShfLbChNum1;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)+12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,5);

        //                                              ��ƵƵ�����±ߴ���ͨ��1�������� 13��
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)+12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,13);
        //--------------------------
        //                                              ��ƵƵ�����ϱߴ���ͨ��1��������  6��
        usFreqNum = g_stSettingParamSet.usShfUbChNum1;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)-12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,6);

        //                                               ��ƵƵ�����ϱߴ���ͨ��1�������� 14��
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)-12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,14);
        //--------------------------------------------------
        //                                               ��ƵƵ�����±ߴ���ͨ��2��������  7��
        usFreqNum = g_stSettingParamSet.usShfLbChNum2;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)+12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,7);

        //                                               ��ƵƵ�����±ߴ���ͨ��2�������� 15��
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)+12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,15);
        //--------------------------
        //                                               ��ƵƵ�����ϱߴ���ͨ��2��������  8��
        usFreqNum = g_stSettingParamSet.usShfUbChNum2;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)-12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,8);

        //                                               ��ƵƵ�����ϱߴ���ͨ��2�������� 16��
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)-12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,16);
        
        break;
        //@@@@@@@@@@@@@@@ ��Ƶ����� @@@@@@@@@@@@@@@

    default: break;
    }
}


/*********************************************************
EGSM900ϵͳ�����ŵ���
                          fu1(n)=890+0.2n (MHz) ��0��n��124
                          fu2(n)=890+0.2(n-1024) (MHz) ��975��n��1023
�����ŵ���
                          fd(n)=fu(n)+45 (MHz)��0��n��124
GSM1800ϵͳ�����ŵ���
                        fu(n)=1710.2+0.2(n-512) (MHz)��512��n��885
�����ŵ���
                        fd(n)=fu(n)+95 (MHz)��512��n��885
*********************************************************/
