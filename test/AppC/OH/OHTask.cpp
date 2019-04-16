/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   OHTask.cpp
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
              将读取短信中心号码的回应分为OH初始化和非初始化两个分支进行分别处理
    4. 日期:  2006/11/07
       作者:  林玮
       描述:  修改问题MCM-20，
              增加按键触发上报功能
    4. 日期:  2007/03/15
       作者:  钟华文
       描述:  修改问题MCM-64，
              将厂家参数的初始化调置移动协议初始化之前，因为某些协议参数
              的初始化依赖于厂家参数。
---------------------------------------------------------------------------*/
//****************************包含头文件**************************//
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
//*****************************宏定义*****************************//
#define MAX_OH_BUF_LEN        2048


//OH任务的初始化状态
#define OH_INIT_IDLE
#define OH_INIT_READ_SMC_ADDR           0
#define OH_INIT_CONN_CENTER             1
#define OH_INIT_LOGIN                   2
#define OH_INIT_COMPLETE                3


//**************************全局变量定义**************************//
//OH任务的初始化状态
static UCHAR g_ucOhInitState;

//用于异步操作的上下文
ASYN_OP_CTX_ST g_astAsynOpCtx[ASYN_OP_CTX_CNT];

//用于异步操作的消息ID
UCHAR g_ucAsynOpMsgID; //用于发起方分配消息ID，只取低7位，最高位固定为0

//保存异步操作数据的缓冲区
ASYN_OP_BUF_ST g_stSECParam;
ASYN_OP_BUF_ST g_stTDParam;
ASYN_OP_SMALL_BUF_ST g_stSMCParam;


//**************************全局变量申明**************************//
extern NM_PARAM_SET_ST g_stNmParamSet;
extern CENTER_CONN_STATUS_ST g_stCenterConnStatus;

//****************************函数申明****************************//
//外部函数申明
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

//在定时器创建时Timer ID被当作pvCtx保存，超时后需要将其发回
void OHTimerCallback(void *pvCtx)
{
    //申请内存
    TIMEOUT_EVENT_ST *pstTimeout = (TIMEOUT_EVENT_ST *)MyMalloc(g_pstMemPool16);
    if(pstTimeout == 0)
    {
        return; //没有办法
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

//本函数应该在任务未运行前调用
void OHTaskInit()
{
    //创建队列
#ifndef M3
    g_pstOHQue = OSQCreate(g_apvOHQue, OH_QUE_SIZE);
#else
    g_pOHQue = TMyQue::BuildQue("OHQue", OH_QUE_SIZE);
#endif

    //MCM-64_20070315_Zhonghw_begin
    //初始化厂家协议部分
    YkppInit();

    //初始化协议处理部分
    CmccInit();
    //MCM-64_20070315_Zhonghw_end

    //初始化异步操作上下文
    for(UCHAR i = 0; i < ASYN_OP_CTX_CNT; i++)
    {
        ClearAsynOpCtx(i);
    }

    //初始化异步操作的缓冲区
    ClearAsynOpBuf();
    g_stSECParam.ucOpObj = ASYN_OP_OBJ_SEC;
    g_stTDParam.ucOpObj  = ASYN_OP_OBJ_TD;
    g_stSMCParam.ucOpObj = ASYN_OP_OBJ_SMC;
}

//本函数在系统启动后执行，读取短信中心地址
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
  Description:    本函数根据异步操作缓冲区中的数据创建异步操作请求
                  并发送，调用方保证缓存数据非空
  Calls:
  Called By:
  Input:          stAsynOpBuf:     存放异步操作参数的缓冲区
                  ucSn:            发送异步请求的SN
  Output:         
  Return:         异步操作对象的位图
  Others:
*************************************************/
UCHAR SendAsynOpYkppReq(ASYN_OP_SMALL_BUF_ST &stAsynOpBuf, UCHAR ucSn)
{
    //申请内存
    UCHAR *pucBuf = (UCHAR *)MyMalloc(g_pstMemPool256);
    if(pucBuf == 0)
    {
        return 0; //不改变设置标志，准备下次再执行
    }

    OHCH_RW_SMC_ADDR_REQ_ST *pstSend = (OHCH_RW_SMC_ADDR_REQ_ST *)pucBuf;
    pstSend->ucMsgSn = ucSn;
    
    if(stAsynOpBuf.ucOpType == NEED_MORE_OP_READ)//读短信中心号码
    {
        pstSend->ulPrmvType = OHCH_R_SMC_ADDR_REQ;
    }
    else if(stAsynOpBuf.ucOpType == NEED_MORE_OP_WRITE)//写短信中心号码
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
  Description:    本函数根据异步操作缓冲区中的数据创建异步操作请求
                  并发送，调用方保证缓存数据非空
  Calls:
  Called By:
  Input:          stAsynOpBuf:     存放异步操作参数的缓冲区
                  ucSn:            发送异步请求的SN
                  g_pstMemPool512: 512字节的内存池
                  g_pstCHSthQue:   CH南向队列
  Output:         
  Return:         异步操作对象的位图
  Others:
*************************************************/
UCHAR SendAsynOpParamReq(ASYN_OP_BUF_ST &stAsynOpBuf, UCHAR ucSn)
{
    //申请内存
    MESSAGE_ST *pstMsg = (MESSAGE_ST *)MyMalloc(g_pstMemPool512);
    if(pstMsg == NULL)
    {
        return 0;
    }

    //构造原语
    switch(stAsynOpBuf.ucOpType)
    {
    case MCPA_CMD_ID_QUERY:
        pstMsg->ulMessType = MESS_QUERY_REQ;
        break;
    case MCPA_CMD_ID_SET:
        pstMsg->ulMessType = MESS_SET_REQ;
        break;
    default: 
        //不可能进入这个分支，因为调用之前已经对命令进行了判断
        break;
    }    

    pstMsg->ucObj   = stAsynOpBuf.ucOpObj;
    pstMsg->ucMsgSn = ucSn;

    if(stAsynOpBuf.usParamLen > 512 - sizeof(MESSAGE_ST) + 1) //防止消息长度不足以容纳参数
    {
        pstMsg->usDataLen = 512 - sizeof(MESSAGE_ST) + 1;
    }
    else
    {
        pstMsg->usDataLen = stAsynOpBuf.usParamLen;
    }

    memcpy(pstMsg->aucData, stAsynOpBuf.aucParam, pstMsg->usDataLen);

    //向南向接口发送消息
    if(OSQPost(g_pstCHSthQue, pstMsg) != OS_NO_ERR)
    {
        MyFree(pstMsg);
        return 0;
    }

    return stAsynOpBuf.ucOpObj;
}

/*************************************************
  Function:
  Description:    本函数遍历所有异步操作上下文，如果需要处理
                  则调用注册的函数统一处理
  Calls:
  Called By:
  Input:          pstPrmv: 接收数据原语
  Output:         
  Return:         SUCCEEDED: 成功
                  FAILED:    失败
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
            //更新上下文
            g_astAsynOpCtx[ucCtxIdx].ucState = OH_WAIT_FOR_RSP;
            g_astAsynOpCtx[ucCtxIdx].ucSn    = GEN_ASYN_OP_MSG_ID(); //不同的异步操作对象使用相同的SN

            //调用回调操作
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
                //启动定时器
                OH_TIMER_CTX_UN uCtx;
                uCtx.stParam.ucTimerID = TIMER_ASYN_OP_TO_ID;
                uCtx.stParam.ucParam1  = ucCtxIdx; //将上报上下文的下标作为参数，在超时传回时判断
                g_astAsynOpCtx[ucCtxIdx].pTimer = OHCreateTimer((void *)uCtx.ulParam, TIMER_ASYN_OP_INTV, 
                                                               &(g_astAsynOpCtx[ucCtxIdx].ulTimerMagicNum));
                //g_astAsynOpCtx[ucCtxIdx].ulDataLen = 0; //暂不使用

                //缓存应答原语
                g_astAsynOpCtx[ucCtxIdx].pvData = pstPrmv; //缓存应答原语
            }
            return SUCCEEDED; //一个请求只会产生一个异步操作上下文
        }
    }
    return FAILED;
}

void OHTask(void *)
{
    void *pvPrmv = NULL;
    static UCHAR ucRetrieveSmcAddrFailedCnt = 0; //读取短信中心地址失败的次数
    UCHAR ucUpgradeResult = 0;
    LONG lRetrieveSmcAddrResult = 0;

    //判断是否升级成功
    ucUpgradeResult = CheckUpgradeAfterStartup();
    
    //读取短消息中心号码
    ReadSmcAddr();

    //无限循环
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
                //可能是异步操作，也可能是数据错误，需要进一步判断
                if(DoAsynOp((OHCH_RECV_DATA_IND_ST *)pvPrmv) != SUCCEEDED)
                {
                    MyFree(pvPrmv); //释放内存
                }

                ClearAsynOpBuf();
            }
            TakeParamEffect();     //移动协议
            TakeYkppParamEffect(); //厂家协议

            if(WaitForUpgradeCompletion() != 0)
            {
                while(1)
                {
                    //等待发送升级完成的应答的确认
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
                        MyDelay(1000); //给CH发送数据一段时间

                        //如果有GPRS连接，则需要断开
                        if(COMM_STATUS_CONNECTED == g_stCenterConnStatus.ucLinkStatus)
                        {
                            DisconnFromCenter();
                            MyDelay(7000); //给CH断开GPRS连接留一段时间
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
                MyFree(pvPrmv); //释放内存
            }    
            break;
        case TIMEOUT_EVENT:
            TimerExpired((TIMEOUT_EVENT_ST *)pvPrmv);
            
            WaitForUpgradeCompletion(); //超时肯定升级不成功，因此无需重启

            //判断在GPRS方式下无应答的次数
            if(g_stCenterConnStatus.ucLoginStatus == COMM_STATUS_LOGIN)
            {
                if(g_stCenterConnStatus.ucNoRspCnt > MAX_HEARTBEAT_TO_CNT)
                {
                    ReconnToCenter(); //准备断开连接，重新登录
                }
            }
            MyFree(pvPrmv); //释放内存
            break;
        case ALARM_STATUS_CHANGED_EVENT:
            MyFree(pvPrmv); //释放内存
            ReportAlarm();
            break;
        case OHCH_SEND_DATA_REQ:
            HndlOHCHSendDataReq((OHCH_SEND_DATA_REQ_ST *)pvPrmv);
            break;
        //调试原语类型    
        case OHCH_UNDE_RECV_IND:   //收到CH发来的未调数据
            //内存在函数内部释放
            HndlOHCHDebgDataInd((OHCH_RECV_DATA_IND_ST *)pvPrmv);
            TakeParamEffect();     //移动协议
            TakeYkppParamEffect(); //厂家协议
            break;
            
        case OHCH_DEBU_SEND_REQ:   //收到CH发来的已调数据
            //内存在函数内部释放
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
            //非初始化阶段对MODEM的操作
            if(g_ucOhInitState != OH_INIT_READ_SMC_ADDR)            
            {
                HandleRWSmcAddrRsp((OHCH_RW_SMC_ADDR_CNF_ST *)pvPrmv);
                MyFree(pvPrmv);
            }
            
            //如果处于初始化阶段则要继续下一个步骤
            else if(g_ucOhInitState == OH_INIT_READ_SMC_ADDR)
            {            
                lRetrieveSmcAddrResult = RetrieveSmcAddr((OHCH_RW_SMC_ADDR_CNF_ST *)pvPrmv);
                MyFree(pvPrmv);

                if((lRetrieveSmcAddrResult != SUCCEEDED)
                 &&(ucRetrieveSmcAddrFailedCnt < 3))
                {
                    ucRetrieveSmcAddrFailedCnt++;
                    ReadSmcAddr(); //如果读取次数不足3次则重新读取，否则不再读取
                }
                else
                {
                    //如果不启用GPRS则初始化完成，否则需要建立到中心的连接
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
            
            if(g_ucOhInitState == OH_INIT_CONN_CENTER) //如果处于初始化过程，则要进入登录阶段
            {
                g_ucOhInitState = OH_INIT_LOGIN;
            }
            break;
        case OHCH_GPRS_DISCONN_IND:
        case OHCH_CSD_DISCONN_IND: //如果使用GPRS通信方式，则需要重新建立到中心的连接
            MyFree(pvPrmv);
            //MCM-5_20061101_zhangjie_begin
            if(g_stNmParamSet.ucCommMode == COMM_MODE_GPRS)
            {
                ReconnToCenter();
            }
            //MCM-5_20061101_zhangjie_end
            break;
        case OHCH_GPRS_DISCONN_CNF: //不处理
            MyFree(pvPrmv);
            break;
        case OHCH_CSD_CONN_IND:
            HndlOHCHCsdConnInd((OHCH_CSD_CONN_IND_ST *)pvPrmv); //重用内存因此不释放
            break;
        //MCM-20_20061107_Linwei_begin
        case KEY_DOWN_EVENT:
            HndlKeyDownReq((KEY_EVENT_ST *)pvPrmv);
            MyFree(pvPrmv); //释放内存
            TakeYkppParamEffect();            
            break;
        //MCM-20_20061107_Linwei_end
        default:
            MyFree(pvPrmv); //释放内存
            break;            
        }

        //只会在第一次登录成功后进入下面的分支
        if((g_ucOhInitState == OH_INIT_LOGIN) && (g_stCenterConnStatus.ucLoginStatus == COMM_STATUS_LOGIN))
        {
            g_ucOhInitState = OH_INIT_COMPLETE;
            
            //判断是否升级成功或失败，如果由于通信原因而无法上报升级结果，此处没有办法补救，只能等待中心查询
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
    pstCtx->ucObjBitmap |= SendAsynOpParamReq(g_stTDParam, pstCtx->ucSn);//返回位图
}

void DoAsynOpSEC(void *pvCtx)
{
    ASYN_OP_CTX_ST *pstCtx = (ASYN_OP_CTX_ST *)pvCtx;
    pstCtx->ucObjBitmap |= SendAsynOpParamReq(g_stSECParam, pstCtx->ucSn);//返回位图
}

void DoAsynOpSMC(void *pvCtx)
{
    ASYN_OP_CTX_ST *pstCtx = (ASYN_OP_CTX_ST *)pvCtx;
    pstCtx->ucObjBitmap |= SendAsynOpYkppReq(g_stSMCParam, pstCtx->ucSn);//返回位图
}
