/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   CMCC_1.h
    ����:     ����
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  2006/02/16
       ����:  ����
       ����:  �������
    2. ����:  2006/11/14
       ����:  �ӻ���
       ����:  �޸�����MCM-32��
              �ӵ��л���ذ汾������ʱ���ݳ����������������Ӧ�ı�־λ��
              ʹ��ʱδ���еĳ���bootloader�´�����ʱѡ�����С���Э��
              �����ݴ�����Ϻ����㣬������������ʹ�豸������
    3. ����:  2007/01/05
       ����:  �ӻ���
       ����:  �޸�����MCM-50,
              ���ļ����������������Ӳ�����ulUnSaveBlocks����              
---------------------------------------------------------------------------*/
#ifndef CMCC_1_H
#define CMCC_1_H

#include "UserType.h"
#include "CMCC_1_Ext.h"
#include "../AppMain.h"

//Э�����Ͷ���
#define AP_A    1
#define AP_B    2
#define AP_C    3
#define VP_A    1 //��NP_A
#define MCP_A   1
#define MCP_B   2

//����Э�����ݵ���󳤶�
#define MAX_APA_LEN     256 //256 Bytes��ָ��"Э������"��"У�鵥Ԫ"���һ���ֽڣ��ڷ��͹���ת�崦��֮ǰ���ֽ�����
#define MAX_APB_LEN     70  //Э�����󳤶���ָ��"��ʼ��־"��"������־"���ڷ��͹���ASCII���ִ���֮����ֽ���
                            //��ʹ��MODEM��SMS����ʽͨ��ʱ��Э�������󳤶ȣ�140 Bytes
#define MAX_APC_LEN     4608//Э�������󳤶ȣ�4096 Bytes��ָ��"Э������"��"У�鵥Ԫ"���һ���ֽڣ��ڷ��͹���ת�崦��֮ǰ���ֽ�����ת��֮����ܴ���4096������ʹ��4608��



//#define APAC_START_FLAG     0x7E    //APA,APC��ʼ��־ ~
//#define APAC_END_FLAG       0x7E    //APA,APC������־ ~
//#define APB_START_FLAG      0x21    //APB��ʼ��־ !
//#define APB_END_FLAG        0x21    //APB������־ !

//VP�㽻����־
#define VP_FLAG_SUCCEEDED           0
#define VP_FLAG_FAILED              1
#define VP_FLAG_COMMAND             0x80

//MCP�����ʶ
#define MCPA_CMD_ID_REPORT              1
#define MCPA_CMD_ID_QUERY               2
#define MCPA_CMD_ID_SET                 3
#define MCPA_CMD_ID_SWITCH_TO_UPGRADE   0x10

//MCM-32_20061114_zhonghw_begin
#define MCPA_CMD_ID_SWITCH_FILE_VER     0x11
//MCM-32_20061114_zhonghw_end

#define MCPA_CMD_ID_EXT_SET             0x83

#define MCPB_CMD_ID_READ            2
#define MCPB_CMD_ID_WRITE           3

//MCPӦ���־
#define RSP_FLAG_SUCCEEDED          0   //�ɹ�
#define RSP_FLAG_PARTIALLY_DONE     1   //���������ִ��
#define RSP_FLAG_CMD_ID_ERR         2   //�����Ŵ�
#define RSP_FLAG_LEN_ERR            3   //���ȴ�
#define RSP_FLAG_CRC_ERR            4   //CRCУ���
#define RSP_FLAG_OTHER_ERR          0xFE//��������
#define RSP_FLAG_CMD                0xFF//����

//��ض���������
#define MOBJ_CORRECT                0   //��ȷ
#define MOBJ_ID_UNRECOGNIZED        1   //������ݱ�ʶ�޷�ʶ��
#define MOBJ_VALUE_OUT_OF_BOUND     2   //������ݵ�����ֵ������Χ
#define MOBJ_VALUE_NOT_MATCH        3   //������ݱ�ʶ�������ݵ�ֵ������Ҫ��
#define MOBJ_LEN_NOT_MATCH          4   //������ݱ�ʶ�������ݳ��Ȳ�ƥ��
#define MOBJ_VALUE_TOO_LOW          5   //������ݵļ��ֵ���ڹ�����Χ
#define MOBJ_VALUE_TOO_HIGH         6   //������ݵļ��ֵ���ڹ�����Χ
#define MOBJ_OTHER_ERR              9   //δ�г�����������

//����ģʽ
#define RUNNING_MODE_MONITOR        0
#define RUNNING_MODE_UPGRADE        1

//MCP:B��������
#define COMFIRM_PER_PACKET          0   //����ÿ��ȷ�Ϸ�ʽ
#define MULTI_PACKET_ONE_COMFIRM    1   //���ö���һ��ʽ

//�����Ľ��
#define UPGRADE_DONE                 0   //�Ѿ��ɹ��������
#define UPGRADE_ABORTED              1   //�豸��ֹ�������
#define UPGRADE_CANCELLED            2   //OMCȡ���������
#define UPGRADE_VALID_FILE_FAILED    3   //�ļ����ʧ��
#define UPGRADE_SAVE_FILE_FAILED     4   //�����ļ�ʧ��
#define UPGRADE_COMM_FAILED          5   //Զ��ͨ����·�쳣
#define UPGRADE_COMM_TIMEOUT         6   //Զ��ͨ�ų�ʱ
#define UPGRADE_FTP_DOWNLOAD_TIMEOUT 7   //�ļ�FTP���س�ʱ
#define UPGRADE_FTP_LOGIN_FAILED     8   //�ļ���¼FTP������ʧ��
#define UPGRADE_OTHER_ERR            17  //�����쳣�ж��������
#define UPGRADE_NOT_OCCURED          0xFF//û�з���������

//�Ƿ�ʹ��GPRS
#define USE_GPRS                    0
#define NOT_USE_GPRS                1

//�ϱ�����
#define REPORT_TYPE_ALARM               1
#define REPORT_TYPE_STA_INIT            2
#define REPORT_TYPE_INSPECTION          3
#define REPORT_TYPE_REPAIR_CONFIRMED    4
#define REPORT_TYPE_CONFIG_CHANGED      5
#define REPORT_TYPE_LOGIN               6
#define REPORT_TYPE_HEARTBEAT           7
#define REPORT_TYPE_UPGRADE             8
#define REPORT_TYPE_COMM_LINK_FAULT     9

#define OTHER_REPORT_COUNT          (REPORT_TYPE_UPGRADE - REPORT_TYPE_ALARM)
#define OTHER_REPORT_BASE           REPORT_TYPE_STA_INIT

//�ļ��������
#define TRANS_CTRL_START            1//20060417zhw
#define TRANS_CTRL_END              2
#define TRANS_CTRL_CANCEL           3
#define TRANS_CTRL_UPGRADE_COMPLETE 4

#define TRANS_CTRL_INIT             0xFF//���Ҷ��壬��ת������ģʽʱ�����״̬
#define TRANS_CTRL_TRANSFERRING     0xFE//���Ҷ��壬���յ���һ�����ݰ�������״̬
#define TRANS_CTRL_ABORT            0xFD//���Ҷ��壬������������Ҫ�ж��˳�ʱ�����״̬

//�ļ����ݰ�Ӧ��
#define TRANS_RSP_OK                0   //�ɹ����գ����Լ������պ������ݰ�
#define TRANS_RSP_RETRANS_REQ       1   //����OMC�ط����ݰ���ǰ�ᣬ֮ǰ�İ��ɹ����գ�
#define TRANS_RSP_DELAY_REQ         2   //����OMC��ʱTP������������ݰ���ǰ�ᣬ�˰��ɹ����գ�
#define TRANS_RSP_CANCEL_REQ        3   //����OMCȡ���������
#define TRANS_RSP_MNFT_ID_ERROR     4   //�����ļ��еĳ��ұ�ʶ�����豸��ֹ�������
#define TRANS_RSP_DEV_ID_ERROR      5   //�����ļ��е��豸��ʶ�����豸��ֹ�������
#define TRANS_RSP_OTHER_ERROR       6   //�����ļ��е����������豸��ֹ�������

//�жϼ�ض�����ȷ�ԣ���ΪUCHAR/uint2/str/dec�ֱ��ж�
#define UINT_1 1
#define SINT_1 1
#define UINT_2 2
#define SINT_2 2
#define UINT_4 4
#define STR_10 10
#define STR_20 20
#define STR_40 40

#define TWO_BYTES       2
#define LO_BYTE         0 //���ֽ����ݵĵ�һ���ֽ�
#define HI_BYTE         1 //���ֽ����ݵĵڶ����ֽ�

//Ŀǰʹ�ö���һ��������ϵ��Ϊ1����˲�ʹ��T4��ֻʹ��T5���Ժ����ʹ�ö���һ��������ϵ������1ʱ��ʹ��T4
#define TIMER_TRANS_CTRL_INTV       (30000 / TIMER_UNIT_MS) //10 seconds
#define TIMER_TRANS_BLK_INTV        (30000 / TIMER_UNIT_MS) //10 seconds

#define TIMER_ALARM_SHORT_INTV      (180000 / TIMER_UNIT_MS)   //3 mins
#define TIMER_ALARM_LONG_INTV       (10800000 / TIMER_UNIT_MS) //3 hours

#define TIMER_HEARTBEAT_TO_INTV     (15000 / TIMER_UNIT_MS)  //�ݶ�15��
#define TIMER_HEARTBEAT_PERIOD_INTV (60000 / TIMER_UNIT_MS)  //Э��涨Ĭ��ֵΪ1����

#define TIMER_LOGIN_TO_INTV         (15000 / TIMER_UNIT_MS)  //�ݶ�15��
#define TIMER_LOGIN_PERIOD_INTV     (180000 / TIMER_UNIT_MS) //��С��3����


//***************Э��ջ����***************
#if defined(__BORLANDC__) || defined(_MSC_VER)
#pragma pack(1) //�����������ݶ�����1�ֽڶ���
#define __attribute__(a)
#endif

//AP:A,AP:B,AP_C����������ݿ���Ϊ4�ֽڣ�����APЭ�����ͣ�PDU���ͺ�CRCУ���룬��������ʼ������־
#define AP_OVERHEAD 4
typedef struct
{
    UCHAR ucApType;
    UCHAR ucPduType;
    UCHAR aucPdu[1];
}__attribute__((packed)) AP_LAYER_ST;

//VP:A����������ݿ���Ϊ9�ֽ�
#define VP_OVERHEAD         9
#define MCP_MIN_LEN         2 //�������Ԫ��Ӧ���־
typedef struct
{
    ULONG  ulStaNum;
    UCHAR  ucDevNum;
    USHORT usPacketID;
    UCHAR  ucVpFlag;
    UCHAR  ucApId;
    UCHAR  aucPdu[MCP_MIN_LEN]; //����Ҫ���������ʶ��Ӧ���־�������ֽ�
}__attribute__((packed)) VPA_LAYER_ST;

//MCP:A��MCP:B�����Ԫ����Ϊ2�ֽ�
#define MCPA_MOBJ_MIN_LEN   3 //�������󳤶ȺͶ����ʶ
#define MCPB_MOBJ_MIN_LEN   4
//MCM-32_20061114_zhonghw_begin
#define MCPA_DATA_SWITCH_VER_LEN      10 //�л��������汾MCP_A���ݵ�Ԫ����,�������Ԫ������ID
#define MCPA_MOBJ_SWITCH_VER_MIN_LEN  4  //�л��������汾����������ݵ�Ԫ����
//MCM-32_20061114_zhonghw_end
typedef struct
{
    UCHAR ucCmdId;
    UCHAR ucRspFlag;
    UCHAR aucContent[1];
}__attribute__((packed)) MCP_LAYER_ST;

typedef struct
{
    UCHAR ucMObjLen;
    UCHAR aucMObjId[TWO_BYTES];
    UCHAR aucMObjContent[1];
}__attribute__((packed)) MCPA_MOBJ_ST;

typedef struct
{
    USHORT usMObjLen;
    UCHAR  aucMObjId[TWO_BYTES];
    UCHAR  aucMObjContent[1];
}__attribute__((packed)) MCPB_MOBJ_ST;


#if defined(__BORLANDC__) || defined(_MSC_VER)
#pragma pack()
#endif









//***************��ض���ṹ�嶨��***************
//MCP:B�ļ�ز�����������NV Memory��
typedef struct
{
    ULONG  ulNextBlockSn;       //��һ���ļ����ݿ����
    ULONG  ulMaxBlockSize;      //֧�ֵ����ݿ鳤��
    ULONG  ulSavedFileSize;     //�ѱ�����ļ����ȣ�����Э��Ҫ�������
    LONG   lUpgradeResult;      //�����Ľ��
    //UCHAR  aucFileId[STR_LEN];  //�ļ���ʶ�룬�����ڲ�ͬ�ļ��У��������ڴ˴�
    //UCHAR  aucFile[1];          //�ļ��������������ڲ�ͬ���ļ��У��������ڴ˴�
}UPGRADE_INFO_SET_ST;




//���±�ע�͵�����ԭ������Ϊʹ�ýṹλ����ʱ�����
//#define LOCAL_ALARM_BIT     0
//#define CENTER_ALARM_BIT    2
//#define ENABLE_BIT          5
//#define SUPPORT_BIT         7

//#define GetBit(obj, bit)  (((obj) >> (bit)) & 1)
//#define SetBit(obj, bit, value) ((obj) &= (~(1 << (bit))), (obj) |= ((value & 1) << bit))

//#define IsAlarmItemSupported(ucInfo)  GetBit((ucInfo), SUPPORT_BIT)
//#define IsAlarmItemEnabled(ucInfo)    GetBit((ucInfo), ENABLE_BIT)

//#define SetAlarmItemSupportedBit(ucInfo, ucValue)   SetBit((ucInfo), SUPPORT_BIT, (ucValue))
//#define SetAlarmItemEnabledBit(ucInfo, ucValue)     SetBit((ucInfo), ENABLE_BIT, (ucValue))

//#define GetLocalAlarmStatus(ucInfo)     GetBit((ucInfo), LOCAL_ALARM_BIT)
//#define GetCenterAlarmStatus(ucInfo)    GetBit((ucInfo), CENTER_ALARM_BIT)

//#define SetLocalAlarmStatus(ucInfo, ucStatus)   SetBit((ucInfo), LOCAL_ALARM_BIT, (ucStatus))
//#define SetCenterAlarmStatus(ucInfo, ucStatus)  SetBit((ucInfo), LOCAL_ALARM_BIT, (ucStatus))


//�ѷ��͵ĸ澯���������һ���澯�ϱ������ͺ����������ĸ澯��ͱ�       
//�����ѷ��͸澯������У��Ұ���ID��С�����˳�����С��澯�������е�
//ulAlarmItemBegin��ָ�򻺳����е�ĳһλ�á����յ��澯Ӧ��󣬽�ʹ��
//�ѷ��͸澯�������н����Ӧ�ĸ澯��������ĸ澯״̬��

//�澯���ͻ������е�ÿһ���澯����԰�������Ľṹ������
typedef struct
{
    UCHAR ucIDLoByte;   //�澯��ID�ĵ��ֽ�
    UCHAR ucUsed   : 1; //�ø澯���Ƿ���Ч
    UCHAR ucStatus : 1; //�澯��״̬
}ALARM_ITEM_IN_BUF_ST;

//#define MAX_ALARM_ITEMS                 ((sizeof(ALARM_ITEM_SET_ST)) / sizeof(ALARM_ITEM_INFO_ST))
#define MOBJ_ALARM_LEN                  sizeof(MCPA_MOBJ_ST)

//�澯���ͻ�����
typedef struct
{
    ULONG ulAlarmItemCount;
    ALARM_ITEM_IN_BUF_ST astAlarmItems[MAX_ALARM_ITEMS];
}ALARM_ITEM_BUF_ST;

//������ʱ����
class Timer;

//�澯�ϱ�������������
typedef struct
{
    Timer  *pTimer;          //��ʱ������
    ULONG  ulTimerMagicNum;  //��ʱ����magic number
    USHORT usPacketID;       //�����ϱ�����ţ����ڶ�Ӧ�澯�ϱ�Ӧ��
    UCHAR  ucCommState;      //ͨ��״̬����ΪIDLE,SEND_DATA_PENDING,WAIT_FOR_RSP
    UCHAR  ucResendState;    //�澯�ط���״̬
    UCHAR  ucAlarmItemCount; //�����ϱ��и澯��ĸ���
    UCHAR  ucAlarmItemBegin; //�澯�����ѷ��͸澯�������е���ʼƫ��
}ALARM_REPORT_CTX_ST;


//�����ϱ���������
typedef struct
{
    Timer  *pTimer;              //��ʱ������
    ULONG  ulTimerMagicNum;      //��ʱ����magic number
    USHORT usPacketID;           //�����ϱ�����ţ����ڶ�Ӧ�ϱ�Ӧ��
    UCHAR  ucCommState;          //ͨ��״̬
}OTHER_REPORT_CTX_ST;


//�����ļ��Ļ�����
#define FLASH_SECTOR_SIZE 0x1000 //4k����Ӧ��һ��FLASH��SECTOR
typedef struct
{
    ULONG ulUsedSize;
    UCHAR aucBuf[FLASH_SECTOR_SIZE];
}FLASH_BUF_ST;

//�����ļ���������
typedef struct
{
    ULONG ulNextBlockSn;        //��һ�����ݿ�����кţ��ļ�����У��ɹ��������NV Memory
    ULONG ulMaxBlockSize;       //ÿ����󳤶ȣ���ʼ��ʱ��NV Memory�ж���
    ULONG ulSavedFileSize;      //�ѱ���������ļ���С��д��NV Memory��Ÿ���SN
    LONG  lUpgradeResult;       //�����Ľ������Ҫ���浽NV Memory��    

    //MCM-50_20070105_zhonghw_begin
    ULONG ulUnSaveBlocks;       //�Ѿ����յ���δ�������ݿ飬��Χ��0~(�����С/����С)
    //MCM-50_20070105_zhonghw_end    
    ULONG ulUpgradePartIdx;     //��������ļ��ķ����±�
    ULONG ulUpgradeProgress;    //�����Ľ���
    Timer *pTimer;              //��ʱ������
    ULONG ulTimerMagicNum;      //��ʱ����magic number
    FLASH_BUF_ST stFileBuf;     //�����ļ��Ļ�����
}UPGRADE_CTX_ST;

//��ز����Ĵ���������
typedef LONG (*MOBJ_HNDLR)(MCPA_MOBJ_ST *pstMcpA, ULONG ulParamAddr, UCHAR ucDataType, 
                           UCHAR ucIsSupported, UCHAR ucOperation);

#define PARAM_NOT_SUPPORTED     0
#define PARAM_SUPPORTED         1

//��ز����������Ҫ������ʱ���г�ʼ�����Ըýṹ������±���Ϊ����ID�ĵ��ֽ�
typedef struct
{
    ULONG ulAddr;            //�����Ĵ洢��ַ����Ӧ�ڴ洢�ṹ�е�λ�� 
    MOBJ_HNDLR pfHandler;    //������ز����ĺ��������ڸ澯���Ƚ�ͳһ����ʼ���Բ���Ҫ��ͬ�Ĳ�������
    UCHAR ucDataType;        //�������������� 
    UCHAR ucIsSupported;     //�ò����Ƿ�֧�֣��ڳ�ʼ��ʱ�ɲ����б����
    //UCHAR ucIsWritable;      //�ò�����д
}MOBJ_HNDLR_TBL_ST;

//�澯ʹ�ܺ͸澯״̬�����������Ҫ������ʱ���г�ʼ����������ṹ��ͬ���ǲ���ID�ĵ��ֽڷ��ڽṹ����
typedef struct
{
    ULONG ulAddr;            //�����Ĵ洢��ַ����Ӧ�ڴ洢�ṹ�е�λ�� 
    MOBJ_HNDLR pfHandler;    //������ز����ĺ��������ڸ澯���Ƚ�ͳһ����ʼ���Բ���Ҫ��ͬ�Ĳ�������
    UCHAR ucDataType;        //�������������� 
    UCHAR ucIsSupported;     //�ò����Ƿ�֧�֣��ڳ�ʼ��ʱ�ɲ����б����
    //UCHAR ucIsWritable;      //�ò�����д
    UCHAR ucIDLoByte;        //�澯��ID�ĵ��ֽ�
}ALARM_ITEM_HNDLR_TBL_ST;


//ע��!!!!!!!���澯���⣬ʹ��ÿһ����������һ����ID��Ϊ����Ĵ�С
#define DEV_INFO_HNDLR_TBL_SIZE         (MOBJ_ID_DI_SEC_ALTITUDE + 1)
#define NM_PARAMS_HNDLR_TBL_SIZE        (MOBJ_ID_NM_FILE_NAME + 1)
#define ALARM_ITEMS_HNDLR_TBL_SIZE      (MAX_ALARM_ITEMS) //��Ϊ�澯����ý������У����ʹ�ø澯�������Ϊ�����С
#define SETTING_PARAMS_HNDLR_TBL_SIZE   (MOBJ_ID_SP_SEC_STARTUP_UNDER_VOL_SW + 1)
#define RT_COLL_PARAMS_HNDLR_TBL_SIZE   (MOBJ_ID_RC_SEC_CHARGE_CAPACITY + 1)

//notice �����ط����������ܵķ��ʹ���
#define RESEND_TIME_DATATYPE_RSP            9   //Ӧ���ط��Ĵ���
#define RESEND_TIME_DATATYPE_ALARM          2   //�澯�ϱ��ط��Ĵ���
#define RESEND_TIME_DATATYPE_REPORT         2   //�ϱ��ط��Ĵ���
#define RESEND_TIME_GPRS_LINKFAULT          0   //GPRS����ʧ�ܺ�����ϱ��ط��Ĵ���

                  
#endif                   
                   
                   
                   
           
                   
                   
