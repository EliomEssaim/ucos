/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   CMCC_1_Init.cpp
    ����:     ����
    �ļ�˵��: ���ļ�ʵ���й��ƶ���ع淶����ӿ�Э���ʼ���Ĳ���
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  
       ����:  ����
       ����:  �������
    2. ����:  2006/11/01
       ����:  �½�
       ����:  �޸�����MCM-5,
              ����ͨ�ŷ�ʽ�Ĵ�����
    3. ����:  2006/11/06
       ����:  ����
       ����:  �޸�����MCM-17��
              �����д�ͨ���ŵ�ͨ��1�Ĳ�����ӳ���Ӧ��
              ������ͨ���ŵĲ����ϣ������豸�Ƕ�ͨ��ʱ����ѯͨ��1�Ĳ�
              ��ʵ�ʷ��ʵ��Ƕ�Ӧ�Ĳ���ͨ���ŵĲ����ĵ�ַ��
    4. ����:  2006/11/23
       ����:  ����
       ����:  �޸�����MCM-37��ÿ����ز������������һ����Ԫ�±��
              ���������С��1������ڳ�ʼ����ز��������ʱ�������±�
              ֵ����"<"�����С��������"<="�����С�����򽫲�������Խ
              �磬��g_astRtCollParamHndlrTbl�ĳ�ʼ����Ӱ�쵽�ڴ�����
              �ڵ�g_stCenterConnStatus��g_stDevTypeTable��Ӱ��ǰ12���ֽڣ�
    5. ����:  2007/11/20
       ����:  �½�
       ����:  �޸�����MCM-80��              
              �����豸����ѡ��̫�������ص�ѹ���ļ�ⷽʽ��
---------------------------------------------------------------------------*/
#include "CMCC_1.h"

extern MOBJ_ID_TBL_ST       g_stMObjIdTbl;
extern DEV_INFO_SET_ST      g_stDevInfoSet;
extern NM_PARAM_SET_ST      g_stNmParamSet;
extern SETTING_PARAM_SET_ST g_stSettingParamSet;
extern RC_PARAM_SET_ST      g_stRcParamSet;
extern ALARM_ENABLE_UN      g_uAlarmEnable;
extern ALARM_ITEM_UN        g_uAlarmItems;

extern MOBJ_HNDLR_TBL_ST g_astDevInfoHndlrTbl[];
extern MOBJ_HNDLR_TBL_ST g_astDevInfoExHndlrTbl[];
extern MOBJ_HNDLR_TBL_ST g_astNmParamHndlrTbl[];
extern ALARM_ITEM_HNDLR_TBL_ST g_astAlarmEnaHndlrTbl[];
extern ALARM_ITEM_HNDLR_TBL_ST g_astAlarmStatusHndlrTbl[];
extern MOBJ_HNDLR_TBL_ST g_astSettingParamHndlrTbl[];
extern MOBJ_HNDLR_TBL_ST g_astRtCollParamHndlrTbl[];

LONG HandleMcpANormalRW(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                        UCHAR ucIsSupported, UCHAR ucOperation);
LONG HandleMcpANormalR(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType,
                       UCHAR ucIsSupported,UCHAR ucOperation);
LONG HandleMcpARcParamWithThr(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType,
                              UCHAR ucIsSupported, UCHAR);
LONG HandleMcpAAlarmStatus(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType,
                            UCHAR ucIsSupported,UCHAR ucOperation);
LONG HandleMcpARfSw(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                      UCHAR ucIsSupported, UCHAR ucOperation);
LONG HandleMcpAChNum(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                     UCHAR ucIsSupported, UCHAR ucOperation);
LONG HandleMcpAAtt(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                   UCHAR ucIsSupported, UCHAR ucOperation);
LONG HandleMcpASmcAddr(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                       UCHAR ucIsSupported, UCHAR ucOperation);
LONG HandleMcpAIpAddr(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                      UCHAR ucIsSupported, UCHAR ucOperation);
LONG HandleMcpAPortNum(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                       UCHAR ucIsSupported, UCHAR ucOperation);
LONG HandleMcpACommMode(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                        UCHAR ucIsSupported, UCHAR ucOperation);                       
LONG HandleMcpADateTime(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                        UCHAR ucIsSupported, UCHAR ucOperation);
LONG HandleMcpARWDevType(MCPA_MOBJ_ST * pstMObj, ULONG ulParamAddr, UCHAR ucDataType,
                         UCHAR ucIsSupported, UCHAR ucOperation);
LONG HandleAsynOpSECParam(MCPA_MOBJ_ST *pstMObj, ULONG, UCHAR ucDataType, 
                          UCHAR ucIsSupported, UCHAR ucOperation);
LONG HandleAsynOpTDParam(MCPA_MOBJ_ST *pstMObj, ULONG, UCHAR ucDataType, 
                         UCHAR ucIsSupported, UCHAR ucOperation);
LONG HandleMcpAChCount(MCPA_MOBJ_ST * pstMObj, ULONG ulParamAddr, 
                       UCHAR ucDataType, UCHAR ucIsSupported, UCHAR ucOperation);
//zhangjie 20071120 ̫�������ص�ѹ
LONG HandleMcpASecVolR(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType,
                       UCHAR ucIsSupported, UCHAR ucOperation);
//zhangjie 20071120 ̫�������ص�ѹ

void InitDevInfoHndlrTbl()
{
    LONG i = 0;

    //�����в�������ʼ��Ϊ��֧��
    //MCM-37_20061123_linyu_begin
    for(i = 0; i < DEV_INFO_HNDLR_TBL_SIZE; i++)
    //MCM-37_20061123_linyu_end
    {
        g_astDevInfoHndlrTbl[i].ucIsSupported   = PARAM_NOT_SUPPORTED;
        g_astDevInfoHndlrTbl[i].pfHandler       = HandleMcpANormalR;
    }

    //�豸���̴���
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_MNFT_ID].ulAddr     = (ULONG)&g_stDevInfoSet.ucMnftId;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_MNFT_ID].ucDataType = UINT_1;

    //�豸���
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_DEV_TYPE].ulAddr     = (ULONG)&g_stDevInfoSet.ucDevType;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_DEV_TYPE].ucDataType = UINT_1;

    //�豸�ͺ�
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_DEV_MODEL].ulAddr     = (ULONG)&g_stDevInfoSet.acDevModel;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_DEV_MODEL].ucDataType = STR_LEN;

    //�豸�������к�  
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_DEV_SN].ulAddr     = (ULONG)&g_stDevInfoSet.acDevSn;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_DEV_SN].ucDataType = STR_LEN;

    //�豸��ʵ���ŵ�����
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_CH_COUNT].ulAddr     = (ULONG)&g_stDevInfoSet.ucChCount;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_CH_COUNT].ucDataType = UINT_1;
    
    //����  
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_LONGITUDE].ulAddr     = (ULONG)&g_stDevInfoSet.acLongitude;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_LONGITUDE].ucDataType = STR_LEN;
    
    //γ�� 
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_LATITUDE].ulAddr     = (ULONG)&g_stDevInfoSet.acLatitude;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_LATITUDE].ucDataType = STR_LEN;
    
    //�豸�ļ�ز����б�
    //g_astDevInfoHndlrTbl[MOBJ_ID_DI_MOBJ_TABLE].ulAddr     = (ULONG)&g_stMObjIdTbl;
    //g_astDevInfoHndlrTbl[MOBJ_ID_DI_MOBJ_TABLE].ucDataType = STR_LEN;

    //��ذ汾��Ϣ
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SFWR_VER].ulAddr     = (ULONG)&g_stDevInfoSet.acSfwrVer;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SFWR_VER].ucDataType = STR_LEN;

    //3G:�豸�ͺ�
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_DEV_MODEL2].ulAddr     = (ULONG)&g_stDevInfoSet.acDevModel2;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_DEV_MODEL2].ucDataType = LONG_STR_LEN;

    //3G:Ƕ��ʽ�������ģʽ������û�����壬ֻ��Ӧ��ʱ��״̬ 
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_RUNNING_MODE].ulAddr     = (ULONG)&g_stDevInfoSet.ucRunningMode;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_RUNNING_MODE].ucDataType = UINT_1;

    //3G:��֧��AP��CЭ�����󳤶�
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_MAX_APC_LEN].ulAddr     = (ULONG)&g_stDevInfoSet.usMaxApcLen;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_MAX_APC_LEN].ucDataType = UINT_2;

    //3G:MCP��B���õĽ�������
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_MCPB_MODE].ulAddr     = (ULONG)&g_stDevInfoSet.ucMcpbMode;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_MCPB_MODE].ucDataType = UINT_1;

    //3G:����ϵ����NC��
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_NC].ulAddr     = (ULONG)&g_stDevInfoSet.ucNc;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_NC].ucDataType = UINT_1;

    //3G:�豸��Ӧ��ʱ��TOT1��
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_T1].ulAddr     = (ULONG)&g_stDevInfoSet.ucT1;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_T1].ucDataType = UINT_1;

    //3G:���ͼ��ʱ�䣨TG������������NC��1ʱ�������壩
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_TG].ulAddr     = (ULONG)&g_stDevInfoSet.usTg;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_TG].ucDataType = UINT_2;

    //3G:��ͣ����ȴ�ʱ�䣨TP��
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_TP].ulAddr     = (ULONG)&g_stDevInfoSet.ucTp;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_TP].ucDataType = UINT_1;

    //3G:ת�����������ģʽʱ��OMC��Ҫ�ȴ���ʱ��
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_TIME_TO_SWITCH].ulAddr     = (ULONG)&g_stDevInfoSet.usTimeToSwitch;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_TIME_TO_SWITCH].ucDataType = UINT_2;

    //3G:�豸ִ����������Ľ�� 
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_UPGRADE_RESULT].ulAddr     = (ULONG)&g_stDevInfoSet.ucUpgradeResult;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_UPGRADE_RESULT].ucDataType = UINT_1;

    //3G:�豸ʹ�õ�Զ��������ʽ 
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_UPGRADE_MODE].ulAddr     = (ULONG)&g_stDevInfoSet.ucUpgradeMode;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_UPGRADE_MODE].ucDataType = UINT_1;

//------------------------------����Ϊ̫���ܿ�������������-----------------------------------

    //̫�����豸���̴���
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_MNFT_ID].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_MNFT_ID].ucDataType = UINT_1;

    //̫�����豸���
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_DEV_TYPE].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_DEV_TYPE].ucDataType = UINT_1;

    //̫�����豸�ͺ�
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_DEV_MODEL].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_DEV_MODEL].ucDataType = STR_20;

    //̫���ܼ�ذ汾��Ϣ
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_SFWR_VER].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_SFWR_VER].ucDataType = STR_20;

    //̫�����豸�������к�
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_DEV_SN].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_DEV_SN].ucDataType = STR_20;

    //����ͺ�
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_SB_MODEL].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_SB_MODEL].ucDataType = STR_20;

    //�����
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_SB_SPEC].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_SB_SPEC].ucDataType = UINT_1;

    //�������
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_SB_CNT].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_SB_CNT].ucDataType = UINT_1;

    //����淽λ��
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_SB_DIR].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_SB_DIR].ucDataType = UINT_1 ;

    //����温����
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_SB_PITCH_ANGLE].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_SB_PITCH_ANGLE].ucDataType = UINT_1;

    //�����ͺ�
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_ST_BAT_MODEL].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_ST_BAT_MODEL].ucDataType = STR_20;

    //���ع��
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_ST_BAT_SPEC].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_ST_BAT_SPEC].ucDataType = STR_20;

    //��������
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_ST_BAT_CNT].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_ST_BAT_CNT].ucDataType = UINT_1;

    //�վ���׼����ʱ��
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_STD_SLMT_PER_DAY].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_STD_SLMT_PER_DAY].ucDataType = UINT_1;

    //̫���ܵ�Դվ�㾭��
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_LONGITUDE].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_LONGITUDE].ucDataType = STR_20;

    //̫���ܵ�Դվ��γ��
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_LATITUDE].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_LATITUDE].ucDataType = STR_20;

    //̫���ܵ�Դվ�㺣��
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_ALTITUDE].pfHandler  = HandleAsynOpSECParam;
    g_astDevInfoHndlrTbl[MOBJ_ID_DI_SEC_ALTITUDE].ucDataType = UINT_2;

//------------------------------����Ϊ̫���ܿ�������������-----------------------------------

}

void InitDevInfoExtHndlrTbl()
{
    LONG i = 0;

    //�����в�������ʼ��Ϊ֧�֣�����Ҫʹ�ü�ز����б�����ʼ��
    //MCM-37_20061123_linyu_begin
    for(i = 0; i < DEV_INFO_HNDLR_TBL_SIZE; i++)
    //MCM-37_20061123_linyu_end
    {
        g_astDevInfoExHndlrTbl[i].ucIsSupported   = PARAM_SUPPORTED;
        g_astDevInfoExHndlrTbl[i].pfHandler       = HandleMcpANormalRW; //��չ�����ǿ�д��
    }
    
    //�豸���̴���
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_MNFT_ID].ulAddr     = (ULONG)&g_stDevInfoSet.ucMnftId;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_MNFT_ID].ucDataType = UINT_1;

    //�豸���
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_DEV_TYPE].ulAddr     = (ULONG)&g_stDevInfoSet.ucDevType;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_DEV_TYPE].ucDataType = UINT_1;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_DEV_TYPE].pfHandler       = HandleMcpARWDevType;

    //�豸�ͺ�
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_DEV_MODEL].ulAddr     = (ULONG)&g_stDevInfoSet.acDevModel;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_DEV_MODEL].ucDataType = STR_LEN;

    //�豸�������к�  
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_DEV_SN].ulAddr     = (ULONG)&g_stDevInfoSet.acDevSn;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_DEV_SN].ucDataType = STR_LEN;

    //�豸��ʵ���ŵ�����
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_CH_COUNT].pfHandler = HandleMcpAChCount;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_CH_COUNT].ulAddr     = (ULONG)&g_stDevInfoSet.ucChCount;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_CH_COUNT].ucDataType = UINT_1;
    
    //����  
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_LONGITUDE].ulAddr     = (ULONG)&g_stDevInfoSet.acLongitude;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_LONGITUDE].ucDataType = STR_LEN;
    
    //γ�� 
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_LATITUDE].ulAddr     = (ULONG)&g_stDevInfoSet.acLatitude;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_LATITUDE].ucDataType = STR_LEN;
    
    //�豸�ļ�ز����б�
    //g_astDevInfoExHndlrTbl[MOBJ_ID_DI_MOBJ_TABLE].ulAddr     = (ULONG)&g_stMObjIdTbl;
    //g_astDevInfoExHndlrTbl[MOBJ_ID_DI_MOBJ_TABLE].ucDataType = STR_LEN;

    //��ذ汾��Ϣ
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_SFWR_VER].ulAddr     = (ULONG)&g_stDevInfoSet.acSfwrVer;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_SFWR_VER].ucDataType = STR_LEN;

    //3G:�豸�ͺ�
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_DEV_MODEL2].ulAddr     = (ULONG)&g_stDevInfoSet.acDevModel2;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_DEV_MODEL2].ucDataType = LONG_STR_LEN;

    //3G:Ƕ��ʽ�������ģʽ������û�����壬ֻ��Ӧ��ʱ��״̬ 
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_RUNNING_MODE].ulAddr     = (ULONG)&g_stDevInfoSet.ucRunningMode;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_RUNNING_MODE].ucDataType = UINT_1;

    //3G:��֧��AP��CЭ�����󳤶�
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_MAX_APC_LEN].ulAddr     = (ULONG)&g_stDevInfoSet.usMaxApcLen;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_MAX_APC_LEN].ucDataType = UINT_2;

    //3G:MCP��B���õĽ�������
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_MCPB_MODE].ulAddr     = (ULONG)&g_stDevInfoSet.ucMcpbMode;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_MCPB_MODE].ucDataType = UINT_1;

    //3G:����ϵ����NC��
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_NC].ulAddr     = (ULONG)&g_stDevInfoSet.ucNc;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_NC].ucDataType = UINT_1;

    //3G:�豸��Ӧ��ʱ��TOT1��
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_T1].ulAddr     = (ULONG)&g_stDevInfoSet.ucT1;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_T1].ucDataType = UINT_1;

    //3G:���ͼ��ʱ�䣨TG������������NC��1ʱ�������壩
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_TG].ulAddr     = (ULONG)&g_stDevInfoSet.usTg;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_TG].ucDataType = UINT_2;

    //3G:��ͣ����ȴ�ʱ�䣨TP��
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_TP].ulAddr     = (ULONG)&g_stDevInfoSet.ucTp;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_TP].ucDataType = UINT_1;

    //3G:ת�����������ģʽʱ��OMC��Ҫ�ȴ���ʱ��
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_TIME_TO_SWITCH].ulAddr     = (ULONG)&g_stDevInfoSet.usTimeToSwitch;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_TIME_TO_SWITCH].ucDataType = UINT_2;

    //3G:�豸ִ����������Ľ�� 
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_UPGRADE_RESULT].ulAddr     = (ULONG)&g_stDevInfoSet.ucUpgradeResult;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_UPGRADE_RESULT].ucDataType = UINT_1;

    //3G:�豸ʹ�õ�Զ��������ʽ 
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_UPGRADE_MODE].ulAddr     = (ULONG)&g_stDevInfoSet.ucUpgradeMode;
    g_astDevInfoExHndlrTbl[MOBJ_ID_DI_UPGRADE_MODE].ucDataType = UINT_1;
}

void InitNmParamHndlrTbl()
{
    LONG i = 0;

    //�����в�������ʼ��Ϊ��֧��
    //MCM-37_20061123_linyu_begin
    for(i = 0; i < NM_PARAMS_HNDLR_TBL_SIZE; i++)
    //MCM-37_20061123_linyu_end
    {
        g_astNmParamHndlrTbl[i].ucIsSupported   = PARAM_NOT_SUPPORTED;
        g_astNmParamHndlrTbl[i].pfHandler       = HandleMcpANormalRW;
    }

    //վ����
    g_astNmParamHndlrTbl[MOBJ_ID_NM_STA_NUM].ulAddr     = (ULONG)&g_stNmParamSet.ulStaNum;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_STA_NUM].ucDataType = UINT_4;

    //�豸���
    g_astNmParamHndlrTbl[MOBJ_ID_NM_DEV_NUM].ulAddr     = (ULONG)&g_stNmParamSet.ucDevNum;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_DEV_NUM].ucDataType = UINT_1;

    //���ŷ������ĺ���(���⴦��)
    g_astNmParamHndlrTbl[MOBJ_ID_NM_SMC_ADDR].pfHandler  = HandleMcpASmcAddr;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_SMC_ADDR].ulAddr     = (ULONG)&g_stNmParamSet.acSmcAddr;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_SMC_ADDR].ucDataType = TEL_NUM_LEN;
    
    //��ѯ/���õ绰����1��5 
    g_astNmParamHndlrTbl[MOBJ_ID_NM_QNS_TEL_NUM_1].ulAddr     = (ULONG)&g_stNmParamSet.acQnsTelNum1;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_QNS_TEL_NUM_1].ucDataType = TEL_NUM_LEN;

    g_astNmParamHndlrTbl[MOBJ_ID_NM_QNS_TEL_NUM_2].ulAddr     = (ULONG)&g_stNmParamSet.acQnsTelNum2;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_QNS_TEL_NUM_2].ucDataType = TEL_NUM_LEN;

    g_astNmParamHndlrTbl[MOBJ_ID_NM_QNS_TEL_NUM_3].ulAddr     = (ULONG)&g_stNmParamSet.acQnsTelNum3;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_QNS_TEL_NUM_3].ucDataType = TEL_NUM_LEN;

    g_astNmParamHndlrTbl[MOBJ_ID_NM_QNS_TEL_NUM_4].ulAddr     = (ULONG)&g_stNmParamSet.acQnsTelNum4;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_QNS_TEL_NUM_4].ucDataType = TEL_NUM_LEN;

    g_astNmParamHndlrTbl[MOBJ_ID_NM_QNS_TEL_NUM_5].ulAddr     = (ULONG)&g_stNmParamSet.acQnsTelNum5;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_QNS_TEL_NUM_5].ucDataType = TEL_NUM_LEN;

    //�ϱ�����
    g_astNmParamHndlrTbl[MOBJ_ID_NM_REPORT_TEL_NUM].ulAddr     = (ULONG)&g_stNmParamSet.acReportTelNum;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_REPORT_TEL_NUM].ucDataType = TEL_NUM_LEN;

    //�������IP��ַ��IP v4��
    g_astNmParamHndlrTbl[MOBJ_ID_NM_EMS_IP_ADDR].pfHandler  = HandleMcpAIpAddr;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_EMS_IP_ADDR].ulAddr     = (ULONG)&g_stNmParamSet.aucEmsIpAddr;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_EMS_IP_ADDR].ucDataType = IP_ADDR_LEN;

    //�������IP��ַ�˿ں�
    g_astNmParamHndlrTbl[MOBJ_ID_NM_EMS_PORT_NUM].pfHandler  = HandleMcpAPortNum;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_EMS_PORT_NUM].ulAddr     = (ULONG)&g_stNmParamSet.usEmsPortNum;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_EMS_PORT_NUM].ucDataType = UINT_2;

    //3G:�豸�Ƿ�ʹ��GPRS��ʽ 
    g_astNmParamHndlrTbl[MOBJ_ID_NM_USE_GPRS].ulAddr     = (ULONG)&g_stNmParamSet.ucUseGprs;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_USE_GPRS].ucDataType = UINT_1;

    //3G:GPRS��������ƣ�����APN��
    g_astNmParamHndlrTbl[MOBJ_ID_NM_GPRS_APN].ulAddr     = (ULONG)&g_stNmParamSet.acGprsApn;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_GPRS_APN].ucDataType = STR_LEN;

    //3G:�豸�����������ʱ�� 
    g_astNmParamHndlrTbl[MOBJ_ID_NM_HEARTBEAT_PERIOD].ulAddr     = (ULONG)&g_stNmParamSet.usHeartbeatPeriod;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_HEARTBEAT_PERIOD].ucDataType = UINT_2;

    //3G:�豸�ĵ绰���� 
    g_astNmParamHndlrTbl[MOBJ_ID_NM_DEV_TEL_NUM].ulAddr     = (ULONG)&g_stNmParamSet.acDevTelNum;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_DEV_TEL_NUM].ucDataType = TEL_NUM_LEN;

    //3G:GPRS�������û���ʶ
    g_astNmParamHndlrTbl[MOBJ_ID_NM_GPRS_USER_ID].ulAddr     = (ULONG)&g_stNmParamSet.acGprsUserId;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_GPRS_USER_ID].ucDataType = STR_LEN;

    //3G:GPRS����������
    g_astNmParamHndlrTbl[MOBJ_ID_NM_GPRS_PWD].ulAddr     = (ULONG)&g_stNmParamSet.acGprsPwd;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_GPRS_PWD].ucDataType = STR_LEN;

    //�ϱ�ͨ�ŷ�ʽ
    g_astNmParamHndlrTbl[MOBJ_ID_NM_REPORT_COMM_MODE].pfHandler  = HandleMcpACommMode;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_REPORT_COMM_MODE].ulAddr     = (ULONG)&g_stNmParamSet.ucReportCommMode;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_REPORT_COMM_MODE].ucDataType = UINT_1;

    //�ϱ�����
    g_astNmParamHndlrTbl[MOBJ_ID_NM_REPORT_TYPE].ulAddr     = (ULONG)&g_stNmParamSet.ucReportType;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_REPORT_TYPE].ucDataType = UINT_1;

    //ͨ�ŷ�ʽ��������(�淶20060810)
    //MCM-5_20061101_zhangjie_begin
    g_astNmParamHndlrTbl[MOBJ_ID_NM_COMM_MODE].pfHandler  = HandleMcpACommMode;    
    //MCM-5_20061101_zhangjie_end
    g_astNmParamHndlrTbl[MOBJ_ID_NM_COMM_MODE].ulAddr     = (ULONG)&g_stNmParamSet.ucCommMode;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_COMM_MODE].ucDataType = UINT_1;    

    //���ڡ�ʱ��(���⴦��)
    g_astNmParamHndlrTbl[MOBJ_ID_NM_DATE_TIME].pfHandler  = HandleMcpADateTime;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_DATE_TIME].ulAddr     = (ULONG)&g_stNmParamSet.aucDateTime;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_DATE_TIME].ucDataType = DATE_TIME_LEN;

    //3G:FTP������IP��ַ��IP v4��
    g_astNmParamHndlrTbl[MOBJ_ID_NM_FTP_SERVER_IP_ADDR].ulAddr     = (ULONG)&g_stNmParamSet.aucFtpServerIpAddr;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_FTP_SERVER_IP_ADDR].ucDataType = IP_ADDR_LEN;

    //3G:FTP������IP��ַ�˿ں�
    g_astNmParamHndlrTbl[MOBJ_ID_NM_FTP_SERVER_PORT_NUM].ulAddr     = (ULONG)&g_stNmParamSet.usFtpServerPortNum;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_FTP_SERVER_PORT_NUM].ucDataType = UINT_2;

    //3G:FTP�����������û���
    g_astNmParamHndlrTbl[MOBJ_ID_NM_FTP_USER_ID].ulAddr     = (ULONG)&g_stNmParamSet.acFtpUserId;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_FTP_USER_ID].ucDataType = STR_LEN;

    //3G:FTP���������Ŀ���
    g_astNmParamHndlrTbl[MOBJ_ID_NM_FTP_PWD].ulAddr     = (ULONG)&g_stNmParamSet.acFtpPwd;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_FTP_PWD].ucDataType = STR_LEN;

    //3G:���·�� 
    g_astNmParamHndlrTbl[MOBJ_ID_NM_FTP_PATH].ulAddr     = (ULONG)&g_stNmParamSet.acFtpPath;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_FTP_PATH].ucDataType = LONG_STR_LEN;

    //3G:�ļ���
    g_astNmParamHndlrTbl[MOBJ_ID_NM_FILE_NAME].ulAddr     = (ULONG)&g_stNmParamSet.acFileName;
    g_astNmParamHndlrTbl[MOBJ_ID_NM_FILE_NAME].ucDataType = LONG_STR_LEN;

}

void InitAlarmItemHndlrTbl()
{
    UCHAR i = 0;
    UCHAR ucIdx = 0;

    //�����в�������ʼ��Ϊ��֧��
    for(i = 0; i < MAX_ALARM_ITEMS; i++)
    {
        g_astAlarmEnaHndlrTbl[i].ulAddr             = (ULONG)&g_uAlarmEnable.aucAlarmEnableArray[i];
        g_astAlarmEnaHndlrTbl[i].pfHandler          = HandleMcpANormalRW;
        g_astAlarmEnaHndlrTbl[i].ucDataType         = UINT_1;
        g_astAlarmEnaHndlrTbl[i].ucIsSupported      = PARAM_NOT_SUPPORTED;

        g_astAlarmStatusHndlrTbl[i].ulAddr          = i; //�澯��λ�ñȽ��������ר�Ŵ������ﱣ������ƫ��
        g_astAlarmStatusHndlrTbl[i].pfHandler       = HandleMcpAAlarmStatus;
        g_astAlarmStatusHndlrTbl[i].ucDataType      = UINT_1;
        g_astAlarmStatusHndlrTbl[i].ucIsSupported   = PARAM_NOT_SUPPORTED;
    }
    
    //��ͨ��1�����и澯ʹ�ܣ�ӳ�䵽����ͨ����ID�ϣ������澯�ж��н���Բ���ͨ����ID����
    //       �жϣ��ж���󽫲���ͨ���ĸ澯״̬���Ƹ�ͨ��1��ID�ϣ����ͨ��1��ID�澯�ж�
    //��ȡͨ��1�Ͳ���ͨ���ŵĲ�����ƫ��
    ULONG ulIdxCh1     = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucDlInOverPwr1) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    ULONG usIdxNoChNum = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucDlInOverPwr) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    g_astAlarmEnaHndlrTbl[ulIdxCh1].ulAddr = g_astAlarmEnaHndlrTbl[usIdxNoChNum].ulAddr;

    ulIdxCh1     = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucDlInUnderPwr1) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    usIdxNoChNum = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucDlInUnderPwr) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    g_astAlarmEnaHndlrTbl[ulIdxCh1].ulAddr = g_astAlarmEnaHndlrTbl[usIdxNoChNum].ulAddr;

    ulIdxCh1     = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucDlOutOverPwr1) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    usIdxNoChNum = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucDlOutOverPwr) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    g_astAlarmEnaHndlrTbl[ulIdxCh1].ulAddr = g_astAlarmEnaHndlrTbl[usIdxNoChNum].ulAddr;
    
    ulIdxCh1     = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucDlOutUnderPwr1) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    usIdxNoChNum = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucDlOutUnderPwr) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    g_astAlarmEnaHndlrTbl[ulIdxCh1].ulAddr = g_astAlarmEnaHndlrTbl[usIdxNoChNum].ulAddr;

    ulIdxCh1     = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucUlOutOverPwr1) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    usIdxNoChNum = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucUlOutOverPwr) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    g_astAlarmEnaHndlrTbl[ulIdxCh1].ulAddr = g_astAlarmEnaHndlrTbl[usIdxNoChNum].ulAddr;

    ulIdxCh1     = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucUlOutUnderPwr1) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    usIdxNoChNum = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucUlOutUnderPwr) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    g_astAlarmEnaHndlrTbl[ulIdxCh1].ulAddr = g_astAlarmEnaHndlrTbl[usIdxNoChNum].ulAddr;

    ulIdxCh1     = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucDlSwr1) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    usIdxNoChNum = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucDlSwr) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    g_astAlarmEnaHndlrTbl[ulIdxCh1].ulAddr = g_astAlarmEnaHndlrTbl[usIdxNoChNum].ulAddr;

    ulIdxCh1     = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucUlSwr1) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    usIdxNoChNum = (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct.ucUlSwr) - (ULONG)&(g_uAlarmEnable.stAlarmEnableStruct);
    g_astAlarmEnaHndlrTbl[ulIdxCh1].ulAddr = g_astAlarmEnaHndlrTbl[usIdxNoChNum].ulAddr;

    //��ʼ���澯��ID���ֽ�
    //1��0x17�澯��ID����
    for(i = 0; i < 0x17; i++)
    {
        g_astAlarmStatusHndlrTbl[i].ucIDLoByte = i + 1;
        g_astAlarmEnaHndlrTbl[i].ucIDLoByte    = g_astAlarmStatusHndlrTbl[i].ucIDLoByte;
    }
    ucIdx = i;

    //0x20��0x2A�澯������
    for(; i < 0x2A - 0x20 + 1 + ucIdx; i++)
    {
        g_astAlarmStatusHndlrTbl[i].ucIDLoByte = i + 0x20 - ucIdx;
        g_astAlarmEnaHndlrTbl[i].ucIDLoByte    = g_astAlarmStatusHndlrTbl[i].ucIDLoByte;
    }
    ucIdx = i;

    //0x30��0x4F�澯������
    for(; i < 0x4F - 0x30 + 1 + ucIdx; i++)
    {
        g_astAlarmStatusHndlrTbl[i].ucIDLoByte = i + 0x30 - ucIdx;
        g_astAlarmEnaHndlrTbl[i].ucIDLoByte    = g_astAlarmStatusHndlrTbl[i].ucIDLoByte;
    }
    ucIdx = i;

    //0x60��0x71�澯������
    for(; i < 0x71 - 0x60 + 1 + ucIdx; i++)
    {
        g_astAlarmStatusHndlrTbl[i].ucIDLoByte = i + 0x60 - ucIdx;
        g_astAlarmEnaHndlrTbl[i].ucIDLoByte    = g_astAlarmStatusHndlrTbl[i].ucIDLoByte;
    }
    ucIdx = i;

//------------------------------����ΪPOI��������-----------------------------------
    //0xA0��0xB1�澯������
    for(; i < MOBJ_ID_A_TRUNK_IN_UNDER_PWR - MOBJ_ID_A_CDMA800_IN_OVER_PWR + 1 + ucIdx; i++)
    {
        g_astAlarmStatusHndlrTbl[i].ucIDLoByte = i + MOBJ_ID_A_CDMA800_IN_OVER_PWR - ucIdx;
        g_astAlarmEnaHndlrTbl[i].ucIDLoByte    = g_astAlarmStatusHndlrTbl[i].ucIDLoByte;
    }
    ucIdx = i;

//------------------------------����ΪPOI��������-----------------------------------

//------------------------------����Ϊ̫���ܿ�������������-----------------------------------
    for(; i < MOBJ_ID_A_SEC_ALEAK - MOBJ_ID_A_SEC_ST_BAT_BLOWOUT + 1 + ucIdx; i++)
    {
        g_astAlarmStatusHndlrTbl[i].ucIDLoByte = i + MOBJ_ID_A_SEC_ST_BAT_BLOWOUT - ucIdx;
        g_astAlarmEnaHndlrTbl[i].ucIDLoByte    = g_astAlarmStatusHndlrTbl[i].ucIDLoByte;

        g_astAlarmStatusHndlrTbl[i].pfHandler  = HandleAsynOpSECParam;
        g_astAlarmEnaHndlrTbl[i].pfHandler     = HandleAsynOpSECParam;
    }
//------------------------------����Ϊ̫���ܿ�������������-----------------------------------

}

//������Ƶ���ء��ŵ��ź�˥����Ҫ���⴦��
void InitSettingParamHndlrTbl()
{
    LONG i = 0;

    //�����в�������ʼ��Ϊ��֧��
    //MCM-37_20061123_linyu_begin
    for(i = 0; i < SETTING_PARAMS_HNDLR_TBL_SIZE; i++)
    //MCM-37_20061123_linyu_end
    {
        g_astSettingParamHndlrTbl[i].ucIsSupported = PARAM_NOT_SUPPORTED;
        g_astSettingParamHndlrTbl[i].pfHandler     = HandleMcpANormalRW;
    }

    //��Ƶ�źſ���״̬(���⴦��)
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW].pfHandler  = HandleMcpARfSw;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW].ulAddr     = (ULONG)&g_stSettingParamSet.ucRfSw;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW].ucDataType = UINT_1;

    //���ſ���״̬1��8
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_1].ulAddr     = (ULONG)&g_stSettingParamSet.ucPaSw1;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_1].ucDataType = UINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_2].ulAddr     = (ULONG)&g_stSettingParamSet.ucPaSw2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_2].ucDataType = UINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_3].ulAddr     = (ULONG)&g_stSettingParamSet.ucPaSw3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_3].ucDataType = UINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_4].ulAddr     = (ULONG)&g_stSettingParamSet.ucPaSw4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_4].ucDataType = UINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_5].ulAddr     = (ULONG)&g_stSettingParamSet.ucPaSw5;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_5].ucDataType = UINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_6].ulAddr     = (ULONG)&g_stSettingParamSet.ucPaSw6;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_6].ucDataType = UINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_7].ulAddr     = (ULONG)&g_stSettingParamSet.ucPaSw7;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_7].ucDataType = UINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_8].ulAddr     = (ULONG)&g_stSettingParamSet.ucPaSw8;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_SW_8].ucDataType = UINT_1;

    //�����ŵ���1��16 uint2��(���⴦��)
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_1].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_1].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum1;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_1].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_2].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_2].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_2].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_3].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_3].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_3].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_4].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_4].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_4].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_5].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_5].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum5;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_5].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_6].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_6].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum6;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_6].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_7].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_7].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum7;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_7].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_8].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_8].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum8;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_8].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_9].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_9].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum9;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_9].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_10].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_10].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum10;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_10].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_11].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_11].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum11;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_11].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_12].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_12].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum12;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_12].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_13].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_13].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum13;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_13].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_14].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_14].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum14;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_14].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_15].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_15].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum15;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_15].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_16].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_16].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkChNum16;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_CH_NUM_16].ucDataType = UINT_2;

    //��Ƶ�ŵ���1��16 uint2��(���⴦��)
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_1].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_1].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum1;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_1].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_2].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_2].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_2].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_3].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_3].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_3].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_4].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_4].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_4].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_5].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_5].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum5;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_5].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_6].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_6].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum6;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_6].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_7].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_7].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum7;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_7].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_8].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_8].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum8;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_8].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_9].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_9].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum9;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_9].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_10].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_10].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum10;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_10].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_11].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_11].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum11;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_11].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_12].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_12].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum12;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_12].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_13].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_13].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum13;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_13].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_14].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_14].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum14;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_14].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_15].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_15].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum15;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_15].ucDataType = UINT_2;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_16].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_16].ulAddr     = (ULONG)&g_stSettingParamSet.usShfChNum16;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_CH_NUM_16].ucDataType = UINT_2;

    //����Ƶ�����ϱߴ��ŵ���
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkUbChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM].ucDataType = UINT_2;

    //����Ƶ�����±ߴ��ŵ���
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkLbChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM].ucDataType = UINT_2;

    //��ƵƵ�����ϱߴ��ŵ���
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM].ulAddr     = (ULONG)&g_stSettingParamSet.usShfUbChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM].ucDataType = UINT_2;

    //��ƵƵ�����±ߴ��ŵ���
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM].ulAddr     = (ULONG)&g_stSettingParamSet.usShfLbChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM].ucDataType = UINT_2;

    //����Ƶ�����ϱߴ��ŵ��ţ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM_1].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM_1].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkUbChNum1;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM_1].ucDataType = UINT_2;

    //����Ƶ�����±ߴ��ŵ��ţ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM_1].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM_1].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkLbChNum1;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM_1].ucDataType = UINT_2;

    //����Ƶ�����ϱߴ��ŵ��ţ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM_2].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM_2].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkUbChNum2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM_2].ucDataType = UINT_2;

    //����Ƶ�����±ߴ��ŵ��ţ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM_2].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM_2].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkLbChNum2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM_2].ucDataType = UINT_2;
    
    //����Ƶ�����ϱߴ��ŵ��ţ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM_3].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM_3].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkUbChNum3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM_3].ucDataType = UINT_2;

    //����Ƶ�����±ߴ��ŵ��ţ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM_3].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM_3].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkLbChNum3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM_3].ucDataType = UINT_2;
    
    //����Ƶ�����ϱߴ��ŵ��ţ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM_4].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM_4].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkUbChNum4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_UB_CH_NUM_4].ucDataType = UINT_2;

    //����Ƶ�����±ߴ��ŵ��ţ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM_4].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM_4].ulAddr     = (ULONG)&g_stSettingParamSet.usWorkLbChNum4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_WORK_LB_CH_NUM_4].ucDataType = UINT_2;

    //����˥��ֵ
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT].pfHandler  = HandleMcpAAtt;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT].ulAddr     = (ULONG)&g_stSettingParamSet.ucUlAtt;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT].ucDataType = UINT_1;

    //����˥��ֵ
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT].pfHandler  = HandleMcpAAtt;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT].ulAddr     = (ULONG)&g_stSettingParamSet.ucDlAtt;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT].ucDataType = UINT_1;

    //����˥��ֵ��ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT_1].pfHandler  = HandleMcpAAtt;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT_1].ulAddr     = (ULONG)&g_stSettingParamSet.ucUlAtt; //Ŀǰ�����㽫ͨ��1��˥��ӳ�䵽����ͨ���ŵ�˥����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT_1].ucDataType = UINT_1;
    
    //����˥��ֵ��ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT_1].pfHandler  = HandleMcpAAtt;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT_1].ulAddr     = (ULONG)&g_stSettingParamSet.ucDlAtt; //Ŀǰ�����㽫ͨ��1��˥��ӳ�䵽����ͨ���ŵ�˥����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT_1].ucDataType = UINT_1;

    //����˥��ֵ��ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT_2].pfHandler  = HandleMcpAAtt;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT_2].ulAddr     = (ULONG)&g_stSettingParamSet.ucUlAtt2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT_2].ucDataType = UINT_1;

    //����˥��ֵ��ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT_2].pfHandler  = HandleMcpAAtt;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT_2].ulAddr     = (ULONG)&g_stSettingParamSet.ucDlAtt2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT_2].ucDataType = UINT_1;

    //����˥��ֵ��ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT_3].pfHandler  = HandleMcpAAtt;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT_3].ulAddr     = (ULONG)&g_stSettingParamSet.ucUlAtt3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT_3].ucDataType = UINT_1;

    //����˥��ֵ��ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT_3].pfHandler  = HandleMcpAAtt;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT_3].ulAddr     = (ULONG)&g_stSettingParamSet.ucDlAtt3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT_3].ucDataType = UINT_1;

    //����˥��ֵ��ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT_4].pfHandler  = HandleMcpAAtt;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT_4].ulAddr     = (ULONG)&g_stSettingParamSet.ucUlAtt4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_ATT_4].ucDataType = UINT_1;

    //����˥��ֵ��ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT_4].pfHandler  = HandleMcpAAtt;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT_4].ulAddr     = (ULONG)&g_stSettingParamSet.ucDlAtt4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_ATT_4].ucDataType = UINT_1;

    //����פ��������
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_SWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.ucDlSwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_SWR_THR].ucDataType = UINT_1;

    //���Ź��¶ȸ澯����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_OVERHEAT_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cPaOverheatThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_PA_OVERHEAT_THR].ucDataType = SINT_1;

    //��ԴС��ʶ�������ֵ
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SRC_CELL_ID].ulAddr     = (ULONG)&g_stSettingParamSet.usSrcCellId;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SRC_CELL_ID].ucDataType = UINT_2;

    //�������빦��Ƿ��������
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cDlInUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR].ucDataType = SINT_1;

    //�������빦�ʹ���������
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cDlInOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR].ucDataType = SINT_1;

    //�����������Ƿ��������
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cDlOutUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR].ucDataType = SINT_1;

    //����������ʹ���������
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cDlOutOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR].ucDataType = SINT_1;

    //����������ʹ���������
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cUlOutOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR].ucDataType = SINT_1;

    //��ƵƵ�����ϱߴ��ŵ��ţ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM_1].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM_1].ulAddr     = (ULONG)&g_stSettingParamSet.usShfUbChNum1;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM_1].ucDataType = UINT_2;

    //��ƵƵ�����±ߴ��ŵ��ţ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM_1].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM_1].ulAddr     = (ULONG)&g_stSettingParamSet.usShfLbChNum1;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM_1].ucDataType = UINT_2;

    //��ƵƵ�����ϱߴ��ŵ��ţ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM_2].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM_2].ulAddr     = (ULONG)&g_stSettingParamSet.usShfUbChNum2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM_2].ucDataType = UINT_2;

    //��ƵƵ�����±ߴ��ŵ��ţ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM_2].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM_2].ulAddr     = (ULONG)&g_stSettingParamSet.usShfLbChNum2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM_2].ucDataType = UINT_2;

    //��ƵƵ�����ϱߴ��ŵ��ţ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM_3].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM_3].ulAddr     = (ULONG)&g_stSettingParamSet.usShfUbChNum3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM_3].ucDataType = UINT_2;

    //��ƵƵ�����±ߴ��ŵ��ţ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM_3].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM_3].ulAddr     = (ULONG)&g_stSettingParamSet.usShfLbChNum3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM_3].ucDataType = UINT_2;

    //��ƵƵ�����ϱߴ��ŵ��ţ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM_4].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM_4].ulAddr     = (ULONG)&g_stSettingParamSet.usShfUbChNum4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_UB_CH_NUM_4].ucDataType = UINT_2;

    //��ƵƵ�����±ߴ��ŵ��ţ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM_4].pfHandler  = HandleMcpAChNum;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM_4].ulAddr     = (ULONG)&g_stSettingParamSet.usShfLbChNum4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SHF_LB_CH_NUM_4].ucDataType = UINT_2;

    //��Ƶ�л�����״̬1��6 ���ز��أ�
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW_1].ulAddr     = (ULONG)&g_stSettingParamSet.ucRfSw1;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW_1].ucDataType = UINT_1;
    
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW_2].ulAddr     = (ULONG)&g_stSettingParamSet.ucRfSw2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW_2].ucDataType = UINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW_3].ulAddr     = (ULONG)&g_stSettingParamSet.ucRfSw3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW_3].ucDataType = UINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW_4].ulAddr     = (ULONG)&g_stSettingParamSet.ucRfSw4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW_4].ucDataType = UINT_1;
    
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW_5].ulAddr     = (ULONG)&g_stSettingParamSet.ucRfSw5;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW_5].ucDataType = UINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW_6].ulAddr     = (ULONG)&g_stSettingParamSet.ucRfSw6;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW_6].ucDataType = UINT_1;

    //�������빦��Ƿ�������ޣ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_1].ulAddr     = (ULONG)&g_stSettingParamSet.cDlInUnderPwrThr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_1].ucDataType = SINT_1;

    //�������빦�ʹ��������ޣ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_1].ulAddr     = (ULONG)&g_stSettingParamSet.cDlInOverPwrThr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_1].ucDataType = SINT_1;

    //�����������Ƿ�������ޣ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_1].ulAddr     = (ULONG)&g_stSettingParamSet.cDlOutUnderPwrThr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_1].ucDataType = SINT_1;

    //����������ʹ��������ޣ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_1].ulAddr     = (ULONG)&g_stSettingParamSet.cDlOutOverPwrThr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_1].ucDataType = SINT_1;

    //����������ʹ��������ޣ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_1].ulAddr     = (ULONG)&g_stSettingParamSet.cUlOutOverPwrThr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_1].ucDataType = SINT_1;

    //����פ�������ޣ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_SWR_THR_1].ulAddr     = (ULONG)&g_stSettingParamSet.ucDlSwrThr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_SWR_THR_1].ucDataType = UINT_1;

    //����פ�������ޣ�ͨ��1����������(�淶20060810)
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_SWR_THR_1].ulAddr     = (ULONG)&g_stSettingParamSet.ucUlSwrThr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_SWR_THR_1].ucDataType = UINT_1;    

    //�������빦��Ƿ�������ޣ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_2].ulAddr     = (ULONG)&g_stSettingParamSet.cDlInUnderPwrThr2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_2].ucDataType = SINT_1;

    //�������빦�ʹ��������ޣ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_2].ulAddr     = (ULONG)&g_stSettingParamSet.cDlInOverPwrThr2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_2].ucDataType = SINT_1;

    //�����������Ƿ�������ޣ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_2].ulAddr     = (ULONG)&g_stSettingParamSet.cDlOutUnderPwrThr2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_2].ucDataType = SINT_1;

    //����������ʹ��������ޣ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_2].ulAddr     = (ULONG)&g_stSettingParamSet.cDlOutOverPwrThr2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_2].ucDataType = SINT_1;

    //����������ʹ��������ޣ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_2].ulAddr     = (ULONG)&g_stSettingParamSet.cUlOutOverPwrThr2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_2].ucDataType = SINT_1;

    //����פ�������ޣ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_SWR_THR_2].ulAddr     = (ULONG)&g_stSettingParamSet.ucDlSwrThr2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_SWR_THR_2].ucDataType = UINT_1;

    //����פ�������ޣ�ͨ��2����������(�淶20060810)
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_SWR_THR_2].ulAddr     = (ULONG)&g_stSettingParamSet.ucUlSwrThr2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_SWR_THR_2].ucDataType = UINT_1;    

    //�������빦��Ƿ�������ޣ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_3].ulAddr     = (ULONG)&g_stSettingParamSet.cDlInUnderPwrThr3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_3].ucDataType = SINT_1;

    //�������빦�ʹ��������ޣ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_3].ulAddr     = (ULONG)&g_stSettingParamSet.cDlInOverPwrThr3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_3].ucDataType = SINT_1;

    //�����������Ƿ�������ޣ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_3].ulAddr     = (ULONG)&g_stSettingParamSet.cDlOutUnderPwrThr3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_3].ucDataType = SINT_1;

    //����������ʹ��������ޣ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_3].ulAddr     = (ULONG)&g_stSettingParamSet.cDlOutOverPwrThr3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_3].ucDataType = SINT_1;

    //����������ʹ��������ޣ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_3].ulAddr     = (ULONG)&g_stSettingParamSet.cUlOutOverPwrThr3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_3].ucDataType = SINT_1;

    //����פ�������ޣ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_SWR_THR_3].ulAddr     = (ULONG)&g_stSettingParamSet.ucDlSwrThr3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_SWR_THR_3].ucDataType = UINT_1;

    //����פ�������ޣ�ͨ��3����������(�淶20060810)
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_SWR_THR_3].ulAddr     = (ULONG)&g_stSettingParamSet.ucUlSwrThr3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_SWR_THR_3].ucDataType = UINT_1;    

    //�������빦��Ƿ�������ޣ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_4].ulAddr     = (ULONG)&g_stSettingParamSet.cDlInUnderPwrThr4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_4].ucDataType = SINT_1;

    //�������빦�ʹ��������ޣ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_4].ulAddr     = (ULONG)&g_stSettingParamSet.cDlInOverPwrThr4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_4].ucDataType = SINT_1;

    //�����������Ƿ�������ޣ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_4].ulAddr     = (ULONG)&g_stSettingParamSet.cDlOutUnderPwrThr4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_4].ucDataType = SINT_1;

    //����������ʹ��������ޣ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_4].ulAddr     = (ULONG)&g_stSettingParamSet.cDlOutOverPwrThr4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_4].ucDataType = SINT_1;

    //����������ʹ��������ޣ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_4].ulAddr     = (ULONG)&g_stSettingParamSet.cUlOutOverPwrThr4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_4].ucDataType = SINT_1;

    //����פ�������ޣ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_SWR_THR_4].ulAddr     = (ULONG)&g_stSettingParamSet.ucDlSwrThr4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_SWR_THR_4].ucDataType = UINT_1;

    //����פ�������ޣ�ͨ��4����������(�淶20060810)
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_SWR_THR_4].ulAddr     = (ULONG)&g_stSettingParamSet.ucUlSwrThr4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_SWR_THR_4].ucDataType = UINT_1;    

    //3G:�������빦��Ƿ�������ޣ�sint2�ͣ���λΪdBm������Ϊ10������ͬ
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX].ulAddr     = (ULONG)&g_stSettingParamSet.sDlInUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX].ucDataType = SINT_2;

    //3G:�������빦�ʹ���������
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX].ulAddr     = (ULONG)&g_stSettingParamSet.sDlInOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX].ucDataType = SINT_2;

    //3G:�����������Ƿ��������
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX].ulAddr     = (ULONG)&g_stSettingParamSet.sDlOutUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX].ucDataType = SINT_2;

    //3G:����������ʹ���������
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX].ulAddr     = (ULONG)&g_stSettingParamSet.sDlOutOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX].ucDataType = SINT_2;

    //3G:����������ʹ���������
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX].ulAddr     = (ULONG)&g_stSettingParamSet.sUlOutOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX].ucDataType = SINT_2;

    //3G:�����������Ƿ�������ޣ�������(�淶20060810)
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX].ulAddr     = (ULONG)&g_stSettingParamSet.sUlOutUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX].ucDataType = SINT_2;

    //����פ�������ޣ�������(�淶20060810)
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_SWR_THR_EX].ulAddr     = (ULONG)&g_stSettingParamSet.ucUlSwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_SWR_THR_EX].ucDataType = UINT_1;    
    

    //3G:�������빦��Ƿ�������ޣ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_1].ulAddr     = (ULONG)&g_stSettingParamSet.sDlInUnderPwrThr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_1].ucDataType = SINT_2;

    //3G:�������빦��Ƿ�������ޣ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_2].ulAddr     = (ULONG)&g_stSettingParamSet.sDlInUnderPwrThr2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_2].ucDataType = SINT_2;

    //3G:�������빦��Ƿ�������ޣ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_3].ulAddr     = (ULONG)&g_stSettingParamSet.sDlInUnderPwrThr3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_3].ucDataType = SINT_2;

    //3G:�������빦��Ƿ�������ޣ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_4].ulAddr     = (ULONG)&g_stSettingParamSet.sDlInUnderPwrThr4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_4].ucDataType = SINT_2;

    //3G:�������빦�ʹ��������ޣ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_1].ulAddr     = (ULONG)&g_stSettingParamSet.sDlInOverPwrThr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_1].ucDataType = SINT_2;

    //3G:�������빦�ʹ��������ޣ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_2].ulAddr     = (ULONG)&g_stSettingParamSet.sDlInOverPwrThr2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_2].ucDataType = SINT_2;

    //3G:�������빦�ʹ��������ޣ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_3].ulAddr     = (ULONG)&g_stSettingParamSet.sDlInOverPwrThr3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_3].ucDataType = SINT_2;

    //3G:�������빦�ʹ��������ޣ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_4].ulAddr     = (ULONG)&g_stSettingParamSet.sDlInOverPwrThr4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_4].ucDataType = SINT_2;

    //3G:�����������Ƿ�������ޣ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_1].ulAddr     = (ULONG)&g_stSettingParamSet.sDlOutUnderPwrThr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_1].ucDataType = SINT_2;

    //3G:�����������Ƿ�������ޣ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_2].ulAddr     = (ULONG)&g_stSettingParamSet.sDlOutUnderPwrThr2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_2].ucDataType = SINT_2;

    //3G:�����������Ƿ�������ޣ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_3].ulAddr     = (ULONG)&g_stSettingParamSet.sDlOutUnderPwrThr3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_3].ucDataType = SINT_2;

    //3G:�����������Ƿ�������ޣ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_4].ulAddr     = (ULONG)&g_stSettingParamSet.sDlOutUnderPwrThr4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_4].ucDataType = SINT_2;

    //3G:����������ʹ��������ޣ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_1].ulAddr     = (ULONG)&g_stSettingParamSet.sDlOutOverPwrThr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_1].ucDataType = SINT_2;

    //3G:����������ʹ��������ޣ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_2].ulAddr     = (ULONG)&g_stSettingParamSet.sDlOutOverPwrThr2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_2].ucDataType = SINT_2;

    //3G:����������ʹ��������ޣ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_3].ulAddr     = (ULONG)&g_stSettingParamSet.sDlOutOverPwrThr3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_3].ucDataType = SINT_2;

    //3G:����������ʹ��������ޣ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_4].ulAddr     = (ULONG)&g_stSettingParamSet.sDlOutOverPwrThr4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_4].ucDataType = SINT_2;

    //3G:����������ʹ��������ޣ�ͨ��1��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_1].ulAddr     = (ULONG)&g_stSettingParamSet.sUlOutOverPwrThr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_1].ucDataType = SINT_2;

    //3G:����������ʹ��������ޣ�ͨ��2��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_2].ulAddr     = (ULONG)&g_stSettingParamSet.sUlOutOverPwrThr2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_2].ucDataType = SINT_2;

    //3G:����������ʹ��������ޣ�ͨ��3��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_3].ulAddr     = (ULONG)&g_stSettingParamSet.sUlOutOverPwrThr3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_3].ucDataType = SINT_2;

    //3G:����������ʹ��������ޣ�ͨ��4��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_4].ulAddr     = (ULONG)&g_stSettingParamSet.sUlOutOverPwrThr4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_4].ucDataType = SINT_2;
    
    //3G:�����������Ƿ�������ޣ�ͨ��1����������(�淶20060810)
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_1].ulAddr     = (ULONG)&g_stSettingParamSet.sUlOutUnderPwrThr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_1].ucDataType = SINT_2;

    //3G:�����������Ƿ�������ޣ�ͨ��2����������(�淶20060810)
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_2].ulAddr     = (ULONG)&g_stSettingParamSet.sUlOutUnderPwrThr2;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_2].ucDataType = SINT_2;

    //3G:�����������Ƿ�������ޣ�ͨ��3����������(�淶20060810)
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_3].ulAddr     = (ULONG)&g_stSettingParamSet.sUlOutUnderPwrThr3;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_3].ucDataType = SINT_2;

    //3G:�����������Ƿ�������ޣ�ͨ��4����������(�淶20060810)
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_4].ulAddr     = (ULONG)&g_stSettingParamSet.sUlOutUnderPwrThr4;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_4].ucDataType = SINT_2;    
//------------------------------����ΪPOI��������-----------------------------------

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_CDMA800_IN_UNDER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cCdma800InUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_CDMA800_IN_UNDER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_CDMA800_IN_OVER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cCdma800InOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_CDMA800_IN_OVER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_MB_GSM_IN_UNDER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cMbGsmInUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_MB_GSM_IN_UNDER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_MB_GSM_IN_OVER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cMbGsmInOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_MB_GSM_IN_OVER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UC_GSM_IN_UNDER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cUcGsmInUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UC_GSM_IN_UNDER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UC_GSM_IN_OVER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cUcGsmInOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UC_GSM_IN_OVER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_MB_DCS_IN_UNDER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cMbDcsInUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_MB_DCS_IN_UNDER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_MB_DCS_IN_OVER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cMbDcsInOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_MB_DCS_IN_OVER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UC_DCS_IN_UNDER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cUcDcsInUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UC_DCS_IN_UNDER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UC_DCS_IN_OVER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cUcDcsInOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_UC_DCS_IN_OVER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_3G1_FDD_IN_UNDER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.c3G1FDDInUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_3G1_FDD_IN_UNDER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_3G1_FDD_IN_OVER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.c3G1FDDInOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_3G1_FDD_IN_OVER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_3G2_FDD_IN_UNDER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.c3G2FDDInUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_3G2_FDD_IN_UNDER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_3G2_FDD_IN_OVER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.c3G2FDDInOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_3G2_FDD_IN_OVER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_3G3_TDD_IN_UNDER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.c3G3TDDInUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_3G3_TDD_IN_UNDER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_3G3_TDD_IN_OVER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.c3G3TDDInOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_3G3_TDD_IN_OVER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_TRUNK_IN_UNDER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cTrunkInUnderPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_TRUNK_IN_UNDER_PWR_THR].ucDataType = SINT_1;

    g_astSettingParamHndlrTbl[MOBJ_ID_SP_TRUNK_IN_OVER_PWR_THR].ulAddr     = (ULONG)&g_stSettingParamSet.cTrunkInOverPwrThr;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_TRUNK_IN_OVER_PWR_THR].ucDataType = SINT_1;

//------------------------------����ΪPOI��������-----------------------------------

//------------------------------����Ϊ̫���ܿ�������������-----------------------------------

    //�����ѹ����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_OVER_CHARGED_THR].pfHandler  = HandleAsynOpSECParam;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_OVER_CHARGED_THR].ucDataType = UINT_2;

    //�������ѹ����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_EVEN_CHARGE_THR].pfHandler  = HandleAsynOpSECParam;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_EVEN_CHARGE_THR].ucDataType = UINT_2;

    //����ָ���ѹ����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_OVER_CHARGED_RESUME_THR].pfHandler  = HandleAsynOpSECParam;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_OVER_CHARGED_RESUME_THR].ucDataType = UINT_2;

    //ǿ��ת�����ѹ����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_FC_TO_PC_VOL_THR].pfHandler  = HandleAsynOpSECParam;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_FC_TO_PC_VOL_THR].ucDataType = UINT_2;

    //Ԥ���ŵ�ѹ����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_OVER_DISCHARGING_THR].pfHandler  = HandleAsynOpSECParam;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_OVER_DISCHARGING_THR].ucDataType = UINT_2;

    //���ŵ�ѹ����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_OVER_DISCHARGED_THR].pfHandler  = HandleAsynOpSECParam;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_OVER_DISCHARGED_THR].ucDataType = UINT_2;

    //���Żָ���ѹ����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_OVER_DISCHARGED_RESUME_THR].pfHandler  = HandleAsynOpSECParam;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_OVER_DISCHARGED_RESUME_THR].ucDataType = UINT_2;

    //����תǿ���������
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_PC_TO_FC_CUR_THR].pfHandler  = HandleAsynOpSECParam;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_PC_TO_FC_CUR_THR].ucDataType = UINT_2;

    //�����������
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_OUTPUT_OVER_CUR_THR].pfHandler  = HandleAsynOpSECParam;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_OUTPUT_OVER_CUR_THR].ucDataType = UINT_2;

    //��λ����
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_RESET_SW].pfHandler  = HandleAsynOpSECParam;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_RESET_SW].ucDataType = UINT_1;

    //��翪��
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_CHARGE_SW].pfHandler  = HandleAsynOpSECParam;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_CHARGE_SW].ucDataType = UINT_1;

    //���ؿ���
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_LOAD_SW].pfHandler  = HandleAsynOpSECParam;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_LOAD_SW].ucDataType = UINT_1;

    //����Ƿѹ��������
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_STARTUP_UNDER_VOL_SW].pfHandler  = HandleAsynOpSECParam;
    g_astSettingParamHndlrTbl[MOBJ_ID_SP_SEC_STARTUP_UNDER_VOL_SW].ucDataType = UINT_1;

//------------------------------����Ϊ̫���ܿ�������������-----------------------------------

}

void InitRtCollParamHndlrTbl()
{
    LONG i = 0;

    //�����в�������ʼ��Ϊ��֧��
    //MCM-37_20061123_linyu_begin
    for(i = 0; i < RT_COLL_PARAMS_HNDLR_TBL_SIZE; i++)
    //MCM-37_20061123_linyu_end
    {
        g_astRtCollParamHndlrTbl[i].ucIsSupported = PARAM_NOT_SUPPORTED;
        g_astRtCollParamHndlrTbl[i].pfHandler     = HandleMcpANormalR;
    }

    //�����¶�ֵ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_PA_TEMP].ulAddr     = (ULONG)&g_stRcParamSet.cPaTemp;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_PA_TEMP].ucDataType = SINT_1;
    
    //�������빦�ʵ�ƽ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR].ulAddr     = (ULONG)&g_stRcParamSet.cDlInPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR].ucDataType = SINT_1;

    //����������ʵ�ƽ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR].pfHandler  = HandleMcpARcParamWithThr; 
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR].ulAddr     = (ULONG)&g_stRcParamSet.cDlOutPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR].ucDataType = SINT_1;

    //������������
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_THE_GAIN].ulAddr     = (ULONG)&g_stRcParamSet.cUlTheGain;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_THE_GAIN].ucDataType = SINT_1;
    
    //����ʵ������
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN].ulAddr     = (ULONG)&g_stRcParamSet.cDlActGain;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN].ucDataType = SINT_1;

    //����פ����ֵ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR].ulAddr     = (ULONG)&g_stRcParamSet.ucDlSwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR].ucDataType = UINT_1;

    //��Դ��Ϣ����Ӫ�̴���
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SS_MNC].ulAddr     = (ULONG)&g_stRcParamSet.ucSsMnc;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SS_MNC].ucDataType = UINT_1;

    //��Դ��Ϣ��λ��������
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SS_LAC].ulAddr     = (ULONG)&g_stRcParamSet.usSsLac;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SS_LAC].ucDataType = UINT_2;

    //��Դ��Ϣ����վʶ����
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SS_BSIC].ulAddr     = (ULONG)&g_stRcParamSet.ucSsBsic;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SS_BSIC].ucDataType = UINT_1;

    //��Դ��Ϣ��BCCH������Ƶ��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SS_BCCH].ulAddr     = (ULONG)&g_stRcParamSet.usSsBcch;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SS_BCCH].ucDataType = UINT_2;

    //��Դ��Ϣ��BCCH���յ�ƽ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SS_BCCH_RX_LEV].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SS_BCCH_RX_LEV].ulAddr     = (ULONG)&g_stRcParamSet.cSsBcchRxLev;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SS_BCCH_RX_LEV].ucDataType = SINT_1;

    //��Դ��Ϣ��С��ʶ����ʵʱֵ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SS_CI].ulAddr     = (ULONG)&g_stRcParamSet.usSsCi;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SS_CI].ucDataType = UINT_2;

    //����������ʵ�ƽ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR].ulAddr     = (ULONG)&g_stRcParamSet.cUlOutPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR].ucDataType = SINT_1;

    //���չ���
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_OPT_RX_PWR].ulAddr     = (ULONG)&g_stRcParamSet.cOptRxPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_OPT_RX_PWR].ucDataType = SINT_1;

    //�ⷢ����
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_OPT_TX_PWR].ulAddr     = (ULONG)&g_stRcParamSet.cOptTxPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_OPT_TX_PWR].ucDataType = SINT_1;
    
    //�������빦�ʵ�ƽ��ͨ��1��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_1].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_1].ulAddr     = (ULONG)&g_stRcParamSet.cDlInPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_1].ucDataType = SINT_1;

    //����������ʵ�ƽ��ͨ��1��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_1].pfHandler  = HandleMcpARcParamWithThr; 
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_1].ulAddr     = (ULONG)&g_stRcParamSet.cDlOutPwr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_1].ucDataType = SINT_1;

    //�����������棨ͨ��1��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_THE_GAIN_1].ulAddr     = (ULONG)&g_stRcParamSet.cUlTheGain; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_THE_GAIN_1].ucDataType = SINT_1;
    
    //����ʵ�����棨ͨ��1��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN_1].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN_1].ulAddr     = (ULONG)&g_stRcParamSet.cDlActGain; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN_1].ucDataType = SINT_1;

    //����������ʵ�ƽ��ͨ��1��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_1].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_1].ulAddr     = (ULONG)&g_stRcParamSet.cUlOutPwr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_1].ucDataType = SINT_1;

    //����פ����ֵ��ͨ��1��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR_1].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR_1].ulAddr     = (ULONG)&g_stRcParamSet.ucDlSwr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR_1].ucDataType = UINT_1;

    //�������빦�ʵ�ƽ��ͨ��2��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_2].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_2].ulAddr     = (ULONG)&g_stRcParamSet.cDlInPwr2;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_2].ucDataType = SINT_1;

    //����������ʵ�ƽ��ͨ��2��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_2].pfHandler  = HandleMcpARcParamWithThr; 
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_2].ulAddr     = (ULONG)&g_stRcParamSet.cDlOutPwr2;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_2].ucDataType = SINT_1;

    //�����������棨ͨ��2��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_THE_GAIN_2].ulAddr     = (ULONG)&g_stRcParamSet.cUlTheGain2;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_THE_GAIN_2].ucDataType = SINT_1;
    
    //����ʵ�����棨ͨ��2��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN_2].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN_2].ulAddr     = (ULONG)&g_stRcParamSet.cDlActGain2;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN_2].ucDataType = SINT_1;

    //����������ʵ�ƽ��ͨ��2��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_2].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_2].ulAddr     = (ULONG)&g_stRcParamSet.cUlOutPwr2;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_2].ucDataType = SINT_1;

    //����פ����ֵ��ͨ��2��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR_2].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR_2].ulAddr     = (ULONG)&g_stRcParamSet.ucDlSwr2;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR_2].ucDataType = UINT_1;

    //�������빦�ʵ�ƽ��ͨ��3��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_3].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_3].ulAddr     = (ULONG)&g_stRcParamSet.cDlInPwr3;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_3].ucDataType = SINT_1;

    //����������ʵ�ƽ��ͨ��3��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_3].pfHandler  = HandleMcpARcParamWithThr; 
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_3].ulAddr     = (ULONG)&g_stRcParamSet.cDlOutPwr3;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_3].ucDataType = SINT_1;

    //�����������棨ͨ��3��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_THE_GAIN_3].ulAddr     = (ULONG)&g_stRcParamSet.cUlTheGain3;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_THE_GAIN_3].ucDataType = SINT_1;
    
    //����ʵ�����棨ͨ��3��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN_3].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN_3].ulAddr     = (ULONG)&g_stRcParamSet.cDlActGain3;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN_3].ucDataType = SINT_1;

    //����������ʵ�ƽ��ͨ��3��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_3].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_3].ulAddr     = (ULONG)&g_stRcParamSet.cUlOutPwr3;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_3].ucDataType = SINT_1;

    //����פ����ֵ��ͨ��3��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR_3].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR_3].ulAddr     = (ULONG)&g_stRcParamSet.ucDlSwr3;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR_3].ucDataType = UINT_1;

    //�������빦�ʵ�ƽ��ͨ��4��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_4].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_4].ulAddr     = (ULONG)&g_stRcParamSet.cDlInPwr4;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_4].ucDataType = SINT_1;

    //����������ʵ�ƽ��ͨ��4��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_4].pfHandler  = HandleMcpARcParamWithThr; 
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_4].ulAddr     = (ULONG)&g_stRcParamSet.cDlOutPwr4;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_4].ucDataType = SINT_1;

    //�����������棨ͨ��4��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_THE_GAIN_4].ulAddr     = (ULONG)&g_stRcParamSet.cUlTheGain4;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_THE_GAIN_4].ucDataType = SINT_1;
    
    //����ʵ�����棨ͨ��4��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN_4].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN_4].ulAddr     = (ULONG)&g_stRcParamSet.cDlActGain4;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_ACT_GAIN_4].ucDataType = SINT_1;

    //����������ʵ�ƽ��ͨ��4��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_4].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_4].ulAddr     = (ULONG)&g_stRcParamSet.cUlOutPwr4;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_4].ucDataType = SINT_1;

    //����פ����ֵ��ͨ��4��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR_4].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR_4].ulAddr     = (ULONG)&g_stRcParamSet.ucDlSwr4;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_SWR_4].ucDataType = UINT_1;
    
    //�����ź���·״̬
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_BYPASS_STATUS].ulAddr     = (ULONG)&g_stRcParamSet.ucUlBypassStatus;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_BYPASS_STATUS].ucDataType = UINT_1;

    //�����ź���·״̬
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_BYPASS_STATUS].ulAddr     = (ULONG)&g_stRcParamSet.ucDlBypassStatus;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_BYPASS_STATUS].ucDataType = UINT_1;

    //3G:�������빦�ʵ�ƽ��sint2�ͣ���λΪdBm������Ϊ10������ͬ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX].ulAddr     = (ULONG)&g_stRcParamSet.sDlInPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX].ucDataType = SINT_2;

    //3G:����������ʵ�ƽ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX].ulAddr     = (ULONG)&g_stRcParamSet.sDlOutPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX].ucDataType = SINT_2;

    //3G:����������ʵ�ƽ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX].ulAddr     = (ULONG)&g_stRcParamSet.sUlOutPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX].ucDataType = SINT_2;

    //3G:����פ���ȣ�������(�淶20060810)
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_SWR].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_SWR].ulAddr     = (ULONG)&g_stRcParamSet.ucUlSwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_SWR].ucDataType = UINT_1;    
    
    //3G:�������빦�ʵ�ƽ��ͨ��1��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX_1].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX_1].ulAddr     = (ULONG)&g_stRcParamSet.sDlInPwr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX_1].ucDataType = SINT_2;
    
    //3G:�������빦�ʵ�ƽ��ͨ��2��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX_2].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX_2].ulAddr     = (ULONG)&g_stRcParamSet.sDlInPwr2;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX_2].ucDataType = SINT_2;
    
    //3G:�������빦�ʵ�ƽ��ͨ��3��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX_3].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX_3].ulAddr     = (ULONG)&g_stRcParamSet.sDlInPwr3;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX_3].ucDataType = SINT_2;
    
    //3G:�������빦�ʵ�ƽ��ͨ��4��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX_4].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX_4].ulAddr     = (ULONG)&g_stRcParamSet.sDlInPwr4;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_IN_PWR_EX_4].ucDataType = SINT_2;

    //3G:����������ʵ�ƽ��ͨ��1��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX_1].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX_1].ulAddr     = (ULONG)&g_stRcParamSet.sDlOutPwr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX_1].ucDataType = SINT_2;

    //3G:����������ʵ�ƽ��ͨ��2��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX_2].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX_2].ulAddr     = (ULONG)&g_stRcParamSet.sDlOutPwr2;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX_2].ucDataType = SINT_2;
    
    //3G:����������ʵ�ƽ��ͨ��3��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX_3].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX_3].ulAddr     = (ULONG)&g_stRcParamSet.sDlOutPwr3;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX_3].ucDataType = SINT_2;

    //3G:����������ʵ�ƽ��ͨ��4��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX_4].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX_4].ulAddr     = (ULONG)&g_stRcParamSet.sDlOutPwr4;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DL_OUT_PWR_EX_4].ucDataType = SINT_2;

    //3G:����������ʵ�ƽ��ͨ��1��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX_1].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX_1].ulAddr     = (ULONG)&g_stRcParamSet.sUlOutPwr; //Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX_1].ucDataType = SINT_2;

    //3G:����������ʵ�ƽ��ͨ��2��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX_2].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX_2].ulAddr     = (ULONG)&g_stRcParamSet.sUlOutPwr2;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX_2].ucDataType = SINT_2;

    //3G:����������ʵ�ƽ��ͨ��3��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX_3].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX_3].ulAddr     = (ULONG)&g_stRcParamSet.sUlOutPwr3;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX_3].ucDataType = SINT_2;

    //3G:����������ʵ�ƽ��ͨ��4��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX_4].pfHandler  = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX_4].ulAddr     = (ULONG)&g_stRcParamSet.sUlOutPwr4;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UL_OUT_PWR_EX_4].ucDataType = SINT_2;

    //ʩ���˿�CPICH���ʵ�ƽ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DONOR_CPICH_LEV].ulAddr     = (ULONG)&g_stRcParamSet.sDonorCpichLev;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_DONOR_CPICH_LEV].ucDataType = SINT_2;

    //3G:�û��˿�CPICH���ʵ�ƽ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SERVICE_CPICH_LEV].ulAddr     = (ULONG)&g_stRcParamSet.sServiceCpichLev;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SERVICE_CPICH_LEV].ucDataType = SINT_2;


//------------------------------����ΪPOI��������-----------------------------------

    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_CDMA800_IN_PWR].pfHandler   = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_CDMA800_IN_PWR].ulAddr      = (ULONG)&g_stRcParamSet.cCdma800InPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_CDMA800_IN_PWR].ucDataType  = SINT_1;

    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_MB_GSM_IN_PWR].pfHandler    = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_MB_GSM_IN_PWR].ulAddr       = (ULONG)&g_stRcParamSet.cMbGsmInPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_MB_GSM_IN_PWR].ucDataType   = SINT_1;

    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UC_GSM_IN_PWR].pfHandler    = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UC_GSM_IN_PWR].ulAddr       = (ULONG)&g_stRcParamSet.cUcGsmInPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UC_GSM_IN_PWR].ucDataType   = SINT_1;

    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_MB_DCS_IN_PWR].pfHandler    = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_MB_DCS_IN_PWR].ulAddr       = (ULONG)&g_stRcParamSet.cMbDcsInPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_MB_DCS_IN_PWR].ucDataType   = SINT_1;

    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UC_DCS_IN_PWR].pfHandler    = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UC_DCS_IN_PWR].ulAddr       = (ULONG)&g_stRcParamSet.cUcDcsInPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_UC_DCS_IN_PWR].ucDataType   = SINT_1;

    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_3G1_FDD_IN_PWR].pfHandler   = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_3G1_FDD_IN_PWR].ulAddr      = (ULONG)&g_stRcParamSet.c3G1FDDInPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_3G1_FDD_IN_PWR].ucDataType  = SINT_1;

    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_3G2_FDD_IN_PWR].pfHandler   = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_3G2_FDD_IN_PWR].ulAddr      = (ULONG)&g_stRcParamSet.c3G2FDDInPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_3G2_FDD_IN_PWR].ucDataType  = SINT_1;

    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_3G3_TDD_IN_PWR].pfHandler   = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_3G3_TDD_IN_PWR].ulAddr      = (ULONG)&g_stRcParamSet.c3G3TDDInPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_3G3_TDD_IN_PWR].ucDataType  = SINT_1;

    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_TRUNK_IN_PWR].pfHandler     = HandleMcpARcParamWithThr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_TRUNK_IN_PWR].ulAddr        = (ULONG)&g_stRcParamSet.cTrunkInPwr;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_TRUNK_IN_PWR].ucDataType    = SINT_1;

//------------------------------����ΪPOI��������-----------------------------------

//------------------------------����Ϊ̫���ܿ�������������-----------------------------------

    //���ػ����¶�
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_ENV_TEMP].pfHandler   = HandleAsynOpSECParam;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_ENV_TEMP].ucDataType  = SINT_2;

    //����ѹ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_SB_VOL].pfHandler     = HandleAsynOpSECParam;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_SB_VOL].ucDataType    = UINT_2;

    //zhangjie 20071120 ̫�������ص�ѹ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_ST_BAT_VOL].pfHandler = HandleMcpASecVolR;
	g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_ST_BAT_VOL].ulAddr    = (ULONG)&g_stRcParamSet.usSecBatVol;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_ST_BAT_VOL].ucDataType= UINT_2;
    //zhangjie 20071120 ̫�������ص�ѹ

    //���24V��ѹ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_VOL_24].pfHandler     = HandleAsynOpSECParam;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_VOL_24].ucDataType    = UINT_2;

    //���12V��ѹ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_VOL_12].pfHandler     = HandleAsynOpSECParam;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_VOL_12].ucDataType    = UINT_2;

    //���5V��ѹ
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_VOL_5].pfHandler      = HandleAsynOpSECParam;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_VOL_5].ucDataType     = UINT_2;

    //������
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_SB_CUR].pfHandler     = HandleAsynOpSECParam;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_SB_CUR].ucDataType    = UINT_2;

    //���ص���
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_LOAD_CUR].pfHandler   = HandleAsynOpSECParam;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_LOAD_CUR].ucDataType  = UINT_2;

    //������
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_CHARGE_CUR].pfHandler = HandleAsynOpSECParam;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_CHARGE_CUR].ucDataType= UINT_2;

    //��������ѹ��
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_WIRE_VOL].pfHandler   = HandleAsynOpSECParam;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_WIRE_VOL].ucDataType  = UINT_2;

    //���غɵ���
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_CAPACITY].pfHandler   = HandleAsynOpSECParam;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_CAPACITY].ucDataType  = UINT_2;

    //�������ĵ���
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_CHARGE_CAPACITY].pfHandler  = HandleAsynOpSECParam;
    g_astRtCollParamHndlrTbl[MOBJ_ID_RC_SEC_CHARGE_CAPACITY].ucDataType = UINT_2;

//------------------------------����Ϊ̫���ܿ�������������-----------------------------------

}

void InitMObjHndlrTbl()
{
    LONG i = 0;
    LONG j = 0;
    UCHAR ucIDHiByte = 0;
    UCHAR ucIDLoByte = 0;
    
    //��ʼ����ز��������
    InitDevInfoHndlrTbl();
    InitDevInfoExtHndlrTbl();
    InitNmParamHndlrTbl();
    InitAlarmItemHndlrTbl();
    InitSettingParamHndlrTbl();
    InitRtCollParamHndlrTbl();

    //���ݼ�ز����б��ʼ���Ƿ�֧�ֱ�־
    for(i = 0; i < g_stMObjIdTbl.usParamCount; i++)
    {
        ucIDHiByte = (g_stMObjIdTbl.ausParamID[i] >> 8) & 0xF;
        ucIDLoByte = g_stMObjIdTbl.ausParamID[i] & 0xFF;
        switch(ucIDHiByte) //���ֽڱ�ʾ�������ֻ࣬ȡ����λ
        {
        case MOBJ_ID_DEV_INFO_SET:
            //MCM-37_20061123_linyu_begin
            if(ucIDLoByte < DEV_INFO_HNDLR_TBL_SIZE)
            //MCM-37_20061123_linyu_end
            {
                g_astDevInfoHndlrTbl[ucIDLoByte].ucIsSupported = PARAM_SUPPORTED;
                //g_astDevInfoExHndlrTbl[ucIDLoByte].ucIsSupported = PARAM_SUPPORTED;
            }
            break;
        case MOBJ_ID_NM_PARAM_SET:
            //MCM-37_20061123_linyu_begin
            if(ucIDLoByte < NM_PARAMS_HNDLR_TBL_SIZE)
            //MCM-37_20061123_linyu_end
            {
                g_astNmParamHndlrTbl[ucIDLoByte].ucIsSupported = PARAM_SUPPORTED;
            }
            break;
        case MOBJ_ID_ALARM_ENA_SET:
            if(ucIDLoByte <= MAX_ALARM_ITEM_ID)
            {
                for(j = 0; j < ucIDLoByte; j++) //��Ϊ�澯����ý������У����ucIDLoByte�϶�����ڶ�Ӧ���±�
                {
                    if(g_astAlarmStatusHndlrTbl[j].ucIDLoByte == ucIDLoByte)
                    {
                        g_astAlarmEnaHndlrTbl[j].ucIsSupported = PARAM_SUPPORTED;
                        break;
                    }
                }
            }
            break;
        case MOBJ_ID_ALARM_STATUS_SET:
            if(ucIDLoByte <= MAX_ALARM_ITEM_ID)
            {
                for(j = 0; j < ucIDLoByte; j++) //��ʼ���澯��Ϣ�е��Ƿ�֧�ֱ�־
                {
                    if(g_astAlarmStatusHndlrTbl[j].ucIDLoByte == ucIDLoByte)
                    {
                        g_astAlarmStatusHndlrTbl[j].ucIsSupported = PARAM_SUPPORTED;
                        break;
                    }
                }
            }
            break;
        case MOBJ_ID_SETTING_PARAM_SET:
            //MCM-37_20061123_linyu_begin
            if(ucIDLoByte < SETTING_PARAMS_HNDLR_TBL_SIZE)
            //MCM-37_20061123_linyu_end
            {
                g_astSettingParamHndlrTbl[ucIDLoByte].ucIsSupported = PARAM_SUPPORTED;
            }
            break;
        case MOBJ_ID_RT_COLL_PARAM_SET:
            //MCM-37_20061123_linyu_begin
            if(ucIDLoByte < RT_COLL_PARAMS_HNDLR_TBL_SIZE)
            //MCM-37_20061123_linyu_end
            {
                g_astRtCollParamHndlrTbl[ucIDLoByte].ucIsSupported = PARAM_SUPPORTED;
            }
            break;
        default: //ϵͳ�����򳧼��Զ���
            break;
        }
    }

    //���⴦����ֹ��֧����Ƶ���ص��豸���ڿ���״̬��ȷ�����¸澯���δ���
    if(g_astSettingParamHndlrTbl[MOBJ_ID_SP_RF_SW].ucIsSupported == PARAM_NOT_SUPPORTED)
    {
        g_stSettingParamSet.ucRfSw = 1;
    }
}


