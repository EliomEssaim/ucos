/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   YKPP.cpp
    作者:     林雨
    文件说明: 本文件包含实现定时器机制的代码
    其它:
    函数列表:

    历史记录:
    1. 日期:  2006/02/16
       作者:  林雨
       描述:  编码完成
    2. 日期:  2006/11/01
       作者:  章杰
       描述:  修改问题MCM-5，
              在程序中所有判断通信方式或者上报通信方式时只使用"通信方式"这个ID
    3. 日期:  2006/11/3
       作者:  钟华文
       描述:  修改问题MCM-10,
              在处理缓存完所有异步操作的参数后，为操作请求申请上
              下文,并注册不同的操作对象的后续处理、超时处理函数。
    4. 日期:  2006/11/07
       作者:  林玮
       描述:  修改问题MCM-20，
              增加按键触发上报功能
    5. 日期:  2006/11/07
       作者:  钟华文
       描述:  修改问题MCM-28，
              将函数从CMCC.cpp移至YKPP.cpp    
    6. 日期:  2007/01/11
       作者:  钟华文
       描述:  修改问题MCM-54，
              厂家命令异步操作时，默认的操作结果为"YKPP_ERR_ASYN_BUF_FULL"，
              然后根据程序运行的实际结果修改操作结果类型。
    7. 日期:  2007/03/15
       作者:  钟华文
       描述:  修改问题MCM-64，
              增加设备厂家协议中主从通信方式的查询设置，并根据设置值置相应的标志位
              提示设备更改了通信配置。
              在"TakeYkppParamEffect"函数中增加应对通信配置修改后重启设备的处理
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

extern NM_PARAM_SET_ST g_stNmParamSet; //用于判断设备编号
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

//**************************全局变量定义**************************//
YKPP_PARAM_SET_ST g_stYkppParamSet;

//MCM-64_20070315_Zhonghw_begin
USHORT g_usYkppSettingParamBitmap;
//MCM-64_20070315_Zhonghw_end
//用于异步操作的上下文
extern ASYN_OP_CTX_ST g_astAsynOpCtx[ASYN_OP_CTX_CNT];

//用于转义的字符集，定义同移动协议
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

//**************************外部变量声明**************************//
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
  Description:    本函数对接收到的协议数据进行扫描，并将有效的邮科厂家协议数据
                  提取出来，输入的数据只可能包含一个协议数据包，这是由下层保证的，
  Calls:
  Called By:
  Input:          pucInput:     待扫描的数据
                  ulInputLen:   待扫描的数据长度，包含起始结束标志
  Output:         pucOutput:    抽取出的完整的数据包（包含起始结束标志），如果无完整数据包则无效
                  ulOutputLen:  0表示没有查到完整数据包，否则表示完整数据包的长度
  Return:         处理的结果
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

    pucOutput = pucInput + i; //指向起始标志
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
  Description:    本函数计算邮科厂家协议的校验码，校验值等于从起始标志
                  之后的第一字节到校验码前一字节的所有字节相加和的最低字节
  Calls:
  Called By:
  Input:          pucData:     待校验的数据
                  ulDataLen:   待校验的数据长度
  Output:         
  Return:         计算得出的校验码
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
  Description:    本函数根据编码方案对数据进行解码，解码在原来的内存上进行
  Calls:
  Called By:
  Input:          ucSchm:      编码方案
                  pucData:     待解码的数据，不包含编码方案
                  ulDataLen:   待解码的数据长度
  Output:         
  Return:         解码后的数据长度，0表示解码失败
  Others:         
*************************************************/
ULONG DecodeYkppSchm(UCHAR ucSchm, UCHAR *pucData, ULONG ulDataLen)
{
    switch(ucSchm) //指向编码方案
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
  Description:    本函数根据编码方案对数据进行编码，编码在原来的内存上进行
  Calls:
  Called By:
  Input:          ucSchm:      编码方案
                  pucData:     待编码的数据，不包含编码方案
                  ulDataLen:   待编码的数据长度
  Output:         
  Return:         编码后的数据长度，0表示编码失败
  Others:         
*************************************************/
ULONG EncodeYkppSchm(UCHAR ucSchm, UCHAR *pucData, ULONG ulDataLen)
{
    switch(ucSchm) //指向编码方案
    {
    case SCHM_A:
        return SplitInto2Bytes(pucData, ulDataLen); //编码方案不参加二字节拆分
    case SCHM_B:
        return EncodeEscape(pucData, ulDataLen, 512,        //允许最大的转义长度达到512字节
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
  Description:    本函数将协议数据结构编码成最终的字节流，包含起始结束标志
  Calls:
  Called By:
  Input:          pstPdu:     协议数据结构
                  ulPduLen:   协议数据（除起始结束标志以外）长度，
  Output:         
  Return:         编码后的数据长度，0表示编码失败
  Others:         
*************************************************/
ULONG BuildYkppPdu(YKPP_PDU_ST *pstPdu, ULONG ulPduLen)
{
    UCHAR *pucPdu = (UCHAR *)pstPdu;
    if((pstPdu->ucFieldIndicator & FI_AVAILABLE) 
    && (pstPdu->ucFieldIndicator & FI_CHECKSUM_NEEDED)) //如果需要校验
    {
        pucPdu[ulPduLen - 1] = CalcuYkppChecksum(pucPdu, ulPduLen - 1); //原来长度中包含校验，需要扣除
    }
    ULONG ulFinalLen = EncodeYkppSchm(pstPdu->ucSchm, pucPdu + 1, ulPduLen - 1);
    ulFinalLen += 1;    //编码方案单元
    //增加起始结束标志
    *(pucPdu - 1) = YKPP_BEGIN_FLAG;
    *(pucPdu + ulFinalLen) = YKPP_END_FLAG;

    return ulFinalLen + 2;
}


/*************************************************
  Function:
  Description:    本函数用于保存厂家协议操作相关参数，
                  由于FLASH的最小写单位是4k，而站点编号
                  保存在另外一个sector中，因此需要同时更新
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

    SaveWritableParam(); //其实只是更新网管参数中的站点编号，但是由于它们处于同一个sector，因此必须一起更新
}

/*************************************************
  Function:
  Description:    本函处理厂家协议的读请求
  Calls:
  Called By:
  Input:          pstPdu:     协议数据结构
                  ulPduLen:   协议数据（除起始结束标志以外）长度，
  Output:         
  Return:         ulActualLen:处理后的长度       
  Others:         
*************************************************/
ULONG HndlYkppReadReq(YKPP_PDU_ST *pstPdu, ULONG ulPduLen)
{
    UCHAR *pucParamTbl = pstPdu->aucParamTbl;           //指向参数表
    ULONG ulActualLen  = sizeof(YKPP_PDU_ST) - 1;       //用以返回处理后的实际长度，不包含起始结束标志，初始化未结构中除去参数表的长度
    ULONG ulParamTblLen = ulPduLen - ulActualLen - 1;   //为参数表总长，1代表校验码
    ULONG ulParamLen = 0;                               //单个参数的长度
    ULONG ulParamTblOffset = 0;                        //参数表中的偏移

    UCHAR ucParamIDHiByte = 0;
    UCHAR ucParamIDLoByte = 0;

    
    while(ulParamTblLen >= 2)       //至少包含参数ID两个字节 
    {
        ucParamIDHiByte = pucParamTbl[ulParamTblOffset + 1];
        ucParamIDLoByte = pucParamTbl[ulParamTblOffset];

        ulParamTblOffset += 2;     //跳过ID指向参数内容
        ulParamTblLen -= 2;         //减去ID长度  
        switch(ucParamIDHiByte)       
        {
        case YKPP_RF_PARAM_SET:
            switch(ucParamIDLoByte)   
            {
            case YKPP_RF_PARAM_ULRFSW:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.ucUlRfSw;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucUlRfSw);
                //置标志，暂时没有任何动作
                break;
            case YKPP_RF_PARAM_DLRFSW:
                pucParamTbl[ulParamTblOffset] = g_stYkppParamSet.stYkppRfParam.ucDlRfSw;
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucDlRfSw);
                //置标志，暂时没有任何动作
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
                //因为没有提供统一的L，因此无法判断后面的数据
                //出错马上就返回错误，不再进行后续处理            
                break;
            }
            break;
        case YKPP_CTRL_PARAM_SET:
            switch(ucParamIDLoByte)          //指向ID低字节
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
                //短信中心号码的操作只能放置在最前面
                if(ulParamTblOffset > 2)        //确保前面无其他参数
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;
                    return ulPduLen;
                }                
                //触发异步操作
                //MCM-54_20070111_Zhonghw_begin
                pstPdu->ucResult = YKPP_ERR_ASYN_BUF_FULL;
                //MCM-54_20070111_Zhonghw_end
                
                g_stSMCParam.ucOpType = NEED_MORE_OP_READ;
                //复制短信中心号码的的监控参量
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
            //一个读请求如果包含一个调试参数，则不能再包含其它参数。
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
                //获取L的值
                memcpy((CHAR *)&usValueLen,&pucParamTbl[ulParamTblOffset],sizeof(usValueLen));
                ulParamLen = sizeof(usValueLen);
                //获取存储单元地址
                memcpy((CHAR *)&usEepromAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],
                      sizeof(usEepromAddr));
                ulParamLen += sizeof(usEepromAddr);
                //获取读取数据的长度
                memcpy((CHAR *)&usEepromDataLen,&pucParamTbl[ulParamTblOffset + ulParamLen],
                      sizeof(usEepromDataLen));                
                ulParamLen += sizeof(usEepromDataLen);
                
                //长度鉴权
                if(usValueLen != (sizeof(usEepromAddr) + sizeof(usEepromDataLen)))
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //厂家协议读存储器错误
                    return ulPduLen;                    
                }
                //更新V的值
                usValueLen = sizeof(usEepromAddr) + sizeof(usEepromDataLen) + usEepromDataLen;
                memcpy(&pucParamTbl[ulParamTblOffset],(CHAR *)&usValueLen,sizeof(usValueLen));  
                
                //如果传入的可用长度小于拷贝的长度,则不再进行数据读取
                //(待完成)
                //存储器参数操作要保证只含有一个参量
                //只包含一个参量的读请求的完整长度与总长度的比较即可得出是否含有其他参量
                if((ulActualLen + ulParamTblOffset + ulParamLen) == (ulPduLen - 1))        //保证后面没有参数，1为校验码长度
                {   
                #ifndef M3
                    I2cReadMore((void *)(ULONG)usEepromAddr, &pucParamTbl[ulParamTblOffset + ulParamLen], (UCHAR)usEepromDataLen);
                #else
                    //memset(&pucParamTbl[ulParamTblOffset + ulParamLen],1,(CHAR)usEepromDataLen);                    
                #endif    
                    ulParamLen += usEepromDataLen;
                }
                else   //写数据目标长度与数据包中数据长度不符合或者多于一个ID
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //厂家协议读存储器错误
                    return ulPduLen;
                }
                break;
            }    
            case YKPP_MEM_PARAM_ADDRBUSDATA:
            {
                USHORT usValueLen = 0;
                ULONG ulBussAddr = 0;
                USHORT usBussDataLen = 0;
                //获取L的值
                memcpy((CHAR *)&usValueLen,&pucParamTbl[ulParamTblOffset],sizeof(usValueLen));
                ulParamLen = sizeof(usValueLen);
                memcpy((CHAR *)&ulBussAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],
                       sizeof(ulBussAddr));                
                ulParamLen += sizeof(ulBussAddr);
                memcpy((CHAR *)&usBussDataLen,&pucParamTbl[ulParamTblOffset + ulParamLen],
                       sizeof(usBussDataLen));
                ulParamLen += sizeof(usBussDataLen);

                //长度鉴权
                if(usValueLen != (sizeof(ulBussAddr) + sizeof(usBussDataLen) + usBussDataLen))
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //厂家协议写存储器错误
                    return ulPduLen;                    
                }

                //更新V的值
                usValueLen = sizeof(ulBussAddr) + sizeof(usBussDataLen) + usBussDataLen;
                memcpy(&pucParamTbl[ulParamTblOffset],(CHAR *)&usValueLen,sizeof(usValueLen)); 

                //只包含一个参量的写请求的完整长度与总长度的比较即可得出是否含有其他参量
                if((ulActualLen + ulParamTblOffset + ulParamLen) == (ulPduLen - 1))        //保证后面没有参数，1为校验码长度  
                {
                    //根据地址大小来区分是读取FLASH或者RAM
                    //FLASH数据,还要区分NOR/NAN FLASH
                    if((ADDR_FLASH_BEGIN <= ulBussAddr)&&(ulBussAddr <= ADDR_FLASH_END))     
                    {
                        //TEST
                        //memset(&pucParamTbl[ulParamTblOffset + ulParamLen],1,(CHAR)usBussDataLen);
                        SimpleFlashRead(ulBussAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],usBussDataLen);
                    }
                    else //RAM数据
                    {
                        //TEST
                        //memset(&pucParamTbl[ulParamTblOffset + ulParamLen],1,(CHAR)usBussDataLen);
                        memcpy(&pucParamTbl[ulParamTblOffset + ulParamLen],(CHAR *)ulBussAddr,usBussDataLen);                    
                    }
                    ulParamLen += usBussDataLen;
                }
                else   //写数据目标长度与数据包中数据长度不符合或者多于一个ID
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //厂家协议读存储器错误
                    return ulPduLen;
                }
                break;
            }
            default:pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //置参数错误标志??
                    return ulPduLen;
            }
            ulActualLen += ulParamTblOffset;
            ulActualLen += ulParamLen;
            return ulActualLen + 1;           //存储器参数只支持一个参量设置所以返回
                                              //1为校验码的长度
        }
        case YKPP_CORRE_PARAM_SET:
            switch(ucParamIDLoByte)
            {
            case YKPP_CORRE_PARAM_STAADDR:
                //读请求无需置bitmap
                //SET_BITMAP(g_usYkppSettingParamBitmap,BITMAP_YKPP_MODIFY_STANUM_DEVNUM);  
                memcpy(&(pucParamTbl[ulParamTblOffset]),(UCHAR *)&(g_stNmParamSet.ulStaNum),
                      sizeof(g_stNmParamSet.ulStaNum));
                ulParamLen = sizeof(g_stNmParamSet.ulStaNum);
                memcpy( &(pucParamTbl[ulParamTblOffset + ulParamLen]),(UCHAR *)&(g_stNmParamSet.ucDevNum),
                     sizeof(g_stNmParamSet.ucDevNum));
                ulParamLen += sizeof(g_stNmParamSet.ucDevNum);
                break;
            default:pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //置参数错误标志??
                break;
            }
            break;
        case YKPP_DEBUG_PARAM_SET:
            //一个读请求如果包含一个调试参数，则不能再包含其它参数。
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
                ulParamLen += 1 + pucParamTbl[ulParamTblOffset]; //1表示长度所占的字节
            }
                break;
            case YKPP_DEBUG_PARAM_STACK_INFO:
#if MY_CHECK_STACK_EN > 0
                pucParamTbl[ulParamTblOffset] = GetStackInfo(&pucParamTbl[ulParamTblOffset + 1], 256);
                ulParamLen += 1 + pucParamTbl[ulParamTblOffset]; //1表示长度所占的字节
#else
                pucParamTbl[ulParamTblOffset] = 0; //长度为0表示不支持
                ulParamLen += 1; //1表示长度所占的字节
#endif
                break;
            case YKPP_DEBUG_PARAM_MEM_INFO:
#if MY_CHECK_MEM_EN > 0
                pucParamTbl[ulParamTblOffset] = GetMemInfo(&pucParamTbl[ulParamTblOffset + 1], 256);
                ulParamLen += 1 + pucParamTbl[ulParamTblOffset]; //1表示长度所占的字节
#else
                pucParamTbl[ulParamTblOffset] = 0; //长度为0表示不支持
                ulParamLen += 1; //1表示长度所占的字节
#endif
                break;
            case YKPP_DEBUG_PARAM_QUE_INFO:
#if MY_CHECK_QUE_EN > 0
                pucParamTbl[ulParamTblOffset] = GetQueInfo(&pucParamTbl[ulParamTblOffset + 1], 256);
                ulParamLen += 1 + pucParamTbl[ulParamTblOffset]; //1表示长度所占的字节
#else
                pucParamTbl[ulParamTblOffset] = 0; //长度为0表示不支持
                ulParamLen += 1; //1表示长度所占的字节
#endif
                break;
            default:
                pstPdu->ucResult = YKPP_ERR_PARAM_ID;
                break;            
            }
            break;
        default:pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //置参数错误标志??
            break;
        }
        ulParamTblLen -= ulParamLen;             //减去一个ID的TV   
        ulParamTblOffset += ulParamLen;          //指向下一个ID

        if(pstPdu->ucResult == YKPP_ERR_PARAM_ID)
        {
            return ulPduLen; //如果发生错误则不再处理，直接使用传入的数据长度返回
        }
    }
    ulActualLen += ulParamTblOffset ;        //返回处理后的长度
    return ulActualLen + 1;                  //加上校验码的长度1
}
/*************************************************
  Function:
  Description:    本函处理厂家协议的执行请求
  Calls:
  Called By:
  Input:          pstPdu:     协议数据结构
                  ulPduLen:   协议数据（除起始结束标志以外）长度，
  Output:         
  Return:         ulActualLen:处理后的长度       
  Others:         
*************************************************/
LONG HndlYkppExeReq(YKPP_PDU_ST *pstPdu, ULONG ulPduLen)
{
    UCHAR *pucParamTbl = pstPdu->aucParamTbl;           //指向参数表
    ULONG ulActualLen  = sizeof(YKPP_PDU_ST) - 1;       //用以返回处理后的实际长度，不包含起始结束标志，初始化为结构中除去参数表的长度
    ULONG ulParamTblLen = ulPduLen - ulActualLen - 1;   //为参数表总长，1代表校验码
    ULONG ulParamLen = 0;                               //单个参数的长度
    ULONG ulParamTblOffset = 0;                        //参数表中的偏移

    UCHAR ucParamIDHiByte = 0;
    UCHAR ucParamIDLoByte = 0;

   
    while(ulParamTblLen >= 2)
    {
        //一个读请求如果包含一个调试参数，则不能再包含其它参数。
        if(ulParamTblOffset > 0)        //确保前面无其他参数
        {
            pstPdu->ucResult = YKPP_ERR_OPER_FAILED;
            break;
        }
        
        ucParamIDHiByte = pucParamTbl[ulParamTblOffset + 1];
        ucParamIDLoByte = pucParamTbl[ulParamTblOffset];
        ulParamTblOffset += 2;     //跳过ID指向参数内容
        ulParamTblLen -= 2;         //减去ID长度    
        switch(ucParamIDLoByte)
        {
        case YKPP_EXE_PARAM_TOPOLOGY:
            break;
        case YKPP_EXE_PARAM_REPORT_TYPE:
        {
            UCHAR ucReportTpye = pucParamTbl[ulParamTblOffset];
            ulParamLen = sizeof(ucReportTpye);  
            if((ulActualLen + ulParamTblOffset + ulParamLen) == (ulPduLen - 1))//确保后面无其他参数
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
            if((ulActualLen + ulParamTblOffset + ulParamLen) == (ulPduLen - 1))//确保后面无其他参数
            {
                if(ucIsAllAlarmEnableSwOn == 0)
                {
                    //关闭所有告警使能
                    memset(&g_uAlarmEnable, 0, sizeof(g_uAlarmEnable));
                }
                else
                {
                    //打开所有告警使能
                    memset(&g_uAlarmEnable, 1, sizeof(g_uAlarmEnable));
                }
            }
            break;            
        }
        case YKPP_EXE_PARAM_RESET:
            //恢复出厂设置，但是不恢复与中心通信相关的参数
            //比如站点编号、设备编号、上报通信方式
            InitCmccDefaultParam();
            InitYkppDefaultParam();
            break;        
        default:
            pstPdu->ucResult = YKPP_ERR_PARAM_ID;
            break;
        }

        ulParamTblLen -= ulParamLen;             //减去一个ID的TV   
        ulParamTblOffset += ulParamLen;          //指向下一个ID
        if(pstPdu->ucResult == YKPP_ERR_PARAM_ID)
        {
            return ulPduLen; //如果发生错误则不再处理，直接使用传入的数据长度返回
        }    
    }
    ulActualLen += ulParamTblOffset;        //返回处理后的长度
    return ulActualLen + 1;                  //加上校验码的长度1    
}
/*************************************************
  Function:
  Description:    本函处理厂家协议的写请求
  Calls:
  Called By:
  Input:          pstPdu:     协议数据结构
                  ulPduLen:   协议数据（除起始结束标志以外）长度，
                  ucNeedToRsp 协议数据是否需要应答标志
  Output:         
  Return:         ulActualLen:处理后的长度       
  Others:         
*************************************************/
ULONG HndlYkppWriteReq(YKPP_PDU_ST *pstPdu, ULONG ulPduLen,UCHAR *ucNeedToRsp)
{
    UCHAR *pucParamTbl = pstPdu->aucParamTbl;           //指向参数表
    ULONG ulActualLen  = sizeof(YKPP_PDU_ST) - 1;       //用以返回处理后的实际长度，不包含起始结束标志，初始化未结构中除去参数表的长度
    ULONG ulParamTblLen = ulPduLen - ulActualLen - 1;   //为参数表总长，1代表校验码
    ULONG ulParamLen = 0;                               //单个参数的长度
    ULONG ulParamTblOffset = 0;                         //参数表中的偏移

    UCHAR ucParamIDHiByte = 0;
    UCHAR ucParamIDLoByte = 0;

    while(ulParamTblLen >= 2)       //至少包含参数ID两个字节 
    {
        ucParamIDHiByte = pucParamTbl[ulParamTblOffset + 1];
        ucParamIDLoByte = pucParamTbl[ulParamTblOffset];

        ulParamTblOffset += 2;     //跳过ID指向参数内容
        ulParamTblLen -= 2;         //减去ID长度  
        switch(ucParamIDHiByte)       
        {
        case YKPP_RF_PARAM_SET:
            switch(ucParamIDLoByte)   
            {
            case YKPP_RF_PARAM_ULRFSW:
                g_stYkppParamSet.stYkppRfParam.ucUlRfSw = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucUlRfSw);
                //置标志，暂时没有任何动作
                break;
            case YKPP_RF_PARAM_DLRFSW:
                g_stYkppParamSet.stYkppRfParam.ucDlRfSw = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppRfParam.ucDlRfSw);
                //置标志，暂时没有任何动作
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
                //因为没有提供统一的L，因此无法判断后面的数据
                //出错马上就返回错误，不再进行后续处理            
                break;
            }
            break;
        case YKPP_CTRL_PARAM_SET:
            switch(ucParamIDLoByte)          //指向ID低字节
            {
            case YKPP_CTRL_PARAM_REBOOT_SW:
                g_stYkppParamSet.stYkppCtrlParam.ucRebootSw = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucRebootSw);
                //重启开关作用是?
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
                //置在线升级模式有效后是否需要特殊动作?
                break;
            case YKPP_CTRL_PARAM_DEBUG_MODE:
                g_stYkppParamSet.stYkppCtrlParam.ucDebugMode = pucParamTbl[ulParamTblOffset];
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucDebugMode);
                //因为调试模式没有明显的切换动作，只通过标志位区分,所以不置bitmap
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
                //比较一下是否设置新值
                if(memcmp(&g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType,
                          &pucParamTbl[ulParamTblOffset],
                          sizeof(g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType)))
                {
                    //使用厂家协议修改T通信主从功能参数要重启
                    SET_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_MODIFY_COMM_SETTING);                
                    g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType= pucParamTbl[ulParamTblOffset];
                }
                ulParamLen = sizeof(g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType);                                
                break;                                  
            //MCM-64_20070315_Zhonghw_begin                                
            default: pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //置参数错误标志??
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
               //短信中心号码的操作只能放置在最前面
               if(ulParamTblOffset > 2)        //确保前面无其他参数
               {
                   pstPdu->ucResult = YKPP_ERR_OPER_FAILED;
                   return ulPduLen;
               }                
                //触发异步操作
                //MCM-54_20070111_Zhonghw_begin
                pstPdu->ucResult = YKPP_ERR_ASYN_BUF_FULL;
                //MCM-54_20070111_Zhonghw_end
                
                g_stSMCParam.ucOpType = NEED_MORE_OP_WRITE;
                
                //复制短信中心号码的的监控参量
                memcpy(g_stSMCParam.aucParam + g_stSMCParam.usParamLen, &(pucParamTbl[ulParamTblOffset - 2]), 
                      sizeof(g_stNmParamSet.acSmcAddr) + 2);
                g_stSMCParam.usParamLen += sizeof(g_stNmParamSet.acSmcAddr) + 2;
                
                ulParamLen = sizeof(g_stNmParamSet.acSmcAddr);
                break;
            default: pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //置参数错误标志??
                break;
            }
            break;
        case YKPP_MEM_PARAM_SET:
        {
            //一个读请求如果包含一个调试参数，则不能再包含其它参数。
            if(ulParamTblOffset > 2)        //保证前面不包含其他参数
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
                //获取L的值
                memcpy((CHAR *)&usValueLen,&pucParamTbl[ulParamTblOffset],sizeof(usValueLen));
                ulParamLen = sizeof(usValueLen);
                //获取存储单元地址
                memcpy((CHAR *)&usEepromAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],
                      sizeof(usEepromAddr));
                ulParamLen += sizeof(usEepromAddr);
                //获取读取数据的长度
                memcpy((CHAR *)&usEepromDataLen,&pucParamTbl[ulParamTblOffset + ulParamLen],
                      sizeof(usEepromDataLen));                
                ulParamLen += sizeof(usEepromDataLen);

                //长度鉴权
                if(usValueLen != (sizeof(usEepromAddr) + sizeof(usEepromDataLen) + usEepromDataLen))
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //厂家协议读存储器错误
                    return ulPduLen;                    
                }
                //如果传入的可用长度小于拷贝的长度,则不再进行数据读取
                //(待完成)
                //如何保证只包含一个参数??
                //只包含一个参量的写请求的完整长度与总长度的比较即可得出是否含有其他参量
                if((ulActualLen + ulParamTblOffset + ulParamLen + usEepromDataLen) == (ulPduLen - 1))        //保证后面没有参数，1为校验码长度
                {
                #ifndef M3
                    I2cWriteSlow((void *)(ULONG)usEepromAddr, &pucParamTbl[ulParamTblOffset + ulParamLen], (UCHAR)usEepromDataLen);
                #else    
                    memset(&pucParamTbl[ulParamTblOffset + ulParamLen],1,(CHAR)usEepromDataLen);
                #endif                    
                    ulParamLen += usEepromDataLen;
                }
                else   //写数据目标长度与数据包中数据长度不符合或者多于一个ID
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //厂家协议写存储器错误
                    return ulPduLen;
                }
                break;
            }    
            case YKPP_MEM_PARAM_ADDRBUSDATA:
            {
                USHORT usValueLen = 0;
                ULONG ulBussAddr = 0;
                USHORT usBussDataLen = 0;
                //获取L的值
                memcpy((CHAR *)&usValueLen,&pucParamTbl[ulParamTblOffset],sizeof(usValueLen));
                ulParamLen = sizeof(usValueLen);
                memcpy((CHAR *)&ulBussAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],
                       sizeof(ulBussAddr));                
                ulParamLen += sizeof(ulBussAddr);
                memcpy((CHAR *)&usBussDataLen,&pucParamTbl[ulParamTblOffset + ulParamLen],
                       sizeof(usBussDataLen));
                ulParamLen += sizeof(usBussDataLen);

                //长度鉴权
                if(usValueLen != (sizeof(ulBussAddr) + sizeof(usBussDataLen) + usBussDataLen))
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //厂家协议写存储器错误
                    return ulPduLen;                    
                }
                //只包含一个参量的写请求的完整长度与总长度的比较即可得出是否含有其他参量
                if((ulActualLen + ulParamTblOffset + ulParamLen + usBussDataLen) == (ulPduLen - 1))        //保证后面没有参数，1为校验码长度
                {
                    //根据地址大小来区分是读取FLASH或者RAM
                    if((ADDR_FLASH_BEGIN <= ulBussAddr)&&(ulBussAddr <= ADDR_FLASH_END))      //FLASH数据,还要区分NOR/NAN FLASH
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
                    else //RAM数据
                    {
                        //TEST
                        //memset(&pucParamTbl[ulParamTblOffset + ulParamLen],1,(CHAR)usBussDataLen);                        
                        memcpy((CHAR *)ulBussAddr,&pucParamTbl[ulParamTblOffset + ulParamLen],usBussDataLen);                    
                    }
                    ulParamLen += usBussDataLen;
                }
                else   //写数据目标长度与数据包中数据长度不符合或者多于一个ID
                {
                    pstPdu->ucResult = YKPP_ERR_OPER_FAILED;           //厂家协议写存储器错误
                    return ulPduLen;
                }
                break;
            }
            default:pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //置参数错误标志??
                    return ulPduLen;
            }
            ulActualLen += ulParamTblOffset;
            ulActualLen += ulParamLen;
            return ulActualLen + 1;           //存储器参数只支持一个参量设置所以返回
                                              //1为校验码的长度
        }
        case YKPP_CORRE_PARAM_SET:
            switch(ucParamIDLoByte)
            {
            case YKPP_CORRE_PARAM_STAADDR:
                //置修改站点编号重启标记
                //使用厂家协议修改站点设备编号，要重启
                SET_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_MODIFY_STANUM_DEVNUM);
                memcpy((UCHAR *)&(g_stNmParamSet.ulStaNum),
                      &(pucParamTbl[ulParamTblOffset]),sizeof(g_stNmParamSet.ulStaNum));
                ulParamLen = sizeof(g_stNmParamSet.ulStaNum);
                memcpy((UCHAR *)&(g_stNmParamSet.ucDevNum),
                      &(pucParamTbl[ulParamTblOffset + ulParamLen]),sizeof(g_stNmParamSet.ucDevNum));
                ulParamLen += sizeof(g_stNmParamSet.ucDevNum);
                break;
            default:pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //置参数错误标志??
                break;
            }
            break;
        default:pstPdu->ucResult = YKPP_ERR_PARAM_ID;           //置参数错误标志??
            break;
//        ulParamTblOffset ++;                      //更新参数指向标志,使指向下一ID低字节  
//        ulParamTblLen --;                    //统一减去1个字节长度
        }
        ulParamTblLen -= ulParamLen;
        ulParamTblOffset += ulParamLen;

        if(pstPdu->ucResult == YKPP_ERR_PARAM_ID)
        {
            return ulPduLen; //如果发生错误则不再处理，直接使用传入的数据长度返回
        }
    }
    ulActualLen += ulParamTblOffset ;        //返回处理后的长度
    return ulActualLen + 1;                  //加上校验码的长度1
}
/*************************************************
  Function:       
  Description:    本函数处理经过起始结束标志判断后的完整的数据包，包含
                  起始结束标志。由于处理是在传入的内存上进行的，因此调
                  用方必须保证内存足够大而不会访问越界
  Calls:          
  Called By:      
  Input:          pucData:    数据指针，指向起始标志，返回数据也通过该指针访问
                  ulDataLen:  数据长度
                  ucCommMode: 通信模式，目前暂时不用
  Output:         ulDataLen:  将更新后的数据长度传出
                  ucDir:      发送数据的方向，北向或南向
  Return:         FORWARD_UNDESTATE  表示需要转发(数据未调)
                  SUCCEEDED_DEBUSTATE表示处理成功(数据已调)
                  FAILED表示处理失败
                  NEED_MORE_OP表示需要进行后续处理
  Others:         
*************************************************/
LONG HandleYkppData(UCHAR *pucData, ULONG &ulDataLen, UCHAR ucCommMode, UCHAR &ucDir)
{
    ULONG ulActualLen = 0;
    UCHAR *pucActualData = pucData + 1;                //跳过起始标志
    UCHAR ucNeedToRsp = 0;                             //是否需要应答，供写请求时使用,初始化为"0"需要回应

    //根据编码方案对数据进行处理
    YKPP_PDU_ST *pstPdu = (YKPP_PDU_ST *)pucActualData;
    ulActualLen = DecodeYkppSchm(pstPdu->ucSchm, pucActualData + 1 , ulDataLen - 3); //扣除起始结束标志和编码方案
    if(ulActualLen == 0)
    {
        return FAILED;
    }
    ulActualLen++; //加上编码方案
    
    
    //处理字段指示
    if(pstPdu->ucFieldIndicator & FI_AVAILABLE)
    {
        if(pstPdu->ucFieldIndicator & FI_CHECKSUM_NEEDED)
        {
            //校验
            UCHAR ucChecksum = CalcuYkppChecksum(pucActualData, ulActualLen - 1);  //ulActualLen为除去起始结束标志的长度
            if(ucChecksum != pucActualData[ulActualLen - 1])
            {
                //返回校验错误
                pstPdu->ucResult = YKPP_ERR_CHECKSUM;
                ulDataLen = BuildYkppPdu(pstPdu, ulActualLen);
                ucDir = COMM_IF_NORTH;
                return SUCCEEDED;
            }
        }

        if(pstPdu->ucFieldIndicator & FI_DEV_NUM_AVAILABLE)
        {
            //判断设备编号
            if(pstPdu->ucDevNum != g_stNmParamSet.ucDevNum)
            {
                //查询系统编号的操作特殊处理，为了支持能在未知系统编号的情况下查询第二层的系统编号
                //上层调用函数检测发现相应的标志后，需将设备编号鉴权去除后转发下层设备
                
                //设备编号不同且查询系统编号时，需要去除设备鉴权标志后重新组包并转发
                if((pstPdu->ucOperType == YKPP_READ_REQ)&&(pstPdu->aucParamTbl[1] == 05)&&(pstPdu->aucParamTbl[0] ==01))
                {
                    pstPdu->ucFieldIndicator &= FI_DEV_NUN_AVAILABLE_DIS;    //更改编码方案
                    ulDataLen = BuildYkppPdu(pstPdu, ulActualLen);
                    ucDir = COMM_IF_SOUTH;
                    return FORWARD_UNDESTATE;//厂家协议一般模式下表示需要将设备编号鉴权修改后转发
                                             //厂家协议调试模式表示未调进来未调返回                    
                }
                else //普通转发
                {
                    ulDataLen = BuildYkppPdu(pstPdu, ulActualLen);
                    ucDir = COMM_IF_SOUTH;
                    return FORWARD_UNDESTATE;      //厂家协议一般模式下表示需要转发
                                                   //厂家协议调试模式表示未调进来未调返回
                }

            }
        }
    }

    //处理操作结果
    //操作结果默认为正确，在以下处理过程中出现错误时候对操作结果进行修改
    pstPdu->ucResult = SUCCEEDED;  

    //根据操作类型来进行处理
    switch(pstPdu->ucOperType)
    {
    case YKPP_READ_REQ:
        //首先判断有无应答的必要，如无需应答，又处于查询，则不再后续动作
        if(g_stYkppParamSet.stYkppCtrlParam.ucYkppRspSw == YKPP_RSP_SW_OFF)  //厂家命令应答开关关闭
        {
            return FAILED;
        }
        ulActualLen = HndlYkppReadReq(pstPdu,ulActualLen);//ulActualLen为扣除起始结束单元的长度
        pstPdu->ucOperType = YKPP_READ_RSP;
        break;
    case YKPP_WIRTE_REQ:
        ulActualLen = HndlYkppWriteReq(pstPdu,ulActualLen,&ucNeedToRsp);
        //写请求只能最外层才能确定是否回应，因为一个写请求可能包含多个操作
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
    //厂家协议涉及的修改(需要保存):
    //1.写请求中对厂家参数的修改
    //2.写请求中对移动网管参数(站点编号)的修改
    //3.执行请求中对告警使能的修改
    SaveYkppParam(); 
    

    //判断是否需要进行异步操作
    if(g_stSMCParam.usParamLen != 0)
    {
        UCHAR ucIdx = 0;
        if(GetAsynOpCtx(ucIdx) == SUCCEEDED) //如果上下文满则直接返回失败的应答 
        {
            //预占上下文
            g_astAsynOpCtx[ucIdx].ucState = OH_SEND_DATA_PENDING;  

            //注册异步操作的处理函数和变量
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
   
    //应答包编码(包含异步操作上下文满的情况)
    ulDataLen = BuildYkppPdu(pstPdu, ulActualLen);
    ucDir = COMM_IF_NORTH;
    return SUCCEEDED_DEBUSTATE;
}

void YkppInit()
{  
    SimpleFlashRead(ADDR_YKPP_PARAM_SET, &g_stYkppParamSet, sizeof(g_stYkppParamSet));

    //对于不保存到NVRAM中的参数在初始化时需要清零     
    g_stYkppParamSet.stYkppCtrlParam.ucRebootSw = 0;         //0表示未开启
    g_stYkppParamSet.stYkppCtrlParam.ucUpdateMode = 0;       //0表示不处在升级模式
    g_stYkppParamSet.stYkppCtrlParam.ucDebugMode = 0;        //0表示不处在调试模式

    //MCM-20080329_Zhonghw_begin
    //将13和15固化在程序中，不将此操作防止在设置机型后的初始化中
    //是为了使升级程序后无需重新设置机型
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
  Description:    本函数初始化出厂默认参数(厂家协议相关),系统监测到设备从来没有被初始化 
                  (通过检查FLASH中相应的初始化位)时调用此函数;或者改变设备
                  类别后会调用此函数，并根据机型配置相应的特殊参数。
  Calls:
  Called By:
  Input:          
  Output:
  Return:         
  Others:         本函数供外部调用                  

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
    g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType= COMM_ROLE_SLAVE;    //0表示通信主机，1表示通信从机，默认为从机
    //MCM-64_20070315_Zhonghw_begin
    
    g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ucIsEnabled = 0;
    g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ucOutput = 0;
    g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLogSrcMask = 0;
    g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulLevelMask = 0;
    g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulMIDMask = 0;

    //校准值参数?
    g_stYkppParamSet.stYkppRfParam.ucDlInCoupler = 25;

    //MCM-20080329_Zhonghw_begin
    UCHAR i;
    for(i = 0;i < MAX_SMS_ADDR_PREFIX_NUM;i++)
    {
        g_stYkppParamSet.stYkppSmsAddrParam.astSmsAddrPrefix[i].ucLen = 0;//设置机型时候的初始化
    }
    //MCM-20080329_Zhonghw_end
}


/*************************************************
  Function:
  Description:    本函数将厂家协议相关设置参数设到硬件设备中，
                  使之生效，将在处理完接收的数据后于最外层调用
  Calls:
  Called By:
  Input:          g_usYkppSettingParamBitmap: 设置参数位图
  Output:         
  Return:         
  Others:         本函数供外部调用
*************************************************/
void TakeYkppParamEffect()
{
     
    //开站上报
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_STA_INIT))
    {
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_STA_INIT);
        ReportOtherEvent(REPORT_TYPE_YKPP_STA_INIT, 0);
    }
    
    //巡检上报
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_INSPECTION))
    {
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_INSPECTION);
        ReportOtherEvent(REPORT_TYPE_YKPP_INSPECTION, 0);
    }
    
    //修复确认上报
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_REPAIR_CONFIRMED))
    {
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_REPAIR_CONFIRMED);
        ReportOtherEvent(REPORT_TYPE_YKPP_REPAIR_CONFIRMED, 0); 
    }
    
    //配置变更上报
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_CONFIG_CHANGED))
    {
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_CONFIG_CHANGED);
        ReportOtherEvent(REPORT_TYPE_YKPP_CONFIG_CHANGED, 0);
    }
    
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_UP_RF_SW))
    {
        //开关动作;    
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_UP_RF_SW);
    }
    
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_DN_RF_SW))
    {
        //开关动作;    
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_DN_RF_SW);
    }

    //MCM-64_20070315_Zhonghw_begin
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_MODIFY_COMM_SETTING))
    {
        //通信配置
        CLEAR_BITMAP(g_usYkppSettingParamBitmap, BITMAP_YKPP_MODIFY_COMM_SETTING);
        MyDelay(1000);
        Restart();
    }
    //MCM-64_20070315_Zhonghw_end
    
    if(IS_BITMAP_SET(g_usYkppSettingParamBitmap, BITMAP_YKPP_MODIFY_STANUM_DEVNUM))
    {
        //厂家协议下修改系统编号，如果当前处于GPRS连接，则会断开重连
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
  Description:    本函数处理按键触发事件
  Calls:
  Called By:
  Input:          pstPrmv: 器件发来的应答消息，消息内存的释放在外层进行
  Output:
  Return:         
  Others:         本函数供外部调用
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
  Description:    本函数将缓存中的厂家协议数据发送出来
  Calls:
  Called By:
  Input:          pstPrmv:缓存中的原语数据
  Output:         
  Return:         
  Others:
*************************************************/
LONG EncodeAndResponseYkppBuf(OHCH_RECV_DATA_IND_ST *pstPrmv)
{
    YKPP_PDU_ST * pstYkPduData = (YKPP_PDU_ST *)(pstPrmv->pucData + 1); //跳过起始标志

    ULONG ulYkppDataLen = BuildYkppPdu(pstYkPduData, pstPrmv->ulDataLen); //输入的是原始数据的长度，不包含起始结束标志

    //向北向接口发送协议数据
    OHCH_SEND_DATA_REQ_ST *pstSend = (OHCH_SEND_DATA_REQ_ST *)pstPrmv;
    pstSend->ulPrmvType   = OHCH_SEND_DATA_REQ;
    pstSend->ulMagicNum   = 0;
    pstSend->ucResendTime = RESEND_TIME_DATATYPE_RSP;
    pstSend->ucDstIf      = COMM_IF_NORTH;

    pstSend->ucDataType = DATA_TYPE_RSP;
    pstSend->ulDataLen  = ulYkppDataLen;

    //将原语发送给CH
    if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
    {
        MyFree(pstSend);
        return FAILED;
    }
    return SUCCEEDED;
}

/*************************************************
  Function:
  Description:    本函数处理由读取短信中心号码异步
                  请求超时的情况，超时后将保存的已
                  处理的应答直接发送
  Calls:
  Called By:
  Input:          pvCtx: 原来保存的上下文指针，实际
                         指向对应的异步操作上下文
  Output:         
  Return:         
  Others:
*************************************************/
void AsynOpYkppToSmc(void *pvCtx)
{
    ASYN_OP_CTX_ST *pstCtx = (ASYN_OP_CTX_ST *)pvCtx;

    //将缓存的应答数据取出发送
    OHCH_RECV_DATA_IND_ST *pstPrmv = (OHCH_RECV_DATA_IND_ST *)pstCtx->pvData;    

    //MCM-54_20070111_Zhonghw_begin
    //将应答标志置为超时
    YKPP_PDU_ST * pstYkPduData = (YKPP_PDU_ST *)(pstPrmv->pucData + 1); //跳过起始标志
    pstYkPduData->ucResult = YKPP_ERR_ASYN_TIMEOUT;
    //MCM-54_20070111_Zhonghw_end
    
    EncodeAndResponseYkppBuf(pstPrmv);
    pstCtx->pvData = NULL; //为了防止接收方还未收到数据，内存就被ClearAsynOpCtx()释放掉；
                           //如果返回失败则已经在内部释放，不需要被ClearAsynOpCtx()释放

    //初始化上下文，此处使用两个结构地址相减获得索引
    ClearAsynOpCtx(pstCtx - (ASYN_OP_CTX_ST *)&g_astAsynOpCtx);
}
//MCM-28_20061109_Zhonghw_End


