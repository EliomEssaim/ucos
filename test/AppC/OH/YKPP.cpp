/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   YKPP.cpp
    ����:     ����
    �ļ�˵��: ���ļ�����ʵ�ֶ�ʱ�����ƵĴ���
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  2006/02/16
       ����:  ����
       ����:  �������
    2. ����:  2006/11/01
       ����:  �½�
       ����:  �޸�����MCM-5��
              �ڳ����������ж�ͨ�ŷ�ʽ�����ϱ�ͨ�ŷ�ʽʱֻʹ��"ͨ�ŷ�ʽ"���ID
    3. ����:  2006/11/3
       ����:  �ӻ���
       ����:  �޸�����MCM-10,
              �ڴ������������첽�����Ĳ�����Ϊ��������������
              ����,��ע�᲻ͬ�Ĳ�������ĺ���������ʱ��������
    4. ����:  2006/11/07
       ����:  ����
       ����:  �޸�����MCM-20��
              ���Ӱ��������ϱ�����
    5. ����:  2006/11/07
       ����:  �ӻ���
       ����:  �޸�����MCM-28��
              ��������CMCC.cpp����YKPP.cpp    
    6. ����:  2007/01/11
       ����:  �ӻ���
       ����:  �޸�����MCM-54��
              ���������첽����ʱ��Ĭ�ϵĲ������Ϊ"YKPP_ERR_ASYN_BUF_FULL"��
              Ȼ����ݳ������е�ʵ�ʽ���޸Ĳ���������͡�
    7. ����:  2007/03/15
       ����:  �ӻ���
       ����:  �޸�����MCM-64��
              �����豸����Э��������ͨ�ŷ�ʽ�Ĳ�ѯ���ã�����������ֵ����Ӧ�ı�־λ
              ��ʾ�豸������ͨ�����á�
              ��"TakeYkppParamEffect"����������Ӧ��ͨ�������޸ĺ������豸�Ĵ���
---------------------------------------------------------------------------*/
#include <string.h>
#include "../Util/Util.h"
#include "../../Periph/flash/flash.h"
#include "../Include/Interface.h"
#include "../include/MyCfg.h"
//MCM-20_20061107_Linwei_begin
#include "../driver/key/key.h"
//MCM-20_20061107_Linwei_end
#include "CMCC_1_Ext.h"
#include "../AppMain.h"
#include "YKPP.h"
#include "OH.h"

extern NM_PARAM_SET_ST g_stNmParamSet; //�����ж��豸���
#define BITMAP_YKPP_UP_RF_SW             (1 << 0)
#define BITMAP_YKPP_DN_RF_SW             (1 << 1)
#define BITMAP_YKPP_MODIFY_STANUM_DEVNUM (1 << 2)
#define BITMAP_YKPP_STA_INIT             (1 << 3)
#define BITMAP_YKPP_INSPECTION           (1 << 4)
#define BITMAP_YKPP_REPAIR_CONFIRMED     (1 << 5)
#define BITMAP_YKPP_CONFIG_CHANGED       (1 << 6)
#define BITMAP_YKPP_MODIFY_SMC_ADDR      (1 << 7)

//MCM-64_20070315_Zhonghw_begin
#define BITMAP_YKPP_MODIFY_COMM_SETTING  (1 << 8)
//MCM-64_20070315_Zhonghw_end

//**************************ȫ�ֱ�������**************************//
YKPP_PARAM_SET_ST g_stYkppParamSet;

//MCM-64_20070315_Zhonghw_begin
USHORT g_usYkppSettingParamBitmap;
//MCM-64_20070315_Zhonghw_end
//�����첽������������
extern ASYN_OP_CTX_ST g_astAsynOpCtx[ASYN_OP_CTX_CNT];

//����ת����ַ���������ͬ�ƶ�Э��
typedef struct
{
    UCHAR ucEscape;
    UCHAR aucBeforeEscape[3];
    UCHAR aucAfterEscape[3];
}YKPP_ESCAPE_ST;

const YKPP_ESCAPE_ST g_stYkppEscape = 
{
    0x5E,
    {0x5E, YKPP_BEGIN_FLAG, YKPP_END_FLAG},
    {0x01, 0x02, 0x03}
};

//**************************�ⲿ��������**************************//
extern NM_PARAM_SET_ST  g_stNmParamSet;
extern CENTER_CONN_STATUS_ST g_stCenterConnStatus;
extern ALARM_ENABLE_UN g_uAlarmEnable;
//MCE-10 20061103_Zhonghw_Begin
extern ASYN_OP_SMALL_BUF_ST g_stSMCParam;
//MCE-10 20061103_Zhonghw_End
extern void SimpleFlashRead(ULONG ulFlashAddr, void *pvData, ULONG ulSize);
extern LONG SimpleFlashWrite(ULONG ulFlashAddr, void *pvData, ULONG ulSize);
extern LONG SimpleFlashEraseSector(ULONG ulFlashAddr);
extern LONG CmpData(ULONG ulFlashAddr, void *pvData, ULONG ulSize);
extern void ReconnToCenter();
extern LONG ReportOtherEvent(UCHAR ucEvent, UCHAR ucUpgradeResult);
extern "C" void I2cWriteSlow(void* address, void *arm_address, UCHAR count);
extern "C" void I2cReadMore(void* address, void *arm_address, UCHAR count);
extern void InitCmccDefaultParam();
extern INT8U MyFree(void * pv);
extern void *MyMalloc(OS_MEM * pmem);
void SaveWritableParam();
void InitYkppDefaultParam();
LONG GetAsynOpCtx(UCHAR & ucIdx);
void DoAsynOpSMC(void *pvCtx);
void AsynOpYkppToSmc(void * pvCtx);
//MCE-28 20061103_Zhonghw_Begin
void ClearAsynOpCtx(UCHAR ucIdx);
//MCE-28 20061103_Zhonghw_End
USHORT GetStackInfo(UCHAR *pucInfo, USHORT usMaxSize);
USHORT GetMemInfo(UCHAR *pucInfo, USHORT usMaxSize);
USHORT GetQueInfo(UCHAR *pucInfo, USHORT usMaxSize);

//MCM-64_20070315_Zhonghw_begin
extern "C" void Restart(void);
//MCM-64_20070315_Zhonghw_end
/*************************************************
  Function:
  Description:    �������Խ��յ���Э�����ݽ���ɨ�裬������Ч���ʿƳ���Э������
                  ��ȡ���������������ֻ���ܰ���һ��Э�����ݰ����������²㱣֤�ģ�
  Calls:
  Called By:
  Input:          pucInput:     ��ɨ�������
                  ulInputLen:   ��ɨ������ݳ��ȣ�������ʼ������־
  Output:         pucOutput:    ��ȡ�������������ݰ���������ʼ������־����������������ݰ�����Ч
                  ulOutputLen:  0��ʾû�в鵽�������ݰ��������ʾ�������ݰ��ĳ���
  Return:         ����Ľ��
  Others:         
*************************************************/
LONG ExtractYkppData(UCHAR *pucInput, ULONG ulInputLen, UCHAR *&pucOutput, ULONG &ulOutputLen)
{
    ULONG i = 0;
    ulOutputLen = 0;
    
    for(; i < ulInputLen; i++)
    {
        if(pucInput[i] == YKPP_BEGIN_FLAG)
        {
            break;
        }
    }

    if(i == ulInputLen)
    {
        return FAILED;
    }

    pucOutput = pucInput + i; //ָ����ʼ��־
    i++;
    for(; i < ulInputLen; i++)
    {
        if(pucInput[i] == YKPP_END_FLAG)
        {
            break;
        }
    }

    if(i == ulInputLen)
    {
        return FAILED;
    }

    ulOutputLen = pucInput + i - pucOutput + 1;
    return SUCCEEDED;
}

/*************************************************
  Function:
  Description:    �����������ʿƳ���Э���У���룬У��ֵ���ڴ���ʼ��־
                  ֮��ĵ�һ�ֽڵ�У����ǰһ�ֽڵ������ֽ���Ӻ͵�����ֽ�
  Calls:
  Called By:
  Input:          pucData:     ��У�������
                  ulDataLen:   ��У������ݳ���
  Output:         
  Return:         ����ó���У����
  Others:         
*************************************************/
UCHAR CalcuYkppChecksum(UCHAR *pucData, ULONG ulDataLen)
{
    UCHAR ucChecksum = 0;
    for(ULONG i = 0; i < ulDataLen; i++)
    {
        ucChecksum += pucData[i];
    }
    return ucChecksum;
}

/*************************************************
  Function:
  Description:    ���������ݱ��뷽�������ݽ��н��룬������ԭ�����ڴ��Ͻ���
  Calls:
  Called By:
  Input:          ucSchm:      ���뷽��
                  pucData:     ����������ݣ����������뷽��
                  ulDataLen:   ����������ݳ���
  Output:         
  Return:         ���������ݳ��ȣ�0��ʾ����ʧ��
  Others:         
*************************************************/
ULONG DecodeYkppSchm(UCHAR ucSchm, UCHAR *pucData, ULONG ulDataLen)
{
    switch(ucSchm) //ָ����뷽��
    {
    case SCHM_A:
        return Merge2Bytes(pucData, ulDataLen);
    case SCHM_B:
        return DecodeEscape(pucData, ulDataLen,
                            g_stYkppEscape.ucEscape, 
                            g_stYkppEscape.aucAfterEscape,
                            g_stYkppEscape.aucBeforeEscape,
                            sizeof(g_stYkppEscape.aucAfterEscape));
    case SCHM_C:
    default:
        return 0;
    }
}

/*************************************************
  Function:
  Description:    ���������ݱ��뷽�������ݽ��б��룬������ԭ�����ڴ��Ͻ���
  Calls:
  Called By:
  Input:          ucSchm:      ���뷽��
                  pucData:     ����������ݣ����������뷽��
                  ulDataLen:   ����������ݳ���
  Output:         
  Return:         ���������ݳ��ȣ�0��ʾ����ʧ��
  Others:         
*************************************************/
ULONG EncodeYkppSchm(UCHAR ucSchm, UCHAR *pucData, ULONG ulDataLen)
{
    switch(ucSchm) //ָ����뷽��
    {
    case SCHM_A:
        return SplitInto2Bytes(pucData, ulDataLen); //���뷽�����μӶ��ֽڲ��
    case SCHM_B:
        return EncodeEscape(pucData, ulDataLen, 512,        //��������ת�峤�ȴﵽ512�ֽ�
                            g_stYkppEscape.ucEscape, 
                            g_stYkppEscape.aucAfterEscape,
                            g_stYkppEscape.aucBeforeEscape,
                            sizeof(g_stYkppEscape.aucAfterEscape));
    case SCHM_C:
    default:
        return 0;
    }
}

/*************************************************
  Function:
  Description:    ��������Э�����ݽṹ��������յ��ֽ�����������ʼ������־
  Calls:
  Called By:
  Input:          pstPdu:     Э�����ݽṹ
                  ulPduLen:   Э�����ݣ�����ʼ������־���⣩���ȣ�
  Output:         
  Return:         ���������ݳ��ȣ�0��ʾ����ʧ��
  Others:         
*************************************************/
ULONG BuildYkppPdu(YKPP_PDU_ST *pstPdu, ULONG ulPduLen)
{
    UCHAR *pucPdu = (UCHAR *)pstPdu;
    if((pstPdu->ucFieldIndicator & FI_AVAILABLE) 
    && (pstPdu->ucFieldIndicator & FI_CHECKSUM_NEEDED)) //�����ҪУ��
    {
        pucPdu[ulPduLen - 1] = CalcuYkppChecksum(pucPdu, ulPduLen - 1); //ԭ�������а���У�飬��Ҫ�۳�
    }
    ULONG ulFinalLen = EncodeYkppSchm(pstPdu->ucSchm, pucPdu + 1, ulPduLen - 1);
    ulFinalLen += 1;    //���뷽����Ԫ
    //������ʼ������־
    *(pucPdu - 1) = YKPP_BEGIN_FLAG;
    *(pucPdu + ulFinalLen) = YKPP_END_FLAG;

    return ulFinalLen + 2;
}


/*************************************************
  Function:
  Description:    ���������ڱ��泧��Э�������ز�����
                  ����FLASH����Сд��λ��4k����վ����
                  ����������һ��sector�У������Ҫͬʱ����
  Calls:
  Called By:
  Input:          
  Output:         
  Return:         
  Others:         
*************************************************/
void SaveYkppParam()
{
    if(CmpData(ADDR_YKPP_PARAM_SET, &g_stYkppParamSet, sizeof(g_stYkppParamSet)) != SUCCEEDED)
    {
        if(SimpleFlashEraseSector(ADDR_YKPP_PARAM_SET) != SUCCEEDED)
        {
            return;
        }    
        SimpleFlashWrite(ADDR_YKPP_PARAM_SET, &g_stYkppParamSet, sizeof(g_stYkppParamSet));        
    }

    SaveWritableParam(); //��ʵֻ�Ǹ������ܲ����е�վ���ţ������������Ǵ���ͬһ��sector����˱���һ�����
}

/*************************************************
  Function:
  Description:    ����������Э��Ķ�����
  Calls:
  Called By:
  Input:          pstPdu:     Э�����ݽṹ
                  ulPduLen:   Э�����ݣ�����ʼ������־���⣩���ȣ�
  Output:         
  Return:         ulActualLen:�����ĳ���       
  Others:         
*************************************************/
ULONG HndlYkppReadReq(YKPP_PDU_ST *pstPdu, ULONG ulPduLen)
{
    UCHAR *pucParamTbl = pstPdu->aucParamTbl;           //ָ�������
    ULONG ulActualLen  = sizeof(YKPP_PDU_ST) - 1;       //���Է��ش�����ʵ�ʳ��ȣ���������ʼ������־����ʼ��δ�ṹ�г�ȥ������ĳ���
    ULONG ulParamTblLen = ulPduLen - ulActualLen - 1;   //Ϊ�������ܳ���1����У����
    ULONG ulParamLen = 0;                               //���������ĳ���
    ULONG ulParamTblOffset = 0;                        //�������е�ƫ��

    UCHAR ucParamIDHiByte = 0;
    UCHAR ucParamIDLoByte = 0;

    
    while(ulParamTblLen >= 2)       //���ٰ�������ID�����ֽ� 
    {
        ucParamIDHiByte = pucParamTbl[ulParamTblOffset + 1];
        ucParamIDLoByte = pucParamTbl[ulParamTblOffset];

        ulParamTblOffset += 2;     //����IDָ���������
        ulParamTblLen -= 2;         //��ȥID����  
        switch(ucParamIDHiByte)       
        {
        case YKPP_RF_PARAM_SET:
            switch(ucParamIDLoByte)   
            {
            case YKPP_RF_PARAM_ULRFSW:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.ucUlRfSw;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucUlRfSw);
                //�ñ�־����ʱû���κζ���
                break;
            case YKPP_RF_PARAM_DLRFSW:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.ucDlRfSw;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucDlRfSw);
                //�ñ�־����ʱû���κζ���
                break;
            case YKPP_RF_PARAM_ULOUTMAX:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.cUlOutMax;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cUlOutMax);
                break;
            case YKPP_RF_PARAM_DLOUTMAX:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.ucDlOutMax;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucDlOutMax);
                break;
            case YKPP_RF_PARAM_ULOUTADJUST:
                memcpy(&(pucParamTbl[ulParamTblOffset]),(CHAR *)&(g_stYkppParamSet.stYkppRfParam.sUlOutAdjust),
                      sizeof(g_stYkppParamSet.stYkppRfParam.sUlOutAdjust));
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.sUlOutAdjust);
                break;
            case YKPP_RF_PARAM_DLOUTADJUST:
                memcpy(&(pucParamTbl[ulParamTblOffset]),(CHAR *)&(g_stYkppParamSet.stYkppRfParam.sDlOutAdjust),
                      sizeof(g_stYkppParamSet.stYkppRfParam.sDlOutAdjust));
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.sDlOutAdjust);
                break;
            case YKPP_RF_PARAM_DLREFLADJUST:
                memcpy(&(pucParamTbl[ulParamTblOffset]),(CHAR *)&(g_stYkppParamSet.stYkppRfParam.sDlReflAdjust),
                      sizeof(g_stYkppParamSet.stYkppRfParam.sDlReflAdjust));                
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.sDlReflAdjust);
                break;
            case YKPP_RF_PARAM_ULGAINMAX:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.ucUlGainMax;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucUlGainMax);
                break;
            case YKPP_RF_PARAM_DLINCOUPLER: 
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.ucDlInCoupler;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucDlInCoupler);
                break;
            case YKPP_RF_PARAM_CDMA800_IN_PWR:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.cCdma800InPwrAdjust;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cCdma800InPwrAdjust);
                break;
            case YKPP_RF_PARAM_MB_GSM_IN_PWR:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.cMbGsmInPwrAdjust;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cMbGsmInPwrAdjust);
                break;
            case YKPP_RF_PARAM_UC_GSM_IN_PWR:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.cUcGsmInPwrAdjust;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cUcGsmInPwrAdjust);
                break;
            case YKPP_RF_PARAM_MB_DCS_IN_PWR:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.cMbDcsInPwrAdjust;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cMbDcsInPwrAdjust);
                break;
            case YKPP_RF_PARAM_UC_DCS_IN_PWR:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.cUcDcsInPwrAdjust;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cUcDcsInPwrAdjust);
                break;
            case YKPP_RF_PARAM_3G1_FDD_IN_PWR:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.c3G1FDDInPwrAdjust;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.c3G1FDDInPwrAdjust);
                break;
            case YKPP_RF_PARAM_3G2_FDD_IN_PWR:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.c3G2FDDInPwrAdjust;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.c3G2FDDInPwrAdjust);
                break;
            case YKPP_RF_PARAM_3G3_TDD_IN_PWR:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.c3G3TDDInPwrAdjust;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.c3G3TDDInPwrAdjust);
                break;
            case YKPP_RF_PARAM_TRUNK_IN_PWR:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.cTrunkInPwrAdjust;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cTrunkInPwrAdjust);
                break;
            case YKPP_RF_PARAM_OUT_PWR_ADJUST1:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.cOutPwrAdjust1;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam);
                break;
            case YKPP_RF_PARAM_OUT_PWR_ADJUST2:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.cOutPwrAdjust2;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cOutPwrAdjust2);
                break;
            case YKPP_RF_PARAM_REFL_PWR_ADJUST1:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.cReflectPwrAdjust1;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cReflectPwrAdjust2);
                break;
            case YKPP_RF_PARAM_REFL_PWR_ADJUST2:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.cReflectPwrAdjust2;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cReflectPwrAdjust2);
                break;
            default:
                pstPdu->ucResult = YKPP_ERR_PARAM_ID;
                //��Ϊû���ṩͳһ��L������޷��жϺ��������
                //�������Ͼͷ��ش��󣬲��ٽ��к�������            
                break;
            }
            break;
        case YKPP_CTRL_PARAM_SET:
            switch(ucParamIDLoByte)          //ָ��ID���ֽ�
            {
            case YKPP_CTRL_PARAM_REBOOT_SW:  
                //pstPdu->ucResult = YKPP_ERR_PARAM_ID;                
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppCtrlParam.ucRebootSw;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucRebootSw);
                break;
            case YKPP_CTRL_PARAM_TELNUM_AUTH_SW:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppCtrlParam.ucTelNumAuthSw;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucTelNumAuthSw);
                break;
            case YKPP_CTRL_PARAM_ALARM_LIMIT_SW:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppCtrlParam.stAlarmLimitSw.ucOnOff;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stAlarmLimitSw.ucOnOff);
                pucParamTbl[ulParamTblOffset + ulParamLen] 
                    = g_stYkppParamSet.stYkppCtrlParam.stAlarmLimitSw.ucMaxAlarmPerHour; 
                ulParamLen += sizeof(g_stYkppParamSet.stYkppCtrlParam.stAlarmLimitSw.ucMaxAlarmPerHour);
                break;
            case YKPP_CTRL_PARAM_ADDR_AUTH_SW:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppCtrlParam.ucAddrAuthSw;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucAddrAuthSw);
                break;
            case YKPP_CTRL_PARAM_YKPP_RS_PSW:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppCtrlParam.ucYkppRspSw;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucYkppRspSw);
                break;
            case YKPP_CTRL_PARAM_ALARM_JUDGE_PARAM:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucTotal;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucTotal);
                
                pucParamTbl[ulParamTblOffset + ulParamLen] 
                    = g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucOccurLimit;
                ulParamLen += sizeof(g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucOccurLimit);
                
                pucParamTbl[ulParamTblOffset + ulParamLen] 
                    = g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucEliminateLimit;
                ulParamLen += sizeof(g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucEliminateLimit);
                break;
            case YKPP_CTRL_PARAM_UPDATE_MODE:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppCtrlParam.ucUpdateMode;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucUpdateMode);
                break;
            case YKPP_CTRL_PARAM_DEBUG_MODE:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppCtrlParam.ucDebugMode;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucDebugMode);
                break;
            case YKPP_CTRL_PARAM_BATT_JUDGE_START_VOLTAGE: 
                memcpy(&(pucParamTbl[ulParamTblOffset]),(CHAR *)&(g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStartVoltage),
                      sizeof(g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStartVoltage));
                ulParamLen =sizeof(g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStartVoltage); 
                break;
            case YKPP_CTRL_PARAM_BATT_JUDGE_STOP_VOLTAGE:
                memcpy(&(pucParamTbl[ulParamTblOffset]),(CHAR *)&(g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStopVoltage),
                      sizeof(g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStopVoltage));
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStopVoltage);
                break;
            case YKPP_CTRL_PARAM_LOG_SW: 
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ucIsEnabled;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ucIsEnabled);
                break; 
            case YKPP_CTRL_PARAM_LOG_OUTPUT:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ucOutput;  
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ucOutput);
                break;
            case YKPP_CTRL_PARAM_LOGSRCMASK:
                memcpy(&(pucParamTbl[ulParamTblOffset]),(CHAR *)&(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLogSrcMask),
                      sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLogSrcMask)); 
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLogSrcMask);
                break;
            case YKPP_CTRL_PARAM_LEVELMASK:
                memcpy(&(pucParamTbl[ulParamTblOffset]),(CHAR *)&(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLevelMask),
                      sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLevelMask)); 
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLevelMask);
                break;
            case YKPP_CTRL_PARAM_MIDMASK:
                memcpy(&(pucParamTbl[ulParamTblOffset]),(CHAR *)&(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulMIDMask),
                      sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulMIDMask)); 
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulMIDMask);                
                break;
            case YKPP_CTRL_PARAM_DLINACQ_MODE:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppCtrlParam.ucDlInAcqMode;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucDlInAcqMode);                
                break;                
            //MCM-64_20070315_Zhonghw_begin                                
            case YKPP_CTRL_PARAM_COMM_ROLE_TYPE:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType);                
                break;                  
            //MCM-64_20070315_Zhonghw_begin                                
            default: pstPdu->ucResult = YKPP_ERR_PARAM_ID;
                break;
            }
            break;
        case YKPP_COMM_PARAM_SET:
            switch(ucParamIDLoByte)
            {
            case YKPP_COMM_PARAM_SLAVE_BITMAP:
                memcpy(&(pucParamTbl[ulParamTblOffset]),(CHAR *)&(g_stYkppParamSet.stYkppCommParam.usSBitmap),
                      sizeof(g_stYkppParamSet.stYkppCommParam.usSBitmap));
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCommParam.usSBitmap);
                break;
            case YKPP_COMM_PARAM_SLAVE_NUM:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppCommParam.ucSNum;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCommParam.ucSNum);
                break;
            case YKPP_COMM_PARAM_SMC_ADDR:
                //�������ĺ���Ĳ���ֻ�ܷ�������ǰ��
                if(ulParamTblOffset > 2)        //ȷ��ǰ������������
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;
                    return ulPduLen;
                }                
                //�����첽����
                //MCM-54_20070111_Zhonghw_begin
                pstPdu->ucResult = YKPP_ERR_ASYN_BUF_FULL;
                //MCM-54_20070111_Zhonghw_end
                
                g_stSMCParam.ucOpType = NEED_MORE_OP_READ;
                //���ƶ������ĺ���ĵļ�ز���
                memcpy(g_stSMCParam.aucParam + g_stSMCParam.usParamLen, &(pucParamTbl[ulParamTblOffset - 2]), 
                      sizeof(g_stNmParamSet.acSmcAddr) + 2);
                g_stSMCParam.usParamLen += sizeof(g_stNmParamSet.acSmcAddr) + 2;
    
                ulParamLen = sizeof(g_stNmParamSet.acSmcAddr);
                break;                
            default: pstPdu->ucResult = YKPP_ERR_PARAM_ID;
                break;
            }
            break;
        case YKPP_MEM_PARAM_SET:
        {
            //һ���������������һ�����Բ����������ٰ�������������
            if(ulParamTblOffset > 2)
            {
                pstPdu->ucResult = YKPP_ERR_OPER_FAILED;
                return ulPduLen;
            }        
            switch(ucParamIDLoByte)
            {
            case YKPP_MEM_PARAM_EEPROMDATA:
            {
                USHORT usValueLen = 0;                
                USHORT usEepromAddr = 0;
                USHORT usEepromDataLen = 0;                
                //��ȡL��ֵ
                memcpy((CHAR *)&usValueLen,&pucParamTbl[ulParamTblOffset],sizeof(usValueLen));
                ulParamLen = sizeof(usValueLen);
                //��ȡ�洢��Ԫ��ַ
                memcpy((CHAR *)&usEepromAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],
                      sizeof(usEepromAddr));
                ulParamLen += sizeof(usEepromAddr);
                //��ȡ��ȡ���ݵĳ���
                memcpy((CHAR *)&usEepromDataLen,&pucParamTbl[ulParamTblOffset + ulParamLen],
                      sizeof(usEepromDataLen));                
                ulParamLen += sizeof(usEepromDataLen);
                
                //���ȼ�Ȩ
                if(usValueLen != (sizeof(usEepromAddr) + sizeof(usEepromDataLen)))
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //����Э����洢������
                    return ulPduLen;                    
                }
                //����V��ֵ
                usValueLen = sizeof(usEepromAddr) + sizeof(usEepromDataLen) + usEepromDataLen;
                memcpy(&pucParamTbl[ulParamTblOffset],(CHAR *)&usValueLen,sizeof(usValueLen));  
                
                //�������Ŀ��ó���С�ڿ����ĳ���,���ٽ������ݶ�ȡ
                //(�����)
                //�洢����������Ҫ��ֻ֤����һ������
                //ֻ����һ�������Ķ�����������������ܳ��ȵıȽϼ��ɵó��Ƿ�����������
                if((ulActualLen + ulParamTblOffset + ulParamLen) == (ulPduLen - 1))        //��֤����û�в�����1ΪУ���볤��
                {   
                #ifndef M3
                    I2cReadMore((void *)(ULONG)usEepromAddr, &pucParamTbl[ulParamTblOffset + ulParamLen], (UCHAR)usEepromDataLen);
                #else
                    //memset(&pucParamTbl[ulParamTblOffset + ulParamLen],1,(CHAR)usEepromDataLen);                    
                #endif    
                    ulParamLen += usEepromDataLen;
                }
                else   //д����Ŀ�곤�������ݰ������ݳ��Ȳ����ϻ��߶���һ��ID
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //����Э����洢������
                    return ulPduLen;
                }
                break;
            }    
            case YKPP_MEM_PARAM_ADDRBUSDATA:
            {
                USHORT usValueLen = 0;
                ULONG ulBussAddr = 0;
                USHORT usBussDataLen = 0;
                //��ȡL��ֵ
                memcpy((CHAR *)&usValueLen,&pucParamTbl[ulParamTblOffset],sizeof(usValueLen));
                ulParamLen = sizeof(usValueLen);
                memcpy((CHAR *)&ulBussAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],
                       sizeof(ulBussAddr));                
                ulParamLen += sizeof(ulBussAddr);
                memcpy((CHAR *)&usBussDataLen,&pucParamTbl[ulParamTblOffset + ulParamLen],
                       sizeof(usBussDataLen));
                ulParamLen += sizeof(usBussDataLen);

                //���ȼ�Ȩ
                if(usValueLen != (sizeof(ulBussAddr) + sizeof(usBussDataLen) + usBussDataLen))
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //����Э��д�洢������
                    return ulPduLen;                    
                }

                //����V��ֵ
                usValueLen = sizeof(ulBussAddr) + sizeof(usBussDataLen) + usBussDataLen;
                memcpy(&pucParamTbl[ulParamTblOffset],(CHAR *)&usValueLen,sizeof(usValueLen)); 

                //ֻ����һ��������д����������������ܳ��ȵıȽϼ��ɵó��Ƿ�����������
                if((ulActualLen + ulParamTblOffset + ulParamLen) == (ulPduLen - 1))        //��֤����û�в�����1ΪУ���볤��  
                {
                    //���ݵ�ַ��С�������Ƕ�ȡFLASH����RAM
                    //FLASH����,��Ҫ����NOR/NAN FLASH
                    if((ADDR_FLASH_BEGIN <= ulBussAddr)&&(ulBussAddr <= ADDR_FLASH_END))     
                    {
                        //TEST
                        //memset(&pucParamTbl[ulParamTblOffset + ulParamLen],1,(CHAR)usBussDataLen);
                        SimpleFlashRead(ulBussAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],usBussDataLen);
                    }
                    else //RAM����
                    {
                        //TEST
                        //memset(&pucParamTbl[ulParamTblOffset + ulParamLen],1,(CHAR)usBussDataLen);
                        memcpy(&pucParamTbl[ulParamTblOffset + ulParamLen],(CHAR *)ulBussAddr,usBussDataLen);                    
                    }
                    ulParamLen += usBussDataLen;
                }
                else   //д����Ŀ�곤�������ݰ������ݳ��Ȳ����ϻ��߶���һ��ID
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //����Э����洢������
                    return ulPduLen;
                }
                break;
            }
            default:pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //�ò��������־??
                    return ulPduLen;
            }
            ulActualLen += ulParamTblOffset;
            ulActualLen += ulParamLen;
            return ulActualLen + 1;           //�洢������ֻ֧��һ�������������Է���
                                              //1ΪУ����ĳ���
        }
        case YKPP_CORRE_PARAM_SET:
            switch(ucParamIDLoByte)
            {
            case YKPP_CORRE_PARAM_STAADDR:
                //������������bitmap
                //SET_BITMAP(g_usYkppSettingParamBitmap,BITMAP_YKPP_MODIFY_STANUM_DEVNUM);  
                memcpy(&(pucParamTbl[ulParamTblOffset]),(UCHAR *)&(g_stNmParamSet.ulStaNum),
                      sizeof(g_stNmParamSet.ulStaNum));
                ulParamLen = sizeof(g_stNmParamSet.ulStaNum);
                memcpy( &(pucParamTbl[ulParamTblOffset + ulParamLen]),(UCHAR *)&(g_stNmParamSet.ucDevNum),
                     sizeof(g_stNmParamSet.ucDevNum));
                ulParamLen += sizeof(g_stNmParamSet.ucDevNum);
                break;
            default:pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //�ò��������־??
                break;
            }
            break;
        case YKPP_DEBUG_PARAM_SET:
            //һ���������������һ�����Բ����������ٰ�������������
            if(ulParamTblOffset > 2)
            {
                pstPdu->ucResult = YKPP_ERR_OPER_FAILED;
                break;
            }
            switch(ucParamIDLoByte)
            {
            case YKPP_DEBUG_PARAM_TIMER_INFO:
            {
                TIMER_INFO_ST stTimerInfo;
                g_TimerMngr.Query(stTimerInfo);
                pucParamTbl[ulParamTblOffset] = sizeof(TIMER_INFO_ST);
                memcpy(&pucParamTbl[ulParamTblOffset + 1], &stTimerInfo, sizeof(TIMER_INFO_ST));
                ulParamLen += 1 + pucParamTbl[ulParamTblOffset]; //1��ʾ������ռ���ֽ�
            }
                break;
            case YKPP_DEBUG_PARAM_STACK_INFO:
#if MY_CHECK_STACK_EN > 0
                pucParamTbl[ulParamTblOffset] = GetStackInfo(&pucParamTbl[ulParamTblOffset + 1], 256);
                ulParamLen += 1 + pucParamTbl[ulParamTblOffset]; //1��ʾ������ռ���ֽ�
#else
                pucParamTbl[ulParamTblOffset] = 0; //����Ϊ0��ʾ��֧��
                ulParamLen += 1; //1��ʾ������ռ���ֽ�
#endif
                break;
            case YKPP_DEBUG_PARAM_MEM_INFO:
#if MY_CHECK_MEM_EN > 0
                pucParamTbl[ulParamTblOffset] = GetMemInfo(&pucParamTbl[ulParamTblOffset + 1], 256);
                ulParamLen += 1 + pucParamTbl[ulParamTblOffset]; //1��ʾ������ռ���ֽ�
#else
                pucParamTbl[ulParamTblOffset] = 0; //����Ϊ0��ʾ��֧��
                ulParamLen += 1; //1��ʾ������ռ���ֽ�
#endif
                break;
            case YKPP_DEBUG_PARAM_QUE_INFO:
#if MY_CHECK_QUE_EN > 0
                pucParamTbl[ulParamTblOffset] = GetQueInfo(&pucParamTbl[ulParamTblOffset + 1], 256);
                ulParamLen += 1 + pucParamTbl[ulParamTblOffset]; //1��ʾ������ռ���ֽ�
#else
                pucParamTbl[ulParamTblOffset] = 0; //����Ϊ0��ʾ��֧��
                ulParamLen += 1; //1��ʾ������ռ���ֽ�
#endif
                break;
            default:
                pstPdu->ucResult = YKPP_ERR_PARAM_ID;
                break;            
            }
            break;
        default:pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //�ò��������־??
            break;
        }
        ulParamTblLen -= ulParamLen;             //��ȥһ��ID��TV   
        ulParamTblOffset += ulParamLen;          //ָ����һ��ID

        if(pstPdu->ucResult == YKPP_ERR_PARAM_ID)
        {
            return ulPduLen; //��������������ٴ���ֱ��ʹ�ô�������ݳ��ȷ���
        }
    }
    ulActualLen += ulParamTblOffset ;        //���ش����ĳ���
    return ulActualLen + 1;                  //����У����ĳ���1
}
/*************************************************
  Function:
  Description:    ����������Э���ִ������
  Calls:
  Called By:
  Input:          pstPdu:     Э�����ݽṹ
                  ulPduLen:   Э�����ݣ�����ʼ������־���⣩���ȣ�
  Output:         
  Return:         ulActualLen:�����ĳ���       
  Others:         
*************************************************/
LONG HndlYkppExeReq(YKPP_PDU_ST *pstPdu, ULONG ulPduLen)
{
    UCHAR *pucParamTbl = pstPdu->aucParamTbl;           //ָ�������
    ULONG ulActualLen  = sizeof(YKPP_PDU_ST) - 1;       //���Է��ش�����ʵ�ʳ��ȣ���������ʼ������־����ʼ��Ϊ�ṹ�г�ȥ������ĳ���
    ULONG ulParamTblLen = ulPduLen - ulActualLen - 1;   //Ϊ�������ܳ���1����У����
    ULONG ulParamLen = 0;                               //���������ĳ���
    ULONG ulParamTblOffset = 0;                        //�������е�ƫ��

    UCHAR ucParamIDHiByte = 0;
    UCHAR ucParamIDLoByte = 0;

   
    while(ulParamTblLen >= 2)
    {
        //һ���������������һ�����Բ����������ٰ�������������
        if(ulParamTblOffset > 0)        //ȷ��ǰ������������
        {
            pstPdu->ucResult = YKPP_ERR_OPER_FAILED;
            break;
        }
        
        ucParamIDHiByte = pucParamTbl[ulParamTblOffset + 1];
        ucParamIDLoByte = pucParamTbl[ulParamTblOffset];
        ulParamTblOffset += 2;     //����IDָ���������
        ulParamTblLen -= 2;         //��ȥID����    
        switch(ucParamIDLoByte)
        {
        case YKPP_EXE_PARAM_TOPOLOGY:
            break;
        case YKPP_EXE_PARAM_REPORT_TYPE:
        {
            UCHAR ucReportTpye = pucParamTbl[ulParamTblOffset];
            ulParamLen = sizeof(ucReportTpye);  
            if((ulActualLen + ulParamTblOffset + ulParamLen) == (ulPduLen - 1))//ȷ����������������
            {
                switch(ucReportTpye)
                {
                case REPORT_TYPE_YKPP_STA_INIT:
                    SET_BITMAP(g_usYkppSettingParamBitmap,BITMAP_YKPP_STA_INIT);
                    break;
                case REPORT_TYPE_YKPP_INSPECTION:
                    SET_BITMAP(g_usYkppSettingParamBitmap,BITMAP_YKPP_INSPECTION);
                    break; 
                case REPORT_TYPE_YKPP_REPAIR_CONFIRMED:
                    SET_BITMAP(g_usYkppSettingParamBitmap,BITMAP_YKPP_REPAIR_CONFIRMED);
                    break; 
                case REPORT_TYPE_YKPP_CONFIG_CHANGED:
                    SET_BITMAP(g_usYkppSettingParamBitmap,BITMAP_YKPP_CONFIG_CHANGED);
                    break;
                default:
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;
                    break;
                }
            }
        }
            break;
        case YKPP_EXE_PARAM_AT_CMD:
            break;
        case YKPP_EXE_PARAM_ALL_ALARM_ENABLE_SW:
        {
            UCHAR ucIsAllAlarmEnableSwOn = pucParamTbl[ulParamTblOffset];
            ulParamLen = sizeof(ucIsAllAlarmEnableSwOn);
            if((ulActualLen + ulParamTblOffset + ulParamLen) == (ulPduLen - 1))//ȷ����������������
            {
                if(ucIsAllAlarmEnableSwOn == 0)
                {
                    //�ر����и澯ʹ��
                    memset(&g_uAlarmEnable, 0, sizeof(g_uAlarmEnable));
                }
                else
                {
                    //�����и澯ʹ��
                    memset(&g_uAlarmEnable, 1, sizeof(g_uAlarmEnable));
                }
            }
            break;            
        }
        case YKPP_EXE_PARAM_RESET:
            //�ָ��������ã����ǲ��ָ�������ͨ����صĲ���
            //����վ���š��豸��š��ϱ�ͨ�ŷ�ʽ
            InitCmccDefaultParam();
            InitYkppDefaultParam();
            break;        
        default:
            pstPdu->ucResult = YKPP_ERR_PARAM_ID;
            break;
        }

        ulParamTblLen -= ulParamLen;             //��ȥһ��ID��TV   
        ulParamTblOffset += ulParamLen;          //ָ����һ��ID
        if(pstPdu->ucResult == YKPP_ERR_PARAM_ID)
        {
            return ulPduLen; //��������������ٴ���ֱ��ʹ�ô�������ݳ��ȷ���
        }    
    }
    ulActualLen += ulParamTblOffset;        //���ش����ĳ���
    return ulActualLen + 1;                  //����У����ĳ���1    
}
/*************************************************
  Function:
  Description:    ����������Э���д����
  Calls:
  Called By:
  Input:          pstPdu:     Э�����ݽṹ
                  ulPduLen:   Э�����ݣ�����ʼ������־���⣩���ȣ�
                  ucNeedToRsp Э�������Ƿ���ҪӦ���־
  Output:         
  Return:         ulActualLen:�����ĳ���       
  Others:         
*************************************************/
ULONG HndlYkppWriteReq(YKPP_PDU_ST *pstPdu, ULONG ulPduLen,UCHAR *ucNeedToRsp)
{
    UCHAR *pucParamTbl = pstPdu->aucParamTbl;           //ָ�������
    ULONG ulActualLen  = sizeof(YKPP_PDU_ST) - 1;       //���Է��ش�����ʵ�ʳ��ȣ���������ʼ������־����ʼ��δ�ṹ�г�ȥ������ĳ���
    ULONG ulParamTblLen = ulPduLen - ulActualLen - 1;   //Ϊ�������ܳ���1����У����
    ULONG ulParamLen = 0;                               //���������ĳ���
    ULONG ulParamTblOffset = 0;                         //�������е�ƫ��

    UCHAR ucParamIDHiByte = 0;
    UCHAR ucParamIDLoByte = 0;

    while(ulParamTblLen >= 2)       //���ٰ�������ID�����ֽ� 
    {
        ucParamIDHiByte = pucParamTbl[ulParamTblOffset + 1];
        ucParamIDLoByte = pucParamTbl[ulParamTblOffset];

        ulParamTblOffset += 2;     //����IDָ���������
        ulParamTblLen -= 2;         //��ȥID����  
        switch(ucParamIDHiByte)       
        {
        case YKPP_RF_PARAM_SET:
            switch(ucParamIDLoByte)   
            {
            case YKPP_RF_PARAM_ULRFSW:
                g_stYkppParamSet.stYkppRfParam.ucUlRfSw = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucUlRfSw);
                //�ñ�־����ʱû���κζ���
                break;
            case YKPP_RF_PARAM_DLRFSW:
                g_stYkppParamSet.stYkppRfParam.ucDlRfSw = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucDlRfSw);
                //�ñ�־����ʱû���κζ���
                break;
            case YKPP_RF_PARAM_ULOUTMAX:
                g_stYkppParamSet.stYkppRfParam.cUlOutMax = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cUlOutMax);
                break;
            case YKPP_RF_PARAM_DLOUTMAX:
                g_stYkppParamSet.stYkppRfParam.ucDlOutMax = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucDlOutMax);
                break;
            case YKPP_RF_PARAM_ULOUTADJUST:
                memcpy((CHAR *)&(g_stYkppParamSet.stYkppRfParam.sUlOutAdjust),
                      &(pucParamTbl[ulParamTblOffset]),
                      sizeof(g_stYkppParamSet.stYkppRfParam.sUlOutAdjust));
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.sUlOutAdjust);
                break;
            case YKPP_RF_PARAM_DLOUTADJUST:
                memcpy((CHAR *)&(g_stYkppParamSet.stYkppRfParam.sDlOutAdjust),
                      &(pucParamTbl[ulParamTblOffset]),
                      sizeof(g_stYkppParamSet.stYkppRfParam.sDlOutAdjust));
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.sDlOutAdjust);
                break;
            case YKPP_RF_PARAM_DLREFLADJUST:
                memcpy((CHAR *)&(g_stYkppParamSet.stYkppRfParam.sDlReflAdjust),
                      &(pucParamTbl[ulParamTblOffset]),
                      sizeof(g_stYkppParamSet.stYkppRfParam.sDlReflAdjust));                
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.sDlReflAdjust);
                break;
            case YKPP_RF_PARAM_ULGAINMAX:
                g_stYkppParamSet.stYkppRfParam.ucUlGainMax = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucUlGainMax);
                break;
            case YKPP_RF_PARAM_DLINCOUPLER: 
                g_stYkppParamSet.stYkppRfParam.ucDlInCoupler = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucDlInCoupler);
                break;
            case YKPP_RF_PARAM_CDMA800_IN_PWR:
                g_stYkppParamSet.stYkppRfParam.cCdma800InPwrAdjust = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cCdma800InPwrAdjust);
                break;
            case YKPP_RF_PARAM_MB_GSM_IN_PWR:
                g_stYkppParamSet.stYkppRfParam.cMbGsmInPwrAdjust = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cMbGsmInPwrAdjust);
                break;
            case YKPP_RF_PARAM_UC_GSM_IN_PWR:
                g_stYkppParamSet.stYkppRfParam.cUcGsmInPwrAdjust = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cUcGsmInPwrAdjust);
                break;
            case YKPP_RF_PARAM_MB_DCS_IN_PWR:
                g_stYkppParamSet.stYkppRfParam.cMbDcsInPwrAdjust = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cMbDcsInPwrAdjust);
                break;
            case YKPP_RF_PARAM_UC_DCS_IN_PWR:
                g_stYkppParamSet.stYkppRfParam.cUcDcsInPwrAdjust = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cUcDcsInPwrAdjust);
                break;
            case YKPP_RF_PARAM_3G1_FDD_IN_PWR:
                g_stYkppParamSet.stYkppRfParam.c3G1FDDInPwrAdjust = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.c3G1FDDInPwrAdjust);
                break;
            case YKPP_RF_PARAM_3G2_FDD_IN_PWR:
                g_stYkppParamSet.stYkppRfParam.c3G2FDDInPwrAdjust = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.c3G2FDDInPwrAdjust);
                break;
            case YKPP_RF_PARAM_3G3_TDD_IN_PWR:
                g_stYkppParamSet.stYkppRfParam.c3G3TDDInPwrAdjust = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.c3G3TDDInPwrAdjust);
                break;
            case YKPP_RF_PARAM_TRUNK_IN_PWR:
                g_stYkppParamSet.stYkppRfParam.cTrunkInPwrAdjust = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cTrunkInPwrAdjust);
                break;
            case YKPP_RF_PARAM_OUT_PWR_ADJUST1:
                g_stYkppParamSet.stYkppRfParam.cOutPwrAdjust1 = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cOutPwrAdjust1);
                break;
            case YKPP_RF_PARAM_OUT_PWR_ADJUST2:
                g_stYkppParamSet.stYkppRfParam.cOutPwrAdjust2 = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cOutPwrAdjust2);
                break;
            case YKPP_RF_PARAM_REFL_PWR_ADJUST1:
                g_stYkppParamSet.stYkppRfParam.cReflectPwrAdjust1 = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cReflectPwrAdjust2);
                break;
            case YKPP_RF_PARAM_REFL_PWR_ADJUST2:
                g_stYkppParamSet.stYkppRfParam.cReflectPwrAdjust2 = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.cReflectPwrAdjust2);
                break;
            default:
                pstPdu->ucResult = YKPP_ERR_PARAM_ID;           
                //��Ϊû���ṩͳһ��L������޷��жϺ��������
                //�������Ͼͷ��ش��󣬲��ٽ��к�������            
                break;
            }
            break;
        case YKPP_CTRL_PARAM_SET:
            switch(ucParamIDLoByte)          //ָ��ID���ֽ�
            {
            case YKPP_CTRL_PARAM_REBOOT_SW:
                g_stYkppParamSet.stYkppCtrlParam.ucRebootSw = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucRebootSw);
                //��������������?
                break;
            case YKPP_CTRL_PARAM_TELNUM_AUTH_SW:
                g_stYkppParamSet.stYkppCtrlParam.ucTelNumAuthSw = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucTelNumAuthSw);
                break;
            case YKPP_CTRL_PARAM_ALARM_LIMIT_SW:
                g_stYkppParamSet.stYkppCtrlParam.stAlarmLimitSw.ucOnOff = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stAlarmLimitSw.ucOnOff);
                g_stYkppParamSet.stYkppCtrlParam.stAlarmLimitSw.ucMaxAlarmPerHour 
                    = pucParamTbl[ulParamTblOffset + ulParamLen]; 
                ulParamLen += sizeof(g_stYkppParamSet.stYkppCtrlParam.stAlarmLimitSw.ucMaxAlarmPerHour);
                break;
            case YKPP_CTRL_PARAM_ADDR_AUTH_SW:
                g_stYkppParamSet.stYkppCtrlParam.ucAddrAuthSw = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucAddrAuthSw);
                break;
            case YKPP_CTRL_PARAM_YKPP_RS_PSW:
                g_stYkppParamSet.stYkppCtrlParam.ucYkppRspSw = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucYkppRspSw);
                if(g_stYkppParamSet.stYkppCtrlParam.ucYkppRspSw == YKPP_RSP_SW_OFF)
                {
                    *ucNeedToRsp = YKPP_NEED_NO_RSP;
                }
                break;
            case YKPP_CTRL_PARAM_ALARM_JUDGE_PARAM:
                g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucTotal = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucTotal);                
                g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucOccurLimit = pucParamTbl[ulParamTblOffset + 1];
                ulParamLen += sizeof(g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucOccurLimit);                 
                g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucEliminateLimit = pucParamTbl[ulParamTblOffset + 2];
                ulParamLen += sizeof(g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucEliminateLimit);
                break;
            case YKPP_CTRL_PARAM_UPDATE_MODE:
                g_stYkppParamSet.stYkppCtrlParam.ucUpdateMode = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucUpdateMode);
                //����������ģʽ��Ч���Ƿ���Ҫ���⶯��?
                break;
            case YKPP_CTRL_PARAM_DEBUG_MODE:
                g_stYkppParamSet.stYkppCtrlParam.ucDebugMode = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucDebugMode);
                //��Ϊ����ģʽû�����Ե��л�������ֻͨ����־λ����,���Բ���bitmap
                break;
            case YKPP_CTRL_PARAM_BATT_JUDGE_START_VOLTAGE: 
                memcpy((CHAR *)&(g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStartVoltage),
                      &(pucParamTbl[ulParamTblOffset]),
                      sizeof(g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStartVoltage));
                ulParamLen =sizeof(g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStartVoltage); 
                break;
            case YKPP_CTRL_PARAM_BATT_JUDGE_STOP_VOLTAGE:
                memcpy((CHAR *)&(g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStopVoltage),
                      &(pucParamTbl[ulParamTblOffset]),
                      sizeof(g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStopVoltage));
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.usBattJudgeStopVoltage);
                break;
            case YKPP_CTRL_PARAM_LOG_SW: 
                g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ucIsEnabled = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ucIsEnabled);
                break; 
            case YKPP_CTRL_PARAM_LOG_OUTPUT:
                g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ucOutput = pucParamTbl[ulParamTblOffset];  
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ucOutput);
                break;
            case YKPP_CTRL_PARAM_LOGSRCMASK:
                memcpy((CHAR *)&(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLogSrcMask),&(pucParamTbl[ulParamTblOffset]),
                      sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLogSrcMask)); 
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLogSrcMask);
                break;
            case YKPP_CTRL_PARAM_LEVELMASK:
                memcpy((CHAR *)&(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLevelMask),&(pucParamTbl[ulParamTblOffset]),
                      sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLevelMask)); 
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLevelMask);                
                break;
            case YKPP_CTRL_PARAM_MIDMASK:
                memcpy((CHAR *)&(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulMIDMask),&(pucParamTbl[ulParamTblOffset]),
                      sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulMIDMask)); 
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulMIDMask);                
                break;
            case YKPP_CTRL_PARAM_DLINACQ_MODE:
                g_stYkppParamSet.stYkppCtrlParam.ucDlInAcqMode = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucDlInAcqMode);                
                break;                  
            //MCM-64_20070315_Zhonghw_begin                
            case YKPP_CTRL_PARAM_COMM_ROLE_TYPE:
                //�Ƚ�һ���Ƿ�������ֵ
                if(memcmp(&g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType,
                          &pucParamTbl[ulParamTblOffset],
                          sizeof(g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType)))
                {
                    //ʹ�ó���Э���޸�Tͨ�����ӹ��ܲ���Ҫ����
                    SET_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_MODIFY_COMM_SETTING);                
                    g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType= pucParamTbl[ulParamTblOffset];
                }
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType);                                
                break;                                  
            //MCM-64_20070315_Zhonghw_begin                                
            default: pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //�ò��������־??
                break;
            }
            break;
        case YKPP_COMM_PARAM_SET:
            switch(ucParamIDLoByte)
            {
            case YKPP_COMM_PARAM_SLAVE_BITMAP:
                memcpy((CHAR *)&(g_stYkppParamSet.stYkppCommParam.usSBitmap),
                      &(pucParamTbl[ulParamTblOffset]),
                      sizeof(g_stYkppParamSet.stYkppCommParam.usSBitmap));
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCommParam.usSBitmap);
                break;
            case YKPP_COMM_PARAM_SLAVE_NUM:
                g_stYkppParamSet.stYkppCommParam.ucSNum = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCommParam.ucSNum);
                break;
            case YKPP_COMM_PARAM_SMC_ADDR:
               //�������ĺ���Ĳ���ֻ�ܷ�������ǰ��
               if(ulParamTblOffset > 2)        //ȷ��ǰ������������
               {
                   pstPdu->ucResult = YKPP_ERR_OPER_FAILED;
                   return ulPduLen;
               }                
                //�����첽����
                //MCM-54_20070111_Zhonghw_begin
                pstPdu->ucResult = YKPP_ERR_ASYN_BUF_FULL;
                //MCM-54_20070111_Zhonghw_end
                
                g_stSMCParam.ucOpType = NEED_MORE_OP_WRITE;
                
                //���ƶ������ĺ���ĵļ�ز���
                memcpy(g_stSMCParam.aucParam + g_stSMCParam.usParamLen, &(pucParamTbl[ulParamTblOffset - 2]), 
                      sizeof(g_stNmParamSet.acSmcAddr) + 2);
                g_stSMCParam.usParamLen += sizeof(g_stNmParamSet.acSmcAddr) + 2;
                
                ulParamLen = sizeof(g_stNmParamSet.acSmcAddr);
                break;
            default: pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //�ò��������־??
                break;
            }
            break;
        case YKPP_MEM_PARAM_SET:
        {
            //һ���������������һ�����Բ����������ٰ�������������
            if(ulParamTblOffset > 2)        //��֤ǰ�治������������
            {
                pstPdu->ucResult = YKPP_ERR_OPER_FAILED;
                return ulPduLen;
            }        
            switch(ucParamIDLoByte)
            {
            case YKPP_MEM_PARAM_EEPROMDATA:
            {
                USHORT usValueLen = 0;                
                USHORT usEepromAddr = 0;
                USHORT usEepromDataLen = 0;                
                //��ȡL��ֵ
                memcpy((CHAR *)&usValueLen,&pucParamTbl[ulParamTblOffset],sizeof(usValueLen));
                ulParamLen = sizeof(usValueLen);
                //��ȡ�洢��Ԫ��ַ
                memcpy((CHAR *)&usEepromAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],
                      sizeof(usEepromAddr));
                ulParamLen += sizeof(usEepromAddr);
                //��ȡ��ȡ���ݵĳ���
                memcpy((CHAR *)&usEepromDataLen,&pucParamTbl[ulParamTblOffset + ulParamLen],
                      sizeof(usEepromDataLen));                
                ulParamLen += sizeof(usEepromDataLen);

                //���ȼ�Ȩ
                if(usValueLen != (sizeof(usEepromAddr) + sizeof(usEepromDataLen) + usEepromDataLen))
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //����Э����洢������
                    return ulPduLen;                    
                }
                //�������Ŀ��ó���С�ڿ����ĳ���,���ٽ������ݶ�ȡ
                //(�����)
                //��α�ֻ֤����һ������??
                //ֻ����һ��������д����������������ܳ��ȵıȽϼ��ɵó��Ƿ�����������
                if((ulActualLen + ulParamTblOffset + ulParamLen + usEepromDataLen) == (ulPduLen - 1))        //��֤����û�в�����1ΪУ���볤��
                {
                #ifndef M3
                    I2cWriteSlow((void *)(ULONG)usEepromAddr, &pucParamTbl[ulParamTblOffset + ulParamLen], (UCHAR)usEepromDataLen);
                #else    
                    memset(&pucParamTbl[ulParamTblOffset + ulParamLen],1,(CHAR)usEepromDataLen);
                #endif                    
                    ulParamLen += usEepromDataLen;
                }
                else   //д����Ŀ�곤�������ݰ������ݳ��Ȳ����ϻ��߶���һ��ID
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //����Э��д�洢������
                    return ulPduLen;
                }
                break;
            }    
            case YKPP_MEM_PARAM_ADDRBUSDATA:
            {
                USHORT usValueLen = 0;
                ULONG ulBussAddr = 0;
                USHORT usBussDataLen = 0;
                //��ȡL��ֵ
                memcpy((CHAR *)&usValueLen,&pucParamTbl[ulParamTblOffset],sizeof(usValueLen));
                ulParamLen = sizeof(usValueLen);
                memcpy((CHAR *)&ulBussAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],
                       sizeof(ulBussAddr));                
                ulParamLen += sizeof(ulBussAddr);
                memcpy((CHAR *)&usBussDataLen,&pucParamTbl[ulParamTblOffset + ulParamLen],
                       sizeof(usBussDataLen));
                ulParamLen += sizeof(usBussDataLen);

                //���ȼ�Ȩ
                if(usValueLen != (sizeof(ulBussAddr) + sizeof(usBussDataLen) + usBussDataLen))
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //����Э��д�洢������
                    return ulPduLen;                    
                }
                //ֻ����һ��������д����������������ܳ��ȵıȽϼ��ɵó��Ƿ�����������
                if((ulActualLen + ulParamTblOffset + ulParamLen + usBussDataLen) == (ulPduLen - 1))        //��֤����û�в�����1ΪУ���볤��
                {
                    //���ݵ�ַ��С�������Ƕ�ȡFLASH����RAM
                    if((ADDR_FLASH_BEGIN <= ulBussAddr)&&(ulBussAddr <= ADDR_FLASH_END))      //FLASH����,��Ҫ����NOR/NAN FLASH
                    {
                        if(CmpData(ulBussAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],usBussDataLen) != SUCCEEDED)
                        {
                            if(SimpleFlashEraseSector(ulBussAddr) != SUCCEEDED)
                            {
                                return ulPduLen;
                            }
                            //TEST
                            //memset(&pucParamTbl[ulParamTblOffset + ulParamLen],1,(CHAR)usBussDataLen);
                            SimpleFlashWrite(ulBussAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],usBussDataLen);
                        }
                    }
                    else //RAM����
                    {
                        //TEST
                        //memset(&pucParamTbl[ulParamTblOffset + ulParamLen],1,(CHAR)usBussDataLen);                        
                        memcpy((CHAR *)ulBussAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],usBussDataLen);                    
                    }
                    ulParamLen += usBussDataLen;
                }
                else   //д����Ŀ�곤�������ݰ������ݳ��Ȳ����ϻ��߶���һ��ID
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //����Э��д�洢������
                    return ulPduLen;
                }
                break;
            }
            default:pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //�ò��������־??
                    return ulPduLen;
            }
            ulActualLen += ulParamTblOffset;
            ulActualLen += ulParamLen;
            return ulActualLen + 1;           //�洢������ֻ֧��һ�������������Է���
                                              //1ΪУ����ĳ���
        }
        case YKPP_CORRE_PARAM_SET:
            switch(ucParamIDLoByte)
            {
            case YKPP_CORRE_PARAM_STAADDR:
                //���޸�վ�����������
                //ʹ�ó���Э���޸�վ���豸��ţ�Ҫ����
                SET_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_MODIFY_STANUM_DEVNUM);
                memcpy((UCHAR *)&(g_stNmParamSet.ulStaNum),
                      &(pucParamTbl[ulParamTblOffset]),sizeof(g_stNmParamSet.ulStaNum));
                ulParamLen = sizeof(g_stNmParamSet.ulStaNum);
                memcpy((UCHAR *)&(g_stNmParamSet.ucDevNum),
                      &(pucParamTbl[ulParamTblOffset + ulParamLen]),sizeof(g_stNmParamSet.ucDevNum));
                ulParamLen += sizeof(g_stNmParamSet.ucDevNum);
                break;
            default:pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //�ò��������־??
                break;
            }
            break;
        default:pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //�ò��������־??
            break;
//        ulParamTblOffset ++;                      //���²���ָ���־,ʹָ����һID���ֽ�  
//        ulParamTblLen --;                    //ͳһ��ȥ1���ֽڳ���
        }
        ulParamTblLen -= ulParamLen;
        ulParamTblOffset += ulParamLen;

        if(pstPdu->ucResult == YKPP_ERR_PARAM_ID)
        {
            return ulPduLen; //��������������ٴ���ֱ��ʹ�ô�������ݳ��ȷ���
        }
    }
    ulActualLen += ulParamTblOffset ;        //���ش����ĳ���
    return ulActualLen + 1;                  //����У����ĳ���1
}
/*************************************************
  Function:       
  Description:    ��������������ʼ������־�жϺ�����������ݰ�������
                  ��ʼ������־�����ڴ������ڴ�����ڴ��Ͻ��еģ���˵�
                  �÷����뱣֤�ڴ��㹻����������Խ��
  Calls:          
  Called By:      
  Input:          pucData:    ����ָ�룬ָ����ʼ��־����������Ҳͨ����ָ�����
                  ulDataLen:  ���ݳ���
                  ucCommMode: ͨ��ģʽ��Ŀǰ��ʱ����
  Output:         ulDataLen:  �����º�����ݳ��ȴ���
                  ucDir:      �������ݵķ��򣬱��������
  Return:         FORWARD_UNDESTATE  ��ʾ��Ҫת��(����δ��)
                  SUCCEEDED_DEBUSTATE��ʾ����ɹ�(�����ѵ�)
                  FAILED��ʾ����ʧ��
                  NEED_MORE_OP��ʾ��Ҫ���к�������
  Others:         
*************************************************/
LONG HandleYkppData(UCHAR *pucData, ULONG &ulDataLen, UCHAR ucCommMode, UCHAR &ucDir)
{
    ULONG ulActualLen = 0;
    UCHAR *pucActualData = pucData + 1;                //������ʼ��־
    UCHAR ucNeedToRsp = 0;                             //�Ƿ���ҪӦ�𣬹�д����ʱʹ��,��ʼ��Ϊ"0"��Ҫ��Ӧ

    //���ݱ��뷽�������ݽ��д���
    YKPP_PDU_ST *pstPdu = (YKPP_PDU_ST *)pucActualData;
    ulActualLen = DecodeYkppSchm(pstPdu->ucSchm, pucActualData + 1 , ulDataLen - 3); //�۳���ʼ������־�ͱ��뷽��
    if(ulActualLen == 0)
    {
        return FAILED;
    }
    ulActualLen++; //���ϱ��뷽��
    
    
    //�����ֶ�ָʾ
    if(pstPdu->ucFieldIndicator & FI_AVAILABLE)
    {
        if(pstPdu->ucFieldIndicator & FI_CHECKSUM_NEEDED)
        {
            //У��
            UCHAR ucChecksum = CalcuYkppChecksum(pucActualData, ulActualLen - 1);  //ulActualLenΪ��ȥ��ʼ������־�ĳ���
            if(ucChecksum != pucActualData[ulActualLen - 1])
            {
                //����У�����
                pstPdu->ucResult = YKPP_ERR_CHECKSUM;
                ulDataLen = BuildYkppPdu(pstPdu, ulActualLen);
                ucDir = COMM_IF_NORTH;
                return SUCCEEDED;
            }
        }

        if(pstPdu->ucFieldIndicator & FI_DEV_NUM_AVAILABLE)
        {
            //�ж��豸���
            if(pstPdu->ucDevNum != g_stNmParamSet.ucDevNum)
            {
                //��ѯϵͳ��ŵĲ������⴦��Ϊ��֧������δ֪ϵͳ��ŵ�����²�ѯ�ڶ����ϵͳ���
                //�ϲ���ú�����ⷢ����Ӧ�ı�־���轫�豸��ż�Ȩȥ����ת���²��豸
                
                //�豸��Ų�ͬ�Ҳ�ѯϵͳ���ʱ����Ҫȥ���豸��Ȩ��־�����������ת��
                if((pstPdu->ucOperType == YKPP_READ_REQ)&&(pstPdu->aucParamTbl[1] == 05)&&(pstPdu->aucParamTbl[0] ==01))
                {
                    pstPdu->ucFieldIndicator &= FI_DEV_NUN_AVAILABLE_DIS;    //���ı��뷽��
                    ulDataLen = BuildYkppPdu(pstPdu, ulActualLen);
                    ucDir = COMM_IF_SOUTH;
                    return FORWARD_UNDESTATE;//����Э��һ��ģʽ�±�ʾ��Ҫ���豸��ż�Ȩ�޸ĺ�ת��
                                             //����Э�����ģʽ��ʾδ������δ������                    
                }
                else //��ͨת��
                {
                    ulDataLen = BuildYkppPdu(pstPdu, ulActualLen);
                    ucDir = COMM_IF_SOUTH;
                    return FORWARD_UNDESTATE;      //����Э��һ��ģʽ�±�ʾ��Ҫת��
                                                   //����Э�����ģʽ��ʾδ������δ������
                }

            }
        }
    }

    //����������
    //�������Ĭ��Ϊ��ȷ�������´�������г��ִ���ʱ��Բ�����������޸�
    pstPdu->ucResult = SUCCEEDED;  

    //���ݲ������������д���
    switch(pstPdu->ucOperType)
    {
    case YKPP_READ_REQ:
        //�����ж�����Ӧ��ı�Ҫ��������Ӧ���ִ��ڲ�ѯ�����ٺ�������
        if(g_stYkppParamSet.stYkppCtrlParam.ucYkppRspSw == YKPP_RSP_SW_OFF)  //��������Ӧ�𿪹عر�
        {
            return FAILED;
        }
        ulActualLen = HndlYkppReadReq(pstPdu,ulActualLen);//ulActualLenΪ�۳���ʼ������Ԫ�ĳ���
        pstPdu->ucOperType = YKPP_READ_RSP;
        break;
    case YKPP_WIRTE_REQ:
        ulActualLen = HndlYkppWriteReq(pstPdu,ulActualLen,&ucNeedToRsp);
        //д����ֻ����������ȷ���Ƿ��Ӧ����Ϊһ��д������ܰ����������
        if(ucNeedToRsp != YKPP_NEED_TO_RSP)
        {
            return FAILED;
        }
        pstPdu->ucOperType = YKPP_WIRTE_RSP;        

        break;
    case YKPP_EXE_REQ:
        HndlYkppExeReq(pstPdu,ulActualLen);
        pstPdu->ucOperType = YKPP_EXE_RSP;
        break;
    default:
        pstPdu->ucResult = YKPP_ERR_OPER_TYPE;
        ulDataLen = BuildYkppPdu(pstPdu, ulActualLen);
        ucDir = COMM_IF_NORTH;
        return SUCCEEDED;
    }
    //����Э���漰���޸�(��Ҫ����):
    //1.д�����жԳ��Ҳ������޸�
    //2.д�����ж��ƶ����ܲ���(վ����)���޸�
    //3.ִ�������жԸ澯ʹ�ܵ��޸�
    SaveYkppParam(); 
    

    //�ж��Ƿ���Ҫ�����첽����
    if(g_stSMCParam.usParamLen != 0)
    {
        UCHAR ucIdx = 0;
        if(GetAsynOpCtx(ucIdx) == SUCCEEDED) //�������������ֱ�ӷ���ʧ�ܵ�Ӧ�� 
        {
            //Ԥռ������
            g_astAsynOpCtx[ucIdx].ucState = OH_SEND_DATA_PENDING;  

            //ע���첽�����Ĵ������ͱ���
            if(g_stSMCParam.usParamLen != 0)
            {
                 g_astAsynOpCtx[ucIdx].astOp[g_stSMCParam.ucOpObj - 1].pfOp = DoAsynOpSMC;
                 g_astAsynOpCtx[ucIdx].astOp[g_stSMCParam.ucOpObj - 1].pvCtx= &g_astAsynOpCtx[ucIdx];
                 g_astAsynOpCtx[ucIdx].pfTo = AsynOpYkppToSmc;
            }
            ulDataLen = ulActualLen;
            return NEED_MORE_OP;            
        }
    }
   
    //Ӧ�������(�����첽�����������������)
    ulDataLen = BuildYkppPdu(pstPdu, ulActualLen);
    ucDir = COMM_IF_NORTH;
    return SUCCEEDED_DEBUSTATE;
}

void YkppInit()
{  
    SimpleFlashRead(ADDR_YKPP_PARAM_SET, &g_stYkppParamSet, sizeof(g_stYkppParamSet));

    //���ڲ����浽NVRAM�еĲ����ڳ�ʼ��ʱ��Ҫ����     
    g_stYkppParamSet.stYkppCtrlParam.ucRebootSw = 0;         //0��ʾδ����
    g_stYkppParamSet.stYkppCtrlParam.ucUpdateMode = 0;       //0��ʾ����������ģʽ
    g_stYkppParamSet.stYkppCtrlParam.ucDebugMode = 0;        //0��ʾ�����ڵ���ģʽ

    //MCM-20080329_Zhonghw_begin
    //��13��15�̻��ڳ����У������˲�����ֹ�����û��ͺ�ĳ�ʼ����
    //��Ϊ��ʹ��������������������û���
    UCHAR i;
    UCHAR aucPreFix1[] = "13";
    UCHAR aucPreFix2[] = "15";

    g_stYkppParamSet.stYkppSmsAddrParam.astSmsAddrPrefix[0].ucLen = 2;
    memcpy(g_stYkppParamSet.stYkppSmsAddrParam.astSmsAddrPrefix[0].aucPrefix,aucPreFix1,sizeof(aucPreFix1) - 1);
    g_stYkppParamSet.stYkppSmsAddrParam.astSmsAddrPrefix[1].ucLen = 2;
    memcpy(g_stYkppParamSet.stYkppSmsAddrParam.astSmsAddrPrefix[1].aucPrefix,aucPreFix2,sizeof(aucPreFix2) - 1);
    //MCM-20080329_Zhonghw_end
}

/*************************************************
  Function:
  Description:    ��������ʼ������Ĭ�ϲ���(����Э�����),ϵͳ��⵽�豸����û�б���ʼ�� 
                  (ͨ�����FLASH����Ӧ�ĳ�ʼ��λ)ʱ���ô˺���;���߸ı��豸
                  �������ô˺����������ݻ���������Ӧ�����������
  Calls:
  Called By:
  Input:          
  Output:
  Return:         
  Others:         ���������ⲿ����                  

*************************************************/
void InitYkppDefaultParam()
{
    g_stYkppParamSet.stYkppCommParam.ucSNum = 0;
    g_stYkppParamSet.stYkppCommParam.usSBitmap = 0;

    g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucTotal          = 90;
    g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucOccurLimit     = 36;
    g_stYkppParamSet.stYkppCtrlParam.stAlarmJudgeParam.ucEliminateLimit = 81;

    g_stYkppParamSet.stYkppCtrlParam.ucRebootSw = 0;
    g_stYkppParamSet.stYkppCtrlParam.ucTelNumAuthSw = 1; 
    g_stYkppParamSet.stYkppCtrlParam.stAlarmLimitSw.ucOnOff = 0;    
    g_stYkppParamSet.stYkppCtrlParam.stAlarmLimitSw.ucMaxAlarmPerHour = 0;  
    g_stYkppParamSet.stYkppCtrlParam.ucAddrAuthSw = 1;
    g_stYkppParamSet.stYkppCtrlParam.ucYkppRspSw = 1;
    g_stYkppParamSet.stYkppCtrlParam.ucDlInAcqMode = 0;    

    //MCM-64_20070315_Zhonghw_begin
    g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType= COMM_ROLE_SLAVE;    //0��ʾͨ��������1��ʾͨ�Ŵӻ���Ĭ��Ϊ�ӻ�
    //MCM-64_20070315_Zhonghw_begin
    
    g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ucIsEnabled = 0;
    g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ucOutput = 0;
    g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLogSrcMask = 0;
    g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLevelMask = 0;
    g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulMIDMask = 0;

    //У׼ֵ����?
    g_stYkppParamSet.stYkppRfParam.ucDlInCoupler = 25;

    //MCM-20080329_Zhonghw_begin
    UCHAR i;
    for(i = 0;i < MAX_SMS_ADDR_PREFIX_NUM;i++)
    {
        g_stYkppParamSet.stYkppSmsAddrParam.astSmsAddrPrefix[i].ucLen = 0;//���û���ʱ��ĳ�ʼ��
    }
    //MCM-20080329_Zhonghw_end
}


/*************************************************
  Function:
  Description:    ������������Э��������ò����赽Ӳ���豸�У�
                  ʹ֮��Ч�����ڴ�������յ����ݺ�����������
  Calls:
  Called By:
  Input:          g_usYkppSettingParamBitmap: ���ò���λͼ
  Output:         
  Return:         
  Others:         ���������ⲿ����
*************************************************/
void TakeYkppParamEffect()
{
     
    //��վ�ϱ�
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_STA_INIT))
    {
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_STA_INIT);
        ReportOtherEvent(REPORT_TYPE_YKPP_STA_INIT, 0);
    }
    
    //Ѳ���ϱ�
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_INSPECTION))
    {
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_INSPECTION);
        ReportOtherEvent(REPORT_TYPE_YKPP_INSPECTION, 0);
    }
    
    //�޸�ȷ���ϱ�
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_REPAIR_CONFIRMED))
    {
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_REPAIR_CONFIRMED);
        ReportOtherEvent(REPORT_TYPE_YKPP_REPAIR_CONFIRMED, 0); 
    }
    
    //���ñ���ϱ�
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_CONFIG_CHANGED))
    {
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_CONFIG_CHANGED);
        ReportOtherEvent(REPORT_TYPE_YKPP_CONFIG_CHANGED, 0);
    }
    
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_UP_RF_SW))
    {
        //���ض���;    
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_UP_RF_SW);
    }
    
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_DN_RF_SW))
    {
        //���ض���;    
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_DN_RF_SW);
    }

    //MCM-64_20070315_Zhonghw_begin
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_MODIFY_COMM_SETTING))
    {
        //ͨ������
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_MODIFY_COMM_SETTING);
        MyDelay(1000);
        Restart();
    }
    //MCM-64_20070315_Zhonghw_end
    
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_MODIFY_STANUM_DEVNUM))
    {
        //����Э�����޸�ϵͳ��ţ������ǰ����GPRS���ӣ����Ͽ�����
        if((g_stNmParamSet.ucCommMode == COMM_MODE_GPRS)
          &&(g_stCenterConnStatus.ucLinkStatus == COMM_STATUS_CONNECTED))
        {
            ReconnToCenter();
            CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_MODIFY_STANUM_DEVNUM);
        }
    }
  
}

//MCM-20_20061107_Linwei_begin
/*************************************************
  Function:
  Description:    �����������������¼�
  Calls:
  Called By:
  Input:          pstPrmv: ����������Ӧ����Ϣ����Ϣ�ڴ���ͷ���������
  Output:
  Return:         
  Others:         ���������ⲿ����
*************************************************/
LONG HndlKeyDownReq(KEY_EVENT_ST *pstPrmv)
{
    USHORT usKeyValue = pstPrmv->usKey;
    switch(usKeyValue)
    {
        case KEY_VALUE_STA_INIT:
            SET_BITMAP(g_usYkppSettingParamBitmap,BITMAP_YKPP_STA_INIT);
            break;
        case KEY_VALUE_INSPECTION:
            SET_BITMAP(g_usYkppSettingParamBitmap,BITMAP_YKPP_INSPECTION);
            break;
        case KEY_VALUE_REPAIR_CONFIRMED:
            SET_BITMAP(g_usYkppSettingParamBitmap,BITMAP_YKPP_REPAIR_CONFIRMED);
            break;
        case KEY_VALUE_CONFIG_CHANGED:
            SET_BITMAP(g_usYkppSettingParamBitmap,BITMAP_YKPP_CONFIG_CHANGED);
            break;
        default:
            return FAILED;
    }
    return SUCCEEDED;
}
//MCM-20_20061107_Linwei_end

//MCM-28_20061109_Zhonghw_Begin
/*************************************************
  Function:
  Description:    �������������еĳ���Э�����ݷ��ͳ���
  Calls:
  Called By:
  Input:          pstPrmv:�����е�ԭ������
  Output:         
  Return:         
  Others:
*************************************************/
LONG EncodeAndResponseYkppBuf(OHCH_RECV_DATA_IND_ST *pstPrmv)
{
    YKPP_PDU_ST * pstYkPduData = (YKPP_PDU_ST *)(pstPrmv->pucData + 1); //������ʼ��־

    ULONG ulYkppDataLen = BuildYkppPdu(pstYkPduData, pstPrmv->ulDataLen); //�������ԭʼ���ݵĳ��ȣ���������ʼ������־

    //����ӿڷ���Э������
    OHCH_SEND_DATA_REQ_ST *pstSend = (OHCH_SEND_DATA_REQ_ST *)pstPrmv;
    pstSend->ulPrmvType   = OHCH_SEND_DATA_REQ;
    pstSend->ulMagicNum   = 0;
    pstSend->ucResendTime = RESEND_TIME_DATATYPE_RSP;
    pstSend->ucDstIf      = COMM_IF_NORTH;

    pstSend->ucDataType = DATA_TYPE_RSP;
    pstSend->ulDataLen  = ulYkppDataLen;

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
  Description:    �����������ɶ�ȡ�������ĺ����첽
                  ����ʱ���������ʱ�󽫱������
                  �����Ӧ��ֱ�ӷ���
  Calls:
  Called By:
  Input:          pvCtx: ԭ�������������ָ�룬ʵ��
                         ָ���Ӧ���첽����������
  Output:         
  Return:         
  Others:
*************************************************/
void AsynOpYkppToSmc(void *pvCtx)
{
    ASYN_OP_CTX_ST *pstCtx = (ASYN_OP_CTX_ST *)pvCtx;

    //�������Ӧ������ȡ������
    OHCH_RECV_DATA_IND_ST *pstPrmv = (OHCH_RECV_DATA_IND_ST *)pstCtx->pvData;    

    //MCM-54_20070111_Zhonghw_begin
    //��Ӧ���־��Ϊ��ʱ
    YKPP_PDU_ST * pstYkPduData = (YKPP_PDU_ST *)(pstPrmv->pucData + 1); //������ʼ��־
    pstYkPduData->ucResult = YKPP_ERR_ASYN_TIMEOUT;
    //MCM-54_20070111_Zhonghw_end
    
    EncodeAndResponseYkppBuf(pstPrmv);
    pstCtx->pvData = NULL; //Ϊ�˷�ֹ���շ���δ�յ����ݣ��ڴ�ͱ�ClearAsynOpCtx()�ͷŵ���
                           //�������ʧ�����Ѿ����ڲ��ͷţ�����Ҫ��ClearAsynOpCtx()�ͷ�

    //��ʼ�������ģ��˴�ʹ�������ṹ��ַ����������
    ClearAsynOpCtx(pstCtx - (ASYN_OP_CTX_ST *)&g_astAsynOpCtx);
}
//MCM-28_20061109_Zhonghw_End


