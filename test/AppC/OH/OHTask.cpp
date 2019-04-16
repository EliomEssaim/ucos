/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   OHTask.cpp
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
              ����ȡ�������ĺ���Ļ�Ӧ��ΪOH��ʼ���ͷǳ�ʼ��������֧���зֱ���
    4. ����:  2006/11/07
       ����:  ����
       ����:  �޸�����MCM-20��
              ���Ӱ��������ϱ�����
    4. ����:  2007/03/15
       ����:  �ӻ���
       ����:  �޸�����MCM-64��
              �����Ҳ����ĳ�ʼ�������ƶ�Э���ʼ��֮ǰ����ΪĳЩЭ�����
              �ĳ�ʼ�������ڳ��Ҳ�����
---------------------------------------------------------------------------*/
//****************************����ͷ�ļ�**************************//
#include <string.h>
#include "../include/Interface.h"
#include "../Timer/Timer.h"
#include "../MM/MemMgmt.h"
#include "../AppMain.h"
#include "OH.h"

#ifdef M3
#include "M3Stub.h"
#include "Queue.h"
#endif
//*****************************�궨��*****************************//
#define MAX_OH_BUF_LEN        2048


//OH����ĳ�ʼ��״̬
#define OH_INIT_IDLE
#define OH_INIT_READ_SMC_ADDR           0
#define OH_INIT_CONN_CENTER             1
#define OH_INIT_LOGIN                   2
#define OH_INIT_COMPLETE                3


//**************************ȫ�ֱ�������**************************//
//OH����ĳ�ʼ��״̬
static UCHAR g_ucOhInitState;

//�����첽������������
ASYN_OP_CTX_ST g_astAsynOpCtx[ASYN_OP_CTX_CNT];

//�����첽��������ϢID
UCHAR g_ucAsynOpMsgID; //���ڷ��𷽷�����ϢID��ֻȡ��7λ�����λ�̶�Ϊ0

//�����첽�������ݵĻ�����
ASYN_OP_BUF_ST g_stSECParam;
ASYN_OP_BUF_ST g_stTDParam;
ASYN_OP_SMALL_BUF_ST g_stSMCParam;


//**************************ȫ�ֱ�������**************************//
extern NM_PARAM_SET_ST g_stNmParamSet;
extern CENTER_CONN_STATUS_ST g_stCenterConnStatus;

//****************************��������****************************//
//�ⲿ��������
LONG HndlOHCHRecvDataInd(OHCH_RECV_DATA_IND_ST * &pstPrmv);
LONG HndlOHCHDebgDataInd(OHCH_RECV_DATA_IND_ST *pstPrmv);
LONG HndlOHCHDebgDataReq(OHCH_RECV_DATA_IND_ST *pstPrmv);
void CmccInit();
void YkppInit();
LONG HndlOHCHSendDataCnf(OHCH_SEND_DATA_CNF_ST *pstPrmv);
void TimerExpired(TIMEOUT_EVENT_ST *pstPrmv);
LONG WaitForUpgradeCompletion();
void ReportAlarm();
void TakeParamEffect();
void TakeYkppParamEffect();
UCHAR CheckUpgradeAfterStartup();
LONG RetrieveSmcAddr(OHCH_RW_SMC_ADDR_CNF_ST *pstPrmv);
void HandleRWSmcAddrRsp(OHCH_RW_SMC_ADDR_CNF_ST *pstPrmv);
void HndlOHCHConnCenterCnf(OHCH_CONN_CENTER_CNF_ST *pstPrmv);
void ConnToCenter();
void ReconnToCenter();
void HndlOHCHCsdConnInd(OHCH_CSD_CONN_IND_ST *pstPrmv);
void HndlOHCHSendDataReq(OHCH_SEND_DATA_REQ_ST *pstPrmv);
//MCM-20_20061107_Linwei_begin
LONG HndlKeyDownReq(KEY_EVENT_ST *pstPrmv);
//MCM-20_20061107_Linwei_end

extern "C" void Restart(void);
void DisconnFromCenter();
void MyDelay(USHORT ulTime);
LONG ReportOtherEvent(UCHAR ucEvent, UCHAR ucUpgradeResult);
void HndlMessQueryOrSetRsp(MESSAGE_ST *pstMsg);
void HndlMessageAlarmRep(MESSAGE_ST *pstMsg);

//�ڶ�ʱ������ʱTimer ID������pvCtx���棬��ʱ����Ҫ���䷢��
void OHTimerCallback(void *pvCtx)
{
    //�����ڴ�
    TIMEOUT_EVENT_ST *pstTimeout = (TIMEOUT_EVENT_ST *)MyMalloc(g_pstMemPool16);
    if(pstTimeout == 0)
    {
        return; //û�а취
    }

    pstTimeout->ulPrmvType = TIMEOUT_EVENT;
    pstTimeout->pvPtr = pvCtx;

#ifndef M3
    OSQPost(g_pstOHQue, pstTimeout);
#else
    ULONG aulMsgBuf[4];
    aulMsgBuf[3] = (ULONG)pstTimeout;
    g_pOHQue->WriteQue((MSG_ST *)aulMsgBuf);
#endif
}

Timer* OHCreateTimer(void *pvCtx, ULONG ulInterval, ULONG *pulMagicNum)
{
    return g_TimerMngr.CreateTimer(OHTimerCallback, pvCtx, ulInterval, NONPERIODIC, pulMagicNum);
}

void OHRemoveTimer(Timer *pTimer, ULONG ulMagicNum)
{
    g_TimerMngr.RemoveTimer(pTimer, ulMagicNum);
}

void ClearAsynOpCtx(UCHAR ucIdx)
{
    if(g_astAsynOpCtx[ucIdx].pTimer)
    {
        OHRemoveTimer(g_astAsynOpCtx[ucIdx].pTimer, g_astAsynOpCtx[ucIdx].ulTimerMagicNum);
    }

    g_astAsynOpCtx[ucIdx].pTimer          = 0;
    g_astAsynOpCtx[ucIdx].ulTimerMagicNum = 0;
    g_astAsynOpCtx[ucIdx].ucSn            = 0;
    g_astAsynOpCtx[ucIdx].ucState         = OH_IDLE;
    g_astAsynOpCtx[ucIdx].ucObjBitmap     = 0;
    g_astAsynOpCtx[ucIdx].ulDataLen       = 0;
    MyFree(g_astAsynOpCtx[ucIdx].pvData);
    g_astAsynOpCtx[ucIdx].pvData          = NULL;

    UCHAR ucObjIdx = 0;
    for(; ucObjIdx < MAX_ASYN_OP_OBJS; ucObjIdx++)
    {
        g_astAsynOpCtx[ucIdx].astOp[ucObjIdx].pfOp = NULL;
        g_astAsynOpCtx[ucIdx].astOp[ucObjIdx].pvCtx= NULL;
    }

    g_astAsynOpCtx[ucIdx].pfTo = NULL;
}

void ClearAsynOpBuf()
{
    g_stTDParam.usParamLen = 0;
    g_stSECParam.usParamLen= 0;
    g_stSMCParam.usParamLen= 0;
}

//������Ӧ��������δ����ǰ����
void OHTaskInit()
{
    //��������
#ifndef M3
    g_pstOHQue = OSQCreate(g_apvOHQue, OH_QUE_SIZE);
#else
    g_pOHQue = TMyQue::BuildQue("OHQue", OH_QUE_SIZE);
#endif

    //MCM-64_20070315_Zhonghw_begin
    //��ʼ������Э�鲿��
    YkppInit();

    //��ʼ��Э�鴦����
    CmccInit();
    //MCM-64_20070315_Zhonghw_end

    //��ʼ���첽����������
    for(UCHAR i = 0; i < ASYN_OP_CTX_CNT; i++)
    {
        ClearAsynOpCtx(i);
    }

    //��ʼ���첽�����Ļ�����
    ClearAsynOpBuf();
    g_stSECParam.ucOpObj = ASYN_OP_OBJ_SEC;
    g_stTDParam.ucOpObj  = ASYN_OP_OBJ_TD;
    g_stSMCParam.ucOpObj = ASYN_OP_OBJ_SMC;
}

//��������ϵͳ������ִ�У���ȡ�������ĵ�ַ
void ReadSmcAddr()
{
    UCHAR *pucBuf = (UCHAR *)MyMalloc(g_pstMemPool256);
    if(pucBuf == 0)
    {
        return;
    }

    OHCH_RW_SMC_ADDR_REQ_ST *pstSend = (OHCH_RW_SMC_ADDR_REQ_ST *)pucBuf;
    pstSend->ulPrmvType = OHCH_R_SMC_ADDR_REQ;
    OSQPost(g_pstCHNthQue, pstSend);

    g_ucOhInitState = OH_INIT_READ_SMC_ADDR;
}

/*************************************************
  Function:
  Description:    �����������첽�����������е����ݴ����첽��������
                  �����ͣ����÷���֤�������ݷǿ�
  Calls:
  Called By:
  Input:          stAsynOpBuf:     ����첽���������Ļ�����
                  ucSn:            �����첽�����SN
  Output:         
  Return:         �첽���������λͼ
  Others:
*************************************************/
UCHAR SendAsynOpYkppReq(ASYN_OP_SMALL_BUF_ST &stAsynOpBuf, UCHAR ucSn)
{
    //�����ڴ�
    UCHAR *pucBuf = (UCHAR *)MyMalloc(g_pstMemPool256);
    if(pucBuf == 0)
    {
        return 0; //���ı����ñ�־��׼���´���ִ��
    }

    OHCH_RW_SMC_ADDR_REQ_ST *pstSend = (OHCH_RW_SMC_ADDR_REQ_ST *)pucBuf;
    pstSend->ucMsgSn = ucSn;
    
    if(stAsynOpBuf.ucOpType == NEED_MORE_OP_READ)//���������ĺ���
    {
        pstSend->ulPrmvType = OHCH_R_SMC_ADDR_REQ;
    }
    else if(stAsynOpBuf.ucOpType == NEED_MORE_OP_WRITE)//д�������ĺ���
    {
        pstSend->ulPrmvType = OHCH_W_SMC_ADDR_REQ;
        memcpy(pstSend->acTelNum, (UCHAR *)&stAsynOpBuf.aucParam + 2, stAsynOpBuf.usParamLen - 2);        
    }


    if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
    {
        MyFree((void *) pstSend);
        return 0;
    }
    return stAsynOpBuf.ucOpObj;

}

/*************************************************
  Function:
  Description:    �����������첽�����������е����ݴ����첽��������
                  �����ͣ����÷���֤�������ݷǿ�
  Calls:
  Called By:
  Input:          stAsynOpBuf:     ����첽���������Ļ�����
                  ucSn:            �����첽�����SN
                  g_pstMemPool512: 512�ֽڵ��ڴ��
                  g_pstCHSthQue:   CH�������
  Output:         
  Return:         �첽���������λͼ
  Others:
*************************************************/
UCHAR SendAsynOpParamReq(ASYN_OP_BUF_ST &stAsynOpBuf, UCHAR ucSn)
{
    //�����ڴ�
    MESSAGE_ST *pstMsg = (MESSAGE_ST *)MyMalloc(g_pstMemPool512);
    if(pstMsg == NULL)
    {
        return 0;
    }

    //����ԭ��
    switch(stAsynOpBuf.ucOpType)
    {
    case MCPA_CMD_ID_QUERY:
        pstMsg->ulMessType = MESS_QUERY_REQ;
        break;
    case MCPA_CMD_ID_SET:
        pstMsg->ulMessType = MESS_SET_REQ;
        break;
    default: 
        //�����ܽ��������֧����Ϊ����֮ǰ�Ѿ�������������ж�
        break;
    }    

    pstMsg->ucObj   = stAsynOpBuf.ucOpObj;
    pstMsg->ucMsgSn = ucSn;

    if(stAsynOpBuf.usParamLen > 512 - sizeof(MESSAGE_ST) + 1) //��ֹ��Ϣ���Ȳ��������ɲ���
    {
        pstMsg->usDataLen = 512 - sizeof(MESSAGE_ST) + 1;
    }
    else
    {
        pstMsg->usDataLen = stAsynOpBuf.usParamLen;
    }

    memcpy(pstMsg->aucData, stAsynOpBuf.aucParam, pstMsg->usDataLen);

    //������ӿڷ�����Ϣ
    if(OSQPost(g_pstCHSthQue, pstMsg) != OS_NO_ERR)
    {
        MyFree(pstMsg);
        return 0;
    }

    return stAsynOpBuf.ucOpObj;
}

/*************************************************
  Function:
  Description:    ���������������첽���������ģ������Ҫ����
                  �����ע��ĺ���ͳһ����
  Calls:
  Called By:
  Input:          pstPrmv: ��������ԭ��
  Output:         
  Return:         SUCCEEDED: �ɹ�
                  FAILED:    ʧ��
  Others:
*************************************************/
LONG DoAsynOp(OHCH_RECV_DATA_IND_ST *pstPrmv)
{
    UCHAR ucCtxIdx = 0;
    UCHAR ucObjIdx = 0;
    for(; ucCtxIdx < ASYN_OP_CTX_CNT; ucCtxIdx++)
    {
        if(g_astAsynOpCtx[ucCtxIdx].ucState == OH_SEND_DATA_PENDING)
        {
            //����������
            g_astAsynOpCtx[ucCtxIdx].ucState = OH_WAIT_FOR_RSP;
            g_astAsynOpCtx[ucCtxIdx].ucSn    = GEN_ASYN_OP_MSG_ID(); //��ͬ���첽��������ʹ����ͬ��SN

            //���ûص�����
            for(ucObjIdx = 0; ucObjIdx < MAX_ASYN_OP_OBJS; ucObjIdx++)
            {
                if(g_astAsynOpCtx[ucCtxIdx].astOp[ucObjIdx].pfOp)
                {
                    g_astAsynOpCtx[ucCtxIdx].astOp[ucObjIdx].pfOp(g_astAsynOpCtx[ucCtxIdx].astOp[ucObjIdx].pvCtx);
                }
            }

            if(g_astAsynOpCtx[ucCtxIdx].ucObjBitmap == 0)
            {
                ClearAsynOpCtx(ucCtxIdx);
                return FAILED;
            }
            else
            {
                //������ʱ��
                OH_TIMER_CTX_UN uCtx;
                uCtx.stParam.ucTimerID = TIMER_ASYN_OP_TO_ID;
                uCtx.stParam.ucParam1  = ucCtxIdx; //���ϱ������ĵ��±���Ϊ�������ڳ�ʱ����ʱ�ж�
                g_astAsynOpCtx[ucCtxIdx].pTimer = OHCreateTimer((void *)uCtx.ulParam, TIMER_ASYN_OP_INTV, 
                                                               &(g_astAsynOpCtx[ucCtxIdx].ulTimerMagicNum));
                //g_astAsynOpCtx[ucCtxIdx].ulDataLen = 0; //�ݲ�ʹ��

                //����Ӧ��ԭ��
                g_astAsynOpCtx[ucCtxIdx].pvData = pstPrmv; //����Ӧ��ԭ��
            }
            return SUCCEEDED; //һ������ֻ�����һ���첽����������
        }
    }
    return FAILED;
}

void OHTask(void *)
{
    void *pvPrmv = NULL;
    static UCHAR ucRetrieveSmcAddrFailedCnt = 0; //��ȡ�������ĵ�ַʧ�ܵĴ���
    UCHAR ucUpgradeResult = 0;
    LONG lRetrieveSmcAddrResult = 0;

    //�ж��Ƿ������ɹ�
    ucUpgradeResult = CheckUpgradeAfterStartup();
    
    //��ȡ����Ϣ���ĺ���
    ReadSmcAddr();

    //����ѭ��
    while(1)
    {
    #ifndef M3
        pvPrmv = OSQPend(g_pstOHQue, 0, &g_ucErr);
    #else
        ULONG aulMsgBuf[4];
        g_pOHQue->ReadQue((MSG_ST *)aulMsgBuf, MSG_LEN);
        pvPrmv = (void *)aulMsgBuf[3];
    #endif

        if(pvPrmv == 0)
        {
            continue;
        }

        switch(*((ULONG*)pvPrmv))
        {
        case OHCH_RECV_DATA_IND:
            if(HndlOHCHRecvDataInd((OHCH_RECV_DATA_IND_ST *)pvPrmv) != SUCCEEDED)
            {
                //�������첽������Ҳ���������ݴ�����Ҫ��һ���ж�
                if(DoAsynOp((OHCH_RECV_DATA_IND_ST *)pvPrmv) != SUCCEEDED)
                {
                    MyFree(pvPrmv); //�ͷ��ڴ�
                }

                ClearAsynOpBuf();
            }
            TakeParamEffect();     //�ƶ�Э��
            TakeYkppParamEffect(); //����Э��

            if(WaitForUpgradeCompletion() != 0)
            {
                while(1)
                {
                    //�ȴ�����������ɵ�Ӧ���ȷ��
                #ifndef M3
                    pvPrmv = OSQPend(g_pstOHQue, 0, &g_ucErr);
                #else
                    ULONG aulMsgBuf[4];
                    g_pOHQue->ReadQue((MSG_ST *)aulMsgBuf, MSG_LEN);
                    pvPrmv = (void *)aulMsgBuf[3];
                #endif
                    if(OHCH_SEND_DATA_CNF == *((ULONG*)pvPrmv))
                    {
                        MyFree(pvPrmv);
                        MyDelay(1000); //��CH��������һ��ʱ��

                        //�����GPRS���ӣ�����Ҫ�Ͽ�
                        if(COMM_STATUS_CONNECTED == g_stCenterConnStatus.ucLinkStatus)
                        {
                            DisconnFromCenter();
                            MyDelay(7000); //��CH�Ͽ�GPRS������һ��ʱ��
                        }
                        break;
                    }
                    else
                    {
                        MyFree(pvPrmv);
                    }
                }

                Restart();
            }
            break;
        case OHCH_SEND_DATA_CNF:
            if(HndlOHCHSendDataCnf((OHCH_SEND_DATA_CNF_ST *)pvPrmv) != SUCCEEDED)
            {
                MyFree(pvPrmv); //�ͷ��ڴ�
            }    
            break;
        case TIMEOUT_EVENT:
            TimerExpired((TIMEOUT_EVENT_ST *)pvPrmv);
            
            WaitForUpgradeCompletion(); //��ʱ�϶��������ɹ��������������

            //�ж���GPRS��ʽ����Ӧ��Ĵ���
            if(g_stCenterConnStatus.ucLoginStatus == COMM_STATUS_LOGIN)
            {
                if(g_stCenterConnStatus.ucNoRspCnt > MAX_HEARTBEAT_TO_CNT)
                {
                    ReconnToCenter(); //׼���Ͽ����ӣ����µ�¼
                }
            }
            MyFree(pvPrmv); //�ͷ��ڴ�
            break;
        case ALARM_STATUS_CHANGED_EVENT:
            MyFree(pvPrmv); //�ͷ��ڴ�
            ReportAlarm();
            break;
        case OHCH_SEND_DATA_REQ:
            HndlOHCHSendDataReq((OHCH_SEND_DATA_REQ_ST *)pvPrmv);
            break;
        //����ԭ������    
        case OHCH_UNDE_RECV_IND:   //�յ�CH������δ������
            //�ڴ��ں����ڲ��ͷ�
            HndlOHCHDebgDataInd((OHCH_RECV_DATA_IND_ST *)pvPrmv);
            TakeParamEffect();     //�ƶ�Э��
            TakeYkppParamEffect(); //����Э��
            break;
            
        case OHCH_DEBU_SEND_REQ:   //�յ�CH�������ѵ�����
            //�ڴ��ں����ڲ��ͷ�
            HndlOHCHDebgDataReq((OHCH_RECV_DATA_IND_ST *)pvPrmv);           
            break;

        case MESS_QUERY_RSP:
        case MESS_SET_RSP:
            HndlMessQueryOrSetRsp((MESSAGE_ST *)pvPrmv);
            MyFree(pvPrmv);
            break;
        case MESS_ALARM_REP:
            HndlMessageAlarmRep((MESSAGE_ST *)pvPrmv);
            MyFree(pvPrmv);
            break;
        case OHCH_RW_SMC_ADDR_CNF:
            //�ǳ�ʼ���׶ζ�MODEM�Ĳ���
            if(g_ucOhInitState != OH_INIT_READ_SMC_ADDR)            
            {
                HandleRWSmcAddrRsp((OHCH_RW_SMC_ADDR_CNF_ST *)pvPrmv);
                MyFree(pvPrmv);
            }
            
            //������ڳ�ʼ���׶���Ҫ������һ������
            else if(g_ucOhInitState == OH_INIT_READ_SMC_ADDR)
            {            
                lRetrieveSmcAddrResult = RetrieveSmcAddr((OHCH_RW_SMC_ADDR_CNF_ST *)pvPrmv);
                MyFree(pvPrmv);

                if((lRetrieveSmcAddrResult != SUCCEEDED)
                 &&(ucRetrieveSmcAddrFailedCnt < 3))
                {
                    ucRetrieveSmcAddrFailedCnt++;
                    ReadSmcAddr(); //�����ȡ��������3�������¶�ȡ�������ٶ�ȡ
                }
                else
                {
                    //���������GPRS���ʼ����ɣ�������Ҫ���������ĵ�����
                    if(g_stNmParamSet.ucCommMode != COMM_MODE_GPRS)
                    {
                        g_ucOhInitState = OH_INIT_COMPLETE;
                        //CheckUpgradeAfterStartup();
                        if(UPGRADE_NOT_OCCURED != ucUpgradeResult)
                        {
                            ReportOtherEvent(REPORT_TYPE_UPGRADE, ucUpgradeResult);
                            ucUpgradeResult = UPGRADE_NOT_OCCURED;
                        }
                    }
                    else
                    {
                        ConnToCenter();
                        g_ucOhInitState = OH_INIT_CONN_CENTER;
                    }
                    ucRetrieveSmcAddrFailedCnt = 0;
                }
            }
            break;
        case OHCH_CONN_CENTER_CNF:
            HndlOHCHConnCenterCnf((OHCH_CONN_CENTER_CNF_ST *)pvPrmv);
            MyFree(pvPrmv);
            
            if(g_ucOhInitState == OH_INIT_CONN_CENTER) //������ڳ�ʼ�����̣���Ҫ�����¼�׶�
            {
                g_ucOhInitState = OH_INIT_LOGIN;
            }
            break;
        case OHCH_GPRS_DISCONN_IND:
        case OHCH_CSD_DISCONN_IND: //���ʹ��GPRSͨ�ŷ�ʽ������Ҫ���½��������ĵ�����
            MyFree(pvPrmv);
            //MCM-5_20061101_zhangjie_begin
            if(g_stNmParamSet.ucCommMode == COMM_MODE_GPRS)
            {
                ReconnToCenter();
            }
            //MCM-5_20061101_zhangjie_end
            break;
        case OHCH_GPRS_DISCONN_CNF: //������
            MyFree(pvPrmv);
            break;
        case OHCH_CSD_CONN_IND:
            HndlOHCHCsdConnInd((OHCH_CSD_CONN_IND_ST *)pvPrmv); //�����ڴ���˲��ͷ�
            break;
        //MCM-20_20061107_Linwei_begin
        case KEY_DOWN_EVENT:
            HndlKeyDownReq((KEY_EVENT_ST *)pvPrmv);
            MyFree(pvPrmv); //�ͷ��ڴ�
            TakeYkppParamEffect();            
            break;
        //MCM-20_20061107_Linwei_end
        default:
            MyFree(pvPrmv); //�ͷ��ڴ�
            break;            
        }

        //ֻ���ڵ�һ�ε�¼�ɹ����������ķ�֧
        if((g_ucOhInitState == OH_INIT_LOGIN) && (g_stCenterConnStatus.ucLoginStatus == COMM_STATUS_LOGIN))
        {
            g_ucOhInitState = OH_INIT_COMPLETE;
            
            //�ж��Ƿ������ɹ���ʧ�ܣ��������ͨ��ԭ����޷��ϱ�����������˴�û�а취���ȣ�ֻ�ܵȴ����Ĳ�ѯ
            //CheckUpgradeAfterStartup();
            if(UPGRADE_NOT_OCCURED != ucUpgradeResult)
            {
                ReportOtherEvent(REPORT_TYPE_UPGRADE, ucUpgradeResult);
                ucUpgradeResult = UPGRADE_NOT_OCCURED;
            }
        }
    }
}

LONG GetAsynOpCtx(UCHAR &ucIdx)
{
    for(ucIdx = 0; ucIdx < ASYN_OP_CTX_CNT; ucIdx++)
    {
        if(g_astAsynOpCtx[ucIdx].ucState == OH_IDLE)
        {
            return SUCCEEDED;
        }
    }

    if(ucIdx >= ASYN_OP_CTX_CNT)
    {
        return FAILED;
    }
}

void DoAsynOpTD(void *pvCtx)
{
    ASYN_OP_CTX_ST *pstCtx = (ASYN_OP_CTX_ST *)pvCtx;
    pstCtx->ucObjBitmap |= SendAsynOpParamReq(g_stTDParam, pstCtx->ucSn);//����λͼ
}

void DoAsynOpSEC(void *pvCtx)
{
    ASYN_OP_CTX_ST *pstCtx = (ASYN_OP_CTX_ST *)pvCtx;
    pstCtx->ucObjBitmap |= SendAsynOpParamReq(g_stSECParam, pstCtx->ucSn);//����λͼ
}

void DoAsynOpSMC(void *pvCtx)
{
    ASYN_OP_CTX_ST *pstCtx = (ASYN_OP_CTX_ST *)pvCtx;
    pstCtx->ucObjBitmap |= SendAsynOpYkppReq(g_stSMCParam, pstCtx->ucSn);//����λͼ
}
