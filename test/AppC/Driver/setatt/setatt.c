/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   setatt.c
    ����:     ����
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  
       ����:  ����
       ����:  �����д���
    2. ����:  2006/11/06
       ����:  ����
       ����:  �޸�����MCM-17�������д�ͨ���ŵ�ͨ��1�Ĳ�����ӳ���Ӧ��
              ������ͨ���ŵĲ����ϣ������豸�Ƕ�ͨ��ʱ����ѯͨ��1�Ĳ�
              ��ʵ�ʷ��ʵ��Ƕ�Ӧ�Ĳ���ͨ���ŵĲ����ĵ�ַ��
---------------------------------------------------------------------------*/
/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
CPLD��·�й���˥���������Ĳ��裺
      1����ATT_CS1��ATT_CS2��Ƭѡĳһ�����Ż�����
      2����˥��ֵ��ATT_1��ATT_16�ͳ���CPLD
      3����ATT_OUTEN���ͣ���CPLD����������ݷ��ͳ�ȥ
      4���ʵ���ʱ�������Ҫ��ʱ�Ļ�������ATT_OUTEN����

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "../../include/UserType.h"
#include "../../include/at91m55800.h"
#include "../../include/MyBoard.h"
#include "../../periph/pio/pio.h"
#include "../../OH/CMCC_1_Ext.h"
#include "../../OH/YKPP.h"
#include "../../periph/usart/Usart.h"
//#include "setatt.h"

//********************************************
//               �ܽ��������               **
//********************************************

#define PIO_STATE &g_stPioB  //�����ڷ�������ʱ��PIOB����

#define ATT_CS1  PB6  //˥����1��Ƭѡ  PIOB��
#define ATT_CS2  PB7  //˥����2��Ƭѡ  PIOB��
#define ATT_OUTEN PB5  //˥�������ݷ���ʹ��  PIOB��
//��˥�������ݷ���ʹ�����ߣ�ʹATT���ھ���״̬
#define SET_CS PioWrite( PIO_STATE, ATT_OUTEN, PIO_SET_OUT)
  //#define SET_CS MA_OUTWM( PIO_SODR_B, ATT_OUTEN, ATT_OUTEN);
//��˥�������ݷ���ʹ�����ͣ���ATT���ݷ��ͳ�ȥ
#define CLR_CS PioWrite( PIO_STATE, ATT_OUTEN, PIO_CLEAR_OUT)
  //#define CLR_CS MA_OUTWM( PIO_CODR_B, ATT_OUTEN, ATT_OUTEN);


#define ATT_1  PB0  //˥����ֵ�����λ  PIOB��
#define ATT_2  PB1  //˥����ֵ2         PIOB��
#define ATT_4  PB2  //˥����ֵ4         PIOB��
#define ATT_8  PB3  //˥����ֵ8         PIOB��
#define ATT_16 PB4  //˥����ֵ�����λ  PIOB��
#define ATT_INDEX 0x1F //��ʾATT���ݽ������5λ

#define ATT_ALL_DATA  ATT_1|ATT_2|ATT_4|ATT_8|ATT_16
#define ATT_ALL_CTRL  ATT_CS1|ATT_CS2|ATT_OUTEN|ATT_ALL_DATA


extern SETTING_PARAM_SET_ST g_stSettingParamSet;

//������չ����
extern YKPP_PARAM_SET_ST g_stYkppParamSet;

void SetAttInit(void);
void SetAtt(void);
void SetRfSw(void);
void SetRfSwUD(void);

//********************************************
//                 ����ʵ��                 **
//********************************************

//======================================================================
//���ܣ�	˥�������õĳ�ʼ��
//����дʱ�䣺2006��3��14��
//======================================================================
void SetAttInit(void)
{
    PioOpen( PIO_STATE, ATT_ALL_CTRL, PIO_OUTPUT); //��������ATT�ܽ�Ϊ���״̬������PIO����
    
//    //ʹ��������˥�������Ƶ�8���ܽŶ���PIO����
//    MA_OUTWM(PIO_PER_B, ATT_ALL_CTRL, ATT_ALL_CTRL);
//    //������������˥�������Ƶ�8���ܽ�Ϊ���״̬
//    MA_OUTWM(PIO_ODR_B, ATT_ALL_CTRL, ATT_ALL_CTRL);
    
    //��˥�������ݷ���ʹ�����ߣ�ʹATT���ھ���״̬
    SET_CS;
}

//======================================================================
//���ܣ�	˥�������ƣ�֮ǰ�����SetAttInit()���йܽų�ʼ��
//����дʱ�䣺2006��3��17��
//��ע��        ����4�������ǰ����ΪATT�������4��Ϊ�ϡ����й��ſ��غ�????
//======================================================================
void SetAtt(void)
{
    UINT32 uiAttSwitch, uiAttOutput;

    //------------ ͨ��1������˥��ֵ1������ ------------
    DataToPio(PIO_STATE, ATT_CS1, ATT_CS1|ATT_CS2); //ѡ���2�飬������ͨ��1������˥������
    
    //����˥��ֵ��ͨ��1�� �� uiAttOutput
    //MCM-17_20061106_linyu_begin
    uiAttOutput = g_stSettingParamSet.ucUlAtt;  //����˥��ֵ��ͨ��1��
    //MCM-17_20061106_linyu_end
    uiAttOutput = ~uiAttOutput;
    uiAttOutput &= 0x1F;
    
    DataToPio(PIO_STATE, uiAttOutput, ATT_INDEX);  //  P0 = uiAttOutput;
  
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //��ʱ����΢��
    CLR_CS;    //ATT1_CS = 1;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //��ʱ����΢��
    SET_CS;    //ATT1_CS = 0;
    
    //------------ ͨ��1������˥��ֵ������ ------------
    DataToPio(PIO_STATE, 0, ATT_CS1|ATT_CS2); //ѡ���1�飬������ͨ��1������˥������
    
    //����˥��ֵ��ͨ��1�� �� uiAttOutput
    //MCM-17_20061106_linyu_begin
    uiAttOutput = g_stSettingParamSet.ucDlAtt;  //����˥��ֵ��ͨ��1��
    //MCM-17_20061106_linyu_end
    uiAttOutput = ~uiAttOutput;
    uiAttOutput &= 0x1F;
    
    DataToPio(PIO_STATE, uiAttOutput, ATT_INDEX);  //  P0 = uiAttOutput;
    
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //��ʱ����΢��
    CLR_CS;    //ATT2_CS = 1;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //��ʱ����΢��
    SET_CS;    //ATT2_CS = 0;
    
    //------------ ͨ��2������˥��ֵ������ ------------
    DataToPio(PIO_STATE, ATT_CS2, ATT_CS1|ATT_CS2); //ѡ���3�飬������ͨ��2������˥������
    
    //����˥��ֵ��ͨ��2�� �� uiAttOutput
    uiAttOutput = g_stSettingParamSet.ucDlAtt2;  //����˥��ֵ��ͨ��2��
    uiAttOutput = ~uiAttOutput;
    uiAttOutput &= 0x1F;
    
    DataToPio(PIO_STATE, uiAttOutput, ATT_INDEX);  //  P0 = uiAttOutput;
    
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //��ʱ����΢��
    CLR_CS;    //ATT3_CS = 1;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //��ʱ����΢��
    SET_CS;    //ATT3_CS = 0;
}

//======================================================================
//���ܣ�	��Ƶ���ؿ��ƣ�֮ǰ�����SetAttInit()���йܽų�ʼ��
//����дʱ�䣺2006��4��29��
//��ע��        ��SETATT�з��������
//======================================================================
void SetRfSw(void)
{
    UINT32 uiAttSwitch, uiAttOutput;

    DataToPio(PIO_STATE, ATT_CS1|ATT_CS2, ATT_CS1|ATT_CS2); //ѡ���4��

    uiAttOutput = 0;
    //��Ƶ�źſ���״̬ �� uiAttSwitch
    uiAttSwitch = g_stSettingParamSet.ucRfSw;  //��Ƶ�źſ���״̬
    if((uiAttSwitch & 0x01) == 0)
    {
        uiAttOutput |= 0x07; //B��00000111��
    }
    else uiAttOutput = 0;

    DataToPio(PIO_STATE, uiAttOutput, ATT_INDEX);  //P0 = uiAttOutput;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //��ʱ����΢��
    CLR_CS;    //ATT4_CS = 1;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //��ʱ����΢��	
    SET_CS;    //ATT4_CS = 0;
}

//======================================================================
//���ܣ�	      ��Ƶ���ط��ϡ����У�����ʱר�ã�δ��ɣ�
//����дʱ�䣺2006��5��11��
//��ע��       
//======================================================================
void SetRfSwUD(void)
{
    UINT32 uiAttSwitch, uiAttOutput;

    DataToPio(PIO_STATE, ATT_CS1|ATT_CS2, ATT_CS1|ATT_CS2); //ѡ���4��

    uiAttOutput = 0;
    
    //��Ƶ�źſ���״̬ �� uiAttSwitch
    uiAttOutput = 0;
    uiAttOutput |= g_stYkppParamSet.stYkppRfParam.ucDlRfSw;  //������Ƶ�źſ���
    uiAttOutput |= g_stYkppParamSet.stYkppRfParam.ucUlRfSw << 1;  //������Ƶ�źſ���
    uiAttOutput |= g_stSettingParamSet.ucRfSw << 2;  //��Ƶ�źſ���״̬

    DataToPio(PIO_STATE, uiAttOutput, ATT_INDEX);  //P0 = uiAttOutput;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //��ʱ����΢��
    CLR_CS;    //ATT4_CS = 1;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //��ʱ����΢��	
    SET_CS;    //ATT4_CS = 0;
}
