/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   YKPP.h
    ����:     ����
    �ļ�˵��: ���ļ�����ʵ�ֶ�ʱ�����ƵĴ���
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  2006/02/16
       ����:  ����
       ����:  �������
    2. ����:  2006/11/3
       ����:  �ӻ���
       ����:  �޸�����MCM-10,�޸Ķദ�жϲ���������첽��������ʽ
    3. ����:  2006/11/29
       ����:  ����
       ����:  �޸�����MCM-42��
              ����ͨ���������������פ������
    6. ����:  2007/01/11
       ����:  �ӻ���
       ����:  �޸�����MCM-54��
              ���Ӳ��������������"YKPP_ERR_ASYN_BUF_FULL".
    7. ����:  2007/03/15
       ����:  �ӻ���
       ����:  �޸�����MCM-64��
              ����ͨ�����ӻ�ID:0x0211��
              �ڳ���Э����Ʋ���������"ͨ�����ӻ�����"��
---------------------------------------------------------------------------*/
#ifndef YKPP_H
#define YKPP_H

#include "UserType.h"
#include "../Log/Log.h"

//�ʿƳ���Э�鶨��

#define YKPP_BEGIN_FLAG '#' //����Э����ʼ��־
#define YKPP_END_FLAG   '*' //����Э�������־

//���뷽��
#define SCHM_A 'A'
#define SCHM_B 'B'
#define SCHM_C 'C'

//��������
#define YKPP_READ_REQ  'R'
#define YKPP_READ_RSP  (YKPP_READ_REQ + 1)
#define YKPP_WIRTE_REQ 'W'
#define YKPP_WIRTE_RSP (YKPP_WIRTE_REQ + 1)
#define YKPP_EXE_REQ   'E'
#define YKPP_EXE_RSP   (YKPP_EXE_REQ + 1)

//�첽�����Ĳ�������,���ƶ�Э��Ĳ����������Ӧ
#define NEED_MORE_OP_READ       2
#define NEED_MORE_OP_WRITE      3

#define FI_CHECKSUM_NEEDED      (1 << 0)
#define FI_DEV_NUM_AVAILABLE    (1 << 1)
#define FI_AVAILABLE            (1 << 7)

#define YKPP_ERR_CHECKSUM        1          //У�������
#define YKPP_ERR_OPER_TYPE       2          //�������ʹ���
#define YKPP_ERR_PARAM_ID        3          //��������Ĳ���ID
#define YKPP_ERR_OPER_FAILED     4          //����ʧ��
#define YKPP_ERR_ASYN_TIMEOUT    5          //�첽������ʱ
//MCM-54_20070111_Zhonghw_begin
#define YKPP_ERR_ASYN_BUF_FULL   6          //�첽������������(Ϊ����Ϣ�������Ӵ˴�������)
//MCM-54_20070111_Zhonghw_end

#define FI_DEV_NUN_AVAILABLE_EN    0X02       //���ӱ��뷽���е��豸��ż�Ȩ
#define FI_DEV_NUN_AVAILABLE_DIS   0XFD       //ȥ�����뷽���е��豸��ż�Ȩ

#define DLIN_MODEN_MEASURE      0          //������MODEN�������������빦��
#define DLIN_MODULE_MEASURE     1          //�ɹ��ʼ��ģ���������������빦��

typedef struct
{
    UCHAR ucSchm;
    UCHAR ucOperType;
    UCHAR ucResult;
    UCHAR ucFieldIndicator;
    UCHAR ucDevNum;
    UCHAR aucParamTbl[1];
}__attribute__((packed)) YKPP_PDU_ST; //��������ʼ������־��У�鵥Ԫ


typedef struct
{
    UCHAR  ucUlRfSw;        //������Ƶ�źſ���
    UCHAR  ucDlRfSw;        //������Ƶ�źſ���
    CHAR   cUlOutMax;      //��������������ֵ
    UCHAR  ucDlOutMax;      //��������������ֵ
    SHORT  sUlOutAdjust;    //�����������У׼ֵ
    SHORT  sDlOutAdjust;    //�����������У׼ֵ
    SHORT  sDlReflAdjust;   //���з��书��У׼ֵ
    UCHAR  ucUlGainMax;     //�����������
    UCHAR  ucDlInCoupler;   //�����������������

//------------------------------����ΪPOI��������-----------------------------------

    CHAR  cCdma800InPwrAdjust;  //CDMA800���빦�ʵ�ƽУ׼ֵ
    CHAR  cMbGsmInPwrAdjust;    //�ƶ�GSM���빦�ʵ�ƽУ׼ֵ
    CHAR  cUcGsmInPwrAdjust;    //��ͨGSM���빦�ʵ�ƽУ׼ֵ
    CHAR  cMbDcsInPwrAdjust;    //�ƶ�DCS���빦�ʵ�ƽУ׼ֵ
    CHAR  cUcDcsInPwrAdjust;    //��ͨDCS���빦�ʵ�ƽУ׼ֵ
    CHAR  c3G1FDDInPwrAdjust;   //3G1-FDD���빦�ʵ�ƽУ׼ֵ
    CHAR  c3G2FDDInPwrAdjust;   //3G2-FDD���빦�ʵ�ƽУ׼ֵ
    CHAR  c3G3TDDInPwrAdjust;   //3G3-TDD���빦�ʵ�ƽУ׼ֵ
    CHAR  cTrunkInPwrAdjust;    //��Ⱥϵͳ���빦�ʵ�ƽУ׼ֵ
    CHAR  cOutPwrAdjust1;       //��һ·������ʵ�ƽУ׼ֵ
    CHAR  cOutPwrAdjust2;       //�ڶ�·������ʵ�ƽУ׼ֵ
    CHAR  cReflectPwrAdjust1;   //��һ·���书�ʵ�ƽУ׼ֵ
    CHAR  cReflectPwrAdjust2;   //�ڶ�·���书�ʵ�ƽУ׼ֵ

//------------------------------����ΪPOI��������-----------------------------------

    //MCM-42_20061129_linwei_begin
    UCHAR  ucDlOutMax2;     //��������������ֵ(ͨ����)
    SHORT  sDlOutAdjust2;   //�����������У׼ֵ(ͨ����)
    SHORT  sDlReflAdjust2;  //���з��书��У׼ֵ(ͨ����)
//------------------------------����Ϊ��������-----------------------------------
    //MCM-42_20061129_linwei_end
    
}YKPP_RF_PARAM_ST;

typedef struct
{
    UCHAR  ucRebootSw;              //�������أ���ʱû��ʹ��
    UCHAR  ucTelNumAuthSw;          //�绰�����Ȩ����
    struct
    {
        UCHAR ucOnOff;              //����
        UCHAR ucMaxAlarmPerHour;    //ÿСʱ�澯�ϱ��������ޣ�������״̬Ϊ����ʱ��Ч
    }stAlarmLimitSw;                //Ƶ���澯���ƿ��أ���ʱû��ʹ��
    
    UCHAR  ucAddrAuthSw;            //��ַ��Ȩ����
    UCHAR  ucYkppRspSw;             //��������Ӧ�𿪹�
    struct
    {
        UCHAR ucTotal;              //һ���ж����ڵ��ܴ���
        UCHAR ucOccurLimit;         //һ���ж������и澯�������޴���
        UCHAR ucEliminateLimit;     //һ���ж������и澯�ָ����޴���
    }stAlarmJudgeParam;             //�澯�жϲ���???
    
    UCHAR ucUpdateMode;             //��������ģʽ����ʱû��ʹ��
    UCHAR ucDebugMode;              //����ģʽ???
    USHORT usBattJudgeStartVoltage; //ģ���ع����ж���ʼ���ص�ѹ��100��
    USHORT usBattJudgeStopVoltage;  //ģ���ع����жϽ������ص�ѹ��100��
    LOG_MGMT_PARAM_ST stLogMgmtParam;
   	UCHAR ucDlInAcqMode;            //�������빦�ʼ�ⷽʽ
   	//MCM-64_20070315_Zhonghw_begin
    UCHAR ucCommRoleType;            //ͨ�����ӻ�:0��ʾͨ��������1��ʾͨ�Ŵӻ�
   	//MCM-64_20070315_Zhonghw_end
}YKPP_CTRL_PARAM_ST;


//�ʿ�����Э����ز����ṹ
typedef struct
{
    UCHAR ucSNum;       //�ӻ����
    UCHAR ucAskingSNum; //������ѯ�Ĵӻ����
    UCHAR ucNewSNum;    //����ע����´ӻ����,0��ʾ�����豸����ע��
    USHORT usSBitmap;   //�ӻ�λͼ
    USHORT usCommAlarm; //����ͨ�Ÿ澯��ʶ��16λ��ʾ16���ӻ�����ӻ�λͼ��Ӧ��1:�澯��0:����
}YKPP_COMM_PARAM_ST;

//MCM-20080329_Zhonghw_begin
#define MAX_SMS_ADDR_PREFIX_NUM    20  //ǰ׺��������
#define MAX_SMS_ADDR_PREFIX_LEN     4   //����ǰ׺����󳤶�

typedef struct
{
UCHAR ucLen;         //ǰ׺�ĳ��ȣ�0��ʾ��ǰ׺����0��ʾҪƥ��ĳ���
CHAR aucPrefix[MAX_SMS_ADDR_PREFIX_LEN]; //ǰ׺�����ַ�
}SMS_ADDR_PREFIX_ST;

typedef struct
{
    //UCHAR ucIsUseNationalSN;  //�Ƿ�ʹ�ù��Ҵ��룬0XF0��ʾ�ֻ�����ʹ��86��0X0F��ʾSP����ʹ��86
    SMS_ADDR_PREFIX_ST astSmsAddrPrefix[MAX_SMS_ADDR_PREFIX_NUM];
}YKPP_SMS_ADDR_PREFIX_ST;
//MCM-20080329_Zhonghw_begin

//�ܵĳ���Э��������ݼ������Ա�����һ��FLASH SECTOR��
typedef struct
{
    YKPP_RF_PARAM_ST    stYkppRfParam;
    YKPP_CTRL_PARAM_ST  stYkppCtrlParam;
    YKPP_COMM_PARAM_ST  stYkppCommParam;
    //MCM-20080329_Zhonghw_begin
    YKPP_SMS_ADDR_PREFIX_ST stYkppSmsAddrParam; //���ǵ����������ǰ�Ĳ�����Ӱ�죬���������
    //MCM-20080329_Zhonghw_end
}YKPP_PARAM_SET_ST;


//***************����Э���ض����ʶ***************
//��Ƶ����
#define YKPP_RF_PARAM_SET                           1        //����ID�ĸ��ֽ�
#define YKPP_RF_PARAM_ULRFSW                        1        //������Ƶ�źſ���   
#define YKPP_RF_PARAM_DLRFSW                        2        //������Ƶ�źſ���
#define YKPP_RF_PARAM_ULOUTMAX                      3        //��������������ֵ
#define YKPP_RF_PARAM_DLOUTMAX                      4        //��������������ֵ
#define YKPP_RF_PARAM_ULOUTADJUST                   5        //�����������У׼ֵ 
#define YKPP_RF_PARAM_DLOUTADJUST                   6        //�����������У׼ֵ  
#define YKPP_RF_PARAM_DLREFLADJUST                  7        //���з��书��У׼ֵ
#define YKPP_RF_PARAM_ULGAINMAX                     8        //�����������
#define YKPP_RF_PARAM_DLINCOUPLER                   9        //�����������������

//------------------------------����ΪPOI��������-----------------------------------

#define YKPP_RF_PARAM_CDMA800_IN_PWR                0x0A    //CDMA800���빦�ʵ�ƽУ׼ֵ
#define YKPP_RF_PARAM_MB_GSM_IN_PWR                 0x0B    //�ƶ�GSM���빦�ʵ�ƽУ׼ֵ
#define YKPP_RF_PARAM_UC_GSM_IN_PWR                 0x0C    //��ͨGSM���빦�ʵ�ƽУ׼ֵ
#define YKPP_RF_PARAM_MB_DCS_IN_PWR                 0x0D    //�ƶ�DCS���빦�ʵ�ƽУ׼ֵ
#define YKPP_RF_PARAM_UC_DCS_IN_PWR                 0x0E    //��ͨDCS���빦�ʵ�ƽУ׼ֵ
#define YKPP_RF_PARAM_3G1_FDD_IN_PWR                0x0F    //3G1-FDD���빦�ʵ�ƽУ׼ֵ
#define YKPP_RF_PARAM_3G2_FDD_IN_PWR                0x10    //3G2-FDD���빦�ʵ�ƽУ׼ֵ
#define YKPP_RF_PARAM_3G3_TDD_IN_PWR                0x11    //3G3-TDD���빦�ʵ�ƽУ׼ֵ
#define YKPP_RF_PARAM_TRUNK_IN_PWR                  0x12    //��Ⱥϵͳ���빦�ʵ�ƽУ׼ֵ
#define YKPP_RF_PARAM_OUT_PWR_ADJUST1               0x13    //��һ·������ʵ�ƽУ׼ֵ
#define YKPP_RF_PARAM_OUT_PWR_ADJUST2               0x14    //�ڶ�·������ʵ�ƽУ׼ֵ
#define YKPP_RF_PARAM_REFL_PWR_ADJUST1              0x15    //��һ·���书�ʵ�ƽУ׼ֵ
#define YKPP_RF_PARAM_REFL_PWR_ADJUST2              0x16    //�ڶ�·���书�ʵ�ƽУ׼ֵ

//------------------------------����ΪPOI��������-----------------------------------

//���Ʋ���
#define YKPP_CTRL_PARAM_SET                         2       //����ID�ĸ��ֽ�
#define YKPP_CTRL_PARAM_REBOOT_SW                   1       //��������                          //
#define YKPP_CTRL_PARAM_TELNUM_AUTH_SW              2       //�绰�����Ȩ����
#define YKPP_CTRL_PARAM_ALARM_LIMIT_SW              3       //Ƶ���澯���ƿ���
#define YKPP_CTRL_PARAM_ADDR_AUTH_SW                4       //��ַ��Ȩ����
#define YKPP_CTRL_PARAM_YKPP_RS_PSW                 5       //��������Ӧ�𿪹�
#define YKPP_CTRL_PARAM_ALARM_JUDGE_PARAM           6       //�澯�жϲ���
#define YKPP_CTRL_PARAM_UPDATE_MODE                 7       //��������ģʽ
#define YKPP_CTRL_PARAM_DEBUG_MODE                  8       //����ģʽ
#define YKPP_CTRL_PARAM_BATT_JUDGE_START_VOLTAGE    9       //ģ���ع����ж���ʼ���ص�ѹ��100��
#define YKPP_CTRL_PARAM_BATT_JUDGE_STOP_VOLTAGE     10      //ģ���ع����жϽ������ص�ѹ��100��
#define YKPP_CTRL_PARAM_LOG_SW                      11      //������־����
#define YKPP_CTRL_PARAM_LOG_OUTPUT                  12      //��־���
#define YKPP_CTRL_PARAM_LOGSRCMASK                  13      //��־Դ����
#define YKPP_CTRL_PARAM_LEVELMASK                   14      //��־��������
#define YKPP_CTRL_PARAM_MIDMASK                     15      //ģ��ID����
#define YKPP_CTRL_PARAM_DLINACQ_MODE                16      //�������빦�ʼ�ⷽʽ 

//MCM-64_20070315_Zhonghw_begin
#define YKPP_CTRL_PARAM_COMM_ROLE_TYPE                  17       //ͨ�����ӻ�
//MCM-64_20070315_Zhonghw_end

//ͨ�Ų���
#define YKPP_COMM_PARAM_SET                         3        //����ID�ĸ��ֽ�
#define YKPP_COMM_PARAM_SLAVE_BITMAP                1        //�ӻ�λͼ
#define YKPP_COMM_PARAM_SLAVE_NUM                   2        //���Ҵӻ����
#define YKPP_COMM_PARAM_SMC_ADDR                    3        //�������ĺ���

//�洢������
#define YKPP_MEM_PARAM_SET                          4        //����ID�ĸ��ֽ�
#define YKPP_MEM_PARAM_EEPROMDATA                   1        //EEPROM����
#define YKPP_MEM_PARAM_ADDRBUSDATA                  2        //FLASH��RAM����

//Э����ز���
#define YKPP_CORRE_PARAM_SET                        5        //����ID�ĸ��ֽ�
#define YKPP_CORRE_PARAM_STAADDR                    1        //��ַ��վ���ź��豸��ţ�

//ִ�в���
#define YKPP_EXE_PARAM_SET                          ':'      //����ID�ĸ��ֽ�
#define YKPP_EXE_PARAM_TOPOLOGY                     1
#define YKPP_EXE_PARAM_REPORT_TYPE                  2        //�����ϱ�
#define YKPP_EXE_PARAM_AT_CMD                       'D'      //AT����
#define YKPP_EXE_PARAM_ALL_ALARM_ENABLE_SW          3        //���и澯ʹ�ܿ���
#define YKPP_EXE_PARAM_RESET                        4        //�ָ����ã�0Ϊ�������ã�1Ϊ��������

//���Բ���
#define YKPP_DEBUG_PARAM_SET                        6        //����ID�ĸ��ֽ�
#define YKPP_DEBUG_PARAM_TIMER_INFO                 1        //��ʱ����Ϣ
#define YKPP_DEBUG_PARAM_STACK_INFO                 2        //��ջ��Ϣ
#define YKPP_DEBUG_PARAM_MEM_INFO                   3        //�ڴ���Ϣ
#define YKPP_DEBUG_PARAM_QUE_INFO                   4        //������Ϣ


//FLASH��ַ��Χ
#define ADDR_FLASH_BEGIN              0x1000000  //��ʼ��ַ
#define ADDR_FLASH_END                0x11FFFFF  //������ַ

//�����ϱ�����
#define REPORT_TYPE_YKPP_STA_INIT            2
#define REPORT_TYPE_YKPP_INSPECTION          3
#define REPORT_TYPE_YKPP_REPAIR_CONFIRMED    4
#define REPORT_TYPE_YKPP_CONFIG_CHANGED      5

#define YKPP_RSP_SW_OFF                      0       //��ʾ����Э��Ӧ�𿪹عر� 
#define YKPP_NEED_NO_RSP                     1       //����Э�������Ӧ 
#define YKPP_NEED_TO_RSP                     0       //����Э������Ӧ    



#endif









