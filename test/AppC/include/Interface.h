/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   Interface.h
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
              ����ȡ�������ĺ����ԭ��ṹ��������������Ϣ���кţ�
              ���ڲ�������ʱ��Ϊ�������������ο���
    3. ����:  2006/11/07
       ����:  ����
       ����:  �޸�����MCM-20��
              ���Ӱ��������ϱ�����
---------------------------------------------------------------------------*/
#ifndef InterfaceH
#define InterfaceH

#include "UserType.h"

//ԭ�����Ͷ���
#define OHCH_SEND_DATA_REQ          0//OH����CH�������ݻ�CH����OHת�����ݣ�Ŀǰֻ������CH��Ҫ��ת����
#define OHCH_SEND_DATA_CNF          1
#define OHCH_RECV_DATA_IND          2
#define OHCH_R_SMC_ADDR_REQ         3
#define OHCH_W_SMC_ADDR_REQ         4
#define OHCH_RW_SMC_ADDR_CNF        5

//#define OHCH_GPRS_CONN_REQ          0x10
//#define OHCH_GPRS_CONN_CNF          0x11
#define OHCH_GPRS_DISCONN_REQ       0x12
#define OHCH_GPRS_DISCONN_CNF       0x13
#define OHCH_GPRS_DISCONN_IND       0x14
#define OHCH_SLAVE_UNREG_REQ        0x15
#define OHCH_SLAVE_UNREG_CNF        0x16
#define OHCH_CSD_CONN_IND           0x17 //CH�����Ȩ
#define OHCH_CSD_CONN_RSP           0x18 //OH�Լ�Ȩ�Ļ�Ӧ
#define OHCH_CSD_DISCONN_IND        0x19 //CH֪ͨ����
#define OHCH_CONN_CENTER_REQ        0x1A //��������GPRS���ӣ���Ҫ����TCP����
#define OHCH_CONN_CENTER_CNF        0x1B

#define MESS_QUERY_REQ              0x00010001 //��ѯ���豸������
#define MESS_QUERY_RSP              0x00010002 //��ѯӦ���������豸
#define MESS_SET_REQ                0x00010003 //���ã��豸������
#define MESS_SET_RSP                0x00010004 //����Ӧ���������豸
#define MESS_ALARM_REP              0x00010005 //�����ϱ����������豸

#define ACCH_MESS_SEND_REQ          0x1C //AC����CH������Ϣ��Ŀǰֻ����Ƶ��Ϣ
#define ACCH_MESS_RECV_IND          0x1D //CH�յ���Ϣ��֪ͨAC��Ŀǰֻ����Ƶ��Ϣ
#define OHCH_MESS_SEND_REQ          0x1E //OH����CH������Ϣ
#define OHCH_MESS_RECV_IND          0x1F //CH�յ���Ϣ��֪ͨOH

#define OHCH_UNDE_SEND_REQ          0x20 //OH����CH����δ������
#define OHCH_UNDE_RECV_IND          0x21 //CH�յ�δ�����ݺ�֪ͨOH
#define OHCH_DEBU_SEND_REQ          0x22 //OH����CH�����ѵ����ݻ�CH����OHת���ѵ�����

#define TIMEOUT_EVENT               0x28
#define ALARM_STATUS_CHANGED_EVENT  0x29
#define KEY_DOWN_EVENT              0x2A

#define LOG_MSG_REQ                 0x30 //��־����

#define MAX_TEL_NUM_LEN     20
#define MAX_DATA_LEN        300

#define COMM_IF_SOUTH       0 //����ӿ�
#define COMM_IF_NORTH       1 //����ӿ�
#define COMM_IF_DEBUG       2 //���Խӿ�
#define DATA_TYPE_RSP       0xFF //���ݵ�����ΪӦ���ϱ�ֱ�Ӳ���Э���й涨���ϱ�����
//#define DATA_TYPE_REPORT    1 //���ݵ�����Ϊ�����ϱ�

typedef struct
{
    ULONG ulPrmvType;   //����ģ��+����ģ��+������
    ULONG ulMagicNum;   //���ڽ������ȷ�϶�Ӧ����
    UCHAR ucResendTime; //��¼�ط�����
    CHAR  cResult;      //��ȷ�Ϸ���ʱ��Ч
    UCHAR ucDstIf;      //�������ݵ�Ŀ�Ľӿڣ����������
    UCHAR ucCommMode;   //ͨ�ŷ�ʽ
    UCHAR ucDCS;        //����Ƕ���Ϣ��������Ҫ���뷽ʽ������0��GSM7Bit��8��UCS2
    UCHAR ucDA;         //��ַ����
    CHAR  acDstTelNum[MAX_TEL_NUM_LEN + 1]; //�绰��������ֵ0��β
    UCHAR ucDataType;   //���ݵ����ͣ�0��ʾ�����ϱ���1��ʾ����Ӧ�������²�ԭ�ⲻ���ط��ع��ϲ��ж�
    ULONG ulDataLen;    //���͵����ݳ���
    UCHAR *pucData;     //���͵�����
}OHCH_SEND_DATA_REQ_ST, OHCH_SEND_DATA_CNF_ST;

#define DATA_START_POS  sizeof(OHCH_SEND_DATA_REQ_ST)  //ԭ������������ʼ��λ��
/*
typedef struct
{
    ULONG ulPrmvType;   //����ģ��+����ģ��+������
    ULONG ulMagicNum;   //��Ӧ�ڷ�������
    UCHAR ucDataType;   //�����Ӧ��SEND_DATA_REQ�е�DataTypeȡֵ��ͬ
    UCHAR ucResult;     //0��ʾ�ɹ�����0��ʾʧ�ܵ�ԭ��
}OHCH_SEND_DATA_CNF_ST;
*/
typedef struct
{
    ULONG ulPrmvType;   //����ģ��+����ģ��+������
    //ULONG ulPadding1;   //����ռλ
    ULONG ulDataLenMax; //��ԭ�����뷽��д����ʾ�����data�ռ���õ���󳤶ȣ���ulDataLen��ʾ����ʵ�ʳ���
    UCHAR ucPadding2;   //����ռλ
    UCHAR ucPadding3;   //����ռλ
    UCHAR ucSrcIf;      //�������ݵ�Դ�ӿڣ����������
    UCHAR ucCommMode;
    UCHAR ucDCS;        //����Ƕ���Ϣ��������Ҫ���뷽ʽ������0��GSM7Bit��8��UCS2
    UCHAR ucDA;         //��ַ����
    CHAR  acSrcTelNum[MAX_TEL_NUM_LEN + 1]; //���ͷ��ĵ绰����
    UCHAR ucPadding4;   //����ռλ
    ULONG ulDataLen;
    UCHAR *pucData;
}OHCH_RECV_DATA_IND_ST;

typedef struct
{
    ULONG ulPrmvType;       //����ģ��+����ģ��+������
    union                   //���ڱ����ڴ�����ʱ��ʱ���õĲ���
    {
        UCHAR aucByte[4];   //�ֽ�����
        ULONG ulDoubleWord; //˫��
        void *pvPtr;        //ָ��
    };
}TIMEOUT_EVENT_ST;

typedef struct
{
    ULONG ulPrmvType;       //����ģ��+����ģ��+������
}ALARM_STATUS_CHANGED_EVENT_ST;

//MCM-20_20061107_Linwei_begin
typedef struct
{
    ULONG ulPrmvType;
    USHORT usKey;      //������ֵ
    UCHAR ucKeyType;  //��������
}KEY_EVENT_ST;
//MCM-20_20061107_Linwei_end

//MCM-10_20061105_Zhonghw_begin
//���������д�������ĺ���
typedef struct
{
    ULONG ulPrmvType;
    UCHAR ucMsgSn;    //��Ϣ���кţ��Ͳ������������ֽ����ÿ��Զ�Ӧ���ƶ�Э��İ���ʶ�ֶ��ϣ�����ӳ��    
    LONG  lResult;
    CHAR  acTelNum[MAX_TEL_NUM_LEN + 1]; //����Ϊ�ַ�������NULL��β
}OHCH_RW_SMC_ADDR_REQ_ST, OHCH_RW_SMC_ADDR_CNF_ST;

typedef struct
{
    ULONG ulPrmvType;
    LONG  lResult;
    CHAR  acTelNum[MAX_TEL_NUM_LEN + 1]; //����Ϊ�ַ�������NULL��β
}OHCH_CSD_CONN_IND_ST, OHCH_CSD_CONN_RSP_ST;
//MCM-10_20061105_Zhonghw_end

#if defined(__BORLANDC__) || defined(_MSC_VER)
#pragma pack(1) //�����������ݶ�����1�ֽڶ���
#define __attribute__(a)
#endif

//��������CH��Զ�˻��߽��˷�������Ϣ֪ͨ���Զˣ�Ŀǰ��Ҫ����Ƶ��Ϣ��̫���ܿ������е�����
typedef struct
{
    ULONG  ulMessType; //��ϢID
    UCHAR  ucObj;      //��������
    UCHAR  ucMsgSn;    //��Ϣ���кţ��Ͳ������������ֽ����ÿ��Զ�Ӧ���ƶ�Э��İ���ʶ�ֶ��ϣ�����ӳ��
    USHORT usDataLen;  //���ݳ���
    UCHAR  aucData[1];
}__attribute__((packed)) MESSAGE_ST;

#if defined(__BORLANDC__) || defined(_MSC_VER)
#pragma pack()
#endif

//������֪ͨ����ֹͣ����(Ŀǰ���ڴӻ�����GPRS����)
#define RESULT_STOP_REQ 0xF1
typedef struct
{
    ULONG  ulPrmvType;
    LONG   lResult;
    UCHAR  aucIpAddr[4]; //������ʱ��Ч
    USHORT usPortNum;    //������ʱ��Ч
}OHCH_CONN_CENTER_REQ_ST, OHCH_CONN_CENTER_CNF_ST;


typedef struct
{
    ULONG ulPrmvType;
    LONG  lResult;
}OHCH_GPRS_DISCONN_REQ_ST, OHCH_GPRS_DISCONN_IND_ST, OHCH_CSD_DISCONN_IND_ST,
OHCH_SLAVE_UNREG_REQ_ST, OHCH_SLAVE_UNREG_CNF_ST;

typedef struct
{
    ULONG  ulPrmvType;
    UCHAR  ucLogSrc;
    UCHAR  ucLevel;
    USHORT usMsgLen;
    UCHAR  aucMsg[1];
}LOG_MSG_REQ_ST;


#endif
 
