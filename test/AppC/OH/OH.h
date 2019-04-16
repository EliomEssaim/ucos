/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   OH.h
    作者:     林雨
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  
       作者:  林雨
       描述:  编码完成
    2. 日期:  2006/11/05
       作者:  钟华文
       描述:  修改问题MCM-10,
              为短信中心操作定义较小的缓存结构,节省内存
---------------------------------------------------------------------------*/
#ifndef OhH
#define OhH

#include "UserType.h"
#include "CMCC_1.h"
#include "YKPP.h"

//协议编号
#define CMCC_1_0    0
#define UNICOM_1_0  1

//用于抽取协议数据的状态
#define NO_FLAG_FOUND               1 
#define AP_START_FLAG_FOUND         2
#define AP_END_FLAG_FOUND           3
#define APAC_START_FLAG_FOUND       (AP_START_FLAG_FOUND + APAC_FLAG)
#define APB_START_FLAG_FOUND        (AP_START_FLAG_FOUND + APB_FLAG)
#define APAC_END_FLAG_FOUND         (AP_END_FLAG_FOUND   + APAC_FLAG)
#define APB_END_FLAG_FOUND          (AP_END_FLAG_FOUND   + APB_FLAG)

#define NO_CMCC_PROTO_DATA_FOUND    0
#define CMCC_PROTO_DATA_FOUND       1

//定时器的分类，可以依靠当时的状态来确定，上报需要定时器，远程升级过程需要定时器
#define OH_TIMER_CMCC_T0   0
#define OH_TIMER_CMCC_T1   1
#define OH_TIMER_CMCC_T2   2
#define OH_TIMER_CMCC_T3   3

//当前配置的协议处理器，只能有一种协议处理器生效
typedef LONG (*PROTO_HNDLR)(UCHAR *pucData, ULONG ulLen);
extern PROTO_HNDLR g_ucProtoHndlr;

//OH发送数据的状态
#define OH_IDLE                 0
#define OH_SEND_DATA_PENDING    1
#define OH_WAIT_FOR_RSP         2

//OH定时器参数格式
typedef union
{
    ULONG ulParam;
    struct
    {
        UCHAR  ucTimerID;
        UCHAR  ucParam1;
        USHORT usParam2;
    }stParam;
}OH_TIMER_CTX_UN;

//定时器定义，ID值小于64，可以同指针或地址区分开
#define TIMER_TRANS_CTRL_ID         1
#define TIMER_TRANS_BLK_ID          2
#define TIMER_ALARM_REPORT_ID       3
#define TIMER_OTHER_REPORT_ID       4
#define TIMER_HEARTBEAT_TO_ID       5 //心跳超时
#define TIMER_HEARTBEAT_PERIOD_ID   6 //心跳周期
#define TIMER_LOGIN_TO_ID           7 //登录超时
#define TIMER_LOGIN_PERIOD_ID       8 //重新登录的间隔
#define TIMER_ASYN_OP_TO_ID         9 //异步操作超时

#define ASYN_OP_OBJ_LOCAL           0
#define ASYN_OP_OBJ_SEC             (1 << 0) //太阳能控制器Solar Energy Controller
#define ASYN_OP_OBJ_TD              (1 << 1) //TD同步模块
#define ASYN_OP_OBJ_SMC             (1 << 2) //短信中心号码


typedef void (*CALLBACK_OP)(void *); //异步操作的回调函数类型

typedef struct
{
    CALLBACK_OP pfOp; //回调操作
    void *pvCtx;      //回调操作的上下文指针
}OP_OBJ;


#define MAX_ASYN_OP_OBJS 8

//用于异步操作的上下文
typedef struct
{
    Timer  *pTimer;
    ULONG  ulTimerMagicNum;
    UCHAR  ucSn;     //用于匹配请求和应答
    UCHAR  ucState;  //空闲(OH_IDLE)、等待应答(OH_WAIT_FOR_RSP)
    UCHAR  ucObjBitmap; //区别不同器件的位图，每一位对应一个异步操作请求，当为全0时表示操作结束
    OP_OBJ astOp[MAX_ASYN_OP_OBJS]; //最大同时支持8个异步操作对象
    CALLBACK_OP pfTo; //上下文超时
    ULONG  ulDataLen;
    void   *pvData;   //指向缓存的数据
}ASYN_OP_CTX_ST;

#define TIMER_ASYN_OP_INTV          (15000 / TIMER_UNIT_MS) //15s，异步操作超时

//用于异步操作的上下文的大小
#define ASYN_OP_CTX_CNT 3

//获取异步操作的消息ID
#define GEN_ASYN_OP_MSG_ID() ((++g_ucAsynOpMsgID) & 0x7F)

typedef struct
{
    UCHAR  ucOpObj;       //操作对象
    UCHAR  ucOpType;      //操作类型
    USHORT usParamLen;    //参数的长度
    UCHAR  aucParam[512]; //参数内容
}ASYN_OP_BUF_ST; //用于分离查询或设置请求中不同器件的参数

//MCM-10_20061105_zhonghw_begin
typedef struct
{
    UCHAR  ucOpObj;       //操作对象
    UCHAR  ucOpType;      //操作类型
    USHORT usParamLen;    //参数的长度
    UCHAR  aucParam[32]; //参数内容
}ASYN_OP_SMALL_BUF_ST; //用于分离查询或设置请求中不同器件的参数,暂用于短信中心操作
//MCM-10_20061105_zhonghw_end
#endif

