/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   CMCC_1.cpp
    ����:     ����
    �ļ�˵��: ���ļ���ʵ���й��ƶ���ع淶����ӿ�Э��Ķ���
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  2006/02/16
       ����:  ����
       ����:  �������
    2. ����:  2006/11/01
       ����:  �½�
       ����:  �޸�����MCM-11��
              ���ӻ������ñ��ڳ����ʼ��ʱ���ݻ��������������ñ�
              �еĸ�����������á�
              ��ԭ�������漰�����жϵĵط�ȫ����Ϊ�Ի������ñ���ĳ
              ����߶���ֵ���жϡ�
    3. ����:  2006/11/01
       ����:  �½�
       ����:  �޸�����MCM-5��
              �ڳ����������ж�ͨ�ŷ�ʽ�����ϱ�ͨ�ŷ�ʽʱֻʹ��
              "ͨ�ŷ�ʽ"���ID�����������ϱ�ͨ��ʱ������������
              ��ͨ�ŵ�ֵӳ�䵽ͨ�ŷ�ʽ�ϣ�������ʹ��2G��3G�豸
              ���ж�ͨ�ŷ�ʽ�ϵõ���ͳһ�� 
    4. ����:  2006/11/3
       ����:  �ӻ���
       ����:  �޸�����MCM-10
              �ڴ������������첽�����Ĳ�����Ϊ��������������
              ����,��ע�᲻ͬ�Ĳ�������ĺ���������ʱ��������\
    5. ����:  2006/11/7
       ����:  �½�
       ����:  �޸�����MCM-21��
              �޸ı��󣬰Ѻ궨���е�FIRELESSCOUPL ��ΪWIRELESSCOUPL 
    6. ����:  2006/11/8
       ����:  �½�
       ����:  �޸�����MCM-24��
              ���޸Ļ��ͺ�ĳ�ʼ�������б�ʱ����Ҫд��Ĳ����б����ڴ��Խ���ж�
    7. ����:  2006/11/8
       ����:  ����
       ����:  �޸�����MCM-26��
              ��ʼ��ʱ���ü�ز����б��ȼ��㺯�������³���ָʾ��Ԫ��ֵ
    8. ����:  2006/11/9
       ����:  �ӻ���
       ����:  �޸�����MCM-28��
              ������Э�鴦����غ�����CMCC.cpp����YKPP.cpp
    9. ����:  2006/11/10
       ����:  �½�
       ����:  �޸�����MCM-29��
              �ڳ�ʼ��ʱ����������һ��������ֵ
    10. ����:  2006/11/14
        ����:  �ӻ���
        ����:  �޸�����MCM-32��
               �ӵ��л���ذ汾������ʱ���ݳ����������������Ӧ�ı�־λ��
               ʹ��ʱδ���еĳ���bootloader�´�����ʱѡ�����С���Э��
               �����ݴ�����Ϻ����㣬������������ʹ�豸������
    13. ����:  2007/01/05
        ����:  �ӻ���
        ����:  �޸�����MCM-50��
               �������쳣�����ļ������ж�ʱ������ʵ����������ļ�������Ϣ��
               ��дһ��FLASH��SECTOR�ɹ��󣬼�ʱ�����ļ�������Ϣ��
    14. ����:  2007/03/15
        ����:  �ӻ���
        ����:  �޸�����MCM-63
               ���Ӳ�������֧��2g�����豸����               
    15. ����:  2007/03/15
        ����:  �ӻ���
        ����:  �޸�����MCM-64��              
               ��"DevTypeTableInit"�����ó��Ҳ����е�"ͨ�����ӻ�"������
               �豸�������ñ��е�ͨ�����ӻ���־����Ӧ���޸ġ�
    16. ����:  2007/04/03
        ����:  �ӻ���
        ����:  �޸�����MCM-72
               �ϵ��ʼ��������������ּ�ذ�δ����ʼ�������ȳ�ʼ��վ���ź��豸���Ϊ0
    17. ����:  2007/06/14
        ����:  �ӻ���
        ����:  �޸�����MCM-74��              
               �޸��豸���ͺ��������ʱ����"�豸��ʵ���ŵ�����"���ó�2
    18. ����:  2007/11/20
        ����:  �½�
        ����:  �޸�����MCM-80��              
               �����豸����ѡ��̫�������ص�ѹ���ļ�ⷽʽ��
---------------------------------------------------------------------------*/

//****************************����ͷ�ļ�**************************//
#ifdef M3
#include "Queue.h" //M3��ʹ��CheckAlarmStatus()�еĶ��в���
#endif

#include <string.h>
#include "../../Periph/flash/flash.h"
#include "../../include/boot.h"
#include "../MM/MemMgmt.h"
#include "../Util/Util.h"
#include "../include/Interface.h"
#include "../Timer/Timer.h"
#include "../AppMain.h"
#include "OH.h"
#include "YKPP.h"
#include "CMCC_DevConfig.cpp"
#include "CMCC_1.h"

#ifdef UNIT_TEST
#include "../../UT/Stub/Stub.h"
#endif

//*****************************�궨��*****************************//

/*ʹ��AP:A�����ȥ��ʼ������Ԫ��ԭʼ�������Ϊ256�ֽڣ���ȥ���㿪�������ݵ�Ԫ��
  �����Ϊ256-15=241�ֽڣ��۳��ϱ����ͣ���ʣ239�ֽ�
  ʹ��AP:B(SMS)�����ȥ��ʼ������Ԫ��ԭʼ�������Ϊ(140-2)/2=69�ֽڣ���ȥ���㿪
  �������ݵ�Ԫ�������Ϊ69-15=54�ֽڣ��۳��ϱ����ͣ���ʣ50�ֽ�
  ÿ���澯����4�ֽڣ���ǰ�������԰���59���澯�����������԰���12���澯
*/
#define REPORT_LEN_EXCEPT_MOBJ_CONTENT  17
#define MAX_ALARM_ITEM_COUNT_BY_APB     12
#define MAX_ALARM_ITEM_COUNT_BY_APA     59
#define MAX_MOBJ_ID_COUNT_BY_APB        24
#define MAX_MOBJ_ID_COUNT_BY_APA        119
#define MAX_MOBJ_ID_COUNT_BY_APC        200 //???��Ҫͨ��֧�ֵ�����AP:C����������


#define SWAP_WORD(word)     ((word) = ADJUST_WORD((word)))
#define SWAP_DWORD(dword)   ((dword)= ADJUST_DWORD((dword)))


//����MCP���ز����Ĵ������
#define SetErrCode(ucMObjIdHiByte, ucErr, ucFlag) \
    ((ucMObjIdHiByte) = (UCHAR)(((ucMObjIdHiByte) & 0x0F) | (ucErr << 4))), (ucFlag) = RSP_FLAG_PARTIALLY_DONE

//ͨ�Ű���ʶֻѭ���������ֽڣ��������ֽڿ��������Ϊ������;���Ӷ���֤��ʶ������
//��������������ֽ����Ϊ1����������������ĸ澯
#define GEN_PACKET_ID() (++g_ucPacketIdLowByte + 0x8000)
#define GET_PACKET_ID() (g_ucPacketIdLowByte + 0x8000)

//����������ʱ�ñ�־���жϱ�־
USHORT g_usSettingParamBitmap = 0;
#define BITMAP_RF_SW                (1 << 0)
#define BITMAP_CH_NUM               (1 << 1)
#define BITMAP_ATT                  (1 << 2)
#define BITMAP_SMC_ADDR             (1 << 3)
#define BITMAP_MODIFY_IP_ADDR       (1 << 4)
#define BITMAP_MODIFY_PORT_NUM      (1 << 5)
#define BITMAP_MODIFY_DEV_TYPE      (1 << 6)
#define BITMAP_MODIFY_MOB_LIST      (1 << 7)
#define BITMAP_MODIFY_COMM_MODE     (1 << 8)
#define BITMAP_MODIFY_CH_COUNT      (1 << 9)
//MCM-32_20061114_zhonghw_begin
#define BITMAP_SWITCH_FILE_VER      (1 << 10)
//MCM-32_20061114_zhonghw_end

#define FIRST_NO_TIMEOUT                0    //��δ��ʱ
#define FIRST_1_SHORT_TIMEOUT           1    //��һ�ֵ�1�ζ̳�ʱ
#define FIRST_2_SHORT_TIMEOUT           2    //��һ�ֵ�2�ζ̳�ʱ
#define FIRST_WAIT_FOR_LONG_TIMEOUT     0xF  //�ȴ���һ�ֳ���ʱ
#define SECOND_NO_TIMEOUT               0x10 //��һ�ֳ���ʱ
#define SECOND_1_SHORT_TIMEOUT          0x11 //�ڶ��ֵ�1�ζ̳�ʱ
#define SECOND_2_SHORT_TIMEOUT          0x12 //�ڶ��ֵ�2�ζ̳�ʱ
#define SECOND_WAIT_FOR_LONG_TIMEOUT    0x1F //�ȴ��ڶ��ֳ���ʱ
#define THIRD_NO_TIMEOUT                0x20 //�ڶ��ֳ���ʱ
#define THIRD_1_SHORT_TIMEOUT           0x21 //�����ֵ�1�ζ̳�ʱ
#define THIRD_2_SHORT_TIMEOUT           0x22 //�����ֵ�2�ζ̳�ʱ

//**************************ȫ�ֱ�������**************************//

//����ͨ�Ű���ʶ��0x8000��0x8FFF���������豸�����ͨ��
UCHAR g_ucPacketIdLowByte = 0; //ͨ�Ű���ʶ�ĵ��ֽ�

//��ز����б�
MOBJ_ID_TBL_ST g_stMObjIdTbl;

//��Ӧ��ȫ�ֱ���
DEV_INFO_SET_ST      g_stDevInfoSet;
NM_PARAM_SET_ST      g_stNmParamSet;
SETTING_PARAM_SET_ST g_stSettingParamSet;

//ʵʱ��������������Ҫ���籣��
RC_PARAM_SET_ST g_stRcParamSet;


//�澯ʹ��
ALARM_ENABLE_UN g_uAlarmEnable;

//���ĸ澯״̬
ALARM_STATUS_UN g_uCenterStatus;

//���и澯��Ϣ
ALARM_ITEM_UN g_uAlarmItems;

//�澯���ͻ�����
ALARM_ITEM_BUF_ST g_stAlarmItemBuf;


//�澯�ϱ�������������
#define MAX_ALARM_REPORTS  6
ALARM_REPORT_CTX_ST g_astAlarmReportCtx[MAX_ALARM_REPORTS];

//�����ϱ���������
OTHER_REPORT_CTX_ST g_astOtherReportCtx[OTHER_REPORT_COUNT];


//���ڳ���������ȫ�ֱ�������Ҫ������NV Memory��
FILE_PART_DESC_ST g_astFilePartTbl[FPD_COUNT]; //ǰ�������ڳ����ļ������һ�����������ļ�

//�����ļ���������
UPGRADE_CTX_ST g_stUpgradeCtx;

//��ز��������
MOBJ_HNDLR_TBL_ST g_astDevInfoHndlrTbl[DEV_INFO_HNDLR_TBL_SIZE];
MOBJ_HNDLR_TBL_ST g_astDevInfoExHndlrTbl[DEV_INFO_HNDLR_TBL_SIZE];
MOBJ_HNDLR_TBL_ST g_astNmParamHndlrTbl[NM_PARAMS_HNDLR_TBL_SIZE];
ALARM_ITEM_HNDLR_TBL_ST g_astAlarmEnaHndlrTbl[MAX_ALARM_ITEMS];
ALARM_ITEM_HNDLR_TBL_ST g_astAlarmStatusHndlrTbl[MAX_ALARM_ITEMS];
MOBJ_HNDLR_TBL_ST g_astSettingParamHndlrTbl[SETTING_PARAMS_HNDLR_TBL_SIZE];
MOBJ_HNDLR_TBL_ST g_astRtCollParamHndlrTbl[RT_COLL_PARAMS_HNDLR_TBL_SIZE];

//GPRSͨ�ŷ�ʽʱ���غ����ĵ�����״̬
CENTER_CONN_STATUS_ST g_stCenterConnStatus;


//����AP:A��AP:Cת����ַ���
typedef struct
{
    UCHAR ucEscape;
    UCHAR aucBeforeEscape[2];
    UCHAR aucAfterEscape[2];
}APAC_ESCAPE_ST;

const APAC_ESCAPE_ST g_stApacEscape = 
{
    0x5E,
    {0x5E, 0x7E},
    {0x5D, 0x7D}
};

//�������ñ�
DEV_TYPE_TABLE_ST g_stDevTypeTable;

//**************************ȫ�ֱ�������**************************//
#ifndef M3
extern const FLASH_DEV_ST g_stNorFlash0; //���ڲ���FLASH
extern UCHAR g_ucCurrCommMode; //��ǰ��ͨ��״̬����ҪCH����ʼ��
#else
const FLASH_DEV_ST g_stNorFlash0 = {0};
UCHAR g_ucCurrCommMode; //��ǰ��ͨ��״̬����ҪCH����ʼ��
#include "../../Stub/M3Stub.h"
#endif
extern YKPP_PARAM_SET_ST g_stYkppParamSet;


//�����첽������������
extern ASYN_OP_CTX_ST g_astAsynOpCtx[ASYN_OP_CTX_CNT];
extern UCHAR g_ucAsynOpMsgID; //���ڷ��𷽷�����ϢID��ֻȡ��7λ�����λ�̶�Ϊ0

extern ASYN_OP_BUF_ST g_stSECParam;
extern ASYN_OP_BUF_ST g_stTDParam;

//****************************��������****************************//
Timer* OHCreateTimer(void *pvCtx, ULONG ulInterval, ULONG *pulMagicNum);
void OHRemoveTimer(Timer *pTimer, ULONG ulMagicNum);
void InitMObjHndlrTbl();

extern "C" void Restart(void);
extern "C" void SetAtt(void);
extern "C" void SetRfSw(void);
extern "C" void SetFreq(void);
extern "C" void RtcGetDateTime(UCHAR *);
extern "C" LONG RtcSetDateTime(UCHAR *);

extern LONG BuildYkppPdu(YKPP_PDU_ST * pstPdu, ULONG ulPduLen);
LONG HandleYkppData(UCHAR *pucData, ULONG &ulDataLen, UCHAR ucCommMode, UCHAR &ucDir);
LONG ExtractYkppData(UCHAR *pucInput, ULONG ulInputLen, UCHAR *&pucOutput, ULONG &ulOutputLen);
LONG HandleMcpAMObjListWrite(MCPA_MOBJ_ST * pstMObj);
void SaveYkppParam();
//void SaveFileParam();
void ClearAsynOpCtx(UCHAR ucIdx);
void SetReportLampStatus(UCHAR ucLampSta);
void InitYkppDefaultParam();

LONG GetAsynOpCtx(UCHAR &ucIdx);
void DoAsynOpTD(void *pvCtx);
void DoAsynOpSEC(void *pvCtx);

void AsynOpToCommon(void *pvCtx);
//MCM-28_20061109_zhonghw_begin
LONG EncodeAndResponseYkppBuf(OHCH_RECV_DATA_IND_ST * pstPrmv);
//MCM-28_20061109_zhonghw_end

//MCM-32_20061114_zhonghw_begin
LONG SwitchFileVer(void);
LONG HandleSwitchSofVer(MCP_LAYER_ST * pstMcp,LONG);
//MCM-32_20061114_zhonghw_end
/*************************************************
  Function:
  Description:    ͨ��ָʾ�����û���ʾ�ϱ�(��վ�������)�Ľ��
  Calls:
  Called By:
  Input:          ucEvent:�ϱ����¼�
                  lResult:�ϱ��Ľ��
  Output:         
  Return:         
  Others:         
*************************************************/
void NotifyReportResult(UCHAR ucEvent, LONG lResult) 
{
    UCHAR uclampsta = 0;
    switch(ucEvent)
    {
    case REPORT_TYPE_STA_INIT:
    case REPORT_TYPE_INSPECTION:
    case REPORT_TYPE_REPAIR_CONFIRMED:
    case REPORT_TYPE_CONFIG_CHANGED:
        if(lResult != SUCCEEDED)       //��Ϊ���ƺ����ӿ�Ϊ0��1��
        {
            uclampsta = 1;
        }
        SetReportLampStatus(uclampsta);
        break;
    }

}

/*************************************************
  Function:
  Description:    �����������������̵Ķ�ʱ��
  Calls:
  Called By:
  Input:          ucTimerID:  ��ʱ��ID
                  ulInterval: ��ʱʱ��
                  g_stUpgradeCtx.pTimer: ��������ʹ�õĶ�ʱ��
  Output:         
  Return:         
  Others:         
*************************************************/
LONG StartUpgradeTimer(UCHAR ucTimerID, ULONG ulInterval)
{
    OH_TIMER_CTX_UN uCtx;
    uCtx.stParam.ucTimerID = ucTimerID;
    
    if(g_stUpgradeCtx.pTimer) //���ԭ���ж�ʱ����Ҫ��ֹͣ
    {
        OHRemoveTimer(g_stUpgradeCtx.pTimer, g_stUpgradeCtx.ulTimerMagicNum);
        g_stUpgradeCtx.pTimer = 0;
        g_stUpgradeCtx.ulTimerMagicNum = 0;
    }

    g_stUpgradeCtx.pTimer = OHCreateTimer((void *)uCtx.ulParam, ulInterval, 
                                          &g_stUpgradeCtx.ulTimerMagicNum);
    if(g_stUpgradeCtx.pTimer == 0)
    {
        return FAILED;
    }
    else
    {
        return SUCCEEDED;
    }
}

/*************************************************
  Function:
  Description:    ������ֹͣ�������̵Ķ�ʱ��
  Calls:
  Called By:
  Input:          g_stUpgradeCtx.pTimer: ��������ʹ�õĶ�ʱ��
  Output:         
  Return:         
  Others:         
*************************************************/
LONG StopUpgradeTimer()
{
    if(g_stUpgradeCtx.pTimer)
    {
        OHRemoveTimer(g_stUpgradeCtx.pTimer, g_stUpgradeCtx.ulTimerMagicNum);
        g_stUpgradeCtx.pTimer = 0;
        g_stUpgradeCtx.ulTimerMagicNum = 0;
    }
    
    return SUCCEEDED;
}

//���·�װFLASH�Ĳ���������ʹ���ø�����
void SimpleFlashRead(ULONG ulFlashAddr, void *pvData, ULONG ulSize)
{
    FlashRead(g_stNorFlash0.pfwBaseAddr, (UCHAR *)ulFlashAddr, (UCHAR *)pvData, ulSize);
}

LONG SimpleFlashWrite(ULONG ulFlashAddr, void *pvData, ULONG ulSize)
{
    return FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR *)ulFlashAddr, (UCHAR *)pvData, ulSize);
}

LONG SimpleFlashEraseSector(ULONG ulFlashAddr)
{
    return FlashEraseSector(g_stNorFlash0.pfwBaseAddr, (UCHAR *)ulFlashAddr);
}

/*************************************************
  Function:
  Description:    ���������ڱȽ�FLASH�е����ݿ��RAM�е�
                  ���ݿ��Ƿ�һ�£���Ҫ������дFLASH֮ǰ��
                  ���жϣ��������һ��������дFLASH
  Calls:
  Called By:
  Input:          ulFlashAddr: FLASH����ʼ��ַ
                  pvData:      RAM�е���ʼ��ַ
                  ulSize:      ���ݿ鳤��
  Output:         
  Return:         SUCCEEDED:   ����һ��
                  FAILED:      ���߲�һ��
  Others:         
*************************************************/
LONG CmpData(ULONG ulFlashAddr, void *pvData, ULONG ulSize)
{
    ULONG i = 0;
    UCHAR ucBuf = 0;

    for(i = 0; i < ulSize; i++)
    {
        SimpleFlashRead(ulFlashAddr + i, &ucBuf, 1);
        if(ucBuf != *((UCHAR *)pvData + i))
        {
            return FAILED;
        }        
    }

    return SUCCEEDED;
}


/*************************************************
  Function:
  Description:    ���������ڱ���ֻ��������������ز���
                  �б���豸��Ϣ������FLASH����Сд��λ
                  ��4k�����������������ͬһ��sector�У�
                  �����Ҫͬʱ����
  Calls:
  Called By:
  Input:          g_stMObjIdTbl:  ��ز����б�
                  g_stDevInfoSet: �豸��Ϣ
  Output:         
  Return:         
  Others:         ֻ������ֻ��ͨ����չ����ͳ�������д��
*************************************************/
void SaveReadOnlyParam()
{
    while(1)
    {
        if(CmpData(ADDR_MOBJ_ID_TBL, &g_stMObjIdTbl, sizeof(g_stMObjIdTbl)) != SUCCEEDED)
        {
            break;
        }

        if(CmpData(ADDR_DEV_INFO_SET, &g_stDevInfoSet, sizeof(g_stDevInfoSet)) != SUCCEEDED)
        {
            break;
        }
        return; //���ݲ�����ֱ�ӷ���
    }

    if(SimpleFlashEraseSector(ADDR_MOBJ_ID_TBL) != SUCCEEDED)
    {
        return;
    }

    SimpleFlashWrite(ADDR_MOBJ_ID_TBL, &g_stMObjIdTbl, sizeof(g_stMObjIdTbl));
    SimpleFlashWrite(ADDR_DEV_INFO_SET, &g_stDevInfoSet, sizeof(g_stDevInfoSet));
}

/*************************************************
  Function:
  Description:    ���������ڱ������ĸ澯״̬
  Calls:
  Called By:
  Input:          g_uCenterStatus: ���ĸ澯״̬
  Output:         
  Return:         
  Others:         
*************************************************/
void SaveAlarmCenterStatus()
{
    if(CmpData(ADDR_CENTER_STATUS_SET, &g_uCenterStatus, sizeof(g_uCenterStatus)) == SUCCEEDED)
    {
        return; //���ݲ�����ֱ�ӷ���
    }
    
    if(SimpleFlashEraseSector(ADDR_CENTER_STATUS_SET) != SUCCEEDED)
    {
        return;
    }

    SimpleFlashWrite(ADDR_CENTER_STATUS_SET, &g_uCenterStatus, sizeof(g_uCenterStatus));
}

/*************************************************
  Function:
  Description:    ���������ڱ����д�ļ�ز�������������
                  ���������ò������澯ʹ�ܲ������������
                  ������ͬһ��sector�У������Ҫͬʱ����
  Calls:
  Called By:
  Input:          g_stNmParamSet:      ���ܲ���
                  g_stSettingParamSet: ���ò���
                  g_uAlarmEnable:      �澯ʹ�ܲ���
  Output:         
  Return:         
  Others:         
*************************************************/
void SaveWritableParam()
{
    while(1)
    {
        if(CmpData(ADDR_NM_PARAM_SET, &g_stNmParamSet, sizeof(g_stNmParamSet)) != SUCCEEDED)
        {
            break;
        }
        if(CmpData(ADDR_SETTING_PARAM_SET, &g_stSettingParamSet, sizeof(g_stSettingParamSet)) != SUCCEEDED)
        {
            break;
        }
        if(CmpData(ADDR_ALARM_ENABLE_SET, &g_uAlarmEnable, sizeof(g_uAlarmEnable)) != SUCCEEDED)
        {
            break;
        }
        return; //���ݲ�����ֱ�ӷ���
    }

    if(SimpleFlashEraseSector(ADDR_NM_PARAM_SET) != SUCCEEDED)
    {
        return;
    }

    SimpleFlashWrite(ADDR_NM_PARAM_SET, &g_stNmParamSet, sizeof(g_stNmParamSet));
    SimpleFlashWrite(ADDR_SETTING_PARAM_SET, &g_stSettingParamSet, sizeof(g_stSettingParamSet));
    SimpleFlashWrite(ADDR_ALARM_ENABLE_SET, &g_uAlarmEnable, sizeof(g_uAlarmEnable));
}

/*************************************************
  Function:
  Description:    ���������ڱ����������йصĲ�������
                  ��������Ϣ���ļ����������������
                  ����ͬһ��sector�У������Ҫͬʱ����
  Calls:
  Called By:
  Input:          g_stUpgradeCtx:  ������Ϣ
                  g_astFilePartTbl:�ļ�������
  Output:         
  Return:         
  Others:         
*************************************************/
void SaveUpgradeInfo()
{
    while(1)
    {
        if(CmpData(ADDR_UPGRADE_INFO_SET, &g_stUpgradeCtx, sizeof(UPGRADE_INFO_SET_ST)) != SUCCEEDED) //ע���������ṹ��С��ͬ
        {
            break;
        }
        if(CmpData(ADDR_FILE_PART_TBL, &g_astFilePartTbl, sizeof(g_astFilePartTbl)) != SUCCEEDED)
        {
            break;
        }
        return; //���ݲ�����ֱ�ӷ���
    }

    if(SimpleFlashEraseSector(ADDR_UPGRADE_INFO_SET) != SUCCEEDED)
    {
        return;
    }

    SimpleFlashWrite(ADDR_UPGRADE_INFO_SET, &g_stUpgradeCtx, sizeof(UPGRADE_INFO_SET_ST)); //ע���������ṹ��С��ͬ
    SimpleFlashWrite(ADDR_FILE_PART_TBL, &g_astFilePartTbl, sizeof(g_astFilePartTbl));
}

//�����ߵ��ֽ�˳���ڶ�����д��ʱ��ɣ����Ǵ�����Щ��������������ʹ�ò�ͬ���ֽ���
void AdjustEndianOfDataFromNVMemory()
{
    SWAP_WORD(g_stDevInfoSet.usMaxApcLen);
    SWAP_WORD(g_stDevInfoSet.usTg);
    SWAP_WORD(g_stDevInfoSet.usTimeToSwitch);
    
    SWAP_WORD(g_stNmParamSet.usEmsPortNum);
    SWAP_WORD(g_stNmParamSet.usFtpServerPortNum);
    SWAP_WORD(g_stNmParamSet.usHeartbeatPeriod);
    SWAP_DWORD(g_stNmParamSet.ulStaNum);

    SWAP_WORD(g_stSettingParamSet.usShfUbChNum);
    SWAP_WORD(g_stSettingParamSet.usShfUbChNum1);
    SWAP_WORD(g_stSettingParamSet.usShfUbChNum2);
    SWAP_WORD(g_stSettingParamSet.usShfUbChNum3);
    SWAP_WORD(g_stSettingParamSet.usShfUbChNum4);

    SWAP_WORD(g_stSettingParamSet.usShfLbChNum);
    SWAP_WORD(g_stSettingParamSet.usShfLbChNum1);
    SWAP_WORD(g_stSettingParamSet.usShfLbChNum2);
    SWAP_WORD(g_stSettingParamSet.usShfLbChNum3);
    SWAP_WORD(g_stSettingParamSet.usShfLbChNum4);

    SWAP_WORD(g_stSettingParamSet.usWorkUbChNum);     
    SWAP_WORD(g_stSettingParamSet.usWorkUbChNum1);
    SWAP_WORD(g_stSettingParamSet.usWorkUbChNum2);
    SWAP_WORD(g_stSettingParamSet.usWorkUbChNum3);
    SWAP_WORD(g_stSettingParamSet.usWorkUbChNum4);

    SWAP_WORD(g_stSettingParamSet.usWorkLbChNum);
    SWAP_WORD(g_stSettingParamSet.usWorkLbChNum1);
    SWAP_WORD(g_stSettingParamSet.usWorkLbChNum2);
    SWAP_WORD(g_stSettingParamSet.usWorkLbChNum3);
    SWAP_WORD(g_stSettingParamSet.usWorkLbChNum4);

    SWAP_WORD(g_stSettingParamSet.usWorkChNum1);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum2);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum3);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum4);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum5);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum6);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum7);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum8);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum9);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum10);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum11);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum12);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum13);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum14);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum15);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum16);

    SWAP_WORD(g_stSettingParamSet.usShfChNum1);
    SWAP_WORD(g_stSettingParamSet.usShfChNum2);
    SWAP_WORD(g_stSettingParamSet.usShfChNum3);
    SWAP_WORD(g_stSettingParamSet.usShfChNum4);
    SWAP_WORD(g_stSettingParamSet.usShfChNum5);
    SWAP_WORD(g_stSettingParamSet.usShfChNum6);
    SWAP_WORD(g_stSettingParamSet.usShfChNum7);
    SWAP_WORD(g_stSettingParamSet.usShfChNum8);
    SWAP_WORD(g_stSettingParamSet.usShfChNum9);
    SWAP_WORD(g_stSettingParamSet.usShfChNum10);
    SWAP_WORD(g_stSettingParamSet.usShfChNum11);
    SWAP_WORD(g_stSettingParamSet.usShfChNum12);
    SWAP_WORD(g_stSettingParamSet.usShfChNum13);
    SWAP_WORD(g_stSettingParamSet.usShfChNum14);
    SWAP_WORD(g_stSettingParamSet.usShfChNum15);
    SWAP_WORD(g_stSettingParamSet.usShfChNum16);

    SWAP_WORD(g_stSettingParamSet.usSrcCellId);
    
    SWAP_WORD(g_stSettingParamSet.sDlInOverPwrThr);
    SWAP_WORD(g_stSettingParamSet.sDlInOverPwrThr1);
    SWAP_WORD(g_stSettingParamSet.sDlInOverPwrThr2);
    SWAP_WORD(g_stSettingParamSet.sDlInOverPwrThr3);
    SWAP_WORD(g_stSettingParamSet.sDlInOverPwrThr4);

    SWAP_WORD(g_stSettingParamSet.sDlInUnderPwrThr);
    SWAP_WORD(g_stSettingParamSet.sDlInUnderPwrThr1);
    SWAP_WORD(g_stSettingParamSet.sDlInUnderPwrThr2);
    SWAP_WORD(g_stSettingParamSet.sDlInUnderPwrThr3);
    SWAP_WORD(g_stSettingParamSet.sDlInUnderPwrThr4);

    SWAP_WORD(g_stSettingParamSet.sDlOutOverPwrThr);
    SWAP_WORD(g_stSettingParamSet.sDlOutOverPwrThr1);
    SWAP_WORD(g_stSettingParamSet.sDlOutOverPwrThr2);
    SWAP_WORD(g_stSettingParamSet.sDlOutOverPwrThr3);
    SWAP_WORD(g_stSettingParamSet.sDlOutOverPwrThr4);

    SWAP_WORD(g_stSettingParamSet.sDlOutUnderPwrThr);
    SWAP_WORD(g_stSettingParamSet.sDlOutUnderPwrThr1);
    SWAP_WORD(g_stSettingParamSet.sDlOutUnderPwrThr2);
    SWAP_WORD(g_stSettingParamSet.sDlOutUnderPwrThr3);
    SWAP_WORD(g_stSettingParamSet.sDlOutUnderPwrThr4);

    SWAP_WORD(g_stSettingParamSet.sUlOutOverPwrThr);
    SWAP_WORD(g_stSettingParamSet.sUlOutOverPwrThr1);
    SWAP_WORD(g_stSettingParamSet.sUlOutOverPwrThr2);
    SWAP_WORD(g_stSettingParamSet.sUlOutOverPwrThr3);
    SWAP_WORD(g_stSettingParamSet.sUlOutOverPwrThr4);
}

/*************************************************
  Function:
  Description:    ��������ʼ������Ĭ�ϲ���,ϵͳ��⵽�豸����û�б���ʼ�� 
                  (ͨ�����FLASH����Ӧ�ĳ�ʼ��λ)ʱ���ô˺���;���߸ı��豸
                  �������ô˺����������ݻ���������Ӧ�����������
  Calls:
  Called By:
  Input:          g_astFilePartTbl: �ļ�������
  Output:
  Return:         
  Others:         ���������ⲿ����                  

*************************************************/
void InitCmccDefaultParam()
{
    //�豸��Ϣ
    g_stDevInfoSet.ucMnftId       = 10;       //���̴���
    g_stDevInfoSet.usMaxApcLen    = 4096;     //3G:��֧��AP��CЭ�����󳤶�
    g_stDevInfoSet.ucMcpbMode     = 1;        //3G:MCP��B���õĽ�������
    g_stDevInfoSet.ucNc           = 1;        //3G:����ϵ����NC��
    g_stDevInfoSet.ucT1           = 30;       //3G:�豸��Ӧ��ʱ��TOT1��
    g_stDevInfoSet.usTg           = 0;        //3G:���ͼ��ʱ�䣨TG������������NC��1ʱ�������壩
    g_stDevInfoSet.ucTp           = 5;        //3G:��ͣ����ȴ�ʱ�䣨TP��
    g_stDevInfoSet.usTimeToSwitch = 30;       //3G:ת�����������ģʽʱ��OMC��Ҫ�ȴ���ʱ��
    g_stDevInfoSet.ucUpgradeMode  = 1;        //3G:�豸ʹ�õ�Զ��������ʽ


    //���ò���(��Ҫ�����ܽ�)
    g_stSettingParamSet.ucRfSw           = 1;    //��Ƶ�źſ���
    g_stSettingParamSet.ucDlSwrThr       = 30;   //����פ��������                 
    g_stSettingParamSet.cPaOverheatThr   = 60;   //���Ź��¶ȸ澯����
    g_stSettingParamSet.cDlInUnderPwrThr = -100; //�������빦��Ƿ��������
    g_stSettingParamSet.cDlInOverPwrThr  = 100;  //�������빦�ʹ���������
    g_stSettingParamSet.cDlOutUnderPwrThr= -100; //�����������Ƿ��������
    g_stSettingParamSet.cDlOutOverPwrThr = 100;  //����������ʹ���������
    g_stSettingParamSet.cUlOutOverPwrThr = 100;  //����������ʹ���������

    g_stSettingParamSet.sDlInUnderPwrThr = -400;  //3G:�������빦��Ƿ��������
    g_stSettingParamSet.sDlInOverPwrThr  = 400;   //3G:�������빦�ʹ���������
    g_stSettingParamSet.sDlOutUnderPwrThr= -400;  //3G:�����������Ƿ��������
    g_stSettingParamSet.sDlOutOverPwrThr = 400;   //3G:����������ʹ���������
    g_stSettingParamSet.sUlOutOverPwrThr = 400;   //3G:����������ʹ���������
    g_stSettingParamSet.sUlOutUnderPwrThr= -400;  //3G:�����������Ƿ��������
    g_stSettingParamSet.ucUlSwrThr       = 30  ;  //3G:����פ��������      

    //MCM-74_20070614_Zhonghw_begin
    g_stDevInfoSet.ucChCount = 2;    //Ĭ��Ϊ2
    //MCM-74_20070614_Zhonghw_end
    

    //notice�澯ʹ��ȫ��
    memset(&g_uAlarmEnable, 1, sizeof(g_uAlarmEnable));

    //���ĸ澯״̬ȫΪ����
    memset(&g_uCenterStatus, 0, sizeof(g_uCenterStatus));

    //�����ļ���������
    memset(&g_stUpgradeCtx, 0, sizeof(g_stUpgradeCtx));
    g_stUpgradeCtx.ulNextBlockSn  = 0;
    //MCM-50_20070105_Zhonghw_begin
    g_stUpgradeCtx.ulUnSaveBlocks = 0;
    //MCM-50_20070105_Zhonghw_end       
    g_stUpgradeCtx.ulMaxBlockSize = 4096;
    g_stUpgradeCtx.ulSavedFileSize= 0;
    g_stUpgradeCtx.lUpgradeResult = 0;


}

void InitAlarmInfo()
{
    ULONG i;

    //��ʼ��g_uAlarmItems�еĸ澯״̬������״̬������״̬����һ��
    for(i = 0; i < MAX_ALARM_ITEMS; i++)
    {
        if(g_uAlarmEnable.aucAlarmEnableArray[i] > 1)
        {
            g_uAlarmEnable.aucAlarmEnableArray[i] = 0; //��δ��ʼ����ʹ�����㣬��ֹFLASH�е������п���Ϊ0xFF
        }
        
        if(g_uCenterStatus.aucAlarmStatusArray[i] > 1) //��δ��ʼ��������״̬���㣬��ֹFLASH�е������п���Ϊ0xFF
        {
            g_uCenterStatus.aucAlarmStatusArray[i] = 0;
        }

        //��ʼ��g_uAlarmItems�еĸ澯״̬������״̬������״̬����һ��
        g_uAlarmItems.astAlarmItemArray[i].ucLocalStatus = g_uCenterStatus.aucAlarmStatusArray[i];
    
        g_uAlarmItems.astAlarmItemArray[i].ucTotalCount = 0;
        g_uAlarmItems.astAlarmItemArray[i].ucChangedCount = 0;

        //�����и澯���ʼ��Ϊ�������澯�������澯�������ĳ�ʼ������λ
        g_uAlarmItems.astAlarmItemArray[i].ucIsInstant = 0;

        //�Ƿ�֧�ָò���
        g_uAlarmItems.astAlarmItemArray[i].ucSupportedBit = g_astAlarmStatusHndlrTbl[i].ucIsSupported;

        //��ʼ���澯����ID���ֽ�
        g_uAlarmItems.astAlarmItemArray[i].ucIDLoByte = g_astAlarmStatusHndlrTbl[i].ucIDLoByte;
    }

    //λ�ø澯���Ž��澯���ⲿ�澯Ϊ�����澯
    g_uAlarmItems.stAlarmItemStruct.stPosition.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stDoor.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt1.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt2.ucIsInstant = 1;    
    g_uAlarmItems.stAlarmItemStruct.stExt3.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt4.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt5.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt6.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt7.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt8.ucIsInstant = 1;

    //��������̫���ܿ������ĸ澯������Ϊ�����澯
    g_uAlarmItems.stAlarmItemStruct.stSecStBatBlowout.ucIsInstant       = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecSelfResumeBlowout.ucIsInstant  = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecOutputOverCur.ucIsInstant      = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecCircuitFault.ucIsInstant       = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecVol24fault.ucIsInstant         = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecVol12Fault.ucIsInstant         = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecVol5Fault.ucIsInstant          = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecUseUnderVol.ucIsInstant        = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecOverVol.ucIsInstant            = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecOverDischarging.ucIsInstant    = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecOverDischarged.ucIsInstant     = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecStBatFault.ucIsInstant         = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecSBFault.ucIsInstant            = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecDoor.ucIsInstant               = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecAleak.ucIsInstant              = 1;

    //��������̫���ܿ�������ʵʱ�澯״̬������Ϊ����״̬
    //̫����ʵʱ״̬�޷��ڱ����ɼ�����ֻ��ȡ����״̬Ϊʵ
    //ʱ״̬������ʵʱ״̬�ͱ���״̬��һ�µ��µ����ϱ�
    g_uAlarmItems.stAlarmItemStruct.stSecStBatBlowout.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecStBatBlowout.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecSelfResumeBlowout.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecSelfResumeBlowout.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecOutputOverCur.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecOutputOverCur.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecCircuitFault.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecCircuitFault.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecVol24fault.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecVol24fault.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecVol12Fault.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecVol12Fault.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecVol5Fault.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecVol5Fault.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecUseUnderVol.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecUseUnderVol.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecOverVol.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecOverVol.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecOverDischarging.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecOverDischarging.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecOverDischarged.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecOverDischarged.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecStBatFault.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecStBatFault.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecSBFault.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecSBFault.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecDoor.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecDoor.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecAleak.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecAleak.ucLocalStatus;

    //��������̫���ܿ������ĸ澯ʹ�ܶ�����
    g_uAlarmEnable.stAlarmEnableStruct.ucSecStBatBlowout       = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecSelfResumeBlowout  = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecOutputOverCur      = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecCircuitFault       = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecVol24fault         = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecVol12Fault         = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecVol5Fault          = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecUseUnderVol        = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecOverVol            = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecOverDischarging    = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecOverDischarged     = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecStBatFault         = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecSBFault            = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecDoor               = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecAleak              = 1;
}

/*************************************************
  Function:
  Description:    �������Ի������ñ���г�ʼ��
  Calls:
  Called By:
  Input:          
  Output:
  Return:         
  Others:         ���������ⲿ����
*************************************************/
void DevTypeTableInit(void)
{
    UCHAR ucDevtype = g_stDevInfoSet.ucDevType;
    if(ucDevtype > DEV_WITH_SEC)  //����̫���ܿ�����
    {
        ucDevtype = ucDevtype - DEV_WITH_SEC;
    }

    if(ucDevtype < 50)
    {
        g_stDevTypeTable.ucGenType = GEN_TYPE_2G;
    }
    else
    {
        g_stDevTypeTable.ucGenType = GEN_TYPE_3G;
    }

    switch(ucDevtype)
    {
    case WIDE_BAND_2G:
    case OPTICAL_WIDEBAND_FAR_2G:
    //MCM-63_20070315_Zhonghw_begin
    case BI_DIRECT_WIDE_BAND_TOWER_AMP_2G:
    //MCM-63_20070315_Zhonghw_begin
    case OPTICAL_CARRIER_WIDEBAND_LOCAL_2G:
    case OPTICAL_CARRIER_WIDEBAND_FAR_2G:
    case WIDE_BAND:
    case OPTICAL_WIDEBAND_FAR:
    case OPTICAL_CARRIER_WIDEBAND_LOCAL:
    case OPTICAL_CARRIER_WIDEBAND_FAR:
        g_stDevTypeTable.ucSetFreqType = FREQTYPE_WIDEBAND;
        break;

    case WIRELESS_FRESELT_2G:
    //MCM-63_20070315_Zhonghw_begin
    case BI_DIRECT_FRE_SELECT_TOWER_AMP_2G:
    //MCM-63_20070315_Zhonghw_begin        
    case OPTICAL_FRESELT_FAR_2G:
    case OPTICAL_CARRIER_FRESELT_LOCAL_2G:
    case OPTICAL_CARRIER_FRESELT_FAR_2G:
    case WIRELESS_FRESELT:
    case OPTICAL_FRESELT_FAR:
    case OPTICAL_CARRIER_FRESELT_LOCAL:
    case OPTICAL_CARRIER_FRESELT_FAR:
    case FRESHIFT_FRESELT_FAR_2G:
        g_stDevTypeTable.ucSetFreqType = FREQTYPE_FRESELT;
        break;

    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL_2G:
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL_2G:
    case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL_2G:
    case FRESHIFT_FRESELT_FAR:
    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL:
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL:
    case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL:
        g_stDevTypeTable.ucSetFreqType = FREQTYPE_FRESHIFT;
        break;

    case FRESHIFT_WIRELESS_WIDEBAND_LOCAL_2G:
    case FRESHIFT_WIDEBAND_FAR_2G:
    case FRESHIFT_WIRELESS_WIDEBAND_LOCAL:
    case FRESHIFT_WIDEBAND_FAR:
        g_stDevTypeTable.ucSetFreqType = FREQTYPE_FRESHIFT_WIDEBAND;
        break;

    default:
        g_stDevTypeTable.ucSetFreqType = FREQTYPE_WIDEBAND;
        break;
    }


    switch(ucDevtype)
    {
    case OPTICAL_WIDEBAND_FAR_2G:
    case OPTICAL_FRESELT_FAR_2G:
    case OPTICAL_CARRIER_WIDEBAND_FAR_2G:
    case OPTICAL_CARRIER_FRESELT_FAR_2G:
    case OPTICAL_WIDEBAND_FAR:
    case OPTICAL_FRESELT_FAR:
    case OPTICAL_CARRIER_WIDEBAND_FAR:
    case OPTICAL_CARRIER_FRESELT_FAR:
    //MCM-63_20070315_Zhonghw_begin
    case BI_FRE_OPTICAL_FAR_2G:
    //MCM-63_20070315_Zhonghw_begin            
    case 58:
        g_stDevTypeTable.ucOpticalType = OPTICAL_REMOTE;
        g_stDevTypeTable.ucCommRoleType = COMM_ROLE_SLAVE;
        g_stDevTypeTable.ucRFRoleType = RF_SLAVE;
        break;

    case OPTICAL_DIRECOUPL_LOCAL_2G:
    case OPTICAL_CARRIER_WIDEBAND_LOCAL_2G:
    case OPTICAL_CARRIER_FRESELT_LOCAL_2G:
    case OPTICAL_WIRELESSCOUPL_LOCAL_2G:
    case OPTICAL_DIRECOUPL_LOCAL:
    case OPTICAL_CARRIER_WIDEBAND_LOCAL:
    case OPTICAL_CARRIER_FRESELT_LOCAL:
    case OPTICAL_WIRELESSCOUPL_LOCAL:
    //MCM-63_20070315_Zhonghw_begin
    case BI_FRE_OPTICAL_DIRECOUPL_LOCAL_2G:
    //MCM-63_20070315_Zhonghw_begin                    
    case 59:
        g_stDevTypeTable.ucOpticalType = OPTICAL_LOCAL;
        g_stDevTypeTable.ucCommRoleType = COMM_ROLE_MASTER;
        g_stDevTypeTable.ucRFRoleType = RF_MASTER;
        break;

    case TRUNK_AMPLIFIER_2G:
    case TRUNK_AMPLIFIER:
        g_stDevTypeTable.ucOpticalType = OPTICAL_NO;
        g_stDevTypeTable.ucCommRoleType = COMM_ROLE_SLAVE;
        g_stDevTypeTable.ucRFRoleType = RF_MASTER;
        break;

    default:
        g_stDevTypeTable.ucOpticalType = OPTICAL_NO;
        g_stDevTypeTable.ucCommRoleType = COMM_ROLE_MASTER;
        g_stDevTypeTable.ucRFRoleType = RF_MASTER;
        break;
    }

    switch(ucDevtype)
    {    
    case WIRELESS_FRESELT:
    case OPTICAL_WIRELESSCOUPL_LOCAL:
    case OPTICAL_FRESELT_FAR:
    case OPTICAL_CARRIER_FRESELT_LOCAL:
    case OPTICAL_CARRIER_FRESELT_FAR:
    case WIRELESS_FRESELT_2G:
    case OPTICAL_WIRELESSCOUPL_LOCAL_2G:
    case OPTICAL_FRESELT_FAR_2G:
    case OPTICAL_CARRIER_FRESELT_LOCAL_2G:
    case OPTICAL_CARRIER_FRESELT_FAR_2G:
    //MCM-63_20070315_Zhonghw_begin
    case BI_FRE_OPTICAL_FAR_2G:
    case BI_DIRECT_FRE_SELECT_TOWER_AMP_2G:
    //MCM-63_20070315_Zhonghw_begin                    
    
        g_stDevTypeTable.ucChannelType = ONLY_CH;        
        break;
        
    case FRESHIFT_FRESELT_FAR:
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL:
    case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL:
    case FRESHIFT_FRESELT_FAR_2G:
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL_2G:
    case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL_2G:
        g_stDevTypeTable.ucChannelType = CH_SHIFT;
        break;
            
    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL:
        g_stDevTypeTable.ucChannelType = ONLY_SHIFT;
        break;
        
    default:
        g_stDevTypeTable.ucChannelType = CH_SHIFT_NOTHING;
        break;
    }

    switch(ucDevtype)
    {
    case OPTICAL_DIRECOUPL_LOCAL_2G:
    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL_2G:
    case OPTICAL_CARRIER_WIDEBAND_LOCAL_2G:
    case OPTICAL_CARRIER_FRESELT_LOCAL_2G:
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL_2G:
    case OPTICAL_DIRECOUPL_LOCAL:
    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL:
    case OPTICAL_CARRIER_WIDEBAND_LOCAL:
    case OPTICAL_CARRIER_FRESELT_LOCAL:
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL:
    //MCM-63_20070315_Zhonghw_begin
    case BI_FRE_OPTICAL_DIRECOUPL_LOCAL_2G:
    //MCM-63_20070315_Zhonghw_begin                    
    case 59:
        g_stDevTypeTable.ucIsCoupling= TRUE;
        break;
        
    default:
        g_stDevTypeTable.ucIsCoupling = FALSE;
        break;
    }

    //Ŀǰ��ʾ�������ڴ���0��
    g_stDevTypeTable.ucUIUSARTType = UI_USART_0;

    //MCM-64_20070315_Zhonghw_begin
    if(g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType== COMM_ROLE_MASTER)
    {
        g_stDevTypeTable.ucCommRoleType = COMM_ROLE_MASTER;
    }
    //MCM-64_20070315_Zhonghw_end
}

/*************************************************
  Function:
  Description:    ��������ϵͳ�������ȫ�ֱ������г�ʼ��
  Calls:
  Called By:
  Input:          g_astFilePartTbl: �ļ�������
  Output:
  Return:         
  Others:         ���������ⲿ����
*************************************************/
void CmccInit()
{
    //MCM-26_20061108_linyu_begin
    //��ʼ����ز����б�
    InitDevCfgTable();
    //MCM-26_20061108_linyu_end
    //��NV Memory����������
    SimpleFlashRead(ADDR_MOBJ_ID_TBL, &g_stMObjIdTbl, sizeof(g_stMObjIdTbl));    
    SimpleFlashRead(ADDR_DEV_INFO_SET, &g_stDevInfoSet, sizeof(g_stDevInfoSet));    
    SimpleFlashRead(ADDR_NM_PARAM_SET, &g_stNmParamSet, sizeof(g_stNmParamSet));
    SimpleFlashRead(ADDR_SETTING_PARAM_SET, &g_stSettingParamSet, sizeof(g_stSettingParamSet));
    SimpleFlashRead(ADDR_ALARM_ENABLE_SET, &g_uAlarmEnable, sizeof(g_uAlarmEnable));
    SimpleFlashRead(ADDR_CENTER_STATUS_SET, &g_uCenterStatus, sizeof(g_uCenterStatus));
    SimpleFlashRead(ADDR_FILE_PART_TBL, &g_astFilePartTbl, sizeof(g_astFilePartTbl));

    //��ʼ���������ñ�
    DevTypeTableInit();

    //�����2G������Ҫ���ϱ�ͨ�ŷ�ʽ������ӳ�䵽ͨ�ŷ�ʽ����
    if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)
    {
        switch(g_stNmParamSet.ucReportCommMode)
        {
        case COMM_REPORT_MODE_GPRS:
            g_stNmParamSet.ucCommMode = COMM_MODE_GPRS;
            break;
        case COMM_REPORT_MODE_SMS:
            g_stNmParamSet.ucCommMode = COMM_MODE_SMS;
            break;
        case COMM_REPORT_MODE_CSD:
            g_stNmParamSet.ucCommMode = COMM_MODE_CSD;
            break;
        default:
            break;
        }
    }

#ifdef BIG_ENDIAN
    //AdjustEndianOfDataFromNVMemory();
#endif //ifdef BIG_ENDIAN

    //��ʼ����������ص���Ϣ
    SimpleFlashRead(ADDR_UPGRADE_INFO_SET, &g_stUpgradeCtx, sizeof(UPGRADE_INFO_SET_ST)); //ע���������ṹ��С��ͬ
    memset((UCHAR *)&g_stUpgradeCtx + sizeof(UPGRADE_INFO_SET_ST), 0, sizeof(g_stUpgradeCtx) - sizeof(UPGRADE_INFO_SET_ST));

    //�������NV Memory��ȫ�ֱ�������    
    memset(&g_stRcParamSet, 0, sizeof(g_stRcParamSet));
    
    memset(&g_stAlarmItemBuf, 0, sizeof(g_stAlarmItemBuf));
    memset(&g_astAlarmReportCtx, 0, sizeof(g_astAlarmReportCtx));
    memset(&g_astOtherReportCtx, 0, sizeof(g_astOtherReportCtx));

    //notice ��ز�����������������Ϊ0����ʱͨ�����û��Ϳɳ�ʼ�������б���չ���������ڲ����б��У�
    if(g_stMObjIdTbl.usParamCount > 2048)
    {
        g_stMObjIdTbl.usParamCount = 0;
    }
    //��ʼ��MCP:A��ز��������
    InitMObjHndlrTbl();

    //��ʼ���͸澯��ص���Ϣ�������ڼ�ز���������ʼ����ɺ����
    InitAlarmInfo();

    //���汾�Ŵ��ļ������������ʼ��
    if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucIsRunning)
    {
        memcpy(g_stDevInfoSet.acSfwrVer, 
               g_astFilePartTbl[PROGRAM_PART_1_IDX].acVersion,
               sizeof(g_stDevInfoSet.acSfwrVer));
    }
    else if(g_astFilePartTbl[PROGRAM_PART_2_IDX].ucIsRunning)
    {
        memcpy(g_stDevInfoSet.acSfwrVer, 
               g_astFilePartTbl[PROGRAM_PART_2_IDX].acVersion,
               sizeof(g_stDevInfoSet.acSfwrVer));
    }
    else
    {
        memset(g_stDevInfoSet.acSfwrVer, '?', sizeof(g_stDevInfoSet.acSfwrVer));
    }
    

    //������״̬
    g_stDevInfoSet.ucRunningMode = RUNNING_MODE_MONITOR;

    //MCM-72_20070403_Zhonghw_begin
    //Ϊ�˼��������̣����㹵ͨ��⣬�ڴ��ȳ�ʼ��վ���ź��豸���
    if((g_stNmParamSet.ulStaNum == 0xFFFFFFFF)&&(g_stNmParamSet.ucDevNum == 0xFF))
    {
        g_stNmParamSet.ulStaNum = 0;
        g_stNmParamSet.ucDevNum = 0; 
    }
    //MCM-72_20070403_Zhonghw_end
    
    //���������õ�Ӳ������Ч�������������
    SetAtt();
    SetRfSw();
    SetFreq();

    //��ʼ����GPRS��¼��صĲ���
    g_stCenterConnStatus.ucLinkStatus           = COMM_STATUS_DISCONNECTED;
    g_stCenterConnStatus.ucLoginStatus          = COMM_STATUS_LOGOUT;
    g_stCenterConnStatus.ucConnAttempCnt        = 0;
    g_stCenterConnStatus.ucNoRspCnt             = 0;
    g_stCenterConnStatus.ucLoginTimeoutCnt      = 0;
    
    //MCM-29_20061110_zhangjie_begin
    g_stRcParamSet.cDlInPwr = (CHAR)((30+80)/2 - g_stYkppParamSet.stYkppRfParam.ucDlInCoupler);
    //MCM-29_20061110_zhangjie_end
}


/*************************************************
  Function:
  Description:    �����������������в���������Э�����ݰ�������ҵ�
                  �ͷ��أ�������ʼ��־��
  Calls:
  Called By:
  Input:          ucFlag:       ��ɨ�����ʼ������־
                  pucInput:     ��ɨ�������
                  ulInputLen:   ��ɨ������ݳ��ȣ�������ʼ������־
  Output:         pulOutputLen: 0��ʾû�в鵽�������ݰ��������ʾ�������ݰ��ĳ���
                  usState:      ����ɨ���״̬
  Return:         ָ�����������ݰ�ͷ
  Others:         
*************************************************/
UCHAR *ScanAP(UCHAR ucFlag, UCHAR *pucInput, ULONG ulInputLen, ULONG *pulOutputLen, USHORT *pusState)
{
    ULONG ulOffset = 0;
    UCHAR *pucStartFlag = NULL;
    
    *pulOutputLen = 0;
    *pusState = NO_FLAG_FOUND;

    while(ulOffset < ulInputLen)
    {
        switch(*pusState) //����״̬���������ַ�
        {
        case NO_FLAG_FOUND:
            if(pucInput[ulOffset] == ucFlag)
            {
                *pusState = AP_START_FLAG_FOUND;
                pucStartFlag = pucInput + ulOffset;
            }
            break;
        case AP_START_FLAG_FOUND:
            if(pucInput[ulOffset] == ucFlag)
            {
                *pusState = AP_END_FLAG_FOUND;
                *pulOutputLen = pucInput + ulOffset - pucStartFlag + 1;
            }
            break;
        default:
            break; //������������֧�������������
        }

        ulOffset++;

        //������Ч���ݣ���ʱ״̬ΪAPAC_END_FLAG_FOUND��APB_END_FLAG_FOUND
        if(*pulOutputLen != 0)
        {
            *pusState += ucFlag;
            return pucStartFlag;
        }
    }

    //û���ҵ�Э�����ݰ�����ʱ״̬ΪNO_FLAG_FOUND��APAC_START_FLAG_FOUND��APB_START_FLAG_FOUND
    if(*pusState != NO_FLAG_FOUND)
    {
        *pusState += ucFlag;
    }
    return pucStartFlag;
}

/*************************************************
  Function:
  Description:    ����������ScanAP()�Խ��յ���Э�����ݽ���ɨ�裬������Ч��Э������
                  ��ȡ���������������ֻ���ܰ���һ��Э�����ݰ����������²㱣֤�ģ�
                  ����Ϊ�˴�����걸����Ȼ���ǿ����յ���������APЭ��
  Calls:
  Called By:
  Input:          pucInput:     ��ɨ�������
                  ulInputLen:   ��ɨ������ݳ��ȣ�������ʼ������־
  Output:         ppucOutput:   ��ȡ�������������ݰ���������ʼ������־����������������ݰ�����Ч
                  pulOutputLen: 0��ʾû�в鵽�������ݰ��������ʾ�������ݰ��ĳ���
  Return:         ����Ľ��
  Others:         
*************************************************/
LONG ExtractCMCCProtoData(UCHAR *pucInput, ULONG ulInputLen, UCHAR **ppucOutput, ULONG *pulOutputLen)
{
    UCHAR *pucData  = pucInput;
    ULONG ulDataLen = ulInputLen;
    USHORT usState   = 0;

    UCHAR *pucApac = NULL;
    ULONG ulApacLen= 0;
    UCHAR *pucApb  = NULL;
    ULONG ulApbLen = 0;

    //ɨ��AP:A��AP:C���ݰ�
    UCHAR *pucStart = ScanAP(APAC_FLAG, pucData, ulDataLen, &ulApacLen, &usState);
    switch(usState)
    {
    case APAC_END_FLAG_FOUND:
        pucApac = pucStart;        
        break;
    case APAC_START_FLAG_FOUND:
    case NO_FLAG_FOUND:
        break;
    }

    //ɨ��AP:B���ݰ�
    pucStart = ScanAP(APB_FLAG, pucData, ulDataLen, &ulApbLen, &usState);

    switch(usState)
    {
    case APB_END_FLAG_FOUND:
        pucApb = pucStart;        
        break;
    case APB_START_FLAG_FOUND:
    case NO_FLAG_FOUND:
        break;
    }
    
    if((ulApacLen > 0) && (ulApbLen > 0)) //�����������Э������
    {
        if(pucApac < pucApb) //AP:AC��ǰ
        {
            *ppucOutput = pucApac;
            *pulOutputLen = ulApacLen;
        }
        else
        {
            *ppucOutput = pucApb;
            *pulOutputLen = ulApbLen;
        }
        return CMCC_PROTO_DATA_FOUND;
    }
    else if(ulApacLen > 0) //ֻ����AP:AC
    {
        *ppucOutput = pucApac;
        *pulOutputLen = ulApacLen;
        return CMCC_PROTO_DATA_FOUND;
    }
    else if(ulApbLen > 0) //ֻ����AP:B
    {
        *ppucOutput = pucApb;
        *pulOutputLen = ulApbLen;
        return CMCC_PROTO_DATA_FOUND;
    }

    return NO_CMCC_PROTO_DATA_FOUND;
}

/*************************************************
  Function:
  Description:    ��������AP��Э�����ݽ��н��룬���ݲ�ͬAPЭ�����ͽ��ж��ֽںϲ�
                  ��CRCУ�飬����ȡNP��PDU�ͳ���
  Calls:
  Called By:
  Input:          pucApData:       AP������ָ�룬ָ����ʼ��־֮��ĵ�һ���ֽ�
                  ulApDataLen:     AP�����ݳ��ȣ���������ʼ������־
  Output:         pulRawApDataLen: AP���ԭʼ���ݳ��ȣ�����CRC����������ʼ������־
                  ppucVpData:      VP�������
                  *ulVpDataLen:    VP���PDU����
  Return:         AP_A:            AP_A���͵�����
                  AP_B:            AP_B���͵�����
                  AP_C:            AP_C���͵�����
                  FAILED:          ���Ժ��Ը����ݰ�
  Others:         
*************************************************/
LONG DecodeApData(UCHAR *pucApData, ULONG ulApDataLen, ULONG *pulRawApDataLen, 
                  UCHAR **ppucVpData, ULONG *ulVpDataLen)
{
    USHORT usCrc = 0;
    LONG lApType = FAILED;

    //�ж�����
    switch(*pucApData)
    {
    case AP_A: //��Ҫת��
        ulApDataLen = DecodeEscape(pucApData, ulApDataLen, g_stApacEscape.ucEscape,
                                   g_stApacEscape.aucAfterEscape,
                                   g_stApacEscape.aucBeforeEscape, 
                                   sizeof(g_stApacEscape.aucAfterEscape));
        lApType = AP_A;
        break;
    case AP_C:
        ulApDataLen = DecodeEscape(pucApData, ulApDataLen, g_stApacEscape.ucEscape,
                                   g_stApacEscape.aucAfterEscape,
                                   g_stApacEscape.aucBeforeEscape, 
                                   sizeof(g_stApacEscape.aucAfterEscape));
        lApType = AP_C;
        break;
    case '0': //AP:B��ֺ�Э������Ϊ��02�������жϵ�һ���ֽ��Ƿ���ȷ
        if(*(pucApData + 1) == '2')
        {
            ulApDataLen = Merge2Bytes(pucApData, ulApDataLen); //���ж��ֽںϲ�
            lApType = AP_B;
        }
        else
        {
            return FAILED;
        }        
        break;
    default: //���Բ�����
        return FAILED;
    }

    //�ж�AP�����Ч����
    if(ulApDataLen < AP_OVERHEAD)
    {
        return FAILED;
    }

    //CRCУ��
    usCrc = CalcuCRC(pucApData, ulApDataLen - CRC_LEN);
    //if(*((USHORT*)(pucApData + ulApDataLen - CRC_LEN)) != ADJUST_WORD(usCrc))
    if(GET_WORD(pucApData + ulApDataLen - CRC_LEN) != usCrc)
    {
        return FAILED; //У�����
    }

    *pulRawApDataLen = ulApDataLen; //����AP��ԭʼ���ݳ���

    //��ȡVP��PDU
    *ulVpDataLen = ulApDataLen - 1 - 1 - CRC_LEN; //��ȥЭ�����͡�����Э�����ͺ�CRC��Ԫ
    *ppucVpData = ((AP_LAYER_ST *)pucApData)->aucPdu;

    return lApType;
}

/*************************************************
  Function:
  Description:    ��������AP��Э�����ݽ��б��룬���ݲ�ͬAPЭ�����ͽ��ж��ֽڲ��
                  ��CRCУ�飬�����ر�����AP���ݺͳ��ȣ����еĲ������ڴ������
                  �ݿ��н��У����÷����뱣֤���ݿ�����㹻�ı���ռ䣻ͨ�������
                  ���ݿ���Ǵ�Ž���AP�����ݵĵط�
  Calls:
  Called By:
  Input:          pucRawApData:   AP���ԭʼ����ָ�룬ָ����ʼ��־��ĵ�һ���ֽڣ�
                                  ͬʱ��Ҫ��֤��ָ���ǰһ���ֽ���Ч�����������ʼ��־��
                  ulRawApDataLen: AP���ԭʼ���ݳ��ȣ���������ʼ������־��У�鵥Ԫ
                  ucApType:       APЭ������
                  ulBufSize:      �����ڱ�����ڴ���С
  Output:         
  Return:         0:              ����ʧ��
                  ��0:            ���������ݳ��ȣ�������ʼ������־��У�鵥Ԫ
  Others:
*************************************************/
ULONG EncodeApData(UCHAR *pucRawApData, ULONG ulRawApDataLen, UCHAR ucApType, ULONG ulBufSize)
{
    USHORT usCrc = 0;
    ULONG ulApDataLen = 0;
    
    AP_LAYER_ST *pstApData = (AP_LAYER_ST *)pucRawApData;
    pstApData->ucApType    = ucApType;
    pstApData->ucPduType   = VP_A; //����Э�����͹̶�ΪVP:A

    //�Ƚ���CRCУ��
    usCrc = CalcuCRC(pucRawApData, ulRawApDataLen);
    //*((USHORT *)(pucRawApData + ulRawApDataLen)) = ADJUST_WORD(usCrc); //����Ϊ���ֽ���ǰ�����ֽ��ں�
    *(pucRawApData + ulRawApDataLen) = GET_WORD_1ST_BYTE(usCrc);
    *(pucRawApData + ulRawApDataLen + 1) = GET_WORD_2ND_BYTE(usCrc);

    //�ж�����
    if(ucApType == AP_B)
    {
        ulApDataLen = SplitInto2Bytes(pucRawApData, ulRawApDataLen + CRC_LEN); //���ж��ֽڲ��
        if(ulApDataLen == 0)
        {
            return 0;
        }
        *(pucRawApData - 1) = APB_FLAG; //��������ǰ�������ʼ��־�����������֤ǰһ���ֽ�����Ч��
        *(pucRawApData + ulApDataLen) = APB_FLAG;
        return ulApDataLen + 2;
    }
    else
    {
        ulApDataLen = EncodeEscape(pucRawApData, ulRawApDataLen + CRC_LEN,
                                   ulBufSize,
                                   g_stApacEscape.ucEscape,
                                   g_stApacEscape.aucAfterEscape,
                                   g_stApacEscape.aucBeforeEscape,
                                   sizeof(g_stApacEscape.aucAfterEscape)); //����ת�崦��
        if(ulApDataLen == 0)
        {
            return 0;
        }
        *(pucRawApData - 1) = APAC_FLAG; //��������ǰ�������ʼ��־�����������֤ǰһ���ֽ�����Ч��
        *(pucRawApData + ulApDataLen) = APAC_FLAG;
        return ulApDataLen + 2;
    }
}

/*************************************************
  Function:
  Description:    ���������������������ڶ�ʱ���������һ������������
                  ���ģ��ͼ�����������ݽ���ʱ�������ڶ�ʱ����Ҫ��
                  ������������ڵȴ�����Ӧ��������
  Calls:
  Called By:
  Input:          
  Output:
  Return:         
  Others:         
*************************************************/
void RestartHeartbeatPeriodTimer()
{
    OH_TIMER_CTX_UN uCtx;
    ULONG ulIdx = REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE;

//    if(g_astOtherReportCtx[ulIdx].ucCommState != OH_IDLE)
//    {
//        return; //���Ѿ�������������ʱ���������������ڶ�ʱ��
//    }
    
    if(g_astOtherReportCtx[ulIdx].pTimer) //���ԭ���ж�ʱ����Ҫ��ֹͣ
    {
        if(g_astOtherReportCtx[ulIdx].ucCommState == OH_IDLE) //������������ڶ�ʱ������Ҫ����
        {
            OHRemoveTimer(g_astOtherReportCtx[ulIdx].pTimer, 
                          g_astOtherReportCtx[ulIdx].ulTimerMagicNum);
            g_astOtherReportCtx[ulIdx].pTimer = 0;
            g_astOtherReportCtx[ulIdx].ulTimerMagicNum = 0;              
        }
        else
        {
            return;
        }
    }

    //������ʱ��
    uCtx.stParam.ucTimerID = TIMER_HEARTBEAT_PERIOD_ID;
    uCtx.stParam.ucParam1  = REPORT_TYPE_HEARTBEAT; 

    g_astOtherReportCtx[ulIdx].pTimer = OHCreateTimer((void *)uCtx.ulParam, TIMER_HEARTBEAT_PERIOD_INTV, 
                                                     &(g_astOtherReportCtx[ulIdx].ulTimerMagicNum));
} 

/*************************************************
  Function:
  Description:    ���������޸�ȷ���ϱ��¼�����У�����ֻ�й����޸����
                  �ָܻ��ĸ澯�������ǵı��ظ澯״̬��Ϊ����
  Calls:
  Called By:
  Input:          
  Output:
  Return:         
  Others:         ���������ⲿ����
*************************************************/
void ActionAfterRepairConfirmReport()
{
    //�Լ��澯
    g_uAlarmItems.stAlarmItemStruct.stSelfOsc.ucLocalStatus = 0;
    g_uAlarmItems.stAlarmItemStruct.stSelfOscShutdown.ucLocalStatus = 0;
    g_uAlarmItems.stAlarmItemStruct.stSelfOscReduceAtt.ucLocalStatus= 0;

    //��ص�ع��ϸ澯
    g_uAlarmItems.stAlarmItemStruct.stBatFalut.ucLocalStatus = 0;    
}

/*************************************************
  Function:
  Description:    �����������յ����ϱ�Ӧ��
  Calls:
  Called By:
  Input:          usPackeID: ���ݰ�ID
                  ucRspFlag: Ӧ���־
  Output:         
  Return:         IGNORE:    ���ٽ��к�������
  Others:         
*************************************************/
LONG HandleReportRsp(USHORT usPacketID, UCHAR ucRspFlag)
{
    UCHAR ucCtxIdx = 0;
    ULONG i = 0;
    ULONG j = 0;
    OH_TIMER_CTX_UN uCtx;
    
    //�澯�������в���
    for(ucCtxIdx = 0; ucCtxIdx < MAX_ALARM_REPORTS; ucCtxIdx++)
    {
        if(g_astAlarmReportCtx[ucCtxIdx].usPacketID == usPacketID)
        {
            //ֹͣ��ʱ��
            if(g_astAlarmReportCtx[ucCtxIdx].pTimer)
            {
                OHRemoveTimer(g_astAlarmReportCtx[ucCtxIdx].pTimer, g_astAlarmReportCtx[ucCtxIdx].ulTimerMagicNum);
                g_astAlarmReportCtx[ucCtxIdx].pTimer = 0;
                g_astAlarmReportCtx[ucCtxIdx].ulTimerMagicNum = 0;
            }
            g_astAlarmReportCtx[ucCtxIdx].usPacketID  = 0;
            g_astAlarmReportCtx[ucCtxIdx].ucCommState = OH_IDLE;

            g_stAlarmItemBuf.ulAlarmItemCount -= g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemCount;

            //���Ӧ���־��Ϊ0�����ܸ��¼�����ĸ澯״̬
            if(ucRspFlag == RSP_FLAG_SUCCEEDED)
            {
                //���Ҹ澯���ͻ��������״̬�ж�Ӧ�ĸ澯�����Ӧ�ÿ���ʹ�ö��ַ����Ч��
                for(i = g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemBegin; i < g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemCount; i++)
                {
                    for(j = 0; j < MAX_ALARM_ITEMS; j++)
                    {
                        if(g_uAlarmItems.astAlarmItemArray[j].ucIDLoByte
                        == g_stAlarmItemBuf.astAlarmItems[i].ucIDLoByte)
                        {
                            g_uCenterStatus.aucAlarmStatusArray[j]  //��������״̬����Ҫ���浽NV Memory��
                                    = g_stAlarmItemBuf.astAlarmItems[i].ucStatus;
                            j++;
                            break;
                        }
                    }

                    //��ո澯���ͻ���
                    g_stAlarmItemBuf.astAlarmItems[i].ucUsed     = 0;
                    g_stAlarmItemBuf.astAlarmItems[i].ucStatus   = 0;
                    g_stAlarmItemBuf.astAlarmItems[i].ucIDLoByte = 0;
                    
                    if(j == MAX_ALARM_ITEMS)
                    {
                        break;
                    }
                }

                SaveAlarmCenterStatus();
            }

            return IGNORE;
        }
    }

    //�������ϱ����������в���
    for(i = 0; i < OTHER_REPORT_COUNT; i++)
    {
        if(g_astOtherReportCtx[i].usPacketID == usPacketID)
        {
            //ֹͣ��ʱ��
            if(g_astOtherReportCtx[i].pTimer)
            {
                OHRemoveTimer(g_astOtherReportCtx[i].pTimer, g_astOtherReportCtx[i].ulTimerMagicNum);
                g_astOtherReportCtx[i].pTimer = 0;
                g_astOtherReportCtx[i].ulTimerMagicNum = 0;
            }
            
            g_astOtherReportCtx[i].ucCommState = OH_IDLE;

            if(i == REPORT_TYPE_LOGIN - OTHER_REPORT_BASE) //��¼��Ӧ��
            {
                if(ucRspFlag == RSP_FLAG_SUCCEEDED) //��¼�ɹ�
                {
                    g_stCenterConnStatus.ucLoginStatus = COMM_STATUS_LOGIN;

                    //RestartHeartbeatPeriodTimer(); //��ʼ�����������ڶ�ʱ�����������ͳһ����
                }
                else //���һ��ʱ�����µ�¼
                {
                    uCtx.stParam.ucTimerID = TIMER_LOGIN_PERIOD_ID;    
                    uCtx.stParam.ucParam1  = REPORT_TYPE_LOGIN;

                    g_astOtherReportCtx[i].pTimer = OHCreateTimer((void *)uCtx.ulParam, TIMER_LOGIN_PERIOD_INTV, 
                                                                 &(g_astOtherReportCtx[i].ulTimerMagicNum));
                }
            }
            else if(i == REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE) //������Ӧ��
            {
                //���һ��ʱ���ط���������Ϊ�����ĵ����ݽ������ᵼ���������ڶ�ʱ����������˷��ڽ�������ʱִ��
            }
            else
            {
                //֪ͨ�ϱ��Ľ��
                NotifyReportResult((UCHAR)i + OTHER_REPORT_BASE, ucRspFlag);

                if((i == REPORT_TYPE_STA_INIT - OTHER_REPORT_BASE) 
                && (ucRspFlag == RSP_FLAG_SUCCEEDED) && (g_stNmParamSet.ucCommMode == COMM_MODE_GPRS))
                {
                    //���֮ǰδ��¼�ɹ������ʱ���ٵ�¼������Ϊ�Ѿ���¼�ɹ�
                    g_stCenterConnStatus.ucLoginStatus = COMM_STATUS_LOGIN;

                    //RestartHeartbeatPeriodTimer(); //��ʼ�����������ڶ�ʱ�����������ͳһ����
                }
                
                //�޸�ȷ�Ϻ���Ҫ����ĳЩ�澯״̬
                if((i == REPORT_TYPE_REPAIR_CONFIRMED - OTHER_REPORT_BASE)
                 &&(ucRspFlag == RSP_FLAG_SUCCEEDED))
                {
                    ActionAfterRepairConfirmReport();
                }
                break;
            }
        }
    }
    
    return IGNORE;
}

/*************************************************
  Function:
  Description:    ��������VP�������ݽ��н���
  Calls:
  Called By:
  Input:          pucVpData:     VP������ָ��
                  ulVpDataLen:   VP�����ݳ���
  Output:         ppucMcpData:   MCP���PDU
                  pulMcpDataLen: MCP���PDU����
  Return:         SUCCEEDED:     ���Խ��к�������
                  IGNORE:        ���Ժ��Ը����ݰ��������ݰ�����ȷ������Ӧ���ʱ
                  NEED_TO_FORWARD: ��Ҫת�������ݰ�
                  
  Others:         
*************************************************/
LONG DecodeVpData(UCHAR *pucVpData, ULONG ulVpDataLen,
                  UCHAR **ppucMcpData, ULONG *pulMcpDataLen)
{
    VPA_LAYER_ST *pstVpData = (VPA_LAYER_ST *)pucVpData;
    
    //1.�ж�VP�����ݵ���Ч����
    if(ulVpDataLen < sizeof(VPA_LAYER_ST))
    {
        return IGNORE;
    }
    
    //2.վ���ź��豸����ж�
    if(g_stYkppParamSet.stYkppCtrlParam.ucAddrAuthSw != 0) //�ж��Ƿ���Ҫ�Ե�ַ���м�Ȩ
    {
        if((ADJUST_DWORD(pstVpData->ulStaNum) != g_stNmParamSet.ulStaNum)||(pstVpData->ucDevNum != g_stNmParamSet.ucDevNum))
        {
            return NEED_TO_FORWARD; //ϵͳ��Ų�ͬ��Ҫת��
        }
    }
    
    //3.VP�㽻����־
    switch(pstVpData->ucVpFlag)
    {
    case VP_FLAG_COMMAND:   //���󣬲�����ͨ�Ű���ʶ
        pstVpData->ucVpFlag = VP_FLAG_SUCCEEDED; //Ӧ���VP�㽻����־
        *ppucMcpData   = pstVpData->aucPdu;
        *pulMcpDataLen = ulVpDataLen - sizeof(VPA_LAYER_ST) + MCP_MIN_LEN;        
        return SUCCEEDED;

    case VP_FLAG_SUCCEEDED: //Ӧ��
        //����Э��涨��֪����VP��ΪӦ��ʱֻҪʹ��ͨ�Ű���ʶ��ƥ�伴��֪����Ӧ���ϱ�����
        return HandleReportRsp(ADJUST_WORD(pstVpData->usPacketID),          //�����ֽ���
                               ((MCP_LAYER_ST *)pstVpData->aucPdu)->ucRspFlag);

    case VP_FLAG_FAILED: //Э��涨�豸æ�޷�������˼�����Ĳ���������־
    default: //�����ͳ����Զ��壬�ݲ�����
        return IGNORE;
    }
}

/*********************************************************************************
  Function:
  Description:    ����������MCP:A�еļ�ز����б��ѯ
  Calls:
  Called By:
  Input:          pstMObj:    MCP������ָ��  
                  ucApType:   AP��������
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpAMObjList(MCPA_MOBJ_ST *pstMObj, UCHAR ucApType)
{
    LONG lRspFlag = MOBJ_CORRECT;
    UCHAR ucMaxMObjIDCountPerQuery = 0;
    UCHAR ucTotalQueryCount        = 0;
    UCHAR ucQueriedMObjIDLen       = 0;

    //�жϡ���ض��������ض��󳤶Ȳ�ƥ�䡱
    if(pstMObj->ucMObjLen != 5)
    {
        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_VALUE_OUT_OF_BOUND, lRspFlag);
        return lRspFlag; 
    }

    if(ucApType == AP_A)
    {
        ucMaxMObjIDCountPerQuery = MAX_MOBJ_ID_COUNT_BY_APA;
    }
    else if(ucApType == AP_B)
    {
        ucMaxMObjIDCountPerQuery = MAX_MOBJ_ID_COUNT_BY_APB;
    }
    else //AP_C
    {
        ucMaxMObjIDCountPerQuery = MAX_MOBJ_ID_COUNT_BY_APC;
    }

    //��ѯ���豸֧�ֵļ�ز����ܸ���
    if(g_stMObjIdTbl.usParamCount % ucMaxMObjIDCountPerQuery > 0)
    {
        ucTotalQueryCount = g_stMObjIdTbl.usParamCount / ucMaxMObjIDCountPerQuery + 1;
    }
    else
    {
        ucTotalQueryCount = g_stMObjIdTbl.usParamCount / ucMaxMObjIDCountPerQuery;
    }

    if((pstMObj->aucMObjContent[0] > ucTotalQueryCount)
     ||(pstMObj->aucMObjContent[1] > ucTotalQueryCount)
     ||(pstMObj->aucMObjContent[0] == 0)
     ||(pstMObj->aucMObjContent[1] == 0))
    {
        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_VALUE_OUT_OF_BOUND, lRspFlag); //����ض������ݳ�����Χ��
    
        //���˴��Ĵ������Ϊ2ʱ����ض���������Ϊ�豸���ص�ʵ��ֵ(�ӿڹ淶8.6)
        //�����б�ĵ�һ�����֣�������һ���յ���ѯ���������
        pstMObj->aucMObjContent[0] = ucTotalQueryCount;
        pstMObj->aucMObjContent[1] = 1;

        if(ucTotalQueryCount > pstMObj->aucMObjContent[1])
        {
            ucQueriedMObjIDLen = ucMaxMObjIDCountPerQuery * UINT_2;                    
        }
        else
        {
            ucQueriedMObjIDLen = g_stMObjIdTbl.usParamCount * UINT_2;                    
        }

        memcpy(&pstMObj->aucMObjContent[2], g_stMObjIdTbl.ausParamID, ucQueriedMObjIDLen);
    }
    else if(pstMObj->aucMObjContent[0] == 1) //��һ�β�ѯ���˴����жϵ�ǰ��ѯ���
    {
        pstMObj->aucMObjContent[0] = ucTotalQueryCount;
        pstMObj->aucMObjContent[1] = 1;

        if(ucTotalQueryCount > pstMObj->aucMObjContent[1])
        {
            ucQueriedMObjIDLen = ucMaxMObjIDCountPerQuery * UINT_2; 
        }
        else
        {
            ucQueriedMObjIDLen = g_stMObjIdTbl.usParamCount * UINT_2; 
        }

        memcpy(&pstMObj->aucMObjContent[2], g_stMObjIdTbl.ausParamID, ucQueriedMObjIDLen);
    }
    else //����pstMObj->aucMObjContent�����ڼ��β�ѯ
    {
        if(ucTotalQueryCount > pstMObj->aucMObjContent[1])
        {
            ucQueriedMObjIDLen = ucMaxMObjIDCountPerQuery * UINT_2;                    
        }
        else
        {
            ucQueriedMObjIDLen = (g_stMObjIdTbl.usParamCount - (ucTotalQueryCount - 1) * ucMaxMObjIDCountPerQuery) * UINT_2;
        }

        memcpy(&pstMObj->aucMObjContent[2],
               (UCHAR *)&g_stMObjIdTbl.ausParamID + (pstMObj->aucMObjContent[1] - 1) * ucMaxMObjIDCountPerQuery * UINT_2,
               ucQueriedMObjIDLen);
    }

    pstMObj->ucMObjLen += ucQueriedMObjIDLen;
    return lRspFlag;
}

/*************************************************
  Function:
  Description:    ���������л�������ģʽʱ���ã�ѡ�������ļ�
                  ��ŵķ���������ʼ����������������
  Calls:
  Called By:
  Input:          g_astFilePartTbl: �ļ�������
  Output:
  Return:         
  Others:
*************************************************/
void SwitchToUpgradeMode()
{
    //ѡ�������ļ���ŵķ���
    if((g_astFilePartTbl[PROGRAM_PART_1_IDX].ucStatus == PROGRAM_STATUS_RUNNABLE)
    && (g_astFilePartTbl[PROGRAM_PART_2_IDX].ucStatus == PROGRAM_STATUS_RUNNABLE))
    {
        if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucNew == FILE_OLD)
        {
            g_stUpgradeCtx.ulUpgradePartIdx = PROGRAM_PART_1_IDX;
        }
        else
        {
            g_stUpgradeCtx.ulUpgradePartIdx = PROGRAM_PART_2_IDX;
        }        
    }
    else if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucStatus == PROGRAM_STATUS_RUNNABLE)
    {
        g_stUpgradeCtx.ulUpgradePartIdx = PROGRAM_PART_2_IDX;
    }
    else if(g_astFilePartTbl[PROGRAM_PART_2_IDX].ucStatus == PROGRAM_STATUS_RUNNABLE)
    {
        g_stUpgradeCtx.ulUpgradePartIdx = PROGRAM_PART_1_IDX;
    }
    else
    {
        //������???
        if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucNew == FILE_OLD)
        {
            g_stUpgradeCtx.ulUpgradePartIdx = PROGRAM_PART_1_IDX;
        }
        else
        {
            g_stUpgradeCtx.ulUpgradePartIdx = PROGRAM_PART_2_IDX;
        }          
    }

    //��ʼ���������̵������ģ�����������CmccInit()�����
    g_stUpgradeCtx.ulUpgradeProgress= TRANS_CTRL_INIT;

    //�л�������ģʽ
    g_stDevInfoSet.ucRunningMode = RUNNING_MODE_UPGRADE;
    
    //������ʱ��
    StartUpgradeTimer(TIMER_TRANS_CTRL_ID, TIMER_TRANS_CTRL_INTV);
}

/*************************************************
  Function:
  Description:    �����������MCP:A�ļ�ز����Ĳ�ѯ�����ã����в�ѯ��ز���
                  ���⴦������ľ�ʹ�ô�����еĻص���������
  Calls:
  Called By:
  Input:          pucMcpData:    MCP������ָ��
                  pulMcpDataLen: MCP�����ݳ���
                  ucApType:      APЭ������
                  usPacketID:    �����
  Output:         pulMcpDataLen: ������MCP�����ݳ��ȣ���ѯ��ز����б�ʱ���Ȼ�ı�
  Return:         SUCCEEDED:     ���Խ��к�������
                  IGNORE:        ���Ժ��Ը����ݰ�
  Others:         
*************************************************/
LONG HandleMcpA(UCHAR* pucMcpData, ULONG *pulMcpDataLen, UCHAR ucApType, USHORT usPacketID)
{
    MCP_LAYER_ST *pstMcp  = (MCP_LAYER_ST *)pucMcpData;
    LONG lMcpDataLen      = (LONG)*pulMcpDataLen;
    ULONG ulActualLen     = 0; //���ڷ��ش�����ʵ�ʳ���
    MCPA_MOBJ_ST *pstMObj = NULL;
    MOBJ_HNDLR pfHndlr    = NULL;
    LONG lRspFlag         = 0;
    LONG j                = 0;

    if(pstMcp->ucRspFlag != RSP_FLAG_CMD)
    {
        return IGNORE; //���Ӧ���־����ȷ��Ӧ��
    }

    pstMcp->ucRspFlag = RSP_FLAG_SUCCEEDED;
    
    switch(pstMcp->ucCmdId)
    {
    case MCPA_CMD_ID_QUERY:
    case MCPA_CMD_ID_SET:
        break;
    case MCPA_CMD_ID_SWITCH_TO_UPGRADE: 
        SwitchToUpgradeMode();
        return SUCCEEDED;
    //MCM-32_20061114_zhonghw_begin
    case MCPA_CMD_ID_SWITCH_FILE_VER:
        if(HandleSwitchSofVer(pstMcp,lMcpDataLen) != SUCCEEDED)
        {
            pstMcp->ucRspFlag = RSP_FLAG_OTHER_ERR;
        }
        return SUCCEEDED;
    //MCM-32_20061114_zhonghw_end        
        
    default: 
        pstMcp->ucRspFlag = RSP_FLAG_CMD_ID_ERR;
        return SUCCEEDED;
    }

    pstMObj = (MCPA_MOBJ_ST *)(pstMcp->aucContent);
    lMcpDataLen -= 2; //�۳����Ԫ����
    ulActualLen += 2;
    while(lMcpDataLen >= MCPA_MOBJ_MIN_LEN) //L��T��ռ2�ֽ�
    {
        if(pstMObj->ucMObjLen < MCPA_MOBJ_MIN_LEN) //��ض��󳤶Ȳ�����֮������ݾͲ��ٴ���
        {
            pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR;
            return SUCCEEDED;
        }

        if(pstMObj->ucMObjLen > lMcpDataLen) //����ʵ�����ݳ���
        {
            pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR;
            return SUCCEEDED;
        }

        switch(pstMObj->aucMObjId[HI_BYTE] & 0xF) //���ֽڱ�ʾ�������ֻ࣬ȡ����λ
        {
        case MOBJ_ID_DEV_INFO_SET:
            if(pstMObj->aucMObjId[LO_BYTE] == MOBJ_ID_DI_MOBJ_TABLE) //��ѯ��ز����б������⴦��
            {
                pstMcp->ucRspFlag = (UCHAR)HandleMcpAMObjList(pstMObj, ucApType);
                ulActualLen += pstMObj->ucMObjLen;
                *pulMcpDataLen = ulActualLen;

                return SUCCEEDED; //�����ľͲ��ٴ�����ѯ��ز����б�Ӧ����������ѯ��
            }
            if(pstMObj->aucMObjId[LO_BYTE] < DEV_INFO_HNDLR_TBL_SIZE)
            {
                pfHndlr = g_astDevInfoHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].pfHandler;
                if(pfHndlr)
                {
                    lRspFlag = pfHndlr(pstMObj, 
                                       g_astDevInfoHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ulAddr,
                                       g_astDevInfoHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucDataType,
                                       g_astDevInfoHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucIsSupported,
                                       pstMcp->ucCmdId);
                    break;
                }
            }

            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        case MOBJ_ID_NM_PARAM_SET:
            if(pstMObj->aucMObjId[LO_BYTE] < NM_PARAMS_HNDLR_TBL_SIZE)
            {
                pfHndlr = g_astNmParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].pfHandler;
                if(pfHndlr)
                {
                    lRspFlag = pfHndlr(pstMObj, 
                                       g_astNmParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ulAddr,
                                       g_astNmParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucDataType,
                                       g_astNmParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucIsSupported,
                                       pstMcp->ucCmdId);
                    break;
                }
            }

            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        case MOBJ_ID_ALARM_ENA_SET:
            if(pstMObj->aucMObjId[LO_BYTE] <= MAX_ALARM_ITEM_ID)
            {
                for(j = 0; j < pstMObj->aucMObjId[LO_BYTE]; j++) //��Ϊ�澯����ý������У����ucIDLoByte�϶�����ڶ�Ӧ���±�
                {
                    if(g_astAlarmEnaHndlrTbl[j].ucIDLoByte == pstMObj->aucMObjId[LO_BYTE])
                    {
                        pfHndlr = g_astAlarmEnaHndlrTbl[j].pfHandler;
                        if(pfHndlr)
                        {
                            lRspFlag = pfHndlr(pstMObj, 
                                               g_astAlarmEnaHndlrTbl[j].ulAddr,
                                               g_astAlarmEnaHndlrTbl[j].ucDataType,
                                               g_astAlarmEnaHndlrTbl[j].ucIsSupported,
                                               pstMcp->ucCmdId);
                        }
                        else
                        {
                            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                        }
                        break;
                    }
                }
                if(j == pstMObj->aucMObjId[LO_BYTE])
                {
                    SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                }
            }                       
            else
            {
                SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            }
            break;
        case MOBJ_ID_ALARM_STATUS_SET:
            if(pstMObj->aucMObjId[LO_BYTE] <= MAX_ALARM_ITEM_ID)
            {
                for(j = 0; j < pstMObj->aucMObjId[LO_BYTE]; j++) //��Ϊ�澯����ý������У����ucIDLoByte�϶�����ڶ�Ӧ���±�
                {
                    if(g_astAlarmStatusHndlrTbl[j].ucIDLoByte == pstMObj->aucMObjId[LO_BYTE])
                    {
                        pfHndlr = g_astAlarmStatusHndlrTbl[j].pfHandler;
                        if(pfHndlr)
                        {
                            lRspFlag = pfHndlr(pstMObj, 
                                               g_astAlarmStatusHndlrTbl[j].ulAddr,
                                               g_astAlarmStatusHndlrTbl[j].ucDataType,
                                               g_astAlarmStatusHndlrTbl[j].ucIsSupported,
                                               pstMcp->ucCmdId);
                        }
                        else
                        {
                            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                        }
                        break;
                    }
                }
                if(j == pstMObj->aucMObjId[LO_BYTE])
                {
                    SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                }
            }                       
            else
            {
                SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            }
            break;
        case MOBJ_ID_SETTING_PARAM_SET:
            if(pstMObj->aucMObjId[LO_BYTE] < SETTING_PARAMS_HNDLR_TBL_SIZE)
            {
                pfHndlr = g_astSettingParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].pfHandler;
                if(pfHndlr)
                {
                    lRspFlag = pfHndlr(pstMObj, 
                                       g_astSettingParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ulAddr,
                                       g_astSettingParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucDataType,
                                       g_astSettingParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucIsSupported,
                                       pstMcp->ucCmdId);
                    break;
                }
            }

            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        case MOBJ_ID_RT_COLL_PARAM_SET:
            if(pstMObj->aucMObjId[LO_BYTE] < RT_COLL_PARAMS_HNDLR_TBL_SIZE)
            {
                pfHndlr = g_astRtCollParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].pfHandler;
                if(pfHndlr)
                {
                    lRspFlag = pfHndlr(pstMObj, 
                                       g_astRtCollParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ulAddr,
                                       g_astRtCollParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucDataType,
                                       g_astRtCollParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucIsSupported,
                                       pstMcp->ucCmdId);
                    break;
                }
            }

            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        case MOBJ_ID_DEV_INFO_SET_EX:
            if(pstMObj->aucMObjId[LO_BYTE] == MOBJ_ID_DI_MOBJ_TABLE) //��ѯ��ز����б������⴦��
            {
                pstMcp->ucRspFlag = (UCHAR)HandleMcpAMObjListWrite(pstMObj);
                
                break;
            }            
            if(pstMObj->aucMObjId[LO_BYTE] < DEV_INFO_HNDLR_TBL_SIZE)
            {
                pfHndlr = g_astDevInfoExHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].pfHandler;
                if(pfHndlr)
                {
                    lRspFlag = pfHndlr(pstMObj,
                                       g_astDevInfoExHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ulAddr,
                                       g_astDevInfoExHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucDataType,
                                       g_astDevInfoExHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucIsSupported,
                                       pstMcp->ucCmdId);
                    break;
                }
            }

            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        default: //ϵͳ�����򳧼��Զ���
            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        }

        if(lRspFlag != RSP_FLAG_SUCCEEDED)
        {
            pstMcp->ucRspFlag = (UCHAR)lRspFlag;
        }

        //������һ����ض���
        lMcpDataLen -= pstMObj->ucMObjLen;
        ulActualLen  += pstMObj->ucMObjLen;
        pstMObj = (MCPA_MOBJ_ST *)(((UCHAR *)pstMObj) + pstMObj->ucMObjLen);     
    }
    
    *pulMcpDataLen = ulActualLen;

    //���������浽NV Memory��
    SaveReadOnlyParam();
    SaveWritableParam();
    SaveAlarmCenterStatus();

    return SUCCEEDED;
}

/*************************************************
  Function:
  Description:    ��������ʼ���������̵��������е�״̬����
                  ׼�����պ������ļ����ݰ����κ�״̬���յ�
                  ��ʼ�����ָʾ��������׼������
  Calls:
  Called By:
  Input:          g_astFilePartTbl: �����ļ�����                  
  Output:         g_stUpgradeCtx:   �����ļ��õ���������
  Return:         
  Others:         
*************************************************/
void StartFileTransmission()
{
    //��ʼ���������̵������ĵ�״̬
    g_stUpgradeCtx.stFileBuf.ulUsedSize = 0; //���������
    g_stUpgradeCtx.ulUpgradeProgress    = TRANS_CTRL_START;
    g_stUpgradeCtx.lUpgradeResult       = UPGRADE_DONE;

    //MCM-50_20070105_zhonghw_begin
    g_stUpgradeCtx.ulUnSaveBlocks = 0;      
    //MCM-50_20070105_zhonghw_end
    //������ʱ��
    StartUpgradeTimer(TIMER_TRANS_CTRL_ID, TIMER_TRANS_CTRL_INTV);
}

/*************************************************
  Function:
  Description:    ���������ļ��������е�����д��FLASH��ÿ�ζ�дһ��sector
  Calls:
  Called By:
  Input:          g_astFilePartTbl: �����ļ�����
                  g_stUpgradeCtx: �����ļ��õ���������
  Output:         
  Return:         SUCCEEDED��ʾ�ɹ���������ʾʧ��
  Others:         
*************************************************/
LONG WriteFileBlockToFlash()
#ifndef M3
{
    if(SimpleFlashEraseSector(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr
                            + g_stUpgradeCtx.ulSavedFileSize) != SUCCEEDED)
    {
        return FAILED;
    }

    if(SimpleFlashWrite(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr
                      + g_stUpgradeCtx.ulSavedFileSize,
                        g_stUpgradeCtx.stFileBuf.aucBuf,
                        g_stUpgradeCtx.stFileBuf.ulUsedSize) != SUCCEEDED)
    {
        return FAILED;
    }
    return  SUCCEEDED;
}
#else
{
    return SaveProgramFile(g_stUpgradeCtx.ulUpgradePartIdx,
                           g_stUpgradeCtx.ulSavedFileSize,
                           g_stUpgradeCtx.stFileBuf.aucBuf,
                           g_stUpgradeCtx.stFileBuf.ulUsedSize);
}
#endif

/*************************************************
  Function:
  Description:    �������������ص��ļ����ݿ飬�������ļ�������ʱ��д��FLASH
                  ����ȴ���һ�����ݿ飬�����ļ����ݿ�ʱ����ʹ��Ӧ�𷵻ش���
  Calls:
  Called By:
  Input:          pucData:   �ļ����ݿ�
                  ulDataLen: ���ݿ鳤�ȣ��������ļ���ĳ��ȣ�������ǰ���L��T����
                             ���÷���֤�ļ��鳤�Ȳ�������󳤶�
                  ulSn:      ��ǰ���ݿ��SN
  Output:         g_stUpgradeCtx: �������̵�������
  Return:         TRANS_RSP_OK:   �ɹ���Ӧ��
                  TRANS_RSP_CANCEL_REQ: ����ȡ������
  Others:         FLASH_SECTOR_SIZE�϶������ݰ�����ulDataLen��������
*************************************************/
LONG RecvFileBlock(UCHAR *pucData, ULONG ulDataLen, ULONG ulSn)
{
    //�ж�״̬
    if(g_stUpgradeCtx.ulUpgradeProgress == TRANS_CTRL_START)
    {
        g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_TRANSFERRING;

    }

    //ֹͣ��ʱ��
    StopUpgradeTimer();
    if(g_stUpgradeCtx.ulUpgradeProgress != TRANS_CTRL_TRANSFERRING)
    {
        //����״̬���յ������ݰ�����Ӧ��������ȡ����������������״̬������
        //g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_ABORT;
        //g_stUpgradeCtx.lUpgradeResult    = UPGRADE_ABORTED;

        //MCM-50_20070105_zhonghw_begin
        //�������ݰ�
        g_stUpgradeCtx.ulNextBlockSn -= g_stUpgradeCtx.ulUnSaveBlocks;
        g_stUpgradeCtx.ulUnSaveBlocks = 0;                        
        //MCM-50_20070105_zhonghw_end
        
    return TRANS_RSP_CANCEL_REQ;
    }

    if(ulSn == 0) //SN��0��ʼ
    {
        //������¿�ʼ���أ����ʼ��Next SN��SavedFileSize
        g_stUpgradeCtx.ulNextBlockSn = 0;
        g_stUpgradeCtx.ulSavedFileSize = 0;
        
        //MCM-50_20070105_zhonghw_begin
        g_stUpgradeCtx.ulUnSaveBlocks = 0;
        //MCM-50_20070105_zhonghw_end
    }
    
    //������ݰ����Ȳ��㣬˵�������һ�������Ƚ�����뻺�棬���յ��ļ���������������ʱ����д��flash
    

    //�����ݷ����ļ�������
    memcpy(g_stUpgradeCtx.stFileBuf.aucBuf + g_stUpgradeCtx.stFileBuf.ulUsedSize, 
           pucData, ulDataLen);
    g_stUpgradeCtx.stFileBuf.ulUsedSize += ulDataLen;

    g_stUpgradeCtx.ulNextBlockSn++;
    
    //MCM-50_20070105_zhonghw_begin
    g_stUpgradeCtx.ulUnSaveBlocks++;
    //MCM-50_20070105_zhonghw_end

    if(g_stUpgradeCtx.stFileBuf.ulUsedSize >= FLASH_SECTOR_SIZE)
    {
        //��������д��FLASH
        if(WriteFileBlockToFlash() != SUCCEEDED)
        {
            //g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_ABORT;
            g_stUpgradeCtx.lUpgradeResult = UPGRADE_SAVE_FILE_FAILED;
            
            //MCM-50_20070105_zhonghw_begin
            //�������ݰ�
            g_stUpgradeCtx.ulNextBlockSn -= g_stUpgradeCtx.ulUnSaveBlocks;
            g_stUpgradeCtx.ulUnSaveBlocks = 0;                        
            //MCM-50_20070105_zhonghw_end
            
            return TRANS_RSP_CANCEL_REQ;
        }

        //MCM-50_20070105_zhonghw_begin
        g_stUpgradeCtx.ulUnSaveBlocks = 0;
        //MCM-50_20070105_zhonghw_end
        
        g_stUpgradeCtx.stFileBuf.ulUsedSize = 0; //���������

        g_stUpgradeCtx.ulSavedFileSize += FLASH_SECTOR_SIZE; //������д��FLASH���ļ���С

        //MCM-50_20070105_Zhonghw_begin
        //����������Ϣ����ֹ�����е��絼��������Ϣδ������
        SaveUpgradeInfo();
        //MCM-50_20070105_Zhonghw_end        
    }

    //����������ʱ��
    StartUpgradeTimer(TIMER_TRANS_BLK_ID, TIMER_TRANS_BLK_INTV);

    return TRANS_RSP_OK; 
}

/*************************************************
  Function:
  Description:    ���������ļ�������Ϻ���ã����ļ����ݴ�FLASH��
                  ������������У���룬���ں��ļ���ʶ�е�У�������
                  �Ƚ�
  Calls:
  Called By:
  Input:          g_astFilePartTbl: �ļ�������
  Output:         g_stUpgradeCtx:   �������̵�������
  Return:         SUCCEEDED:        У��ɹ�
                  FAILED:           У��ʧ��
  Others:         
*************************************************/
LONG ValidateFile()
{
/*
    //�ļ���ʶ���е�ǰ���ֽ�ʱ�ļ�CRCУ���룬���ֽ���ǰ�����ֽ��ں�
    USHORT usCrcInFileID = g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].aucFileId[0] 
                        + (g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].aucFileId[1] << 8);

    //���ļ���FLASH�ж������ֶν���У�飬ֱ�����������ļ�ʱ�Ļ�����
    ULONG ulCheckedFileSize = 0;
    USHORT usCrc = 0;

    //һ��һ��sector���ݵĶ�ȡ������У��ֵ
    while((LONG)(g_stUpgradeCtx.ulSavedFileSize - ulCheckedFileSize) >= FLASH_SECTOR_SIZE)
    {
        SimpleFlashRead(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr + ulCheckedFileSize,
                        g_stUpgradeCtx.stFileBuf.aucBuf, 
                        FLASH_SECTOR_SIZE);

        usCrc = CalcuCRC(g_stUpgradeCtx.stFileBuf.aucBuf, FLASH_SECTOR_SIZE, usCrc);
        ulCheckedFileSize += FLASH_SECTOR_SIZE;
    }

    //����һ��sector���ݵĶ�ȡ������У��ֵ
    if(g_stUpgradeCtx.ulSavedFileSize > ulCheckedFileSize)
    {
        SimpleFlashRead(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr + ulCheckedFileSize,
                        g_stUpgradeCtx.stFileBuf.aucBuf, 
                        g_stUpgradeCtx.ulSavedFileSize - ulCheckedFileSize);

        usCrc = CalcuCRC(g_stUpgradeCtx.stFileBuf.aucBuf, FLASH_SECTOR_SIZE, usCrc);
    }

    //�Ƚ��ļ�У����
    if(usCrcInFileID == usCrc)
    {
        return SUCCEEDED;
    }
    else
    {
        return FAILED;
    }
*/
//test
return SUCCEEDED;
}

/*************************************************
  Function:
  Description:    �����������ļ����ع��̣����ļ�βд��FLASH��ͬʱУ���ļ���
                  ������ִ����޷���Ӧ���з��أ�ֻ�ܵȵ�������ɺ��ϱ�����
                  ���
  Calls:
  Called By:
  Input:          g_astFilePartTbl: �ļ�������
                  g_stUpgradeCtx:   �������̵�������
                  
  Output:         
  Return:         
  Others:         FLASH_SECTOR_SIZE�϶������ݰ�����ulDataLen��������
*************************************************/
void EndFileTransmission()
{
    //����UPGRADE_COMPLETE,ABORT,CANCEL���״̬�½��յ�END�����Դ�����ǰ����״̬������̬
    //ֹͣ��ʱ��
    StopUpgradeTimer();

    //�������ع���ָʾ
    g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_END;

    //���ļ�βд��FLASH
    if(WriteFileBlockToFlash() != SUCCEEDED)
    {
        g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_ABORT;
        g_stUpgradeCtx.lUpgradeResult    = UPGRADE_SAVE_FILE_FAILED;
        return;
    }

    g_stUpgradeCtx.ulSavedFileSize += g_stUpgradeCtx.stFileBuf.ulUsedSize;

    //У���ļ�
    if(ValidateFile() != SUCCEEDED)
    {
        g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_ABORT;
        g_stUpgradeCtx.lUpgradeResult    = UPGRADE_VALID_FILE_FAILED;
        return;
    }

    //��ulNextBlockSn, ulSavedFileSize, lUpgradeResult���浽NV Memory��
    //SimpleFlashWrite(ADDR_UPGRADE_INFO_SET, &g_stUpgradeCtx, sizeof(UPGRADE_INFO_SET_ST));

    //������ʱ��
    StartUpgradeTimer(TIMER_TRANS_CTRL_ID, TIMER_TRANS_CTRL_INTV);
}

/*************************************************
  Function:
  Description:    ���������յ�OMCȡ������ʱ���ã���ȡ��ǰ�����صĲ���
                  ���浽NV Memory��ͬʱ���ؼ��ģʽ
  Calls:
  Called By:
  Input:          g_stUpgradeCtx: �������̵�������
  Output:         
  Return:         
  Others:         
*************************************************/
void CancelFileTransmission()
{
    //ֹͣ��ʱ��
    StopUpgradeTimer();
    
    //����״̬�ͽ����׼�����ؼ��ģʽ
    g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_CANCEL;
    g_stUpgradeCtx.lUpgradeResult    = UPGRADE_CANCELLED;

    //MCM-50_20070105_zhonghw_begin
    //�������ݰ�(��ӦOMC����ȡ�������)
    g_stUpgradeCtx.ulNextBlockSn -= g_stUpgradeCtx.ulUnSaveBlocks;
    g_stUpgradeCtx.ulUnSaveBlocks = 0;                        
    //MCM-50_20070105_zhonghw_end
    
    //��ulNextBlockSn, ulSavedFileSize, lUpgradeResult���浽NV Memory��
    //SimpleFlashWrite(ADDR_UPGRADE_INFO_SET, &g_stUpgradeCtx, sizeof(UPGRADE_INFO_SET_ST));
}

/*************************************************
  Function:
  Description:    ���������յ�����������־������
  Calls:
  Called By:
  Input:          g_stUpgradeCtx: �������̵�������
  Output:         
  Return:         
  Others:         
*************************************************/
void RecvUpgradeComplete()
{
    //ֹͣ��ʱ��
    StopUpgradeTimer();
    
    if(g_stUpgradeCtx.ulUpgradeProgress != TRANS_CTRL_END)
    {
        //����״̬���յ������ݰ����ж�
        g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_UPGRADE_COMPLETE;
        g_stUpgradeCtx.lUpgradeResult    = UPGRADE_ABORTED;

        //MCM-50_20070105_zhonghw_begin
        //�������ݰ�(��ӦOMC�����)
        g_stUpgradeCtx.ulNextBlockSn -= g_stUpgradeCtx.ulUnSaveBlocks;
        g_stUpgradeCtx.ulUnSaveBlocks = 0;                        
        //MCM-50_20070105_zhonghw_end         
        return;
    }

    g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_UPGRADE_COMPLETE;

    //��lUpgradeResult���浽NV Memory�У�����������ϱ����������Ӧ��֮��������㴦��
    //SimpleFlashWrite(ADDR_UPGRADE_INFO_SET, &g_stUpgradeCtx, sizeof(UPGRADE_INFO_SET_ST));
}

/*************************************************
  Function:
  Description:    ��������鲻ͬ���ȵ����ͼ�����ĳ����Ƿ���ȷ
  Calls:
  Called By:
  Input:          pstMObj:       ��ض���
                  ulExpectedLen: ����ĳ���
  Output:         ucFlag:        �ü�����Ƿ�������ڱ�ʶЭ�鴦���ִ���
  Return:         SUCCEEDED:     ����ɹ�
                  FAILED:        ����ʧ��
  Others:
*************************************************/
LONG McpBCheckInt(MCPB_MOBJ_ST *pstMObj, ULONG ulExpectedLen, UCHAR *ucFlag)
{
    if(pstMObj->usMObjLen != ulExpectedLen + 4) //��4��ʾ�����ֶκͶ����ʶ�ֶε��ܳ���
    {
        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_LEN_NOT_MATCH, *ucFlag);
        return FAILED;
    }
    return SUCCEEDED;
}

/*************************************************
  Function:
  Description:    �����������ļ������Ӧ���������Ӧ���־��SN
  Calls:
  Called By:
  Input:          pucData:   Ӧ�����ʼ��λ��
                  ucRspFlag: Ӧ���־
                  ulSn:      ��ǰ�ļ����ݿ��SN
  Output:         
  Return:         Ӧ����ĳ���
  Others:
*************************************************/
ULONG EncodeFileTransRsp(UCHAR *pucData, UCHAR ucRspFlag, ULONG ulSn)
{
    ULONG ulLen = 0;
    MCPB_MOBJ_ST *pstMObj = (MCPB_MOBJ_ST *)pucData;
    
    pstMObj->usMObjLen = MCPB_MOBJ_MIN_LEN + 1; //LT��ռ�����ֽ�
    pstMObj->aucMObjId[LO_BYTE] = MOBJ_ID_UF_TRANS_RSP_FLAG;
    pstMObj->aucMObjId[HI_BYTE] = MOBJ_ID_UPGRADE_FILE_INFO_SET;
    pstMObj->aucMObjContent[0]  = ucRspFlag; 
    
    ulLen += pstMObj->usMObjLen;
    pstMObj = (MCPB_MOBJ_ST *)(pucData + ulLen);

    pstMObj->usMObjLen = MCPB_MOBJ_MIN_LEN + 4; //LT��ռ�����ֽ�
    pstMObj->aucMObjId[LO_BYTE] = MOBJ_ID_UF_CURRENT_BLOCK_SN;
    pstMObj->aucMObjId[HI_BYTE] = MOBJ_ID_UPGRADE_FILE_INFO_SET;
    //*(ULONG *)pstMObj->aucMObjContent = ulSn;
    SET_DWORD(pstMObj->aucMObjContent, ulSn);
    ulLen += pstMObj->usMObjLen;

    return ulLen + 2;//20060421zhw
}

/*************************************************
  Function:
  Description:    ����������Լ�ض���Ĳ�ѯ����㺯����֤MAP�����ݵĳ��ȴ���3��
                  �������ʧ�ܻ���д������룬���еĴ�����ֱ���ڴ����������
                  ���е�
  Calls:
  Called By:
  Input:          pucMcpData:    MAP�����ݵ�Ԫָ��
                  pulMcpDataLen: MAP�����ݵ�Ԫ����
  Output:         pulMcpDataLen: ���ش�����MAP�����ݳ���
  Return:         SUCCEEDED��ʾ���Խ��к�������FAILED��ʾ���Ը����ݰ�
  Others:         ������ܵĳ��ȴ����򷵻�Ӧ���־ΪRSP_FLAG_LEN_ERR����ʱ����
                  ���ض������еĴ�����룻����Ǽ�ض���ĳ��ȴ�������д��
                  ����룬�ҷ���Ӧ���־ΪRSP_FLAG_PARTIALLY_DONE
*************************************************/
LONG HandleMcpB(UCHAR* pucMcpData, ULONG *pulMcpDataLen)
{
    MCP_LAYER_ST *pstMcp  = (MCP_LAYER_ST *)pucMcpData;
    LONG lMcpDataLen      = (LONG)*pulMcpDataLen;
    ULONG ulActualLen     = 0; //���ڷ��ش�����ʵ�ʳ���
    MCPB_MOBJ_ST *pstMObj = NULL;
    ULONG ulCurrentSn     = 0; //���ڼ�¼��ǰ�ļ����ݿ�SN
    BOOL IsSameFileID     = TRUE; //��FILE ID��ͬʱ���Է��ؼ�¼�Ķϵ���Ϣ

    if(pstMcp->ucRspFlag != RSP_FLAG_CMD)
    {
        return FAILED; //���Ӧ���־����ȷ��Ӧ��
    }

    if((pstMcp->ucCmdId != MCPB_CMD_ID_READ) && (pstMcp->ucCmdId != MCPB_CMD_ID_WRITE))
    {
        pstMcp->ucRspFlag = RSP_FLAG_CMD_ID_ERR;
        return SUCCEEDED;
    }

    pstMcp->ucRspFlag = RSP_FLAG_SUCCEEDED;

    pstMObj = (MCPB_MOBJ_ST *)(pstMcp->aucContent);
    lMcpDataLen -= 2; //�۳����Ԫ����
    ulActualLen += 2;
    while(lMcpDataLen >= MCPB_MOBJ_MIN_LEN) //L��T��ռ2�ֽ�
    {
        if(pstMObj->usMObjLen < MCPB_MOBJ_MIN_LEN) //��ض��󳤶Ȳ�����֮������ݾͲ��ٴ���
        {
            pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR;
            return SUCCEEDED;
        }

        if(pstMObj->usMObjLen > lMcpDataLen) //����ʵ�����ݳ���
        {
            pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR;
            return SUCCEEDED;
        }

        switch(pstMObj->aucMObjId[HI_BYTE] & 0xF) //���ֽڱ�ʾ�������ֻ࣬ȡ����λ
        {
        case MOBJ_ID_UPGRADE_INFO_SET: //���������֧�Ķ����ɶ���������
            switch(pstMObj->aucMObjId[LO_BYTE])
            {
            case MOBJ_ID_UI_RUNNING_MODE: //�ɶ�������д
                if(McpBCheckInt(pstMObj, UINT_1, &pstMcp->ucRspFlag) == SUCCEEDED)
                {
                    pstMObj->aucMObjContent[0] = g_stDevInfoSet.ucRunningMode;
                }
                break;
            case MOBJ_ID_UI_NEXT_BLOCK_SN: //�ɶ�������д
                if(McpBCheckInt(pstMObj, UINT_4, &pstMcp->ucRspFlag) == SUCCEEDED)
                {
                    if(IsSameFileID)
                    {
                        //*(ULONG *)(pstMObj->aucMObjContent) = ADJUST_DWORD(g_stUpgradeCtx.ulNextBlockSn);
                        SET_DWORD(pstMObj->aucMObjContent, g_stUpgradeCtx.ulNextBlockSn);
                    }
                    else
                    {
                        //*(ULONG *)(pstMObj->aucMObjContent) = ADJUST_DWORD(1); //����ļ�ID��ͬ������Ҫ���·���
                        SET_DWORD(pstMObj->aucMObjContent, 0);
                    }
                }
                break;
            case MOBJ_ID_UI_MAX_BLOCK_SIZE: //�ɶ�������д
                if(McpBCheckInt(pstMObj, UINT_2, &pstMcp->ucRspFlag) == SUCCEEDED)
                {
                    //*(USHORT *)(pstMObj->aucMObjContent) = (USHORT)g_stUpgradeCtx.ulMaxBlockSize;
                    SET_WORD(pstMObj->aucMObjContent, (USHORT)g_stUpgradeCtx.ulMaxBlockSize);
                }

                //���ڻ�ȡ�ϵ���Ϣ�̶�����0x0301,0x0202,0x0203������ڴ�������ʱ��
                StartUpgradeTimer(TIMER_TRANS_CTRL_ID, TIMER_TRANS_CTRL_INTV);
                break;
            default:
                SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                break;
            }
            break;
        case MOBJ_ID_UPGRADE_FILE_INFO_SET:
            switch(pstMObj->aucMObjId[LO_BYTE])
            {
            case MOBJ_ID_UF_FILE_ID: //�ɶ���д����������ӿڹ淶�б�9-55�й涨��д����
                if(McpBCheckInt(pstMObj, STR_20, &pstMcp->ucRspFlag) == SUCCEEDED)
                {
                    if(pstMcp->ucCmdId == MCPB_CMD_ID_READ)
                    {
                        memcpy(pstMObj->aucMObjContent, 
                               g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].aucFileId,
                               STR_20);
                    }
                    else
                    {
                        //�Ƚ��Ƿ�ͱ������ͬ
                        for(LONG i = 0; i < STR_20; i++)
                        {
                            if(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].aucFileId[i]
                            != pstMObj->aucMObjContent[i])
                            {
                                IsSameFileID = FALSE;

                                //���µ��ļ�IDд��
                                memcpy(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].aucFileId,
                                       pstMObj->aucMObjContent, STR_20);
                                break;
                            }
                        }

                        //IsSameFileID = TRUE;
                    }
                }
                break;
            case MOBJ_ID_UF_TRANS_CTRL_BYTE: //��������д�����У���û�����壬��˲���Ҫ����
                if(McpBCheckInt(pstMObj, UINT_1, &pstMcp->ucRspFlag) == SUCCEEDED)
                {
                    switch(pstMObj->aucMObjContent[0])//20060417zhw
                    {
                    case TRANS_CTRL_START: //�ļ����俪ʼ
                        StartFileTransmission();
                        break;
                    case TRANS_CTRL_END: //�ļ��������
                        EndFileTransmission();
                        break;
                    case TRANS_CTRL_CANCEL: //ȡ���ļ����䣬��Ҫ���ؼ��ģʽ
                        CancelFileTransmission();
                        break;
                    case TRANS_CTRL_UPGRADE_COMPLETE: //������ɣ����ԣ����������ؼ��ģʽ���ϱ��������
                        RecvUpgradeComplete();
                        break;
                    default:
                        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_VALUE_OUT_OF_BOUND, pstMcp->ucRspFlag);
                        break;
                    }
                }
                break;
            case MOBJ_ID_UF_TRANS_RSP_FLAG: //������д�����У�����λ����д����û�����壬��˲���Ҫ����
                McpBCheckInt(pstMObj, UINT_1, &pstMcp->ucRspFlag);                
                break;
            case MOBJ_ID_UF_CURRENT_BLOCK_SN: //������д�����У���û�����壬��˲���Ҫ����
                if(McpBCheckInt(pstMObj, UINT_4, &pstMcp->ucRspFlag) == SUCCEEDED)
                {
                    //ulCurrentSn = ADJUST_DWORD(*(ULONG *)(pstMObj->aucMObjContent));
                    ulCurrentSn = GET_DWORD(pstMObj->aucMObjContent);
                    
                    //�ж��Ƿ����һ���������ͬ�������Ƿ��ǵ�һ��
                    if((ulCurrentSn != 0) && (ulCurrentSn != g_stUpgradeCtx.ulNextBlockSn))
                    {
                        //�����к�������ֱ�ӷ���Ӧ���
                        *pulMcpDataLen = EncodeFileTransRsp(pstMcp->aucContent, 
                                                            TRANS_RSP_CANCEL_REQ, //ȡ����������
                                                            ulCurrentSn);
                        //MCM-50_20070105_zhonghw_begin
                        //�������ݰ�(��ӦOMC����)
                        g_stUpgradeCtx.ulNextBlockSn -= g_stUpgradeCtx.ulUnSaveBlocks;
                        g_stUpgradeCtx.ulUnSaveBlocks = 0;                        
                        //MCM-50_20070105_zhonghw_end                        

                        return SUCCEEDED;
                    }
                }               
                break;
            case MOBJ_ID_UF_FILE_BLOCK: //������д�����У���û�����壬��˲���Ҫ����
                if(pstMObj->usMObjLen <= MCPB_MOBJ_MIN_LEN + g_stUpgradeCtx.ulMaxBlockSize) //LT��ռ�����ֽ�
                {
                    UCHAR ucRspFlag = (UCHAR)RecvFileBlock(pstMObj->aucMObjContent, pstMObj->usMObjLen - MCPB_MOBJ_MIN_LEN, ulCurrentSn);
                    *pulMcpDataLen = EncodeFileTransRsp(pstMcp->aucContent, ucRspFlag, ulCurrentSn);
                    return SUCCEEDED;
                }
                else
                {
                    SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_LEN_NOT_MATCH, pstMcp->ucRspFlag);
                }
                
                break;
            default:
                SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                break;
            }
            break;
        default: //ϵͳ�����򳧼��Զ���
            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        }

        //������һ����ض���
        lMcpDataLen -= pstMObj->usMObjLen;
        ulActualLen  += pstMObj->usMObjLen;
        pstMObj = (MCPB_MOBJ_ST *)(((UCHAR *)pstMObj) + pstMObj->usMObjLen);     
    }   

    *pulMcpDataLen = ulActualLen;

    //��Ҫ������ʱ�����ȴ���һ�����ݰ��ĵ�����Ŀǰ��ʱ�����ڴ�������ƺ����ݿ�ĺ���������

    //���������浽NV Memory��
    SaveUpgradeInfo();
    return SUCCEEDED;
}

/*************************************************
  Function:       
  Description:    ��������������ʼ������־�жϺ�����������ݰ�������
                  ��ʼ������־
  Calls:          
  Called By:      
  Input:          pucApData:  ����ָ�룬ָ����ʼ��־
                  pulDataLen: ���ݳ���
                  ulDataLenMax: ���ݵ���󳤶ȣ������������ڴ��С����
                  ucCommMode: ͨ��ģʽ��Ŀǰ��ʱ����
  Output:         pulDataLen: �����º�����ݳ��ȴ���
                  pucDir:     �������ݵķ��򣬱��������
  Return:         FORWARD_UNDESTATE��ʾת��
                  SUCCEEDED_DUBUSTATE��ʾ����ɹ�
                  FAILED��ʾ����ʧ��
                  NEED_MORE_OP��ʾ��Ҫ���к�������
  Others:         
*************************************************/
LONG HandleCMCCProtoData(UCHAR *pucApData, ULONG *pulDataLen, ULONG ulDataLenMax, UCHAR ucCommMode, UCHAR *pucDir)
{
    LONG lApType           = FAILED;
    LONG lResult           = SUCCEEDED;
    ULONG ulDataLen        = *pulDataLen;
    ULONG ulRawApDataLen   = 0;
    UCHAR *pucVpData       = NULL;
    ULONG ulVpDataLen      = 0;
    UCHAR *pucMcpData      = NULL;
    ULONG ulMcpDataLen     = 0;
    VPA_LAYER_ST *pstVpData= NULL;
    
    //AP�����ݽ���
    lApType = DecodeApData(pucApData + 1, ulDataLen - 2, //������ʼ��־���۳���ʼ������־�ĳ���
                           &ulRawApDataLen, &pucVpData, &ulVpDataLen);
    if(lApType == FAILED)
    {
        return FAILED;
    }

    //VP�����ݽ���
    lResult = DecodeVpData(pucVpData, ulVpDataLen, &pucMcpData, &ulMcpDataLen);
    if(lResult == NEED_TO_FORWARD) //��Ҫ������ӿ�ת��
    {
        ulDataLen = EncodeApData(pucApData + 1, ulRawApDataLen - CRC_LEN, 
                                 ((AP_LAYER_ST *)(pucApData + 1))->ucApType, ulDataLenMax);
        if(ulDataLen == 0)
        {
            return FAILED;
        }
        else
        {
            *pulDataLen = ulDataLen;
            *pucDir = COMM_IF_SOUTH;
            return FORWARD_UNDESTATE;     //����ģʽ��ʾδ������δ������
                                          //UNDESTATE������FAILEDֵ��ͬ
                                          //���ƶ�ģʽ�±�ʾ��Ҫת��
                                          //�ڵ���ģʽ�±�ʾδ���Է���
        }
    }
    else if(lResult == IGNORE)
    {
        return FAILED;
    }

    //����MCP������
    pstVpData = (VPA_LAYER_ST *)pucVpData;
    if(g_stDevInfoSet.ucRunningMode == RUNNING_MODE_MONITOR)
    {
        if(pstVpData->ucApId == MCP_A)
        {
            if(HandleMcpA(pucMcpData, &ulMcpDataLen, (UCHAR)lApType, pstVpData->usPacketID) == IGNORE)
            {
                return FAILED;
            }
            
            //�ж��Ƿ���Ҫ�����첽����
            if((g_stTDParam.usParamLen != 0) || (g_stSECParam.usParamLen != 0))
            {
                UCHAR ucIdx = 0;
                if(GetAsynOpCtx(ucIdx) == SUCCEEDED) //�������������ֱ���������ʧ�ܵ�Ӧ��
                {
                    //Ԥռ������
                    g_astAsynOpCtx[ucIdx].ucState = OH_SEND_DATA_PENDING;

                    //ע���첽�����Ĵ������ͱ���
                    if(g_stTDParam.usParamLen != 0)
                    {
                        g_astAsynOpCtx[ucIdx].astOp[g_stTDParam.ucOpObj - 1].pfOp = DoAsynOpTD;
                        g_astAsynOpCtx[ucIdx].astOp[g_stTDParam.ucOpObj - 1].pvCtx= &g_astAsynOpCtx[ucIdx];
                        g_astAsynOpCtx[ucIdx].pfTo = AsynOpToCommon;
                    }

                    if(g_stSECParam.usParamLen != 0)
                    {
                        g_astAsynOpCtx[ucIdx].astOp[g_stSECParam.ucOpObj - 1].pfOp = DoAsynOpSEC;
                        g_astAsynOpCtx[ucIdx].astOp[g_stSECParam.ucOpObj - 1].pvCtx= &g_astAsynOpCtx[ucIdx];
                        g_astAsynOpCtx[ucIdx].pfTo = AsynOpToCommon;
                    }

                    *pulDataLen = ulRawApDataLen + 2; //AP�����ݳ��ȼ�����ʼ������־
                    return NEED_MORE_OP; //��Ҫ�����첽��������ʱ��Ӧ��
                }
            }
        }
        else
        {
            return FAILED; //���Բ�����
        }
    }    
    else
    {
        if(pstVpData->ucApId == MCP_B)
        {
            if(HandleMcpB(pucMcpData, &ulMcpDataLen) == FAILED)
            {
                return FAILED;
            }
        }
        else
        {
            return FAILED; //���Բ�����
        }
    }
    
    //VP�����ݱ��룬��Ҫ�������վ���ź��豸��ŵ��������Ҫ������Ӧ�ֶ�
    pstVpData->ulStaNum = g_stNmParamSet.ulStaNum;
    pstVpData->ucDevNum = g_stNmParamSet.ucDevNum;

    //AP�����ݱ���
    ulRawApDataLen = ulMcpDataLen + AP_OVERHEAD + VP_OVERHEAD - CRC_LEN; //4��9�ֱ���AP��NP��Ŀ�����Ҫ�۳�CRC����
    
    //pucDataָ����ʼ��־
    ulDataLen = EncodeApData(pucApData + 1, ulRawApDataLen, 
                             ((AP_LAYER_ST *)(pucApData + 1))->ucApType, ulDataLenMax);

    if(ulDataLen == 0)
    {
        return FAILED;
    }

    //����ӿڷ���Э������
    *pulDataLen = ulDataLen;
    *pucDir = COMM_IF_NORTH;

     return SUCCEEDED_DEBUSTATE;          //����ģʽ��ʾδ�������ѵ�����
                                          //UNDESTATE������FAILED��FORWARD_UNDESTATEֵ��ͬ
                                          //���ƶ�ģʽ�±�ʾ�ɹ�����
                                          //�ڵ���ģʽ�±�ʾ�ѵ�����
}

//�������Ƚ������绰���룬�Ƚ�ʱҪȥ���绰�����ǰ׺��SUCCEEDED��ʾ�Ϸ��������ʾ���Ϸ�
LONG ValidateTelNum(CHAR *pcTelNum1, CHAR *pcTelNum2)
{
    ULONG ulLen1,ulLen2,ulLen;
    ulLen1 = FindChar((UCHAR*)pcTelNum1, 0, 1, TEL_NUM_LEN);
    ulLen2 = FindChar((UCHAR*)pcTelNum2, 0, 1, TEL_NUM_LEN);
    
    if((ulLen1 == FIND_NO_CHAR) || (ulLen2 == FIND_NO_CHAR))
    {
        return FAILED;
    }

    if((ulLen1 == 0) || (ulLen2 == 0))
    {
        return FAILED;
    }
    //ȡ���Ƚ�С��
    if(ulLen1 >= ulLen2)
    {
        ulLen = ulLen2;
    }
    else
    {
        ulLen = ulLen1;
    }
    //�Ӻ���ǰ�Ƚ�
    ULONG i1 = ulLen1;
    ULONG i2 = ulLen2;
    for(ULONG i = ulLen ; i > 0; i--,i1--,i2--)
    {
        if(pcTelNum1[i1-1] != pcTelNum2[i2-1])
        {
            return FAILED;
        }
    }
    
    return SUCCEEDED;
}
/*************************************************
  Function:       
  Description:    �����������CH���յ����ѵ�����
  Calls:          
  Called By:      
  Input:          pstRecv: �������ݵ�ԭ��
  Output:         
  Return:         SUCCEEDED��ʾ�ɹ���FAILED��ʾʧ��
  Others:         ���������ⲿ����
*************************************************/
LONG HndlOHCHDebgDataReq(OHCH_RECV_DATA_IND_ST *pstRecv)
{
    OHCH_SEND_DATA_REQ_ST *pstSend = (OHCH_SEND_DATA_REQ_ST *)pstRecv;
    OHCH_SEND_DATA_REQ_ST *pstSendScnd;
    if(g_stYkppParamSet.stYkppCtrlParam.ucDebugMode == TRUE)      //����ģʽ
    {
        pstSend->ucDstIf = COMM_IF_DEBUG;
        if(OSQPost(g_pstCHDebQue, pstSend)!=OS_NO_ERR)
        {
            MyFree((void*)pstSend);  //�ͷ��ڴ�
            return FAILED;
        }
        return SUCCEEDED;
    }
    else
    {
        if(pstRecv->ucSrcIf == COMM_IF_NORTH)
        {
            pstSend->ucDstIf = COMM_IF_SOUTH; 
            if(OSQPost(g_pstCHSthQue, pstSend)!=OS_NO_ERR)
            {
                MyFree((void*)pstSend);
                return FAILED;
            }
            return SUCCEEDED;
        }
        else      //��������ӿ�
        {
            //�ȿ�������
            pstSendScnd = (OHCH_SEND_DATA_REQ_ST *)MyMalloc(g_pstMemPool256);
            if(pstSendScnd != NULL)
            {
                memcpy(pstSendScnd,pstSend,sizeof(*pstSend)+pstSend->ulDataLen);
            }
            
            pstSend->ucDstIf = COMM_IF_SOUTH;         
            if(OSQPost(g_pstCHSthQue, pstSend)!=OS_NO_ERR)
            {
                MyFree((void*)pstSend); 
            }
            //��������ӿ�            
            if(pstSendScnd != NULL)
            {
                pstSendScnd->ucDstIf = COMM_IF_NORTH;         
                if(OSQPost(g_pstCHNthQue, pstSendScnd)!=OS_NO_ERR)
                {
                    MyFree((void*)pstSendScnd); //д����ʧ���ͷ���������ڴ�
                    return FAILED;
                }             
            }             
            return SUCCEEDED;
        }
    }
}

/*************************************************
  Function:       
  Description:    �����������CH���յ���δ������
  Calls:          
  Called By:      
  Input:          pstRecv: �������ݵ�ԭ��
  Output:         
  Return:         SUCCEEDED��ʾ�ɹ���FAILED��ʾʧ��
  Others:         ���������ⲿ����
*************************************************/
LONG HndlOHCHDebgDataInd(OHCH_RECV_DATA_IND_ST *pstRecv)
{
    UCHAR ucDir = 0;
    UCHAR ucSrcIf = pstRecv->ucSrcIf;  //��¼������Դͨ�ſ�
    OHCH_SEND_DATA_REQ_ST *pstSend = (OHCH_SEND_DATA_REQ_ST *)pstRecv;
    OHCH_SEND_DATA_REQ_ST *pstSendScnd; //��Ҫ������������ʱcopy����
    UCHAR *pucData = NULL;
    ULONG ulDataLen= 0;
    OS_EVENT * pstQueue =NULL;
    UCHAR SendIsBoth = FALSE;            //true(1)Ҫ�����ϱ�˫��    
    LONG lResult = 0;

    //����ڵ���ģʽ���յ����Էǵ��Դ��ڵ�δ����������
    if((pstRecv->ulPrmvType == OHCH_UNDE_RECV_IND)&&(g_stYkppParamSet.stYkppCtrlParam.ucDebugMode == TRUE)
    &&(pstRecv->ucSrcIf !=COMM_IF_DEBUG))
    {
        return FAILED;
    }

    UCHAR *pucCmccData = NULL;
    ULONG ulCmccDataLen= 0;
    UCHAR *pucYkppData = NULL;
    ULONG ulYkppDataLen= 0;  

    //�ж��Ƿ����ƶ�Э��
    ExtractCMCCProtoData(pstRecv->pucData, pstRecv->ulDataLen, &pucCmccData, &ulCmccDataLen);

    //�ж��Ƿ��г���Э��
    ExtractYkppData(pstRecv->pucData, pstRecv->ulDataLen, pucYkppData, ulYkppDataLen);
    
    if((ulCmccDataLen > 0) && (ulYkppDataLen > 0)) //���ͬʱ��������Э������
    {
        if(pucCmccData < pucYkppData) //����ƶ�Э��������ǰ������Ϊ�ƶ�Э��������Ч
        {
            ulYkppDataLen = 0;
            
        }
        else //����ƶ�Э�������ں�����Ϊ�ʿƳ���Э��������Ч
        {
            ulCmccDataLen = 0;
        }
    }
    
    if(ulCmccDataLen > 0) //�����ƶ�Э������
    {
        pucData = pucCmccData;
        ulDataLen = ulCmccDataLen;        
        lResult = HandleCMCCProtoData(pucCmccData, &ulDataLen, pstRecv->ulDataLenMax, pstRecv->ucCommMode, &ucDir);
    }
    
    else if(ulYkppDataLen > 0)//������Э������
    {
        pucData = pucYkppData;
        ulDataLen = ulYkppDataLen;
        lResult = HandleYkppData(pucYkppData, ulDataLen, pstRecv->ucCommMode, ucDir);        
    }
    
    else
    {
        return FAILED;
    }

    //pstSend->ulPrmvType = OHCH_SEND_DATA_REQ;   //����lResult������ֵͬ
    pstSend->ulMagicNum = 0;
    pstSend->ucResendTime = RESEND_TIME_DATATYPE_RSP;
    //pstSend->ucDstIf    = ucDir;                //����lResult������ֵͬ
    //pstSend->ucCommMode; //����
    //pstSend->ucDCS;      //����
    //pstSend->acDstTelNum;//���䣬ԭ��SrcTelNum��λ��
    pstSend->ucDataType = DATA_TYPE_RSP;
    pstSend->ulDataLen  = ulDataLen;
    pstSend->pucData    = pucData;

//    ucSendBoth = HandleDebugSendParam(lResult, ucSrcIf,pstSend,&g_pstQue);
    if(lResult == FORWARD_UNDESTATE)              //UNDESTATE��ʾδ��
    {
        pstSend->ulPrmvType = OHCH_UNDE_SEND_REQ; 
        pstSend->ucDstIf = COMM_IF_SOUTH;//�����δ�����ݣ��������ǹ���
        pstQueue = g_pstCHSthQue;
        
        if(g_stYkppParamSet.stYkppCtrlParam.ucDebugMode != TRUE)
        {
            if(ucSrcIf == COMM_IF_SOUTH)  
            {
                SendIsBoth = TRUE;
            }
        }
        else//����ģʽ
        {
            SendIsBoth = TRUE;
        }
    }
    else if(lResult == SUCCEEDED_DEBUSTATE) //��ʾ�����ѵ�
    {
        //����ԭͨ�ſ�
        pstSend->ulPrmvType = OHCH_DEBU_SEND_REQ; 
        pstSend->ucDstIf    = ucSrcIf;  
        if(ucSrcIf == COMM_IF_SOUTH)
        {
            pstQueue = g_pstCHSthQue;
        }
        else if(ucSrcIf == COMM_IF_NORTH)
        {
            pstQueue = g_pstCHNthQue;
        }
        else
        {
            pstQueue = g_pstCHDebQue;
        }
    }

    //��Ϊ�����Ѿ�ȷ����ԭ�����ͼ������������ԭ�ӿ�
    //���Դ˺����ֱ�ӽ���д���в���
    //����жϵ����ϱ���ı�־�������µ��ڴ沢ִ�з�������Ĳ���
    
    if((lResult == FORWARD_UNDESTATE)&&(SendIsBoth == TRUE))//ȷ��δ��״̬�ŷ����ϱ�˫��
    {   
        //�������ڴ�
        pstSendScnd = (OHCH_SEND_DATA_REQ_ST *)MyMalloc(g_pstMemPool256);
        if(pstSendScnd != NULL)
        {
            //����ԭ�Ｐ���ݰ���������ڴ�
            memcpy(pstSendScnd,pstSend,sizeof(*pstSend)+pstSend->ulDataLen);
            //��������ӿ�
            pstSendScnd->ucDstIf = COMM_IF_NORTH;
            if(OSQPost(g_pstCHNthQue, pstSendScnd) != OS_NO_ERR)
            {
                MyFree((void*)pstSendScnd); //�ͷ���������ڴ�
            }
        }
    }
    //ֱ�ӷ���
    if(OSQPost(pstQueue, pstSend) != OS_NO_ERR)
    {
        MyFree((void*)pstSend); //д������в��ɹ�ֱ���ͷ��ڴ�
        return FAILED;
    }
    return SUCCEEDED;  
}

/*************************************************
  Function:       
  Description:    �����������CH���յ�������
  Calls:          
  Called By:      
  Input:          pstRecv: �������ݵ�ԭ��
  Output:         
  Return:         SUCCEEDED��ʾ�ɹ���FAILED��ʾʧ�ܣ�NEED_MORE_OP��ʾ��Ҫ��������
  Others:         ���������ⲿ����
*************************************************/
LONG HndlOHCHRecvDataInd(OHCH_RECV_DATA_IND_ST * &pstRecv)
{
    UCHAR ucDir = 0;
    UCHAR *pucData = NULL;
    ULONG ulDataLen= 0;
    LONG lResult = 0;

    //Ϊ�˷�ֹ�ڲ�ѯ��ز����б�ʱCH������ڴ治�������ͳһ�ڴ������������ڴ��
    if((pstRecv->ucCommMode != COMM_MODE_SMS) && (pstRecv->ulDataLenMax < MEM_BLK_SIZE_256))
    {
        //�����ڴ�
        OHCH_RECV_DATA_IND_ST *pstBuf = (OHCH_RECV_DATA_IND_ST *)MyMalloc(g_pstMemPool512);
        if(pstBuf != NULL)
        {
            memcpy(pstBuf, pstRecv, sizeof(OHCH_RECV_DATA_IND_ST));
            pstBuf->ulDataLenMax = MEM_BLK_SIZE_512 - 4 - sizeof(OHCH_RECV_DATA_IND_ST);
            pstBuf->pucData = (UCHAR *)pstBuf + sizeof(OHCH_RECV_DATA_IND_ST);
            memcpy(pstBuf->pucData, pstRecv->pucData, pstRecv->ulDataLen);

            MyFree(pstRecv);
            pstRecv = pstBuf;
        } //����ڴ�����ʧ�����������ִ��
    }

    OHCH_SEND_DATA_REQ_ST *pstSend = (OHCH_SEND_DATA_REQ_ST *)pstRecv;

    UCHAR *pucCmccData = NULL;
    ULONG ulCmccDataLen= 0;
    UCHAR *pucYkppData = NULL;
    ULONG ulYkppDataLen= 0;

    //�ж��Ƿ����ƶ�Э��
    ExtractCMCCProtoData(pstRecv->pucData, pstRecv->ulDataLen, &pucCmccData, &ulCmccDataLen);

    //�ж��Ƿ��г���Э��
    ExtractYkppData(pstRecv->pucData, pstRecv->ulDataLen, pucYkppData, ulYkppDataLen);

    if((ulCmccDataLen > 0) && (ulYkppDataLen > 0)) //���ͬʱ��������Э������
    {
        if(pucCmccData < pucYkppData) //����ƶ�Э��������ǰ������Ϊ�ƶ�Э��������Ч
        {
            ulYkppDataLen = 0;
        }
        else //����ƶ�Э�������ں�����Ϊ�ʿƳ���Э��������Ч
        {
            ulCmccDataLen = 0;
        }
    }
    
    if(ulCmccDataLen > 0) //�����ƶ�Э������
    {
        pucData = pucCmccData;
        ulDataLen = ulCmccDataLen;

        //����Ƕ���Ϣ���жϵ绰���룬CSD�ĺ��������CH��������ʣ���������ʱ�ж�
        if(pstRecv->ucCommMode == COMM_MODE_SMS)
        {
            if(g_stYkppParamSet.stYkppCtrlParam.ucTelNumAuthSw != 0) //�ж��Ƿ���е绰�����Ȩ
            {
                if((ValidateTelNum(pstRecv->acSrcTelNum, g_stNmParamSet.acQnsTelNum1) != SUCCEEDED)
                && (ValidateTelNum(pstRecv->acSrcTelNum, g_stNmParamSet.acQnsTelNum2) != SUCCEEDED)
                && (ValidateTelNum(pstRecv->acSrcTelNum, g_stNmParamSet.acQnsTelNum3) != SUCCEEDED)
                && (ValidateTelNum(pstRecv->acSrcTelNum, g_stNmParamSet.acQnsTelNum4) != SUCCEEDED)
                && (ValidateTelNum(pstRecv->acSrcTelNum, g_stNmParamSet.acQnsTelNum5) != SUCCEEDED))
                {
                    return FAILED;
                }
            }
        }    

        //����������ֵ�Ƿ��Ҫ
        //pstRecv->pucData   = pucCmccData;   //����ԭ���е�����ָ�룬ʹ��ָ����ʼ��־
        //pstRecv->ulDataLen = ulDataLen;     //����ԭ���е����ݳ���Ϊ������ʼ������־��δ����AP���������ݳ���
    
        //�����ƶ�Э������
        lResult = HandleCMCCProtoData(pucCmccData, &ulDataLen, pstRecv->ulDataLenMax, pstRecv->ucCommMode, &ucDir);

        //�ڵ�¼�ɹ���ֻҪ�յ����ݾ������������ڶ�ʱ����ͬʱ��������ʱ��������
        if(g_stCenterConnStatus.ucLoginStatus == COMM_STATUS_LOGIN)
        {
            RestartHeartbeatPeriodTimer();

            g_stCenterConnStatus.ucNoRspCnt = 0;
        }
    }
    else if(ulYkppDataLen > 0) //������Э������
    {
        pucData = pucYkppData;
        ulDataLen = ulYkppDataLen;
        lResult = HandleYkppData(pucYkppData, ulDataLen, pstRecv->ucCommMode, ucDir);
    }
    else
    {
        return FAILED;
    }
    
    if(lResult == NEED_MORE_OP)
    {
        pstRecv->pucData   = pucData;
        pstRecv->ulDataLen = ulDataLen; //�������ݳ���
        return lResult;
    }
    else if(lResult == FAILED)
    {
        return lResult;
    }

    pstSend->ulPrmvType = OHCH_SEND_DATA_REQ;
    pstSend->ulMagicNum = 0;
    pstSend->ucResendTime = RESEND_TIME_DATATYPE_RSP;
    pstSend->ucDstIf    = ucDir;
    //pstSend->ucCommMode; //����
    //pstSend->ucDCS;      //����
    //pstSend->acDstTelNum;//���䣬ԭ��SrcTelNum��λ��
    pstSend->ucDataType = DATA_TYPE_RSP;
    pstSend->ulDataLen  = ulDataLen;
    pstSend->pucData    = pucData;

    //��ԭ�﷢�͸�CH
    if(ucDir == COMM_IF_NORTH)
    {
        if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
        {
            MyFree((void*)pstSend);             
            return FAILED;
        }
    }
    else
    {
        if(OSQPost(g_pstCHSthQue, pstSend) != OS_NO_ERR)
        {
            MyFree((void*)pstSend);         
            return FAILED;
        }
    }
    
    return SUCCEEDED;
}


/*************************************************
  Function:
  Description:    ���������ϱ����б��룬����ֻ���뵽���ϱ����͡���ض��������
                  �澯�������ϱ�����Ҫ������������ز������б���
  Calls:
  Called By:
  Input:          pucBuf:   ��ű�����ڴ棬�����ɵ��÷���֤
                  event:    �ϱ��¼�
                  commMode: ͨ�ŷ�ʽ����������ʹ�ú���APЭ��
                  PID:      ͨ�Ű���ʶ
  Output:         pucBuf:   �����Ľ��
  Return:         ������AP������ݳ���
  Others:
*************************************************/
ULONG EncodeReportIncludeReportTypeOnly(UCHAR *pucBuf, UCHAR ucReportType, 
                                        UCHAR ucCommMode, USHORT usPid)
{
    VPA_LAYER_ST* pstVpData  = NULL;
    MCP_LAYER_ST* pstMcpData = NULL;

    //AP�����
    if(ucCommMode == COMM_MODE_SMS)
    {
        ((AP_LAYER_ST*)pucBuf)->ucApType = AP_B;
    }
    else if(ucCommMode == COMM_MODE_CSD)
    {
        ((AP_LAYER_ST*)pucBuf)->ucApType = AP_A;
    }
    else //gprs
    {
        ((AP_LAYER_ST*)pucBuf)->ucApType = AP_C;
    }

    ((AP_LAYER_ST*)pucBuf)->ucPduType = VP_A;

    //VP�����
    pstVpData = (VPA_LAYER_ST*)(((AP_LAYER_ST *)pucBuf)->aucPdu);

    //1.վ���ź��豸���
    pstVpData->ulStaNum = g_stNmParamSet.ulStaNum;
    pstVpData->ucDevNum = g_stNmParamSet.ucDevNum;

    //2.ͨ�Ű����
    pstVpData->usPacketID = ADJUST_WORD(usPid);

    //3.VP�㽻����־
    pstVpData->ucVpFlag = VP_FLAG_COMMAND;

    //4.Ӧ��Э���ʶ
    pstVpData->ucApId = MCP_A;

    //MAP�����
    pstMcpData = (MCP_LAYER_ST*)pstVpData->aucPdu;
    pstMcpData->ucCmdId       = MCPA_CMD_ID_REPORT;
    pstMcpData->ucRspFlag     = RSP_FLAG_CMD;
    pstMcpData->aucContent[0] = 4;                      //���󳤶ȣ��̶�Ϊ4
    pstMcpData->aucContent[1] = MOBJ_ID_NM_REPORT_TYPE; //�����ʶ���̶�
    pstMcpData->aucContent[2] = MOBJ_ID_NM_PARAM_SET;
    pstMcpData->aucContent[3] = ucReportType;           //�������ݣ��ϱ�����

    return 17; //�ϱ������ݳ���Ϊ17����ȥ��ʼ������־�͸澯�CRCУ�鵥Ԫ��
}

/*************************************************
  Function:
  Description:    ���������ϱ�����Ļ������������������
                  �ļ��汾����������
  Calls:
  Called By:
  Input:          pucData:  ��ʼ����ĵط�
                  ucResult: �����Ľ��
                  g_stDevInfoSet.acSfwrVer: ��ǰ����汾��
  Output:         pucData:  ����Ľ��
  Return:         ����ĳ���
                  
  Others:
*************************************************/
ULONG EncodeUpgradeResult(UCHAR *pucData, UCHAR ucResult)
{
    ULONG ulLen = 0;
    MCPA_MOBJ_ST *pstMObj = (MCPA_MOBJ_ST *)pucData;
    
    pstMObj->ucMObjLen = MCPA_MOBJ_MIN_LEN + sizeof(g_stDevInfoSet.acSfwrVer); //LT��ռ�����ֽ�
    pstMObj->aucMObjId[LO_BYTE] = MOBJ_ID_DI_SFWR_VER;
    pstMObj->aucMObjId[HI_BYTE] = MOBJ_ID_DEV_INFO_SET;
    memcpy(pstMObj->aucMObjContent, g_stDevInfoSet.acSfwrVer, sizeof(g_stDevInfoSet.acSfwrVer)); 
    
    ulLen += pstMObj->ucMObjLen;
    pstMObj = (MCPA_MOBJ_ST *)(pucData + ulLen);

    pstMObj->ucMObjLen = MCPA_MOBJ_MIN_LEN + 1; //LT��ռ�����ֽ�
    pstMObj->aucMObjId[LO_BYTE] = MOBJ_ID_DI_UPGRADE_RESULT;
    pstMObj->aucMObjId[HI_BYTE] = MOBJ_ID_DEV_INFO_SET;
    pstMObj->aucMObjContent[0]  = ucResult;
    ulLen += pstMObj->ucMObjLen;
    
    return ulLen;
}

/*************************************************
  Function:
  Description:    ���������ϱ�֮ǰ���ã���鵱ǰ�Ƿ�����ϱ�
                  ��һ������
  Calls:
  Called By:
  Input:          ucEvent:                        �ϱ����¼�
                  g_ucCurrCommMode:               ��ǰͨ�ŷ�ʽ
                  g_stDevInfoSet.ucRunningMode:   ����ģʽ
                  g_stNmParamSet.ucCommMode:      ͨ�ŷ�ʽ
  Output:
  Return:         TRUE��ʾ�����ϱ���FALSE��ʾ�����ϱ�         
  Others:         ���������ⲿ����
*************************************************/
LONG IsReportAllowed(UCHAR ucEvent)
{
    //��������ģʽ���򲻴���
    if(g_stDevInfoSet.ucRunningMode != RUNNING_MODE_MONITOR)
    {
        return FALSE;
    }

    if(g_ucCurrCommMode == COMM_NO_CONN)
    {
        return TRUE; //�������ֱ����ʽ�����������ϱ�
    }

    if(g_ucCurrCommMode != g_stNmParamSet.ucCommMode)
    {
        return FALSE; //����ϱ�ͨ�ŷ�ʽ��Ŀǰ��������ͨ��״̬��һ�£����ϱ�
    }
    
    //GPRS��ʽ�µ��ϱ����˵�¼�Ϳ�վ�ϱ����ⶼҪ�ڵ�¼��ɺ����
    switch(g_stNmParamSet.ucCommMode)
    {
    case COMM_MODE_SMS:
        if((ucEvent == REPORT_TYPE_LOGIN) || (ucEvent == REPORT_TYPE_HEARTBEAT))
        {
            return FALSE; //��¼�������ϱ�������GPRS��ʽ�½���
        }
        return TRUE;
    case COMM_MODE_CSD:
        if((ucEvent == REPORT_TYPE_LOGIN) || (ucEvent == REPORT_TYPE_HEARTBEAT))
        {
            return FALSE; //��¼�������ϱ�������GPRS��ʽ�½���
        }

        if(ucEvent == REPORT_TYPE_ALARM)
        {
            return FALSE; //CSD��ʽ�²��ܷ��͸澯
        }
        return TRUE;
    case COMM_MODE_GPRS:
        if(g_stCenterConnStatus.ucLinkStatus != COMM_STATUS_CONNECTED)
        {
            return FALSE; //��������ĵ�������δ�������ܷ���
        }

        if((ucEvent != REPORT_TYPE_STA_INIT) && (ucEvent != REPORT_TYPE_LOGIN))//��վ�ϱ��͵�¼�ϱ�������δ��¼������·���
        {
            if(g_stCenterConnStatus.ucLoginStatus != COMM_STATUS_LOGIN)
            {
                return FALSE;
            }
        }
        //ͨ�Ŵӻ����ܷ��͵�¼����������
        if((g_stDevTypeTable.ucCommRoleType != COMM_ROLE_MASTER)&&((ucEvent == REPORT_TYPE_LOGIN)||(ucEvent == REPORT_TYPE_HEARTBEAT)))
        {
            return FALSE;
        }        
        return TRUE;
    default:
        return FALSE; //��Ӧ�ý��������֧
    }
}

/*************************************************
  Function:
  Description:    �������ṩ�����ϱ����񣬵��ϱ��¼�����������ñ�����; �ɵײ㸺
                  ��ʵ�ʵ�ͨ�ŷ�ʽ
  Calls:
  Called By:
  Input:          ucEvent: �ϱ��¼�
                  ucResult:�����Ľ����������ϱ���������Ļ�
  Output:
  Return:         SUCCEED��ʾ�ɹ��������ʾʧ��
  Others:         ���������ⲿ����
*************************************************/
LONG ReportOtherEvent(UCHAR ucEvent, UCHAR ucUpgradeResult)
{
    ULONG ulLen = 0;
    UCHAR *pucBuf = NULL;
    UCHAR *pucRawApData = NULL;
    OHCH_SEND_DATA_REQ_ST *pstSend = NULL;

    //�ж��Ƿ�����ϱ�
    if(IsReportAllowed(ucEvent) == FALSE)
    {
        return FAILED;
    }
    
    //��������
    USHORT usPacketID = GEN_PACKET_ID();

    //�����ڴ�
    pucBuf = (UCHAR *)MyMalloc(g_pstMemPool256);
    if(pucBuf == 0)
    {
        return FAILED;
    }

    //�ȹ���ԭ��
    pstSend = (OHCH_SEND_DATA_REQ_ST *)pucBuf;
    pstSend->ulPrmvType = OHCH_SEND_DATA_REQ;
    pstSend->ulMagicNum = usPacketID; //ʹ��packet ID��Ϊmagic number
    pstSend->ucResendTime = RESEND_TIME_DATATYPE_REPORT;
    pstSend->ucDstIf    = COMM_IF_NORTH;
    pstSend->ucCommMode = g_stNmParamSet.ucCommMode;
    if(pstSend->ucCommMode == COMM_MODE_SMS)
    {
        pstSend->ucDCS      = 0; //��ʱĬ��ΪGSM 7 bit���룬����Ƕ���Ϣ��ʽ�Ļ�
        memcpy(pstSend->acDstTelNum, g_stNmParamSet.acReportTelNum, sizeof(pstSend->acDstTelNum));
    }
    pstSend->ucDataType = ucEvent; //DATA_TYPE_REPORT;
    
    pstSend->pucData    = pucBuf + DATA_START_POS; //Ϊ����߱���Ч�ʣ����ڴ��ǰ�����㹻ԭ�����Ŀռ�

    pucRawApData = pstSend->pucData + 1; //��������ʼ��־�ĵط� 
    
    ulLen = EncodeReportIncludeReportTypeOnly(pucRawApData, 
                                              ucEvent, g_stNmParamSet.ucCommMode,
                                              usPacketID);

    if(ucEvent == REPORT_TYPE_UPGRADE)
    {
        ulLen += EncodeUpgradeResult(pucRawApData + ulLen, ucUpgradeResult);
    }
    
    //�����ϱ���AP�㴦��
    ulLen = EncodeApData(pucRawApData, ulLen, 
                         ((AP_LAYER_ST *)pucRawApData)->ucApType, MEM_BLK_SIZE_256 - 4 - sizeof(OHCH_SEND_DATA_REQ_ST));
    if(ulLen == 0)
    {
        return FAILED;
    }
    pstSend->ulDataLen  = ulLen;
    
    //����������
    g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].usPacketID = usPacketID;
    if(g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].ucCommState == OH_WAIT_FOR_RSP)
    {
        //ֹͣ��ʱ��
        if(g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].pTimer) //���ԭ���ж�ʱ����Ҫ��ֹͣ
        {
            OHRemoveTimer(g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].pTimer, 
                          g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].ulTimerMagicNum);
            g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].pTimer = 0;
            g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].ulTimerMagicNum = 0;
        }
    }
    g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].ucCommState = OH_SEND_DATA_PENDING;    

    //��ԭ�﷢�͸�CH
    if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
    {
        MyFree((void * )pstSend);
        return FAILED;
    }
    if((ucEvent == REPORT_TYPE_YKPP_STA_INIT)||(ucEvent == REPORT_TYPE_YKPP_INSPECTION)||
      (ucEvent == REPORT_TYPE_YKPP_REPAIR_CONFIRMED)||(ucEvent == REPORT_TYPE_YKPP_CONFIG_CHANGED))  
    {
        SetReportLampStatus(1);//�������Ƴ���ָʾ�ϱ������Ѿ���ʼ
    }
    return SUCCEEDED;
}

/*************************************************
  Function:
  Description:    ��������ϵͳ��������֮ǰ�Ƿ���й������������
                  ���ϱ������Ľ�������ֻ���ļ�����У��ɹ�������
                  ��Ż�ʹ�ñ����������жϣ��Ҵ�ʱ���ʧ��ֻ������
                  ��Ϊbootloader�������ɹ�
  Calls:
  Called By:
  Input:          g_astFilePartTbl: �ļ�������
  Output:
  Return:         �����Ľ��
  Others:         ���������ⲿ����
*************************************************/
UCHAR CheckUpgradeAfterStartup()
{
    UCHAR ucUpgradeResult = UPGRADE_NOT_OCCURED;
    
    if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucStatus == PROGRAM_STATUS_UPGRADE)
    {
        g_astFilePartTbl[PROGRAM_PART_1_IDX].ucStatus = PROGRAM_STATUS_RUNNABLE;

        //��������汾��
        memcpy(g_stDevInfoSet.acSfwrVer, 
               g_astFilePartTbl[PROGRAM_PART_1_IDX].acVersion, 
               sizeof(g_stDevInfoSet.acSfwrVer));
        
        //ReportOtherEvent(REPORT_TYPE_UPGRADE, UPGRADE_DONE);
        ucUpgradeResult = UPGRADE_DONE;
    }
    else if(g_astFilePartTbl[PROGRAM_PART_2_IDX].ucStatus == PROGRAM_STATUS_UPGRADE)
    {
        g_astFilePartTbl[PROGRAM_PART_2_IDX].ucStatus = PROGRAM_STATUS_RUNNABLE;

        //��������汾��
        memcpy(g_stDevInfoSet.acSfwrVer, 
               g_astFilePartTbl[PROGRAM_PART_2_IDX].acVersion, 
               sizeof(g_stDevInfoSet.acSfwrVer));

        //ReportOtherEvent(REPORT_TYPE_UPGRADE, UPGRADE_DONE);
        ucUpgradeResult = UPGRADE_DONE;
    }

    if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucStatus == PROGRAM_STATUS_UNRUNNABLE)
    {
        g_astFilePartTbl[PROGRAM_PART_1_IDX].ucStatus = PROGRAM_STATUS_INVALID;

        //ReportOtherEvent(REPORT_TYPE_UPGRADE, UPGRADE_ABORTED);
        ucUpgradeResult = UPGRADE_ABORTED;
    }
    else if(g_astFilePartTbl[PROGRAM_PART_2_IDX].ucStatus == PROGRAM_STATUS_UNRUNNABLE)
    {
        g_astFilePartTbl[PROGRAM_PART_2_IDX].ucStatus = PROGRAM_STATUS_INVALID;

        //ReportOtherEvent(REPORT_TYPE_UPGRADE, UPGRADE_ABORTED);
        ucUpgradeResult = UPGRADE_ABORTED;
    }
    
    //�����ļ���Ϣ��FLASH
    SaveUpgradeInfo();

    return ucUpgradeResult;
}

/*************************************************
  Function:
  Description:    ������������㴦����Ӧ�����ã���鵱ǰ�Ƿ�
                  ��������ģʽ����������ж������Ƿ������������ش���
  Calls:
  Called By:
  Input:          g_stDevInfoSet.ucRunningMode: ����ģʽ
                  g_stUpgradeCtx:               �������̵�������
  Output:         
  Return:         0��ʾ����������������ʾ��Ҫ����
                  
  Others:         ���������ⲿ����
*************************************************/
LONG WaitForUpgradeCompletion()
{
    if(g_stDevInfoSet.ucRunningMode != RUNNING_MODE_UPGRADE)
    {
        return 0;
    }

    if((g_stUpgradeCtx.ulUpgradeProgress == TRANS_CTRL_ABORT)
     ||(g_stUpgradeCtx.ulUpgradeProgress == TRANS_CTRL_CANCEL)
     ||((g_stUpgradeCtx.ulUpgradeProgress == TRANS_CTRL_UPGRADE_COMPLETE) 
     && (g_stUpgradeCtx.lUpgradeResult != UPGRADE_DONE)))
    {
        //Ϊ�˷�ֹ�ж�ʱ����©
        StopUpgradeTimer();

        //�л������״̬
        g_stDevInfoSet.ucRunningMode = RUNNING_MODE_MONITOR;
        //MCM-50_20070105_zhonghw_begin
        g_stUpgradeCtx.ulUnSaveBlocks = 0;
        //MCM-50_20070105_zhonghw_end

        //�ϱ������Ľ��
        ReportOtherEvent(REPORT_TYPE_UPGRADE, (UCHAR)g_stUpgradeCtx.lUpgradeResult);

        //Ӧ�ò���Ҫ�������ĸ�λ����Ϊû�б��浽NV Memory��
        return 0;
    }
    else if(g_stUpgradeCtx.ulUpgradeProgress == TRANS_CTRL_UPGRADE_COMPLETE) 
    {
        //Ϊ�˷�ֹ�ж�ʱ����©
        StopUpgradeTimer();

        //�л������״̬
        g_stDevInfoSet.ucRunningMode = RUNNING_MODE_MONITOR;

        //�����ݱ��浽NV Memory��
        g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ucStatus  = PROGRAM_STATUS_VALID;
        g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileSize= g_stUpgradeCtx.ulSavedFileSize;
        g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ucNew     = FILE_NEW; //�����ļ����¾�״̬
        if(PROGRAM_PART_1_IDX == g_stUpgradeCtx.ulUpgradePartIdx)
        {
            g_astFilePartTbl[PROGRAM_PART_2_IDX].ucNew = FILE_OLD;
        }
        else
        {
            g_astFilePartTbl[PROGRAM_PART_1_IDX].ucNew = FILE_OLD;
        }
#ifndef M3
        //���ļ�ͷ��Ϣ���������浽�ļ���������
        memcpy(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].acFileName, 
               (void *)g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr,
               FPD_FILE_NAME_LEN);
        memcpy(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].aucTimeStamp,
               (void *)(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr + FPD_FILE_NAME_LEN),
               TIME_STAMP_LEN);
        memcpy(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].acVersion,
               (void *)(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr + FPD_FILE_NAME_LEN + TIME_STAMP_LEN),
               FILE_VERSION_LEN);
#endif        
        g_stUpgradeCtx.ulNextBlockSn = 0; //��ʼ��Next SN
        //MCM-50_20070105_zhonghw_begin
        g_stUpgradeCtx.ulUnSaveBlocks = 0;
        //MCM-50_20070105_zhonghw_end

        SaveUpgradeInfo();
        
        //��������
        return 1;
    }

    return 0;
}

/*************************************************
  Function:
  Description:    ������������ĸ澯���ͻ����еĸ澯��
                  ����ɸ澯��ض��󣬱����ڴ�����ڴ�
                  ���н��У��ɵ��÷���֤�ڴ泤���㹻
  Calls:
  Called By:
  Input:          pucDst: �澯�������ʼ����ĵ�ַ
                  pusSrc: ���ͻ������еĸ澯���ַ
                  ucCnt:  �澯�����
  Output:
  Return:         �����ĳ���
  Others:         
*************************************************/
ULONG EncodeAlarmItem(UCHAR *pucAlarmMsg, ALARM_ITEM_IN_BUF_ST *pstAlarmBuf, UCHAR ucCnt)
{
    MCPA_MOBJ_ST* pstMObj = NULL;
    UCHAR i = 0;
    ULONG ulLen = 0;

    for(i = 0; i < ucCnt; i++)
    {
        pstMObj = (MCPA_MOBJ_ST *)(pucAlarmMsg + ulLen);
        pstMObj->ucMObjLen = MOBJ_ALARM_LEN;
        pstMObj->aucMObjId[HI_BYTE]= MOBJ_ID_ALARM_STATUS_SET;
        pstMObj->aucMObjId[LO_BYTE]= (pstAlarmBuf + i)->ucIDLoByte;
        pstMObj->aucMObjContent[0] = (pstAlarmBuf + i)->ucStatus;
        ulLen += MOBJ_ALARM_LEN;
    }
    return ulLen;
}


/*************************************************
  Function:
  Description:    ���������췢�͸澯��ԭ��
  Calls:
  Called By:
  Input:          ucCtxIdx:            �澯�����ĵ�����
                  g_astAlarmReportCtx: �����ϱ�����������
                  g_stAlarmItemBuf:    �澯���ͻ���
  Output:         
  Return:         0��ʾ����ʧ�ܣ������ʾԭ��ָ��
                  
  Others:         
*************************************************/
UCHAR* BuildAlarmReport(UCHAR ucCtxIdx)
{
    UCHAR *pucBuf = NULL;
    OHCH_SEND_DATA_REQ_ST *pstSend = NULL;
    ULONG ulLen = 0;
    UCHAR *pucRawApData = NULL;
    
    //�����ڴ�
    pucBuf = (UCHAR *)MyMalloc(g_pstMemPool512);
    if(pucBuf == 0)
    {
        return 0;
    }

    //�ȹ���ԭ��
    pstSend = (OHCH_SEND_DATA_REQ_ST *)pucBuf;
    pstSend->ulPrmvType = OHCH_SEND_DATA_REQ;
    pstSend->ulMagicNum = g_astAlarmReportCtx[ucCtxIdx].usPacketID; //ʹ��packet ID��Ϊmagic number
    pstSend->ucResendTime = RESEND_TIME_DATATYPE_ALARM;
    pstSend->ucDstIf    = COMM_IF_NORTH;
    pstSend->ucCommMode = g_stNmParamSet.ucCommMode;
    if(pstSend->ucCommMode == COMM_MODE_SMS)
    {
        pstSend->ucDCS      = 0; //��ʱĬ��ΪGSM 7 bit���룬����Ƕ���Ϣ��ʽ�Ļ�
        memcpy(pstSend->acDstTelNum, g_stNmParamSet.acReportTelNum, sizeof(pstSend->acDstTelNum));
    }
    pstSend->ucDataType = REPORT_TYPE_ALARM; //DATA_TYPE_REPORT;
    
    pstSend->pucData    = pucBuf + DATA_START_POS; //Ϊ����߱���Ч�ʣ����ڴ��ǰ�����㹻ԭ�����Ŀռ�

    pucRawApData = pstSend->pucData + 1; //��������ʼ��־�ĵط� 
    
    ulLen = EncodeReportIncludeReportTypeOnly(pucRawApData, REPORT_TYPE_ALARM,
                                              g_stNmParamSet.ucCommMode,
                                              g_astAlarmReportCtx[ucCtxIdx].usPacketID);

    ulLen += EncodeAlarmItem(pucRawApData + ulLen,
                             &g_stAlarmItemBuf.astAlarmItems[g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemBegin],
                             g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemCount);

    //�����ϱ���AP�㴦��
    ulLen = EncodeApData(pucRawApData, ulLen, 
                         ((AP_LAYER_ST *)pucRawApData)->ucApType, MEM_BLK_SIZE_512 - 4 - sizeof(OHCH_SEND_DATA_REQ_ST));
    if(ulLen == 0)
    {
        return NULL;
    }
    pstSend->ulDataLen  = ulLen;

    return pucBuf;
}

//�������������ӵ�������ģ���CH����Ϊ�������裬�Ƚ���GPRS���ӣ��ٽ���TCP����
void ConnToCenter()
{
    UCHAR *pucBuf = (UCHAR *)MyMalloc(g_pstMemPool16);
    if(pucBuf == 0)
    {
        return; //�ڳ�������֮����Ӧ�ý��������֧�������޷����������⣬ֻ���ó����˳�
    }

    OHCH_CONN_CENTER_REQ_ST *pstSend = (OHCH_CONN_CENTER_REQ_ST *)pucBuf;
    pstSend->ulPrmvType = OHCH_CONN_CENTER_REQ;
    memcpy(pstSend->aucIpAddr, g_stNmParamSet.aucEmsIpAddr, sizeof(pstSend->aucIpAddr));
    pstSend->usPortNum = g_stNmParamSet.usEmsPortNum;
    
    if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
    {
        MyFree((void * )pstSend);
        return;
    }
    g_stCenterConnStatus.ucConnAttempCnt++;
}

//����������Ͽ�����
void DisconnFromCenter()
{
    //�Ͽ�GPRS���ӵ�ͬʱ�����¼��������ʱ��    
    if(g_astOtherReportCtx[REPORT_TYPE_LOGIN - OTHER_REPORT_BASE].pTimer)
    {
        OHRemoveTimer(g_astOtherReportCtx[REPORT_TYPE_LOGIN - OTHER_REPORT_BASE].pTimer, 
                      g_astOtherReportCtx[REPORT_TYPE_LOGIN - OTHER_REPORT_BASE].ulTimerMagicNum);
        g_astOtherReportCtx[REPORT_TYPE_LOGIN - OTHER_REPORT_BASE].pTimer = 0;
        g_astOtherReportCtx[REPORT_TYPE_LOGIN - OTHER_REPORT_BASE].ulTimerMagicNum = 0;
    }
    
    if(g_astOtherReportCtx[REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE].pTimer)
    {
        OHRemoveTimer(g_astOtherReportCtx[REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE].pTimer, 
                      g_astOtherReportCtx[REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE].ulTimerMagicNum);
        g_astOtherReportCtx[REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE].pTimer = 0;
        g_astOtherReportCtx[REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE].ulTimerMagicNum = 0;
    }
    
    UCHAR *pucBuf = (UCHAR *)MyMalloc(g_pstMemPool16);
    if(pucBuf == 0)
    {
        return;
    }

    OHCH_GPRS_DISCONN_REQ_ST *pstSend = (OHCH_GPRS_DISCONN_REQ_ST *)pucBuf;
    pstSend->ulPrmvType = OHCH_GPRS_DISCONN_REQ;
    if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
    {
        MyFree((void * )pstSend);
        return;
    }

    g_stCenterConnStatus.ucLinkStatus = COMM_STATUS_DISCONNECTED;
    g_stCenterConnStatus.ucLoginStatus= COMM_STATUS_LOGOUT;
}

//���������½��������ĵ����ӣ����ȶϿ����ӣ�Ȼ����������
void ReconnToCenter()
{
    if(g_stCenterConnStatus.ucLinkStatus == COMM_STATUS_CONNECTED)
    {
        DisconnFromCenter();
    }

    g_stCenterConnStatus.ucLinkStatus           = COMM_STATUS_DISCONNECTED;
    g_stCenterConnStatus.ucLoginStatus          = COMM_STATUS_LOGOUT;
    g_stCenterConnStatus.ucConnAttempCnt        = 0;
    g_stCenterConnStatus.ucNoRspCnt             = 0;
    g_stCenterConnStatus.ucLoginTimeoutCnt      = 0;
    
    ConnToCenter();
}

/*************************************************
  Function:
  Description:    �������޸��豸���ͺ��޸ļ�ز����б�
                  �� ����Ӧ��ȫ�ּ�ز���д��FLASH,����������
  Calls:
  Called By:
  Input:          
  Output:         
  Return:         
  Others:         ���������ⲿ����
*************************************************/
void UpdateDevConfig()
{
    USHORT* pusMObjTbl;
    ULONG ulTblSize = 0;

    //��Ϊ�������ö��仯�ˣ�����Ҫ��ʼ��ȫ����������Ϊֻ����Ϊ���ͱ仯����ĳ�ʼ��,���Բ�������
    //����bootloader��һ��������е�flash���ݣ�����Ҫ�����еı������г�ʼ�����ﵽ������ʼ����Ҫ��

    //���ܲ���(�˲��ֱ������ƶ�����������أ����̵����в����׶�����и�λ)
    g_stNmParamSet.ulStaNum          = 0;     //վ����
    g_stNmParamSet.ucDevNum          = 0;     //�豸���
    g_stNmParamSet.usHeartbeatPeriod = 60;    //�������
    g_stNmParamSet.ucCommMode        = COMM_MODE_SMS; //ͨ�ŷ�ʽ

    
    InitCmccDefaultParam();
    InitYkppDefaultParam();

    
    //�����豸�������ò����б���Ӧ���͵��������
    switch(g_stDevInfoSet.ucDevType)
    {
    case WIDE_BAND:               //���ֱ��վ
        pusMObjTbl = g_aus3gWideBandTbl;
        ulTblSize = sizeof(g_aus3gWideBandTbl);
        break;
    case WIRELESS_FRESELT:        //ѡƵֱ��վ
        pusMObjTbl = g_aus3gWirLessFreSeltTbl;
        ulTblSize = sizeof(g_aus3gWirLessFreSeltTbl);
        break;
    case OPTICAL_DIRECOUPL_LOCAL: //����ֱ��վֱ����Ͻ��˻�
        pusMObjTbl = g_aus3gOpticalDireCouplLocalTbl;
        ulTblSize = sizeof(g_aus3gOpticalDireCouplLocalTbl);
        break;
    case OPTICAL_WIDEBAND_FAR:    //����ֱ��վ���Զ�˻�
        pusMObjTbl= g_aus3gOpticalWideBandFar;
        ulTblSize = sizeof(g_aus3gOpticalWideBandFar);    
        break;
    case TRUNK_AMPLIFIER:           //���߷Ŵ���
        pusMObjTbl= g_aus3gDryAmplifier;
        ulTblSize = sizeof(g_aus3gDryAmplifier);     
        break;
    case FRESHIFT_FRESELT_FAR:              //��Ƶֱ��վѡƵԶ�˻�
        pusMObjTbl = g_aus3gFreShiftFreSeltFarTbl;
        ulTblSize = sizeof(g_aus3gFreShiftFreSeltFarTbl);
        break;
    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL: //��Ƶֱ��վֱ����Ͽ�����˻�
        pusMObjTbl = g_aus3gFreShiftDireCouplWideBandLocalTbl;
        ulTblSize = sizeof(g_aus3gFreShiftDireCouplWideBandLocalTbl);
        break;   
    case FRESHIFT_WIRELESS_WIDEBAND_LOCAL: //��Ƶֱ��վ������Ͽ�����˻�
        pusMObjTbl = g_aus3gFreShiftWireLessCouplWideBandLocalTbl;
        ulTblSize = sizeof(g_aus3gFreShiftWireLessCouplWideBandLocalTbl);
        break;
    case OPTICAL_FRESELT_FAR:              //����ֱ��վѡƵԶ�˻�
        pusMObjTbl = g_aus3gOpticalFreseltFarTbl;
        ulTblSize = sizeof(g_aus3gOpticalFreseltFarTbl);
        break;
    case OPTICAL_WIRELESSCOUPL_LOCAL:      //����ֱ��վ������Ͻ��˻�
        pusMObjTbl = g_aus3gOpticalWireLessCouplLocalTbl;
        ulTblSize = sizeof(g_aus3gOpticalWireLessCouplLocalTbl);
        break;
    case FRESHIFT_WIDEBAND_FAR:            //��Ƶֱ��վ���Զ�˻�
        pusMObjTbl = g_aus3gFreShiftWideBandFarTbl;
        ulTblSize = sizeof(g_aus3gFreShiftWideBandFarTbl);
        break; 
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL: //��Ƶֱ��վֱ�����ѡƵ���˻�
        pusMObjTbl = g_aus3gFreShiftDireCouplFreSeltLocalTbl;
        ulTblSize = sizeof(g_aus3gFreShiftDireCouplFreSeltLocalTbl);
        break;
    case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL: //��Ƶֱ��վ�������ѡƵ���˻�
        pusMObjTbl = g_aus3gFreShiftWireLessCouplFreSeltLocalTbl;
        ulTblSize = sizeof(g_aus3gFreShiftWireLessCouplFreSeltLocalTbl);
        break;
    case BS_AMPLIFIER:                    //��վ�Ŵ���
        break;        
    case (WIDE_BAND + DEV_WITH_SEC):
        pusMObjTbl= g_ausWideBandWithSecTbl;
        ulTblSize = sizeof(g_ausWideBandWithSecTbl);
        break;
    case POI_STAKEOUT:
        pusMObjTbl= g_ausPoiDevConfig;
        ulTblSize = sizeof(g_ausPoiDevConfig);
        break;
//---------------------2g����------------------------//
    case WIDE_BAND_2G:               //���ֱ��վ
        pusMObjTbl = g_aus2gWideBandTbl;
        ulTblSize = sizeof(g_aus2gWideBandTbl);
        break;
    case WIRELESS_FRESELT_2G:        //ѡƵֱ��վ
        pusMObjTbl = g_aus2gWirLessFreSeltTbl;
        ulTblSize = sizeof(g_aus2gWirLessFreSeltTbl);
        break;
    case OPTICAL_DIRECOUPL_LOCAL_2G: //����ֱ��վֱ����Ͻ��˻�
        pusMObjTbl = g_aus2gOpticalDireCouplLocalTbl;
        ulTblSize = sizeof(g_aus2gOpticalDireCouplLocalTbl);
        break;
    case OPTICAL_WIDEBAND_FAR_2G:    //����ֱ��վ���Զ�˻�
        pusMObjTbl= g_aus2gOpticalWideBandFar;
        ulTblSize = sizeof(g_aus2gOpticalWideBandFar);    
        break;
    case TRUNK_AMPLIFIER_2G:           //���߷Ŵ���
        pusMObjTbl= g_aus2gDryAmplifier;
        ulTblSize = sizeof(g_aus2gDryAmplifier);     
        break;
    case FRESHIFT_FRESELT_FAR_2G:              //��Ƶֱ��վѡƵԶ�˻�
        pusMObjTbl = g_aus2gFreShiftFreSeltFarTbl;
        ulTblSize = sizeof(g_aus2gFreShiftFreSeltFarTbl);
        break;
    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL_2G: //��Ƶֱ��վֱ����Ͽ�����˻�
        pusMObjTbl = g_aus2gFreShiftDireCouplWideBandLocalTbl;
        ulTblSize = sizeof(g_aus2gFreShiftDireCouplWideBandLocalTbl);
        break;   

    //MCM-63_20070315_Zhonghw_begin        
    case BI_FRE_OPTICAL_FAR_2G:              //˫Ƶ����ֱ��վԶ�˻�
        pusMObjTbl = g_aus2gBiFreOpticalFarTbl;
        ulTblSize = sizeof(g_aus2gBiFreOpticalFarTbl);
        break;
    case BI_FRE_OPTICAL_DIRECOUPL_LOCAL_2G:              //˫Ƶ����ֱ��վֱ����Ͻ��˻�
        pusMObjTbl = g_aus2gBiFreOpticalDireCouplLocalTbl;
        ulTblSize = sizeof(g_aus2gBiFreOpticalDireCouplLocalTbl);
        break;
    case BI_DIRECT_WIDE_BAND_TOWER_AMP_2G:              //˫��������
        pusMObjTbl = g_aus2gBiDirectionalWideBandTowerAmpTbl;
        ulTblSize = sizeof(g_aus2gBiDirectionalWideBandTowerAmpTbl);
        break;
    case BI_DIRECT_FRE_SELECT_TOWER_AMP_2G:              //˫��ѡƵ����
        pusMObjTbl = g_aus2gBiDirectionalFreSelectTowerAmpTbl;
        ulTblSize = sizeof(g_aus2gBiDirectionalFreSelectTowerAmpTbl);
        break;        
    //MCM-63_20070315_Zhonghw_begin            
            
    case FRESHIFT_WIRELESS_WIDEBAND_LOCAL_2G: //��Ƶֱ��վ������Ͽ�����˻�
        pusMObjTbl = g_aus2gFreShiftWireLessCouplWideBandLocalTbl;
        ulTblSize = sizeof(g_aus2gFreShiftWireLessCouplWideBandLocalTbl);
        break;
    case OPTICAL_FRESELT_FAR_2G:              //����ֱ��վѡƵԶ�˻�
        pusMObjTbl = g_aus2gOpticalFreseltFarTbl;
        ulTblSize = sizeof(g_aus2gOpticalFreseltFarTbl);
        break;
    //MCM-63_20070315_Zhonghw_end
    case OPTICAL_CARRIER_WIDEBAND_LOCAL_2G:              //�����ز��ؿ����վ�˻�
        pusMObjTbl = g_aus2gOpticalCarryWideBandLocalTbl;
        ulTblSize = sizeof(g_aus2gOpticalCarryWideBandLocalTbl);
        break;
    case OPTICAL_CARRIER_FRESELT_LOCAL_2G:              //�����ز���ѡƵ��վ�˻�
        pusMObjTbl = g_aus2gOpticalCarryFreSelectLocalTbl;
        ulTblSize = sizeof(g_aus2gOpticalCarryFreSelectLocalTbl);
        break;
    case OPTICAL_CARRIER_WIDEBAND_FAR_2G:              //�����ز��ؿ��Զ�˻�
        pusMObjTbl = g_aus2gOpticalCarryWideBandFarTbl;
        ulTblSize = sizeof(g_aus2gOpticalCarryWideBandFarTbl);
        break;
    case OPTICAL_CARRIER_FRESELT_FAR_2G:              //�����ز���ѡƵԶ�˻�
        pusMObjTbl = g_aus2gOpticalCarryFreSelectFarTbl;
        ulTblSize = sizeof(g_aus2gOpticalCarryFreSelectFarTbl);
        break;        
    //MCM-63_20070315_Zhonghw_end
    
    case OPTICAL_WIRELESSCOUPL_LOCAL_2G:      //����ֱ��վ������Ͻ��˻�
        pusMObjTbl = g_aus2gOpticalWireLessCouplLocalTbl;
        ulTblSize = sizeof(g_aus2gOpticalWireLessCouplLocalTbl);
        break;
    case FRESHIFT_WIDEBAND_FAR_2G:            //��Ƶֱ��վ���Զ�˻�
        pusMObjTbl = g_aus2gFreShiftWideBandFarTbl;
        ulTblSize = sizeof(g_aus2gFreShiftWideBandFarTbl);
        break; 
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL_2G: //��Ƶֱ��վֱ�����ѡƵ���˻�
        pusMObjTbl = g_aus2gFreShiftDireCouplFreSeltLocalTbl;
        ulTblSize = sizeof(g_aus2gFreShiftDireCouplFreSeltLocalTbl);
        break;
    case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL_2G: //��Ƶֱ��վ�������ѡƵ���˻�
        pusMObjTbl = g_aus2gFreShiftWireLessCouplFreSeltLocalTbl;
        ulTblSize = sizeof(g_aus2gFreShiftWireLessCouplFreSeltLocalTbl);
        break;
    case (WIRELESS_FRESELT_2G + DEV_WITH_SEC):        //ѡƵֱ��վ+̫����
        pusMObjTbl = g_aus2gWirLessFreSeltWithSecTbl;
        ulTblSize = sizeof(g_aus2gWirLessFreSeltWithSecTbl);
        break;
    case BS_AMPLIFIER_2G:                    //��վ�Ŵ���
        //MCM-63_20070315_Zhonghw_begin
        pusMObjTbl = g_ausBSAmpTbl;
        ulTblSize = sizeof(g_ausBSAmpTbl);    
        break;
        //MCM-63_20070315_Zhonghw_end

//---------------------2g����------------------------//
//---------------------���Ի���------------------------//
    case MACHINE_FOR_TEST:
        pusMObjTbl = g_ausTestMachine;
        ulTblSize = sizeof(g_ausTestMachine);
        break;
//---------------------���Ի���------------------------//
    default:
        pusMObjTbl = g_ausTestMachine;
        ulTblSize = sizeof(g_ausTestMachine);
        return;
    }
    
    //MCM-24_20061108_zhangjie_begin
    if(ulTblSize > sizeof(g_stMObjIdTbl))
    {
        ulTblSize = sizeof(g_stMObjIdTbl);
    }
    //MCM-24_20061108_zhangjie_end
    
    memcpy(&g_stMObjIdTbl, pusMObjTbl, ulTblSize);

    //����������Ϣ
    SaveReadOnlyParam();
    SaveAlarmCenterStatus();

    SaveUpgradeInfo();
    SaveWritableParam();
    SaveYkppParam();
    
    MyDelay(1000);
    Restart();
}
/*************************************************
  Function:
  Description:    ����������ʵ�ʵ��ŵ������Զ������ŵ��ź���Ƶ��
                  ID��ʹ���ŵ�������Ƶ�������ŵ��������Ӧ��
  Calls:
  Called By:
  Input:          
  Output:         
  Return:         
  Others:         ���������ⲿ����
*************************************************/
void HandleModifyChNumAccordWithChCnt( )
{
    USHORT usChCount = g_stDevInfoSet.ucChCount;
    USHORT usShiftChCount = g_stDevInfoSet.ucChCount;    
    USHORT usParamCount = g_stMObjIdTbl.usParamCount;
    USHORT usCpyCnt = 0;
    UCHAR i = 0;
    USHORT j = 0;
    
    //*****************************���ù����ŵ���********************************
    if((g_stDevTypeTable.ucChannelType == CH_SHIFT_NOTHING)||(g_stDevTypeTable.ucChannelType == ONLY_SHIFT))
    {
        usChCount = 0;
    }    
    for(i = 0; i < usChCount; i++)    //ȷ������Ӧ�ð����Ĳ���
    {    
        for(j=0;j<usParamCount;j++)
        {
            if(g_stMObjIdTbl.ausParamID[j] == g_ausWorkChNumMObjId[i])
            {
               break;
            }
        }
        if(j >= usParamCount)    //δ�ҵ�Ӧ�ð����Ĳ������ʴ��źź������п�����ID
        {
            memcpy((CHAR *)&g_stMObjIdTbl.ausParamID + usParamCount * sizeof(USHORT),
                   (CHAR *)&g_ausWorkChNumMObjId[i],
                   sizeof(USHORT));
        
            g_stMObjIdTbl.usParamCount++;//���¼�ز�������
            usParamCount++;
        }
    }
    
    for(i=usChCount;i<16;i++)      //ȷ����������Ӧ�ð����Ĳ�����16��ʾ�źźŵ�����
    {
        for(j=0;j<usParamCount;j++)
        {
            if(g_stMObjIdTbl.ausParamID[j] == g_ausWorkChNumMObjId[i])//�ҵ���Ӧ�ð����Ĳ�����Ӧ��ɾ����ID            
            {
                usCpyCnt = usParamCount - j - 1;
                memcpy((CHAR *)&g_stMObjIdTbl.ausParamID[j],(CHAR *)&g_stMObjIdTbl.ausParamID[j+1],sizeof(USHORT) * usCpyCnt);
                g_stMObjIdTbl.usParamCount --;//���¼�ز�������
                usParamCount --;
                break;
            }
        }
    }
       
    //*****************************������Ƶ�ŵ���********************************
    if((g_stDevTypeTable.ucChannelType == CH_SHIFT_NOTHING)||(g_stDevTypeTable.ucChannelType == ONLY_CH))
    {
        usShiftChCount = 0;
    }
    
    for(i = 0; i < usShiftChCount; i++)    //ȷ������Ӧ�ð����Ĳ���
    {
        for(j=0;j<usParamCount;j++)
        {
            if(g_stMObjIdTbl.ausParamID[j] == g_ausShiftChNumMObjId[i])
            {
               break;
            }
        }
        if(j >= usParamCount)    //δ�ҵ�Ӧ�ð����Ĳ������ʴ��źź������п�����ID
        {
            memcpy((CHAR *)&g_stMObjIdTbl.ausParamID + usParamCount * sizeof(USHORT),
                   (CHAR *)&g_ausShiftChNumMObjId[i],
                   sizeof(USHORT));
        
            g_stMObjIdTbl.usParamCount++;//���¼�ز�������
            usParamCount++;
        }
    }
    
    for(i=usShiftChCount;i<16;i++)      //ȷ����������Ӧ�ð����Ĳ�����16��ʾ�źźŵ�����
    {
        for(j=0;j<usParamCount;j++)
        {
            if(g_stMObjIdTbl.ausParamID[j] == g_ausShiftChNumMObjId[i])//�ҵ���Ӧ�ð����Ĳ�����Ӧ��ɾ����ID            
            {
                usCpyCnt = usParamCount - j - 1;
                memcpy((CHAR *)&g_stMObjIdTbl.ausParamID[j],(CHAR *)&g_stMObjIdTbl.ausParamID[j+1],sizeof(USHORT) * usCpyCnt);
                g_stMObjIdTbl.usParamCount --;//���¼�ز�������
                usParamCount --;
                break;
            }
        }
    }
}

/*************************************************
  Function:
  Description:    �����������ò����赽Ӳ���豸�У�ʹ֮��Ч������
                  ��������յ����ݺ�����������
  Calls:
  Called By:
  Input:          g_stSettingParamSet:    ���ò�����
                  g_usSettingParamBitmap: ���ò���λͼ
  Output:         
  Return:         
  Others:         ���������ⲿ����
*************************************************/
void TakeParamEffect()
{
    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_RF_SW))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_RF_SW);
        SetRfSw();
    }
    
    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_ATT))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_ATT);
        SetAtt();
    }

    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_CH_NUM))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_CH_NUM);
        SetFreq();
    }

    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_SMC_ADDR))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_SMC_ADDR);

        //�����ڴ�
        UCHAR *pucBuf = (UCHAR *)MyMalloc(g_pstMemPool256);
        if(pucBuf == 0)
        {
            return; //���ı����ñ�־��׼���´���ִ��
        }

        OHCH_RW_SMC_ADDR_REQ_ST *pstSend = (OHCH_RW_SMC_ADDR_REQ_ST *)pucBuf;
        pstSend->ulPrmvType = OHCH_W_SMC_ADDR_REQ;
        memcpy(pstSend->acTelNum, g_stNmParamSet.acSmcAddr, sizeof(g_stNmParamSet.acSmcAddr));

        if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
        {
            MyFree((void *) pstSend);
            return;
        }
    }

    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_MODIFY_DEV_TYPE))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_DEV_TYPE);
        UpdateDevConfig();
    }
    
    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_MODIFY_CH_COUNT))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_CH_COUNT);
        MyDelay(1000);
        Restart(); 
    }    

    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_MODIFY_MOB_LIST))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_MOB_LIST);
        MyDelay(1000);
        Restart();
    }
    //MCM-32_20061114_zhonghw_begin
    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_SWITCH_FILE_VER ))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_SWITCH_FILE_VER);
        MyDelay(1000);
        Restart();
    }    
    //MCM-32_20061114_zhonghw_end
    UCHAR ucNeedToConnCenter = 0;
    
    //�ж�ͨ�ŷ�ʽ�Ƿ񱻸ı䣬����ı�����Ҫ�����л�
    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_MODIFY_COMM_MODE))
    {
        if((g_stNmParamSet.ucCommMode == COMM_MODE_SMS) //��GPRS�л���SMS
         &&(g_stCenterConnStatus.ucLinkStatus == COMM_STATUS_CONNECTED))
        {
            DisconnFromCenter();
        }

        else if((g_stNmParamSet.ucCommMode == COMM_MODE_CSD) //��GPRS�л���CSD
         &&(g_stCenterConnStatus.ucLinkStatus == COMM_STATUS_CONNECTED))
        {
            DisconnFromCenter();
        }
        
        else if(g_stNmParamSet.ucCommMode == COMM_MODE_GPRS)
        {
            ucNeedToConnCenter = 1;
        }
        
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_COMM_MODE);
    }

    //�����GPRS��ʽ���޸���IP��ַ��˿ںţ�����Ҫ��������
    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_MODIFY_IP_ADDR)
    || IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_MODIFY_PORT_NUM))
    {
        if((g_stNmParamSet.ucCommMode == COMM_MODE_GPRS)
          &&(g_stCenterConnStatus.ucLinkStatus == COMM_STATUS_CONNECTED))
        {
            ucNeedToConnCenter = 1;
        }

        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_IP_ADDR);
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_PORT_NUM);
    }

    if(ucNeedToConnCenter)
    {
        ReconnToCenter();
    }
    
}

/*************************************************
  Function:
  Description:    �������ڽ���ԭ���϶����е�Э�����ݽ���
                  ���벢���ͣ��������ʧ�ܣ�ֱ�����ڲ���
                  ԭ���ͷŵ�
  Calls:
  Called By:
  Input:          pstPrmv: �������ݵ�ԭ��
  Output:         
  Return:         SUCCEEDED��ʾ�ɹ���FAILED��ʾʧ��
  Others:         
*************************************************/
LONG EncodeAndResponse(OHCH_RECV_DATA_IND_ST *pstPrmv)
{
    AP_LAYER_ST *pstApData  = (AP_LAYER_ST *)(pstPrmv->pucData + 1); //pstPrmv->pucDataָ����ʼ��־��δ��ɵ�Э��Ӧ�����ݣ���δ����AP�����
    VPA_LAYER_ST *pstVpData = (VPA_LAYER_ST *)pstApData->aucPdu;

    //VP�����ݱ��룬��Ҫ�������վ���ź��豸��ŵ��������Ҫ������Ӧ�ֶ�
    pstVpData->ulStaNum = g_stNmParamSet.ulStaNum;
    pstVpData->ucDevNum = g_stNmParamSet.ucDevNum;

    //AP�����ݱ���
    ULONG ulRawApDataLen = pstPrmv->ulDataLen - 2 - CRC_LEN; //Ҫ�۳���ʼ������־��CRC����

    //pucDataָ����ʼ��־
    ULONG ulDataLen = EncodeApData(pstPrmv->pucData + 1, ulRawApDataLen,
                                   ((AP_LAYER_ST *)(pstPrmv->pucData + 1))->ucApType, pstPrmv->ulDataLenMax);

    if(ulDataLen == 0)
    {
        MyFree(pstPrmv);
        return FAILED;
    }

    //����ӿڷ���Э������
    OHCH_SEND_DATA_REQ_ST *pstSend = (OHCH_SEND_DATA_REQ_ST *)pstPrmv;
    pstSend->ulPrmvType   = OHCH_SEND_DATA_REQ;
    pstSend->ulMagicNum   = 0;
    pstSend->ucResendTime = RESEND_TIME_DATATYPE_RSP;
    pstSend->ucDstIf      = COMM_IF_NORTH;
    //pstSend->ucCommMode; //����
    //pstSend->ucDCS;      //����
    //pstSend->acDstTelNum;//���䣬ԭ��SrcTelNum��λ��
    pstSend->ucDataType = DATA_TYPE_RSP;
    pstSend->ulDataLen  = ulDataLen;

    //��ԭ�﷢�͸�CH
    if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
    {
        MyFree(pstSend);
        return FAILED;
    }

    return SUCCEEDED;
}

/*************************************************
  Function:
  Description:    �����������ϱ����̶�ʱ����ʱ�����
  Calls:
  Called By:
  Input:          pstPrmv:                      ��ʱ����ʱԭ��
                  g_stDevInfoSet.ucRunningMode: ����ģʽ
                  g_astOtherReportCtx:          �����ϱ�����������
                  g_stUpgradeCtx:               �������̵�������
  Output:         
  Return:         
                  
  Others:         ���������ⲿ���ã�����ԭ��������ͷ�
*************************************************/
void TimerExpired(TIMEOUT_EVENT_ST *pstPrmv)
{
    OH_TIMER_CTX_UN uCtx;
    uCtx.ulParam = (ULONG)pstPrmv->pvPtr;
    UCHAR *pucBuf = NULL;
    UCHAR ucCtxIdx = 0;
    
    switch(uCtx.stParam.ucTimerID)
    {
    case TIMER_ALARM_REPORT_ID:
        ucCtxIdx = uCtx.stParam.ucParam1;
        switch(g_astAlarmReportCtx[ucCtxIdx].ucResendState)
        {
        case FIRST_NO_TIMEOUT:              //��δ��ʱ
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = FIRST_1_SHORT_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;
            break;
        case FIRST_1_SHORT_TIMEOUT:         //��һ�ֵ�1�ζ̳�ʱ
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = FIRST_2_SHORT_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;
            break;
        case FIRST_2_SHORT_TIMEOUT:         //��һ�ֵ�2�ζ̳�ʱ
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = FIRST_WAIT_FOR_LONG_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;

            //����һ������ʱ��ʱ��
            g_astAlarmReportCtx[ucCtxIdx].pTimer
                = OHCreateTimer((void *)uCtx.ulParam, TIMER_ALARM_LONG_INTV, 
                               &(g_astAlarmReportCtx[ucCtxIdx].ulTimerMagicNum));
            return;
        case FIRST_WAIT_FOR_LONG_TIMEOUT:   //�ȴ���һ�ֳ���ʱ
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = SECOND_NO_TIMEOUT;
            break;
        case SECOND_NO_TIMEOUT:             //��һ�ֳ���ʱ
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = SECOND_1_SHORT_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;
            break;
        case SECOND_1_SHORT_TIMEOUT:        //�ڶ��ֵ�1�ζ̳�ʱ
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = SECOND_2_SHORT_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;
            break;
        case SECOND_2_SHORT_TIMEOUT:        //�ڶ��ֵ�2�ζ̳�ʱ
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = SECOND_WAIT_FOR_LONG_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;

            //����һ������ʱ��ʱ��
            g_astAlarmReportCtx[ucCtxIdx].pTimer
                = OHCreateTimer((void *)uCtx.ulParam, TIMER_ALARM_LONG_INTV, 
                               &(g_astAlarmReportCtx[ucCtxIdx].ulTimerMagicNum));
            return;
        case SECOND_WAIT_FOR_LONG_TIMEOUT:  //�ȴ��ڶ��ֳ���ʱ
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = THIRD_NO_TIMEOUT;
            break;
        case THIRD_NO_TIMEOUT:              //�ڶ��ֳ���ʱ
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = THIRD_1_SHORT_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;
            break;
        case THIRD_1_SHORT_TIMEOUT:         //�����ֵ�1�ζ̳�ʱ
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = THIRD_2_SHORT_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;
            break;
        case THIRD_2_SHORT_TIMEOUT:         //�����ֵ�2�ζ̳�ʱ
            g_stCenterConnStatus.ucNoRspCnt++;
            //����澯״̬���仯���򲻻��ٷ��͸澯���������do nothing
            return;
        default: //��Ӧ�ý��������֧
            return;
        }

        if(g_stCenterConnStatus.ucLoginStatus == COMM_STATUS_LOGIN)
        {
            if(g_stCenterConnStatus.ucNoRspCnt > MAX_HEARTBEAT_TO_CNT)
            {
                return; //����澯��ʱ������Ӧ������������ֵ�����˳��������������
            }
        }
        
        //�澯�ط��Ķ�ʱ������Ӧ�����յ�OHCH_SEND_DATA_CNF������
        //����������ʱ��uCtx.stParam.ucTimerID��uCtx.stParam.ucParam1����
        //g_astAlarmReportCtx[ucCtxIdx].pTimer
        //        = OHCreateTimer((void *)uCtx.ulParam, TIMER_ALARM_SHORT_INTV, 
        //                       &(g_astAlarmReportCtx[ucCtxIdx].ulTimerMagicNum));
        g_astAlarmReportCtx[ucCtxIdx].ucCommState = OH_SEND_DATA_PENDING;

        //�ж��Ƿ�����ϱ�
        if(IsReportAllowed(REPORT_TYPE_ALARM) == FALSE)
        {
            return;
        }

        //�����������
        pucBuf = BuildAlarmReport(ucCtxIdx);
        if(pucBuf == 0)
        {
            return; //�������ڴ���䲻������ģ���˵ȴ��´θ澯����ʱ�ٳ���
        }        
        
        //��ԭ�﷢�͸�CH
        if(OSQPost(g_pstCHNthQue, pucBuf) != OS_NO_ERR)
        {
            MyFree((void * )pucBuf);
            return;
        }        
        break;
    case TIMER_OTHER_REPORT_ID:
        switch(uCtx.stParam.ucParam1) //�����д�ŵ����ϱ�����
        {
        case REPORT_TYPE_STA_INIT:
        case REPORT_TYPE_INSPECTION:
        case REPORT_TYPE_REPAIR_CONFIRMED:
        case REPORT_TYPE_CONFIG_CHANGED:
        case REPORT_TYPE_UPGRADE:
            g_astOtherReportCtx[uCtx.stParam.ucParam1 - OTHER_REPORT_BASE].pTimer = 0;
            g_astOtherReportCtx[uCtx.stParam.ucParam1 - OTHER_REPORT_BASE].ulTimerMagicNum = 0;
            g_astOtherReportCtx[uCtx.stParam.ucParam1 - OTHER_REPORT_BASE].ucCommState = OH_IDLE;
            
            //�����ϱ���ʱ
            NotifyReportResult(uCtx.stParam.ucParam1, FAILED);
            break;
        default:
            break;
        }
        break;
    case TIMER_HEARTBEAT_PERIOD_ID: //����������
        ReportOtherEvent(REPORT_TYPE_HEARTBEAT, 0);
        break;
    case TIMER_LOGIN_PERIOD_ID: //���͵�¼
        ReportOtherEvent(REPORT_TYPE_LOGIN, 0);
        break;
    case TIMER_HEARTBEAT_TO_ID:
        //������ʱ����ı�ͨ�ſ�״̬
        g_astOtherReportCtx[REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE].ucCommState = OH_IDLE;
        
        g_stCenterConnStatus.ucNoRspCnt++;
        if(g_stCenterConnStatus.ucNoRspCnt > MAX_HEARTBEAT_TO_CNT)
        {
            //ReconnToCenter(); //׼���Ͽ����ӣ����µ�¼���������ͳһ��
        }
        else
        {
            RestartHeartbeatPeriodTimer(); //�����������ڶ�ʱ��
        }
        break;
    case TIMER_LOGIN_TO_ID:
        g_stCenterConnStatus.ucLoginTimeoutCnt++;
        if(g_stCenterConnStatus.ucLoginTimeoutCnt > MAX_LOGIN_TO_CNT)
        {
            ReconnToCenter(); //׼���Ͽ����ӣ����µ�¼
        }
        else
        {
            ReportOtherEvent(REPORT_TYPE_LOGIN, 0);
        }
        break;
    
    case TIMER_TRANS_CTRL_ID:
    case TIMER_TRANS_BLK_ID:
        g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_ABORT;
        g_stUpgradeCtx.lUpgradeResult    = UPGRADE_COMM_TIMEOUT;
        g_stUpgradeCtx.pTimer            = 0;
        
        //MCM-50_20070105_zhonghw_begin
        //�������ݰ�
        g_stUpgradeCtx.ulNextBlockSn -= g_stUpgradeCtx.ulUnSaveBlocks;
        g_stUpgradeCtx.ulUnSaveBlocks = 0;                        
        //MCM-50_20070105_zhonghw_end        

        break;

    //�첽������ʱ
    case TIMER_ASYN_OP_TO_ID:
        ucCtxIdx = uCtx.stParam.ucParam1;

        if(g_astAsynOpCtx[ucCtxIdx].pfTo)
        {
            g_astAsynOpCtx[ucCtxIdx].pfTo(&g_astAsynOpCtx[ucCtxIdx]);
        }
        break;
    default:
        return;
    }
}

/*************************************************
  Function:
  Description:    ����������CH���صķ������ݽ��ȷ�ϣ������
                  Ӧ�����ݣ��ɹ�������ͨ�Ŷ�ʱ����ʧ����ֱ��
                  �����ط���ʱ��(�澯)������ʾʧ��(�����ϱ�)��
  Calls:
  Called By:
  Input:          pstPrmv: ���صķ��ͽ��ȷ��
  Output:
  Return:         IGNORE��ʾ���ݿ����ͷţ�SUCCEEDED��ʾ���ݲ����ͷ�
  Others:         ���������ⲿ���ã�����ԭ��������ͷ�
*************************************************/
LONG HndlOHCHSendDataCnf(OHCH_SEND_DATA_CNF_ST *pstPrmv)
{
    ULONG i = 0;
    OH_TIMER_CTX_UN uCtx;
    ULONG ulIntv = 0;

    //�ж��Ƿ����ϱ�����
    if(pstPrmv->ucDataType == DATA_TYPE_RSP)
    {
        if(pstPrmv->cResult != SUCCEEDED)
        {
            if(pstPrmv->ucResendTime == 0) //����ط�������Ϊ0�������ٷ���
            {
                return IGNORE;
            }

            pstPrmv->ucResendTime --;
            
            pstPrmv->ulPrmvType = OHCH_SEND_DATA_REQ;
            if(OSQPost(g_pstCHNthQue, pstPrmv) != OS_NO_ERR)
            {
                MyFree((void * )pstPrmv);
                return IGNORE;
            }
            return SUCCEEDED;
        }
        else
        {
            return IGNORE;
        }
    }
    else //�ϱ�
    {
        if(pstPrmv->cResult != SUCCEEDED)
        {
            if(pstPrmv->ucResendTime > 0) //����ط�����������0���������
            {
                pstPrmv->ucResendTime --;
                pstPrmv->ulPrmvType = OHCH_SEND_DATA_REQ;
                if(OSQPost(g_pstCHNthQue, pstPrmv) != OS_NO_ERR)
                {
                    MyFree((void * )pstPrmv);
                    return IGNORE;
                }
                return SUCCEEDED;
            }
        }
    }

    //�澯�������в���
    for(i = 0; i < MAX_ALARM_REPORTS; i++)
    {
        if(g_astAlarmReportCtx[i].usPacketID == (USHORT)pstPrmv->ulMagicNum)
        {
            if(pstPrmv->cResult == SUCCEEDED)
            {
                //���ͳɹ������״̬Ǩ�ƣ�����ֱ��������ʱ�����ȴ��´γ�ʱ�ط�
                g_astAlarmReportCtx[i].ucCommState = OH_WAIT_FOR_RSP;
            
                //GPRS�³ɹ���¼������гɹ��ϱ���Ҫ��λ������ʱ��
                if(g_stCenterConnStatus.ucLoginStatus == COMM_STATUS_LOGIN)
                {   
                    //���OH����CH�ɹ����͵���û�еõ�����ȷ��
                    //����CH��OMC�ɹ����͵���û���յ�Ӧ��
                    //һ���������ķ��ͽ�����ڲþ��׶�
                    RestartHeartbeatPeriodTimer();
                }            
            }
            
            //������ʱ��
            uCtx.stParam.ucTimerID = TIMER_ALARM_REPORT_ID;    
            uCtx.stParam.ucParam1  = (UCHAR)i; //���ϱ������ĵ��±���Ϊ�������ڳ�ʱ����ʱ�ж�

            g_astAlarmReportCtx[i].pTimer = OHCreateTimer((void *)uCtx.ulParam, TIMER_ALARM_SHORT_INTV, 
                                                         &(g_astAlarmReportCtx[i].ulTimerMagicNum));
            return IGNORE;
        }
    }

    //������Ǹ澯�����������ϱ����������в���
    for(i = 0; i < OTHER_REPORT_COUNT; i++)
    {
        if(g_astOtherReportCtx[i].usPacketID == pstPrmv->ulMagicNum)
        {
            if(pstPrmv->cResult == SUCCEEDED) //���ͳɹ�
            {
                //������ʱ��
                if(i + OTHER_REPORT_BASE == REPORT_TYPE_LOGIN)
                {
                    uCtx.stParam.ucTimerID = TIMER_LOGIN_TO_ID;
                    ulIntv = TIMER_LOGIN_TO_INTV;
                }
                else if(i + OTHER_REPORT_BASE == REPORT_TYPE_HEARTBEAT)
                {
                    uCtx.stParam.ucTimerID = TIMER_HEARTBEAT_TO_ID;
                    ulIntv = TIMER_HEARTBEAT_TO_INTV;
                }
                else
                {
                    uCtx.stParam.ucTimerID = TIMER_OTHER_REPORT_ID;
                    ulIntv = TIMER_ALARM_SHORT_INTV;

                    //GPRS�³ɹ���¼������гɹ��ϱ���Ҫ��λ������ʱ��
                    if(g_stCenterConnStatus.ucLoginStatus == COMM_STATUS_LOGIN)
                    {   
                        //���OH����CH�ɹ����͵���û�еõ�����ȷ��
                        //����CH��OMC�ɹ����͵���û���յ�Ӧ��
                        //��һ���������Ľ���������ڲþ��׶Σ�����λ������ʱ��
                        RestartHeartbeatPeriodTimer();
                    }            
                }
                uCtx.stParam.ucParam1  = (UCHAR)i + OTHER_REPORT_BASE; //���ϱ�������Ϊ�������ڳ�ʱ����ʱ�ж�

                g_astOtherReportCtx[i].pTimer = OHCreateTimer((void *)uCtx.ulParam, ulIntv, 
                                                             &(g_astOtherReportCtx[i].ulTimerMagicNum));

                //״̬Ǩ��
                g_astOtherReportCtx[i].ucCommState = OH_WAIT_FOR_RSP;
            }
            else //����ʧ��
            {
                if(g_astOtherReportCtx[i].pTimer) //���ԭ���ж�ʱ����Ҫ��ֹͣ
                {
                    OHRemoveTimer(g_astOtherReportCtx[i].pTimer, 
                                  g_astOtherReportCtx[i].ulTimerMagicNum);
                    g_astOtherReportCtx[i].pTimer = 0;
                    g_astOtherReportCtx[i].ulTimerMagicNum = 0;              
                }
                
                g_astOtherReportCtx[i].ucCommState = OH_IDLE;

                if(i + OTHER_REPORT_BASE == REPORT_TYPE_LOGIN) //��Ȼ������ʱ�����ȴ���ʱ��Ĵ���
                {
                    uCtx.stParam.ucTimerID = TIMER_LOGIN_TO_ID;
                    uCtx.stParam.ucParam1  = (UCHAR)i + OTHER_REPORT_BASE;
                    g_astOtherReportCtx[i].pTimer = OHCreateTimer((void *)uCtx.ulParam, TIMER_LOGIN_TO_INTV, 
                                                                 &(g_astOtherReportCtx[i].ulTimerMagicNum));
                }                
                else if(i + OTHER_REPORT_BASE == REPORT_TYPE_HEARTBEAT) //��Ȼ������ʱ�����ȴ���ʱ��Ĵ���
                {
                    uCtx.stParam.ucTimerID = TIMER_HEARTBEAT_TO_ID;
                    uCtx.stParam.ucParam1  = (UCHAR)i + OTHER_REPORT_BASE;
                    g_astOtherReportCtx[i].pTimer = OHCreateTimer((void *)uCtx.ulParam, TIMER_HEARTBEAT_TO_INTV, 
                                                                 &(g_astOtherReportCtx[i].ulTimerMagicNum));
                }
                else
                {
                    //֪ͨ�ϱ�ʧ��
                    NotifyReportResult((UCHAR)i + OTHER_REPORT_BASE, FAILED);
                }
            }
        }
    }
    return IGNORE;
}

/*************************************************
  Function:
  Description:    �������ṩ�ϱ��澯�ķ��񣬽���������
                  ������Э����룻�ɵײ㸺��ʵ�ʵ�ͨ�ŷ�ʽ
  Calls:
  Called By:
  Input:          g_astAlarmReportCtx: �澯������
                  g_uSavedAlarmItems:  �澯����Ϣ
                  g_stAlarmItemBuf:    ���͸澯�Ļ�����

  Output:
  Return:
  Others:         ���������ⲿ����
*************************************************/
void ReportAlarm()
{
    ULONG i = 0;
    UCHAR ucLocal  = 0;
    //UCHAR ucCenter = 0;
    UCHAR ucMaxAlarmItemCnt = 0;
    UCHAR ucCtxIdx = 0;
    UCHAR *pucBuf = NULL;

    //�ж��Ƿ�����ϱ�
    if(IsReportAllowed(REPORT_TYPE_ALARM) == FALSE)
    {
        return;
    }

    //�����µ������ģ�ԭ���Ķ����
    for(i = 0; i < MAX_ALARM_REPORTS; i++)
    {
        if(g_astAlarmReportCtx[i].pTimer) //���ԭ���ж�ʱ����Ҫ��ֹͣ
        {
            OHRemoveTimer(g_astAlarmReportCtx[i].pTimer, 
                          g_astAlarmReportCtx[i].ulTimerMagicNum);
            g_astAlarmReportCtx[i].pTimer = 0;
            g_astAlarmReportCtx[i].ulTimerMagicNum = 0;

            //����CH���ԭ���ķ��Ͳ���???
        }
    }
    memset(g_astAlarmReportCtx, 0, sizeof(g_astAlarmReportCtx));

    //��ո澯���ͻ���������Ϊ�澯�ط�ʱ�жϸ澯�����0Ϊ��Ч����
    memset(&g_stAlarmItemBuf, 0, sizeof(g_stAlarmItemBuf));

    //���ɻ���澯��
    for(i = 0; i < MAX_ALARM_ITEMS; i++)
    {
        if(g_uAlarmItems.astAlarmItemArray[i].ucSupportedBit == 0)
        {
            continue; //��֧�ָø澯��
        }
        
        ucLocal  = g_uAlarmItems.astAlarmItemArray[i].ucLocalStatus; //���ظ澯״̬
        
        if(ucLocal != g_uCenterStatus.aucAlarmStatusArray[i]) //���غ�����״̬��һ��
        {
            if(g_uAlarmItems.astAlarmItemArray[i].ucEffectiveBit) //����澯�����λ��߸澯ʹ�ܱ��ر�����Ҫ����
            {
                g_stAlarmItemBuf.astAlarmItems[g_stAlarmItemBuf.ulAlarmItemCount].ucUsed = 1;
                g_stAlarmItemBuf.astAlarmItems[g_stAlarmItemBuf.ulAlarmItemCount].ucIDLoByte 
                            = g_uAlarmItems.astAlarmItemArray[i].ucIDLoByte;
                g_stAlarmItemBuf.astAlarmItems[g_stAlarmItemBuf.ulAlarmItemCount].ucStatus = ucLocal;
                g_stAlarmItemBuf.ulAlarmItemCount++;
            }
        }
    }

    if(g_stNmParamSet.ucCommMode == COMM_MODE_SMS)
    {
        ucMaxAlarmItemCnt = MAX_ALARM_ITEM_COUNT_BY_APB; //����Ϣֻ������12���澯����Ŀǰ���ֻ֧�ַ���48���澯
    }
    else //Ŀǰֻ����ʹ��AP:A��������ʹ��AP:C???
    {
        ucMaxAlarmItemCnt = MAX_ALARM_ITEM_COUNT_BY_APA; //256�ֽ����ݿ�������59���澯��
    }

    i = 0;
    while(i < g_stAlarmItemBuf.ulAlarmItemCount)
    {
        //����������
        g_astAlarmReportCtx[ucCtxIdx].usPacketID = GEN_PACKET_ID();
        g_astAlarmReportCtx[ucCtxIdx].ucCommState = OH_SEND_DATA_PENDING;
        g_astAlarmReportCtx[ucCtxIdx].ucResendState = FIRST_NO_TIMEOUT;

        if(g_stAlarmItemBuf.ulAlarmItemCount - i >= ucMaxAlarmItemCnt)
        {
            g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemCount 
                    = (UCHAR)(ucMaxAlarmItemCnt);
        }
        else
        {
            g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemCount 
                    = (UCHAR)(g_stAlarmItemBuf.ulAlarmItemCount - i);
        }

        g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemBegin = (UCHAR)i;

        pucBuf = BuildAlarmReport(ucCtxIdx);
        if(pucBuf == 0)
        {
            return; //�������ڴ���䲻������ģ���˵ȴ��´θ澯����ʱ�ٳ���
        }        
        
        //��ԭ�﷢�͸�CH
        if(OSQPost(g_pstCHNthQue, pucBuf) != OS_NO_ERR)
        {
           MyFree((void * )pucBuf);
           return;
        }        
        i += ucMaxAlarmItemCnt;
        ucCtxIdx++;
        
        if(ucCtxIdx > MAX_ALARM_REPORTS) //��������Ժ����ĸ澯��
        {
            return;
        }
    }
}    

/*************************************************
  Function:
  Description:    �������ڸ澯�ɼ��ж������е���;
                  �����ж��Ƿ���Ҫ�ϱ��澯�������Ҫ���ϱ������ȱȽϱ��غ�
                  ���ĸ澯״̬���жϸ澯״̬�Ƿ����仯������澯����Ч����ԣ���
                  ����仯��׼��������뵽�������У�����仯�����������д��ڣ���
                  �����������������˵����Ҫ���·��͸澯�����ڱ��غ����ĸ澯״
                  ̬һ�£����Ǹ澯���Դ������������е��������Ҫ���·��͸澯��ԭ
                  ���������ǲ�����������˵������״̬�����䣬�ȵ�Ӧ����������
                  ���ĸ澯״̬�����������������ڸ澯ʹ�ܺ����εĲ���Ҫ������
                  �ϱ������λ�ʹ�ܱ��رյĸ澯�������Ҫ���������µĸ澯�
  Calls:
  Called By:
  Input:          g_uAlarmItems:    �澯����Ϣ
                  g_stAlarmItemBuf: �澯���ͻ���
  Output:         
  Return:         
  Others:         ���������ⲿ���ã�����ȫ�ֱ������и�ֵ
*************************************************/
void CheckAlarmStatus()
{
    UCHAR i = 0;
    UCHAR ucLocal  = 0;
    //UCHAR ucCenter = 0;
    UCHAR ucBufIdx = 0;       //�澯���ͻ����и澯������
    ULONG ulAlarmItemCnt = 0; //��¼�ȽϹ����������ĸ澯����ۼ���

    for(i = 0; i < MAX_ALARM_ITEMS; i++)
    {
        if(g_uAlarmItems.astAlarmItemArray[i].ucSupportedBit == 0)
        {
            continue; //��֧�ָø澯��
        }
    
        ucLocal  = g_uAlarmItems.astAlarmItemArray[i].ucLocalStatus; //���ظ澯״̬

        if(ucLocal != g_uCenterStatus.aucAlarmStatusArray[i]) //���غ�����״̬��һ��
        {
            if(ulAlarmItemCnt >= g_stAlarmItemBuf.ulAlarmItemCount) //˵����������û�иø澯����Ҫ�����ϱ�
            {
                if(g_uAlarmItems.astAlarmItemArray[i].ucEffectiveBit)
                {
                    break; //û�б�������ʹ�ܴ�
                }
                else
                {
                    continue;
                }
            }

            while(g_stAlarmItemBuf.astAlarmItems[ucBufIdx].ucUsed == 0) 
            {
                ucBufIdx++; //�����м�Ӧȷ�ϵĲ��֣�����澯�����ͻ�����������
            }

            if((g_stAlarmItemBuf.astAlarmItems[ucBufIdx].ucIDLoByte != g_uAlarmItems.astAlarmItemArray[i].ucIDLoByte)
            || ((g_stAlarmItemBuf.astAlarmItems[ucBufIdx].ucIDLoByte == g_uAlarmItems.astAlarmItemArray[i].ucIDLoByte)
              &&(g_stAlarmItemBuf.astAlarmItems[ucBufIdx].ucStatus != ucLocal))) //��һ��˵���澯�����仯����Ҫ���·���
            {
                if(g_uAlarmItems.astAlarmItemArray[i].ucEffectiveBit)
                {
                    break; //û�б�������ʹ�ܴ�
                }
            }
            else //˵���澯�仯�Ѿ������ͣ����ǻ�δӦ����˲���Ҫ���·���
            {
                ulAlarmItemCnt++;
                ucBufIdx++; //׼���Ƚ���һ���澯��
            }
        }
        else //���غ�����״̬һ��
        {
            if(ulAlarmItemCnt >= g_stAlarmItemBuf.ulAlarmItemCount) //��������û�иø澯��
            {
                continue;
            }

            while(g_stAlarmItemBuf.astAlarmItems[ucBufIdx].ucUsed == 0) 
            {
                ucBufIdx++; //�����м�Ӧȷ�ϵĲ��֣�����澯�����ͻ�����������
            }
            
            if(g_stAlarmItemBuf.astAlarmItems[ucBufIdx].ucIDLoByte 
            == g_uAlarmItems.astAlarmItemArray[i].ucIDLoByte)
            {
                break; //�澯���һ�£���ø澯����Ҫ�ӻ������������Ҫ�����ϱ��澯
            }
        }
    }

    if(i == MAX_ALARM_ITEMS) //����Ҫ���͸澯
    {
        return;
    }

    //���͸澯�仯֪ͨ��OH
    ALARM_STATUS_CHANGED_EVENT_ST *pstEvent = (ALARM_STATUS_CHANGED_EVENT_ST *)MyMalloc(g_pstMemPool16);
    if(pstEvent == 0)
    {
        return; //û�а취
    }

    pstEvent->ulPrmvType = ALARM_STATUS_CHANGED_EVENT;
#ifndef M3
    if(OSQPost(g_pstOHQue, pstEvent) != OS_NO_ERR)
    {
        MyFree((void * )pstEvent);
        return;
    }
#else
    ULONG aulMsgBuf[4];
    aulMsgBuf[3] = (ULONG)pstEvent;
    g_pOHQue->WriteQue((MSG_ST *)aulMsgBuf);
#endif
}

/*************************************************
  Function:
  Description:    ������������֤MCP:A��������Ч�ԣ��ж�
                  �Ƿ�֧�ָò������Լ������Ƿ���ȷ
  Calls:
  Called By:
  Input:          pstMcpA:      MCP:A����
                  ucDataType:   �������ͣ�ʵ���Ǳ�ʾ����
                  ucIsSupported:�Ƿ�֧��
  Output:         
  Return:         
  Others:         
*************************************************/
LONG ValidateMcpAParam(MCPA_MOBJ_ST *pstMcpA, UCHAR ucDataType, UCHAR ucIsSupported)
{
    LONG lRspFlag = MOBJ_CORRECT;
    ULONG ulMObjLen = 0;                      //���ڴ��mcpData�е�L��Ԫ��ֵ
    ULONG ulExpectedLen = 0;                  //���ڴ��Ԥ�ڵ�LTV�ܳ�
    
    //�ж��Ƿ�֧��
    if(ucIsSupported != PARAM_SUPPORTED)
    {
        //��ucMObjId[HI_BYTE]���ü�ض����޷�ʶ��Ĵ������ͣ�����rspFlag����ֱ�����
        SetErrCode(pstMcpA->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, lRspFlag);
        return  lRspFlag;   
    }

    //���L��ֵ�Ͷ���ṹ�е�L��Ԫ��ռ���ֽ���
    ulMObjLen = pstMcpA->ucMObjLen;

    //������Ч����
    ulExpectedLen = 3 + ucDataType; //3��T��Lռ���ֽ���

    //����Ϊ���ȼ�Ȩ
    if(ulExpectedLen != ulMObjLen)
    {
        SetErrCode(pstMcpA->aucMObjId[HI_BYTE], MOBJ_LEN_NOT_MATCH, lRspFlag);
        return lRspFlag;
    }

    return lRspFlag;
}
/*********************************************************************************
  Function:
  Description:    ����������MCP:A�еļ�ز����б�����
  Calls:
  Called By:
  Input:          pstMObj:    MCP������ָ��  
                  ucApType:   AP��������
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpAMObjListWrite(MCPA_MOBJ_ST *pstMObj)
{
    //��0��1�ֽ�Ϊ�����б��ܸ���(���ֽ���ǰ)
    //��2�ֽ�Ϊ�����б�ƫ�Ƶĸ���
    //��3�ֽ�Ϊ��ǰ���õĸ���
    //��4�ֽڿ�ʼΪ��ز����б�
    
    LONG lRspFlag = MOBJ_CORRECT;
    USHORT usDonedParamCnt = 0;
    USHORT usParamTotalCnt = 0;
    //��¼�ܸ���
    memcpy((UCHAR *)&usParamTotalCnt,&pstMObj->aucMObjContent[0],sizeof(usParamTotalCnt));
    //����Ч�Խ��м�Ȩ
    USHORT ucDataType = pstMObj->aucMObjContent[3] * 2 + 4; //Ϊ�˵���У�麯����У��ucDataType
    lRspFlag = ValidateMcpAParam(pstMObj,ucDataType,PARAM_SUPPORTED);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }
    memcpy((UCHAR *)&g_stMObjIdTbl.ausParamID + pstMObj->aucMObjContent[2] * 2,
           &pstMObj->aucMObjContent[4],pstMObj->aucMObjContent[3] * 2);

    //�����Ѿ����صĸ���
    usDonedParamCnt = pstMObj->aucMObjContent[2] + pstMObj->aucMObjContent[3];
    //�Ƚϼ�ز����б��Ƿ��������
    if(usParamTotalCnt == usDonedParamCnt)//����б��������
    {
        g_stMObjIdTbl.usParamCount = usDonedParamCnt;
        SET_BITMAP(g_usSettingParamBitmap,BITMAP_MODIFY_MOB_LIST);        
    }
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    ��������MCP:A�������в�ѯ�����ã���Щ����֧�ֲ�ѯ���ã������
                  NV Memory�У���ѯ���ù�����ֻ�������ݣ�û����������
                  ʹ�øú����Ĳ����������ܲ������澯ʹ����
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ 
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ�������   
  Output:         
  Return:         ��ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpANormalRW(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                        UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //����Ϊ�ж������������Ͳ����д���ͬʱ����2�ֽڻ���4�ֽ����ͽ��б�Ҫ���ֽ�����������
    //����Ҫ����ʵ��ֵ�Ĵ�С�����Ƿ�����ֽ�������ν������IP��ַ
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //��ѯ,�洢��������
        if((ucDataType == UINT_2) || (ucDataType == SINT_2))
        {
            //*(USHORT *)pstMObj->aucMObjContent = ADJUST_WORD(*(USHORT *)ulParamAddr);
            SET_WORD(pstMObj->aucMObjContent, GET_WORD(ulParamAddr));
        }
        else if(ucDataType == UINT_4)
        {
            //*(ULONG *)pstMObj->aucMObjContent = ADJUST_DWORD(*(ULONG *)ulParamAddr);
            SET_DWORD(pstMObj->aucMObjContent, GET_DWORD(ulParamAddr));
        }
        else
        {
            memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        }
        break;
    case MCPA_CMD_ID_SET: //����,�洢��д����
        if(ucDataType == UINT_2)
        {
            //*(USHORT *)ulParamAddr = ADJUST_WORD(*(USHORT *)pstMObj->aucMObjContent);
            SET_WORD(ulParamAddr, GET_WORD(pstMObj->aucMObjContent));
        }
        else if(ucDataType == UINT_4)
        {
            //*(ULONG *)ulParamAddr = ADJUST_DWORD(*(ULONG *)pstMObj->aucMObjContent);
            SET_DWORD(ulParamAddr, GET_DWORD(pstMObj->aucMObjContent));
        }
        else
        {
            memcpy((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        }    
        break;
        //ֻ֧�֡���ѯ���á��Ĳ���������Ĳ���������Э�鶨�巶Χ
    default: 
        //�����ܽ��������֧����Ϊ����֮ǰ�Ѿ�������������ж�
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    ��������MCP:Aֻ������"�豸��ʵ���ŵ�����"���ж�д�������ò�����д
                  ���������⣬��Ҫ�������õ�ʵ���ŵ�����ȥ���ü�ز����б��е��ŵ���
                  ID���������������ı�־���������Ҫ���������豸ʹ�����õļ�ز���
                  �б���Ч��
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ 
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ�������   
  Output:         
  Return:         ��ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpAChCount(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                       UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }
    //����Ϊ�ж������������Ͳ����д���
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //��ѯ,�洢��������
        pstMObj->aucMObjContent[0] = *(UCHAR *)ulParamAddr;
        break;
    case MCPA_CMD_ID_SET: //����,�洢��д����
        if(pstMObj->aucMObjContent[0] > 16)//16��ʾ�豸֧�ֵ����
        {
            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_VALUE_OUT_OF_BOUND, lRspFlag);
        }
        else if(*((UCHAR *)ulParamAddr) != (pstMObj->aucMObjContent[0]))
        {
            *((UCHAR *)ulParamAddr) = pstMObj->aucMObjContent[0];
            SET_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_CH_COUNT);
            HandleModifyChNumAccordWithChCnt();
        }
        break;
    default: 
        //�����ܽ��������֧����Ϊ����֮ǰ�Ѿ�������������ж�
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    ��������MCP:A�������в�ѯ�������ã�����Щ����ֻ֧�ֲ�ѯ�������
                  NV Memory����SRAM�У���ѯ���ù�����ֻ�������ݣ�û����������
                  ʹ�øú����ٵĲ��������豸��Ϣ��ʵʱ��������(������Χ��־��)
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ 
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ�������  
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpANormalR(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType,
                       UCHAR ucIsSupported, UCHAR)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }
    
    //���´����Ϊ���⣬Ϊ��Ӧ�ԡ�д�������г��֡�ֻ����������
    //��ֻ���Ĳ��������ж�������ֱ࣬�Ӷ�������ȡ����
    memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);

    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    �������������Χ��־��ʵʱ�����������ڷ���ʵʱ����������ͬʱҲ
                  �����Ƿ񳬳���Χ�ı�־
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ 
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ�������  
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpARcParamWithThr(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType,
                              UCHAR ucIsSupported, UCHAR)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //��ȡ��Χ��־����Χ��־��ʵ�ʲ���ƫ��Ϊ1
    UCHAR ucErr = *(UCHAR *)(ulParamAddr + ucDataType);

    if(ucErr == MOBJ_CORRECT)
    {
        memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);        
    }
    else
    {
        SetErrCode(pstMObj->aucMObjId[HI_BYTE], ucErr, lRspFlag);
    }

    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    ����������MCP:A�еĸ澯���ѯ���澯���ѯ����Ҫ��������״̬
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulIdx:        �澯���������е�����
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ������� 
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpAAlarmStatus(MCPA_MOBJ_ST *pstMObj, ULONG ulIdx, UCHAR ucDataType,
                           UCHAR ucIsSupported, UCHAR)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }
    
    //ֻ֧�ָ澯״̬��ѯ
    pstMObj->aucMObjContent[0] = g_uAlarmItems.astAlarmItemArray[ulIdx].ucLocalStatus;

    //��������״̬
    g_uCenterStatus.aucAlarmStatusArray[ulIdx]
            = g_uAlarmItems.astAlarmItemArray[ulIdx].ucLocalStatus;
   
    return lRspFlag;
}
/*********************************************************************************
  Function:
  Description:    ����������MCP:A�е��豸���ͣ��������豸����ʱ��Ҫ���ñ�־��
                  ����豸���������ñ仯�����������������������Ҫ��FLASH
                  �еĲ����б���и���
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ������� 
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpARWDevType(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                      UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //����Ϊ�ж������������Ͳ����д���   
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY:           //��ѯ��һ��Ĵ洢��������
        pstMObj->aucMObjContent[0] = *(UCHAR *)ulParamAddr;
        break;
    case MCPA_CMD_ID_SET:             //���ã��洢��д������ͬʱ
                                      //Ҫ�ñ�־������㽫��ز����б�д��FLASH
        if(*(UCHAR *)ulParamAddr != pstMObj->aucMObjContent[0])       //����豸���Ͳ�δ�ı��򲻶���
        {
            switch( pstMObj->aucMObjContent[0])
            {
            //--------3g����--------//
            case WIDE_BAND:
            case WIRELESS_FRESELT:
            case OPTICAL_DIRECOUPL_LOCAL:
            case OPTICAL_WIDEBAND_FAR:
            case TRUNK_AMPLIFIER:
            case FRESHIFT_FRESELT_FAR:
            case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL:
            case FRESHIFT_WIRELESS_WIDEBAND_LOCAL:
            case OPTICAL_FRESELT_FAR:
            case OPTICAL_CARRIER_WIDEBAND_LOCAL:
            case OPTICAL_CARRIER_FRESELT_LOCAL: 
            case OPTICAL_CARRIER_WIDEBAND_FAR: 
            case OPTICAL_CARRIER_FRESELT_FAR:                 
            case OPTICAL_WIRELESSCOUPL_LOCAL:
            case FRESHIFT_WIDEBAND_FAR:
            case FRESHIFT_DIRECOUPL_FRESELT_LOCAL:
            case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL:
            case BS_AMPLIFIER:
            case POI_STAKEOUT:
            case (WIDE_BAND + DEV_WITH_SEC):
            //--------3g����--------//                
            //--------2g����--------//
            case WIDE_BAND_2G:                              
            case WIRELESS_FRESELT_2G:                       
            case OPTICAL_DIRECOUPL_LOCAL_2G:                
            case OPTICAL_WIDEBAND_FAR_2G:                   
            case TRUNK_AMPLIFIER_2G:                          
            case FRESHIFT_FRESELT_FAR_2G:                   
            case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL_2G:      
            //MCM-63_20070315_Zhonghw_begin
            case BI_FRE_OPTICAL_FAR_2G:
            case BI_FRE_OPTICAL_DIRECOUPL_LOCAL_2G:
            case BI_DIRECT_WIDE_BAND_TOWER_AMP_2G:
            case BI_DIRECT_FRE_SELECT_TOWER_AMP_2G:
            //MCM-63_20070315_Zhonghw_end                
            case FRESHIFT_WIRELESS_WIDEBAND_LOCAL_2G:       
            case OPTICAL_FRESELT_FAR_2G:                    
            case OPTICAL_CARRIER_WIDEBAND_LOCAL_2G:         
            case OPTICAL_CARRIER_FRESELT_LOCAL_2G:          
            case OPTICAL_CARRIER_WIDEBAND_FAR_2G:           
            case OPTICAL_CARRIER_FRESELT_FAR_2G:            
            case OPTICAL_WIRELESSCOUPL_LOCAL_2G:            
            case FRESHIFT_WIDEBAND_FAR_2G:                  
            case FRESHIFT_DIRECOUPL_FRESELT_LOCAL_2G:       
            case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL_2G:   
            case BS_AMPLIFIER_2G:
            case MACHINE_FOR_TEST:
            case (WIRELESS_FRESELT_2G + DEV_WITH_SEC):
            //--------2g����--------//
                *(UCHAR *)ulParamAddr = pstMObj->aucMObjContent[0];
                SET_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_DEV_TYPE);
                break;
            default :          //�������ô���
                SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_VALUE_OUT_OF_BOUND, lRspFlag);
                return  lRspFlag; 
            }
        }

        break;
    default:
        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, lRspFlag);
        return  lRspFlag; 
    }
    return lRspFlag;
}


/*********************************************************************************
  Function:
  Description:    ����������MCP:A�е���Ƶ���أ���������Ƶ����ʱ��Ҫ���ñ�־��
                  ��������������������Ӳ������
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ������� 
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpARfSw(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                      UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //����Ϊ�ж������������Ͳ����д���
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //��ѯ,�洢��������
        pstMObj->aucMObjContent[0] = *(UCHAR *)ulParamAddr;
        break;
    case MCPA_CMD_ID_SET: //����,�洢��д����
        *(UCHAR *)ulParamAddr = pstMObj->aucMObjContent[0];
        SET_BITMAP(g_usSettingParamBitmap, BITMAP_RF_SW);
        break;
    default: 
        //�����ܽ��������֧����Ϊ����֮ǰ�Ѿ�������������ж�
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    ����������MCP:A�е��ŵ��������������ŵ�ʱ��Ҫ���ñ�־��
                  ��������������������Ӳ������
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ������� 
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpAChNum(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                     UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //����Ϊ�ж������������Ͳ����д���
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //��ѯ,�洢��������
        //*(USHORT *)pstMObj->aucMObjContent = ADJUST_WORD(*(USHORT *)ulParamAddr);
        SET_WORD(pstMObj->aucMObjContent, GET_WORD(ulParamAddr));
        break;
    case MCPA_CMD_ID_SET: //����,�洢��д����
        //*(USHORT *)ulParamAddr = ADJUST_WORD(*(USHORT *)pstMObj->aucMObjContent);
        SET_WORD(ulParamAddr, GET_WORD(pstMObj->aucMObjContent));
        SET_BITMAP(g_usSettingParamBitmap, BITMAP_CH_NUM);
        break;
    default: 
        //�����ܽ��������֧����Ϊ����֮ǰ�Ѿ�������������ж�
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    ����������MCP:A�е�˥�ģ�������˥��ʱ��Ҫ���ñ�־��
                  ��������������������Ӳ������
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ������� 
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpAAtt(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                   UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //����Ϊ�ж������������Ͳ����д���
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //��ѯ,�洢��������
        pstMObj->aucMObjContent[0] = *(UCHAR *)ulParamAddr;
        break;
    case MCPA_CMD_ID_SET: //����,�洢��д����
        *(UCHAR *)ulParamAddr = pstMObj->aucMObjContent[0];
        SET_BITMAP(g_usSettingParamBitmap, BITMAP_ATT);
        break;
    default: 
        //�����ܽ��������֧����Ϊ����֮ǰ�Ѿ�������������ж�
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    ����������MCP:A�еĶ���Ϣ���ĺ��룬�����ö�С�����ĺ���ʱ
                  ��Ҫ���ñ�־�����������������������CH�����õ�MODEM��
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ������� 
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpASmcAddr(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                       UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //����Ϊ�ж������������Ͳ����д���
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //��ѯ,�洢��������
        memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    case MCPA_CMD_ID_SET: //����,�洢��д����
        memcpy((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        SET_BITMAP(g_usSettingParamBitmap, BITMAP_SMC_ADDR);
        break;
    default: 
        //�����ܽ��������֧����Ϊ����֮ǰ�Ѿ�������������ж�
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    ����������MCP:A�е�IP��ַ��������IP��ַ�Ҹı�ʱ
                  ��Ҫ���ñ�־�����������������������Ǵ���GPRS��ʽ���
                  ���µ�¼
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ������� 
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpAIpAddr(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                      UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //����Ϊ�ж������������Ͳ����д���
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //��ѯ,�洢��������
        memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    case MCPA_CMD_ID_SET: //����,�洢��д����
        if(memcmp((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN))
        {
            SET_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_IP_ADDR); //��������仯���ñ�־
        }
        memcpy((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    default: 
        //�����ܽ��������֧����Ϊ����֮ǰ�Ѿ�������������ж�
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    ����������MCP:A�еĶ˿ںţ������ö˿ں��Ҹı�ʱ
                  ��Ҫ���ñ�־�����������������������Ǵ���GPRS��ʽ���
                  ���µ�¼
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ������� 
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpAPortNum(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                       UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //����Ϊ�ж������������Ͳ����д���
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //��ѯ,�洢��������
        memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    case MCPA_CMD_ID_SET: //����,�洢��д����
        if(memcmp((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN))
        {
            SET_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_PORT_NUM); //��������仯���ñ�־
        }
        memcpy((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    default: 
        //�����ܽ��������֧����Ϊ����֮ǰ�Ѿ�������������ж�
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    ����������MCP:A�е��ϱ�ͨ�ŷ�ʽ���������ϱ�ͨ�ŷ�ʽ�Ҹı�ʱ
                  ��Ҫ���ñ�־���������������������ͨ�ŷ�ʽ�л����д���
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ������� 
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpACommMode(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                        UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //����Ϊ�ж������������Ͳ����д���
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //��ѯ,�洢��������
        memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    case MCPA_CMD_ID_SET: //����,�洢��д����
        if(memcmp((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN))
        {
            //����Ƕ��ϱ�ͨ�ŵ���������Ҫӳ�䵽ͨ�ŷ�ʽ����
            if(pstMObj->aucMObjId[LO_BYTE] == MOBJ_ID_NM_REPORT_COMM_MODE)
            {
                switch(pstMObj->aucMObjContent[0])
                {
                case COMM_REPORT_MODE_GPRS:
                    g_stNmParamSet.ucCommMode = COMM_MODE_GPRS;
                    break;
                case COMM_REPORT_MODE_SMS:
                    g_stNmParamSet.ucCommMode = COMM_MODE_SMS;
                    break;
                case COMM_REPORT_MODE_CSD:
                    g_stNmParamSet.ucCommMode = COMM_MODE_CSD;
                    break;
                default:
                    break;
                }
            }
            
            SET_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_COMM_MODE); //��������仯���ñ�־
        }
        memcpy((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    default: 
        //�����ܽ��������֧����Ϊ����֮ǰ�Ѿ�������������ж�
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    ����������MCP:A�е����ں�ʱ�䣬��ѯֱ�Ӵ�RTC�ж�ȡ������ֱ��
                  ���õ�RTC��
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ������� 
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpADateTime(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                        UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //����Ϊ�ж������������Ͳ����д���
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //��ѯRTC
        RtcGetDateTime((UCHAR *)ulParamAddr);
        memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    case MCPA_CMD_ID_SET: //����RTC
        memcpy((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        if(RtcSetDateTime((UCHAR *)ulParamAddr) == FAILED)
        {
            lRspFlag = MOBJ_OTHER_ERR;
        }
        break;
    default: 
        //�����ܽ��������֧����Ϊ����֮ǰ�Ѿ�������������ж�
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    ����������Ҫ���͵�TDͬ��ģ��Ĳ��������临��
                  ��ȫ�ֻ�������
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ���˴���Ч
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ������� 
  Output:         g_stTDParam:  ���ڱ������ڱ������TDͬ��ģ�����
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleAsynOpTDParam(MCPA_MOBJ_ST *pstMObj, ULONG, UCHAR ucDataType, 
                         UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    g_stTDParam.ucOpType = ucOperation;

    //����TDͬ��ģ��ļ�ز���
    memcpy(g_stTDParam.aucParam + g_stTDParam.usParamLen, pstMObj, pstMObj->ucMObjLen);
    g_stTDParam.usParamLen += pstMObj->ucMObjLen;
    
    //��Ҫ�ȴ�Ӧ��Ĳ�����ʱ��Ϊ�����ڻ��������Ӧ�������������־
    SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_OTHER_ERR, lRspFlag); //�˴���lRspFlag��������

    return MOBJ_CORRECT;
}

/*********************************************************************************
  Function:
  Description:    ����������Ҫ���͵�̫���ܿ������Ĳ��������临��
                  ��ȫ�ֻ�������
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ���˴���Ч
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ������� 
  Output:         g_stTDParam:  ���ڱ������ڱ������TDͬ��ģ�����
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleAsynOpSECParam(MCPA_MOBJ_ST *pstMObj, ULONG, UCHAR ucDataType, 
                          UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    g_stSECParam.ucOpType = ucOperation;

    //����̫���ܿ������ļ�ز���
    memcpy(g_stSECParam.aucParam + g_stSECParam.usParamLen, pstMObj, pstMObj->ucMObjLen);
    g_stSECParam.usParamLen += pstMObj->ucMObjLen;
    
    //��Ҫ�ȴ�Ӧ��Ĳ�����ʱ��Ϊ�����ڻ��������Ӧ�������������־
    SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_OTHER_ERR, lRspFlag); //�˴���lRspFlag��������

    return MOBJ_CORRECT;
}

//zhangjie 20071120 ̫�������ص�ѹ
/*********************************************************************************
  Function:
  Description:    ��������̫�������ص�ѹ��ѯ���������ݻ��Ͳ�ͬȷ�������õĺ�����
                  ��Ϊ̫���ܿ���������ʱʹ���첽������������Ĭ�ϸò�����֧����ͨ����
                  �صõ�����
  Calls:
  Called By:
  Input:          pstMObj:      MCP������ָ��  
                  ulParamAddr:  ��ض���洢��ַ 
                  ucDataType:   ��ض�����������
                  ucIsSupported:��ض����Ƿ�֧��
                  ucOperation:  ����Ĳ�������  
  Output:         
  Return:         rspFlag����ض���Ĵ����Ƿ����������
  Others:  
*********************************************************************************/
LONG HandleMcpASecVolR(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType,
                       UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag;
    if(g_stDevInfoSet.ucDevType == WIRELESS_FRESELT_2G + DEV_WITH_SEC)//̫���ܿ���������
    {
        lRspFlag = HandleAsynOpSECParam(pstMObj, ulParamAddr, ucDataType, 
                          ucIsSupported, ucOperation);
    }
	else//�������;�Ĭ��֧�ָò���
	{
	    lRspFlag = HandleMcpANormalR(pstMObj, ulParamAddr, ucDataType,
                       PARAM_SUPPORTED, ucOperation);
	}

    return lRspFlag;
}
//zhangjie 20071120 ̫�������ص�ѹ

/*********************************************************************************
  Function:
  Description:    ��������CH�����Ķ������ĺ��뱣�浽ȫ�ֱ����У���ȡ�������ĺ����
                  ������ÿ��ϵͳ���������
  Calls:
  Called By:
  Input:          pstPrmv: ���ض������Ľ����ԭ��
  Output:         
  Return:         ��ȡ�Ľ����SUCCEEDED��ʾ�ɹ���FAILED��ʾʧ��
  Others:  
*********************************************************************************/
LONG RetrieveSmcAddr(OHCH_RW_SMC_ADDR_CNF_ST *pstPrmv)
{
    if(pstPrmv->lResult == SUCCEEDED)
    {
        memcpy(g_stNmParamSet.acSmcAddr, pstPrmv->acTelNum, sizeof(g_stNmParamSet.acSmcAddr));
    }
    return pstPrmv->lResult;
}


/*************************************************
  Function:
  Description:    �������ϱ�ͨ����·���ϣ���3�γ��Խ�����
                  ������ĵ�����ʧ�ܺ��ϱ�������Ҫ�ж��Ƿ�
                  �����յ��ϱ���Ӧ����˲��÷���������
  Calls:
  Called By:
  Input:          
  Output:
  Return:         SUCCEED��ʾ�ɹ��������ʾʧ��
  Others:         ���������ⲿ����
*************************************************/
LONG ReportCommLinkFault()
{
    ULONG ulLen = 0;
    UCHAR *pucBuf = NULL;
    UCHAR *pucRawApData = NULL;
    OHCH_SEND_DATA_REQ_ST *pstSend = NULL;

    //���ϱ�ʹ�ö��ŷ�ʽ�����ж��Ƿ������ϱ�
    
    //��������
    USHORT usPacketID = GEN_PACKET_ID();

    //�����ڴ�
    pucBuf = (UCHAR *)MyMalloc(g_pstMemPool256);
    if(pucBuf == 0)
    {
        return FAILED;
    }

    //�ȹ���ԭ��
    pstSend = (OHCH_SEND_DATA_REQ_ST *)pucBuf;
    pstSend->ulPrmvType = OHCH_SEND_DATA_REQ;
    pstSend->ulMagicNum = usPacketID; //ʹ��packet ID��Ϊmagic number
    pstSend->ucResendTime = RESEND_TIME_GPRS_LINKFAULT;//GPRS���ӹ����ϱ�����ϱ�ʧ�ܺ��ظ�����
    pstSend->ucDstIf    = COMM_IF_NORTH;
    pstSend->ucCommMode = COMM_MODE_SMS; //���ϱ��̶�ʹ�ö��ŷ�ʽ
    if(pstSend->ucCommMode == COMM_MODE_SMS)
    {
        pstSend->ucDCS      = 0; //��ʱĬ��ΪGSM 7 bit���룬����Ƕ���Ϣ��ʽ�Ļ�
        memcpy(pstSend->acDstTelNum, g_stNmParamSet.acReportTelNum, sizeof(pstSend->acDstTelNum));
    }
    pstSend->ucDataType = REPORT_TYPE_COMM_LINK_FAULT; //DATA_TYPE_REPORT;
    
    pstSend->pucData    = pucBuf + DATA_START_POS; //Ϊ����߱���Ч�ʣ����ڴ��ǰ�����㹻ԭ�����Ŀռ�

    pucRawApData = pstSend->pucData + 1; //��������ʼ��־�ĵط� 
    
    ulLen = EncodeReportIncludeReportTypeOnly(pucRawApData, 
                                              REPORT_TYPE_COMM_LINK_FAULT, 
                                              COMM_MODE_SMS,
                                              usPacketID);

    //�����ϱ���AP�㴦��
    ulLen = EncodeApData(pucRawApData, ulLen, 
                         ((AP_LAYER_ST *)pucRawApData)->ucApType, MEM_BLK_SIZE_256 - 4 - sizeof(OHCH_SEND_DATA_REQ_ST));

    pstSend->ulDataLen  = ulLen;
    
    //��ԭ�﷢�͸�CH
    if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
    {
        MyFree((void * )pstSend);
        return FAILED;
    }
    return SUCCEEDED;
}

/*************************************************
  Function:
  Description:    ����������GPRS���ӵ�ȷ�ϣ��������ʧ��
                  ����Ҫ������ÿʧ��3�ξ��ϱ�һ��ͨ����
                  ·���ϡ�����������GPRS���Ӳ�����ζ�Ž�
                  ��GPRS���ӣ�����ζ�Ž���TCP���ӣ���Ϊ
                  ��Щ������MODEM���
  Calls:
  Called By:
  Input:          pstPrmv: GPRS����ȷ��
  Output:
  Return:         
  Others:         ���������ⲿ����
*************************************************/
void HndlOHCHConnCenterCnf(OHCH_CONN_CENTER_CNF_ST *pstPrmv)
{
    if(pstPrmv->lResult == SUCCEEDED)
    {
        g_stCenterConnStatus.ucLinkStatus = COMM_STATUS_CONNECTED;
        g_stCenterConnStatus.ucConnAttempCnt = 0;
        
        //ǡ�����ĸı�ͨ�ŷ�ʽ
        if(g_stNmParamSet.ucCommMode != COMM_MODE_GPRS)
        {
            DisconnFromCenter();
        }
        else
        {
            //���е�¼
            ReportOtherEvent(REPORT_TYPE_LOGIN, 0);
        }
    }
    else if(pstPrmv->lResult == RESULT_STOP_REQ) //��B��ͨ�ŷ�ʽ�����ó�GPRS��Ҫ��¼ʱ���յ����cause
    {
        //do nothing
    }
    else //��Ҫ�������ԣ���ÿ3��ʧ���ϱ�һ��ͨ����·����
    {
        if(g_stNmParamSet.ucCommMode == COMM_MODE_GPRS)
        {
            g_stCenterConnStatus.ucLinkStatus = COMM_STATUS_DISCONNECTED;
            if(g_stCenterConnStatus.ucConnAttempCnt >= MAX_CONN_ATTEMP_CNT)
            {
                DisconnFromCenter(); //��ֹGPRS�����������ĵ���TCP���ӽ��������������
                ReportCommLinkFault();
                g_stCenterConnStatus.ucConnAttempCnt = 0;
            }
            ConnToCenter(); //CH��Ҫ���Ƕ���δ���ͳɹ����յ�������GPRS���ӵĴ���
        }
    }
}

/*************************************************
  Function:
  Description:    ����������CSD����ָʾ���������е�����
                  ������м�Ȩ�����û��������ʾ����CH
                  ��ʹ�ø�ԭ��֪ͨOH
  Calls:
  Called By:
  Input:          pstPrmv: CSD����ָʾ
  Output:
  Return:         
  Others:         ���������ⲿ����
*************************************************/
void HndlOHCHCsdConnInd(OHCH_CSD_CONN_IND_ST *pstPrmv)
{
    if(g_stYkppParamSet.stYkppCtrlParam.ucTelNumAuthSw != 0) //�ж��Ƿ���е绰�����Ȩ
    {
        if((ValidateTelNum(pstPrmv->acTelNum, g_stNmParamSet.acQnsTelNum1) != 0)
        && (ValidateTelNum(pstPrmv->acTelNum, g_stNmParamSet.acQnsTelNum2) != 0)
        && (ValidateTelNum(pstPrmv->acTelNum, g_stNmParamSet.acQnsTelNum3) != 0)
        && (ValidateTelNum(pstPrmv->acTelNum, g_stNmParamSet.acQnsTelNum4) != 0)
        && (ValidateTelNum(pstPrmv->acTelNum, g_stNmParamSet.acQnsTelNum5) != 0))
        {
            pstPrmv->lResult = FAILED;
        }
        else
        {
            pstPrmv->lResult = SUCCEEDED;
        }
    }
    else
    {
        pstPrmv->lResult = SUCCEEDED;
    }
    
    pstPrmv->ulPrmvType = OHCH_CSD_CONN_RSP;

    if(OSQPost(g_pstCHNthQue, pstPrmv) != OS_NO_ERR)
    {
        MyFree((void * )pstPrmv);
        return;
    }
}

/*************************************************
  Function:
  Description:    �����������ɴӻ����������ݣ���ת��������ӿڣ�
                  �������ı���Ҫת�������ݣ�ͬʱ���䵱���Լ���
                  Ӧ�����ݴ���
  Calls:
  Called By:
  Input:          pstPrmv: �ӻ�����������
  Output:
  Return:         
  Others:         ���������ⲿ����
*************************************************/
void HndlOHCHSendDataReq(OHCH_SEND_DATA_REQ_ST *pstPrmv)
{
    //������ϱ���Ҫ���ж��Ƿ���Է���
    if(DATA_TYPE_RSP != pstPrmv->ucDataType)
    {
        pstPrmv->ucResendTime = RESEND_TIME_DATATYPE_ALARM;
        if(IsReportAllowed(pstPrmv->ucDataType) == FALSE)
        {
            MyFree(pstPrmv);
            return;
        }
    }
    else 
    {
        pstPrmv->ucResendTime = RESEND_TIME_DATATYPE_RSP;
    }
    
    //����OHCH_SLAVE_SENDDATA_IND_ST��OHCH_SEND_DATA_REQ_ST�ṹ��ͬ�����ֱ�����������
    pstPrmv->ulPrmvType   = OHCH_SEND_DATA_REQ;
    pstPrmv->ulMagicNum   = 0xFFFFFFFF; //�˴�MagicNum���ܻ�ͱ����ϱ�������ͻ�����ǳ�ͻ�Ŀ����Էǳ�С
    pstPrmv->ucDstIf      = COMM_IF_NORTH;

    //��ԭ�﷢�͸�CH
    if(OSQPost(g_pstCHNthQue, pstPrmv) != OS_NO_ERR)
    {
        MyFree((void * )pstPrmv);
        return;
    }    
}

/*************************************************
  Function:
  Description:    ������������������������Ӧ�����Ӧ�������
                  һ�£�������ӿڷ���Ӧ�����ݣ�����ȴ�ʣ��
                  ��Ӧ��
  Calls:
  Called By:
  Input:          pstMsg: ����������Ӧ����Ϣ����Ϣ�ڴ���ͷ���������
  Output:
  Return:         
  Others:         ���������ⲿ����
*************************************************/
void HndlMessQueryOrSetRsp(MESSAGE_ST *pstMsg)
{
    UCHAR ucIdx = 0;
    
    //������ϢSN���첽�����������в���
    for(; ucIdx < ASYN_OP_CTX_CNT; ucIdx++)
    {
        if(pstMsg->ucMsgSn == g_astAsynOpCtx[ucIdx].ucSn)
        {
            break;
        }
    }

    if(ucIdx >= ASYN_OP_CTX_CNT)
    {
        return;
    }

    //������Ӧ�����ݸ��µ������Ӧ����
    OHCH_RECV_DATA_IND_ST *pstPrmv = (OHCH_RECV_DATA_IND_ST *)g_astAsynOpCtx[ucIdx].pvData;
    AP_LAYER_ST *pstApData  = (AP_LAYER_ST *)(pstPrmv->pucData + 1); //pstPrmv->pucDataָ����ʼ��־��δ��ɵ�Э��Ӧ�����ݣ���δ����AP�����
    VPA_LAYER_ST *pstVpData = (VPA_LAYER_ST *)pstApData->aucPdu;
    MCP_LAYER_ST *pstMcp    = (MCP_LAYER_ST *)pstVpData->aucPdu;

    LONG lMObjTotalLen = 0;       //Ӧ���еļ�ز���
    MCPA_MOBJ_ST *pstMObj = NULL; //Ӧ���еļ�ز����ĳ���

    MCPA_MOBJ_ST *pstParamInMsg = (MCPA_MOBJ_ST *)pstMsg->aucData; //��Ϣ�еļ�ز���
    LONG lParamLen = pstMsg->usDataLen;                            //��Ϣ�еļ�ز����ĳ���
    UCHAR ucParamInMsgMatched = false;                             //��Ϣ�еļ�ز����ͱ��������ƥ��

    while(lParamLen >= pstParamInMsg->ucMObjLen) //��������������Ϣ�����еĲ���
    {
        if(pstParamInMsg->ucMObjLen < MCPA_MOBJ_MIN_LEN)
        {
            pstMcp->ucRspFlag = RSP_FLAG_PARTIALLY_DONE;
            break;
        }
        
        lMObjTotalLen = (LONG)(pstPrmv->ulDataLen - 2 - AP_OVERHEAD - VP_OVERHEAD - MCP_MIN_LEN); //2��ʾ��ʼ������־����
        pstMObj = (MCPA_MOBJ_ST *)(pstMcp->aucContent);

        while(lMObjTotalLen >= pstMObj->ucMObjLen) //���������Ӧ���еĲ���
        {
            if(((pstMObj->aucMObjId[HI_BYTE] & 0xF) == (pstParamInMsg->aucMObjId[HI_BYTE] & 0xF))
             &&((pstMObj->aucMObjId[LO_BYTE]) == (pstParamInMsg->aucMObjId[LO_BYTE])))
            {
                memcpy(pstMObj, pstParamInMsg, pstMObj->ucMObjLen);

                if((pstParamInMsg->aucMObjId[HI_BYTE] & 0xF0) != 0) //�жϸò����Ƿ���ڴ����������Ӧ���־
                {
                     pstMcp->ucRspFlag = RSP_FLAG_PARTIALLY_DONE;
                }
                ucParamInMsgMatched = true;
                break;
            }
            else
            {
                lMObjTotalLen -= pstMObj->ucMObjLen; //ƥ����һ������
                pstMObj = (MCPA_MOBJ_ST *)(((UCHAR *)pstMObj) + pstMObj->ucMObjLen);
            }
        }

        //����Ǹ澯������Ҫͬ��ʵʱ�澯״̬�����ĸ澯״̬������״̬����AC�б�ʵʱ
        //�澯״̬��ֵ�����������ж���״̬�Ƿ���з�ת������������Ҫ������һ�澯
        //�Ƿ��ǲ�ѯ����������ģ��������˵���������ܴ������󣬴�ʱ���Ը��±��ظ�
        //��״̬�����ǲ��ܸ������ĸ澯״̬
        if((pstParamInMsg->aucMObjId[HI_BYTE] & 0xF) == MOBJ_ID_ALARM_STATUS_SET)
        {
            for(ULONG j = 0; j < MAX_ALARM_ITEMS; j++)
            {
                if(g_uAlarmItems.astAlarmItemArray[j].ucIDLoByte
                == pstParamInMsg->aucMObjId[LO_BYTE])
                {
                    if(pstParamInMsg->aucMObjContent[0])
                    {
                    #ifndef M3
                        g_uAlarmItems.astAlarmItemArray[j].ucRcStatus = 1;
                    #else
                        g_uAlarmItems.astAlarmItemArray[j].ucLocalStatus = 1;
                    #endif
                        if(ucParamInMsgMatched)
                        {
                            g_uCenterStatus.aucAlarmStatusArray[j] = 1;
                        }
                    }
                    else
                    {
                    #ifndef M3
                        g_uAlarmItems.astAlarmItemArray[j].ucRcStatus = 0;
                    #else
                        g_uAlarmItems.astAlarmItemArray[j].ucLocalStatus = 0;
                    #endif
                        if(ucParamInMsgMatched)
                        {
                            g_uCenterStatus.aucAlarmStatusArray[j] = 0;
                        }    
                    }

                    break;
                }
            }
        }

        lParamLen -= pstParamInMsg->ucMObjLen; //ƥ����һ������
        pstParamInMsg = (MCPA_MOBJ_ST *)(((UCHAR *)pstParamInMsg) + pstParamInMsg->ucMObjLen);
        ucParamInMsgMatched = false;
    }

    //�����������״̬������Ҫ���浽NV Memory��
    SaveAlarmCenterStatus();

    //�п��ܻ���������ظ���Ӧ��Ĳ������ڸ�����������Ĳ�������ʱû�а취����Ӧ����������֤

    //���ݲ����������λͼ
    g_astAsynOpCtx[ucIdx].ucObjBitmap &= ~pstMsg->ucObj;

    if(g_astAsynOpCtx[ucIdx].ucObjBitmap)
    {
        return; //��������δ����Ӧ�𣬼����ȴ�
    }

    //ֹͣ��ʱ��
    if(g_astAsynOpCtx[ucIdx].pTimer)
    {
        OHRemoveTimer(g_astAsynOpCtx[ucIdx].pTimer, g_astAsynOpCtx[ucIdx].ulTimerMagicNum);
        g_astAsynOpCtx[ucIdx].pTimer = NULL;
    }
    g_astAsynOpCtx[ucIdx].ucState = OH_IDLE;

    EncodeAndResponse(pstPrmv);
    g_astAsynOpCtx[ucIdx].pvData = NULL; //Ϊ�˷�ֹ���շ���δ�յ����ݣ��ڴ�ͱ�ClearAsynOpCtx()�ͷŵ���
                                         //�������ʧ�����Ѿ����ڲ��ͷţ�����Ҫ��ClearAsynOpCtx()�ͷ�

    //��ʼ��������
    ClearAsynOpCtx(ucIdx);
}

/*************************************************
  Function:
  Description:    �����������������������ĸ澯�������澯����
                  �����ظ澯������
  Calls:
  Called By:
  Input:          pstMsg: ����������Ӧ����Ϣ����Ϣ�ڴ���ͷ���������
  Output:
  Return:         
  Others:         ���������ⲿ����
*************************************************/
void HndlMessageAlarmRep(MESSAGE_ST *pstMsg)
{
    LONG i, j;

    //�ֽ����������澯��ʵʱ״̬����Ϊ����
    //�˴����ڸ澯�ɼ��������ڸ澯�жϲ����Ľ������ֻ����ʵʱ״̬
    for(i = MOBJ_ID_A_SEC_ST_BAT_BLOWOUT; i <= MOBJ_ID_A_SEC_ALEAK; i++)
    {
        for(j = 0; j < MAX_ALARM_ITEMS; j++)
        {
            if(g_uAlarmItems.astAlarmItemArray[j].ucIDLoByte == i)
            {
            #ifndef M3
                g_uAlarmItems.astAlarmItemArray[j].ucRcStatus = 0;
            #else
                g_uAlarmItems.astAlarmItemArray[j].ucLocalStatus = 0;
            #endif
            }
        }
    }

    //��������Ϣ�еĸ澯����µ�ʵʱ�澯״̬��
    MCPA_MOBJ_ST *pstAlarmItem = (MCPA_MOBJ_ST *)pstMsg->aucData;
    SHORT sDataLen = (SHORT)pstMsg->usDataLen;
    while(sDataLen >= MOBJ_ALARM_LEN)
    {
        for(i = 0; i < MAX_ALARM_ITEMS; i++) //��ʼ���Դӳ��Ҷ���ĸ澯�ʼ���ң����ٲ��Ҵ���
        {
            if((pstAlarmItem->aucMObjId[HI_BYTE] == MOBJ_ID_ALARM_STATUS_SET)
            && (pstAlarmItem->aucMObjId[LO_BYTE] == g_uAlarmItems.astAlarmItemArray[i].ucIDLoByte))
            {
                if(pstAlarmItem->aucMObjContent[0])
                {
                #ifndef M3
                    g_uAlarmItems.astAlarmItemArray[i].ucRcStatus = 1;
                #else
                    g_uAlarmItems.astAlarmItemArray[i].ucLocalStatus = 1;
                #endif
                }
                break;
            }
        }
        pstAlarmItem = (MCPA_MOBJ_ST *)((UCHAR *)pstAlarmItem + MOBJ_ALARM_LEN);
        sDataLen -= MOBJ_ALARM_LEN;
    }
}

/*************************************************
  Function:
  Description:    �������������ƶ�Э�鴥�����첽��
                  ��ʱ���������ʱ�󽫱�����Ѵ�
                  ���Ӧ����ͣ�ͬʱ��Ӧ���־��Ϊ
                  ���ִ��󣬼�ز����Ĵ��������֮
                  ǰ�Ѿ���д
  Calls:
  Called By:
  Input:          pvCtx: ԭ�������������ָ�룬ʵ��ָ���Ӧ���첽����������
  Output:         
  Return:         
  Others:
*************************************************/
void AsynOpToCommon(void *pvCtx)
{
    ASYN_OP_CTX_ST *pstCtx = (ASYN_OP_CTX_ST *)pvCtx;

    //�������Ӧ������ȡ������
    OHCH_RECV_DATA_IND_ST *pstPrmv = (OHCH_RECV_DATA_IND_ST *)pstCtx->pvData;    
    AP_LAYER_ST *pstApData   = (AP_LAYER_ST *)(pstPrmv->pucData + 1); //pstPrmv->pucDataָ����ʼ��־��δ��ɵ�Э��Ӧ�����ݣ���δ����AP�����
    VPA_LAYER_ST *pstVpData  = (VPA_LAYER_ST *)pstApData->aucPdu;
    MCP_LAYER_ST *pstMcpData = (MCP_LAYER_ST *)pstVpData->aucPdu;

    //����MCPӦ���־
    pstMcpData->ucRspFlag = RSP_FLAG_PARTIALLY_DONE;
    EncodeAndResponse(pstPrmv);
    pstCtx->pvData = NULL; //Ϊ�˷�ֹ���շ���δ�յ����ݣ��ڴ�ͱ�ClearAsynOpCtx()�ͷŵ���
                           //�������ʧ�����Ѿ����ڲ��ͷţ�����Ҫ��ClearAsynOpCtx()�ͷ�

    //��ʼ�������ģ��˴�ʹ�������ṹ��ַ����������
    ClearAsynOpCtx(pstCtx - (ASYN_OP_CTX_ST *)&g_astAsynOpCtx);
}


/*********************************************************************************
  Function:
  Description:    ����������ǳ�ʼ���׶�CH��д�������ĺ���Ľ�������������ȷ���򽫴�
                  ���浽ȫ�ֱ�����,���ö�ȡ�Ķ������ĺ�����»��浥Ԫ���������������
                  ����޸�Ϊ��ȷ�������ȡʧ�ܣ��򽫳�����������޸�Ϊ��������
                  ���������ݣ�������ʵ����������Ƿ�رն�ʱ����
  Calls:
  Called By:
  Input:          pstPrmv: ���ض������Ľ����ԭ��
  Output:         
  Return:         
  Others:  
*********************************************************************************/
void HandleRWSmcAddrRsp(OHCH_RW_SMC_ADDR_CNF_ST *pstPrmv)
{
    //�ҵ��������ĺ�������ĵ����������
    UCHAR ucIdx = 0;
    for(; ucIdx < ASYN_OP_CTX_CNT; ucIdx++)
    {
        if(pstPrmv->ucMsgSn == g_astAsynOpCtx[ucIdx].ucSn)
        {
            break;
        }            
    }
    
    if(ucIdx < ASYN_OP_CTX_CNT)        //�������Ӧ������ȡ������
    {
        OHCH_RECV_DATA_IND_ST *pstBufPrmv = (OHCH_RECV_DATA_IND_ST *)g_astAsynOpCtx[ucIdx].pvData;   
        YKPP_PDU_ST * pstYkPduData = (YKPP_PDU_ST *)(pstBufPrmv->pucData + 1); //������ʼ��־

        if(pstPrmv->lResult == SUCCEEDED)            
        {
            //����Ӧ���־Ϊ��ȷ
            pstYkPduData->ucResult = SUCCEEDED;
            
            //����ȡ�Ķ������ĺ���������Ӧ�ڴ浥Ԫ
            //memcpy(g_stNmParamSet.acSmcAddr, pstPrmv->acTelNum, sizeof(g_stNmParamSet.acSmcAddr));
            
            //�û�Ӧ���ݸ��»���
            memcpy(pstYkPduData->aucParamTbl + 2, pstPrmv->acTelNum,
                   sizeof(g_stNmParamSet.acSmcAddr));
        }
        else
        {
            pstYkPduData->ucResult = YKPP_ERR_OPER_FAILED;
        }

        //�����水��Э���ʽ���
        EncodeAndResponseYkppBuf(pstBufPrmv);
        g_astAsynOpCtx[ucIdx].pvData = NULL;
        
        g_astAsynOpCtx[ucIdx].ucObjBitmap &= ~ASYN_OP_OBJ_SMC;

        if(g_astAsynOpCtx[ucIdx].ucObjBitmap)
        {
            return; //��������δ����Ӧ�𣬼����ȴ�
        }
        //ֹͣ��ʱ��
        if(g_astAsynOpCtx[ucIdx].pTimer)
        {
            OHRemoveTimer(g_astAsynOpCtx[ucIdx].pTimer, g_astAsynOpCtx[ucIdx].ulTimerMagicNum);
            g_astAsynOpCtx[ucIdx].pTimer = NULL;
        }
        g_astAsynOpCtx[ucIdx].ucState = OH_IDLE;

        //��ʼ��������
        ClearAsynOpCtx(ucIdx);            
    }
    return;
}

//MCM-32_20061114_zhonghw_begin
/*********************************************************************************
  Function:
  Description:   ����������������汾�л���������³�������еĳ����¾�״̬
  Calls:
  Called By:
  Input:          
  Output:         
  Return:         
  Others:  
*********************************************************************************/
LONG SwitchFileVer()
{
    UCHAR i = 0;
    
    //ȷ���Ƿ������׿����г���
    for(i = 0;i<2;i++)
    {
        if(g_astFilePartTbl[i].ucStatus != PROGRAM_STATUS_RUNNABLE)
        {
            return FAILED;
        } 
    }

    //���жϿ��Թ��ucIsRunning������1�����    
    if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucIsRunning == 1)
    {
        g_astFilePartTbl[PROGRAM_PART_1_IDX].ucNew = FILE_OLD;
        g_astFilePartTbl[PROGRAM_PART_2_IDX].ucNew = FILE_NEW;            
    }
    else
    {
        g_astFilePartTbl[PROGRAM_PART_2_IDX].ucNew = FILE_OLD;
        g_astFilePartTbl[PROGRAM_PART_1_IDX].ucNew = FILE_NEW;                        
    }
    
    return SUCCEEDED;
}
/*********************************************************************************
  Function:
  Description:   �ú����������ݵ�Ԫ�г��Ҵ��뼰�豸����Ȩ�Ƿ���ȷ��
                 �����ȷ����ð汾�л�����,����bitmap��Ӧ��־λ
  Calls:
  Called By:
  Input:          
  Output:         
  Return:        SUCCEEDED:��ʾ������ȷ������������Ϊ���ȴ����µĴ���ʧ��
                 FAILED   :��ʾ��Ϊ������������Ĵ���ʧ��

  Others:   ע��:
                 ���������жϺ�������Ϊʧ��ʱ��ͳһ��MCPӦ���־��Ϊ"��������"
                 �൱���л��������汾�Ĵ����У�����MCPӦ���־�����¼��ֽ��:
                 1.��ȷ����   2.���ȴ���   3.��������
  
*********************************************************************************/
LONG HandleSwitchSofVer(MCP_LAYER_ST * pstMcp,LONG lMcpDataLen)
{
    MCPA_MOBJ_ST *pstMObj        = NULL;
    UCHAR ucSwitchVerMnftAuth    = 0;        //��Ȩͨ����־��0��ʾδͨ����1��ʾͨ��
    UCHAR ucSwitchVerDevTypeAuth = 0;        //��Ȩͨ����־��0��ʾδͨ����1��ʾͨ��

    //�����ж�
    if(lMcpDataLen != MCPA_DATA_SWITCH_VER_LEN)//ȷ��������ֻ��������ID
    {
        pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR;
        return SUCCEEDED;
    }
    
    lMcpDataLen -= 2;   //�۳����Ԫ���� 
    pstMObj = (MCPA_MOBJ_ST *)pstMcp->aucContent;
        
    while(lMcpDataLen >= MCPA_MOBJ_SWITCH_VER_MIN_LEN)  
    {
        if(pstMObj->ucMObjLen != MCPA_MOBJ_SWITCH_VER_MIN_LEN)
        {
            pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR; 
            return SUCCEEDED;
        }

        if(pstMObj->ucMObjLen > lMcpDataLen)
        {
            pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR; 
            return SUCCEEDED;
        }
        
        if(pstMObj->aucMObjId[HI_BYTE] == MOBJ_ID_DEV_INFO_SET)
        {
            if(g_astDevInfoHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucIsSupported)                
            {    
                switch(pstMObj->aucMObjId[LO_BYTE])
                {
                case MOBJ_ID_DI_MNFT_ID:
                    if(pstMObj->aucMObjContent[0] == g_stDevInfoSet.ucMnftId)
                    {
                        ucSwitchVerMnftAuth = 1;    //���Ҵ����Ȩͨ��
                    }
                    else
                    {
                        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_OTHER_ERR, pstMcp->ucRspFlag);
                    }
                    break;
                case MOBJ_ID_DI_DEV_TYPE:
                    if(pstMObj->aucMObjContent[0] == g_stDevInfoSet.ucDevType)
                    {
                        ucSwitchVerDevTypeAuth = 1;//�豸����Ȩͨ��
                    }
                    else
                    {
                        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_OTHER_ERR, pstMcp->ucRspFlag);
                    }
                    break;
                default:
                    //���ô˺�������ʵ pstMcp->ucRspFlag���������յ�ֵ
                    //�����ú������ֱ�����ΪFAILED��ͳһ������Ϊ"��������"                    
                    SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                    break;
                }
            }
            else
            {
                SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            }
        }
        else
        {
            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);        
        }
        
        lMcpDataLen -= pstMObj->ucMObjLen;
        pstMObj = (MCPA_MOBJ_ST *)(((UCHAR *)pstMObj) + pstMObj->ucMObjLen);
    }

    if((ucSwitchVerMnftAuth == 1)&&(ucSwitchVerDevTypeAuth == 1))
    {
        if(SwitchFileVer() != SUCCEEDED)
        {
            return FAILED;
        }
        
        SaveUpgradeInfo();
        SET_BITMAP(g_usSettingParamBitmap, BITMAP_SWITCH_FILE_VER);        
    }
    else
    {
        return FAILED;
    }
    
    return SUCCEEDED;
}
//MCM-32_20061114_zhonghw_end

