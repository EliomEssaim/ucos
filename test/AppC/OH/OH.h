/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   OH.h
    ����:     ����
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  
       ����:  ����
       ����:  �������
    2. ����:  2006/11/05
       ����:  �ӻ���
       ����:  �޸�����MCM-10,
              Ϊ�������Ĳ��������С�Ļ���ṹ,��ʡ�ڴ�
---------------------------------------------------------------------------*/
#ifndef OhH
#define OhH

#include "UserType.h"
#include "CMCC_1.h"
#include "YKPP.h"

//Э����
#define CMCC_1_0    0
#define UNICOM_1_0  1

//���ڳ�ȡЭ�����ݵ�״̬
#define NO_FLAG_FOUND               1 
#define AP_START_FLAG_FOUND         2
#define AP_END_FLAG_FOUND           3
#define APAC_START_FLAG_FOUND       (AP_START_FLAG_FOUND + APAC_FLAG)
#define APB_START_FLAG_FOUND        (AP_START_FLAG_FOUND + APB_FLAG)
#define APAC_END_FLAG_FOUND         (AP_END_FLAG_FOUND   + APAC_FLAG)
#define APB_END_FLAG_FOUND          (AP_END_FLAG_FOUND   + APB_FLAG)

#define NO_CMCC_PROTO_DATA_FOUND    0
#define CMCC_PROTO_DATA_FOUND       1

//��ʱ���ķ��࣬����������ʱ��״̬��ȷ�����ϱ���Ҫ��ʱ����Զ������������Ҫ��ʱ��
#define OH_TIMER_CMCC_T0   0
#define OH_TIMER_CMCC_T1   1
#define OH_TIMER_CMCC_T2   2
#define OH_TIMER_CMCC_T3   3

//��ǰ���õ�Э�鴦������ֻ����һ��Э�鴦������Ч
typedef LONG (*PROTO_HNDLR)(UCHAR *pucData, ULONG ulLen);
extern PROTO_HNDLR g_ucProtoHndlr;

//OH�������ݵ�״̬
#define OH_IDLE                 0
#define OH_SEND_DATA_PENDING    1
#define OH_WAIT_FOR_RSP         2

//OH��ʱ��������ʽ
typedef union
{
    ULONG ulParam;
    struct
    {
        UCHAR  ucTimerID;
        UCHAR  ucParam1;
        USHORT usParam2;
    }stParam;
}OH_TIMER_CTX_UN;

//��ʱ�����壬IDֵС��64������ָͬ����ַ���ֿ�
#define TIMER_TRANS_CTRL_ID         1
#define TIMER_TRANS_BLK_ID          2
#define TIMER_ALARM_REPORT_ID       3
#define TIMER_OTHER_REPORT_ID       4
#define TIMER_HEARTBEAT_TO_ID       5 //������ʱ
#define TIMER_HEARTBEAT_PERIOD_ID   6 //��������
#define TIMER_LOGIN_TO_ID           7 //��¼��ʱ
#define TIMER_LOGIN_PERIOD_ID       8 //���µ�¼�ļ��
#define TIMER_ASYN_OP_TO_ID         9 //�첽������ʱ

#define ASYN_OP_OBJ_LOCAL           0
#define ASYN_OP_OBJ_SEC             (1 << 0) //̫���ܿ�����Solar Energy Controller
#define ASYN_OP_OBJ_TD              (1 << 1) //TDͬ��ģ��
#define ASYN_OP_OBJ_SMC             (1 << 2) //�������ĺ���


typedef void (*CALLBACK_OP)(void *); //�첽�����Ļص���������

typedef struct
{
    CALLBACK_OP pfOp; //�ص�����
    void *pvCtx;      //�ص�������������ָ��
}OP_OBJ;


#define MAX_ASYN_OP_OBJS 8

//�����첽������������
typedef struct
{
    Timer  *pTimer;
    ULONG  ulTimerMagicNum;
    UCHAR  ucSn;     //����ƥ�������Ӧ��
    UCHAR  ucState;  //����(OH_IDLE)���ȴ�Ӧ��(OH_WAIT_FOR_RSP)
    UCHAR  ucObjBitmap; //����ͬ������λͼ��ÿһλ��Ӧһ���첽�������󣬵�Ϊȫ0ʱ��ʾ��������
    OP_OBJ astOp[MAX_ASYN_OP_OBJS]; //���ͬʱ֧��8���첽��������
    CALLBACK_OP pfTo; //�����ĳ�ʱ
    ULONG  ulDataLen;
    void   *pvData;   //ָ�򻺴������
}ASYN_OP_CTX_ST;

#define TIMER_ASYN_OP_INTV          (15000 / TIMER_UNIT_MS) //15s���첽������ʱ

//�����첽�����������ĵĴ�С
#define ASYN_OP_CTX_CNT 3

//��ȡ�첽��������ϢID
#define GEN_ASYN_OP_MSG_ID() ((++g_ucAsynOpMsgID) & 0x7F)

typedef struct
{
    UCHAR  ucOpObj;       //��������
    UCHAR  ucOpType;      //��������
    USHORT usParamLen;    //�����ĳ���
    UCHAR  aucParam[512]; //��������
}ASYN_OP_BUF_ST; //���ڷ����ѯ�����������в�ͬ�����Ĳ���

//MCM-10_20061105_zhonghw_begin
typedef struct
{
    UCHAR  ucOpObj;       //��������
    UCHAR  ucOpType;      //��������
    USHORT usParamLen;    //�����ĳ���
    UCHAR  aucParam[32]; //��������
}ASYN_OP_SMALL_BUF_ST; //���ڷ����ѯ�����������в�ͬ�����Ĳ���,�����ڶ������Ĳ���
//MCM-10_20061105_zhonghw_end
#endif

