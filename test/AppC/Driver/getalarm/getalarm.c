/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

get_alarm->stAlarmItemSet->c

CPLD��·�й��ڿ������ɼ��������裺
    1��GET_DA_CE�����ڿ������ɼ�������ѡƵ������֮���л������л����������ɼ���
    2��Ȼ������GET_DA_S0��GET_DA_S3��ѡ��ɼ�ĳһ��8·��������
    3�����žͿ��Դ�GET_DA0��GET_DA7������ֵ��

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "../../OH/CMCC_1_Ext.h"
#include "../../OH/YKPP.h"
#include "../../AC/Actask.h"
#include "getalarm.h"

extern ALARM_ITEM_UN g_uAlarmItems;

//���������Э����صĲ���
//ͨ���жϴӻ���ſ�����Ƿ�ע��
extern YKPP_PARAM_SET_ST g_stYkppParamSet;

//********************************************
//                �ܽ��������               **
//********************************************
#define GET_DA0 PB8  //������ֵ�����0��  PIOB��
#define GET_DA1 PB9  //������ֵ�����1��  PIOB��
#define GET_DA2 PB10  //������ֵ�����2��  PIOB��
#define GET_DA3 PB11  //������ֵ�����3��  PIOB��
#define GET_DA4 PB12  //������ֵ�����4��  PIOB��
#define GET_DA5 PB13  //������ֵ�����5��  PIOB��
#define GET_DA6 PB14  //������ֵ�����6��  PIOB��
#define GET_DA7 PB15  //������ֵ�����7��  PIOB��
#define GET_DA_DATA  GET_DA7|GET_DA6|GET_DA5|GET_DA4|GET_DA3|GET_DA2|GET_DA1|GET_DA0
#define MOVE_TIME 8

#define GET_DA_CE PA2  //�������ɼ���ʹ�ܽ�  PIOA��

//��GET_DA_CE�����㣬ѡ�񿪹����ɼ�
#define SELECT_ALARM  PioWrite( &g_stPioA, GET_DA_CE, PIO_CLEAR_OUT)
//#define SELECT_ALARM  MA_OUTWM( PIO_CODR_A, GET_DA_CE, GET_DA_CE)

#define GET_DA_S0 PA3  //�������ɼ���Ƭѡ��0��  PIOA��
#define GET_DA_S1 PA4  //�������ɼ���Ƭѡ��1��  PIOA��
#define GET_DA_S2 PA5  //�������ɼ���Ƭѡ��2��  PIOA��
#define GET_DA_S3 PA6  //�������ɼ���Ƭѡ��3��  PIOA��
#define GET_DA_SALL GET_DA_S0|GET_DA_S1|GET_DA_S2|GET_DA_S3
#define GET_DA_CONTROL  GET_DA_CE|GET_DA_SALL

extern void SetAtt(void);
extern void SetFreq(void);

void GetAlarmInit(void);
void GetAlarmData(void);

UCHAR ucSetFreq; //��Ҫ����Ƶ�ʱ�־
UCHAR ucSetAtt;   //��Ҫ����˥�ı�־

//======================================================================
//���ܣ�	�������ɼ��ĳ�ʼ��
//����дʱ�䣺2006��3��14��
//======================================================================
void GetAlarmInit(void)
{
    PioOpen( &g_stPioB, GET_DA_DATA, PIO_INPUT); //���������ɼ����ݽ���PIO���ƣ�������Ϊ����״̬
    PioOpen( &g_stPioA, GET_DA_CONTROL, PIO_OUTPUT); //���������ɼ����ƽ���PIO���ƣ�������Ϊ���״̬
}

#ifdef YK_POI
#include "./poi_switch.c"
#else
#include "./gen_switch.c"
#endif

//======================================================================
//���ܣ�	�������ɼ�����ɿ������ɼ������ֵ
//������        ֮ǰ�����ȵ���GetAlarmInit()���г�ʼ��
//����дʱ�䣺2006��3��14��
//���أ�        ��
//======================================================================
void GetAlarmData(void)
{
    UCHAR aucWAlarmValue[5]; //������ʱ��Ųɼ����Ľ��
    
    SELECT_ALARM; //������Ƶ���Ǳ��л��ؿ������ɼ������ⲿCPLD��·����
    
    PioWrite( &g_stPioA, GET_DA_SALL, PIO_CLEAR_OUT); //Ƭѡ��ѡ���1��8��������
    aucWAlarmValue[0] = ( ( PioRead( &g_stPioB)) >> MOVE_TIME);
    PioWrite( &g_stPioA, GET_DA_S0, PIO_SET_OUT); //Ƭѡ��ѡ���2��8��������
    aucWAlarmValue[1] = ( ( PioRead( &g_stPioB)) >> MOVE_TIME);
    PioWrite( &g_stPioA, GET_DA_S1, PIO_SET_OUT); //Ƭѡ��ѡ���4��8��������
    aucWAlarmValue[3] = ( ( PioRead( &g_stPioB)) >> MOVE_TIME);
    PioWrite( &g_stPioA, GET_DA_SALL, PIO_CLEAR_OUT); //Ƭѡ���Ȼָ�ȫ0���
    PioWrite( &g_stPioA, GET_DA_S1, PIO_SET_OUT); //Ƭѡ��ѡ���3��8��������
    aucWAlarmValue[2] = ( ( PioRead( &g_stPioB)) >> MOVE_TIME);
    PioWrite( &g_stPioA, GET_DA_SALL, PIO_CLEAR_OUT); //Ƭѡ���Ȼָ�ȫ0���
    PioWrite( &g_stPioA, GET_DA_S2, PIO_SET_OUT); //Ƭѡ��ѡ���5��3��������
    aucWAlarmValue[4] = ( ( PioRead( &g_stPioB)) >> MOVE_TIME);

    //------------ �������ݲɼ���ϣ����濪ʼ��д����  -------------
    DisposeResult(aucWAlarmValue);

}



