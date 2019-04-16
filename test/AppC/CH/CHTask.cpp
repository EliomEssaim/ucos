/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   CHTask.cpp
    ����:     �½�
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  2006/09/28
       ����:  �½�
       ����:  �������
    2. ����:  2006/11/10
       ����:  �½�
       ����:  �޸�����MCM-29��
              �ѷ���AT+CCED��������ԭ��һ���Ӹ�Ϊ30��
    3. ����:  2006/11/01
       ����:  �½�
       ����:  �޸�����MCM-11����ԭ�������漰�����жϵĵط�ȫ����Ϊ��
              �������ñ���ĳ����߶���ֵ���жϡ�
    4. ����:  2006/11/06
       ����:  ����
       ����:  �޸�����MCM-18���޸ı��󼴿ɣ���CH_STH_CTRL_QUE_SIZE
              ��ΪCH_DEB_CTRL_QUE_SIZE 
    5. ����:  2006/11/30
       ����:  �½�
       ����:  �޸�����MCM-45��Ϊ���485�����������ٶ����ޣ������յ���
              ѯ���ݺ͹㲥����ʱ����ֶ��������⣬�������޸�:
              1.��������ѯ����ʱ������ӿ�æ 
              2.ֻ�е���ѯ��ʱ�����յ�����ӿ�Ӧ������ʱ��������ӿ��� 
              3.��ֻ������ӿ���ʱ�Żᷢ�͹㲥���� 
              4.���������㲥���ݲ����������ѯ���ݺ��ͣ�����ѯ����
                ���ܽ����ڹ㲥����֮��
    6. ����:  2007/01/09
       ����:  �ӻ���
       ����:  �޸�����MCM-51
              ��AtRtnIsOk()�����У�����CH������Ϣʱ���Ѳ��ܱ��ϵĿ�����Ϣ
              �Ż���Ϣ���У��ɿ�����Ϣ������������
    7. ����:  2007/01/11
       ����:  �ӻ���
       ����:  �޸�����MCM-53
              �޸ı����ڶ������Ĳ�����Ӧ��AtRtnIsOk()��������ʱ��
              �������ȴ�ʱ���6���޸�Ϊ2�롣            
    8. ����:  2007/06/14
       ����:  �ӻ���
       ����:  �޸�����MCM-73
              �ڶԶ��Ž���PUD��ʽ��������У�ֻ��11λ���ֻ����������ǰ�����86��
              ͬʱȷ����������ǰ�涼����86����󽫴˶�������Ϊ���ڶ���
    9. ����:  2007/07/12
       ����:  ����
       ����:  �޸�����MCM-76
              ��PDU��ʽ���ŵ�DCS��λ�����ж�
---------------------------------------------------------------------------*/
//****************************����ͷ�ļ�**************************//
#include "string.h"
#include "../include/Interface.h"
#include "../Util/Util.h"
#include "../AppMain.h"
#include "../OH/CMCC_1_Ext.h"
#include "../OH/YKPP.h"
#include "../MM/MemMgmt.h"
#include "../periph/usart/Usart.h"
#include "../periph/flash/Flash.h"
#include "../periph/pio/Pio.h"
#include "../periph/rtc/Rtc.h"
#include "../Log/Log.h"
#include "./CH.h"

//*****************************�궨��*****************************//
//CH����ʱʱ�䵥λms
#define CH_DELAY_TIME   20

//CH�����ʱ��ʱ�䣬��λms
#define TIME_WAIT_SMSSEND       8000//�ȴ�����Ϣ����ʱ��
#define TIME_WAIT_ATCMD         2000//�ȴ�AT�����Ӧ��ʱ��
#define TIME_WAIT_ATCONGPRS     6000//����GPRS��AT����Ļ�Ӧʱ�䣬��3��AT����
#define TIME_WAIT_ATDISCONGPRS  6000//�Ͽ�GPRS��AT����Ļ�Ӧʱ�䣬��2��AT����
#define TIME_WAIT_ATACSDCONNECT 15000//����ATA��ȴ�CSD�����ϵ�ʱ��
//MCM-29_20061110_zhangjie_begin
#define TIME_GET_ZONEINFO       30000//��ѯС����Ϣ����ʱ��
//MCM-29_20061110_zhangjie_end
#define TIME_TIMEOUT_USARTREC   500//���ڽ��ճ�ʱʱ��
#define TIME_TIMEOUT_ASKSLAVE   500//��ѯ�ӻ���ʱʱ��
#define TIME_TIMEOUT_DEBUG      600000//����ģʽ��ʱʱ��
#define TIME_SLAVEREG_PROTECT   8000//�ӻ�ע�ᱣ��ʱ��
#define TIME_TIMEOUT_RESET      (24*3600*1000+11300)//����modem�͵�·������ʱ��,ȡ������Ϊ�˺���Щ�����Ե�����ܿ�


//PDU��ʱ���򳤶�
#define PDU_SCTS_LEN   7

//ƥ�����ݰ�״̬����
#define DATAPACK_STATE_END_2        6
#define DATAPACK_STATE_END_1        5
#define DATAPACK_STATE_START_2      4
#define DATAPACK_STATE_START_1      3
#define DATAPACK_STATE_START        2
#define DATAPACK_STATE_END          1
#define DATAPACK_STATE_NO           0

//����ƥ�����ݰ�������ָ������
typedef void (*DP_HNDL_FUNC)(void*); 

//AT�������ݰ�״̬����
#define DATAPACK_STATE_CMT              20
#define DATAPACK_STATE_CRING            21
#define DATAPACK_STATE_UNKNOW           22
#define DATAPACK_STATE_SOCKETCLOSE      23
#define DATAPACK_STATE_CSCA             24
#define DATAPACK_STATE_CCED             25

//CH���ƶ��г���
#define CH_NTH_CTRL_QUE_SIZE 5
#define CH_STH_CTRL_QUE_SIZE 5
#define CH_DEB_CTRL_QUE_SIZE 1

//����ƥ�����ݰ���С
#define DATAPACK_CMCC_APAC_SIZE USART_SIZE_BUFFER
#define DATAPACK_CMCC_APB_SIZE  USART_SIZE_BUFFER
#define DATAPACK_ATCMD_SIZE     USART_SIZE_BUFFER
#define DATAPACK_PRIVATE_SIZE   USART_SIZE_BUFFER
#define DATAPACK_MS_SIZE        USART_SIZE_BUFFER
//AT��������һ�����ݴ�С,�����ݴ�IP��ַ��С����Ϣ���������ĺ���
#define DATAPACK_ATFULL_DATA1_SIZE 50

#define DATAPACK_MS_MINSIZE 10//����β�ַ�

//CH��modem��������ʱ������
//AtRtnIsOk()ʹ��
#define CH_SENDAT_NORMAL       1//��ͨAT����
#define CH_SENDAT_GPRSCONN     2//����GPRS����
#define CH_SENDAT_GPRSOTCP     3//׼������GPRS����
#define CH_SENDAT_SOCKETCLOSE  4//�Ͽ�SOCKET����
#define CH_SENDAT_CSCA         5//��ѯ����Ϣ
#define CH_SENDAT_CCED         6//��ѯС����Ϣ
#define CH_SENDAT_ATA          7//CSDժ��

//CH�������Ϣԭ������
//ĿǰCH��ר�ã�����ú�����ԭ�����Ͳ�ͬ
#define CH_MG_AT_TIMEOUT            0x31
#define CH_MG_AT_ERROR              0x32
#define CH_MG_AT_OK                 0x33
//test����3����ʱ����
#define CH_MG_AT_TIMEOUT_SMS        0x34
#define CH_MG_AT_ERROR_SMS          0x35
#define CH_MG_AT_OK_SMS             0x36

#define CH_MG_AT_GPRS_ACT           0x37
#define CH_MG_AT_GPRS_WAITDATA      0x38
#define CH_MG_AT_GPRS_SOCKETCLOSE   0x39

#define CH_MG_AT_CSCA               0x3A
#define CH_MG_AT_CCED               0x3B

#define CH_MG_AT_CSD_CON            0x3C
#define CH_MG_AT_ATA                0x3D
#define CH_MG_AT_ATH                0x3E
#define CH_MG_AT_CONNECT            0x3F

#define CH_MG_GET_ZONEINFO          0x40
#define CH_MG_RESET                 0x41

//�����Ǻ�����Э���йص���Ϣԭ������
#define CH_MG_SM_ANS                0x50//���豸����Ӧ�𣬴ӵ���
#define CH_MG_SM_REG                0x51//����ע�ᣬ�ӵ���
#define CH_MG_SM_UNREGCONF          0x52//ȷ������ɾ�����ӵ���
#define CH_MG_MS_ASK                0x53//���豸���߲�ѯ��������
#define CH_MG_MS_REGANS             0x54//����ע���Ӧ��������
#define CH_MG_MS_UNREGANS           0x55//����ɾ����Ӧ��������

//GPRS����ת���ַ�
#define GPRS_ESCAPE_CHAR  0x10

//����Э������ת���ַ�
#define MS_ESCAPE_CHAR  0x6E

//������ʼ������־ 
#define MS_START_END_FLAG        0xAA    
//�������ڹ㲥���豸���
#define MS_BROADCAST_DEVNUM 0xFF
//���������µĴ��豸���
#define MS_NEWS_DEVNUM 0x00
//����Э�����ͱ�ʶ
#define MS_PROTOCOL_TYPE 0xF1

//����Э�����Ԫ����
#define MS_CMD_ASK                  0x00//���豸���߲�ѯ��������,��ʱ�����ݵ�Ԫ
#define SM_CMD_ANS                  0x40//���豸����Ӧ�𣬴ӵ���,��ʱ�����ݵ�Ԫ
#define MS_CMD_QUERY                0x01//��ѯ��������
#define SM_CMD_QUERYANS             0x41//��ѯ��Ӧ�𣬴ӵ���
#define MS_CMD_SET                  0x02//���ã�������
#define SM_CMD_SETANS               0x42//���õ�Ӧ�𣬴ӵ���
#define MS_CMD_TAKEOHTER            0x03//Э��ĳ��أ�������
#define SM_CMD_TAKEOHTER            0x43//Э��ĳ��أ��ӵ���
#define MS_CMD_ONLYONEANS           0x04//��ռ���ӵ�Ӧ��������
#define SM_CMD_ONLYONE              0x44//��ռ���ӵ����룬�ӵ���
#define MS_CMD_CANCLEONLYONEANS     0x05//ȡ����ռ��Ӧ��������
#define SM_CMD_CANCLEONLYONE        0x45//ȡ����ռ�����룬�ӵ���
#define MS_CMD_REGANS               0x06//����ע���Ӧ��������
#define SM_CMD_REG                  0x46//����ע�ᣬ�ӵ���
#define MS_CMD_UNREGANS             0x07//����ɾ����Ӧ��������
#define SM_CMD_UNREG                0x47//����ɾ�����ӵ���
#define SM_CMD_UNREGCONF            0x48//ȷ������ɾ�����ӵ���
//#define MS_CMD_TAKERFINFO           0x0A//��Ƶ��Ϣ��ת����������
//#define SM_CMD_TAKERFINFO           0x4A//��Ƶ��Ϣ��ת�����ӵ���


//����Э��ջ��δʹ���ֽڶ���
typedef struct
{
    UCHAR ucProtocolType;//Э������
    UCHAR ucSNum;//�ӻ����
    UCHAR ucCmdUnit;//���Ԫ
    UCHAR ucFlagUnit;//��־��Ԫ    
    UCHAR *pucDataUnit;//���ݵ�Ԫָ��
    ULONG ulDataUnitSize;//���ݵ�Ԫ���� 
}MS_PROTOCOL_ST;

//��Ч������ΪpucDataPack[0]=< ... <pucDataPack[uiInPos]
typedef struct
{
    UCHAR ucState;//���ݰ�״̬  
    //test�����ĸ�������ʱ����
    UCHAR *pucHead;//ͷ�ַ���ָ��
    UCHAR *pucEnd;//β�ַ���ָ��
    UCHAR ucHeadSize;//ͷ�ַ�������
    UCHAR ucEndSize;//β�ַ�������
    
    UCHAR *pucData;//���ݰ�ָ��
    UCHAR *pucInPos;    
    ULONG ulDataPackMaxSize;//���ݰ���󳤶�
 
}DATA_PACK_ST;

//���ڴ��������AT���ݣ���Ϊ���ܺ�������0D0A�����ݰ�
//Ŀǰ֧������0D0A
typedef struct
{
    UCHAR ucState;//���ݰ�״̬      
    UCHAR *pucData1;//���ݰ�ָ��
    UCHAR *pucData2;//���ݰ�ָ��    
    ULONG ulData1Size;    
    ULONG ulData2Size;    
    ULONG ulDataPack1MaxSize;//���ݰ���󳤶�
    ULONG ulDataPack2MaxSize;//���ݰ���󳤶�     
}AT_DATA_PACK_ST;

//��ʱ���ṹ,ͨ��,Ŀǰֻ��CH��ʹ��
typedef struct
{
    Timer *pTimer;              //��ʱ������
    ULONG ulTimerMagicNum;      //��ʱ����magic number 
    TX_TIMER_FUNC pvFunc;       //�ص�����
}GENERAL_TIMER;

//CH�������Ϣ���нṹ��Ŀǰֻ�ṩ��CH�Լ�ʹ��
typedef struct
{
    ULONG ulPrmvType;   //����
    UCHAR ucParam1;     //����1
}CH_CTR_ST;

typedef struct
{
    CHAR  acSCTelNum[MAX_TEL_NUM_LEN + 1]; //����Ϣ���ĵ绰��������ֵ0��β
    UCHAR ucPID;
    UCHAR ucDCS;        //����Ƕ���Ϣ��������Ҫ���뷽ʽ������0��GSM7Bit��8��UCS2
    UCHAR ucDA;         //�����ַ����
    CHAR acSCTS[PDU_SCTS_LEN];
    CHAR  acTelNum[MAX_TEL_NUM_LEN + 1]; //�绰��������ֵ0��β
    ULONG ulDataLen;    //�û����ݳ���
    UCHAR *pucData;     //�û�����ָ��
}SMS_PARAM_ST;

//**************************ȫ�ֱ�������**************************//
//����Э���к�������ͨ����ز���
extern YKPP_PARAM_SET_ST g_stYkppParamSet;

//�豸��Ϣ
extern DEV_INFO_SET_ST g_stDevInfoSet;

//ʵʱ�ɼ���
extern RC_PARAM_SET_ST g_stRcParamSet;

extern CENTER_CONN_STATUS_ST g_stCenterConnStatus;

//MCM-11_20061101_zhangjie_begin
extern DEV_TYPE_TABLE_ST g_stDevTypeTable;
//MCM-11_20061101_zhangjie_end

//**************************ȫ�ֱ�������**************************//
//test
ULONG g_ulTest = 0;

UCHAR g_aucSMCAddr[DATAPACK_ATFULL_DATA1_SIZE];//��Ų�ѯ�������ĺ�����յ�������
UCHAR g_aucZoneInfo[DATAPACK_ATFULL_DATA1_SIZE];//��Ų�ѯС����Ϣ���յ�������
UCHAR g_aucCLIPTel[DATAPACK_ATFULL_DATA1_SIZE];//���CSD�������
UCHAR g_aucIPAddr[DATAPACK_ATFULL_DATA1_SIZE];//���GPRS����󱾻���IP��ַ

UCHAR g_ucCurrCommMode; //��ǰ��ͨ��״̬����ҪCH����ʼ��

ULONG g_ulACCHSendMessageNum = 0;//AC����CH������Ϣ�ĸ���

#define SIM_STA_NORMAL   0//sim�������κζ���Ϣ
#define SIM_STA_UNREAD   1//sim������δ������Ϣ
#define SIM_STA_READED   2//sim������ȫ���Ѷ�����Ϣ
UCHAR g_ucCurrSimSMSSta = SIM_STA_NORMAL; //��ǰ��SIM�������Ϣ״̬

DATA_PACK_ST g_stDataPackCMCCAPAC;
DATA_PACK_ST g_stDataPackCMCCAPB;
DATA_PACK_ST g_stDataPackAT;
DATA_PACK_ST g_stDataPackPrivate;
DATA_PACK_ST g_stDataPackNthMS;
DATA_PACK_ST g_stDataPackSthMS;

DATA_PACK_ST g_stDataPackDebAPAC;
DATA_PACK_ST g_stDataPackDebPrivate;

AT_DATA_PACK_ST g_stDataPackATFull;

//GPRS��ת���ַ��б�
UCHAR g_aucGPRSEscaped[] =
{
   0x03,
   0x10,
};

//GPRSδת���ַ�(����Ҫת��)�б�
UCHAR g_aucGPRSUnEscape[] =
{
   0x03,
   0x10,
};

//����Э����ת���ַ��б�
UCHAR g_aucMSEscaped[] =
{
   0x5D,
   0x7D,
};

//����Э��δת���ַ�(����Ҫת��)�б�
UCHAR g_aucMSUnEscape[] =
{
   0x6E,
   MS_START_END_FLAG,
};

//����ӿڽ������ݳ�ʱ��ʱ��
void NthRecTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerNthRecTimeOut =
{
    NULL,
    NULL,
    NthRecTimeOutCallBack
};

//����ӿڽ������ݳ�ʱ��ʱ��
void SthRecTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerSthRecTimeOut =
{
    NULL,
    NULL,
    SthRecTimeOutCallBack
};

//���Խӿڽ������ݳ�ʱ��ʱ��
void DebRecTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerDebRecTimeOut =
{
    NULL,
    NULL,
    DebRecTimeOutCallBack
};

//����ӿ���ѯ�ӻ���ʱ��ʱ��
void SthAskTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerSthAskTimeOut =
{
    NULL,
    NULL,
    SthAskTimeOutCallBack
};

//����AT����ȴ���Ӧ��ʱ��ʱ��
void ATRtnTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerATRtnTimeOut =
{
    NULL,
    NULL,
    ATRtnTimeOutCallBack
};

//��ѯС����Ϣ��ʱ��
void ZoneInfoTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerZoneInfo =
{
    NULL,
    NULL,
    ZoneInfoTimeOutCallBack
};

//����ģʽ��ʱ��ʱ��
void DebugTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerDebugTimeOut =
{
    NULL,
    NULL,
    DebugTimeOutCallBack
};

//�ӻ�ע�ᱣ����ʱ��
void SlaveRegProtectTimeCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerSlaveRegProtect =
{
    NULL,
    NULL,
    SlaveRegProtectTimeCallBack
};

//modem����ذ�������ʱ��
void ResetTimeCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerReset =
{
    NULL,
    NULL,
    ResetTimeCallBack
};

COMM_OBJ_ST g_stNorthInterface =
{
    COMM_OBJ_TYPE_USART,
    USART_MODEM_OR_DIRECT,
    COMM_STATE_SEND_IDLE,
    &g_stUsart0
};

COMM_OBJ_ST g_stSouthInterface=
{
    COMM_OBJ_TYPE_USART,
    USART_SLAVE_OPT,
    COMM_STATE_SEND_IDLE,    
    &g_stUsart2
};

COMM_OBJ_ST g_stDebugInterface=
{
    COMM_OBJ_TYPE_USART,
    USART_MODEM_OR_DIRECT,
    COMM_STATE_SEND_IDLE,    
    &g_stUsart1
};

//ͨ�������ڲ�ʹ�õĶ���,���ڴ����ͨ���������յ���Ϣ
OS_EVENT *g_pstCHNthCtrlQue;//����ӿڿ��ƶ���
OS_EVENT *g_pstCHSthCtrlQue;//����ӿڿ��ƶ���
OS_EVENT *g_pstCHDebCtrlQue;//���Խӿڿ��ƶ���
void *g_apvCHNthCtrlQue[CH_NTH_CTRL_QUE_SIZE];
void *g_apvCHSthCtrlQue[CH_STH_CTRL_QUE_SIZE];
void *g_apvCHDebCtrlQue[CH_DEB_CTRL_QUE_SIZE];

//*********** �����͵�AT����� ***********
UCHAR g_aucSAT_ATANDF[] = "AT&F\r";     //�ظ���������
UCHAR g_aucSAT_ATOFFLINE[] = "+++";     //�л���OFFLINEģʽ
UCHAR g_aucSAT_ATATO[] = "ATO\n\r";     //�л���DATAģʽ
UCHAR g_aucSAT_CMGF0[] = "AT+CMGF=0\r";     //����ΪPDUģʽ
UCHAR g_aucSAT_CNMI0[] = "AT+CNMI=2,0,0,0,0\r";     //�¶���Ϣ��֪ͨ
UCHAR g_aucSAT_CNMI1[] = "AT+CNMI=2,1,0,0,0\r";     //�¶���Ϣ֪ͨ
UCHAR g_aucSAT_CNMI2[] = "AT+CNMI=2,2,0,0,0\r";     //�¶���Ϣֱ�ӷ���
UCHAR g_aucSAT_ATE0[] = "ATE0\r";//ȥ������
UCHAR g_aucSAT_GPRSCGATT[] = "AT+CGATT=1\r";//ע������
UCHAR g_aucSAT_GPRSCONN[] = "AT#CONNECTIONSTART\r";//��������
UCHAR g_aucSAT_GPRSIPSET[] = "AT#TCPSERV=";//��������IP
UCHAR g_aucSAT_GPRSPORTSET[] = "AT#TCPPORT=";//���ö˿ں�
UCHAR g_aucSAT_GPRSOTCP[] = "AT#OTCP\r";//������׼���շ�����
UCHAR g_aucSAT_GPRSSOCKETCLOSE[] = "\x3";//�Ͽ�socket����
UCHAR g_aucSAT_GPRSDISCONN[] = "AT#CONNECTIONSTOP\r";//�Ͽ�����
UCHAR g_aucSAT_ATA[] = "ATA\r";//CSDժ��
UCHAR g_aucSAT_ATH[] = "ATH\r";//CSD�һ�
UCHAR g_aucSAT_CMGS[] = "AT+CMGS=";//���Ͷ���Ϣ
UCHAR g_aucSAT_CR[] = "\r";//�����ַ�
UCHAR g_aucSAT_CSCAQ[] = "AT+CSCA?\r";//��ѯ�������ĺ���
UCHAR g_aucSAT_CSCA[] = "AT+CSCA=";//���ö������ĺ���
UCHAR g_aucSAT_CSMP[] = "AT+CSMP=1,167,0,0\r";//test
UCHAR g_aucSAT_CLIP[] = "AT+CLIP=1\r";//������ʾ
UCHAR g_aucSAT_CRC[] = "AT+CRC=1\r";//��ʾ��������
UCHAR g_aucSAT_ATANDW[] = "AT&W\r";//��������
UCHAR g_aucSAT_CSAS[] = "AT+CSAS\r";//��������
UCHAR g_aucSAT_CCED[] = "AT+CCED=0,1\r";//��ѯС����Ϣ
UCHAR g_aucSAT_CMGL[] = "AT+CMGL=4\r";//�������ж���
UCHAR g_aucSAT_CMGD[] = "AT+CMGD=1,3\r";//ɾ�����ж���

//********* ���ܽ��յ���AT����ͷ *********
UCHAR g_aucRAT_OK[] = "OK";
UCHAR g_aucRAT_ERROR[] = "ERROR";
UCHAR g_aucRAT_CMT[] = "+CMT:";
UCHAR g_aucRAT_CMGL[] = "+CMGL:";
UCHAR g_aucRAT_CMTI[] = "+CMTI:";
UCHAR g_aucRAT_CMGS[] = "+CMGS:";
UCHAR g_aucRAT_CRING[] = "+CRING:";
UCHAR g_aucRAT_CSCA[] = "+CSCA:";
UCHAR g_aucRAT_CCED[] = "+CCED:";
UCHAR g_aucRAT_CLIP[] = "+CLIP:";
UCHAR g_aucRAT_GPRSAct[] = "Ok_Info_GprsActivation";
UCHAR g_aucRAT_GPRSActed[] = "#CME ERROR: 35840";//GPRS�ѱ�����
UCHAR g_aucRAT_TCPClosed[] = "#CME ERROR: 37966";//�Է��Ͽ�TCP����
UCHAR g_aucRAT_GPRSWaitData[] = "Ok_Info_WaitingForData";
UCHAR g_aucRAT_GPRSSocketClose[] = "Ok_Info_SocketClosed";
UCHAR g_aucRAT_NOCARRIER[] = "NO CARRIER";
UCHAR g_aucRAT_CONNECT[] = "CONNECT";

//****************************��������****************************//
extern ULONG Merge2Bytes(UCHAR* pucData, ULONG ulDataLen);
extern ULONG SplitInto2Bytes(UCHAR *pucData, ULONG ulDataLen);
extern void SaveYkppParam();
extern "C" void Restart(void);

LONG ConnectGPRSandCenter(COMM_OBJ_ST *pstCOMMObj,UCHAR aucIpAddr[],USHORT usPortNum);
LONG DisConnGPRS(COMM_OBJ_ST *pstCOMMObj);
LONG AtRtnIsOk(COMM_OBJ_ST *pstCOMMObj,ULONG ulWaitTime,ULONG ulATType);
void MSChangeBit(USHORT *pusBitMap,UCHAR ucNum, UCHAR ucTrue);
ULONG EncodeMSProtocol(UCHAR ucSNum,UCHAR ucCMD,UCHAR ucFlag,UCHAR *pucData, ULONG ulSize);
void SthSendtoOPTor485(COMM_OBJ_ST *pstCOMMObj, UCHAR *pucData, ULONG ulSize);
LONG GetSMCAddr(COMM_OBJ_ST *pstCOMMObj,CHAR *pcTelNum);
LONG SetSMCAddr(COMM_OBJ_ST *pstCOMMObj,CHAR *pcTelNum);
void SendStrToUsart(COMM_OBJ_ST *pstCOMMObj,UCHAR pucData[],ULONG ulSize);
LONG GetZoneInfo(void);
LONG NthSendtoMorD(COMM_OBJ_ST *pstCOMMObj, void * pvPrmv);
void CHSthReadyAndSend(void);
void SetDA(CHAR * pcTelNum, UCHAR aucPDUData [ ], ULONG & ulCurrIndex,UCHAR ucDA);

/*********************************************************
  ��������StrToULONG()

  ˵���� ���ַ���ת��ΪULONG
  ���ߣ�
 *************************************************/
ULONG StrToULONG(UCHAR* pucStr, ULONG ulStrLen)
{
    ULONG i,j=1,ulTemp=0,ulRet=0;
    if(ulStrLen == 0)
    {
        return 0;
    }

    for(i=ulStrLen;i>0;i--)
    {
        ulTemp = pucStr[i-1] - 0x30;
        if(ulTemp > 9)
        {
            return 0;
        }
        ulRet = ulTemp *j + ulRet;
        j = j *10;
    }
    return ulRet;
}


/*********************************************************
  ��������ULONGToStr
  Input:       pucStr :ת�����ַ�����ŵĵ�ַ  
                  ulValue: ��Ҫת������ֵ
  Return:     ת�����ַ����ĳ���
  ˵���� ��ULONGת��Ϊ�ַ������ɵ����߱�֤�ַ����ڴ��С
  ���ߣ�
 *************************************************/
ULONG ULONGToStr(UCHAR* pucStr, ULONG ulValue)
{
    ULONG i,j=10,ulTemp=0,ulStrLen;
    for(ulStrLen=1;ulValue/j>0;ulStrLen++)
    {
        j = j*10;
    }
    j=j/10;
    for(i=0;i<ulStrLen;i++)
    {
        ulTemp = ulValue / j;
        pucStr[i] = ulTemp + 0x30;
        ulValue = ulValue - ulTemp*j;
        j = j/10;
    }    

    return ulStrLen;
}


/*************************************************
  Function:GetTelNumFromPDU
  Description:    ���������ֽںϲ����PUD��������ȡ�绰����
                        ���ڵ绰�����ĩβ���00
  Calls:
  Called By:
  Input:          pucPDU :��ŵ绰�����PDU����ָ��   
                  ulPDULen: ��ŵ绰�����PDU����
                  pcTelNum: ��ŵ绰�����ַ���ָ��
  Output:         
  Return:         
  Others:         
*************************************************/
void GetTelNumFromPDU(UCHAR* pucPDU, ULONG ulPDULen, CHAR *pcTelNum)
{
    UCHAR ucTemp;
    ULONG i;
    for(i=0;i<ulPDULen;i++)
    {
        ucTemp = pucPDU[i];
        //��ȡ���ֽ�
        pcTelNum[2*i] = (ucTemp & 0x0F) + 0x30;        
        //��ȡ���ֽ�
        pcTelNum[2*i+1] = ((ucTemp & 0xF0) >> 4) + 0x30;
    }
    //�ж�ĩλ�Ƿ���F,����ĩλ���00
    i = 2*ulPDULen - 1;
    if(pcTelNum[i] > 0x39)
    {
        pcTelNum[i] = 0;
    }
    else
    {
        pcTelNum[i+1] = 0;
    }    
}
/*************************************************
  Function:WriteTelNumToPDU
  Description:    �������ѵ绰����д��PDU���ݰ�
                        ����PDU��ĩβ����F
  Calls:
  Called By:
  Input:       pucPDU :��ŵ绰�����PDU����ָ��   
                  ulTelNumLen: �绰������ַ�������
                  pcTelNum: ��ŵ绰�����ַ���ָ��
  Output:         
  Return:   ��ŵ绰�����PDU����         
  Others:         
*************************************************/
ULONG WriteTelNumToPDU(UCHAR* pucPDU, ULONG ulTelNumLen, CHAR *pcTelNum)
{
    ULONG i,ulPDULen;
    //ת��ΪPDU����
    ulPDULen = ulTelNumLen/2 + ulTelNumLen %2;
    for(i=0;i<ulPDULen;i++)
    {
        //������ڸ��ֽڣ�ǰ����ڵ��ֽ�
        pucPDU[i] = (pcTelNum[2*i] -0x30) + ((pcTelNum[2*i+1] - 0x30) <<4);
    }
    //�ж�ĩλ�Ƿ�Ҫ��дF
    i = ulPDULen - 1;
    if(ulTelNumLen %2 !=0)
    {
        pucPDU[i] = pucPDU[i] |0xF0;
    }
    return ulPDULen;
}
/*************************************************
  Function:DecodeGSM7BitData
  Description:    ��������7bit����
  Called By:
  Input:       pucData :�����������ָ��   
                  ulDataLen: ���ݳ��ȣ�ָseptet(7bit�ַ��ĸ���)
                  pcTelNum: ����������ָ��
  Output:         
  Return:         
  Others:         
*************************************************/
void DecodeGSM7BitData(UCHAR *pucData, ULONG ulDataLen, UCHAR *pucDecodeData)
{
    ULONG ulMoveToLeftBit = 0,i,j=0;
    USHORT usTmp = 0;
    for(i = 0; i < ulDataLen; i++)
    {
        ulMoveToLeftBit = i % 8;
        if(ulMoveToLeftBit == 0)
        {
            pucDecodeData[i] = pucData[j] & 0x7F; //û�п�Խ2���ֽ�
        }
        else if(ulMoveToLeftBit == 7)
        {
            pucDecodeData[i] = (pucData[j] >> 1) & 0x7F; //û�п�Խ2���ֽ�
            j++;
        }
        else
        {
            usTmp = (USHORT)(pucData[j + 1]);
            usTmp = (usTmp << 8); //��λ����
            usTmp |= pucData[j];          //��λ����
            usTmp <<= ulMoveToLeftBit;
            pucDecodeData[i] = (UCHAR)((usTmp >> 8) & 0x7F);
            j++;
        }
    }
}

/*********************************************************
  ��������DecodeAtCmtInPDUMode()
  ���룺  pucCMT:    ���յ���+CMT�����ַ�������"+CMT"��ʼ����һ��
                    <CR><LF>����
                   ulCMTLen:  ���յ������ݳ���
                    pucPDU:    ���յ���PDU�����ַ���������<CR><LF>֮��
                   ulPDULen:  ���յ���PDU���ݳ���
  �����  pstSMS:  ����Ϣ�ĸ�����Ϣ

  ����ֵ��SUCCEEDED�ɹ�FAILDʧ��
  ˵���� 
  ���ߣ�
 *************************************************/
LONG DecodeAtCmtInPDUMode(UCHAR *pucCMT, ULONG ulCMTLen, 
            UCHAR *pucPDU, ULONG ulPDULen, SMS_PARAM_ST *pstSMS)
{
    ULONG ulStartPos, ulTPDULen, ulSCALen, ulOALen, ulUDLen, ulCurrIndex=0;

    //ȡ��TPDU����,֧��cmgl,cmgl�ǵ��������ź�ȡ����
    ulStartPos = FindChar(pucCMT, ',', 3, ulCMTLen);
    if(ulStartPos >= ulCMTLen - 1)
    {
        ulStartPos = FindChar(pucCMT, ',', 1, ulCMTLen);
        if(ulStartPos >= ulCMTLen - 1)
        {
            return FAILED;
        }
    }

    ulTPDULen = StrToULONG(pucCMT + ulStartPos + 1,ulCMTLen - ulStartPos - 1);

    //�ڴ治���ƣ���ΪҪ����7bit����
    //�ڴ������SMS�ṹ����ָ�ڴ�
    //memcpy(pstSMS->pucData,pucPDU,ulPDULen);
    //pucPDU = pstSMS->pucData;
    
    //�ֽںϲ�
    ulPDULen = Merge2Bytes(pucPDU, ulPDULen);

    //�жϳ��ȺϷ���  
    //ulSCALen����SCA��Length,���Ǻ�TypeOfAddr
    ulSCALen = (ULONG)(pucPDU[ulCurrIndex]);
    if(ulPDULen != ulSCALen + 1 + ulTPDULen)
    {
        return FAILED;
    }    
    //����SCA��Length��TypeOfAddr
    ulCurrIndex = ulCurrIndex + 2;

    //���SCA
    if(ulSCALen != 0)
    {
        GetTelNumFromPDU(pucPDU+ulCurrIndex, ulSCALen-1, pstSMS->acSCTelNum);
        ulCurrIndex = ulCurrIndex + ulSCALen - 1;
    }
    else
    {
        ulCurrIndex++;
    }

    //�ж�MTI,(RP,UDHI,SRI,MMS,MTI),��������
    //0��ʾSMS-DELIVER�������ʾSMS-SUBMIT
    if(pucPDU[ulCurrIndex] & 0x03 != 0)
    {
        return FAILED;
    }
    ulCurrIndex++;

    //���OA����绰����
    //�ṹͬSCA������Length���Ժ�������Ϊ��λ
    ulOALen = (ULONG)(pucPDU[ulCurrIndex]);
    //ת������
    if(ulOALen%2 != 0)
    {
        ulOALen++;
    }    
    ulOALen = ulOALen/2;
    //����OA��Length
    ulCurrIndex++;
    
    //��ȡTypeOfAddr
    pstSMS->ucDA = pucPDU[ulCurrIndex];
    ulCurrIndex++;
    
    GetTelNumFromPDU(pucPDU+ulCurrIndex, ulOALen, pstSMS->acTelNum);
    ulCurrIndex = ulCurrIndex + ulOALen;

    //���PID
    pstSMS->ucPID = pucPDU[ulCurrIndex];
    ulCurrIndex++;

    //���DCS
    pstSMS->ucDCS= pucPDU[ulCurrIndex];
    ulCurrIndex++;    

    //���SCTS
    memcpy(pstSMS->acSCTS, pucPDU+ulCurrIndex, PDU_SCTS_LEN);
    ulCurrIndex = ulCurrIndex + PDU_SCTS_LEN;    

    //���UDL��UD��д�����Ϣ����
    ulUDLen = pucPDU[ulCurrIndex];
    //��������
    ulCurrIndex++;
    //�ж�PDU����
    if((ulCurrIndex + (ulUDLen*7 + (8-1))/8)!= ulPDULen)
    {
        return FAILED;
    }

    //MCM-76_20061110_linyu_begin
    if((pstSMS->ucDCS & 0xC0) == 0) //�����λbit 7..6Ϊ0
    {   
        //�ж�bit 3..2
        switch(((pstSMS->ucDCS) >> 2) & 3)
        {
        case 0: //GSM 7 bit
            DecodeGSM7BitData(pucPDU+ulCurrIndex, ulUDLen, pstSMS->pucData);
            break;
        case 1: //8 bit data
        case 2: //UCS2
            memcpy(pstSMS->pucData, pucPDU+ulCurrIndex, ulUDLen);
            break;
        default:
            return FAILED;
        }
    }
    else if((pstSMS->ucDCS & 0xF0) == 0xF0) //���4λȫΪ1
    {
        //�ж�bit 2
        if(((pstSMS->ucDCS) >> 2) & 1) //8 bit data
        {
            memcpy(pstSMS->pucData, pucPDU+ulCurrIndex, ulUDLen);
        }
        else //GSM 7 bit
        {
            DecodeGSM7BitData(pucPDU+ulCurrIndex, ulUDLen, pstSMS->pucData);
        }
    }
    else
    {
        return FAILED;
    }
    //MCM-76_20061110_linyu_end
     
    //���볤��
    pstSMS->ulDataLen = ulUDLen;

    return SUCCEEDED;
    
}


/*************************************************
  Function:EncodeGSM7BitData
  Description:    ��������7bit����
  Called By:
  Input:       pucData :�����������ָ��   
                  ulDataLen: ���ݳ���
                  pcTelNum: ����������ָ��
  Output:         
  Return:     ���������ݳ���         
  Others:     �ɵ��÷���ָ֤��ĺϷ���   
*************************************************/
ULONG EncodeGSM7BitData(UCHAR *pucData, ULONG ulDataLen, UCHAR *pucEncodeData)
{
    ULONG ulEncodeLen;
    //��������ĳ���
    ulEncodeLen = (ulDataLen * 7 + (8-1))/8;
    memset(pucEncodeData, 0, ulEncodeLen);
    
    UCHAR ucHighByte = 0, ucLowByte = 0;
    USHORT usTmp = 0;
    ULONG ulMoveToRightBit = 0;
    ULONG j = 0; //��ʾ�ѱ�����ַ�ռ�õ��ֽ���
    for(ULONG i = 0; i < ulDataLen; i++)
    {
        ulMoveToRightBit = i % 8;
        if(ulMoveToRightBit == 0)
        {
            pucEncodeData[j] = pucData[i] & 0x7F; //û�п�Խ2���ֽ�
        }
        else if(ulMoveToRightBit == 7)
        {
            pucEncodeData[j] |= (pucData[i] & 0x7F) << 1; //û�п�Խ2���ֽ�
            j++;
        }
        else
        {
            usTmp = (USHORT)(pucData[i] & 0x7F);
            usTmp = (usTmp << 8) & 0xFF00;          //b0 a7 a6 a5 a4 a3 a2 a1 a0
            ucHighByte = (UCHAR)(((usTmp >> ulMoveToRightBit) >> 8) & 0xFF);//c1 c0 b7 b6 b5 b4 b3 b2 b1
            ucLowByte  = (UCHAR)((usTmp >> ulMoveToRightBit) & 0xFF);       //         c7 c6 c5 c4 c3 c2
            pucEncodeData[j] |= ucLowByte;
            pucEncodeData[j + 1] = ucHighByte;
            j++;
        }
    }
    return ulEncodeLen;    
    
}

/*************************************************
  Function:SendSMSInPDUMode
  Description:    ��������ָ��ͨ�ſ���PDU��ʽ���Ͷ���Ϣ
  Calls:
  Called By:
  Input:       pstCOMMObj:ͨ�ſڶ���ָ��
                  pcTelNum :�Է��绰����   
                  pucData: ����������ָ��
                  ulSize: ���������ݳ���
                  ucDCS:���뷽ʽ08:UCS2 00:7bit
                  pcSCTelNum:�������ĺ��룬NULL��ʹ��
  Output:         
  Return:         
  Others:         
*************************************************/
LONG SendSMSInPDUMode(COMM_OBJ_ST *pstCOMMObj,CHAR *pcTelNum, UCHAR *pucData, 
        ULONG ulSize, UCHAR ucDCS, UCHAR ucDA, CHAR *pcSCTelNum)
{
    UCHAR aucPDUData[384];
    ULONG ulCurrIndex = 0, ulDALen, ulTPDULen, ulSCALen, ulPDULen;

    //��ֹ����Ϣ����
    if(ucDCS == 0)
    {
        if(ulSize > 160)
        {
            ulSize = 160;
        }
    }
    else
    {
        if(ulSize > 140)
        {
            ulSize = 140;
        }
    }

    //д��SCA
    //SCAΪ0����ʾʹ�ô洢��SIM���еĶ���Ϣ���ĺ���
    if(pcSCTelNum == NULL)
    {
        aucPDUData[ulCurrIndex] = 0;
        ulSCALen = 1;
        ulCurrIndex++;
    }
    //notice д��SCA����δд
    else
    {
    }
    
    //д��MTI��
    //VPF=01 ����relative format
    aucPDUData[ulCurrIndex] = 0x11;
    ulCurrIndex++;

    //д��MR
    aucPDUData[ulCurrIndex] = 0x00;
    ulCurrIndex++;

    //д��DA
    SetDA(pcTelNum, aucPDUData, ulCurrIndex,ucDA);

    //д��PID
    aucPDUData[ulCurrIndex] = 0x00;
    ulCurrIndex++;

    //д��DCS
    aucPDUData[ulCurrIndex] = ucDCS;
    ulCurrIndex++;

    //д��VP
    /*
    0 to 143:     (VP + 1) x 5 minutes (i.e. 5 minutes intervals up to 12 hours)
    144 to 167:  2 hours + ((VP-143) x 30 minutes)
    168 to 196:  (VP-166) x 1 day
    197 to 255:  (VP-192) x 1 week
    */
    aucPDUData[ulCurrIndex] = 173;//7days
    ulCurrIndex++;

    //д��UDL
    aucPDUData[ulCurrIndex] = ulSize;
    ulCurrIndex++;

    //MCM-76_20061110_linyu_begin
    //д��UD  
    if((ucDCS & 0xC0) == 0) //�����λbit 7..6Ϊ0
    {   
        //�ж�bit 3..2
        switch((ucDCS >> 2) & 3)
        {
        case 0: //GSM 7 bit
            ulSize = EncodeGSM7BitData(pucData, ulSize, aucPDUData+ulCurrIndex);
            break;
        case 1: //8 bit data
        case 2: //UCS2
            memcpy(aucPDUData+ulCurrIndex,pucData,ulSize);
            break;
        default:
            return FAILED;
        }
    }
    else if((ucDCS & 0xF0) == 0xF0) //���4λȫΪ1
    {
        //�ж�bit 2
        if((ucDCS >> 2) & 1) //8 bit data
        {
            memcpy(aucPDUData+ulCurrIndex,pucData,ulSize);
        }
        else //GSM 7 bit
        {
            ulSize = EncodeGSM7BitData(pucData, ulSize, aucPDUData+ulCurrIndex);
        }
    }
    else
    {
        return FAILED;
    }
    //MCM-76_20061110_linyu_end
    
    ulCurrIndex = ulCurrIndex + ulSize;

    //д��TPDU����(����SCA)
    ulTPDULen = ulCurrIndex - ulSCALen;

    //�ֽڲ��
    ulPDULen = SplitInto2Bytes(aucPDUData, ulCurrIndex);

    //�������򴮿�д����
    UINT32 cpu_sr;    
    OS_ENTER_CRITICAL();
    //д��CMGS
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            g_aucSAT_CMGS, sizeof(g_aucSAT_CMGS)-1);
    //д��TPDU����(����SCA)
    UCHAR pucTemp[4];
    ulTPDULen = ULONGToStr(pucTemp, ulTPDULen);
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            pucTemp, ulTPDULen);
    //д��0D
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            g_aucSAT_CR, sizeof(g_aucSAT_CR)-1);
    //д��PDU����
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            aucPDUData, ulPDULen);
    //д��1A    
    UCHAR ucTemp = 0x1A;
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            &ucTemp, 1);   
    OS_EXIT_CRITICAL();
    
    return SUCCEEDED;
}
/*************************************************
  Function:StartGeneralTimer
  Description:    ����������ͨ�õĶ�ʱ��
  Calls:
  Called By:
  Input:       pvCtx :���ص������Ĳ���   
                  ulInterval: ��ʱʱ��
                  pstGTimer: ͨ�ö�ʱ��ָ��
                  enType:��ʱ�����ͣ�Ŀǰֻ�������Ժͷ�����������
  Output:         
  Return:         
  Others:         
*************************************************/
LONG StartGeneralTimer(GENERAL_TIMER *pstGTimer,ULONG ulInterval,void *pvCtx, TIMER_TYPE_EN enType)
{
   
    if(pstGTimer->pTimer) //���ԭ���ж�ʱ����Ҫ��ֹͣ
    {
        g_TimerMngr.RemoveTimer(pstGTimer->pTimer, pstGTimer->ulTimerMagicNum); 

        pstGTimer->pTimer = NULL;
        pstGTimer->ulTimerMagicNum = NULL;
    }

    pstGTimer->pTimer = g_TimerMngr.CreateTimer(pstGTimer->pvFunc, 
            pvCtx, ulInterval, enType, &(pstGTimer->ulTimerMagicNum));
    if(pstGTimer->pTimer == NULL)
    {
        return FAILED;
    }
    else
    {
        return SUCCEEDED;
    }
}

/*************************************************
  Function:StopGeneralTimer
  Description:    ������ֹͣͨ�õĶ�ʱ��
  Calls:
  Called By:
  Input:  
                  pstGTimer: ͨ�ö�ʱ��ָ��
  Output:         
  Return:         
  Others:         
*************************************************/
LONG StopGeneralTimer(GENERAL_TIMER *pstGTimer)
{
   
    if(pstGTimer->pTimer) //���ԭ���ж�ʱ����Ҫ��ֹͣ
    {
        g_TimerMngr.RemoveTimer(pstGTimer->pTimer, pstGTimer->ulTimerMagicNum); 

        pstGTimer->pTimer = NULL;
        pstGTimer->ulTimerMagicNum = NULL;
    }

    return SUCCEEDED;

}

/****************************************
����ӿڽ��ճ�ʱ�ص�����
****************************************/
void NthRecTimeOutCallBack(void *pvCtx)
{
    g_stTimerNthRecTimeOut.pTimer = NULL;
    g_stTimerNthRecTimeOut.ulTimerMagicNum = NULL;

    //����Ҫֹͣ��ʱ������Ϊ�Ѿ���ʱ
    //StopGeneralTimer(&g_stTimerNthRecTimeOut);

    //ֻ�б���ӿڻ��յ�APAC��APB��AT����ͳ���Э������ݰ�
    g_stDataPackCMCCAPAC.ucState = DATAPACK_STATE_NO;
    g_stDataPackCMCCAPB.ucState = DATAPACK_STATE_NO;
    g_stDataPackAT.ucState = DATAPACK_STATE_NO;
    g_stDataPackATFull.ucState = DATAPACK_STATE_NO;
    g_stDataPackPrivate.ucState = DATAPACK_STATE_NO;
    //��������Э��
    g_stDataPackNthMS.ucState = DATAPACK_STATE_NO;
}

/****************************************
����ӿڽ��ճ�ʱ�ص�����
****************************************/
void SthRecTimeOutCallBack(void *pvCtx)
{
    g_stTimerSthRecTimeOut.pTimer = NULL;
    g_stTimerSthRecTimeOut.ulTimerMagicNum = NULL;

    //��������Э��
    g_stDataPackSthMS.ucState = DATAPACK_STATE_NO;
}

/****************************************
���Խӿڽ��ճ�ʱ�ص�����
****************************************/
void DebRecTimeOutCallBack(void *pvCtx)
{
    g_stTimerDebRecTimeOut.pTimer = NULL;
    g_stTimerDebRecTimeOut.ulTimerMagicNum = NULL;

    g_stDataPackDebAPAC.ucState = DATAPACK_STATE_NO;
    g_stDataPackDebPrivate.ucState = DATAPACK_STATE_NO;
}

/****************************************
����ӿ���ѯ�ӻ���ʱ�ص�����
****************************************/
void SthAskTimeOutCallBack(void *pvCtx)
{
    g_stTimerSthAskTimeOut.pTimer = NULL;
    g_stTimerSthAskTimeOut.ulTimerMagicNum = NULL;

    //������ͨ�Ÿ澯λ
    if(g_stYkppParamSet.stYkppCommParam.ucAskingSNum != MS_NEWS_DEVNUM)
    {
        MSChangeBit(&g_stYkppParamSet.stYkppCommParam.usCommAlarm, g_stYkppParamSet.stYkppCommParam.ucAskingSNum, 1);
        //δ�յ�ɾ��ȷ��ʱ��������澯����λͼ�����޴˴ӻ�
        g_stYkppParamSet.stYkppCommParam.usCommAlarm = g_stYkppParamSet.stYkppCommParam.usCommAlarm & g_stYkppParamSet.stYkppCommParam.usSBitmap;
    }

    //MCM-45_20061130_zhangjie_begin
    if(g_stSouthInterface.ucInterfaceType == USART_SLAVE_485)
    {
        g_stSouthInterface.ucState = COMM_STATE_SEND_IDLE;
    }
    //MCM-45_20061130_zhangjie_end
                
    //���Ͷ���һ���ӻ�����ѯ
    CH_CTR_ST stCHCtrMsg;
    stCHCtrMsg.ulPrmvType = CH_MG_MS_ASK;
    UCHAR *pucMem = (UCHAR *) MyMalloc(g_pstMemPool16);
    if(pucMem == (void*)NULL)
    {
        //�ȴ����Ź���������Ϊ������벻���ڴ�������᲻����ѯ
        while(1);
    }
    memcpy(pucMem,&stCHCtrMsg,sizeof(stCHCtrMsg));

    //������Ϣ����Ϣ�������������ݰ�
    if(OSQPost(g_pstCHSthCtrlQue, pucMem) == OS_Q_FULL)
    {
        //�ȴ����Ź���������Ϊ���������Ϣ���ɹ�������᲻����ѯ
        MyFree(pucMem);
        while(1);
    }
}

/****************************************
AT����س�ʱ�ص�������Ŀǰֻ���ڱ���
****************************************/
void ATRtnTimeOutCallBack(void *pvCtx)
{
    g_stTimerATRtnTimeOut.pTimer = NULL;
    g_stTimerATRtnTimeOut.ulTimerMagicNum = NULL;

    UCHAR *pucMem;
    CH_CTR_ST stCHCtrMsg;

    stCHCtrMsg.ulPrmvType = CH_MG_AT_TIMEOUT;

    //notice���ﲻ�������벻���ڴ�
    pucMem = (UCHAR *) MyMalloc(g_pstMemPool16);

    //notice:CH����һֱ���ͷ��Լ�������Ȩ�Ӷ���������
    if(pucMem == (void*)NULL)
    {
        while(1);
    }

    memcpy(pucMem,&stCHCtrMsg,sizeof(stCHCtrMsg));

    //notice���ﲻ������Ϣ������
    OSQPost(g_pstCHNthCtrlQue, pucMem);

}

/****************************************
��ѯС����Ϣ��ʱ�ص�������Ŀǰֻ���ڱ���
****************************************/
void ZoneInfoTimeOutCallBack(void *pvCtx)
{
    UCHAR *pucMem;
    CH_CTR_ST stCHCtrMsg;

    stCHCtrMsg.ulPrmvType = CH_MG_GET_ZONEINFO;

    pucMem = (UCHAR *) MyMalloc(g_pstMemPool16);
    
    //notice:���ڴ˶�ʱ���������Եģ��������벻���ڴ��ֱ�ӷ���
    if(pucMem == (void*)NULL)
    {
        return;
    }

    memcpy(pucMem,&stCHCtrMsg,sizeof(stCHCtrMsg));

    if(OSQPost(g_pstCHNthCtrlQue, pucMem) == OS_Q_FULL)
    {        
        MyFree(pucMem);
    }

    //test����GPRS���ӺͶϿ����������ӺͶϿ�
    /*
    OHCH_CONN_CENTER_REQ_ST stOHCHConnCenterMsg;
    if(g_ucCurrCommMode != COMM_MODE_GPRS)
    {
        stOHCHConnCenterMsg.ulPrmvType = OHCH_CONN_CENTER_REQ;
        stOHCHConnCenterMsg.aucIpAddr[0] = 218;
        stOHCHConnCenterMsg.aucIpAddr[1] = 5;
        stOHCHConnCenterMsg.aucIpAddr[2] = 5;
        stOHCHConnCenterMsg.aucIpAddr[3] = 40;
        stOHCHConnCenterMsg.usPortNum = 4667;
    }
    else
    {
        stOHCHConnCenterMsg.ulPrmvType = OHCH_GPRS_DISCONN_REQ;
    }

    //notice���ﲻ�������벻���ڴ�
    pucMem = (UCHAR *) MyMalloc(g_pstMemPool256);

    memcpy(pucMem,&stOHCHConnCenterMsg,sizeof(stOHCHConnCenterMsg));

    OSQPost(g_pstCHNthQue, pucMem);
    */
    //test����GPRS���ӺͶϿ����������ӺͶϿ�
/*
            
            if(g_ulTest == 0)
            {
                g_ulTest = 1;
                PioWrite(&LED_PIO_CTRL, 
                LED_MASK,       
                LED_ON);       
            }
            else
            {
                g_ulTest = 0;
                PioWrite(&LED_PIO_CTRL, 
                LED_MASK,       
                LED_OFF);       
            }
*/            
}

/****************************************
����ģʽ��ʱ�ص�����
****************************************/

void DebugTimeOutCallBack(void *pvCtx)
{
    g_stTimerDebugTimeOut.pTimer = NULL;
    g_stTimerDebugTimeOut.ulTimerMagicNum = NULL;
    
    g_stYkppParamSet.stYkppCtrlParam.ucDebugMode = FALSE;
    //notice���Կ��������Ϣ

}

/****************************************
�ӻ�ע�ᱣ����ʱ���ص�����
****************************************/
void SlaveRegProtectTimeCallBack(void *pvCtx)
{
    g_stTimerSlaveRegProtect.pTimer = NULL;
    g_stTimerSlaveRegProtect.ulTimerMagicNum = NULL;
    //���´��豸����ע��
    g_stYkppParamSet.stYkppCommParam.ucNewSNum = MS_NEWS_DEVNUM;
}

/****************************************
�����豸��ʱ�ص�������Ŀǰֻ���ڱ���
****************************************/
void ResetTimeCallBack(void * pvCtx)
{
    UCHAR *pucMem;
    CH_CTR_ST stCHCtrMsg;

    stCHCtrMsg.ulPrmvType = CH_MG_RESET;

    pucMem = (UCHAR *) MyMalloc(g_pstMemPool16);
    
    //notice:���ڴ˶�ʱ���������Եģ��������벻���ڴ��ֱ�ӷ���
    if(pucMem == (void*)NULL)
    {
        return;
    }

    memcpy(pucMem,&stCHCtrMsg,sizeof(stCHCtrMsg));

    if(OSQPost(g_pstCHNthCtrlQue, pucMem) == OS_Q_FULL)
    {        
        MyFree(pucMem);
    }
}

/*************************************************
  Function:     CHTaskInit
  Description:    ��������ʼ��CH����
  Calls:
  Called By:
  Input:          
  Output:     
  Return:         
  Others:
*************************************************/
void CHTaskInit(void)
{
    g_pstCHNthQue=OSQCreate(g_apvCHNthQue, CH_NTH_QUE_SIZE);
    g_pstCHSthQue=OSQCreate(g_apvCHSthQue, CH_STH_QUE_SIZE);
    g_pstCHDebQue=OSQCreate(g_apvCHDebQue, CH_DEB_QUE_SIZE);
    g_pstCHNthCtrlQue=OSQCreate(g_apvCHNthCtrlQue, CH_NTH_CTRL_QUE_SIZE);
    g_pstCHSthCtrlQue=OSQCreate(g_apvCHSthCtrlQue, CH_STH_CTRL_QUE_SIZE);
    
    //MCM-18_20061106_zhangjie_begin
    g_pstCHDebCtrlQue=OSQCreate(g_apvCHDebCtrlQue, CH_DEB_CTRL_QUE_SIZE);
    //MCM-18_20061106_zhangjie_end

    //��ǰͨ��״̬
    g_ucCurrCommMode = COMM_NO_CONN;
    //������Ϊ�ǵ���ģʽ������������·�澯,�����´��豸ע��
    g_stYkppParamSet.stYkppCtrlParam.ucDebugMode = FALSE;
    g_stYkppParamSet.stYkppCommParam.usCommAlarm= 0x0000;
    g_stYkppParamSet.stYkppCommParam.ucNewSNum = 0;
    
    //���ݻ��ͽ�������
    //�����豸�������ò����б���Ӧ���͵��������
    //MCM-11_20061101_zhangjie_begin

    if((g_stDevTypeTable.ucOpticalType != OPTICAL_NO) && (g_stDevTypeTable.ucCommRoleType== COMM_ROLE_MASTER))
    {
        g_stNorthInterface.pvCOMMDev = &g_stUsart0;//232(����ʱʹ�øô���)
        g_stSouthInterface.pvCOMMDev = &g_stUsart1;//232
        g_stDebugInterface.pvCOMMDev = &g_stUsart2;//485(��ʹ�õ��Դ���)
        g_stNorthInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
        g_stSouthInterface.ucInterfaceType = USART_SLAVE_OPT;
        g_stDebugInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
        //��˻�ʹ�ܴ�
        PioWrite(&OPT_PIO_CTRL,OPT_ENABLE,OPT_ENABLE_ON);
        //��˻����ó����
        PioOpen(&OPT_PIO_CTRL, OPT_ENABLE,PIO_OUTPUT);
    }
    else if((g_stDevTypeTable.ucOpticalType != OPTICAL_NO) && (g_stDevTypeTable.ucCommRoleType == COMM_ROLE_SLAVE))
    {
        g_stNorthInterface.pvCOMMDev = &g_stUsart1;//232
        g_stSouthInterface.pvCOMMDev = &g_stUsart2;//485
        g_stDebugInterface.pvCOMMDev = &g_stUsart0;//232(����ʱʹ�øô���)
        g_stNorthInterface.ucInterfaceType = USART_SLAVE_OPT;
        g_stSouthInterface.ucInterfaceType = USART_SLAVE_485;
        g_stDebugInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
        //��˻�ʹ�ܹر�
        PioWrite(&OPT_PIO_CTRL,OPT_ENABLE,OPT_ENABLE_OFF);
        //��˻����ó����
        PioOpen(&OPT_PIO_CTRL, OPT_ENABLE,PIO_OUTPUT);
    }
    else if((g_stDevTypeTable.ucOpticalType == OPTICAL_NO) && (g_stDevTypeTable.ucCommRoleType == COMM_ROLE_SLAVE))
    {
        g_stNorthInterface.pvCOMMDev = &g_stUsart2;//485
        g_stSouthInterface.pvCOMMDev = &g_stUsart1;//485
        g_stDebugInterface.pvCOMMDev = &g_stUsart0;//232(����ʱʹ�øô���)
        g_stNorthInterface.ucInterfaceType = USART_SLAVE_485;
        g_stSouthInterface.ucInterfaceType = USART_SLAVE_485;
        g_stDebugInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
    }
    else
    {
        g_stNorthInterface.pvCOMMDev = &g_stUsart0;//232(����ʱʹ�øô���)
        g_stSouthInterface.pvCOMMDev = &g_stUsart2;//485
        g_stDebugInterface.pvCOMMDev = &g_stUsart1;//232(�ݲ�ʹ�õ��Դ���)
        g_stNorthInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
        g_stSouthInterface.ucInterfaceType = USART_SLAVE_485;
        g_stDebugInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
    }   
    //MCM-11_20061101_zhangjie_end

    
    //��ֹ�ӻ���ŷǷ�
    if(g_stYkppParamSet.stYkppCommParam.ucSNum > 0x10)
    {
        g_stYkppParamSet.stYkppCommParam.ucSNum = 0;        
        SaveYkppParam();
    }
    
}

/*************************************************
  Function:       MSChangeBit
  Description:    �������ı�����Э���дӻ�λͼ�ʹӻ��澯״̬�ĸ�λ
                  ������ֻʹ��������ӿ�
  Calls:
  Called By:
  Input:          pusBitMap:��Ҫ�ı��USHORT����
                  ucNum:  Bit��λ�ã���λΪ1����λΪ16
                  ucTrue:1��1��0��0
  Output:     
  Return:         
  Others:
*************************************************/
void MSChangeBit(USHORT *pusBitMap,UCHAR ucNum, UCHAR ucTrue)
{
    USHORT usTemp;
    //ucNumȡֵ��1-16��źϷ�
    if((ucNum<=0) || (ucNum>16))
    {
        return;
    }
    if(ucTrue)
    {
        usTemp = 0x8000>>(ucNum-1);
        usTemp = (*pusBitMap) | usTemp;
    }
    else
    {
        usTemp = ~(0x8000>>(ucNum-1));
        usTemp = (*pusBitMap) & usTemp;
    }
    *pusBitMap = usTemp;
}

/*************************************************
  Function:       GetNextSNum
  Description:    �������ı�ȡ����һ��Ҫ��ѯ�Ĵӻ����
                  ͬʱ�Զ��ı�ucAskingSNum��ֵ
                  ������ֻʹ��������ӿ�
  Calls:
  Called By:
  Input:          
  Output:     
  Return:         ��һ��Ҫ��ѯ�Ĵӻ����
  Others:
*************************************************/
UCHAR GetNextSNum(void)
{
    USHORT usTemp;
    UCHAR i;
    if((g_stYkppParamSet.stYkppCommParam.ucAskingSNum>16)||(g_stYkppParamSet.stYkppCommParam.usSBitmap==0))
    {
        g_stYkppParamSet.stYkppCommParam.ucAskingSNum = MS_NEWS_DEVNUM;
        return MS_NEWS_DEVNUM;
    }
    usTemp = g_stYkppParamSet.stYkppCommParam.usSBitmap;
    i = g_stYkppParamSet.stYkppCommParam.ucAskingSNum;
    for(;i<16;i++)
    {
        if(((0x8000>>i) & usTemp) != 0)
        {
            g_stYkppParamSet.stYkppCommParam.ucAskingSNum = i+1;
            return (i+1);
        }
    }
    g_stYkppParamSet.stYkppCommParam.ucAskingSNum = MS_NEWS_DEVNUM;
    return MS_NEWS_DEVNUM;
}

/*************************************************
  Function:       GetNewSNum
  Description:    ������ȡ���µĴӻ����
                  ��������ı�ucNewSNum��ֵ
                  ������ֻʹ��������ӿ�
  Calls:
  Called By:
  Input:          
  Output:     
  Return:         �µĴӻ���ţ�MS_NEWS_DEVNUM��ʾû�пɷ���Ĵӻ����
  Others:
*************************************************/
UCHAR GetNewSNum(void)
{
    UCHAR i;
    if(g_stYkppParamSet.stYkppCommParam.usSBitmap == 0xFFFF)
    {
        g_stYkppParamSet.stYkppCommParam.ucNewSNum = MS_NEWS_DEVNUM;
        return MS_NEWS_DEVNUM;
    }

    for(i=0;i<16;i++)
    {
        if(((0x8000>>i) & g_stYkppParamSet.stYkppCommParam.usSBitmap) == 0)
        {
            g_stYkppParamSet.stYkppCommParam.ucNewSNum = i+1;
            return (i+1);
        }
    }
    //notice�������FFFF�϶��ܷ��䵽
    //g_stYkppParamSet.stYkppCommParam.ucNewSNum = MS_NEWS_DEVNUM;
    //return MS_NEWS_DEVNUM;
}


/*************************************************
  Function:       HndlTakeOther
  Description:    ���������յ�������Э��������ݽ��д���

  Calls:
  Called By:
  Input:          
                  pucMem:  �յ��ĳ���������ָ��
                  ulMaxSize: �ڴ����󳤶�
                  ucSrc:   ��Դ������or����
  Output:     
  Return:         
  Others:
*************************************************/
LONG HndlTakeOther(UCHAR* pucMem,ULONG ulMaxSize, UCHAR ucSrc)
{
    MESSAGE_ST *pstMessage;
    OHCH_RECV_DATA_IND_ST *pstOHCHRec;
    //���ݲ�ͬԭ����������Ӧ����
    switch(*((ULONG*)pucMem))
    {
    case OHCH_SEND_DATA_REQ:
        //��������ָ��
        pstOHCHRec = (OHCH_RECV_DATA_IND_ST*)pucMem;
        pstOHCHRec->pucData = (UCHAR*)pstOHCHRec + sizeof(*pstOHCHRec);

        if(ucSrc == COMM_IF_SOUTH)
        {
            //ԭ�����Ͳ���Ҫ�ı�
            //pstOHCHRec->ulPrmvType = OHCH_SEND_DATA_REQ;
            pstOHCHRec->ucSrcIf = COMM_IF_SOUTH;
        }
        else
        {
            //�ı�ԭ������
            pstOHCHRec->ulPrmvType = OHCH_RECV_DATA_IND;
            pstOHCHRec->ulDataLenMax = ulMaxSize - sizeof(pstOHCHRec);
            pstOHCHRec->ucSrcIf = COMM_IF_NORTH;
        }

        //������Ϣ����Ϣ�������������ݰ�
        if(OSQPost(g_pstOHQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            return FAILED;
        }
        break;
    case OHCH_MESS_SEND_REQ:        
        pstMessage = (MESSAGE_ST*)pucMem;
        //�ı���Ϣ����
        pstMessage->ulMessType = OHCH_MESS_RECV_IND;

        //������Ϣ����Ϣ�������������ݰ�
        if(OSQPost(g_pstOHQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            return FAILED;
        }
        break;
    case ACCH_MESS_SEND_REQ:
        pstMessage = (MESSAGE_ST*)pucMem;

        //�ı���Ϣ����
        pstMessage->ulMessType = ACCH_MESS_RECV_IND;

        //������Ϣ����Ϣ�������������ݰ�
        if(OSQPost(g_pstACQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            return FAILED;
        }
        break;
    case OHCH_DEBU_SEND_REQ:
    case OHCH_UNDE_SEND_REQ:
        //��������ָ��
        pstOHCHRec = (OHCH_RECV_DATA_IND_ST*)pucMem;
        pstOHCHRec->pucData = (UCHAR*)pstOHCHRec + sizeof(*pstOHCHRec);

        if(ucSrc == COMM_IF_SOUTH)
        {
            pstOHCHRec->ucSrcIf = COMM_IF_SOUTH;
        }
        else
        {
            pstOHCHRec->ucSrcIf = COMM_IF_NORTH;
        }
        //�ı�ԭ������
        if(pstOHCHRec->ulPrmvType == OHCH_UNDE_SEND_REQ)
        {
            pstOHCHRec->ulPrmvType = OHCH_UNDE_RECV_IND;
            pstOHCHRec->ulDataLenMax = ulMaxSize - sizeof(pstOHCHRec);
        }

        //������Ϣ����Ϣ�������������ݰ�
        if(OSQPost(g_pstOHQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            return FAILED;
        }
        break;
    case MESS_QUERY_REQ:
    case MESS_QUERY_RSP:
    case MESS_SET_REQ:
    case MESS_SET_RSP:
    case MESS_ALARM_REP:
        //��Ϣ���������ֱ�ӷ���OH
        //������Ϣ����Ϣ�������������ݰ�
        if(OSQPost(g_pstOHQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            return FAILED;
        }
        break;
    default:
        break;
    }
}


/*************************************************
  Function:     DPNthRecMSHndl
  Description:    ����������������ݽ�������Э��Ĵ���
                       ������ֻʹ���ڱ���ӿ�
  Calls:
  Called By:
  Input:          
                  pstDataPack:  ƥ�����ݰ�����ָ��
  Output:     
  Return:         
  Others:
*************************************************/
LONG DPNthRecMSHndl(DATA_PACK_ST *pstDataPack)
{
    ULONG ulSize;
    USHORT usCrc;
    UCHAR *pucData;
    MS_PROTOCOL_ST stMSProtocol;
    UCHAR *pucMem;   
    CH_CTR_ST stCHCtrMsg;
    BOOL bSendCtrlMsg = FALSE;    

    //�ó����ݰ�����
    ulSize = ULONG((pstDataPack->pucInPos)-(pstDataPack->pucData)); 
    //�����ж�
    if(ulSize < DATAPACK_MS_MINSIZE)
    {
        return FAILED;
    }

    pucData = pstDataPack->pucData;
    //ȥ����β
    pucData = pucData + 3;
    ulSize = ulSize - 4;

    //ת�崦��
    ulSize = DecodeEscape(pucData, ulSize, MS_ESCAPE_CHAR, 
            g_aucMSEscaped, g_aucMSUnEscape, 2);

    //ת��ʧ���˳�
    if(ulSize == 0)
    {
        return FAILED;
    }

    //crcУ��
    ulSize = ulSize - 2;
    usCrc = CalcuCRC(pucData, ulSize);
    if(usCrc != GET_WORD(pucData + ulSize))
    {
        return FAILED;
    }

    //�������Э������
    stMSProtocol.ucProtocolType = pucData[0];
    stMSProtocol.ucSNum = pucData[1];
    stMSProtocol.ucCmdUnit = pucData[2];
    stMSProtocol.ucFlagUnit = pucData[3];
    stMSProtocol.pucDataUnit = pucData + 4;
    stMSProtocol.ulDataUnitSize = ulSize - 4;

    //�ж�Э������
    if(stMSProtocol.ucProtocolType != MS_PROTOCOL_TYPE)
    {
        return FAILED;
    }

    //�ӱ�־��Ԫ�л�ȡ���豸�ĵ�ǰͨ��״̬
    UCHAR ucFlag = stMSProtocol.ucFlagUnit & 0x06;//ȡ��BIT2:1
    switch(ucFlag)
    {
    case 0:
        g_ucCurrCommMode = COMM_NO_CONN;
        g_stCenterConnStatus.ucLinkStatus = COMM_STATUS_DISCONNECTED;
        g_stCenterConnStatus.ucLoginStatus = COMM_STATUS_LOGOUT;
        break;
    case 2:
        g_ucCurrCommMode = COMM_MODE_CSD;
        g_stCenterConnStatus.ucLinkStatus = COMM_STATUS_DISCONNECTED;
        g_stCenterConnStatus.ucLoginStatus = COMM_STATUS_LOGOUT;
        break;
    case 4:
        g_ucCurrCommMode = COMM_MODE_GPRS;
        //������½����������ֱ����״̬����������ʹOH�����ϱ�GPRS����
        g_stCenterConnStatus.ucLinkStatus = COMM_STATUS_CONNECTED;
        g_stCenterConnStatus.ucLoginStatus = COMM_STATUS_LOGIN;
        break;
    default:
        break;
    }
    

    //�жϴӻ����(���㲥��ʽ)
    if((stMSProtocol.ucSNum != g_stYkppParamSet.stYkppCommParam.ucSNum) &&
            (stMSProtocol.ucSNum != MS_BROADCAST_DEVNUM))
    {
        return FAILED;
    }            

    //�ж��Ƿ������豸�����������ָ����˳�
    if(stMSProtocol.ucSNum == MS_NEWS_DEVNUM)
    {
        switch(stMSProtocol.ucCmdUnit)
        {
        case MS_CMD_ASK:
            //��������ע��
            stCHCtrMsg.ulPrmvType = CH_MG_SM_REG;
            bSendCtrlMsg = TRUE;
            break;
        case MS_CMD_REGANS:
            //�޸Ĵӻ����
            //��������Ӧ��
            //notice������Ҫ��дһ�����ò�ѯ����ĺ���
            if(GET_WORD(stMSProtocol.pucDataUnit +2) == 0x8001)
            {
                g_stYkppParamSet.stYkppCommParam.ucSNum = *(stMSProtocol.pucDataUnit +4);
                //notice
                if(g_stYkppParamSet.stYkppCommParam.ucSNum == MS_NEWS_DEVNUM)
                {
                    //����Ϣ�Ͻ���OH��֪ͨע��ʧ�ܣ�ԭ�������豸�޿��õĴӻ����
                    return FAILED;
                }                
                //noticeдFLASH
                SaveYkppParam();
                stCHCtrMsg.ulPrmvType = CH_MG_SM_ANS;
                bSendCtrlMsg = TRUE;
                //notice����Ϣ��OH��֪ͨע��ɹ�
            }
            else
            {
                return FAILED;
            }
            break;
        default:
            break;
        }
    }

    //�ж��Ƿ��ǹ㲥�����������ָ����˳�
    //Ŀǰ�����豸��֤�㲥��ʽ��ֻ����Э�����
    else if(stMSProtocol.ucSNum == MS_BROADCAST_DEVNUM)
    {
        ULONG ulMaxSize = 0;
        
        switch(stMSProtocol.ucCmdUnit)
        {
        case MS_CMD_TAKEOHTER:
            //����Ϣ����OH��
            //��Ϊ��֪�Ƕ���Ϣ����ֱ������Ҫ�жϴ�С
            if(stMSProtocol.ulDataUnitSize < MEM_BLK_SIZE_256 - 4)
            {
                pucMem = (UCHAR *) MyMalloc(g_pstMemPool256);
                ulMaxSize = MEM_BLK_SIZE_256 - 4;
            }
            else if(stMSProtocol.ulDataUnitSize < MEM_BLK_SIZE_512 - 4)
            {
                pucMem = (UCHAR *) MyMalloc(g_pstMemPool512);
                ulMaxSize = MEM_BLK_SIZE_512 - 4;
            }
            else
            {
                return FAILED;
            }
            
            if(pucMem == (void*)NULL)
            {
                return FAILED;
            }
            memcpy(pucMem,stMSProtocol.pucDataUnit,stMSProtocol.ulDataUnitSize);

            //���ݲ�ͬԭ����������Ӧ����
            HndlTakeOther(pucMem,ulMaxSize,COMM_IF_NORTH);
            
            //test
            /*
            if(g_ucTest == 0)
            {
                g_ucTest = 1;
                PioWrite(&LED_PIO_CTRL, 
                LED_MASK,       
                LED_ON);       
            }
            else
            {
                g_ucTest = 0;
                PioWrite(&LED_PIO_CTRL, 
                LED_MASK,       
                LED_OFF);       
            }
            */


        default:
            break;
        }
    }    

    //�����豸�ҷǹ㲥���Ԫ����
    else
    {
        switch(stMSProtocol.ucCmdUnit)
        {
        case MS_CMD_ASK:
            //�ñ���ӿ���
            g_stNorthInterface.ucState = COMM_STATE_SEND_IDLE;
            break;
        case MS_CMD_UNREGANS:
            //����ȷ��ɾ����Ϣ
            stCHCtrMsg.ulPrmvType = CH_MG_SM_UNREGCONF;
            bSendCtrlMsg = TRUE;            
            break; 
        default:
            break;
        }
    }

    if(bSendCtrlMsg)
    {
        pucMem = (UCHAR *) MyMalloc(g_pstMemPool16);
        if(pucMem == (void*)NULL)
        {
            return FAILED;
        }
        memcpy(pucMem,&stCHCtrMsg,sizeof(stCHCtrMsg));

        //������Ϣ����Ϣ�������������ݰ�
        if(OSQPost(g_pstCHNthCtrlQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            return FAILED;
        }
    }    
    return SUCCEEDED;
    
    //test
/*
    UINT32 cpu_sr;
    OS_ENTER_CRITICAL();    
    UsartWrite(&g_stUsart0, pstDataPack->pucData, 
            (pstDataPack->pucInPos)-(pstDataPack->pucData));
    OS_EXIT_CRITICAL();
    while(g_stUsart0.pstData->pucTxIn!=g_stUsart0.pstData->pucTxOut);
 */
}



/*************************************************
  Function:     DPSthRecMSHndl
  Description:    ����������������ݽ�������Э��Ĵ���
                  ������ֻʹ��������ӿ�
  Calls:
  Called By:
  Input:          
                  pstDataPack:  ƥ�����ݰ�����ָ��
  Output:     
  Return:         
  Others:
*************************************************/
LONG DPSthRecMSHndl(DATA_PACK_ST *pstDataPack)
{
    ULONG ulSize;
    USHORT usCrc;
    UCHAR *pucData;
    MS_PROTOCOL_ST stMSProtocol;
    UCHAR *pucMem;   
    CH_CTR_ST stCHCtrMsg;
    BOOL bSendCtrlMsg = FALSE;

    //���жϷ�ֹ��ʱ��ʱ
    //����ͬʱ������ѯ��ʱ��Ϣ�����¿�����Ϣ
    //OS_ENTER_CRITICAL();
    //notice�ظ��ط��޸�
    //�ó����ݰ�����
    ulSize = (ULONG)((pstDataPack->pucInPos)-(pstDataPack->pucData)); 
    //�����ж�
    if(ulSize < DATAPACK_MS_MINSIZE)
    {
        //OS_EXIT_CRITICAL();
        return FAILED;
    }

    pucData = pstDataPack->pucData;
    //ȥ����β
    pucData = pucData + 3;
    ulSize = ulSize - 4;

    //ת�崦��
    ulSize = DecodeEscape(pucData, ulSize, MS_ESCAPE_CHAR, 
            g_aucMSEscaped, g_aucMSUnEscape, 2);

    //ת��ʧ���˳�
    if(ulSize == 0)
    {
        return FAILED;
    }

    //notice�������ճ�ʱ��ʱ��
    //StartGeneralTimer(&g_stTimerSthRecTimeOut, TIME_TIMEOUT_USARTREC/TIMER_UNIT_MS, NULL,NONPERIODIC);

    //crcУ��
    ulSize = ulSize - 2;
    usCrc = CalcuCRC(pucData, ulSize);
    if(usCrc != GET_WORD(pucData + ulSize))
    {
        //OS_EXIT_CRITICAL();
        return FAILED;
    }

    //�������Э������
    stMSProtocol.ucProtocolType = pucData[0];
    stMSProtocol.ucSNum = pucData[1];
    stMSProtocol.ucCmdUnit = pucData[2];
    stMSProtocol.ucFlagUnit = pucData[3];    
    stMSProtocol.pucDataUnit = pucData + 4;
    stMSProtocol.ulDataUnitSize = ulSize - 4;

    //�ж�Э������
    if(stMSProtocol.ucProtocolType != MS_PROTOCOL_TYPE)
    {
        //OS_EXIT_CRITICAL();
        return FAILED;
    }

    //MCM-45_20061130_zhangjie_begin
    if(g_stSouthInterface.ucInterfaceType == USART_SLAVE_485)
    {
        g_stSouthInterface.ucState = COMM_STATE_SEND_IDLE;
    }
    //MCM-45_20061130_zhangjie_end

    //�������ѯ�Ĳ�һ�£������ð�
    //notice
    /*
    if(stMSProtocol.ucSNum != g_stYkppParamSet.stYkppCommParam.ucAskingSNum)
    {
        //OS_EXIT_CRITICAL();
        return FAILED;
    }
    */
    //�ж��Ƿ������豸�����������ָ����˳�
    if(stMSProtocol.ucSNum == MS_NEWS_DEVNUM)
    {
        //����ѯ��ʱ��ʱ��
        StopGeneralTimer(&g_stTimerSthAskTimeOut);
        //��ʱ���رպ�Ϳɴ��ж�
        //OS_EXIT_CRITICAL();
        switch(stMSProtocol.ucCmdUnit)
        {
        case SM_CMD_REG:
            //��������ע��Ӧ��
            stCHCtrMsg.ulPrmvType = CH_MG_MS_REGANS;
            bSendCtrlMsg = TRUE;
            break;
        default:
            //������ѯ��һ���ӻ�������
            stCHCtrMsg.ulPrmvType = CH_MG_MS_ASK;
            bSendCtrlMsg = TRUE;
            break;
        }
    }    

    //�ж��Ƿ��ǹ㲥�����������ָ����˳�
    //Ŀǰ����ӿ��յ��㲥��ϢΪ�Ƿ�
    else if(stMSProtocol.ucSNum == MS_BROADCAST_DEVNUM)
    {
        //OS_EXIT_CRITICAL();
        return FAILED;
    }

    //�����豸�ҷǹ㲥���Ԫ����
    else
    {
        //����ѯ��ʱ��ʱ��
        StopGeneralTimer(&g_stTimerSthAskTimeOut);
        //��ʱ���رպ�Ϳɴ��ж�
        //OS_EXIT_CRITICAL();
        //�������ͨ�Ÿ澯λ
        MSChangeBit(&g_stYkppParamSet.stYkppCommParam.usCommAlarm, stMSProtocol.ucSNum, 0);

        ULONG ulMaxSize = 0;
        
        switch(stMSProtocol.ucCmdUnit)
        {
        case SM_CMD_ANS:            
            //�������ע���豸��Ӧ����ı�ӻ�λͼ
            if(stMSProtocol.ucSNum == g_stYkppParamSet.stYkppCommParam.ucNewSNum)
            {
                MSChangeBit(&g_stYkppParamSet.stYkppCommParam.usSBitmap, stMSProtocol.ucSNum, 1);
                g_stYkppParamSet.stYkppCommParam.ucNewSNum = MS_NEWS_DEVNUM;
                //������flash
                SaveYkppParam();
            }           
            
            //������ѯ��һ���ӻ�������
            stCHCtrMsg.ulPrmvType = CH_MG_MS_ASK;
            bSendCtrlMsg = TRUE;
            break;
        case SM_CMD_UNREG:
            //��������ɾ����Ӧ��
            stCHCtrMsg.ulPrmvType = CH_MG_MS_UNREGANS;
            stCHCtrMsg.ucParam1 = stMSProtocol.ucSNum;//��Ҫ���ݴӻ����
            bSendCtrlMsg = TRUE;
            break;
        case SM_CMD_UNREGCONF:
            //��������ɾ��Ӧ���ʱ���Ѿ�ɾ����Ӧ�Ĵӻ����
            //������ѯ��һ���ӻ�������
            stCHCtrMsg.ulPrmvType = CH_MG_MS_ASK;
            bSendCtrlMsg = TRUE;
            break;
        case SM_CMD_TAKEOHTER:
            //������ѯ��һ���ӻ�������
            stCHCtrMsg.ulPrmvType = CH_MG_MS_ASK;
            bSendCtrlMsg = TRUE;
            //����Ϣ����OH
            //��Ϊ��֪�Ƕ���Ϣ����ֱ������Ҫ�жϴ�С            
            if(stMSProtocol.ulDataUnitSize < MEM_BLK_SIZE_256 - 4)
            {
                pucMem = (UCHAR *) MyMalloc(g_pstMemPool256);
                ulMaxSize = MEM_BLK_SIZE_256 - 4;
            }
            else if(stMSProtocol.ulDataUnitSize < MEM_BLK_SIZE_512 - 4)
            {
                pucMem = (UCHAR *) MyMalloc(g_pstMemPool512);
                ulMaxSize = MEM_BLK_SIZE_512 - 4;
            }
            else
            {
                return FAILED;
            }
            
            if(pucMem == (void*)NULL)
            {
                return FAILED;
            }
            
            memcpy(pucMem,stMSProtocol.pucDataUnit,stMSProtocol.ulDataUnitSize);

            //���ݲ�ͬԭ����������Ӧ����
            HndlTakeOther(pucMem,ulMaxSize,COMM_IF_SOUTH);
            break;
            
        default:
            //������ѯ��һ���ӻ�������
            stCHCtrMsg.ulPrmvType = CH_MG_MS_ASK;
            bSendCtrlMsg = TRUE;
            break;
        }
    }

    if(bSendCtrlMsg)
    {
        pucMem = (UCHAR *) MyMalloc(g_pstMemPool16);
        if(pucMem == (void*)NULL)
        {
            return FAILED;
        }
        memcpy(pucMem,&stCHCtrMsg,sizeof(stCHCtrMsg));

        //������Ϣ����Ϣ�������������ݰ�
        if(OSQPost(g_pstCHSthCtrlQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            return FAILED;
        }
    }
    return SUCCEEDED;
}

/*************************************************
  Function:     DPHandInCSDorGPRS
  Description:    �������������CSD���ݰ��Ͻ���OH��
                       ������Ŀǰֻʹ���ڱ���ӿ�
  Calls:
  Called By:
  Input:          
                  pstDataPack:  ƥ�����ݰ�����ָ��
  Output:     
  Return:         
  Others:
*************************************************/
LONG DPHandInCSDorGPRS(DATA_PACK_ST *pstDataPack )
{
    UCHAR *pucMem;
    OHCH_RECV_DATA_IND_ST stOHCHRec;
    ULONG ulSize;
    
    //�ó����ݰ�����
    ulSize = ULONG((pstDataPack->pucInPos)-(pstDataPack->pucData));
    
    //�����ڴ棬ʧ���������ݰ�
    //notice���������ڴ��С��С�Ĳ�����������

    if((sizeof(stOHCHRec)+ulSize) <= MEM_BLK_SIZE_256 - 4 )
    {
        pucMem = (UCHAR *) MyMalloc(g_pstMemPool256);
        stOHCHRec.ulDataLenMax = MEM_BLK_SIZE_256 - 4 - sizeof(stOHCHRec);
    }
    else if((sizeof(stOHCHRec)+ulSize) <= MEM_BLK_SIZE_512 - 4 )
    {
        pucMem = (UCHAR *) MyMalloc(g_pstMemPool512);
        stOHCHRec.ulDataLenMax = MEM_BLK_SIZE_512 - 4 - sizeof(stOHCHRec);
    }
    else
    {
        return FAILED;
    }
    
    if(pucMem == (void*)NULL)
    {
        return FAILED;
    }   

    
    //�ж��Ƿ���GPRS
    if(g_ucCurrCommMode == COMM_MODE_GPRS)
    {
        //��ת��
        ulSize = DecodeEscape(pstDataPack->pucData, ulSize, 
                GPRS_ESCAPE_CHAR, g_aucGPRSEscaped, g_aucGPRSUnEscape, 2);

        //ת��ʧ���˳�
        if(ulSize == 0)
        {
            MyFree(pucMem);
            return FAILED;
        }
        
        stOHCHRec.ucCommMode = COMM_MODE_GPRS;
    }
    else
    {
        stOHCHRec.ucCommMode = COMM_MODE_CSD;
    }

    //����ԭ�Ｐ���ݰ���������ڴ�
    stOHCHRec.ulPrmvType = OHCH_RECV_DATA_IND;
    //stOHCHRec.ulPadding1 = 0;
    stOHCHRec.ucPadding2 = 0;
    stOHCHRec.ucPadding3 = 0;
    stOHCHRec.ucSrcIf = COMM_IF_NORTH;
    //stOHCHRec.ucCommMode = COMM_MODE_CSD;
    stOHCHRec.ucDCS= 8;
    //pstOHCHRec.acSrcTelNum
    stOHCHRec.ucPadding4 = 0;
    stOHCHRec.ulDataLen = ulSize;
    stOHCHRec.pucData = pucMem + sizeof(stOHCHRec);
    memcpy(pucMem,&stOHCHRec,sizeof(stOHCHRec));
    memcpy(stOHCHRec.pucData, pstDataPack->pucData, ulSize);    


    //MY_LOG_MSG_BIN((LS_TRACE, LL_INFO, pucMem,sizeof(stOHCHRec)+ulSize));


    //test��������Э��Ĳ��ԣ���������ԭ������ݰ�
/*    
    UINT32 cpu_sr;
    OS_ENTER_CRITICAL();    
    UsartWrite(&g_stUsart0, (UCHAR*)pucMem, 
            sizeof(stOHCHRec)+ulSize);
    OS_EXIT_CRITICAL();
    while(g_stUsart0.pstData->pucTxIn!=g_stUsart0.pstData->pucTxOut);
    return SUCCEEDED;
*/

            //test
            /*
            if(g_ucTest == 0)
            {
                g_ucTest = 1;
                PioWrite(&LED_PIO_CTRL, 
                LED_MASK,       
                LED_ON);       
            }
            else
            {
                g_ucTest = 0;
                PioWrite(&LED_PIO_CTRL, 
                LED_MASK,       
                LED_OFF);       
            }
            */
    
    //������Ϣ����Ϣ�������������ݰ�
    if(OSQPost(g_pstOHQue, pucMem) == OS_Q_FULL)
    {
        MyFree(pucMem);
        return(FAILED);
    }

    return SUCCEEDED;
    
    //test
/*    
    UINT32 cpu_sr;
    OS_ENTER_CRITICAL();    
    UsartWrite(&g_stUsart0, pstDataPack->pucData, 
            (pstDataPack->pucInPos)-(pstDataPack->pucData));
    OS_EXIT_CRITICAL();
    while(g_stUsart0.pstData->pucTxIn!=g_stUsart0.pstData->pucTxOut);
*/    
    
}

UCHAR g_aucDebugOpen[] = "#debug open*";
UCHAR g_aucDebugClose[] = "#debug close*";

/*************************************************
  Function:     DPHandInDeb
  Description:    ������������ĵ������ݰ���CSD�ķ�ʽ�Ͻ���OH��
                       ������Ŀǰֻʹ���ڵ��Խӿ�
  Calls:
  Called By:
  Input:          
                  pstDataPack:  ƥ�����ݰ�����ָ��
  Output:     
  Return:         
  Others:
*************************************************/
LONG DPHandInDeb(DATA_PACK_ST *pstDataPack )
{
    UCHAR *pucMem;
    OHCH_RECV_DATA_IND_ST stOHCHRec;
    ULONG ulSize; 

    //�ó����ݰ�����
    ulSize = ULONG((pstDataPack->pucInPos)-(pstDataPack->pucData));

    //notice ������ʱ����ֹû��ֹͣdebugģʽ
    StartGeneralTimer(&g_stTimerDebugTimeOut, 
        TIME_TIMEOUT_DEBUG/TIMER_UNIT_MS, NULL,NONPERIODIC);

    //�����ڴ棬ʧ���������ݰ�
    //notice���������ڴ��С��С�Ĳ�����������
    /*
    if((sizeof(stOHCHRec)+ulSize) <= MEM_BLK_SIZE_256 - 4)
    {
        pucMem = (UCHAR *) MyMalloc(g_pstMemPool256);
        stOHCHRec.ulDataLenMax = MEM_BLK_SIZE_256 - 4 - sizeof(stOHCHRec);
    }
    */
    //��ֹ��ѯ��ز����б����ݱ䳤
    if((sizeof(stOHCHRec)+ulSize) <= MEM_BLK_SIZE_512 - 4)
    {
        pucMem = (UCHAR *) MyMalloc(g_pstMemPool512);
        stOHCHRec.ulDataLenMax = MEM_BLK_SIZE_512 - 4 - sizeof(stOHCHRec);
    }
    else
    {
        return FAILED;
    }
    
    if(pucMem == (void*)NULL)
    {
        return FAILED;
    }
    
    stOHCHRec.ucCommMode = COMM_MODE_CSD;

    //����ԭ�Ｐ���ݰ���������ڴ�
    stOHCHRec.ulPrmvType = OHCH_UNDE_RECV_IND;
    //stOHCHRec.ulPadding1 = 0;
    stOHCHRec.ucPadding2 = 0;
    stOHCHRec.ucPadding3 = 0;
    stOHCHRec.ucSrcIf = COMM_IF_DEBUG;
    //stOHCHRec.ucCommMode = COMM_MODE_CSD;
    stOHCHRec.ucDCS= 8;
    //pstOHCHRec.acSrcTelNum
    stOHCHRec.ucPadding4 = 0;
    stOHCHRec.ulDataLen = ulSize;
    stOHCHRec.pucData = pucMem + sizeof(stOHCHRec);
    memcpy(pucMem,&stOHCHRec,sizeof(stOHCHRec));
    memcpy(stOHCHRec.pucData, pstDataPack->pucData, ulSize);

    //notice CH����ֱ�Ӵ򿪺͹رյ���ģʽ
    UCHAR ucTemp = 0;
    if(strncmp((const char*)pstDataPack->pucData,(const char*)g_aucDebugOpen,ulSize)==0)
    {            
        g_stYkppParamSet.stYkppCtrlParam.ucDebugMode = TRUE;
        ucTemp = 1;
    }
    else if(strncmp((const char*)pstDataPack->pucData,(const char*)g_aucDebugClose,ulSize)==0)
    {
        g_stYkppParamSet.stYkppCtrlParam.ucDebugMode = FALSE;
        ucTemp = 1;
    }    
    if(ucTemp)
    {            
        stOHCHRec.ulPrmvType = OHCH_DEBU_SEND_REQ;
        memcpy(pucMem,&stOHCHRec,sizeof(stOHCHRec));
        //notice ������ʱ����ֹû��ֹͣdebugģʽ
        //������Ϣ��debug���У���Ϣ�������������ݰ�
        if(OSQPost(g_pstCHDebQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            return FAILED;
        }
        return SUCCEEDED;
    }
        


    //�ǵ���ģʽ�򲻴���
    if(g_stYkppParamSet.stYkppCtrlParam.ucDebugMode != TRUE)
    {
        MyFree(pucMem);
        return SUCCEEDED;
    }
    
    //������Ϣ����Ϣ�������������ݰ�
    if(OSQPost(g_pstOHQue, pucMem) == OS_Q_FULL)
    {
        MyFree(pucMem);
        return FAILED;
    }

    return SUCCEEDED;
}

/*************************************************
  Function:     ATCMDISEqual
  Description:    �������Ƚ�����AT����
  Calls:
  Called By:
  Input:     pucRecATָ���յ���AT����
                pucConstATָ����õ�AT����        
  Output:   
  Return:  TRUE:���        
  Others:
*************************************************/
LONG ATCMDISEqual(UCHAR pucRecAT[],ULONG ulRecSize,UCHAR pucConstAT[],ULONG ulConstSize)
{
    if(ulRecSize >= ulConstSize)
    {
        if(strncmp((const char*)pucRecAT,(const char*)pucConstAT,ulConstSize) == 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}



/*************************************************
  Function:     DPHandInATCMD
  Description:    ����������AT���ص����ݰ�
                       ����б�Ҫ���Ͻ���OH��(�����Ϣ)
                       ������Ŀǰֻʹ���ڱ���ӿ�
  Calls:
  Called By:
  Input:         
                  pstDataPack:  ƥ�����ݰ�����ָ��
  Output:     
  Return:         
  Others:
*************************************************/
LONG DPHandInATCMD(DATA_PACK_ST *pstDataPack )
{
    UINT32 cpu_sr,ulATSize;
    BOOL bSendCtrlMsg,bDP2Succeeded;
    UCHAR *pucMem;
    UCHAR *pucATData;    
    CH_CTR_ST stCHCtrMsg;

    //���жϷ�ֹ��ʱ��ʱ
    //����ͬʱ����TIMEOUT ��OK ERROR��Ϣ
    OS_ENTER_CRITICAL();
    
    ulATSize = pstDataPack->pucInPos - pstDataPack->pucData;

    //�жϳ��ȣ�ȥ����β2��0D0A
    if(ulATSize>5)//notice ȥ��>�ĸ���
    {
        ulATSize = ulATSize -4;
        pucATData = pstDataPack->pucData + 2;
    }
    else
    {
        OS_EXIT_CRITICAL();
        return FAILED;
    }

    //Ĭ�ϴ����2�������ɹ�
    bDP2Succeeded = FAILED;
    
    //�����ж��ǲ���Ҫ����ڶ�����
    if(g_stDataPackATFull.ucState == DATAPACK_STATE_CMT)
    {
        //���ڶ���Ϣ��������Ҳ���������򴮿ڿ�����Ϣ
        //������ǰ���ж�
        OS_EXIT_CRITICAL();

        SMS_PARAM_ST stSMS;
        //�����ڴ棬ʧ���������ݰ�
        //teseȡ��512�ڴ������
        pucMem = (UCHAR *) MyMalloc(g_pstMemPool512);
        if(pucMem != (void*)NULL)
        {
            //������Ϣ�ṹ�����ڴ�
            stSMS.pucData = (UCHAR*)pucMem;
            //PDU����
            if(DecodeAtCmtInPDUMode(g_stDataPackATFull.pucData1, g_stDataPackATFull.ulData1Size, 
                    pucATData, ulATSize, &stSMS) == SUCCEEDED)
            {
                UCHAR *pucMem1;
                OHCH_RECV_DATA_IND_ST stOHCHRec;
                
                bDP2Succeeded = SUCCEEDED;
                //��OH�ύSMS��Ϣ
                //�����ڴ棬ʧ���������ݰ�
                pucMem1 = (UCHAR *) MyMalloc(g_pstMemPool256);
                if(pucMem1 == (void*)NULL)
                {
                    return FAILED;
                }
                //����ԭ�Ｐ���ݰ���������ڴ�
                stOHCHRec.ulPrmvType = OHCH_RECV_DATA_IND;
                stOHCHRec.ulDataLenMax = MEM_BLK_SIZE_256 - 4 - sizeof(stOHCHRec);
                //stOHCHRec.ulPadding1 = 0;
                stOHCHRec.ucPadding2 = 0;
                stOHCHRec.ucPadding3 = 0;
                stOHCHRec.ucSrcIf = COMM_IF_NORTH;
                stOHCHRec.ucCommMode = COMM_MODE_SMS;
                stOHCHRec.ucDCS= stSMS.ucDCS;
                stOHCHRec.ucDA= stSMS.ucDA;
                //notice strcpy
                memcpy(stOHCHRec.acSrcTelNum,stSMS.acTelNum,MAX_TEL_NUM_LEN+1);
                stOHCHRec.ucPadding4 = 0;
                stOHCHRec.ulDataLen = stSMS.ulDataLen;
                stOHCHRec.pucData = pucMem1 + sizeof(stOHCHRec);
                memcpy(pucMem1,&stOHCHRec,sizeof(stOHCHRec));
                memcpy(stOHCHRec.pucData, stSMS.pucData, stSMS.ulDataLen);

                //MY_LOG_MSG_BIN((LS_TRACE, LL_INFO, pucMem1,sizeof(stOHCHRec)+stSMS.ulDataLen));

                MyFree(pucMem);

            //test
            /*
            if(g_ucTest == 0)
            {
                g_ucTest = 1;
                PioWrite(&LED_PIO_CTRL, 
                LED_MASK,       
                LED_ON);       
            }
            else
            {
                g_ucTest = 0;
                PioWrite(&LED_PIO_CTRL, 
                LED_MASK,       
                LED_OFF);       
            }
            */

                //������Ϣ����Ϣ�������������ݰ�
                if(OSQPost(g_pstOHQue, pucMem1) == OS_Q_FULL)
                {
                    MyFree(pucMem1);
                    return FAILED;
                }
            }
            else
            {
                MyFree(pucMem);
                bDP2Succeeded = FAILED;
            }
        }
        else
        {
            bDP2Succeeded = FAILED;
        }

        bSendCtrlMsg = FALSE;
        g_stDataPackATFull.ucState = DATAPACK_STATE_NO;        
    }//if(g_stDataPackATFull.ucState == DATAPACK_STATE_CMT)
    else if(g_stDataPackATFull.ucState == DATAPACK_STATE_CRING)
    {
        //�����յ��������
        if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_CLIP, sizeof(g_aucRAT_CLIP)-1))
        {
            //ȡ�������������ݴ���
            memcpy(g_aucCLIPTel,pucATData,ulATSize);
            stCHCtrMsg.ulPrmvType = CH_MG_AT_CSD_CON;
            bSendCtrlMsg = TRUE;
            bDP2Succeeded = SUCCEEDED;
        }
        //���յ�һ��CRING˵����������ʾ,��ʱժ��
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_CRING, sizeof(g_aucRAT_CRING)-1))
        {
            stCHCtrMsg.ulPrmvType = CH_MG_AT_ATA;
            bSendCtrlMsg = TRUE;
            bDP2Succeeded = SUCCEEDED;
        }
        else
        {
            bSendCtrlMsg = FALSE;
            bDP2Succeeded = FAILED;        
        }
        g_stDataPackATFull.ucState = DATAPACK_STATE_NO;        
    }
    else if(g_stDataPackATFull.ucState == DATAPACK_STATE_SOCKETCLOSE)
    {
        //ʧȥGPRS���ӻ����յ�һ��OK
        if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_OK, sizeof(g_aucRAT_OK)-1))
        {
            stCHCtrMsg.ulPrmvType = CH_MG_AT_GPRS_SOCKETCLOSE;
            bSendCtrlMsg = TRUE;
            bDP2Succeeded = SUCCEEDED;
        }
        else
        {
            bSendCtrlMsg = FALSE;
            bDP2Succeeded = FAILED;        
        }
        g_stDataPackATFull.ucState = DATAPACK_STATE_NO;
    }
    else if(g_stDataPackATFull.ucState == DATAPACK_STATE_CSCA)
    {
        //�յ��������ĺ���󻹻��յ�һ��OK
        if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_OK, sizeof(g_aucRAT_OK)-1))
        {
            //�ӵ�һ����ȡ���������ĺ������������ĺ����ݴ���
            memcpy(g_aucSMCAddr,g_stDataPackATFull.pucData1,
                    g_stDataPackATFull.ulData1Size);
            stCHCtrMsg.ulPrmvType = CH_MG_AT_CSCA;
            bSendCtrlMsg = TRUE;
            bDP2Succeeded = SUCCEEDED;
        }
        else
        {
            bSendCtrlMsg = FALSE;
            bDP2Succeeded = FAILED;        
        }
        g_stDataPackATFull.ucState = DATAPACK_STATE_NO;
    }
    else if(g_stDataPackATFull.ucState == DATAPACK_STATE_CCED)
    {
        //�յ�С����Ϣ�󻹻��յ�һ��OK
        if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_OK, sizeof(g_aucRAT_OK)-1))
        {
            //�ӵ�һ����ȡ��С����Ϣ����С����Ϣ�ݴ���
            memcpy(g_aucZoneInfo,g_stDataPackATFull.pucData1,
                    g_stDataPackATFull.ulData1Size);
            stCHCtrMsg.ulPrmvType = CH_MG_AT_CCED;
            bSendCtrlMsg = TRUE;
            bDP2Succeeded = SUCCEEDED;
        }
        else
        {
            bSendCtrlMsg = FALSE;
            bDP2Succeeded = FAILED;        
        }
        g_stDataPackATFull.ucState = DATAPACK_STATE_NO;
    }

    //�����ж����ݰ����ܹ���
    //��������򲻴������ѵڶ���������Ϊ׼
    //notice max���ú�
    if(ulATSize>g_stDataPackATFull.ulDataPack1MaxSize) 
    {
        OS_EXIT_CRITICAL();
        return bDP2Succeeded;    
    }    

    //����ڶ��������ɹ������ƥ��AT��������ȶ�+�ж�
    if(bDP2Succeeded == FAILED)
    {
        //notice�ȱȽϽϳ���AT����,ע�����ȼ�
        if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_OK, sizeof(g_aucRAT_CMGS)-1))
        {    
            stCHCtrMsg.ulPrmvType = DATAPACK_STATE_UNKNOW;
            bSendCtrlMsg = FALSE;
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_OK, sizeof(g_aucRAT_OK)-1))
        {    
            stCHCtrMsg.ulPrmvType = CH_MG_AT_OK;
            bSendCtrlMsg = TRUE;
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_ERROR, sizeof(g_aucRAT_ERROR)-1))
        {
            stCHCtrMsg.ulPrmvType = CH_MG_AT_ERROR;
            bSendCtrlMsg = TRUE;        
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_CMT, sizeof(g_aucRAT_CMT)-1))
        {
            //�ı��״̬    
            g_stDataPackATFull.ucState = DATAPACK_STATE_CMT;
            bSendCtrlMsg = FALSE;
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_CMGL, sizeof(g_aucRAT_CMGL)-1))
        {
            //�ı��״̬    
            g_stDataPackATFull.ucState = DATAPACK_STATE_CMT;
            //��cmglһ���Ƕ��������ж���Ϣ
            g_ucCurrSimSMSSta = SIM_STA_READED;
            bSendCtrlMsg = FALSE;
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_CMTI, sizeof(g_aucRAT_CMTI)-1))
        {
            //sim������δ������Ϣ
            g_ucCurrSimSMSSta = SIM_STA_UNREAD;
            bSendCtrlMsg = FALSE;
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_CRING, sizeof(g_aucRAT_CRING)-1))
        {
            //�ı��״̬
            //�ж��Ƿ������ݺ���
            if(pucATData[ulATSize-1]=='C')
            {
                //�������ճ�ʱ��ʱ��
                //������δ��������ʾ���ܣ�Ҫ�ȵ���һ��CRING�������жϣ�����������ʱ
                StartGeneralTimer(&g_stTimerNthRecTimeOut, 
                        3000/TIMER_UNIT_MS, NULL,NONPERIODIC);
                g_stDataPackATFull.ucState = DATAPACK_STATE_CRING;
                bSendCtrlMsg = FALSE;
            }
            //����������һ�
            else
            {
                stCHCtrMsg.ulPrmvType = CH_MG_AT_ATH;
                bSendCtrlMsg = TRUE;
            }
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_GPRSAct, sizeof(g_aucRAT_GPRSAct)-1))
        {
            //notice���Դӵ�һ�����õ�IP��ַ
            memcpy(g_aucIPAddr,g_stDataPackATFull.pucData1,
                    g_stDataPackATFull.ulData1Size);
            stCHCtrMsg.ulPrmvType = CH_MG_AT_GPRS_ACT;
            bSendCtrlMsg = TRUE;        
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_GPRSActed, sizeof(g_aucRAT_GPRSActed)-1))
        {
            //��ʱGPRS�Ѽ���
            stCHCtrMsg.ulPrmvType = CH_MG_AT_GPRS_ACT;
            bSendCtrlMsg = TRUE;        
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_GPRSWaitData, sizeof(g_aucRAT_GPRSWaitData)-1))
        {
            stCHCtrMsg.ulPrmvType = CH_MG_AT_GPRS_WAITDATA;
            bSendCtrlMsg = TRUE;        
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_GPRSSocketClose, sizeof(g_aucRAT_GPRSSocketClose)-1))
        {
            g_stDataPackATFull.ucState = DATAPACK_STATE_SOCKETCLOSE;
            bSendCtrlMsg = FALSE;        
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_CSCA, sizeof(g_aucRAT_CSCA)-1))
        {
            g_stDataPackATFull.ucState = DATAPACK_STATE_CSCA;
            bSendCtrlMsg = FALSE;        
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_CCED, sizeof(g_aucRAT_CCED)-1))
        {
            g_stDataPackATFull.ucState = DATAPACK_STATE_CCED;
            bSendCtrlMsg = FALSE;        
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_NOCARRIER, sizeof(g_aucRAT_NOCARRIER)-1))
        {
            stCHCtrMsg.ulPrmvType = CH_MG_AT_ATH;
            bSendCtrlMsg = TRUE;        
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_CONNECT, sizeof(g_aucRAT_CONNECT)-1))
        {
            stCHCtrMsg.ulPrmvType = CH_MG_AT_CONNECT;
            bSendCtrlMsg = TRUE;        
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_TCPClosed, sizeof(g_aucRAT_TCPClosed)-1))
        {
            stCHCtrMsg.ulPrmvType = CH_MG_AT_GPRS_SOCKETCLOSE;
            bSendCtrlMsg = TRUE;        
        }
        else
        {
            //�ı��״̬
            g_stDataPackATFull.ucState = DATAPACK_STATE_UNKNOW;
            bSendCtrlMsg = FALSE;        
        }    
    }

    //���´�������Ҫ���Ϳ�����Ϣ
    if(bSendCtrlMsg)
    {
        //ֹͣ��ʱ��
        StopGeneralTimer(&g_stTimerATRtnTimeOut);

        //ֹͣ��ʱ�����жϣ���Ϊ�������TIMEOUT��Ϣ
        OS_EXIT_CRITICAL();

        pucMem = (UCHAR *) MyMalloc(g_pstMemPool16);
        if(pucMem == (void*)NULL)
        {
            //���´򿪶�ʱ��
            StartGeneralTimer(&g_stTimerATRtnTimeOut, TIME_WAIT_SMSSEND/TIMER_UNIT_MS, NULL,NONPERIODIC);
            return FAILED;
        }
        memcpy(pucMem,&stCHCtrMsg,sizeof(stCHCtrMsg));

        //������Ϣ����Ϣ�������������ݰ�
        if(OSQPost(g_pstCHNthCtrlQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            //���´򿪶�ʱ��
            StartGeneralTimer(&g_stTimerATRtnTimeOut, TIME_WAIT_SMSSEND/TIMER_UNIT_MS, NULL,NONPERIODIC);        
            return FAILED;
        }
        return SUCCEEDED;
    }
    //���´����ǲ���Ҫ���Ϳ�����Ϣ    
    else
    {
        OS_EXIT_CRITICAL();
        //���ۺ�������������һ����
        memcpy(g_stDataPackATFull.pucData1,pucATData,ulATSize);
        g_stDataPackATFull.ulData1Size = ulATSize;
        return SUCCEEDED;        
    }
    
    //test
    /*
    UINT32 cpu_sr;
    OS_ENTER_CRITICAL();    
    UsartWrite(&g_stUsart0, pstDataPack->pucData, 
            (pstDataPack->pucInPos)-(pstDataPack->pucData));
    OS_EXIT_CRITICAL();
    while(g_stUsart0.pstData->pucTxIn!=g_stUsart0.pstData->pucTxOut);
    */
    
}


/*************************************************
  Function:     DPMatch11
  Description:    ����������������ݰ�������β�ַ���ƥ��
                       ֻ���ڵ��ֽ�����ͬ����β�ַ�
                       ����ҵ�ƥ�����ֱ�ӵ�����Ӧ�������д���
                       �ú���Ŀǰʹ����CMCC��APAC��APBЭ���ƥ��
                       �ú���Ŀǰֻʹ���ڱ���ӿ�
  Calls: 
  Called By:
  Input:          pucData:         ���������
                  ulSize:            �������ݵĳ���
                  pstDataPack:  ƥ�����ݰ�����ָ��
                  ucMatch:      ����ƥ�����β�ַ�
  Output:         pstDataPack:  ƥ�����ݰ�����ָ��
                                       ��ƥ���õ������ݺ�״̬����Ϣ
                  pfFun:   ����ƥ�����������ָ��
  Return:         
  Others:
*************************************************/
void DPMatch11(DATA_PACK_ST *pstDataPack,UCHAR ucMatch,
        UCHAR *pucData,ULONG ulSize,DP_HNDL_FUNC pfFun)
{
    ULONG ulEndPos=0,ulStartPos=0; 
    for(;;)
    {
        ulStartPos = FindChar(pucData, ucMatch, 1, ulSize);
        ulEndPos = FindChar(pucData, ucMatch, 2, ulSize);
        if(pstDataPack->ucState == DATAPACK_STATE_NO)
        {
            pstDataPack->pucInPos = pstDataPack->pucData;
            //����NO S NO E
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {                    
                //ֹͣ����
                break;
            }
            //����NO S ONLY E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //��β�ַ���ͬʱ��������ǷǷ���              
                break;
            }
            //����ONLY S NO E
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                memcpy(pstDataPack->pucData, pucData+ulStartPos, ulSize-ulStartPos);
                pstDataPack->pucInPos = pstDataPack->pucInPos + ulSize-ulStartPos;
                pstDataPack->ucState= DATAPACK_STATE_START;
                break;
            }
            //ulStartPos != FIND_NO_CHAR && ulEndPos != FIND_NO_CHAR
            //����S E
            else if(ulStartPos < ulEndPos)
            {
                memcpy(pstDataPack->pucData, 
                        pucData+ulStartPos, ulEndPos - ulStartPos +1);
                pstDataPack->pucInPos = pstDataPack->pucInPos + ulEndPos - ulStartPos +1;
                //����������
                pfFun(pstDataPack);
                //�������״̬Ϊ�����ݰ� 
                pstDataPack->ucState = DATAPACK_STATE_NO;
                //����ʣ������ȣ�����β�ַ�����Ϊ��β��ͬ
                ulSize = ulSize- ulEndPos;
                //����ʣ�������                    
                memcpy(pucData, pucData+ulEndPos, ulSize);
                //��������ʣ�����ݰ�
                continue;
            }
            //��β�ַ���ͬʱ��״̬Ϊ�Ƿ�
            //����E  S
            else
            {
                break;
            }
        }//pstDataPack->ucState == DATAPACK_STATE_NO
        
        if(pstDataPack->ucState == DATAPACK_STATE_START)
        {
            //����ONLY S NO E
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //ȷ�����ݰ������
                if((pstDataPack->pucInPos + ulSize) <= 
                        ((pstDataPack->pucData) + (pstDataPack->ulDataPackMaxSize)))
                {
                    memcpy(pstDataPack->pucInPos, pucData, ulSize);
                    pstDataPack->pucInPos = pstDataPack->pucInPos+ ulSize;
                    //�������
                    break;
                }
                //����������ָ�ԭ״
                else
                {
                    pstDataPack->ucState= DATAPACK_STATE_NO;
                    pstDataPack->pucInPos = pstDataPack->pucData;
                    break;
                }
            }
            //����S E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                pstDataPack->ucState = DATAPACK_STATE_NO;
                pstDataPack->pucInPos = pstDataPack->pucData;
                break;
            }
            //����S S
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //ȷ�����ݰ������
                if((pstDataPack->pucInPos + ulStartPos + 1) <= 
                        ((pstDataPack->pucData) + (pstDataPack->ulDataPackMaxSize)))
                {            
                    //ǰ���յ��İ�S S��Ч����Ϊ��β��ͬ
                    memcpy(pstDataPack->pucInPos, pucData, ulStartPos+1);
                    pstDataPack->pucInPos =  pstDataPack->pucInPos + ulStartPos+1;
                    //����������
                    pfFun(pstDataPack);
                }
                //��Ϊ����ʣ������������ʱ��������

                //�������״̬Ϊ�����ݰ�
                pstDataPack->ucState = DATAPACK_STATE_NO;
                
                //����ʣ������ȣ�����β�ַ�����Ϊ��β��ͬ
                ulSize = ulSize- ulStartPos;
                //����ʣ�������                    
                memcpy(pucData, pucData+ulStartPos, ulSize);
                //��������ʣ�����ݰ�
                continue;
            }
            //ulStartPos != FIND_NO_CHAR && ulEndPos != FIND_NO_CHAR
            //����S S E,��Ϊ��β�ַ���ͬ������������Ч��                    
            else if(ulStartPos < ulEndPos)
            {
                //ȷ�����ݰ������
                if((pstDataPack->pucInPos + ulStartPos + 1) <= 
                        (pstDataPack->pucData + pstDataPack->ulDataPackMaxSize))
                {            
                    //ǰ���յ��İ�S S��Ч����Ϊ��β��ͬ
                    memcpy(pstDataPack->pucInPos, pucData, ulStartPos+1);
                    pstDataPack->pucInPos =  pstDataPack->pucInPos + ulStartPos+1;
                    //����������
                    pfFun(pstDataPack);
                }
                //��Ϊ����ʣ������������ʱ��������

                //ָ�븴λ
                pstDataPack->pucInPos = pstDataPack->pucData;

                //�����յ��İ�S E��Ч
                memcpy(pstDataPack->pucInPos, pucData+ulStartPos, ulEndPos-ulStartPos+1);
                pstDataPack->pucInPos =  pstDataPack->pucInPos + ulEndPos-ulStartPos+1;
                //����������
                pfFun(pstDataPack);

                //�������״̬Ϊ�����ݰ�
                pstDataPack->ucState = DATAPACK_STATE_NO;
                //����ʣ������ȣ�����β�ַ�����Ϊ��β��ͬ
                ulSize = ulSize- ulEndPos;
                //����ʣ�������                    
                memcpy(pucData, pucData+ulEndPos, ulSize);
                //��������ʣ�����ݰ�
                continue;
            }
            //����S E S,��β��ͬʱΪ�Ƿ�״̬
            else
            {
                pstDataPack->ucState = DATAPACK_STATE_NO;
                pstDataPack->pucInPos = pstDataPack->pucData;
                break;
            }
       }//pstDataPack->ucState == DATAPACK_STATE_START
    }//for(;;)
}

/*************************************************
  Function:     DPMatch11Diff
  Description:    ����������������ݰ�������β�ַ���ƥ��
                       ֻ���ڵ��ֽ��Ҳ���ͬ����β�ַ�
                       ����ҵ�ƥ�����ֱ�ӵ�����Ӧ�������д���
                       �ú���Ŀǰʹ���ڳ���Э���ƥ��
                       �ú���Ŀǰֻʹ���ڱ���ӿ�
  Calls: 
  Called By:
  Input:          pucData:         ���������
                  ulSize:            �������ݵĳ���
                  pstDataPack:  ƥ�����ݰ�����ָ��
                  ucMatch1,ucMatch2:      ����ƥ�����β�ַ�
  Output:         pstDataPack:  ƥ�����ݰ�����ָ��
                                       ��ƥ���õ������ݺ�״̬����Ϣ
                  pfFun:   ����ƥ�����������ָ��
  Return:         
  Others:
*************************************************/
void DPMatch11Diff(DATA_PACK_ST *pstDataPack,UCHAR ucMatch1, UCHAR ucMatch2,
        UCHAR *pucData,ULONG ulSize,DP_HNDL_FUNC pfFun)
{
    ULONG ulStartPos,ulEndPos;
    for(;;)
    {
        ulStartPos = FindChar(pucData, ucMatch1, 1, ulSize);
        ulEndPos = FindChar(pucData, ucMatch2, 1, ulSize);
        if(pstDataPack->ucState== DATAPACK_STATE_NO)
        {
            pstDataPack->pucInPos = pstDataPack->pucData;
            //����NO S NO E
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {                    
                //�˳�
                break;
            }
            //����NO S ONLY E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //�˳�
                break;
            }
            //����ONLY S NO E
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //��ֹS���滹��S
                *(pstDataPack->pucInPos) = ucMatch1;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                pstDataPack->ucState= DATAPACK_STATE_START;

                //����ʣ���
                ulSize = ulSize -ulStartPos -1;
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                
                continue;
            }
            //ulStartPos != FIND_NO_CHAR && ulEndPos != FIND_NO_CHAR
            //����S E
            else
            {
                //��ֹS���滹��S
                *(pstDataPack->pucInPos) = ucMatch1;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                pstDataPack->ucState= DATAPACK_STATE_START;

                //����ʣ���
                ulSize = ulSize -ulStartPos -1;
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                
                continue;
            }
        }//if(pstDataPack.ucState== DATAPACK_STATE_NO)
        
        if(pstDataPack->ucState == DATAPACK_STATE_START)
        {
            //����ONLY S NO E
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //ȷ�����ݰ������
                if((pstDataPack->pucInPos + ulSize) <= 
                        (pstDataPack->pucData + pstDataPack->ulDataPackMaxSize))
                {
                    memcpy(pstDataPack->pucInPos, pucData, ulSize);
                    pstDataPack->pucInPos = pstDataPack->pucInPos + ulSize;
                }
                else
                {
                    //ǰ���յ��İ���Ч
                    pstDataPack->ucState = DATAPACK_STATE_NO;                
                }
                //�˳�
                break;
            }
            //����S E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //ȷ�����ݰ������
                if((pstDataPack->pucInPos + ulEndPos + 1) <= 
                        (pstDataPack->pucData + pstDataPack->ulDataPackMaxSize))
                {
                    memcpy(pstDataPack->pucInPos,pucData, ulEndPos + 1);
                    pstDataPack->pucInPos = pstDataPack->pucInPos + ulEndPos + 1;
                    //����������
                    pfFun(pstDataPack);
                }

                pstDataPack->ucState = DATAPACK_STATE_NO;
                //��Ϊû��S����ʣ�����Ч
                break;
            }
            //����S S
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //ָ�븴λ
                pstDataPack->pucInPos = pstDataPack->pucData;
                //��ֹS���滹��S
                *(pstDataPack->pucInPos) = ucMatch1;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                pstDataPack->ucState= DATAPACK_STATE_START;

                //����ʣ���
                ulSize = ulSize -ulStartPos -1;
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                
                continue;
            }
            //ulStartPos != FIND_NO_CHAR && ulEndPos != FIND_NO_CHAR
            //����S S E                    
            else if(ulStartPos < ulEndPos)
            {
                //ǰ���յ��İ�S S��Ч
                //ָ�븴λ
                pstDataPack->pucInPos = pstDataPack->pucData;
                //��ֹS���滹��S
                *(pstDataPack->pucInPos) = ucMatch1;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                pstDataPack->ucState= DATAPACK_STATE_START;

                //����ʣ���
                ulSize = ulSize -ulStartPos -1;
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                
                continue;
            }
            //����S E S
            //ulEndPosǰ��İ�S EΪ��Ч��
            else if(ulStartPos > ulEndPos)
            {
                //ȷ�����ݰ������
                if((pstDataPack->pucInPos + ulEndPos + 1) <= 
                        (pstDataPack->pucData + pstDataPack->ulDataPackMaxSize))
                {
                    memcpy(pstDataPack->pucInPos, pucData, ulEndPos +1);
                    pstDataPack->pucInPos  = pstDataPack->pucInPos + ulEndPos + 1;
                    //����������
                    pfFun(pstDataPack);
                }
                
                //E SΪ��Ч���ݰ�

                //����S���������                
                //ָ�븴λ
                pstDataPack->pucInPos = pstDataPack->pucData;
                //��ֹS���滹��S
                *(pstDataPack->pucInPos) = ucMatch1;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                pstDataPack->ucState= DATAPACK_STATE_START;

                //����ʣ���
                ulSize = ulSize -ulStartPos -1;
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                
                continue;
            }
            //Ŀǰ�޴������
            else
            {
                pstDataPack->ucState = DATAPACK_STATE_NO;
                break;
            }
        }//if(pstDataPack.ucState == DATAPACK_STATE_START)
    }    
}


/*************************************************
  Function:     DPMatch22
  Description:    ����������������ݰ�������β�ַ���ƥ��
                       ֻ����˫�ֽ�����ͬ����β�ַ�
                       ����ҵ�ƥ�����ֱ�ӵ�����Ӧ�������д���
                       �ú���Ŀǰʹ��AT�����ƥ��
                       �ú���Ŀǰֻʹ���ڱ���ӿ�
  Calls: 
  Called By:
  Input:          pucData:      ���������
                  ulSize:       �������ݵĳ���
                  pstDataPack:  ƥ�����ݰ�����ָ��
                  ucMatch:      ����ƥ�����β�ַ�
  Output:         pstDataPack:  ƥ�����ݰ�����ָ��
                                ��ƥ���õ������ݺ�״̬����Ϣ
                  pfFun:        ����ƥ�����������ָ��
  Return:         
  Others:
*************************************************/
void DPMatch22(DATA_PACK_ST *pstDataPack,UCHAR ucMatch1,UCHAR ucMatch2,
        UCHAR *pucData,ULONG ulSize,DP_HNDL_FUNC pfFun)
{
    ULONG ulStartPos1=0,ulStartPos2=0; 
    for(;;)
    {
        ulStartPos1 = FindChar(pucData, ucMatch1, 1, ulSize); 
        ulStartPos2 = FindChar(pucData, ucMatch2, 1, ulSize);

        if(pstDataPack->ucState == DATAPACK_STATE_NO)
        {
            pstDataPack->pucInPos = pstDataPack->pucData;
            //����NO S1
            if(ulStartPos1 == FIND_NO_CHAR)
            {                    
                //ֹͣ����
                break;
            }
            //����ONLY S1 NO S2
            else if(ulStartPos2 == FIND_NO_CHAR)
            {
                //S1�����
                //���±�﷽ʽ������������S1
                if(pucData[ulSize -1] == ucMatch1)
                {
                    *(pstDataPack->pucInPos) = ucMatch1;
                    (pstDataPack->pucInPos)++;
                    pstDataPack->ucState = DATAPACK_STATE_START_1;
                }
                break;
            }
            //����S1 S2����һ��
            else if(ulStartPos2 == ulStartPos1 + 1)
            {
                *(pstDataPack->pucInPos) = ucMatch1;
                (pstDataPack->pucInPos)++;
                *(pstDataPack->pucInPos) = ucMatch2;
                (pstDataPack->pucInPos)++;
                pstDataPack->ucState = DATAPACK_STATE_START;
                //���S2�������һ������ʣ��������ж�
                if(ulStartPos2 != ulSize -1)
                {
                    ulSize = ulSize -ulStartPos2 -1;
                    memcpy(pucData, pucData+ulStartPos2+1, ulSize);
                    continue;
                }
                break;
            }	
            //����S1S2������һ����S1��ǰ
            else if(ulStartPos1 < ulStartPos2)
            {
                //��S2ǰһ����ʼ����,��S2ǰ�滹������S1
                ulSize = ulSize - ulStartPos2 + 1;
                memcpy(pucData, pucData+ulStartPos2-1, ulSize);
                //pstDataPack->ucState = DATAPACK_STATE_NO;	
                continue;
            }	 
            //����S1S2������һ����S2��ǰ
            else
            {
                //��S1��ʼ����,��S1���滹������S2
                ulSize = ulSize - ulStartPos1;
                memcpy(pucData, pucData+ulStartPos1, ulSize);
                //pstDataPack->ucState = DATAPACK_STATE_NO;
                continue;
            }	
        }//pstDataPack->ucState == DATAPACK_STATE_NO

        else if(pstDataPack->ucState == DATAPACK_STATE_START_1)
        {
            //����ONLY S1 NO S2
            if(ulStartPos2 == FIND_NO_CHAR)
            {                    
                //S1 �����,��Ŀǰ״̬һ��
                if(pucData[ulSize -1] == ucMatch1)
                {
                    break;                
                }
                //ֹͣ����
                pstDataPack->ucState = DATAPACK_STATE_NO;                
                break;
            }
            //����S1 S2����һ��
            else if(ulStartPos2 == 0)
            {
                *(pstDataPack->pucInPos) = ucMatch2;
                (pstDataPack->pucInPos)++;
                pstDataPack->ucState = DATAPACK_STATE_START;
                //���S2�������һ������ʣ��������ж�
                if(ulStartPos2 != ulSize -1)
                {
                    ulSize = ulSize -1;
                    memcpy(pucData, pucData+1, ulSize);
                    continue;
                }
                break;
            }
            //����ԭ�е�S1�����ڵ�S2������һ��
            else
            {
                //��S2ǰһ����ʼ����,��S2ǰ�滹������S1
                ulSize = ulSize - ulStartPos2 + 1;
                memcpy(pucData, pucData+ulStartPos2-1, ulSize);
                pstDataPack->ucState = DATAPACK_STATE_NO;
                continue;
            }
        }//pstDataPack->ucState == DATAPACK_STATE_START_1

        else if(pstDataPack->ucState == DATAPACK_STATE_START)
        {
            //����S1S2
            if((ulStartPos1 == FIND_NO_CHAR) && (ulStartPos2 == FIND_NO_CHAR))
            { 
                //ȷ�����ݰ������
                if((pstDataPack->pucInPos + ulSize) <= 
                        ((pstDataPack->pucData) + (pstDataPack->ulDataPackMaxSize)))
                {
                    //��������
                    memcpy(pstDataPack->pucInPos, pucData, ulSize);
                    pstDataPack->pucInPos = pstDataPack->pucInPos + ulSize;
                    break;
                }
                //����������ָ�ԭ״
                else
                {
                    pstDataPack->ucState= DATAPACK_STATE_NO;
                    break;
                }
            }
            //����S1S2 E1
            else if((ulStartPos1 != FIND_NO_CHAR) && (ulStartPos2 == FIND_NO_CHAR))
            {
                //���E1�������һ����Ƿ�
                if(ulStartPos1 != ulSize -1)
                {
                    //Ҫ��������S1�����
                    //���ݰ����һ����ΪS1                
                    if(pucData[ulSize -1] != ucMatch1)
                    {
                        pstDataPack->ucState = DATAPACK_STATE_NO;
                        break;
                    } 
                    //���ݰ����һ��ΪS1
                    else
                    {
                        //ָ�븴λ
                        pstDataPack->pucInPos = pstDataPack->pucData;
                        
                        *(pstDataPack->pucInPos) = ucMatch1;
                        (pstDataPack->pucInPos)++;                    
                        pstDataPack->ucState = DATAPACK_STATE_START_1;
                        break;
                    } 
                }
                //���E1�����һ��
                else
                {
                    //ȷ�����ݰ������
                    if((pstDataPack->pucInPos + ulSize) <= 
                            ((pstDataPack->pucData) + (pstDataPack->ulDataPackMaxSize)))
                    {
                        //��������
                        memcpy(pstDataPack->pucInPos, pucData, ulSize);
                        pstDataPack->pucInPos = pstDataPack->pucInPos + ulSize;
                        pstDataPack->ucState = DATAPACK_STATE_END_1;
                        break;
                    }
                    //����������ָ�ԭ״
                    else
                    {
                        pstDataPack->ucState= DATAPACK_STATE_NO;
                        break;
                    }      
                }
                break;
            }
            //����S1S2 E2
            else if((ulStartPos1 == FIND_NO_CHAR) && (ulStartPos2 != FIND_NO_CHAR))
            {
                pstDataPack->ucState = DATAPACK_STATE_NO;
                break;
            }
            //����S1S2 E1E2
            else if(ulStartPos1 == ulStartPos2 - 1)
            {
                //ȷ�����ݰ������
                if((pstDataPack->pucInPos + ulSize) <= 
                        ((pstDataPack->pucData) + (pstDataPack->ulDataPackMaxSize)))
                {
                    //����E2ǰ��İ�������������            
                    memcpy(pstDataPack->pucInPos, pucData, ulStartPos2+1); 
                    pstDataPack->pucInPos = pstDataPack->pucInPos + ulStartPos2+1;
                    pfFun(pstDataPack);
                }
                //����������ָ�ԭ״
                else
                {
                    pstDataPack->ucState= DATAPACK_STATE_NO;
                }

                //ָ�븴λ
                pstDataPack->pucInPos = pstDataPack->pucData;
                //β����
                *(pstDataPack->pucInPos) = ucMatch1;
                (pstDataPack->pucInPos)++;
                *(pstDataPack->pucInPos) = ucMatch2;
                (pstDataPack->pucInPos)++;
                pstDataPack->ucState = DATAPACK_STATE_START;
                //���E2�������һ������ʣ��������ж�
                if(ulStartPos2 != ulSize -1)
                {
                    ulSize = ulSize -ulStartPos2 -1;
                    memcpy(pucData, pucData+ulStartPos2+1, ulSize);
                    continue;
                }
                break;
            }
            //����E1E2������һ����E1��ǰ
            else if(ulStartPos1 < ulStartPos2)
            {
                //��E2ǰһ����ʼ����,��E2ǰ�滹������E1
                ulSize = ulSize - ulStartPos2 + 1;
                memcpy(pucData, pucData+ulStartPos2-1, ulSize);
                pstDataPack->ucState = DATAPACK_STATE_NO;
                continue;
            } 
            //����E1E2������һ����E2��ǰ
            else
            {
                //��E1��ʼ����,��E1���滹������E2
                ulSize = ulSize - ulStartPos1;
                memcpy(pucData, pucData+ulStartPos1, ulSize);
                pstDataPack->ucState = DATAPACK_STATE_NO;
                continue;
            }  
        }//pstDataPack->ucState == DATAPACK_STATE_START

        else if(pstDataPack->ucState == DATAPACK_STATE_END_1)
        {
            //����ONLY S1S2E1 NO E2
            if(ulStartPos2 == FIND_NO_CHAR)
            { 
                //E1 �����
                if(pucData[ulSize -1] == ucMatch1)
                {
                    //ָ�븴λ
                    pstDataPack->pucInPos = pstDataPack->pucData;                
                    *(pstDataPack->pucInPos) = ucMatch1;
                    (pstDataPack->pucInPos)++;
                    pstDataPack->ucState = DATAPACK_STATE_START_1;                
                    break;                
                }            
                //ֹͣ����
                pstDataPack->ucState = DATAPACK_STATE_NO;                
                break;
            }
            //����S1S2 E1E2
            else if(ulStartPos2 == 0)
            {
                //����������            
                *(pstDataPack->pucInPos) = ucMatch2;
                (pstDataPack->pucInPos)++;
                pfFun(pstDataPack);

                //ָ�븴λ
                pstDataPack->pucInPos = pstDataPack->pucData;
                //β����
                *(pstDataPack->pucInPos) = ucMatch1;
                (pstDataPack->pucInPos)++;
                *(pstDataPack->pucInPos) = ucMatch2;
                (pstDataPack->pucInPos)++;
                pstDataPack->ucState = DATAPACK_STATE_START;
                
                //���E2�������һ������ʣ��������ж�
                if(ulStartPos2 != ulSize -1)
                {
                    ulSize = ulSize -ulStartPos2-1;
                    memcpy(pucData, pucData+ulStartPos2+1, ulSize);
                    continue;
                }
                break;
            }
            //����ԭ�е�E1�����ڵ�E2������һ��
            else
            {
                //��E2ǰһ����ʼ����,��E2ǰ�滹������E1
                ulSize = ulSize - ulStartPos2 + 1;
                memcpy(pucData, pucData+ulStartPos2-1, ulSize);
                pstDataPack->ucState = DATAPACK_STATE_NO;
                continue;
            }
        }//pstDataPack->ucState == DATAPACK_STATE_END_1
        
    }//for(;;)
}

/*************************************************
  Function:     DPMatch31
  Description:    ����������������ݰ�������β�ַ���ƥ��
                       �������ַ�Ϊ3��β�ַ�1�����ַ���ͬ������
                       ����ҵ�ƥ����������Ӧ�������д���
                       �ú���Ŀǰʹ��������Э���ƥ��
                       �ú���Ŀǰ�ڱ���ӿں�����ӿ��о�ʹ��
  Calls: 
  Called By:
  Input:          pucData:         ���������
                  ulSize:            �������ݵĳ���
                  pstDataPack:  ƥ�����ݰ�����ָ��
                  ucMatch:      ����ƥ�����β�ַ�
  Output:     pstDataPack:  ƥ�����ݰ�����ָ��
                                       ��ƥ���õ������ݺ�״̬����Ϣ
                  pfFun:   ����ƥ�����������ָ��
  Return:         
  Others:
*************************************************/
void DPMatch31(DATA_PACK_ST *pstDataPack,UCHAR ucMatch,
        UCHAR *pucData,ULONG ulSize, DP_HNDL_FUNC pfFun)
{
    ULONG ulEndPos=0,ulStartPos=0; 
    for(;;)
    {
        ulStartPos = FindChar(pucData, ucMatch, 1, ulSize);
        ulEndPos = FindChar(pucData, ucMatch, 2, ulSize);
        if(pstDataPack->ucState == DATAPACK_STATE_NO)
        {
            pstDataPack->pucInPos = pstDataPack->pucData;
            //����NO S 
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {                    
                //ֹͣ����
                break;
            }
            //����NO S ONLY E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //��β�ַ���ͬʱ��������ǷǷ���              
                break;
            }
            //����ONLY 1��S 
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //�ж�S�ǲ��������
                if(ulStartPos == ulSize - 1)
                {
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                    pstDataPack->ucState= DATAPACK_STATE_START_1;
                }
                break;
            }
            //ulStartPos != FIND_NO_CHAR && ulEndPos != FIND_NO_CHAR
            //����2��S������һ��
            else if(ulStartPos == ulEndPos-1)
            {
                //д��2��S
                *(pstDataPack->pucInPos) = ucMatch;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                *(pstDataPack->pucInPos) = ucMatch;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;

                pstDataPack->ucState = DATAPACK_STATE_START_2;

                //����ʣ�������
                ulSize = ulSize- ulEndPos-1;
                //����ʣ�������                    
                memcpy(pucData, pucData+ulEndPos+1, ulSize);
                //��������ʣ�����ݰ�
                continue;
            }
            //����2��S�Ҳ�����һ��
            else
            {
                //д��1��S
                *(pstDataPack->pucInPos) = ucMatch;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;

                pstDataPack->ucState = DATAPACK_STATE_START_1;

                //����ʣ�������
                ulSize = ulSize- ulEndPos-1;
                //����ʣ�������                    
                memcpy(pucData, pucData+ulEndPos+1, ulSize);
                //��������ʣ�����ݰ�
                continue;
            }
        }//pstDataPack->ucState == DATAPACK_STATE_NO

        else if(pstDataPack->ucState == DATAPACK_STATE_START_1)
        {
            //����NO S 
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {                    
                //��������û���ҵ���ֹͣ����
                if(ulSize != 0)
                {
                    pstDataPack->ucState= DATAPACK_STATE_NO;
                }
                break;
            }
            //����NO S ONLY E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //��β�ַ���ͬʱ��������ǷǷ���
                pstDataPack->ucState= DATAPACK_STATE_NO;
                break;
            }
            //����ONLY 1��S 
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //�ж�S�ǲ�������ǰ
                if(ulStartPos == 0)
                {
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                    pstDataPack->ucState= DATAPACK_STATE_START_2;

                    //����ʣ�������
                    ulSize = ulSize- ulStartPos-1;
                    //����ʣ�������                    
                    memcpy(pucData, pucData+ulStartPos+1, ulSize);
                    //��������ʣ�����ݰ�
                    continue;                    
                }
                //�ж�S�ǲ��������
                else if(ulStartPos == ulSize - 1)
                {
                    //ָ�븴λ
                    pstDataPack->pucInPos = pstDataPack->pucData;
                    
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                    pstDataPack->ucState= DATAPACK_STATE_START_1;
                }
                else
                {
                    pstDataPack->ucState= DATAPACK_STATE_NO;
                }                
                break;
            }
            //ulStartPos != FIND_NO_CHAR && ulEndPos != FIND_NO_CHAR
            //����2��S������һ��
            else if(ulStartPos == ulEndPos-1)
            {
                //S�ڿ�ͷ
                if(ulStartPos == 0)
                {
                    //д��2��S
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;                    
                    pstDataPack->ucState= DATAPACK_STATE_START;

                    //����ʣ�������
                    ulSize = ulSize- ulEndPos-1;
                    //����ʣ�������                    
                    memcpy(pucData, pucData+ulEndPos+1, ulSize);
                    //��������ʣ�����ݰ�
                    continue;                
                }
                //S���ڿ�ͷ������һ��
                else
                {
                    //ָ�븴λ
                    pstDataPack->pucInPos = pstDataPack->pucData;                
                    //д��2��S
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;

                    pstDataPack->ucState = DATAPACK_STATE_START_2;

                    //����ʣ�������
                    ulSize = ulSize- ulEndPos-1;
                    //����ʣ�������                    
                    memcpy(pucData, pucData+ulEndPos+1, ulSize);
                    //��������ʣ�����ݰ�
                    continue;
                }
            }
            //����2��S�Ҳ�����һ��
            else
            {
                //ָ�븴λ
                pstDataPack->pucInPos = pstDataPack->pucData;             
                //д��1��S
                *(pstDataPack->pucInPos) = ucMatch;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;

                pstDataPack->ucState = DATAPACK_STATE_START_1;

                //����ʣ�������
                ulSize = ulSize- ulEndPos-1;
                //����ʣ�������                    
                memcpy(pucData, pucData+ulEndPos+1, ulSize);
                //��������ʣ�����ݰ�
                continue;
            }
        }//else if(pstDataPack->ucState == DATAPACK_STATE_NO)

        else if(pstDataPack->ucState == DATAPACK_STATE_START_2)
        {
            //����NO S 
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {                    
                //��������û���ҵ���ֹͣ����
                if(ulSize != 0)
                {
                    pstDataPack->ucState= DATAPACK_STATE_NO;
                }
                break;
            }
            //����NO S ONLY E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //��β�ַ���ͬʱ��������ǷǷ���
                pstDataPack->ucState= DATAPACK_STATE_NO;
                break;
            }
            //����ONLY 1��S 
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //�ж�S�ǲ�������ǰ
                if(ulStartPos == 0)
                {
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                    pstDataPack->ucState= DATAPACK_STATE_START;

                    //����ʣ�������
                    ulSize = ulSize- ulStartPos-1;
                    //����ʣ�������                    
                    memcpy(pucData, pucData+ulStartPos+1, ulSize);
                    //��������ʣ�����ݰ�
                    continue;                    
                }
                //�ж�S�ǲ��������
                else if(ulStartPos == ulSize - 1)
                {
                    //ָ�븴λ
                    pstDataPack->pucInPos = pstDataPack->pucData;
                    
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                    pstDataPack->ucState= DATAPACK_STATE_START_1;
                }
                else
                {
                    pstDataPack->ucState= DATAPACK_STATE_NO;
                }                
                break;
            }
            //ulStartPos != FIND_NO_CHAR && ulEndPos != FIND_NO_CHAR
             //S�ڿ�ͷ
            else if(ulStartPos == 0)
            {
                //д��1��S
                *(pstDataPack->pucInPos) = ucMatch;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                 
                pstDataPack->ucState= DATAPACK_STATE_START;

                //����ʣ�������
                ulSize = ulSize- ulStartPos-1;
                //����ʣ�������                    
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                //��������ʣ�����ݰ�
                continue;                
            }
            //S���ڿ�ͷ
            else
            {
                    //ָ�븴λ
                    pstDataPack->pucInPos = pstDataPack->pucData;                
                    //д��1��S
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;

                    pstDataPack->ucState = DATAPACK_STATE_START_1;

                    //����ʣ�������
                    ulSize = ulSize- ulStartPos-1;
                    //����ʣ�������                    
                    memcpy(pucData, pucData+ulStartPos+1, ulSize);
                    //��������ʣ�����ݰ�
                    continue;
            }
        }//else if(pstDataPack->ucState == DATAPACK_STATE_START_2)

        else if(pstDataPack->ucState == DATAPACK_STATE_START)
        {
            //����NO S 
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //ȷ�����ݰ������
                if((pstDataPack->pucInPos + ulSize) <= 
                        ((pstDataPack->pucData) + (pstDataPack->ulDataPackMaxSize)))
                {            
                    memcpy(pstDataPack->pucInPos,pucData,ulSize);
                    pstDataPack->pucInPos = pstDataPack->pucInPos + ulSize;
                }
                else
                {
                    pstDataPack->ucState= DATAPACK_STATE_NO;
                }
                break;
            }
            //����NO S ONLY E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //��β�ַ���ͬʱ��������ǷǷ���
                pstDataPack->ucState= DATAPACK_STATE_NO;
                break;
            }
            //����ulStartPos != FIND_NO_CHAR
            //��E֮ǰ����������
            else if((ulStartPos != 0)||(pstDataPack->pucInPos-pstDataPack->pucData>3))
            {
                //����������
                //ȷ�����ݰ������
                if((pstDataPack->pucInPos + ulStartPos+1) <= 
                        ((pstDataPack->pucData) + (pstDataPack->ulDataPackMaxSize)))
                {                 
                    memcpy(pstDataPack->pucInPos,pucData,ulStartPos+1);
                    pstDataPack->pucInPos = pstDataPack->pucInPos + ulStartPos+1;
                    pfFun(pstDataPack);
                }

                //����S,��Ϊ��β�ַ���ͬ
                //ָ�븴λ
                pstDataPack->pucInPos = pstDataPack->pucData;
                    
                *(pstDataPack->pucInPos) = ucMatch;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                pstDataPack->ucState= DATAPACK_STATE_START_1;

                //����ʣ�������
                ulSize = ulSize- ulStartPos-1;
                //����ʣ�������                    
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                //��������ʣ�����ݰ�
                continue;
            }            
            //��E֮ǰû����������,˵���������ֳ���3��AA
            else
            {
                //״̬���䣬��ȻΪDATAPACK_STATE_START
                //����ʣ�������
                ulSize = ulSize- ulStartPos-1;
                //����ʣ�������                    
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                //��������ʣ�����ݰ�
                continue;                
            }
        }//else if(pstDataPack->ucState == DATAPACK_STATE_START)        
        
    }//for(;;)
}

/*************************************************
  Function:       CHSthSendReady
  Description:    �������ж�ͨ�Žӿ��Ƿ����
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊͨ�Žӿڵ�ͨ�Ŷ���

  Output:         TRUE:����FALSE:æ     
  Return:         
  Others:
*************************************************/
BOOL CHSendReady(COMM_OBJ_ST *pstCOMMObj)
{
    //�жϺ���ͨ�Ŷ���
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {    
        //�ж����򴮿��Ƿ�������δ����
        if(UsartSendReady ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev)))
        {
            //���������ݷ������ٸ��ݽӿ���������һ���ж�
            switch(pstCOMMObj->ucInterfaceType)
            {
            //���������������ʽ��ͬ����Ϊ����ӿڲ���Ҫ�ȴ���ѯ����
            case USART_SLAVE_485:            
            case USART_MODEM_OR_DIRECT:
            case USART_SLAVE_OPT:
                if(pstCOMMObj->ucState == COMM_STATE_SEND_IDLE)
                {
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }                
                break;
            default:
                return TRUE;
                break;  
            }
        }
        else
        {
            return FALSE;
        }
    }
}


/*************************************************
  Function:       HndlCHSthRec
  Description:    ��������������ӿ��յ�������
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���

  Output:     
  Return:         
  Others:
*************************************************/
void HndlCHSthRec(COMM_OBJ_ST *pstCOMMObj)
{
    UINT32  cpu_sr;
    ULONG ulCount = 0;
    UCHAR aucRecToMatch[USART_SIZE_BUFFER];//����ĳ�����ݰ�ƥ��
    //�жϺ���ͨ�Ŷ���
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {    
        //�����򴮿�
        if(UsartReadReady ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev)))
        {
            //notice�������ճ�ʱ��ʱ��            
            StartGeneralTimer(&g_stTimerSthRecTimeOut, TIME_TIMEOUT_USARTREC/TIMER_UNIT_MS, NULL,NONPERIODIC);

            //��������
            OS_ENTER_CRITICAL();
            ulCount = UsartRead ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev), aucRecToMatch);
            OS_EXIT_CRITICAL();

            //������ݽӿ����Ͷ����ݽ��д���
            //��Ϊ����ӿ�ֻ���յ�����Э��

            //ƥ������Э��             
            DPMatch31(&g_stDataPackSthMS, MS_START_END_FLAG, 
                    aucRecToMatch, ulCount, (DP_HNDL_FUNC)DPSthRecMSHndl);
        }
    }
}

/*************************************************
  Function:     HndlCHSthSend
  Description:    ��������������ӿڷ��͵�����
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���
                     pvPrmv:              ���յ�����Ϣָ��
  Output:     
  Return:         
  Others:       ���ж�ͨ�Ŷ������ж�ͨ�Žӿ�����ж�ԭ������
*************************************************/
void HndlCHSthSend(COMM_OBJ_ST *pstCOMMObj, void * pvPrmv)
{
    OHCH_SEND_DATA_REQ_ST *pstOHCHDataReq = (OHCH_SEND_DATA_REQ_ST *)pvPrmv;   
    MESSAGE_ST *pstMessage = (MESSAGE_ST *)pvPrmv;
    //�жϺ���ͨ�Ŷ���
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {
        //����Ǵ��豸�����ӹ�˻�(�ж�ͨ�Žӿ�)
        if(pstCOMMObj->ucInterfaceType == USART_SLAVE_OPT ||
            pstCOMMObj->ucInterfaceType == USART_SLAVE_485)    
        {
            UCHAR aucData[USART_SIZE_BUFFER];
            ULONG ulSize;
            UCHAR ucFlag = 0;

            //д���־λ��Ŀǰ����ͨ��״̬��Ϣ
            //д��ͨ��״̬
            ucFlag = ucFlag & 0xF9;//BIT2:1=00
            switch(g_ucCurrCommMode)
            {
            case COMM_NO_CONN:
                break;//BIT2:1=00
            case COMM_MODE_CSD:
                ucFlag = ucFlag | 0x02;//BIT2:1=01
                break;
            case COMM_MODE_GPRS:
                ucFlag = ucFlag | 0x04;//BIT2:1=10
                break;
            default:
                break;
            }
           
            //����ԭ�����ͽ��д���
            switch(*((ULONG*)pvPrmv))
            {
            //�����Ǵ��豸���Զ�����ԭ�����ʹ���һ��
            //������Э������ֱ�ӷ���
            case OHCH_SEND_DATA_REQ:
            case OHCH_DEBU_SEND_REQ:
            case OHCH_UNDE_SEND_REQ:                
                //д����Ϣ����
                memcpy(aucData,pstOHCHDataReq,sizeof(*pstOHCHDataReq));
                memcpy(aucData+sizeof(*pstOHCHDataReq), 
                        pstOHCHDataReq->pucData, pstOHCHDataReq->ulDataLen);
                ulSize = sizeof(*pstOHCHDataReq) + pstOHCHDataReq->ulDataLen;
                

                //����Ϣ���ݽ�������Э��ı���,�Թ㲥��ʽ����
                ulSize = EncodeMSProtocol(MS_BROADCAST_DEVNUM, MS_CMD_TAKEOHTER,ucFlag, 
                        aucData, ulSize);
                
                //��������
                SthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                //notice��ԭ������ͳһ
                //pstOHCHDataReq->ulPrmvType = OHCH_SEND_DATA_CNF;
                
                //��Ҫ��ȷ����Ϣ
                MyFree(pvPrmv);
                pvPrmv = NULL;
                break;

            case ACCH_MESS_SEND_REQ:
                //��Ҫ��ȷ����Ϣ�Ҳ���Ҫ���ͣ���Ϊ�ⲻ�����һ��AC��Ϣ��������Ҳû������
                //������˵�������һ��
                if(g_ulACCHSendMessageNum != 1)
                {
                    if(g_ulACCHSendMessageNum > 1)
                    {
                        g_ulACCHSendMessageNum--;
                    }
                    MyFree(pvPrmv);
                    pvPrmv = NULL;
                    break;
                }
                //�����һ������з���(ͬOHCH_MESS_SEND_REQ����)
                g_ulACCHSendMessageNum = 0;
            case OHCH_MESS_SEND_REQ:
            case MESS_QUERY_REQ:
            case MESS_QUERY_RSP:
            case MESS_SET_REQ:
            case MESS_SET_RSP:
            case MESS_ALARM_REP:
                //д����Ϣ����
                ulSize = pstMessage->usDataLen + 8 ;
                memcpy(aucData,pstMessage,ulSize);

                //����Ϣ���ݽ�������Э��ı���,�Թ㲥��ʽ����
                ulSize = EncodeMSProtocol(MS_BROADCAST_DEVNUM, MS_CMD_TAKEOHTER,ucFlag, 
                        aucData, ulSize);
                
                //��������
                SthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                //��Ҫ��ȷ����Ϣ
                MyFree(pvPrmv);
                pvPrmv = NULL;
                break;
                
            default:
                //�����޷�ȷ�ϵ��ͷ��ڴ�
                MyFree(pvPrmv);
                pvPrmv = NULL;
                break;               
            }//switch(pstOHCHDataReq->ulPrmvType)
            
            //��ȷ�ϰ�
            if(pvPrmv != NULL)
            {
                if(OSQPost(g_pstOHQue, pvPrmv) == OS_Q_FULL)
                {
                    MyFree(pvPrmv);
                }
            }
            
        }//if(pstCOMMObj->ucInterfaceType == USART_SLAVE)        

        //Ŀǰ����ӿ������ֽӿ�
        else if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)
        {
        }//else if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)


    }//if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
}

/*************************************************
  Function:     HndlCHDebSend
  Description:    ��������������ӿڷ��͵�����
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���
                     pvPrmv:              ���յ�����Ϣָ��
  Output:     
  Return:         
  Others:       ���ж�ͨ�Ŷ������ж�ͨ�Žӿ�����ж�ԭ������
*************************************************/
void HndlCHDebSend(COMM_OBJ_ST *pstCOMMObj, void * pvPrmv)
{
    OHCH_SEND_DATA_REQ_ST *pstOHCHDataReq = (OHCH_SEND_DATA_REQ_ST *)pvPrmv;   
    //�жϺ���ͨ�Ŷ���
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {            
        //����ԭ�����ͽ��д���
        switch(*((ULONG*)pvPrmv))
        {
        //��Ҫ����ԭ������
            case OHCH_DEBU_SEND_REQ:
                if(NthSendtoMorD(pstCOMMObj,pvPrmv)==SUCCEEDED)
                {
                    pstOHCHDataReq->cResult= SUCCEEDED;
                }
                else
                {
                    pstOHCHDataReq->cResult= FAILED;                
                } 
                break;//case OHCH_DEBU_SEND_REQ              
            default:
                break;
        }//switch(pstOHCHDataReq->ulPrmvType)
        //����Ҫ��ȷ�ϰ�
        MyFree(pvPrmv);
    }//if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
}


/*************************************************
  Function:       HndlCHNthRec
  Description:    ������������ӿ��յ�������
                  �����������Ͻ�����Ӧ��Ϣ����
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���

  Output:     
  Return:         
  Others:
*************************************************/
void HndlCHNthRec(COMM_OBJ_ST *pstCOMMObj)
{
    UINT32  cpu_sr;
    ULONG ulCount = 0;
    UCHAR aucRec[USART_SIZE_BUFFER];//���ڱ������������
    UCHAR aucRecToMatch[USART_SIZE_BUFFER];//����ĳ�����ݰ�ƥ��
    //�жϺ���ͨ�Ŷ���
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {    
        //�����򴮿�
        if(UsartReadReady ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev)))
        {
            //�������ճ�ʱ��ʱ��
            StartGeneralTimer(&g_stTimerNthRecTimeOut, TIME_TIMEOUT_USARTREC/TIMER_UNIT_MS, NULL,NONPERIODIC);

            //��������
            OS_ENTER_CRITICAL();
            ulCount = UsartRead ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev), aucRec);
            
            //test�ѱ��򴮿��յ������ݴӵ��Դ����з�����Ϊ�˹۲�modem������������
            //UsartWrite ((USART_DEV_ST*)(g_stDebugInterface.pvCOMMDev),
              //      aucRec, ulCount);
            
            OS_EXIT_CRITICAL();

            //���ݽӿ����Ͷ����ݽ��д���
            switch(pstCOMMObj->ucInterfaceType)
            {
            case USART_MODEM_OR_DIRECT:
                //ƥ��AT�����ֱ������
                memcpy(aucRecToMatch,aucRec,ulCount);
                DPMatch11(&g_stDataPackCMCCAPAC, APAC_FLAG, aucRecToMatch, ulCount, (DP_HNDL_FUNC)DPHandInCSDorGPRS);
                memcpy(aucRecToMatch,aucRec,ulCount);                
                DPMatch11(&g_stDataPackCMCCAPB, APB_FLAG, aucRecToMatch, ulCount,(DP_HNDL_FUNC)DPHandInCSDorGPRS);
                memcpy(aucRecToMatch,aucRec,ulCount);
                DPMatch22(&g_stDataPackAT, 0x0D, 0x0A, aucRecToMatch, ulCount,(DP_HNDL_FUNC)DPHandInATCMD);
                memcpy(aucRecToMatch,aucRec,ulCount);
                DPMatch11Diff(&g_stDataPackPrivate, YKPP_BEGIN_FLAG, YKPP_END_FLAG,
                        aucRecToMatch, ulCount,(DP_HNDL_FUNC)DPHandInCSDorGPRS);                
                break;
            //���ڶ���˵���������������ʽ��ͬ
            case USART_SLAVE_485:
            case USART_SLAVE_OPT:
                //ƥ������Э��            
                memcpy(aucRecToMatch,aucRec,ulCount);
                DPMatch31(&g_stDataPackNthMS, MS_START_END_FLAG, 
                        aucRecToMatch, ulCount, (DP_HNDL_FUNC)DPNthRecMSHndl);
                break;
            default:
                break;   
            }
        }
    }
}

/*************************************************
  Function:       HndlCHDebRec
  Description:    ������������Խӿ��յ�������
                  �����������Ͻ�����Ӧ��Ϣ����
  Calls:
  Called By:
  Input:          pstCOMMObj:         ͨ�Ŷ���

  Output:     
  Return:         
  Others:
*************************************************/
void HndlCHDebRec(COMM_OBJ_ST *pstCOMMObj)
{
    UINT32  cpu_sr;
    ULONG ulCount = 0;
    UCHAR aucRec[USART_SIZE_BUFFER];//���ڱ������������
    UCHAR aucRecToMatch[USART_SIZE_BUFFER];//����ĳ�����ݰ�ƥ��
    //�жϺ���ͨ�Ŷ���
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {    
        if(UsartReadReady ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev)))
        {
            //�������ճ�ʱ��ʱ��
            StartGeneralTimer(&g_stTimerDebRecTimeOut, TIME_TIMEOUT_USARTREC/TIMER_UNIT_MS, NULL,NONPERIODIC);

            //��������
            OS_ENTER_CRITICAL();
            ulCount = UsartRead ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev), aucRec);
            OS_EXIT_CRITICAL();
            
            //ֻƥ��ֱ������
            memcpy(aucRecToMatch,aucRec,ulCount);
            DPMatch11(&g_stDataPackDebAPAC, APAC_FLAG, aucRecToMatch, ulCount, (DP_HNDL_FUNC)DPHandInDeb);
            memcpy(aucRecToMatch,aucRec,ulCount);
            DPMatch11Diff(&g_stDataPackDebPrivate, YKPP_BEGIN_FLAG, YKPP_END_FLAG,
                    aucRecToMatch, ulCount,(DP_HNDL_FUNC)DPHandInDeb);          

        }
    }
}

/*************************************************
  Function:     CHNthSendReady
  Description:  �������жϱ���ӿ��Ƿ����
  Calls:
  Called By:
  Input:        pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���

  Output:       TRUE:����FALSE:æ     
  Return:         
  Others:
*************************************************/
/*
BOOL CHNthSendReady(COMM_OBJ_ST *pstCOMMObj)
{
    //�жϺ���ͨ�Ŷ���
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {    
        //�жϱ��򴮿��Ƿ�������δ����
        if(UsartSendReady ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev)))
        {
            //���������ݷ������ٸ��ݽӿ���������һ���ж�
            switch(pstCOMMObj->ucInterfaceType)
            {
            //���������������ʽ��ͬ���ȵ���ѯ��ɷ���
            case USART_SLAVE_485:            
            case USART_MODEM_OR_DIRECT:
                if(pstCOMMObj->ucState == COMM_STATE_SEND_IDLE)
                {
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
                break;
            //������ùع�˻�ʹ�ܷ�ʽ1����3���������ͬ
            case USART_SLAVE_OPT:
                //test����������ʱ���رչ�˻�ʹ��,��ʹ�ܷ�ʽ2
                //Ŀǰ���ù�ʹ�ܷ�ʽ1
                if(pstCOMMObj->ucState == COMM_STATE_SEND_IDLE)
                {
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }                
                break;
            default:
                return TRUE;
                break;  
            }
        }
        else
        {
            return FALSE;
        }
    }
}
*/
/*************************************************
  Function:       EncodeMSProtocol
  Description:    �����������ݽ�������Э��ı���
                  ��������ԭ���ڴ��Ͻ��б���
  Calls:
  Called By:
  Input:          ucSNum:         �ӻ����
                  ucCMD:          ���Ԫ
                  ucFlag:         ��־��Ԫ
                  pucData,ulSize: Ҫ���������ָ��ʹ�С
                  ulSizeΪ0��˵��û�����ݵ�Ԫ
                  pucData����ʱ��Ҫ�����������ݵ�Ԫָ��
                  pucData���ʱ�Ǳ���������ָ��
  Output:     
  Return:         ���������ݳ���        
  Others:
*************************************************/
ULONG EncodeMSProtocol(UCHAR ucSNum,UCHAR ucCMD,UCHAR ucFlag,UCHAR *pucData, ULONG ulSize)
{
    USHORT usCrc;
    //�ж��Ƿ������ݵ�Ԫ
    if(ulSize != 0)
    {
        //�������ݵ�Ԫ���������д���
        for(ULONG i=ulSize; i>0 ;i--)
        {
            //ǰ������6�ֽ����ڴ��3��AA,Э������,�ӻ����,������,��־��Ԫ
            *(pucData + i - 1 + 7) = *(pucData + i - 1);
        }
    }
    ///3��AA,Э������,�ӻ����,������,��־��Ԫ
    pucData[0] = MS_START_END_FLAG;
    pucData[1] = MS_START_END_FLAG;
    pucData[2] = MS_START_END_FLAG;
    pucData[3] = MS_PROTOCOL_TYPE;
    pucData[4] = ucSNum;
    pucData[5] = ucCMD;
    pucData[6] = ucFlag;    
    
    //crcУ��
    //��Э�����Ϳ�ʼ
    usCrc = CalcuCRC(pucData+3, ulSize+4);
    SET_WORD(pucData+ulSize+7, usCrc);

    //ת��
    ulSize = EncodeEscape(pucData+3, ulSize+6, USART_SIZE_BUFFER, MS_ESCAPE_CHAR, 
            g_aucMSEscaped, g_aucMSUnEscape, 2);

    //���1��AA
    pucData[ulSize+3] = MS_START_END_FLAG;
    ulSize = ulSize + 4;
    return ulSize;
}

/*************************************************
  Function:       ResetModem
  Description:    ����������modem
  Calls:
  Called By:
  Input:          
  Output:     
  Return:            
  Others:
*************************************************/
void ResetModem(void)
{
    //�������������ӳ������͹ܽ�
    PioOpen(&MODEMRESET_PIO_CTRL,MODEM_RESET_PIN,PIO_OUTPUT);
    PioWrite(&MODEMRESET_PIO_CTRL,MODEM_RESET_PIN,1);
    MyDelay(50);
    PioWrite(&MODEMRESET_PIO_CTRL,MODEM_RESET_PIN,0);
}

/*************************************************
  Function:       SthSendtoOPTor485
  Description:    ������������ӿڷ��͹�˻�����
                  ����������ȷ��ͨ�Žӿڵ�����±�����
                  �������������豸������ӿڷ�������
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���
                  pucData,ulSize:      Ҫ���͵�����ָ��ʹ�С
  Output:     
  Return:         
  Others:
*************************************************/
void SthSendtoOPTor485(COMM_OBJ_ST *pstCOMMObj, UCHAR *pucData, ULONG ulSize)
{
    UINT32 cpu_sr;
    //UCHAR ucState;

    //�ȴ�����ӿ���,��ʹ�ܷ�ʽ1  
    //ucState = pstCOMMObj->ucState;//���浱ǰͨ��״̬
    //pstCOMMObj->ucState = COMM_STATE_SEND_IDLE;
    //test���������Ϣ����Ҳ����sendready�����Բ���˶κ���
    //testͬʱҲ����Ҫ���浱ǰͨ��״̬
    /*
    while(CHSthSendReady(pstCOMMObj)!=TRUE)
    {
        MyDelay(10);
        //��������
        //test��Ҫ��������ӿڵĽ��պͱ���ӿڵĽ��շ���
        HndlCHSthRec(pstCOMMObj);
    }
    */
    //�����485����Ҫ�򿪷���ʹ��
    if(pstCOMMObj->ucInterfaceType == USART_SLAVE_485)
    {
        MyDelay(20);
        PioWrite(&U485_PIO_CTRL,((USART_DEV_ST*)(pstCOMMObj->pvCOMMDev))->uiU485Enable,
            U485_ENABLE_ON);
    }
    //��������
    OS_ENTER_CRITICAL();
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            pucData, ulSize);
    OS_EXIT_CRITICAL();
    //�����485����Ҫ�رշ���ʹ��
    if(pstCOMMObj->ucInterfaceType == USART_SLAVE_485)
    {
        //notice�ȴ�������Ϲرշ���ʹ��
        UCHAR ucState = pstCOMMObj->ucState;//���浱ǰͨ��״̬
        pstCOMMObj->ucState = COMM_STATE_SEND_IDLE;
        while(CHSendReady(pstCOMMObj)!=TRUE)
        {
            MyDelay(10);
            //��������
            //notice��Ҫ��������ӿڵĽ��պͱ���ӿڵĽ���
            HndlCHNthRec(pstCOMMObj);
            HndlCHSthRec(&g_stSouthInterface);
        }
        pstCOMMObj->ucState = ucState;
        MyDelay(20);

        //�ر�485����ʹ��
        PioWrite(&U485_PIO_CTRL,((USART_DEV_ST*)(pstCOMMObj->pvCOMMDev))->uiU485Enable,
            U485_ENABLE_OFF);
    }
    //�ָ���ǰͨ��״̬
    //pstCOMMObj->ucState = ucState;     

}

/*************************************************
  Function:       NthSendtoOPTor485
  Description:    ����������ӿڷ��͹�˻�����
                  ����������ȷ��ͨ�Žӿڵ�����±�����
                  �������������豸�ı���ӿڷ�������
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���
                  pucData,ulSize:      Ҫ���͵�����ָ��ʹ�С
  Output:     
  Return:         
  Others:
*************************************************/
void NthSendtoOPTor485(COMM_OBJ_ST *pstCOMMObj, UCHAR *pucData, ULONG ulSize)
{
    UINT32 cpu_sr;
    UCHAR ucState;

    //�����485����Ҫ�򿪷���ʹ��
    if(pstCOMMObj->ucInterfaceType == USART_SLAVE_485)
    {
        //�򿪷���ʹ��
        PioWrite(&U485_PIO_CTRL,((USART_DEV_ST*)(pstCOMMObj->pvCOMMDev))->uiU485Enable,
            U485_ENABLE_ON);
             
        //��ʱ
        MyDelay(20);            

        OS_ENTER_CRITICAL();
        UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
                pucData, ulSize);
        OS_EXIT_CRITICAL();
           
        //notice�ȴ�������Ϲرշ���ʹ��
        ucState = pstCOMMObj->ucState;//���浱ǰͨ��״̬
        pstCOMMObj->ucState = COMM_STATE_SEND_IDLE;
        while(CHSendReady(pstCOMMObj)!=TRUE)
        {
            MyDelay(10);
            //��������
            //notice��Ҫ��������ӿڵĽ��շ��ͺͱ���ӿڵĽ���
            HndlCHNthRec(pstCOMMObj);
            HndlCHSthRec(&g_stSouthInterface);
            CHSthReadyAndSend();
        }
        pstCOMMObj->ucState = ucState;
        MyDelay(20);
    
        //�رշ���ʹ��
        PioWrite(&U485_PIO_CTRL,((USART_DEV_ST*)(pstCOMMObj->pvCOMMDev))->uiU485Enable,
            U485_ENABLE_OFF);
    }

    //��˻�
    else
    {
        //notice��˻���ʹ��
        PioWrite(&OPT_PIO_CTRL,OPT_ENABLE,OPT_ENABLE_ON);
             
        //��ʱ
        MyDelay(40);
            
        //����00����
        UCHAR ucTemp=0;
        OS_ENTER_CRITICAL();
        UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
                &ucTemp, 1);    
        UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
                pucData, ulSize);
        OS_EXIT_CRITICAL();
           
        //notice�ȴ�������Ϲ�˻��ر�ʹ�� ,��ʹ�ܷ�ʽ1
        ucState = pstCOMMObj->ucState;//���浱ǰͨ��״̬
        pstCOMMObj->ucState = COMM_STATE_SEND_IDLE;
        while(CHSendReady(pstCOMMObj)!=TRUE)
        {
            MyDelay(10);
            //��������
            //notice��Ҫ��������ӿڵĽ��շ��ͺͱ���ӿڵķ���
            HndlCHNthRec(pstCOMMObj);
            HndlCHSthRec(&g_stSouthInterface);
            CHSthReadyAndSend();
        }
        pstCOMMObj->ucState = ucState;
        MyDelay(10);
    
        //notice��˻��ر�ʹ��
        PioWrite(&OPT_PIO_CTRL,OPT_ENABLE,OPT_ENABLE_OFF);
    }
}

/*************************************************
  Function:       NthSendtoMorD
  Description:    ����������ӿڻ��ߵ��Խӿڷ���MODEM����ֱ������
                  ����������ȷ��ͨ�Žӿڵ�����±�����
                  �������������豸�ı�����ߵ��Խӿڷ�������
                  ���Խӿ�ֻ�ᷢCSD����
  Calls:
  Called By:
  Input:           pstCOMMObj:         ��Ϊͨ�Žӿڵ�ͨ�Ŷ���
                   pvPrmv:              ���յ�����Ϣָ��
  Output:     
  Return:         
  Others:
*************************************************/
LONG NthSendtoMorD(COMM_OBJ_ST *pstCOMMObj, void * pvPrmv)
{
    OHCH_SEND_DATA_REQ_ST *pstOHCHDataReq = (OHCH_SEND_DATA_REQ_ST *)pvPrmv;
    UINT32  cpu_sr;
    LONG lSucceeded = FAILED;

    //noticeͨ��״̬ΪGPRS��Ҫ���Ͷ���Ϣ
    //�����ִ���ʽ:ֱ���ϱ�ʧ�ܻ����л���ʽ������ 

    //Ҫ���Ͷ���Ϣ
    if(pstOHCHDataReq->ucCommMode == COMM_MODE_SMS)       
    {
        //notice
        if(g_ucCurrCommMode != COMM_MODE_SMS)
        {
            return FAILED;
        }
    
        //��װAT�������
        SendSMSInPDUMode(pstCOMMObj,pstOHCHDataReq->acDstTelNum, pstOHCHDataReq->pucData,
                pstOHCHDataReq->ulDataLen, pstOHCHDataReq->ucDCS, pstOHCHDataReq->ucDA,NULL);

        //�жϽ��
        if(AtRtnIsOk(pstCOMMObj, TIME_WAIT_SMSSEND, CH_SENDAT_NORMAL) == TRUE)
        {
            lSucceeded = SUCCEEDED;
        }
        else
        {
            lSucceeded = FAILED;
            //noticΪ�˶���ѹ��
            /*
            for(UINT32 i=0;i<200;i++)
            {
                MyDelay(50);
                HndlCHNthRec(pstCOMMObj);
                HndlCHSthRec(&g_stSouthInterface);
            }
            */
        }   
    }

    //Ҫ����ֱ������
    else if(pstOHCHDataReq->ucCommMode == COMM_MODE_CSD)       
    {
        //notice
        if(g_ucCurrCommMode == COMM_MODE_GPRS)
        {
            return FAILED;
        }
        //��������
        OS_ENTER_CRITICAL();
        UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
                pstOHCHDataReq->pucData, pstOHCHDataReq->ulDataLen);
        OS_EXIT_CRITICAL();
        lSucceeded = SUCCEEDED;
    }

    //Ҫ����GPRS����
    else if(pstOHCHDataReq->ucCommMode == COMM_MODE_GPRS)
    {
        //���ͨ��״̬������
        if(g_ucCurrCommMode != COMM_MODE_GPRS)
        {
            lSucceeded = FAILED;
        }
        else
        {
            //ת�崦��Ҫ�����ڴ棬�����ƻ�ԭ����
            //��Ϊԭ����Ҫ�ظ�OH��
            UCHAR *pucMem;
            ULONG ulSize = pstOHCHDataReq->ulDataLen;
            pucMem = (UCHAR *) MyMalloc(g_pstMemPool512);
            if(pucMem == (void*)NULL)
            {
                lSucceeded = FAILED;
            }
            else
            {
                //��������
                memcpy((UCHAR*)pucMem,pstOHCHDataReq->pucData,ulSize);
                //����ת��
                ulSize = EncodeEscape((UCHAR*)pucMem, ulSize, USART_SIZE_BUFFER, 
                        GPRS_ESCAPE_CHAR, g_aucGPRSEscaped, g_aucGPRSUnEscape, 2);
                //��������
                OS_ENTER_CRITICAL();
                UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
                        (UCHAR*)pucMem, ulSize);
                OS_EXIT_CRITICAL();
                MyFree(pucMem);
                lSucceeded = SUCCEEDED;                
            }        
        }
    }//else if(pstOHCHDataReq->ucCommMode == COMM_MODE_GPRS)    
    
    return lSucceeded;
}

/*************************************************
  Function:       HndlCHNthSend
  Description:    ������������ӿڷ��͵�����
                  Ŀǰֻ����CH�㷢�������ݻ���
                  ����ӿ�ת��������(��ͬ��CH�㷢��������)

  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���
                  pvPrmv:             ���յ�����Ϣָ��
  Output:     
  Return:         
  Others:         ���ж�ͨ�Ŷ������ж�ͨ�Žӿ�����ж�ԭ������
*************************************************/
void HndlCHNthSend(COMM_OBJ_ST *pstCOMMObj, void * pvPrmv)
{
    OHCH_SEND_DATA_REQ_ST *pstOHCHDataReq = (OHCH_SEND_DATA_REQ_ST *)pvPrmv;
    MESSAGE_ST *pstMessage = (MESSAGE_ST *)pvPrmv;
    OHCH_CONN_CENTER_REQ_ST *pstOHCHConnCenterReq = (OHCH_CONN_CENTER_REQ_ST *)pvPrmv;
    OHCH_GPRS_DISCONN_REQ_ST *pstOHCHGprsDisconnReq = (OHCH_GPRS_DISCONN_REQ_ST *)pvPrmv;
    OHCH_SLAVE_UNREG_REQ_ST *pstOHCHSlaveUnRegReq = (OHCH_SLAVE_UNREG_REQ_ST *)pvPrmv;
    OHCH_RW_SMC_ADDR_REQ_ST *pstOHCHSMCAddrReq = (OHCH_RW_SMC_ADDR_REQ_ST *)pvPrmv;
    OHCH_CSD_CONN_RSP_ST *pstOHCHCSDConnRsp = (OHCH_CSD_CONN_RSP_ST *)pvPrmv;
    //�жϺ���ͨ�Ŷ���
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {
        //����Ǵ��豸�����ӹ�˻�(�ж�ͨ�Žӿ�)
        if(pstCOMMObj->ucInterfaceType == USART_SLAVE_OPT ||
            pstCOMMObj->ucInterfaceType == USART_SLAVE_485)    
        {
            UCHAR aucData[USART_SIZE_BUFFER];
            ULONG ulSize;
            
            //����ԭ�����ͽ��д���
            switch(*((ULONG*)pvPrmv))
            {
            //�����Ǵ��豸���Զ�����ԭ�����ʹ���һ��
            //������Э������ֱ�ӷ���
            case OHCH_SEND_DATA_REQ:
            case OHCH_UNDE_SEND_REQ:
            case OHCH_DEBU_SEND_REQ:
                //д����Ϣ����
                memcpy(aucData,pstOHCHDataReq,sizeof(*pstOHCHDataReq));
                memcpy(aucData+sizeof(*pstOHCHDataReq), 
                        pstOHCHDataReq->pucData, pstOHCHDataReq->ulDataLen);
                ulSize = sizeof(*pstOHCHDataReq) + pstOHCHDataReq->ulDataLen;
                //����Ϣ���ݽ�������Э��ı���
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_TAKEOHTER,0, 
                        aucData, ulSize);
                //��������
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                //notice �������ݽ������ٸı�ԭ������
                pstOHCHDataReq->ulPrmvType = OHCH_SEND_DATA_CNF;
                break;

            case ACCH_MESS_SEND_REQ:
                //��Ҫ��ȷ����Ϣ�Ҳ���Ҫ���ͣ���Ϊ�ⲻ�����һ��AC��Ϣ��������Ҳû������
                //������˵�������һ��
                if(g_ulACCHSendMessageNum != 1)
                {
                    if(g_ulACCHSendMessageNum > 1)
                    {
                        g_ulACCHSendMessageNum--;
                    }
                    MyFree(pvPrmv);
                    pvPrmv = NULL;
                    break;
                }
                //�����һ������з���(ͬOHCH_MESS_SEND_REQ����)
                g_ulACCHSendMessageNum = 0;
            case OHCH_MESS_SEND_REQ:
                //д����Ϣ����
                ulSize = pstMessage->usDataLen + 8;                
                memcpy(aucData,pstMessage,ulSize);

                //����Ϣ���ݽ�������Э��ı���
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_TAKEOHTER,0, 
                        aucData, ulSize);
                //��������
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                //��Ҫ��ȷ����Ϣ
                MyFree(pvPrmv);
                pvPrmv = NULL;
                break;

            case OHCH_CONN_CENTER_REQ:
                pstOHCHConnCenterReq->ulPrmvType = OHCH_CONN_CENTER_CNF;
                //�ӻ���֧�ַ���GPRS����
                pstOHCHConnCenterReq->lResult = RESULT_STOP_REQ;
                //����ͨӦ��
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_ANS,0, 
                        aucData, 0);
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;
            case OHCH_GPRS_DISCONN_REQ:
                pstOHCHGprsDisconnReq->ulPrmvType = OHCH_GPRS_DISCONN_CNF;
                //�ӻ���֧�ַ���GPRS����
                pstOHCHGprsDisconnReq->lResult = RESULT_STOP_REQ;
                //����ͨӦ��
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_ANS,0, 
                        aucData, 0);
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;
            case OHCH_SLAVE_UNREG_REQ:
                pstOHCHSlaveUnRegReq->ulPrmvType = OHCH_SLAVE_UNREG_CNF;
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_UNREG,0, 
                        aucData, 0);
                //����ԭ��ֱ�ӷ�������
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;
            case OHCH_R_SMC_ADDR_REQ:
            case OHCH_W_SMC_ADDR_REQ:
                pstOHCHSMCAddrReq->ulPrmvType = OHCH_RW_SMC_ADDR_CNF;
                //�ӻ���֧�ֶ������ĺ���Ĵ���
                memset(pstOHCHSMCAddrReq->acTelNum,0,MAX_TEL_NUM_LEN);
                pstOHCHSMCAddrReq->lResult = SUCCEEDED;
                //����ͨӦ��
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_ANS,0, 
                        aucData, 0);
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;
            default:
                //�����޷�ȷ�ϵ��ͷ��ڴ�
                MyFree(pvPrmv);
                pvPrmv = NULL;
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_ANS,0, 
                        aucData, 0);
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;               
            }//switch(pstOHCHDataReq->ulPrmvType)
      
            //��ȷ�ϰ�
            if(pvPrmv != NULL)
            {
                if(OSQPost(g_pstOHQue, pvPrmv) == OS_Q_FULL)
                {
                    MyFree(pvPrmv);
                }
            }
            //�豱��ӿ�æ����Ϊ���Ӳ���һ��һ��ķ�ʽ
            pstCOMMObj->ucState = COMM_STATE_SEND_BUSY;            
        }//if(pstCOMMObj->ucInterfaceType == USART_SLAVE)        

        //notice�Ƶ���ǰ
        else if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)
        {
            //�豱��ӿ�æ
            pstCOMMObj->ucState = COMM_STATE_SEND_BUSY;
            
            //����ԭ�����ͽ��д���
            switch(*((ULONG*)pvPrmv))
            {
            //���۴��豸ת���������豸����������Ӧ��
            //��Ҫ����ԭ������
            case OHCH_SEND_DATA_REQ:
            
                MY_LOG_MSG_BIN((LS_TRACE, LL_INFO, (UCHAR*)pvPrmv,sizeof(pstOHCHDataReq)+pstOHCHDataReq->ulDataLen));
                
                pstOHCHDataReq->ulPrmvType = OHCH_SEND_DATA_CNF;
                if(NthSendtoMorD(pstCOMMObj,pvPrmv)==SUCCEEDED)
                {
                    pstOHCHDataReq->cResult= SUCCEEDED;                    
                }
                else
                {
                    pstOHCHDataReq->cResult= FAILED;
                    //test
                    //MyFree(pvPrmv);
                    //pvPrmv = NULL;
                } 
                break;//case OHCH_SEND_DATA_REQ

            //noticeע�����Ѵ��������յ�������Ϣ
            //�Լ����ѶϿ������������յ��Ͽ�������Ϣ
            case OHCH_CONN_CENTER_REQ:

                MY_LOG_MSG_BIN((LS_TRACE, LL_INFO, (UCHAR*)pvPrmv,sizeof(pstOHCHConnCenterReq)));
            
                pstOHCHConnCenterReq->ulPrmvType = OHCH_CONN_CENTER_CNF;            
                if(ConnectGPRSandCenter(pstCOMMObj,pstOHCHConnCenterReq->aucIpAddr,
                        pstOHCHConnCenterReq->usPortNum)==SUCCEEDED)
                {
                    //��ȫ��ͨ�ŷ�ʽ
                    g_ucCurrCommMode = COMM_MODE_GPRS;
                    pstOHCHConnCenterReq->lResult= SUCCEEDED;
                }
                else
                {
                    //����GPRS���ʧ����Ҫ���ԶϿ�
                    //����Ϊ�˷�ֹδ�Ͽ�GPRS����Ϊ�Ͽ���
                    DisConnGPRS(pstCOMMObj);
                    g_ucCurrCommMode = COMM_NO_CONN;                    
                    pstOHCHConnCenterReq->lResult= FAILED;                
                }
                //test����GPRS���ӺͶϿ����������ӺͶϿ�
                  //              StartGeneralTimer(&g_stTimerZoneInfo, 
                    //    TIME_GET_ZONEINFO/TIMER_UNIT_MS, NULL);
                        
                break;//case OHCH_GPRS_CONN_REQ

            case OHCH_GPRS_DISCONN_REQ:

                MY_LOG_MSG_BIN((LS_TRACE, LL_INFO, (UCHAR*)pvPrmv,sizeof(pstOHCHGprsDisconnReq)));
            
                pstOHCHGprsDisconnReq->ulPrmvType = OHCH_GPRS_DISCONN_CNF;            
                if(DisConnGPRS(pstCOMMObj)==SUCCEEDED)
                {
                    //��ȫ��ͨ�ŷ�ʽ
                    g_ucCurrCommMode = COMM_NO_CONN;
                    pstOHCHGprsDisconnReq->lResult= SUCCEEDED;
                }
                else
                {
                    //�Ͽ�����ʧ��Ҳ�ı�ȫ��ͨ�ŷ�ʽ
                    g_ucCurrCommMode = COMM_NO_CONN;
                    pstOHCHGprsDisconnReq->lResult= FAILED;                
                }
                //test����GPRS���ӺͶϿ����������ӺͶϿ�
                  //              StartGeneralTimer(&g_stTimerZoneInfo, 
                    //    TIME_GET_ZONEINFO/TIMER_UNIT_MS, NULL);
                        
                break;//case OHCH_GPRS_DISCONN_REQ

            case OHCH_R_SMC_ADDR_REQ:                
                pstOHCHSMCAddrReq->ulPrmvType = OHCH_RW_SMC_ADDR_CNF;            
                if(GetSMCAddr(pstCOMMObj,pstOHCHSMCAddrReq->acTelNum)==SUCCEEDED)
                {
                    pstOHCHSMCAddrReq->lResult= SUCCEEDED;
                }
                else
                {
                    pstOHCHSMCAddrReq->lResult= FAILED;                
                }               
                break;//case OHCH_R_SMC_ADDR_REQ

            case OHCH_W_SMC_ADDR_REQ:                
                pstOHCHSMCAddrReq->ulPrmvType = OHCH_RW_SMC_ADDR_CNF;            
                if(SetSMCAddr(pstCOMMObj,pstOHCHSMCAddrReq->acTelNum)==SUCCEEDED)
                {
                    pstOHCHSMCAddrReq->lResult= SUCCEEDED;
                }
                else
                {
                    pstOHCHSMCAddrReq->lResult= FAILED;                
                }               
                break;//case OHCH_W_SMC_ADDR_REQ

            case OHCH_CSD_CONN_RSP:
                //��Ȩ��Ӧ�ж��Ƿ�ժ��
                if(pstOHCHCSDConnRsp->lResult== SUCCEEDED)
                {
                    SendStrToUsart(pstCOMMObj, g_aucSAT_ATA, 
                           sizeof(g_aucSAT_ATA)-1);
                    //�жϽ��,���۳ɹ���ʧ�ܾ��˳�����Ϊ�������Ż����յ�CRING��Ϣ
                    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATACSDCONNECT,CH_SENDAT_ATA)==TRUE)
                    {
                        //��ȫ��ͨ�ŷ�ʽ
                        g_ucCurrCommMode = COMM_MODE_CSD;
                    }
                }
                //�һ�
                else
                {
                    SendStrToUsart(pstCOMMObj, g_aucSAT_ATH, 
                        sizeof(g_aucSAT_ATH)-1);
                    //���۳ɹ��������ڴ�����Ϊ��ժ���Ͳ������CSDģʽ
                    g_ucCurrCommMode = COMM_NO_CONN;
                    AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL);
                }
                //�����Ӧ��OH,�����ͷ��ڴ�
                MyFree(pvPrmv);
                pvPrmv = NULL;
                break;//case OHCH_CSD_CONN_RSP
                
            default:
                //�����޷�ȷ�ϵ��ͷ��ڴ�
                MyFree(pvPrmv);
                pvPrmv = NULL;
                break;
            }//switch(pstOHCHDataReq->ulPrmvType)
            //��ȷ�ϰ�
            if(pvPrmv != NULL)
            {
                if(OSQPost(g_pstOHQue, pvPrmv) == OS_Q_FULL)
                {
                    MyFree(pvPrmv);
                }
            }
            
            //�豱��ӿ���
            pstCOMMObj->ucState = COMM_STATE_SEND_IDLE;
        }//else if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)

    }//if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
}

/*************************************************
  Function:     HndlCHSthCtrlMsg
  Description:  ��������������ӿ��յ��Ŀ�����Ϣ
                ������������������ͻ���Ŀ�����Ϣ
  Calls:
  Called By:
  Input:        pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���
  Output:     
  Return:         
  Others:
*************************************************/
void HndlCHSthCtrlMsg(COMM_OBJ_ST *pstCOMMObj)
{
    void *pvPrmv = NULL;
    
    //notice��Ҫ�ж�ͨ�Ŷ�������

        //��������ӿڿ�����Ϣ,����Ϣ�򷵻�
        pvPrmv = OSQAccept(g_pstCHSthCtrlQue, &g_ucErr);
        
        if(pvPrmv == (void*)NULL)
        {
            return;
        }
        //����Ϣ����
        if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)
        {
            //noticeĿǰ����:�ӵ����п�����Ϣֱ����Ϊֹ
        }
        else if(pstCOMMObj->ucInterfaceType == USART_SLAVE_OPT ||
            pstCOMMObj->ucInterfaceType == USART_SLAVE_485)
        {
            UCHAR aucData[25];//����Э�������Ҫ�ڴ�
            ULONG ulSize;
            UCHAR ucFlag = 0;

            //д���־λ��Ŀǰ����ͨ��״̬��Ϣ
            //д��ͨ��״̬
            ucFlag = ucFlag & 0xF9;//BIT2:1=00
            switch(g_ucCurrCommMode)
            {
            case COMM_NO_CONN:
                break;//BIT2:1=00
            case COMM_MODE_CSD:
                ucFlag = ucFlag | 0x02;//BIT2:1=01
                break;
            case COMM_MODE_GPRS:
                ucFlag = ucFlag | 0x04;//BIT2:1=10
                break;
            default:
                break;
            }
            
            switch(((CH_CTR_ST*)pvPrmv)->ulPrmvType)
            {
            case CH_MG_MS_ASK:

                //MCM-45_20061130_zhangjie_begin
                if(pstCOMMObj->ucInterfaceType == USART_SLAVE_485)
                {
                    pstCOMMObj->ucState = COMM_STATE_SEND_BUSY;
                }
                //MCM-45_20061130_zhangjie_end
                
                //˵�����µĴӻ���ű����䵫��û���յ�Ӧ��
                if(g_stYkppParamSet.stYkppCommParam.ucNewSNum != MS_NEWS_DEVNUM)
                {
                    ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucNewSNum, 
                            MS_CMD_ASK,ucFlag, aucData, 0);
                }
                else
                {
                    ulSize = EncodeMSProtocol(GetNextSNum(), MS_CMD_ASK,ucFlag, 
                            aucData, 0);
                }
                //notice������ʱ20ms������ѯ����������һ���ӻ��ر�ʹ�ܵ�ʱ��
                MyDelay(20);
                
                SthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;
            case CH_MG_MS_REGANS:
                //��Ŀǰ��ѯ�Ĵӻ����Ϊ�µ��豸���
                g_stYkppParamSet.stYkppCommParam.ucAskingSNum = MS_NEWS_DEVNUM;
                //notice����Ӧ�������ݵ�Ԫ�ı���
                aucData[0] = 0x03;
                aucData[1] = 0x00;
                aucData[2] = 0x01;
                aucData[3] = 0x80;
                aucData[4] = GetNewSNum();
                ulSize = EncodeMSProtocol(MS_NEWS_DEVNUM, MS_CMD_REGANS,ucFlag, 
                        aucData, 5);
                SthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                //�����ӻ�ע�ᱣ����ʱ��
                StartGeneralTimer(&g_stTimerSlaveRegProtect, 
                        TIME_SLAVEREG_PROTECT/TIMER_UNIT_MS, NULL,NONPERIODIC);
                break;

            case CH_MG_MS_UNREGANS:
                UCHAR ucSNum;
                ucSNum = ((CH_CTR_ST*)pvPrmv)->ucParam1;
                //�ȷ���Ӧ��
                ulSize = EncodeMSProtocol(ucSNum, MS_CMD_UNREGANS,ucFlag, 
                        aucData, 0);
                SthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                //ɾ���ӻ����
                MSChangeBit(&g_stYkppParamSet.stYkppCommParam.usSBitmap, ucSNum, 0);
                //��Ŀǰ��ѯ�Ĵӻ����
                g_stYkppParamSet.stYkppCommParam.ucAskingSNum = ucSNum;
                //����flash
                SaveYkppParam();
                break;
            default:
                break;
            }//switch(((CH_CTR_ST*)pvPrmv)->ulPrmvType)
            
            //������ѯ��ʱ��ʱ��
            StartGeneralTimer(&g_stTimerSthAskTimeOut, TIME_TIMEOUT_ASKSLAVE/TIMER_UNIT_MS, NULL,NONPERIODIC);
            
        }//else if(pstCOMMObj->ucInterfaceType == USART_SLAVE)

        MyFree(pvPrmv);  
}


/*************************************************
  Function:     HndlCHNthCtrlMsg
  Description:  ������������ӿ��յ��Ŀ�����Ϣ
                ������������������ͻ���Ŀ�����Ϣ
  Calls:
  Called By:
  Input:        pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���
  Output:     
  Return:         
  Others:
*************************************************/
void HndlCHNthCtrlMsg(COMM_OBJ_ST *pstCOMMObj)
{
    void *pvPrmv = NULL;

    //notice��Ҫ�ж�ͨ�Ŷ�������

        //������ӿڿ�����Ϣ,����Ϣ�򷵻�
        pvPrmv = OSQAccept(g_pstCHNthCtrlQue, &g_ucErr);
        
        if(pvPrmv == (void*)NULL)
        {
            return;
        }
        //����Ϣ����
        if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)
        {
            OS_Q_DATA stOSQData;
            //Ŀǰ����:modem�Է�����Ļ�Ӧ
            //����һ��һ��Ļ�Ӧ����AtRtnIsOk�д������
            switch(((CH_CTR_ST*)pvPrmv)->ulPrmvType)
            {
            case CH_MG_AT_ATA:
                SendStrToUsart(pstCOMMObj, g_aucSAT_ATA, 
                       sizeof(g_aucSAT_ATA)-1);
                //�жϽ��,���۳ɹ���ʧ�ܾ��˳�����Ϊ�������Ż����յ�CRING��Ϣ
                if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATACSDCONNECT,CH_SENDAT_ATA)!=TRUE)
                {
                    break;
                }
                //��ȫ��ͨ�ŷ�ʽ
                g_ucCurrCommMode = COMM_MODE_CSD;

                MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "CHTASK ATA"));
                
                break;
            case CH_MG_AT_ATH:
                SendStrToUsart(pstCOMMObj, g_aucSAT_ATH, 
                       sizeof(g_aucSAT_ATH)-1);
                OHCH_CSD_DISCONN_IND_ST *pstOHCHCSDDisConnInd;
                //�����ڴ�
                pstOHCHCSDDisConnInd = (OHCH_CSD_DISCONN_IND_ST *) MyMalloc(g_pstMemPool16);
                if(pstOHCHCSDDisConnInd == (void*)NULL)
                {
                    break;
                }
                //֪ͨOH�ѹҶ�                
                pstOHCHCSDDisConnInd->ulPrmvType = OHCH_CSD_DISCONN_IND;
                pstOHCHCSDDisConnInd->lResult = FAILED;
                if(OSQPost(g_pstOHQue, pstOHCHCSDDisConnInd) == OS_Q_FULL)
                {
                    MyFree(pstOHCHCSDDisConnInd);            
                }
                //��Ϊֻ���յ�NO CARRIER�����������е�����²ŻᷢATH
                //�������۳ɹ���ʧ�ܾ��˳�����ͨ��״̬Ϊ��CSD
                if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
                {
                    //notice��ȫ��ͨ�ŷ�ʽ
                    g_ucCurrCommMode = COMM_NO_CONN;
                    break;
                }
                //��ȫ��ͨ�ŷ�ʽ
                g_ucCurrCommMode = COMM_NO_CONN;

                MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "CHTASK ATH"));
                
                break;
            case CH_MG_AT_CSD_CON:
                ULONG ulPos1,ulPos2;
                OHCH_CSD_CONN_IND_ST *pstOHCHCSDConnInd;
                
                //��ȡ����"��ĵ绰����
                ulPos1 = FindChar(g_aucCLIPTel, '"', 1, 15);
                ulPos2 = FindChar(g_aucCLIPTel, '"', 2, 40);
                if((ulPos1 == FIND_NO_CHAR) || (ulPos2 == FIND_NO_CHAR))
                {
                    break;
                }
                //д��绰����
                pstOHCHCSDConnInd = (OHCH_CSD_CONN_IND_ST *) MyMalloc(g_pstMemPool256);
                if(pstOHCHCSDConnInd == (void*)NULL)
                {
                    break;
                }
                memset(pstOHCHCSDConnInd,0,sizeof(*pstOHCHCSDConnInd));
                pstOHCHCSDConnInd->ulPrmvType = OHCH_CSD_CONN_IND;
                pstOHCHCSDConnInd->lResult = SUCCEEDED;
                memcpy(pstOHCHCSDConnInd->acTelNum,g_aucCLIPTel+ulPos1+1,
                        ulPos2-ulPos1-1);

                //���֪ͨOH
                //������Ϣ����Ϣ�������������ݰ�
                if(OSQPost(g_pstOHQue, pstOHCHCSDConnInd) == OS_Q_FULL)
                {
                    MyFree(pstOHCHCSDConnInd);            
                }
                break;
            case CH_MG_GET_ZONEINFO:
                //�жϱ�������Ƿ�æ
                OSQQuery(g_pstCHNthQue, &stOSQData);
                //��æ��ͨ�������ӵ�����²�ѯС����Ϣ
                if((stOSQData.OSNMsgs<1)&&(g_ucCurrCommMode == COMM_NO_CONN))
                {
                    SendStrToUsart(pstCOMMObj, g_aucSAT_CCED, 
                           sizeof(g_aucSAT_CCED)-1);
                    //�жϽ��,���۳ɹ���ʧ�ܾ��˳���������������ʱ��
                    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_CCED)==TRUE)
                    {                    
                        //����С����Ϣ
                        GetZoneInfo();
                    }
                }
                break;
            case CH_MG_AT_GPRS_SOCKETCLOSE:
                OHCH_GPRS_DISCONN_IND_ST *pstOHCHGPRSDisconnInd;
                
                //�ı䵱ǰͨ�ŷ�ʽ
                g_ucCurrCommMode = COMM_NO_CONN;
                
                pstOHCHGPRSDisconnInd = (OHCH_GPRS_DISCONN_IND_ST *) MyMalloc(g_pstMemPool16);
                if(pstOHCHGPRSDisconnInd == (void*)NULL)
                {
                    break;
                }
                pstOHCHGPRSDisconnInd->ulPrmvType = OHCH_GPRS_DISCONN_IND;
                pstOHCHGPRSDisconnInd->lResult = SUCCEEDED;

                //���֪ͨOH
                //������Ϣ����Ϣ�������������ݰ�
                if(OSQPost(g_pstOHQue, pstOHCHGPRSDisconnInd) == OS_Q_FULL)
                {
                    MyFree(pstOHCHGPRSDisconnInd);            
                }

                MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "REMOTE DISCONN GPRS"));
                
                break;
                
            case CH_MG_RESET:
                //�жϱ�������Ƿ�æ
                OSQQuery(g_pstCHNthQue, &stOSQData);
                //��æ��ͨ�������ӵ����������
                if((stOSQData.OSNMsgs<1)&&(g_ucCurrCommMode == COMM_NO_CONN))
                {
                    ResetModem();
                    Restart();
                }
                break;
            default:
                break;
            }//switch(((CH_CTR_ST*)pvPrmv)->ulPrmvType)
        }
        else if(pstCOMMObj->ucInterfaceType == USART_SLAVE_OPT ||
            pstCOMMObj->ucInterfaceType == USART_SLAVE_485)
        {
            UCHAR aucData[25];//����Э�������Ҫ�ڴ�
            ULONG ulSize;
            switch(((CH_CTR_ST*)pvPrmv)->ulPrmvType)
            {
            case CH_MG_SM_ANS:
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_ANS,0, 
                        aucData, 0);
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;
            case CH_MG_SM_REG:
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_REG,0, 
                        aucData, 0);
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;
            case CH_MG_SM_UNREGCONF:
                //�ȷ���ȷ����Ϣ
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_UNREGCONF,0, 
                        aucData, 0);
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);                
                //�����ٸı�ӻ�����Է�ֹ�ٴα�ע��
                //g_stYkppParamSet.stYkppCommParam.ucSNum = MS_NEWS_DEVNUM;

                //���֪ͨOH
                UCHAR *pucMem;
                OHCH_RECV_DATA_IND_ST stOHCHRec;
                pucMem = (UCHAR *) MyMalloc(g_pstMemPool256);
                if(pucMem == (void*)NULL)
                {
                    break;
                }
                stOHCHRec.ucSrcIf = COMM_IF_NORTH;
                stOHCHRec.ulPrmvType= OHCH_SLAVE_UNREG_CNF;
                stOHCHRec.pucData = NULL;
                stOHCHRec.ulDataLen = 0;
                memcpy(pucMem,&stOHCHRec,sizeof(stOHCHRec));
                //������Ϣ����Ϣ�������������ݰ�
                if(OSQPost(g_pstOHQue, pucMem) == OS_Q_FULL)
                {
                    MyFree(pucMem);            
                }
                break;
            default:
                break;
            }//switch(((CH_CTR_ST*)pvPrmv)->ulPrmvType)
        }//else if(pstCOMMObj->ucInterfaceType == USART_SLAVE)

        MyFree(pvPrmv);  
}

/*************************************************
  Function:       AtRtnIsOk
  Description:    ������������ӿں���������
                  ���ڻ��modem�Ļ�Ӧ���жϳ�ʱ
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���
                     ulWaitTime:      �ȴ���Ӧ��ʱ�䣬��λms
                     ulWaitTime��ֵӦ�ô���ulWaitTime
                     ulATType:  ����AT���������
  Output:     
  Return:         TRUE:OK
  Others:
*************************************************/
LONG AtRtnIsOk(COMM_OBJ_ST *pstCOMMObj,ULONG ulWaitTime,ULONG ulATType)
{
    void *pvPrmv = NULL;

    //������ʱ��    
    StartGeneralTimer(&g_stTimerATRtnTimeOut, 
            ulWaitTime/TIMER_UNIT_MS, NULL,NONPERIODIC);
    for(;;)
    {
        //������ӿڽ�������
        //notice��Ҫ��������ӿڵķ��ͽ��պͱ���ӿڵĽ���
        HndlCHNthRec(pstCOMMObj);
        HndlCHSthRec(&g_stSouthInterface);
        CHSthReadyAndSend();

        //������ӿڿ�����Ϣ
        //notice���ƶ����Ƿ�ҲҪ����ͨ�Ŷ���ṹ����
        pvPrmv = OSQAccept(g_pstCHNthCtrlQue, &g_ucErr);

        //����Ϣ�����ȴ�
        if(pvPrmv == (void*)NULL)
        {
            //MYDELAY������������������
            MyDelay(CH_DELAY_TIME);
            continue;
        }
        //����Ϣ����
        switch(((CH_CTR_ST*)pvPrmv)->ulPrmvType)
        {
        case CH_MG_AT_TIMEOUT:
        case CH_MG_AT_ERROR:
            MyFree(pvPrmv);
            return FALSE;           
            break;
            
        case CH_MG_AT_OK:
            MyFree(pvPrmv);
            if(ulATType == CH_SENDAT_NORMAL)
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }            
            break;

        case CH_MG_AT_GPRS_ACT:
            MyFree(pvPrmv);
            if(ulATType == CH_SENDAT_GPRSCONN)  
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }            
            break;

        case CH_MG_AT_GPRS_SOCKETCLOSE:
            MyFree(pvPrmv);
            if(ulATType == CH_SENDAT_SOCKETCLOSE)  
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }            
            break;            

        case CH_MG_AT_GPRS_WAITDATA:
            MyFree(pvPrmv);
            if(ulATType == CH_SENDAT_GPRSOTCP)  
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }            
            break; 
            
        case CH_MG_AT_CSCA:
            MyFree(pvPrmv);
            if(ulATType == CH_SENDAT_CSCA)  
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }            
            break;

        case CH_MG_AT_CCED:
            MyFree(pvPrmv);
            if(ulATType == CH_SENDAT_CCED)  
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }            
            break;

        case CH_MG_AT_CONNECT:
            MyFree(pvPrmv);
            if(ulATType == CH_SENDAT_ATA)  
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }            
            break;

        default:
            //MCM-51_20070109_Zhonghw_begin
            //�Ѳ��ܱ��ϵĿ�����Ϣ�Ż���Ϣ���У��ɿ�����Ϣ������������
            if(OSQPost(g_pstCHNthCtrlQue, pvPrmv) == OS_Q_FULL)
            {
                MyFree(pvPrmv);
                return FALSE;
            }        
            //MCM-51_20070109_Zhonghw_end
            break;
        }
    }
}

/*************************************************
  Function:     SendStrToUsart
  Description:    ������������ӿں���������
                       ���ڷ���AT����
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���

  Output:     
  Return:        
  Others:
*************************************************/
void SendStrToUsart(COMM_OBJ_ST *pstCOMMObj,UCHAR pucData[],ULONG ulSize)
{
    UINT32 cpu_sr;    
    //��������
    OS_ENTER_CRITICAL();
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            pucData, ulSize);
    OS_EXIT_CRITICAL();
}

/*************************************************
  Function:       InitNth
  Description:    ��������ʼ������ӿ�
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���

  Output:     
  Return:  SUCCEEDED:�ɹ� FAILED:ʧ��        
  Others:
*************************************************/
LONG InitNth(COMM_OBJ_ST *pstCOMMObj)
{
    //��Ҫ��ʼ��modem
    if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)
    {
        //���ȶϿ�GPRS���ӣ��������ܳɹ���ʼ��modem
        MyDelay(10);        
        DisConnGPRS(pstCOMMObj);

        //�Ͽ����ݲ���
        SendStrToUsart(pstCOMMObj, g_aucSAT_ATH, 
               sizeof(g_aucSAT_ATH)-1);
        //�ȴ���������۽����ζ�����ִ��
        AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL);

        //AT��ʼ������,ȥ������������3��
        SendStrToUsart(pstCOMMObj, g_aucSAT_ATE0, 
               sizeof(g_aucSAT_ATE0)-1);
        //�жϽ��
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            SendStrToUsart(pstCOMMObj, g_aucSAT_ATE0, 
                   sizeof(g_aucSAT_ATE0)-1);
            if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
            {
                SendStrToUsart(pstCOMMObj, g_aucSAT_ATE0, 
                       sizeof(g_aucSAT_ATE0)-1);
                if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
                {
                    return FAILED;   
                }    
            }            
        }

        //AT��ʼ������,����PDUģʽ
        SendStrToUsart(pstCOMMObj, g_aucSAT_CMGF0, 
               sizeof(g_aucSAT_CMGF0)-1);
        //�жϽ��
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }

        //AT��ʼ������,�����¶���Ϣֱ�ӷ���
        SendStrToUsart(pstCOMMObj, g_aucSAT_CNMI2, 
               sizeof(g_aucSAT_CNMI2)-1);
        //�жϽ��
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }

        //noticeAT��ʼ������
        SendStrToUsart(pstCOMMObj, g_aucSAT_CSMP, 
               sizeof(g_aucSAT_CSMP)-1);
        //�жϽ��
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }

        //������ʾ
        SendStrToUsart(pstCOMMObj, g_aucSAT_CLIP, 
               sizeof(g_aucSAT_CLIP)-1);
        //�жϽ��
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }

        //��ʾ��������
        SendStrToUsart(pstCOMMObj, g_aucSAT_CRC,
               sizeof(g_aucSAT_CRC)-1);
        //�жϽ��
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }
        
        //notice����Ƶ��

        //��������
        SendStrToUsart(pstCOMMObj, g_aucSAT_ATANDW,
               sizeof(g_aucSAT_ATANDW)-1);
        //�жϽ��
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }

        //��������
        SendStrToUsart(pstCOMMObj, g_aucSAT_CSAS,
               sizeof(g_aucSAT_CSAS)-1);
        //�жϽ��
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }
        
        //notice����sim����Ķ���

        //�������ж���
        SendStrToUsart(pstCOMMObj, g_aucSAT_CMGL,
               sizeof(g_aucSAT_CMGL)-1);
        //�жϽ��
        if(AtRtnIsOk(pstCOMMObj,60000,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }
        //�����ʼ��ʱƵ��ɾ������Ϣ
        if(g_ucCurrSimSMSSta == SIM_STA_READED)
        {
            //ɾ�����ж���
            SendStrToUsart(pstCOMMObj, g_aucSAT_CMGD,
                   sizeof(g_aucSAT_CMGD)-1);
            //�жϽ��
            if(AtRtnIsOk(pstCOMMObj,8000,CH_SENDAT_NORMAL)!=TRUE)
            {
                return FAILED;
            }
            else
            {
                g_ucCurrSimSMSSta = SIM_STA_NORMAL;
            }
        }
        return SUCCEEDED;

    }//if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)
    //��modem��Ϊ�ӻ�ֱ�ӷ��سɹ������ýӿ�æ
    else if(pstCOMMObj->ucInterfaceType == USART_SLAVE_485 ||
            pstCOMMObj->ucInterfaceType == USART_SLAVE_OPT)
    {
        pstCOMMObj->ucState = COMM_STATE_SEND_BUSY;
        return SUCCEEDED;
    }    
    //������ֱ�ӷ��سɹ�
    else
    {
        return SUCCEEDED;
    }
}

/*************************************************
  Function:       InitSth
  Description:    ��������ʼ������ӿ�
                  ��Ҫ��������ѯ����
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���

  Output:     
  Return:  SUCCEEDED:�ɹ� FAILED:ʧ��        
  Others:
*************************************************/
LONG InitSth(COMM_OBJ_ST *pstCOMMObj)
{
    if(pstCOMMObj->ucInterfaceType == USART_SLAVE_485 ||
            pstCOMMObj->ucInterfaceType == USART_SLAVE_OPT)
    {
        pstCOMMObj->ucState = COMM_STATE_SEND_IDLE;
        //notice������ѯ��ʱ��ʱ��
        g_stYkppParamSet.stYkppCommParam.ucAskingSNum = 0;
        //500ms��ʼ��ѯ
        StartGeneralTimer(&g_stTimerSthAskTimeOut, 500/TIMER_UNIT_MS, NULL,NONPERIODIC);
    }
    return SUCCEEDED;
}

/*************************************************
  Function:       GetZoneInfoPartValue
  Description:    ��������2�����ż������ݣ�����������ż��������򷵻�0
                  �������ż䲻�ܳ���8�ֽڣ����򷵻�ʧ��
                  ������ֻ��GetZoneInfo����
                  ���������ƻ�pucDataָ�������
  Calls:
  Called By:
  Input:          pucData:ָ�����ݵ�ָ��
                  ulFirstNum:�������ǰ�Ķ����ǵڼ�������
                  pucValue:��ŵõ�������ֵ
                  ucISHex:1:ת��Ϊ16��������0:ת��Ϊ10������
  Output:     
  Return:  FAILED:ʧ��        SUCCEEDED:�ɹ�
  Others:
*************************************************/
LONG GetZoneInfoPartValue(UCHAR* pucData,ULONG ulFirstNum,ULONG *pucValue,UCHAR ucISHex)
{
    ULONG ulPos1,ulPos2,ulSize;
    UCHAR aucTemp[8];
    //��ȡ�������ż������
    ulPos1 = FindChar(pucData, ',', ulFirstNum, DATAPACK_ATFULL_DATA1_SIZE);
    ulPos2 = FindChar(pucData, ',', ulFirstNum+1, DATAPACK_ATFULL_DATA1_SIZE);
    ulSize = ulPos2 - ulPos1 - 1;

    if((ulPos2 == FIND_NO_CHAR)||(ulSize>8))
    {
        return FAILED;
    }
    
    (*pucValue) = 0;
    
    //�������Ų�������һ�������д���
    if(ulSize != 0)
    {
        //Ϊ�����������λ���0
        if(ulSize%2 != 0)
        {
            aucTemp[0] = 0x30;
            memcpy(aucTemp+1,pucData+ulPos1+1,ulSize);
            ulSize = ulSize + 1;
        }
        else
        {
            memcpy(aucTemp,pucData+ulPos1+1,ulSize);
        }
        ulSize = Merge2Bytes(aucTemp, ulSize);
        //�Ѷ��ֽںϲ��������ת��ΪULONG����
        //CCED���ݸ�λ��ǰ����λ�ں�
        ULONG j=1;
        if(ucISHex)
        {
            for(ULONG i=ulSize;i>0;i--)
            {
                (*pucValue) = (*pucValue) + (aucTemp[i-1])*j;
                j = j*256; 
            }
        }
        else
        {
            for(ULONG i=ulSize;i>0;i--)
            {
                aucTemp[i-1] = (aucTemp[i-1]/16)*10 + aucTemp[i-1]%16;
                (*pucValue) = (*pucValue) + (aucTemp[i-1])*j;
                j = j*100; 
            }
        }
    }
    else
    {
        (*(ULONG*)pucValue) = 0xFFFFFFFF;
    }
    return SUCCEEDED;
}

/*************************************************
  Function:       GetZoneInfo
  Description:    ��������ȫ�ֱ�����ֱ�ӻ�ȡС����Ϣ
                  ����С����Ϣ����CMCC�ж���Ĵ洢��
  Calls:
  Called By:
  Input:                            
  Output:     
  Return:  SUCCEEDED:�ɹ� FAILED:ʧ��        
  Others:
*************************************************/
LONG GetZoneInfo(void)
{
    ULONG ulTemp;
    //��1��2����֮��Ϊ��Ӫ�̴���
    if(GetZoneInfoPartValue(g_aucZoneInfo, 1, &ulTemp, 0) != SUCCEEDED)
    {
        return FAILED;
    }
    //д����Ӧλ��
    g_stRcParamSet.ucSsMnc = (UCHAR)ulTemp;

    //��2��3����֮��Ϊλ��������(16����)
    if(GetZoneInfoPartValue(g_aucZoneInfo, 2, &ulTemp, 1) != SUCCEEDED)
    {
        return FAILED;
    }
    g_stRcParamSet.usSsLac= (USHORT)ulTemp;

    //��3��4����֮��ΪС��ʶ����(16����)
    if(GetZoneInfoPartValue(g_aucZoneInfo, 3, &ulTemp, 1) != SUCCEEDED)
    {
        return FAILED;
    }
    g_stRcParamSet.usSsCi= (USHORT)ulTemp;

    //��4��5����֮��Ϊ��վʶ����
    if(GetZoneInfoPartValue(g_aucZoneInfo, 4, &ulTemp, 0) != SUCCEEDED)
    {
        return FAILED;
    }
    g_stRcParamSet.ucSsBsic = (UCHAR)ulTemp;

    //��5��6����֮��ΪBCCH��Ƶ��
    if(GetZoneInfoPartValue(g_aucZoneInfo, 5, &ulTemp, 0) != SUCCEEDED)
    {
        return FAILED;
    }
    g_stRcParamSet.usSsBcch = (USHORT)ulTemp;

    //��6��7����֮��ΪBCCH���յ�ƽ
    if(GetZoneInfoPartValue(g_aucZoneInfo, 6, &ulTemp, 0) != SUCCEEDED)
    {
        return FAILED;
    }
    //notice��Ҫ���չ�ʽ���¼���
    g_stRcParamSet.cSsBcchRxLev = (CHAR)ulTemp;    

    return SUCCEEDED;
}
/*************************************************
  Function:       GetSMCAddr
  Description:    ��������ö������ĺ���
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���
                  pcTelNum:           ��ŵ绰�����ָ��

  Output:     
  Return:  SUCCEEDED:�ɹ� FAILED:ʧ��        
  Others:
*************************************************/
LONG GetSMCAddr(COMM_OBJ_ST *pstCOMMObj,CHAR *pcTelNum)
{
    ULONG ulPos1,ulPos2;
    SendStrToUsart(pstCOMMObj, g_aucSAT_CSCAQ, 
           sizeof(g_aucSAT_CSCAQ)-1);
    memset(pcTelNum,0,MAX_TEL_NUM_LEN+1);
    //�жϽ��testʱ��
    //MCM-53_20070111_Zhonghw_begin
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_CSCA)!=TRUE)
    //MCM-53_20070111_Zhonghw_end
    {
        return FAILED;
    }
    //��ȡ����"��ĵ绰����
    ulPos1 = FindChar(g_aucSMCAddr, '"', 1, 15);
    ulPos2 = FindChar(g_aucSMCAddr, '"', 2, 40);
    if((ulPos1 == FIND_NO_CHAR) || (ulPos2 == FIND_NO_CHAR))
    {
        return FAILED;
    }
    //д��绰����
    memcpy(pcTelNum,g_aucSMCAddr+ulPos1+1,
            ulPos2-ulPos1-1);
    return SUCCEEDED;
}

/*************************************************
  Function:       SetSMCAddr
  Description:    ���������ö������ĺ���
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���
                  pcTelNum:           ��ŵ绰�����ָ��

  Output:     
  Return:  SUCCEEDED:�ɹ� FAILED:ʧ��        
  Others:
*************************************************/
LONG SetSMCAddr(COMM_OBJ_ST *pstCOMMObj,CHAR *pcTelNum)
{
    ULONG ulSize;
    UINT32 cpu_sr;
    ulSize = FindChar((UCHAR*)pcTelNum, 0, 1, MAX_TEL_NUM_LEN + 1);
    
    if(ulSize == FIND_NO_CHAR)
    {
        return FAILED;
    }
    
    OS_ENTER_CRITICAL();
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            g_aucSAT_CSCA, sizeof(g_aucSAT_CSCA)-1);
    //д��绰����
    UsartWrite ((USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            (UCHAR*)pcTelNum, ulSize);
    //д��0D
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            g_aucSAT_CR, sizeof(g_aucSAT_CR)-1);
    OS_EXIT_CRITICAL();
    
    //MCM-53_20070111_Zhonghw_begin
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
    //MCM-53_20070111_Zhonghw_end
    {
        return FAILED;
    }
    return SUCCEEDED;
}

/*************************************************
  Function:       SetIPandPort
  Description:    ����������IP��ַ�Ͷ˿ں�
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���

  Output:     
  Return:  SUCCEEDED:�ɹ� FAILED:ʧ��        
  Others:
*************************************************/
LONG SetIPandPort(COMM_OBJ_ST *pstCOMMObj,UCHAR aucIpAddr[],USHORT usPortNum)
{
    UCHAR aucData[35];
    ULONG cpu_sr,ulSize,ulCurrPos=0;
    
    //д��AT#TCPSERV=
    memcpy(aucData,g_aucSAT_GPRSIPSET,sizeof(g_aucSAT_GPRSIPSET)-1);
    ulCurrPos = ulCurrPos + sizeof(g_aucSAT_GPRSIPSET)-1;
    //д��"ipaddr"
    *(aucData + ulCurrPos) = '"';
    ulCurrPos++;
    for(ULONG i=0;i<4;i++)
    {
        ulSize = ULONGToStr(aucData+ulCurrPos, (ULONG)aucIpAddr[i]);
        ulCurrPos = ulCurrPos + ulSize;
        *(aucData + ulCurrPos) = '.';
        ulCurrPos++;
    }
    //�����һ�����Ϊ�ֺ�
    *(aucData + ulCurrPos - 1) = '"';
    //д��0D
    *(aucData + ulCurrPos) = 0x0D;
    ulCurrPos++;
    ulSize = ulCurrPos;    
    //����IP
    OS_ENTER_CRITICAL();
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            aucData, ulSize);
    OS_EXIT_CRITICAL();
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCONGPRS,CH_SENDAT_NORMAL)!=TRUE)
    {
        return FAILED;
    }

    ulCurrPos = 0;
    //д��AT#TCPPORT=
    memcpy(aucData,g_aucSAT_GPRSPORTSET,sizeof(g_aucSAT_GPRSPORTSET)-1);
    ulCurrPos = ulCurrPos + sizeof(g_aucSAT_GPRSPORTSET)-1;
    //д��PORT
    ulSize = ULONGToStr(aucData+ulCurrPos, (ULONG)usPortNum);
    ulCurrPos = ulCurrPos + ulSize;
    //д��0D
    *(aucData + ulCurrPos) = 0x0D;
    ulCurrPos++;
    ulSize = ulCurrPos;
    //����IP
    OS_ENTER_CRITICAL();
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            aucData, ulSize);
    OS_EXIT_CRITICAL();
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCONGPRS,CH_SENDAT_NORMAL)!=TRUE)
    {
        return FAILED;
    }
    return SUCCEEDED;
}

/*************************************************
  Function:     ConnectGPRSandCenter
  Description:    ����������GPRS�����IP��ַ�Ͷ˿ں�����������
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���

  Output:     
  Return:  SUCCEEDED:�ɹ� FAILED:ʧ��        
  Others:
*************************************************/
LONG ConnectGPRSandCenter(COMM_OBJ_ST *pstCOMMObj,UCHAR aucIpAddr[],USHORT usPortNum)
{

    MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "CHTASK CONNECTING GPRS"));

    //notice���Կ�������֮ǰ�Ͽ����ӣ�������03

    //ע������
    SendStrToUsart(pstCOMMObj, g_aucSAT_GPRSCGATT, 
           sizeof(g_aucSAT_GPRSCGATT)-1);    
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCONGPRS,CH_SENDAT_NORMAL)!=TRUE)
    {
        //ʧ���п������յ�"<"�����Բ����أ�������������
        //return FAILED;
    }
    //��������
    SendStrToUsart(pstCOMMObj, g_aucSAT_GPRSCONN, 
           sizeof(g_aucSAT_GPRSCONN)-1);    
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCONGPRS,CH_SENDAT_GPRSCONN)!=TRUE)
    {
        return FAILED;
    }

    //����IP��PORT
    if(SetIPandPort(pstCOMMObj, aucIpAddr, usPortNum) != SUCCEEDED)
    {
        return FAILED;
    }

    //׼���շ���������  
    SendStrToUsart(pstCOMMObj, g_aucSAT_GPRSOTCP, 
           sizeof(g_aucSAT_GPRSOTCP)-1);    
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCONGPRS,CH_SENDAT_GPRSOTCP)!=TRUE)
    {
        //��ʱʧ�ܵ������Ѿ����ϣ�����Ҫȷ���Ƿ�����
        //����+++ʧ����˵��δ����        
        SendStrToUsart(pstCOMMObj, g_aucSAT_ATOFFLINE, 
            sizeof(g_aucSAT_ATOFFLINE)-1);    
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATDISCONGPRS,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }
        //����+++�ɹ���˵�����ϣ���Ҫ��֤ת��������ģʽ:������10��
        else
        {
            for(ULONG i = 0; i < 10; i++)
            {
                SendStrToUsart(pstCOMMObj, g_aucSAT_ATATO, 
                    sizeof(g_aucSAT_ATATO)-1);    
                if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCONGPRS,CH_SENDAT_NORMAL)==TRUE)
                {
                    return SUCCEEDED;
                }                
            }
            //�����������10�λ����ɹ�����Ͽ�����
            DisConnGPRS(pstCOMMObj);
            return FAILED;
        }
    }

    MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "CHTASK CONNECT GPRS SUCCEED"));

    return SUCCEEDED;
    
}

/*************************************************
  Function:     DisConnGPRS
  Description:    �������Ͽ�GPRS����
  Calls:
  Called By:
  Input:          pstCOMMObj:         ��Ϊ����ӿڵ�ͨ�Ŷ���

  Output:     
  Return:  SUCCEEDED:�ɹ� FAILED:ʧ��        
  Others:
*************************************************/
LONG DisConnGPRS(COMM_OBJ_ST *pstCOMMObj)
{

    MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "CHTASK DISCONN GPRS"));

    MyDelay(500);//��ʱ500ms����ֹǰ�������ݻ����
    //����ֱ�ӷ���0x03���Ͽ�����
    SendStrToUsart(pstCOMMObj, g_aucSAT_GPRSSOCKETCLOSE, 
           sizeof(g_aucSAT_GPRSSOCKETCLOSE)-1);    
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATDISCONGPRS,CH_SENDAT_SOCKETCLOSE) == TRUE)
    {
        return SUCCEEDED;
    }
        
    //�л���OFFLINEģʽ
    SendStrToUsart(pstCOMMObj, g_aucSAT_ATOFFLINE, 
           sizeof(g_aucSAT_ATOFFLINE)-1);    
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATDISCONGPRS,CH_SENDAT_NORMAL)!=TRUE)
    {
        //�Ͽ�����,��ֹĿǰ�Ѿ���OFFLINEģʽ
        SendStrToUsart(pstCOMMObj, g_aucSAT_GPRSDISCONN, 
               sizeof(g_aucSAT_GPRSDISCONN)-1);    
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATDISCONGPRS,CH_SENDAT_NORMAL)==TRUE)
        {
            return SUCCEEDED;
        }
    }
    //�Ͽ�����
    SendStrToUsart(pstCOMMObj, g_aucSAT_GPRSDISCONN, 
           sizeof(g_aucSAT_GPRSDISCONN)-1);    
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATDISCONGPRS,CH_SENDAT_NORMAL)!=TRUE)
    {
        return FAILED;
    } 
    
    return SUCCEEDED;
    
}
//test
extern NM_PARAM_SET_ST      g_stNmParamSet;

/*************************************************
  Function:     CHSthReadyAndSend
  Description:    �������ȴ�����ӿ�׼���ú���������
  Calls:
  Called By:
  Input:          
  Output:     
  Return:         
  Others:       ���ж�ͨ�Žӿ��к��ڴ���������
*************************************************/
void CHSthReadyAndSend(void)
{
    void *pvPrmv = NULL;
    if(CHSendReady(&g_stSouthInterface) == TRUE)
    {
        //������Ϣ
        pvPrmv = OSQAccept(g_pstCHSthQue, &g_ucErr);
        if(pvPrmv != (void*)NULL)
        {
            HndlCHSthSend(&g_stSouthInterface, pvPrmv);
            //���۷����Ƿ�ɹ�,ԭ���ݾ����ظ�OH,�������ͷ��ڴ�
            //MyFree(pvPrmv);
        }
        //û����Ϣ���жϽӿ�
        //notice�����Ҫ��ѯ���������Ϣ
        else if(g_stSouthInterface.ucInterfaceType == USART_SLAVE_OPT ||
            g_stSouthInterface.ucInterfaceType == USART_SLAVE_485)
        {
            //��������ӿڿ�����Ϣ
            HndlCHSthCtrlMsg(&g_stSouthInterface);
        }            
    }
}

/*************************************************
  Function:     HndlSimSMS
  Description:  ����������sim����Ķ���Ϣ                
  Calls:
  Called By:
  Input:
  Output:     
  Return:         
  Others:
*************************************************/
void HndlSimSMS(void)
{
    OS_Q_DATA stOSQData;
    COMM_OBJ_ST *pstCOMMObj = &g_stNorthInterface;
    if(pstCOMMObj->ucInterfaceType != USART_MODEM_OR_DIRECT)
    {
        return;
    }
    //����ӿڿ��е�ʱ���ٴ���sim��
    if(CHSendReady(pstCOMMObj) != TRUE)
    {
        return;
    }

    //�жϱ�������Ƿ�æ
    OSQQuery(g_pstCHNthQue, &stOSQData);
    //��æ��ͨ�������ӵ�����´���sim��
    if((stOSQData.OSNMsgs > 0)||(g_ucCurrCommMode != COMM_NO_CONN))
    {
        return;
    }

    if(g_ucCurrSimSMSSta == SIM_STA_UNREAD)
    {

        SendStrToUsart(pstCOMMObj, g_aucSAT_CNMI2, 
               sizeof(g_aucSAT_CNMI2)-1);
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)==TRUE)
        {                    
            //�������ж���
            SendStrToUsart(pstCOMMObj, g_aucSAT_CMGL,
                sizeof(g_aucSAT_CMGL)-1);
            //�жϽ��
            if(AtRtnIsOk(pstCOMMObj,60000,CH_SENDAT_NORMAL) == TRUE)
            {
                //ɾ�����ж���
                SendStrToUsart(pstCOMMObj, g_aucSAT_CMGD,
                    sizeof(g_aucSAT_CMGD)-1);
                //�жϽ��
                if(AtRtnIsOk(pstCOMMObj,8000,CH_SENDAT_NORMAL) == TRUE)
                {
                    //�ı����Ϣ״̬
                    g_ucCurrSimSMSSta = SIM_STA_NORMAL;
                }
                //ɾ������Ϣʧ��
                else
                {
                    //�ı����Ϣ״̬
                    g_ucCurrSimSMSSta = SIM_STA_READED;
                }
            }
            //�����ж���ʧ���򱣳�ԭ״̬������ʱ�ѳɹ����CNMI=22000�����Բ��õ����ж��Ż�©��
            else
            {
                //do nothing
            }
        }
    }
    else if(g_ucCurrSimSMSSta == SIM_STA_READED)
    {
        //ɾ�����ж���
        SendStrToUsart(pstCOMMObj, g_aucSAT_CMGD,
            sizeof(g_aucSAT_CMGD)-1);
        //�жϽ��
        if(AtRtnIsOk(pstCOMMObj,8000,CH_SENDAT_NORMAL) == TRUE)
        {
            //�ı����Ϣ״̬
            g_ucCurrSimSMSSta = SIM_STA_NORMAL;
        }
        //ɾ������Ϣʧ��
        else
        {
            //�ı����Ϣ״̬
            g_ucCurrSimSMSSta = SIM_STA_READED;
        }
    }
}

void CHTask(void *pv)
{
    void *pvPrmv = NULL;

    //�����ʼ��

    //APAC���ݰ���ʼ��,��β���ַ�(���ֲ���ʼ��)
    UCHAR aucData1[DATAPACK_CMCC_APAC_SIZE];//�����ڴ���APAC���ݰ�
    g_stDataPackCMCCAPAC.ulDataPackMaxSize = DATAPACK_CMCC_APAC_SIZE;
    g_stDataPackCMCCAPAC.pucData = aucData1;
    g_stDataPackCMCCAPAC.ucState = DATAPACK_STATE_NO;
    g_stDataPackCMCCAPAC.pucInPos = g_stDataPackCMCCAPAC.pucData;

    //APB���ݰ���ʼ��,��β���ַ�(���ֲ���ʼ��)
    UCHAR aucData2[DATAPACK_CMCC_APB_SIZE];//�����ڴ���APB���ݰ�
    g_stDataPackCMCCAPB.ulDataPackMaxSize = DATAPACK_CMCC_APB_SIZE;
    g_stDataPackCMCCAPB.pucData = aucData2;
    g_stDataPackCMCCAPB.ucState = DATAPACK_STATE_NO;
    g_stDataPackCMCCAPB.pucInPos = g_stDataPackCMCCAPB.pucData;

    //AT�������ݰ���ʼ��,��β˫�ַ�(���ֲ���ʼ��)
    UCHAR aucData3[DATAPACK_ATCMD_SIZE];//�����ڴ���ATCMD���ݰ�
    g_stDataPackAT.ulDataPackMaxSize = DATAPACK_ATCMD_SIZE;
    g_stDataPackAT.pucData = aucData3;
    g_stDataPackAT.ucState = DATAPACK_STATE_NO;
    g_stDataPackAT.pucInPos = g_stDataPackAT.pucData;

    //AT�������ݰ���ʼ��
    UCHAR aucData4[DATAPACK_ATFULL_DATA1_SIZE];//�����ڴ���ATFULL���ݰ�1
    UCHAR aucData5[256];//�����ڴ���ATFULL���ݰ�2
    g_stDataPackATFull.ulDataPack1MaxSize= DATAPACK_ATFULL_DATA1_SIZE;
    g_stDataPackATFull.pucData1 = aucData4;
    g_stDataPackATFull.ulDataPack2MaxSize= 256;
    g_stDataPackATFull.pucData2 = aucData5;    
    g_stDataPackAT.ucState = DATAPACK_STATE_NO;

    //����Э�����ݰ���ʼ��
    UCHAR aucData6[DATAPACK_PRIVATE_SIZE];//�����ڴ�������Э�����ݰ�
    g_stDataPackPrivate.ulDataPackMaxSize = DATAPACK_PRIVATE_SIZE;
    g_stDataPackPrivate.pucData = aucData6;
    g_stDataPackPrivate.ucState = DATAPACK_STATE_NO;
    g_stDataPackPrivate.pucInPos = g_stDataPackPrivate.pucData;

    //��������Э�����ݰ���ʼ��
    UCHAR aucData7[DATAPACK_MS_SIZE];//�����ڴ�
    g_stDataPackNthMS.ulDataPackMaxSize = DATAPACK_MS_SIZE;
    g_stDataPackNthMS.pucData = aucData7;
    g_stDataPackNthMS.ucState = DATAPACK_STATE_NO;
    g_stDataPackNthMS.pucInPos = g_stDataPackNthMS.pucData;

    //��������Э�����ݰ���ʼ��
    UCHAR aucData8[DATAPACK_MS_SIZE];//�����ڴ�
    g_stDataPackSthMS.ulDataPackMaxSize = DATAPACK_MS_SIZE;
    g_stDataPackSthMS.pucData = aucData8;
    g_stDataPackSthMS.ucState = DATAPACK_STATE_NO;
    g_stDataPackSthMS.pucInPos = g_stDataPackSthMS.pucData;

    //���Գ���Э�����ݰ���ʼ��
    UCHAR aucData9[DATAPACK_PRIVATE_SIZE];//�����ڴ�������Э�����ݰ�
    g_stDataPackDebPrivate.ulDataPackMaxSize = DATAPACK_PRIVATE_SIZE;
    g_stDataPackDebPrivate.pucData = aucData9;
    g_stDataPackDebPrivate.ucState = DATAPACK_STATE_NO;
    g_stDataPackDebPrivate.pucInPos = g_stDataPackPrivate.pucData;

    //����APACЭ�����ݰ���ʼ��
    UCHAR aucData10[DATAPACK_PRIVATE_SIZE];//�����ڴ�������Э�����ݰ�
    g_stDataPackDebAPAC.ulDataPackMaxSize = DATAPACK_CMCC_APAC_SIZE;
    g_stDataPackDebAPAC.pucData = aucData10;
    g_stDataPackDebAPAC.ucState = DATAPACK_STATE_NO;
    g_stDataPackDebAPAC.pucInPos = g_stDataPackPrivate.pucData;
    
    //test
    /*
    g_stNorthInterface.pvCOMMDev = &g_stUsart0;//232(����ʱʹ�øô���)
    g_stSouthInterface.pvCOMMDev = &g_stUsart2;//485
    g_stDebugInterface.pvCOMMDev = &g_stUsart1;//232(�ݲ�ʹ�õ��Դ���)
    g_stNorthInterface.ucInterfaceType = USART_SLAVE_OPT;
    g_stSouthInterface.ucInterfaceType = USART_SLAVE_485;
    g_stDebugInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
    
    g_stNmParamSet.ucDevNum = 0;              //�豸���
    g_stYkppParamSet.stYkppCommParam.ucSNum = 1;
    g_stYkppParamSet.stYkppCommParam.usSBitmap = 0xFFFF;
    */
    //test RTC
    /*
    UCHAR tmp1[7];
    
    tmp1[0] = 0x20;
    tmp1[1] = 0x06;
    tmp1[2] = 0x07;
    tmp1[3] = 0x17;
    tmp1[4] = 0x09;
    tmp1[5] = 0x13;
    tmp1[6] = 0x00;
    
    RtcOpen();
    RtcSet24();
    //RtcSetDateTime(tmp1);
    while(1)
    {
        MyDelay(1000);
        RtcGetDateTime(tmp1); 
    }
    */
   
        
    //��ʼ������ӿ�
    InitNth(&g_stNorthInterface);

    //��ʼ������ӿ�
    //notice��Ҫ�ȱ����ʼ����Ϻ��ٳ�ʼ������
    InitSth(&g_stSouthInterface);
    
    //������ѯС����Ϣ��ʱ����������ʱ��
    if(g_stNorthInterface.ucInterfaceType == USART_MODEM_OR_DIRECT)
    {
        StartGeneralTimer(&g_stTimerZoneInfo, TIME_GET_ZONEINFO/TIMER_UNIT_MS, NULL,PERIODIC);
        StartGeneralTimer(&g_stTimerReset, TIME_TIMEOUT_RESET/TIMER_UNIT_MS, NULL,PERIODIC);
    }

    //����ѭ��
    while(1)
    {
        //MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "CHTASK START RUN"));
        //MY_LOG_MSG_BIN((LS_TRACE, LL_INFO, (UCHAR*)&g_ulTest,4));
        //g_ulTest++;
        //MY_TRACE((LS_TRACE, LL_INFO, MID_CH, 0xfe));
        
        //������ӿڽ�������
        HndlCHNthRec(&g_stNorthInterface);
        
        //��������ӿڽ�������
        HndlCHSthRec(&g_stSouthInterface);
        
        //������Խӿڽ�������
        HndlCHDebRec(&g_stDebugInterface);


        //������ӿڿ�����Ϣ
        HndlCHNthCtrlMsg(&g_stNorthInterface);

        //����sim���϶���Ϣ
        HndlSimSMS();

        //������Խӿڿ�����Ϣ


        //������ӿڷ�������
        if(CHSendReady(&g_stNorthInterface) == TRUE)
        {
            //������Ϣ
            pvPrmv = OSQAccept(g_pstCHNthQue, &g_ucErr);
            if(pvPrmv != (void*)NULL)
            {
                HndlCHNthSend(&g_stNorthInterface, pvPrmv);
                //���۷����Ƿ�ɹ�,ԭ���ݾ����ظ�OH,�������ͷ��ڴ�
                //MyFree(pvPrmv);
            }
            //û����Ϣ�ӻ�Ҫ����ѯ��Ӧ
            else if(g_stNorthInterface.ucInterfaceType == USART_SLAVE_OPT ||
                g_stNorthInterface.ucInterfaceType == USART_SLAVE_485)
            {
                UCHAR aucData[20];
                ULONG ulSize;
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_ANS,0, 
                        aucData, 0);
                NthSendtoOPTor485(&g_stNorthInterface, aucData, ulSize);
                //��Ӧ��ѯ����Ҫ��ͨ�Žӿ�æ
                g_stNorthInterface.ucState = COMM_STATE_SEND_BUSY;
            }            
        }

        //��������ӿڷ�������
        CHSthReadyAndSend();
        
        //������Խӿڷ�������
        //������Ϣ
        if(CHSendReady(&g_stDebugInterface) == TRUE)
        {
            pvPrmv = OSQAccept(g_pstCHDebQue, &g_ucErr);
            if(pvPrmv != (void*)NULL)
            {
                HndlCHDebSend(&g_stDebugInterface, pvPrmv);
            }           
        }


        MyDelay(CH_DELAY_TIME);

    }
}

/*************************************************
  Function:       HndlCHNthRec
  Description:   ���������ö���Ϣ�е�ַ���͵�Ԫ��ֵ������Ǳ���Ӧ��İ���
                      ��ַ���Ͳ��ö��Ž���ʱ���������ĵ�ֵַ�����Ҳ��Ե绰����
                      ���б༭���������������İ��������������϶�PDU�е绰����
                      ��ز�����������
                      1��0x91 �� "86" �� �ֻ�����
                      2��0xA1 �� SP����
  Calls:
  Called By:
  Input:          pcTelNum:���ͺ���
                     aucPDUData:����ϢPDU��
                     ulCurrIndex:ָ��aucPDUData��ǰ���뵥Ԫ
                     ucDA:����Ϣ��ַ����
  Output:     
  Return:         
  Others:
*************************************************/
void SetDA(CHAR *pcTelNum, UCHAR aucPDUData[], ULONG &ulCurrIndex,UCHAR ucDA)
{
    UCHAR ucDALen;
    UCHAR i , j ;
    UCHAR ucCompLen;
    
    //д��DALEN
    ucDALen = (UCHAR)FindChar((UCHAR*)pcTelNum, 0, 1, MAX_TEL_NUM_LEN+1);

    //��ֹ�绰����������
    if(ucDALen == FIND_NO_CHAR)
    {
        ucDALen = MAX_TEL_NUM_LEN;
    }

    //��ֹ����+
    if(pcTelNum[0] == '+')
    {
        for(i = 0; i < ucDALen-1 ; i++)
        {
            pcTelNum[i] = pcTelNum[i+1];
        }
        ucDALen--;
        pcTelNum[ucDALen] = 0x00;
    }

    if((ucDA == 0xA1)||(ucDA == 0x91))//����Ӧ��İ�
    {
        aucPDUData[ulCurrIndex] = ucDALen;
        ulCurrIndex++;
        
        aucPDUData[ulCurrIndex] = ucDA;
    }
    else//��������İ�
    {
        for(i = 0 ; i < MAX_SMS_ADDR_PREFIX_NUM ; i++)
        {
            ucCompLen = g_stYkppParamSet.stYkppSmsAddrParam.astSmsAddrPrefix[i].ucLen;
            if((ucCompLen == 0)||(ucCompLen >ucDALen))//С��ǰ׺�ĳ���
            {
                continue;
            }
    
            for(j = 0 ; j < ucCompLen ; j++)
            {
                if(pcTelNum[j] != g_stYkppParamSet.stYkppSmsAddrParam.astSmsAddrPrefix[i].aucPrefix[j])
                {
                    break;
                }
            }
    
            if(j >= ucCompLen)//ƥ��ɹ�
            {
                 //�ں���ǰ������"86"
                 for(j = ucDALen; j > 0 ; j--)
                {
                    pcTelNum[j-1+2] = pcTelNum[j-1];
                }
                pcTelNum[0] = '8';
                pcTelNum[1] = '6';
                ucDALen  += 2;
                pcTelNum[ucDALen] = 0x00;
            }
        }
        
        aucPDUData[ulCurrIndex] = ucDALen;
        ulCurrIndex++;

        //д��TypeOfAddr�����ǰ׺��86���ǹ��ʺ��룬����Ϊ���ں���
        if((pcTelNum[0] == '8') && (pcTelNum[1] == '6'))
        {
            aucPDUData[ulCurrIndex] = 0x91;//91��ʾ���ʺ���
        }
        else
        {
            aucPDUData[ulCurrIndex] = 0xA1;//A1��ʾ���ں���
        }
    }
    ulCurrIndex++;

    //д��绰����
    ucDALen = WriteTelNumToPDU(aucPDUData+ulCurrIndex, ucDALen, pcTelNum);
    ulCurrIndex = ulCurrIndex + ucDALen;
}
