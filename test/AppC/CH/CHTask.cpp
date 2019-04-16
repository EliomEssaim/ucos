/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   CHTask.cpp
    作者:     章杰
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  2006/09/28
       作者:  章杰
       描述:  编码完成
    2. 日期:  2006/11/10
       作者:  章杰
       描述:  修改问题MCM-29，
              把发送AT+CCED的周期由原来一分钟改为30秒
    3. 日期:  2006/11/01
       作者:  章杰
       描述:  修改问题MCM-11，把原代码中涉及机型判断的地方全部改为对
              机型配置表中某项或者多项值的判断。
    4. 日期:  2006/11/06
       作者:  林雨
       描述:  修改问题MCM-18，修改笔误即可：把CH_STH_CTRL_QUE_SIZE
              改为CH_DEB_CTRL_QUE_SIZE 
    5. 日期:  2006/11/30
       作者:  章杰
       描述:  修改问题MCM-45，为解决485从器件处理速度有限，连续收到轮
              询数据和广播数据时会出现丢包的问题，做如下修改:
              1.当发送轮询数据时置南向接口忙 
              2.只有当轮询超时或者收到南向接口应答数据时才置南向接口闲 
              3.而只有南向接口闲时才会发送广播数据 
              4.这样处理后广播数据不会紧跟在轮询数据后发送，但轮询数据
                可能紧跟在广播数据之后
    6. 日期:  2007/01/09
       作者:  钟华文
       描述:  修改问题MCM-51
              在AtRtnIsOk()函数中，处理CH控制消息时，把不能辨认的控制消息
              放回消息队列，由控制消息处理函数来处理。
    7. 日期:  2007/01/11
       作者:  钟华文
       描述:  修改问题MCM-53
              修改笔误，在短信中心操作对应的AtRtnIsOk()函数调用时，
              将参数等待时间从6秒修改为2秒。            
    8. 日期:  2007/06/14
       作者:  钟华文
       描述:  修改问题MCM-73
              在对短信进行PUD格式编码过程中，只有11位的手机号码才在其前面加上86，
              同时确保其他号码前面都不带86，最后将此短信设置为国内短信
    9. 日期:  2007/07/12
       作者:  林雨
       描述:  修改问题MCM-76
              对PDU格式短信的DCS按位进行判断
---------------------------------------------------------------------------*/
//****************************包含头文件**************************//
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

//*****************************宏定义*****************************//
//CH层延时时间单位ms
#define CH_DELAY_TIME   20

//CH层各定时器时间，单位ms
#define TIME_WAIT_SMSSEND       8000//等待短消息发送时间
#define TIME_WAIT_ATCMD         2000//等待AT命令回应的时间
#define TIME_WAIT_ATCONGPRS     6000//连接GPRS各AT命令的回应时间，共3条AT命令
#define TIME_WAIT_ATDISCONGPRS  6000//断开GPRS各AT命令的回应时间，共2条AT命令
#define TIME_WAIT_ATACSDCONNECT 15000//发送ATA后等待CSD连接上的时间
//MCM-29_20061110_zhangjie_begin
#define TIME_GET_ZONEINFO       30000//查询小区信息周期时间
//MCM-29_20061110_zhangjie_end
#define TIME_TIMEOUT_USARTREC   500//串口接收超时时间
#define TIME_TIMEOUT_ASKSLAVE   500//轮询从机超时时间
#define TIME_TIMEOUT_DEBUG      600000//调试模式超时时间
#define TIME_SLAVEREG_PROTECT   8000//从机注册保护时间
#define TIME_TIMEOUT_RESET      (24*3600*1000+11300)//重启modem和电路板周期时间,取非整是为了和有些周期性的任务避开


//PDU中时间域长度
#define PDU_SCTS_LEN   7

//匹配数据包状态类型
#define DATAPACK_STATE_END_2        6
#define DATAPACK_STATE_END_1        5
#define DATAPACK_STATE_START_2      4
#define DATAPACK_STATE_START_1      3
#define DATAPACK_STATE_START        2
#define DATAPACK_STATE_END          1
#define DATAPACK_STATE_NO           0

//完整匹配数据包处理函数指针类型
typedef void (*DP_HNDL_FUNC)(void*); 

//AT完整数据包状态类型
#define DATAPACK_STATE_CMT              20
#define DATAPACK_STATE_CRING            21
#define DATAPACK_STATE_UNKNOW           22
#define DATAPACK_STATE_SOCKETCLOSE      23
#define DATAPACK_STATE_CSCA             24
#define DATAPACK_STATE_CCED             25

//CH控制队列长度
#define CH_NTH_CTRL_QUE_SIZE 5
#define CH_STH_CTRL_QUE_SIZE 5
#define CH_DEB_CTRL_QUE_SIZE 1

//各种匹配数据包大小
#define DATAPACK_CMCC_APAC_SIZE USART_SIZE_BUFFER
#define DATAPACK_CMCC_APB_SIZE  USART_SIZE_BUFFER
#define DATAPACK_ATCMD_SIZE     USART_SIZE_BUFFER
#define DATAPACK_PRIVATE_SIZE   USART_SIZE_BUFFER
#define DATAPACK_MS_SIZE        USART_SIZE_BUFFER
//AT完整包第一包数据大小,用于暂存IP地址，小区信息，短信中心号码
#define DATAPACK_ATFULL_DATA1_SIZE 50

#define DATAPACK_MS_MINSIZE 10//含首尾字符

//CH向modem发送数据时的类型
//AtRtnIsOk()使用
#define CH_SENDAT_NORMAL       1//普通AT命令
#define CH_SENDAT_GPRSCONN     2//连接GPRS网络
#define CH_SENDAT_GPRSOTCP     3//准备发送GPRS数据
#define CH_SENDAT_SOCKETCLOSE  4//断开SOCKET连接
#define CH_SENDAT_CSCA         5//查询短消息
#define CH_SENDAT_CCED         6//查询小区信息
#define CH_SENDAT_ATA          7//CSD摘机

//CH层控制消息原语类型
//目前CH层专用，但最好和其他原语类型不同
#define CH_MG_AT_TIMEOUT            0x31
#define CH_MG_AT_ERROR              0x32
#define CH_MG_AT_OK                 0x33
//test以下3个暂时不用
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

//以下是和主从协议有关的消息原语类型
#define CH_MG_SM_ANS                0x50//从设备在线应答，从到主
#define CH_MG_SM_REG                0x51//申请注册，从到主
#define CH_MG_SM_UNREGCONF          0x52//确认申请删除，从到主
#define CH_MG_MS_ASK                0x53//从设备在线查询，主到从
#define CH_MG_MS_REGANS             0x54//申请注册的应答，主到从
#define CH_MG_MS_UNREGANS           0x55//申请删除的应答，主到从

//GPRS数据转义字符
#define GPRS_ESCAPE_CHAR  0x10

//主从协议数据转义字符
#define MS_ESCAPE_CHAR  0x6E

//主从起始结束标志 
#define MS_START_END_FLAG        0xAA    
//主从用于广播的设备编号
#define MS_BROADCAST_DEVNUM 0xFF
//主从用于新的从设备编号
#define MS_NEWS_DEVNUM 0x00
//主从协议类型标识
#define MS_PROTOCOL_TYPE 0xF1

//主从协议命令单元定义
#define MS_CMD_ASK                  0x00//从设备在线查询，主到从,此时无数据单元
#define SM_CMD_ANS                  0x40//从设备在线应答，从到主,此时无数据单元
#define MS_CMD_QUERY                0x01//查询，主到从
#define SM_CMD_QUERYANS             0x41//查询的应答，从到主
#define MS_CMD_SET                  0x02//设置，主到从
#define SM_CMD_SETANS               0x42//设置的应答，从到主
#define MS_CMD_TAKEOHTER            0x03//协议的承载，主到从
#define SM_CMD_TAKEOHTER            0x43//协议的承载，从到主
#define MS_CMD_ONLYONEANS           0x04//独占主从的应答，主到从
#define SM_CMD_ONLYONE              0x44//独占主从的申请，从到主
#define MS_CMD_CANCLEONLYONEANS     0x05//取消独占的应答，主到从
#define SM_CMD_CANCLEONLYONE        0x45//取消独占的申请，从到主
#define MS_CMD_REGANS               0x06//申请注册的应答，主到从
#define SM_CMD_REG                  0x46//申请注册，从到主
#define MS_CMD_UNREGANS             0x07//申请删除的应答，主到从
#define SM_CMD_UNREG                0x47//申请删除，从到主
#define SM_CMD_UNREGCONF            0x48//确认申请删除，从到主
//#define MS_CMD_TAKERFINFO           0x0A//射频信息的转发，主到从
//#define SM_CMD_TAKERFINFO           0x4A//射频信息的转发，从到主


//主从协议栈，未使用字节对齐
typedef struct
{
    UCHAR ucProtocolType;//协议类型
    UCHAR ucSNum;//从机编号
    UCHAR ucCmdUnit;//命令单元
    UCHAR ucFlagUnit;//标志单元    
    UCHAR *pucDataUnit;//数据单元指针
    ULONG ulDataUnitSize;//数据单元长度 
}MS_PROTOCOL_ST;

//有效数据区为pucDataPack[0]=< ... <pucDataPack[uiInPos]
typedef struct
{
    UCHAR ucState;//数据包状态  
    //test以下四个变量暂时不用
    UCHAR *pucHead;//头字符串指针
    UCHAR *pucEnd;//尾字符串指针
    UCHAR ucHeadSize;//头字符串长度
    UCHAR ucEndSize;//尾字符串长度
    
    UCHAR *pucData;//数据包指针
    UCHAR *pucInPos;    
    ULONG ulDataPackMaxSize;//数据包最大长度
 
}DATA_PACK_ST;

//用于存放完整的AT数据，因为可能含有若干0D0A的数据包
//目前支持两组0D0A
typedef struct
{
    UCHAR ucState;//数据包状态      
    UCHAR *pucData1;//数据包指针
    UCHAR *pucData2;//数据包指针    
    ULONG ulData1Size;    
    ULONG ulData2Size;    
    ULONG ulDataPack1MaxSize;//数据包最大长度
    ULONG ulDataPack2MaxSize;//数据包最大长度     
}AT_DATA_PACK_ST;

//定时器结构,通用,目前只有CH层使用
typedef struct
{
    Timer *pTimer;              //定时器引用
    ULONG ulTimerMagicNum;      //定时器的magic number 
    TX_TIMER_FUNC pvFunc;       //回调函数
}GENERAL_TIMER;

//CH层控制消息队列结构，目前只提供给CH自己使用
typedef struct
{
    ULONG ulPrmvType;   //类型
    UCHAR ucParam1;     //参数1
}CH_CTR_ST;

typedef struct
{
    CHAR  acSCTelNum[MAX_TEL_NUM_LEN + 1]; //短消息中心电话号码以数值0结尾
    UCHAR ucPID;
    UCHAR ucDCS;        //如果是短消息传输则需要编码方式，其中0：GSM7Bit，8：UCS2
    UCHAR ucDA;         //保存地址类型
    CHAR acSCTS[PDU_SCTS_LEN];
    CHAR  acTelNum[MAX_TEL_NUM_LEN + 1]; //电话号码以数值0结尾
    ULONG ulDataLen;    //用户数据长度
    UCHAR *pucData;     //用户数据指针
}SMS_PARAM_ST;

//**************************全局变量申明**************************//
//厂家协议中含有主从通信相关参数
extern YKPP_PARAM_SET_ST g_stYkppParamSet;

//设备信息
extern DEV_INFO_SET_ST g_stDevInfoSet;

//实时采集量
extern RC_PARAM_SET_ST g_stRcParamSet;

extern CENTER_CONN_STATUS_ST g_stCenterConnStatus;

//MCM-11_20061101_zhangjie_begin
extern DEV_TYPE_TABLE_ST g_stDevTypeTable;
//MCM-11_20061101_zhangjie_end

//**************************全局变量定义**************************//
//test
ULONG g_ulTest = 0;

UCHAR g_aucSMCAddr[DATAPACK_ATFULL_DATA1_SIZE];//存放查询短信中心号码后收到的数据
UCHAR g_aucZoneInfo[DATAPACK_ATFULL_DATA1_SIZE];//存放查询小区信息后收到的数据
UCHAR g_aucCLIPTel[DATAPACK_ATFULL_DATA1_SIZE];//存放CSD来电号码
UCHAR g_aucIPAddr[DATAPACK_ATFULL_DATA1_SIZE];//存放GPRS激活后本机的IP地址

UCHAR g_ucCurrCommMode; //当前的通信状态，需要CH来初始化

ULONG g_ulACCHSendMessageNum = 0;//AC请求CH发送消息的个数

#define SIM_STA_NORMAL   0//sim卡里无任何短消息
#define SIM_STA_UNREAD   1//sim卡里有未读短消息
#define SIM_STA_READED   2//sim卡里有全是已读短消息
UCHAR g_ucCurrSimSMSSta = SIM_STA_NORMAL; //当前的SIM卡里短消息状态

DATA_PACK_ST g_stDataPackCMCCAPAC;
DATA_PACK_ST g_stDataPackCMCCAPB;
DATA_PACK_ST g_stDataPackAT;
DATA_PACK_ST g_stDataPackPrivate;
DATA_PACK_ST g_stDataPackNthMS;
DATA_PACK_ST g_stDataPackSthMS;

DATA_PACK_ST g_stDataPackDebAPAC;
DATA_PACK_ST g_stDataPackDebPrivate;

AT_DATA_PACK_ST g_stDataPackATFull;

//GPRS已转义字符列表
UCHAR g_aucGPRSEscaped[] =
{
   0x03,
   0x10,
};

//GPRS未转义字符(即需要转义)列表
UCHAR g_aucGPRSUnEscape[] =
{
   0x03,
   0x10,
};

//主从协议已转义字符列表
UCHAR g_aucMSEscaped[] =
{
   0x5D,
   0x7D,
};

//主从协议未转义字符(即需要转义)列表
UCHAR g_aucMSUnEscape[] =
{
   0x6E,
   MS_START_END_FLAG,
};

//北向接口接收数据超时定时器
void NthRecTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerNthRecTimeOut =
{
    NULL,
    NULL,
    NthRecTimeOutCallBack
};

//南向接口接收数据超时定时器
void SthRecTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerSthRecTimeOut =
{
    NULL,
    NULL,
    SthRecTimeOutCallBack
};

//调试接口接收数据超时定时器
void DebRecTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerDebRecTimeOut =
{
    NULL,
    NULL,
    DebRecTimeOutCallBack
};

//南向接口轮询从机超时定时器
void SthAskTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerSthAskTimeOut =
{
    NULL,
    NULL,
    SthAskTimeOutCallBack
};

//发送AT命令等待回应超时定时器
void ATRtnTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerATRtnTimeOut =
{
    NULL,
    NULL,
    ATRtnTimeOutCallBack
};

//查询小区信息定时器
void ZoneInfoTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerZoneInfo =
{
    NULL,
    NULL,
    ZoneInfoTimeOutCallBack
};

//调试模式超时定时器
void DebugTimeOutCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerDebugTimeOut =
{
    NULL,
    NULL,
    DebugTimeOutCallBack
};

//从机注册保护定时器
void SlaveRegProtectTimeCallBack(void *pvCtx);
GENERAL_TIMER g_stTimerSlaveRegProtect =
{
    NULL,
    NULL,
    SlaveRegProtectTimeCallBack
};

//modem及监控板重启定时器
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

//通信任务内部使用的队列,用于存放自通信任务发自收的消息
OS_EVENT *g_pstCHNthCtrlQue;//北向接口控制队列
OS_EVENT *g_pstCHSthCtrlQue;//南向接口控制队列
OS_EVENT *g_pstCHDebCtrlQue;//调试接口控制队列
void *g_apvCHNthCtrlQue[CH_NTH_CTRL_QUE_SIZE];
void *g_apvCHSthCtrlQue[CH_STH_CTRL_QUE_SIZE];
void *g_apvCHDebCtrlQue[CH_DEB_CTRL_QUE_SIZE];

//*********** 待发送的AT命令表 ***********
UCHAR g_aucSAT_ATANDF[] = "AT&F\r";     //回复厂家设置
UCHAR g_aucSAT_ATOFFLINE[] = "+++";     //切换到OFFLINE模式
UCHAR g_aucSAT_ATATO[] = "ATO\n\r";     //切换到DATA模式
UCHAR g_aucSAT_CMGF0[] = "AT+CMGF=0\r";     //设置为PDU模式
UCHAR g_aucSAT_CNMI0[] = "AT+CNMI=2,0,0,0,0\r";     //新短消息不通知
UCHAR g_aucSAT_CNMI1[] = "AT+CNMI=2,1,0,0,0\r";     //新短消息通知
UCHAR g_aucSAT_CNMI2[] = "AT+CNMI=2,2,0,0,0\r";     //新短消息直接发出
UCHAR g_aucSAT_ATE0[] = "ATE0\r";//去掉回显
UCHAR g_aucSAT_GPRSCGATT[] = "AT+CGATT=1\r";//注册网络
UCHAR g_aucSAT_GPRSCONN[] = "AT#CONNECTIONSTART\r";//连接网络
UCHAR g_aucSAT_GPRSIPSET[] = "AT#TCPSERV=";//设置中心IP
UCHAR g_aucSAT_GPRSPORTSET[] = "AT#TCPPORT=";//设置端口号
UCHAR g_aucSAT_GPRSOTCP[] = "AT#OTCP\r";//打开连接准备收发数据
UCHAR g_aucSAT_GPRSSOCKETCLOSE[] = "\x3";//断开socket连接
UCHAR g_aucSAT_GPRSDISCONN[] = "AT#CONNECTIONSTOP\r";//断开连接
UCHAR g_aucSAT_ATA[] = "ATA\r";//CSD摘机
UCHAR g_aucSAT_ATH[] = "ATH\r";//CSD挂机
UCHAR g_aucSAT_CMGS[] = "AT+CMGS=";//发送短消息
UCHAR g_aucSAT_CR[] = "\r";//换行字符
UCHAR g_aucSAT_CSCAQ[] = "AT+CSCA?\r";//查询短信中心号码
UCHAR g_aucSAT_CSCA[] = "AT+CSCA=";//设置短信中心号码
UCHAR g_aucSAT_CSMP[] = "AT+CSMP=1,167,0,0\r";//test
UCHAR g_aucSAT_CLIP[] = "AT+CLIP=1\r";//来电显示
UCHAR g_aucSAT_CRC[] = "AT+CRC=1\r";//显示来电类型
UCHAR g_aucSAT_ATANDW[] = "AT&W\r";//保存设置
UCHAR g_aucSAT_CSAS[] = "AT+CSAS\r";//保存设置
UCHAR g_aucSAT_CCED[] = "AT+CCED=0,1\r";//查询小区信息
UCHAR g_aucSAT_CMGL[] = "AT+CMGL=4\r";//读出所有短信
UCHAR g_aucSAT_CMGD[] = "AT+CMGD=1,3\r";//删除所有短信

//********* 可能接收到的AT命令头 *********
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
UCHAR g_aucRAT_GPRSActed[] = "#CME ERROR: 35840";//GPRS已被激活
UCHAR g_aucRAT_TCPClosed[] = "#CME ERROR: 37966";//对方断开TCP连接
UCHAR g_aucRAT_GPRSWaitData[] = "Ok_Info_WaitingForData";
UCHAR g_aucRAT_GPRSSocketClose[] = "Ok_Info_SocketClosed";
UCHAR g_aucRAT_NOCARRIER[] = "NO CARRIER";
UCHAR g_aucRAT_CONNECT[] = "CONNECT";

//****************************函数申明****************************//
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
  函数名：StrToULONG()

  说明： 将字符串转换为ULONG
  作者：
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
  函数名：ULONGToStr
  Input:       pucStr :转换后字符串存放的地址  
                  ulValue: 需要转换的数值
  Return:     转换后字符串的长度
  说明： 将ULONG转换为字符串，由调用者保证字符串内存大小
  作者：
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
  Description:    本函数从字节合并后的PUD数据中提取电话号码
                        并在电话号码的末尾添加00
  Calls:
  Called By:
  Input:          pucPDU :存放电话号码的PDU数据指针   
                  ulPDULen: 存放电话号码的PDU长度
                  pcTelNum: 存放电话号码字符串指针
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
        //先取低字节
        pcTelNum[2*i] = (ucTemp & 0x0F) + 0x30;        
        //再取高字节
        pcTelNum[2*i+1] = ((ucTemp & 0xF0) >> 4) + 0x30;
    }
    //判断末位是否是F,并在末位添加00
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
  Description:    本函数把电话号码写入PDU数据包
                        并在PDU的末尾补齐F
  Calls:
  Called By:
  Input:       pucPDU :存放电话号码的PDU数据指针   
                  ulTelNumLen: 电话号码的字符串长度
                  pcTelNum: 存放电话号码字符串指针
  Output:         
  Return:   存放电话号码的PDU长度         
  Others:         
*************************************************/
ULONG WriteTelNumToPDU(UCHAR* pucPDU, ULONG ulTelNumLen, CHAR *pcTelNum)
{
    ULONG i,ulPDULen;
    //转换为PDU长度
    ulPDULen = ulTelNumLen/2 + ulTelNumLen %2;
    for(i=0;i<ulPDULen;i++)
    {
        //后面的在高字节，前面的在低字节
        pucPDU[i] = (pcTelNum[2*i] -0x30) + ((pcTelNum[2*i+1] - 0x30) <<4);
    }
    //判断末位是否要填写F
    i = ulPDULen - 1;
    if(ulTelNumLen %2 !=0)
    {
        pucPDU[i] = pucPDU[i] |0xF0;
    }
    return ulPDULen;
}
/*************************************************
  Function:DecodeGSM7BitData
  Description:    本函数对7bit解码
  Called By:
  Input:       pucData :待解码的数据指针   
                  ulDataLen: 数据长度，指septet(7bit字符的个数)
                  pcTelNum: 解码后的数据指针
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
            pucDecodeData[i] = pucData[j] & 0x7F; //没有跨越2个字节
        }
        else if(ulMoveToLeftBit == 7)
        {
            pucDecodeData[i] = (pucData[j] >> 1) & 0x7F; //没有跨越2个字节
            j++;
        }
        else
        {
            usTmp = (USHORT)(pucData[j + 1]);
            usTmp = (usTmp << 8); //高位部分
            usTmp |= pucData[j];          //低位部分
            usTmp <<= ulMoveToLeftBit;
            pucDecodeData[i] = (UCHAR)((usTmp >> 8) & 0x7F);
            j++;
        }
    }
}

/*********************************************************
  函数名：DecodeAtCmtInPDUMode()
  输入：  pucCMT:    接收到的+CMT数据字符串，从"+CMT"开始到第一个
                    <CR><LF>结束
                   ulCMTLen:  接收到的数据长度
                    pucPDU:    接收到的PDU数据字符串，两组<CR><LF>之间
                   ulPDULen:  接收到的PDU数据长度
  输出：  pstSMS:  短消息的各种信息

  返回值：SUCCEEDED成功FAILD失败
  说明： 
  作者：
 *************************************************/
LONG DecodeAtCmtInPDUMode(UCHAR *pucCMT, ULONG ulCMTLen, 
            UCHAR *pucPDU, ULONG ulPDULen, SMS_PARAM_ST *pstSMS)
{
    ULONG ulStartPos, ulTPDULen, ulSCALen, ulOALen, ulUDLen, ulCurrIndex=0;

    //取得TPDU长度,支持cmgl,cmgl是第三个逗号后取数据
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

    //内存不搬移，因为要进行7bit解码
    //内存搬移至SMS结构体所指内存
    //memcpy(pstSMS->pucData,pucPDU,ulPDULen);
    //pucPDU = pstSMS->pucData;
    
    //字节合并
    ulPDULen = Merge2Bytes(pucPDU, ulPDULen);

    //判断长度合法性  
    //ulSCALen不含SCA的Length,但是含TypeOfAddr
    ulSCALen = (ULONG)(pucPDU[ulCurrIndex]);
    if(ulPDULen != ulSCALen + 1 + ulTPDULen)
    {
        return FAILED;
    }    
    //跳过SCA的Length和TypeOfAddr
    ulCurrIndex = ulCurrIndex + 2;

    //获得SCA
    if(ulSCALen != 0)
    {
        GetTelNumFromPDU(pucPDU+ulCurrIndex, ulSCALen-1, pstSMS->acSCTelNum);
        ulCurrIndex = ulCurrIndex + ulSCALen - 1;
    }
    else
    {
        ulCurrIndex++;
    }

    //判断MTI,(RP,UDHI,SRI,MMS,MTI),忽略其他
    //0表示SMS-DELIVER，否则表示SMS-SUBMIT
    if(pucPDU[ulCurrIndex] & 0x03 != 0)
    {
        return FAILED;
    }
    ulCurrIndex++;

    //获得OA放入电话号码
    //结构同SCA，但是Length是以号码数字为单位
    ulOALen = (ULONG)(pucPDU[ulCurrIndex]);
    //转换长度
    if(ulOALen%2 != 0)
    {
        ulOALen++;
    }    
    ulOALen = ulOALen/2;
    //跳过OA的Length
    ulCurrIndex++;
    
    //获取TypeOfAddr
    pstSMS->ucDA = pucPDU[ulCurrIndex];
    ulCurrIndex++;
    
    GetTelNumFromPDU(pucPDU+ulCurrIndex, ulOALen, pstSMS->acTelNum);
    ulCurrIndex = ulCurrIndex + ulOALen;

    //获得PID
    pstSMS->ucPID = pucPDU[ulCurrIndex];
    ulCurrIndex++;

    //获得DCS
    pstSMS->ucDCS= pucPDU[ulCurrIndex];
    ulCurrIndex++;    

    //获得SCTS
    memcpy(pstSMS->acSCTS, pucPDU+ulCurrIndex, PDU_SCTS_LEN);
    ulCurrIndex = ulCurrIndex + PDU_SCTS_LEN;    

    //获得UDL和UD，写入短消息内容
    ulUDLen = pucPDU[ulCurrIndex];
    //跳过长度
    ulCurrIndex++;
    //判断PDU长度
    if((ulCurrIndex + (ulUDLen*7 + (8-1))/8)!= ulPDULen)
    {
        return FAILED;
    }

    //MCM-76_20061110_linyu_begin
    if((pstSMS->ucDCS & 0xC0) == 0) //最高两位bit 7..6为0
    {   
        //判断bit 3..2
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
    else if((pstSMS->ucDCS & 0xF0) == 0xF0) //最高4位全为1
    {
        //判断bit 2
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
     
    //填入长度
    pstSMS->ulDataLen = ulUDLen;

    return SUCCEEDED;
    
}


/*************************************************
  Function:EncodeGSM7BitData
  Description:    本函数对7bit编码
  Called By:
  Input:       pucData :待编码的数据指针   
                  ulDataLen: 数据长度
                  pcTelNum: 编码后的数据指针
  Output:         
  Return:     编码后的数据长度         
  Others:     由调用方保证指针的合法性   
*************************************************/
ULONG EncodeGSM7BitData(UCHAR *pucData, ULONG ulDataLen, UCHAR *pucEncodeData)
{
    ULONG ulEncodeLen;
    //计算编码后的长度
    ulEncodeLen = (ulDataLen * 7 + (8-1))/8;
    memset(pucEncodeData, 0, ulEncodeLen);
    
    UCHAR ucHighByte = 0, ucLowByte = 0;
    USHORT usTmp = 0;
    ULONG ulMoveToRightBit = 0;
    ULONG j = 0; //表示已编码的字符占用的字节数
    for(ULONG i = 0; i < ulDataLen; i++)
    {
        ulMoveToRightBit = i % 8;
        if(ulMoveToRightBit == 0)
        {
            pucEncodeData[j] = pucData[i] & 0x7F; //没有跨越2个字节
        }
        else if(ulMoveToRightBit == 7)
        {
            pucEncodeData[j] |= (pucData[i] & 0x7F) << 1; //没有跨越2个字节
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
  Description:    本函数向指定通信口以PDU方式发送短消息
  Calls:
  Called By:
  Input:       pstCOMMObj:通信口对象指针
                  pcTelNum :对方电话号码   
                  pucData: 待发送数据指针
                  ulSize: 待发送数据长度
                  ucDCS:编码方式08:UCS2 00:7bit
                  pcSCTelNum:短信中心号码，NULL则不使用
  Output:         
  Return:         
  Others:         
*************************************************/
LONG SendSMSInPDUMode(COMM_OBJ_ST *pstCOMMObj,CHAR *pcTelNum, UCHAR *pucData, 
        ULONG ulSize, UCHAR ucDCS, UCHAR ucDA, CHAR *pcSCTelNum)
{
    UCHAR aucPDUData[384];
    ULONG ulCurrIndex = 0, ulDALen, ulTPDULen, ulSCALen, ulPDULen;

    //防止短消息过长
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

    //写入SCA
    //SCA为0，表示使用存储在SIM卡中的短消息中心号码
    if(pcSCTelNum == NULL)
    {
        aucPDUData[ulCurrIndex] = 0;
        ulSCALen = 1;
        ulCurrIndex++;
    }
    //notice 写入SCA代码未写
    else
    {
    }
    
    //写入MTI等
    //VPF=01 采用relative format
    aucPDUData[ulCurrIndex] = 0x11;
    ulCurrIndex++;

    //写入MR
    aucPDUData[ulCurrIndex] = 0x00;
    ulCurrIndex++;

    //写入DA
    SetDA(pcTelNum, aucPDUData, ulCurrIndex,ucDA);

    //写入PID
    aucPDUData[ulCurrIndex] = 0x00;
    ulCurrIndex++;

    //写入DCS
    aucPDUData[ulCurrIndex] = ucDCS;
    ulCurrIndex++;

    //写入VP
    /*
    0 to 143:     (VP + 1) x 5 minutes (i.e. 5 minutes intervals up to 12 hours)
    144 to 167:  2 hours + ((VP-143) x 30 minutes)
    168 to 196:  (VP-166) x 1 day
    197 to 255:  (VP-192) x 1 week
    */
    aucPDUData[ulCurrIndex] = 173;//7days
    ulCurrIndex++;

    //写入UDL
    aucPDUData[ulCurrIndex] = ulSize;
    ulCurrIndex++;

    //MCM-76_20061110_linyu_begin
    //写入UD  
    if((ucDCS & 0xC0) == 0) //最高两位bit 7..6为0
    {   
        //判断bit 3..2
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
    else if((ucDCS & 0xF0) == 0xF0) //最高4位全为1
    {
        //判断bit 2
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

    //写入TPDU长度(不含SCA)
    ulTPDULen = ulCurrIndex - ulSCALen;

    //字节拆分
    ulPDULen = SplitInto2Bytes(aucPDUData, ulCurrIndex);

    //以下是向串口写数据
    UINT32 cpu_sr;    
    OS_ENTER_CRITICAL();
    //写入CMGS
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            g_aucSAT_CMGS, sizeof(g_aucSAT_CMGS)-1);
    //写入TPDU长度(不含SCA)
    UCHAR pucTemp[4];
    ulTPDULen = ULONGToStr(pucTemp, ulTPDULen);
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            pucTemp, ulTPDULen);
    //写入0D
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            g_aucSAT_CR, sizeof(g_aucSAT_CR)-1);
    //写入PDU数据
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            aucPDUData, ulPDULen);
    //写入1A    
    UCHAR ucTemp = 0x1A;
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            &ucTemp, 1);   
    OS_EXIT_CRITICAL();
    
    return SUCCEEDED;
}
/*************************************************
  Function:StartGeneralTimer
  Description:    本函数启动通用的定时器
  Calls:
  Called By:
  Input:       pvCtx :给回调函数的参数   
                  ulInterval: 定时时长
                  pstGTimer: 通用定时器指针
                  enType:定时器类型，目前只有周期性和非周期性两种
  Output:         
  Return:         
  Others:         
*************************************************/
LONG StartGeneralTimer(GENERAL_TIMER *pstGTimer,ULONG ulInterval,void *pvCtx, TIMER_TYPE_EN enType)
{
   
    if(pstGTimer->pTimer) //如果原来有定时器需要先停止
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
  Description:    本函数停止通用的定时器
  Calls:
  Called By:
  Input:  
                  pstGTimer: 通用定时器指针
  Output:         
  Return:         
  Others:         
*************************************************/
LONG StopGeneralTimer(GENERAL_TIMER *pstGTimer)
{
   
    if(pstGTimer->pTimer) //如果原来有定时器需要先停止
    {
        g_TimerMngr.RemoveTimer(pstGTimer->pTimer, pstGTimer->ulTimerMagicNum); 

        pstGTimer->pTimer = NULL;
        pstGTimer->ulTimerMagicNum = NULL;
    }

    return SUCCEEDED;

}

/****************************************
北向接口接收超时回调函数
****************************************/
void NthRecTimeOutCallBack(void *pvCtx)
{
    g_stTimerNthRecTimeOut.pTimer = NULL;
    g_stTimerNthRecTimeOut.ulTimerMagicNum = NULL;

    //不需要停止定时器，因为已经超时
    //StopGeneralTimer(&g_stTimerNthRecTimeOut);

    //只有北向接口会收到APAC，APB，AT命令和厂家协议的数据包
    g_stDataPackCMCCAPAC.ucState = DATAPACK_STATE_NO;
    g_stDataPackCMCCAPB.ucState = DATAPACK_STATE_NO;
    g_stDataPackAT.ucState = DATAPACK_STATE_NO;
    g_stDataPackATFull.ucState = DATAPACK_STATE_NO;
    g_stDataPackPrivate.ucState = DATAPACK_STATE_NO;
    //北向主从协议
    g_stDataPackNthMS.ucState = DATAPACK_STATE_NO;
}

/****************************************
南向接口接收超时回调函数
****************************************/
void SthRecTimeOutCallBack(void *pvCtx)
{
    g_stTimerSthRecTimeOut.pTimer = NULL;
    g_stTimerSthRecTimeOut.ulTimerMagicNum = NULL;

    //南向主从协议
    g_stDataPackSthMS.ucState = DATAPACK_STATE_NO;
}

/****************************************
调试接口接收超时回调函数
****************************************/
void DebRecTimeOutCallBack(void *pvCtx)
{
    g_stTimerDebRecTimeOut.pTimer = NULL;
    g_stTimerDebRecTimeOut.ulTimerMagicNum = NULL;

    g_stDataPackDebAPAC.ucState = DATAPACK_STATE_NO;
    g_stDataPackDebPrivate.ucState = DATAPACK_STATE_NO;
}

/****************************************
南向接口轮询从机超时回调函数
****************************************/
void SthAskTimeOutCallBack(void *pvCtx)
{
    g_stTimerSthAskTimeOut.pTimer = NULL;
    g_stTimerSthAskTimeOut.ulTimerMagicNum = NULL;

    //置主从通信告警位
    if(g_stYkppParamSet.stYkppCommParam.ucAskingSNum != MS_NEWS_DEVNUM)
    {
        MSChangeBit(&g_stYkppParamSet.stYkppCommParam.usCommAlarm, g_stYkppParamSet.stYkppCommParam.ucAskingSNum, 1);
        //未收到删除确认时，会产生告警，但位图中已无此从机
        g_stYkppParamSet.stYkppCommParam.usCommAlarm = g_stYkppParamSet.stYkppCommParam.usCommAlarm & g_stYkppParamSet.stYkppCommParam.usSBitmap;
    }

    //MCM-45_20061130_zhangjie_begin
    if(g_stSouthInterface.ucInterfaceType == USART_SLAVE_485)
    {
        g_stSouthInterface.ucState = COMM_STATE_SEND_IDLE;
    }
    //MCM-45_20061130_zhangjie_end
                
    //发送对下一个从机的轮询
    CH_CTR_ST stCHCtrMsg;
    stCHCtrMsg.ulPrmvType = CH_MG_MS_ASK;
    UCHAR *pucMem = (UCHAR *) MyMalloc(g_pstMemPool16);
    if(pucMem == (void*)NULL)
    {
        //等待看门狗重启，因为如果申请不到内存则南向会不再轮询
        while(1);
    }
    memcpy(pucMem,&stCHCtrMsg,sizeof(stCHCtrMsg));

    //发送消息，消息队列满则丢弃数据包
    if(OSQPost(g_pstCHSthCtrlQue, pucMem) == OS_Q_FULL)
    {
        //等待看门狗重启，因为如果发送消息不成功则南向会不再轮询
        MyFree(pucMem);
        while(1);
    }
}

/****************************************
AT命令返回超时回调函数，目前只用于北向
****************************************/
void ATRtnTimeOutCallBack(void *pvCtx)
{
    g_stTimerATRtnTimeOut.pTimer = NULL;
    g_stTimerATRtnTimeOut.ulTimerMagicNum = NULL;

    UCHAR *pucMem;
    CH_CTR_ST stCHCtrMsg;

    stCHCtrMsg.ulPrmvType = CH_MG_AT_TIMEOUT;

    //notice这里不允许申请不到内存
    pucMem = (UCHAR *) MyMalloc(g_pstMemPool16);

    //notice:CH任务一直不释放自己的运行权从而导致重启
    if(pucMem == (void*)NULL)
    {
        while(1);
    }

    memcpy(pucMem,&stCHCtrMsg,sizeof(stCHCtrMsg));

    //notice这里不允许消息队列满
    OSQPost(g_pstCHNthCtrlQue, pucMem);

}

/****************************************
查询小区信息超时回调函数，目前只用于北向
****************************************/
void ZoneInfoTimeOutCallBack(void *pvCtx)
{
    UCHAR *pucMem;
    CH_CTR_ST stCHCtrMsg;

    stCHCtrMsg.ulPrmvType = CH_MG_GET_ZONEINFO;

    pucMem = (UCHAR *) MyMalloc(g_pstMemPool16);
    
    //notice:由于此定时器是周期性的，所以申请不到内存就直接返回
    if(pucMem == (void*)NULL)
    {
        return;
    }

    memcpy(pucMem,&stCHCtrMsg,sizeof(stCHCtrMsg));

    if(OSQPost(g_pstCHNthCtrlQue, pucMem) == OS_Q_FULL)
    {        
        MyFree(pucMem);
    }

    //test测试GPRS连接和断开，周期连接和断开
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

    //notice这里不允许申请不到内存
    pucMem = (UCHAR *) MyMalloc(g_pstMemPool256);

    memcpy(pucMem,&stOHCHConnCenterMsg,sizeof(stOHCHConnCenterMsg));

    OSQPost(g_pstCHNthQue, pucMem);
    */
    //test测试GPRS连接和断开，周期连接和断开
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
调试模式超时回调函数
****************************************/

void DebugTimeOutCallBack(void *pvCtx)
{
    g_stTimerDebugTimeOut.pTimer = NULL;
    g_stTimerDebugTimeOut.ulTimerMagicNum = NULL;
    
    g_stYkppParamSet.stYkppCtrlParam.ucDebugMode = FALSE;
    //notice可以考虑输出信息

}

/****************************************
从机注册保护定时器回调函数
****************************************/
void SlaveRegProtectTimeCallBack(void *pvCtx)
{
    g_stTimerSlaveRegProtect.pTimer = NULL;
    g_stTimerSlaveRegProtect.ulTimerMagicNum = NULL;
    //无新从设备正在注册
    g_stYkppParamSet.stYkppCommParam.ucNewSNum = MS_NEWS_DEVNUM;
}

/****************************************
重启设备超时回调函数，目前只用于北向
****************************************/
void ResetTimeCallBack(void * pvCtx)
{
    UCHAR *pucMem;
    CH_CTR_ST stCHCtrMsg;

    stCHCtrMsg.ulPrmvType = CH_MG_RESET;

    pucMem = (UCHAR *) MyMalloc(g_pstMemPool16);
    
    //notice:由于此定时器是周期性的，所以申请不到内存就直接返回
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
  Description:    本函数初始化CH任务
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

    //当前通信状态
    g_ucCurrCommMode = COMM_NO_CONN;
    //重启后为非调试模式，且无主从链路告警,且无新从设备注册
    g_stYkppParamSet.stYkppCtrlParam.ucDebugMode = FALSE;
    g_stYkppParamSet.stYkppCommParam.usCommAlarm= 0x0000;
    g_stYkppParamSet.stYkppCommParam.ucNewSNum = 0;
    
    //根据机型进行配置
    //根据设备类型配置参量列表及对应机型的特殊参数
    //MCM-11_20061101_zhangjie_begin

    if((g_stDevTypeTable.ucOpticalType != OPTICAL_NO) && (g_stDevTypeTable.ucCommRoleType== COMM_ROLE_MASTER))
    {
        g_stNorthInterface.pvCOMMDev = &g_stUsart0;//232(调试时使用该串口)
        g_stSouthInterface.pvCOMMDev = &g_stUsart1;//232
        g_stDebugInterface.pvCOMMDev = &g_stUsart2;//485(不使用调试串口)
        g_stNorthInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
        g_stSouthInterface.ucInterfaceType = USART_SLAVE_OPT;
        g_stDebugInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
        //光端机使能打开
        PioWrite(&OPT_PIO_CTRL,OPT_ENABLE,OPT_ENABLE_ON);
        //光端机设置成输出
        PioOpen(&OPT_PIO_CTRL, OPT_ENABLE,PIO_OUTPUT);
    }
    else if((g_stDevTypeTable.ucOpticalType != OPTICAL_NO) && (g_stDevTypeTable.ucCommRoleType == COMM_ROLE_SLAVE))
    {
        g_stNorthInterface.pvCOMMDev = &g_stUsart1;//232
        g_stSouthInterface.pvCOMMDev = &g_stUsart2;//485
        g_stDebugInterface.pvCOMMDev = &g_stUsart0;//232(调试时使用该串口)
        g_stNorthInterface.ucInterfaceType = USART_SLAVE_OPT;
        g_stSouthInterface.ucInterfaceType = USART_SLAVE_485;
        g_stDebugInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
        //光端机使能关闭
        PioWrite(&OPT_PIO_CTRL,OPT_ENABLE,OPT_ENABLE_OFF);
        //光端机设置成输出
        PioOpen(&OPT_PIO_CTRL, OPT_ENABLE,PIO_OUTPUT);
    }
    else if((g_stDevTypeTable.ucOpticalType == OPTICAL_NO) && (g_stDevTypeTable.ucCommRoleType == COMM_ROLE_SLAVE))
    {
        g_stNorthInterface.pvCOMMDev = &g_stUsart2;//485
        g_stSouthInterface.pvCOMMDev = &g_stUsart1;//485
        g_stDebugInterface.pvCOMMDev = &g_stUsart0;//232(调试时使用该串口)
        g_stNorthInterface.ucInterfaceType = USART_SLAVE_485;
        g_stSouthInterface.ucInterfaceType = USART_SLAVE_485;
        g_stDebugInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
    }
    else
    {
        g_stNorthInterface.pvCOMMDev = &g_stUsart0;//232(调试时使用该串口)
        g_stSouthInterface.pvCOMMDev = &g_stUsart2;//485
        g_stDebugInterface.pvCOMMDev = &g_stUsart1;//232(暂不使用调试串口)
        g_stNorthInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
        g_stSouthInterface.ucInterfaceType = USART_SLAVE_485;
        g_stDebugInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
    }   
    //MCM-11_20061101_zhangjie_end

    
    //防止从机编号非法
    if(g_stYkppParamSet.stYkppCommParam.ucSNum > 0x10)
    {
        g_stYkppParamSet.stYkppCommParam.ucSNum = 0;        
        SaveYkppParam();
    }
    
}

/*************************************************
  Function:       MSChangeBit
  Description:    本函数改变主从协议中从机位图和从机告警状态的各位
                  本函数只使用在南向接口
  Calls:
  Called By:
  Input:          pusBitMap:需要改变的USHORT数据
                  ucNum:  Bit的位置，高位为1，低位为16
                  ucTrue:1置1，0清0
  Output:     
  Return:         
  Others:
*************************************************/
void MSChangeBit(USHORT *pusBitMap,UCHAR ucNum, UCHAR ucTrue)
{
    USHORT usTemp;
    //ucNum取值在1-16间才合法
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
  Description:    本函数改变取得下一个要轮询的从机编号
                  同时自动改变ucAskingSNum的值
                  本函数只使用在南向接口
  Calls:
  Called By:
  Input:          
  Output:     
  Return:         下一个要轮询的从机编号
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
  Description:    本函数取得新的从机编号
                  本函数会改变ucNewSNum的值
                  本函数只使用在南向接口
  Calls:
  Called By:
  Input:          
  Output:     
  Return:         新的从机编号，MS_NEWS_DEVNUM表示没有可分配的从机编号
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
    //notice如果不是FFFF肯定能分配到
    //g_stYkppParamSet.stYkppCommParam.ucNewSNum = MS_NEWS_DEVNUM;
    //return MS_NEWS_DEVNUM;
}


/*************************************************
  Function:       HndlTakeOther
  Description:    本函数对收到的主从协议承载数据进行处理

  Calls:
  Called By:
  Input:          
                  pucMem:  收到的承载数据首指针
                  ulMaxSize: 内存的最大长度
                  ucSrc:   来源，北向or南向
  Output:     
  Return:         
  Others:
*************************************************/
LONG HndlTakeOther(UCHAR* pucMem,ULONG ulMaxSize, UCHAR ucSrc)
{
    MESSAGE_ST *pstMessage;
    OHCH_RECV_DATA_IND_ST *pstOHCHRec;
    //根据不同原语类型做相应处理
    switch(*((ULONG*)pucMem))
    {
    case OHCH_SEND_DATA_REQ:
        //调整数据指针
        pstOHCHRec = (OHCH_RECV_DATA_IND_ST*)pucMem;
        pstOHCHRec->pucData = (UCHAR*)pstOHCHRec + sizeof(*pstOHCHRec);

        if(ucSrc == COMM_IF_SOUTH)
        {
            //原语类型不需要改变
            //pstOHCHRec->ulPrmvType = OHCH_SEND_DATA_REQ;
            pstOHCHRec->ucSrcIf = COMM_IF_SOUTH;
        }
        else
        {
            //改变原语类型
            pstOHCHRec->ulPrmvType = OHCH_RECV_DATA_IND;
            pstOHCHRec->ulDataLenMax = ulMaxSize - sizeof(pstOHCHRec);
            pstOHCHRec->ucSrcIf = COMM_IF_NORTH;
        }

        //发送消息，消息队列满则丢弃数据包
        if(OSQPost(g_pstOHQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            return FAILED;
        }
        break;
    case OHCH_MESS_SEND_REQ:        
        pstMessage = (MESSAGE_ST*)pucMem;
        //改变消息类型
        pstMessage->ulMessType = OHCH_MESS_RECV_IND;

        //发送消息，消息队列满则丢弃数据包
        if(OSQPost(g_pstOHQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            return FAILED;
        }
        break;
    case ACCH_MESS_SEND_REQ:
        pstMessage = (MESSAGE_ST*)pucMem;

        //改变消息类型
        pstMessage->ulMessType = ACCH_MESS_RECV_IND;

        //发送消息，消息队列满则丢弃数据包
        if(OSQPost(g_pstACQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            return FAILED;
        }
        break;
    case OHCH_DEBU_SEND_REQ:
    case OHCH_UNDE_SEND_REQ:
        //调整数据指针
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
        //改变原语类型
        if(pstOHCHRec->ulPrmvType == OHCH_UNDE_SEND_REQ)
        {
            pstOHCHRec->ulPrmvType = OHCH_UNDE_RECV_IND;
            pstOHCHRec->ulDataLenMax = ulMaxSize - sizeof(pstOHCHRec);
        }

        //发送消息，消息队列满则丢弃数据包
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
        //消息无需调整，直接发给OH
        //发送消息，消息队列满则丢弃数据包
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
  Description:    本函数把输入的数据进行主从协议的处理
                       本函数只使用在北向接口
  Calls:
  Called By:
  Input:          
                  pstDataPack:  匹配数据包类型指针
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

    //得出数据包长度
    ulSize = ULONG((pstDataPack->pucInPos)-(pstDataPack->pucData)); 
    //长度判断
    if(ulSize < DATAPACK_MS_MINSIZE)
    {
        return FAILED;
    }

    pucData = pstDataPack->pucData;
    //去除首尾
    pucData = pucData + 3;
    ulSize = ulSize - 4;

    //转义处理
    ulSize = DecodeEscape(pucData, ulSize, MS_ESCAPE_CHAR, 
            g_aucMSEscaped, g_aucMSUnEscape, 2);

    //转义失败退出
    if(ulSize == 0)
    {
        return FAILED;
    }

    //crc校验
    ulSize = ulSize - 2;
    usCrc = CalcuCRC(pucData, ulSize);
    if(usCrc != GET_WORD(pucData + ulSize))
    {
        return FAILED;
    }

    //获得主从协议数据
    stMSProtocol.ucProtocolType = pucData[0];
    stMSProtocol.ucSNum = pucData[1];
    stMSProtocol.ucCmdUnit = pucData[2];
    stMSProtocol.ucFlagUnit = pucData[3];
    stMSProtocol.pucDataUnit = pucData + 4;
    stMSProtocol.ulDataUnitSize = ulSize - 4;

    //判断协议类型
    if(stMSProtocol.ucProtocolType != MS_PROTOCOL_TYPE)
    {
        return FAILED;
    }

    //从标志单元中获取主设备的当前通信状态
    UCHAR ucFlag = stMSProtocol.ucFlagUnit & 0x06;//取出BIT2:1
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
        //跳过登陆和心跳过程直接置状态，这样可以使OH可以上报GPRS数据
        g_stCenterConnStatus.ucLinkStatus = COMM_STATUS_CONNECTED;
        g_stCenterConnStatus.ucLoginStatus = COMM_STATUS_LOGIN;
        break;
    default:
        break;
    }
    

    //判断从机编号(含广播方式)
    if((stMSProtocol.ucSNum != g_stYkppParamSet.stYkppCommParam.ucSNum) &&
            (stMSProtocol.ucSNum != MS_BROADCAST_DEVNUM))
    {
        return FAILED;
    }            

    //判断是否是新设备，是则处理相关指令后退出
    if(stMSProtocol.ucSNum == MS_NEWS_DEVNUM)
    {
        switch(stMSProtocol.ucCmdUnit)
        {
        case MS_CMD_ASK:
            //发送申请注册
            stCHCtrMsg.ulPrmvType = CH_MG_SM_REG;
            bSendCtrlMsg = TRUE;
            break;
        case MS_CMD_REGANS:
            //修改从机编号
            //发送在线应答
            //notice最终是要编写一个设置查询处理的函数
            if(GET_WORD(stMSProtocol.pucDataUnit +2) == 0x8001)
            {
                g_stYkppParamSet.stYkppCommParam.ucSNum = *(stMSProtocol.pucDataUnit +4);
                //notice
                if(g_stYkppParamSet.stYkppCommParam.ucSNum == MS_NEWS_DEVNUM)
                {
                    //发消息上交至OH，通知注册失败，原因是主设备无可用的从机编号
                    return FAILED;
                }                
                //notice写FLASH
                SaveYkppParam();
                stCHCtrMsg.ulPrmvType = CH_MG_SM_ANS;
                bSendCtrlMsg = TRUE;
                //notice发消息至OH，通知注册成功
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

    //判断是否是广播，是则处理相关指令后退出
    //目前由主设备保证广播方式下只会是协议承载
    else if(stMSProtocol.ucSNum == MS_BROADCAST_DEVNUM)
    {
        ULONG ulMaxSize = 0;
        
        switch(stMSProtocol.ucCmdUnit)
        {
        case MS_CMD_TAKEOHTER:
            //把消息交到OH层
            //因为不知是短消息还是直连所以要判断大小
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

            //根据不同原语类型做相应处理
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

    //非新设备且非广播命令单元处理
    else
    {
        switch(stMSProtocol.ucCmdUnit)
        {
        case MS_CMD_ASK:
            //置北向接口闲
            g_stNorthInterface.ucState = COMM_STATE_SEND_IDLE;
            break;
        case MS_CMD_UNREGANS:
            //发送确认删除消息
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

        //发送消息，消息队列满则丢弃数据包
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
  Description:    本函数把输入的数据进行主从协议的处理
                  本函数只使用在南向接口
  Calls:
  Called By:
  Input:          
                  pstDataPack:  匹配数据包类型指针
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

    //关中断防止此时超时
    //避免同时产生轮询超时消息和以下控制消息
    //OS_ENTER_CRITICAL();
    //notice重复地方修改
    //得出数据包长度
    ulSize = (ULONG)((pstDataPack->pucInPos)-(pstDataPack->pucData)); 
    //长度判断
    if(ulSize < DATAPACK_MS_MINSIZE)
    {
        //OS_EXIT_CRITICAL();
        return FAILED;
    }

    pucData = pstDataPack->pucData;
    //去除首尾
    pucData = pucData + 3;
    ulSize = ulSize - 4;

    //转义处理
    ulSize = DecodeEscape(pucData, ulSize, MS_ESCAPE_CHAR, 
            g_aucMSEscaped, g_aucMSUnEscape, 2);

    //转义失败退出
    if(ulSize == 0)
    {
        return FAILED;
    }

    //notice启动接收超时定时器
    //StartGeneralTimer(&g_stTimerSthRecTimeOut, TIME_TIMEOUT_USARTREC/TIMER_UNIT_MS, NULL,NONPERIODIC);

    //crc校验
    ulSize = ulSize - 2;
    usCrc = CalcuCRC(pucData, ulSize);
    if(usCrc != GET_WORD(pucData + ulSize))
    {
        //OS_EXIT_CRITICAL();
        return FAILED;
    }

    //获得主从协议数据
    stMSProtocol.ucProtocolType = pucData[0];
    stMSProtocol.ucSNum = pucData[1];
    stMSProtocol.ucCmdUnit = pucData[2];
    stMSProtocol.ucFlagUnit = pucData[3];    
    stMSProtocol.pucDataUnit = pucData + 4;
    stMSProtocol.ulDataUnitSize = ulSize - 4;

    //判断协议类型
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

    //如果和轮询的不一致，则丢弃该包
    //notice
    /*
    if(stMSProtocol.ucSNum != g_stYkppParamSet.stYkppCommParam.ucAskingSNum)
    {
        //OS_EXIT_CRITICAL();
        return FAILED;
    }
    */
    //判断是否是新设备，是则处理相关指令后退出
    if(stMSProtocol.ucSNum == MS_NEWS_DEVNUM)
    {
        //关轮询超时定时器
        StopGeneralTimer(&g_stTimerSthAskTimeOut);
        //定时器关闭后就可打开中断
        //OS_EXIT_CRITICAL();
        switch(stMSProtocol.ucCmdUnit)
        {
        case SM_CMD_REG:
            //发送申请注册应答
            stCHCtrMsg.ulPrmvType = CH_MG_MS_REGANS;
            bSendCtrlMsg = TRUE;
            break;
        default:
            //发送轮询下一个从机的命令
            stCHCtrMsg.ulPrmvType = CH_MG_MS_ASK;
            bSendCtrlMsg = TRUE;
            break;
        }
    }    

    //判断是否是广播，是则处理相关指令后退出
    //目前南向接口收到广播信息为非法
    else if(stMSProtocol.ucSNum == MS_BROADCAST_DEVNUM)
    {
        //OS_EXIT_CRITICAL();
        return FAILED;
    }

    //非新设备且非广播命令单元处理
    else
    {
        //关轮询超时定时器
        StopGeneralTimer(&g_stTimerSthAskTimeOut);
        //定时器关闭后就可打开中断
        //OS_EXIT_CRITICAL();
        //清除主从通信告警位
        MSChangeBit(&g_stYkppParamSet.stYkppCommParam.usCommAlarm, stMSProtocol.ucSNum, 0);

        ULONG ulMaxSize = 0;
        
        switch(stMSProtocol.ucCmdUnit)
        {
        case SM_CMD_ANS:            
            //如果是新注册设备的应答则改变从机位图
            if(stMSProtocol.ucSNum == g_stYkppParamSet.stYkppCommParam.ucNewSNum)
            {
                MSChangeBit(&g_stYkppParamSet.stYkppCommParam.usSBitmap, stMSProtocol.ucSNum, 1);
                g_stYkppParamSet.stYkppCommParam.ucNewSNum = MS_NEWS_DEVNUM;
                //保存至flash
                SaveYkppParam();
            }           
            
            //发送轮询下一个从机的命令
            stCHCtrMsg.ulPrmvType = CH_MG_MS_ASK;
            bSendCtrlMsg = TRUE;
            break;
        case SM_CMD_UNREG:
            //发送申请删除的应答
            stCHCtrMsg.ulPrmvType = CH_MG_MS_UNREGANS;
            stCHCtrMsg.ucParam1 = stMSProtocol.ucSNum;//需要传递从机编号
            bSendCtrlMsg = TRUE;
            break;
        case SM_CMD_UNREGCONF:
            //发送申请删除应答的时候已经删除相应的从机编号
            //发送轮询下一个从机的命令
            stCHCtrMsg.ulPrmvType = CH_MG_MS_ASK;
            bSendCtrlMsg = TRUE;
            break;
        case SM_CMD_TAKEOHTER:
            //发送轮询下一个从机的命令
            stCHCtrMsg.ulPrmvType = CH_MG_MS_ASK;
            bSendCtrlMsg = TRUE;
            //把消息交到OH
            //因为不知是短消息还是直连所以要判断大小            
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

            //根据不同原语类型做相应处理
            HndlTakeOther(pucMem,ulMaxSize,COMM_IF_SOUTH);
            break;
            
        default:
            //发送轮询下一个从机的命令
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

        //发送消息，消息队列满则丢弃数据包
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
  Description:    本函数把输入的CSD数据包上交至OH层
                       本函数目前只使用在北向接口
  Calls:
  Called By:
  Input:          
                  pstDataPack:  匹配数据包类型指针
  Output:     
  Return:         
  Others:
*************************************************/
LONG DPHandInCSDorGPRS(DATA_PACK_ST *pstDataPack )
{
    UCHAR *pucMem;
    OHCH_RECV_DATA_IND_ST stOHCHRec;
    ULONG ulSize;
    
    //得出数据包长度
    ulSize = ULONG((pstDataPack->pucInPos)-(pstDataPack->pucData));
    
    //申请内存，失败则丢弃数据包
    //notice考虑所需内存大小，小的不够再申请大的

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

    
    //判断是否是GPRS
    if(g_ucCurrCommMode == COMM_MODE_GPRS)
    {
        //解转义
        ulSize = DecodeEscape(pstDataPack->pucData, ulSize, 
                GPRS_ESCAPE_CHAR, g_aucGPRSEscaped, g_aucGPRSUnEscape, 2);

        //转义失败退出
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

    //复制原语及数据包至申请的内存
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


    //test用于主从协议的测试，构建含有原语的数据包
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
    
    //发送消息，消息队列满则丢弃数据包
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
  Description:    本函数把输入的调试数据包以CSD的方式上交至OH层
                       本函数目前只使用在调试接口
  Calls:
  Called By:
  Input:          
                  pstDataPack:  匹配数据包类型指针
  Output:     
  Return:         
  Others:
*************************************************/
LONG DPHandInDeb(DATA_PACK_ST *pstDataPack )
{
    UCHAR *pucMem;
    OHCH_RECV_DATA_IND_ST stOHCHRec;
    ULONG ulSize; 

    //得出数据包长度
    ulSize = ULONG((pstDataPack->pucInPos)-(pstDataPack->pucData));

    //notice 启动定时器防止没有停止debug模式
    StartGeneralTimer(&g_stTimerDebugTimeOut, 
        TIME_TIMEOUT_DEBUG/TIMER_UNIT_MS, NULL,NONPERIODIC);

    //申请内存，失败则丢弃数据包
    //notice考虑所需内存大小，小的不够再申请大的
    /*
    if((sizeof(stOHCHRec)+ulSize) <= MEM_BLK_SIZE_256 - 4)
    {
        pucMem = (UCHAR *) MyMalloc(g_pstMemPool256);
        stOHCHRec.ulDataLenMax = MEM_BLK_SIZE_256 - 4 - sizeof(stOHCHRec);
    }
    */
    //防止查询监控参量列表数据变长
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

    //复制原语及数据包至申请的内存
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

    //notice CH可以直接打开和关闭调试模式
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
        //notice 启动定时器防止没有停止debug模式
        //发送消息至debug队列，消息队列满则丢弃数据包
        if(OSQPost(g_pstCHDebQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            return FAILED;
        }
        return SUCCEEDED;
    }
        


    //非调试模式则不处理
    if(g_stYkppParamSet.stYkppCtrlParam.ucDebugMode != TRUE)
    {
        MyFree(pucMem);
        return SUCCEEDED;
    }
    
    //发送消息，消息队列满则丢弃数据包
    if(OSQPost(g_pstOHQue, pucMem) == OS_Q_FULL)
    {
        MyFree(pucMem);
        return FAILED;
    }

    return SUCCEEDED;
}

/*************************************************
  Function:     ATCMDISEqual
  Description:    本函数比较两个AT命令
  Calls:
  Called By:
  Input:     pucRecAT指向收到的AT命令
                pucConstAT指向定义好的AT命令        
  Output:   
  Return:  TRUE:相等        
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
  Description:    本函数处理AT承载的数据包
                       如果有必要则上交至OH层(如短消息)
                       本函数目前只使用在北向接口
  Calls:
  Called By:
  Input:         
                  pstDataPack:  匹配数据包类型指针
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

    //关中断防止此时超时
    //避免同时产生TIMEOUT 和OK ERROR消息
    OS_ENTER_CRITICAL();
    
    ulATSize = pstDataPack->pucInPos - pstDataPack->pucData;

    //判断长度，去除首尾2组0D0A
    if(ulATSize>5)//notice 去除>的干扰
    {
        ulATSize = ulATSize -4;
        pucATData = pstDataPack->pucData + 2;
    }
    else
    {
        OS_EXIT_CRITICAL();
        return FAILED;
    }

    //默认处理第2个包不成功
    bDP2Succeeded = FAILED;
    
    //首先判断是不是要处理第二个包
    if(g_stDataPackATFull.ucState == DATAPACK_STATE_CMT)
    {
        //由于短消息处理过长且不会产生北向串口控制消息
        //所以提前打开中断
        OS_EXIT_CRITICAL();

        SMS_PARAM_ST stSMS;
        //申请内存，失败则丢弃数据包
        //tese取消512内存的申请
        pucMem = (UCHAR *) MyMalloc(g_pstMemPool512);
        if(pucMem != (void*)NULL)
        {
            //给短消息结构分配内存
            stSMS.pucData = (UCHAR*)pucMem;
            //PDU解码
            if(DecodeAtCmtInPDUMode(g_stDataPackATFull.pucData1, g_stDataPackATFull.ulData1Size, 
                    pucATData, ulATSize, &stSMS) == SUCCEEDED)
            {
                UCHAR *pucMem1;
                OHCH_RECV_DATA_IND_ST stOHCHRec;
                
                bDP2Succeeded = SUCCEEDED;
                //向OH提交SMS消息
                //申请内存，失败则丢弃数据包
                pucMem1 = (UCHAR *) MyMalloc(g_pstMemPool256);
                if(pucMem1 == (void*)NULL)
                {
                    return FAILED;
                }
                //复制原语及数据包至申请的内存
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

                //发送消息，消息队列满则丢弃数据包
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
        //还会收到来电号码
        if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_CLIP, sizeof(g_aucRAT_CLIP)-1))
        {
            //取出号码放入号码暂存区
            memcpy(g_aucCLIPTel,pucATData,ulATSize);
            stCHCtrMsg.ulPrmvType = CH_MG_AT_CSD_CON;
            bSendCtrlMsg = TRUE;
            bDP2Succeeded = SUCCEEDED;
        }
        //又收到一个CRING说明无来电显示,此时摘机
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
        //失去GPRS连接还会收到一个OK
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
        //收到短信中心号码后还会收到一个OK
        if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_OK, sizeof(g_aucRAT_OK)-1))
        {
            //从第一包中取出短信中心号码放入短信中心号码暂存区
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
        //收到小区信息后还会收到一个OK
        if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_OK, sizeof(g_aucRAT_OK)-1))
        {
            //从第一包中取出小区信息放入小区信息暂存区
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

    //首先判断数据包不能过长
    //如果过长则不处理，并已第二包处理结果为准
    //notice max改用宏
    if(ulATSize>g_stDataPackATFull.ulDataPack1MaxSize) 
    {
        OS_EXIT_CRITICAL();
        return bDP2Succeeded;    
    }    

    //处理第二个包不成功则逐个匹配AT命令可以先对+判断
    if(bDP2Succeeded == FAILED)
    {
        //notice先比较较长的AT命令,注意优先级
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
            //改变包状态    
            g_stDataPackATFull.ucState = DATAPACK_STATE_CMT;
            bSendCtrlMsg = FALSE;
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_CMGL, sizeof(g_aucRAT_CMGL)-1))
        {
            //改变包状态    
            g_stDataPackATFull.ucState = DATAPACK_STATE_CMT;
            //有cmgl一定是读出了所有短消息
            g_ucCurrSimSMSSta = SIM_STA_READED;
            bSendCtrlMsg = FALSE;
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_CMTI, sizeof(g_aucRAT_CMTI)-1))
        {
            //sim卡里有未读短消息
            g_ucCurrSimSMSSta = SIM_STA_UNREAD;
            bSendCtrlMsg = FALSE;
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_CRING, sizeof(g_aucRAT_CRING)-1))
        {
            //改变包状态
            //判断是否是数据呼叫
            if(pucATData[ulATSize-1]=='C')
            {
                //启动接收超时定时器
                //可能是未开来电显示功能，要等到下一个CRING才能做判断，所以增加延时
                StartGeneralTimer(&g_stTimerNthRecTimeOut, 
                        3000/TIMER_UNIT_MS, NULL,NONPERIODIC);
                g_stDataPackATFull.ucState = DATAPACK_STATE_CRING;
                bSendCtrlMsg = FALSE;
            }
            //语音呼叫则挂机
            else
            {
                stCHCtrMsg.ulPrmvType = CH_MG_AT_ATH;
                bSendCtrlMsg = TRUE;
            }
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_GPRSAct, sizeof(g_aucRAT_GPRSAct)-1))
        {
            //notice可以从第一个包得到IP地址
            memcpy(g_aucIPAddr,g_stDataPackATFull.pucData1,
                    g_stDataPackATFull.ulData1Size);
            stCHCtrMsg.ulPrmvType = CH_MG_AT_GPRS_ACT;
            bSendCtrlMsg = TRUE;        
        }
        else if(ATCMDISEqual(pucATData, ulATSize, 
                g_aucRAT_GPRSActed, sizeof(g_aucRAT_GPRSActed)-1))
        {
            //此时GPRS已激活
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
            //改变包状态
            g_stDataPackATFull.ucState = DATAPACK_STATE_UNKNOW;
            bSendCtrlMsg = FALSE;        
        }    
    }

    //以下处理是需要发送控制消息
    if(bSendCtrlMsg)
    {
        //停止定时器
        StopGeneralTimer(&g_stTimerATRtnTimeOut);

        //停止定时器后开中断，因为不会产生TIMEOUT消息
        OS_EXIT_CRITICAL();

        pucMem = (UCHAR *) MyMalloc(g_pstMemPool16);
        if(pucMem == (void*)NULL)
        {
            //重新打开定时器
            StartGeneralTimer(&g_stTimerATRtnTimeOut, TIME_WAIT_SMSSEND/TIMER_UNIT_MS, NULL,NONPERIODIC);
            return FAILED;
        }
        memcpy(pucMem,&stCHCtrMsg,sizeof(stCHCtrMsg));

        //发送消息，消息队列满则丢弃数据包
        if(OSQPost(g_pstCHNthCtrlQue, pucMem) == OS_Q_FULL)
        {
            MyFree(pucMem);
            //重新打开定时器
            StartGeneralTimer(&g_stTimerATRtnTimeOut, TIME_WAIT_SMSSEND/TIMER_UNIT_MS, NULL,NONPERIODIC);        
            return FAILED;
        }
        return SUCCEEDED;
    }
    //以下处理是不需要发送控制消息    
    else
    {
        OS_EXIT_CRITICAL();
        //无论何种情况均缓存第一个包
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
  Description:    本函数把输入的数据包进行首尾字符的匹配
                       只用于单字节且相同的首尾字符
                       如果找到匹配包则直接调用相应函数进行处理
                       该函数目前使用在CMCC的APAC和APB协议的匹配
                       该函数目前只使用在北向接口
  Calls: 
  Called By:
  Input:          pucData:         输入的数据
                  ulSize:            输入数据的长度
                  pstDataPack:  匹配数据包类型指针
                  ucMatch:      用于匹配的首尾字符
  Output:         pstDataPack:  匹配数据包类型指针
                                       含匹配后得到的数据和状态等信息
                  pfFun:   完整匹配包处理函数的指针
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
            //属于NO S NO E
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {                    
                //停止处理
                break;
            }
            //属于NO S ONLY E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //首尾字符相同时这种情况是非法的              
                break;
            }
            //属于ONLY S NO E
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                memcpy(pstDataPack->pucData, pucData+ulStartPos, ulSize-ulStartPos);
                pstDataPack->pucInPos = pstDataPack->pucInPos + ulSize-ulStartPos;
                pstDataPack->ucState= DATAPACK_STATE_START;
                break;
            }
            //ulStartPos != FIND_NO_CHAR && ulEndPos != FIND_NO_CHAR
            //属于S E
            else if(ulStartPos < ulEndPos)
            {
                memcpy(pstDataPack->pucData, 
                        pucData+ulStartPos, ulEndPos - ulStartPos +1);
                pstDataPack->pucInPos = pstDataPack->pucInPos + ulEndPos - ulStartPos +1;
                //处理完整包
                pfFun(pstDataPack);
                //处理完后状态为无数据包 
                pstDataPack->ucState = DATAPACK_STATE_NO;
                //调整剩余包长度，保留尾字符，因为首尾相同
                ulSize = ulSize- ulEndPos;
                //读出剩余包数据                    
                memcpy(pucData, pucData+ulEndPos, ulSize);
                //继续处理剩余数据包
                continue;
            }
            //首尾字符相同时此状态为非法
            //属于E  S
            else
            {
                break;
            }
        }//pstDataPack->ucState == DATAPACK_STATE_NO
        
        if(pstDataPack->ucState == DATAPACK_STATE_START)
        {
            //属于ONLY S NO E
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //确保数据包不溢出
                if((pstDataPack->pucInPos + ulSize) <= 
                        ((pstDataPack->pucData) + (pstDataPack->ulDataPackMaxSize)))
                {
                    memcpy(pstDataPack->pucInPos, pucData, ulSize);
                    pstDataPack->pucInPos = pstDataPack->pucInPos+ ulSize;
                    //处理完毕
                    break;
                }
                //数据溢出，恢复原状
                else
                {
                    pstDataPack->ucState= DATAPACK_STATE_NO;
                    pstDataPack->pucInPos = pstDataPack->pucData;
                    break;
                }
            }
            //属于S E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                pstDataPack->ucState = DATAPACK_STATE_NO;
                pstDataPack->pucInPos = pstDataPack->pucData;
                break;
            }
            //属于S S
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //确保数据包不溢出
                if((pstDataPack->pucInPos + ulStartPos + 1) <= 
                        ((pstDataPack->pucData) + (pstDataPack->ulDataPackMaxSize)))
                {            
                    //前面收到的包S S有效，因为首尾相同
                    memcpy(pstDataPack->pucInPos, pucData, ulStartPos+1);
                    pstDataPack->pucInPos =  pstDataPack->pucInPos + ulStartPos+1;
                    //处理完整包
                    pfFun(pstDataPack);
                }
                //因为还有剩余包，所以溢出时继续处理

                //处理完后状态为无数据包
                pstDataPack->ucState = DATAPACK_STATE_NO;
                
                //调整剩余包长度，保留尾字符，因为首尾相同
                ulSize = ulSize- ulStartPos;
                //读出剩余包数据                    
                memcpy(pucData, pucData+ulStartPos, ulSize);
                //继续处理剩余数据包
                continue;
            }
            //ulStartPos != FIND_NO_CHAR && ulEndPos != FIND_NO_CHAR
            //属于S S E,因为首尾字符相同所以有两个有效包                    
            else if(ulStartPos < ulEndPos)
            {
                //确保数据包不溢出
                if((pstDataPack->pucInPos + ulStartPos + 1) <= 
                        (pstDataPack->pucData + pstDataPack->ulDataPackMaxSize))
                {            
                    //前面收到的包S S有效，因为首尾相同
                    memcpy(pstDataPack->pucInPos, pucData, ulStartPos+1);
                    pstDataPack->pucInPos =  pstDataPack->pucInPos + ulStartPos+1;
                    //处理完整包
                    pfFun(pstDataPack);
                }
                //因为还有剩余包，所以溢出时继续处理

                //指针复位
                pstDataPack->pucInPos = pstDataPack->pucData;

                //后面收到的包S E有效
                memcpy(pstDataPack->pucInPos, pucData+ulStartPos, ulEndPos-ulStartPos+1);
                pstDataPack->pucInPos =  pstDataPack->pucInPos + ulEndPos-ulStartPos+1;
                //处理完整包
                pfFun(pstDataPack);

                //处理完后状态为无数据包
                pstDataPack->ucState = DATAPACK_STATE_NO;
                //调整剩余包长度，保留尾字符，因为首尾相同
                ulSize = ulSize- ulEndPos;
                //读出剩余包数据                    
                memcpy(pucData, pucData+ulEndPos, ulSize);
                //继续处理剩余数据包
                continue;
            }
            //属于S E S,首尾相同时为非法状态
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
  Description:    本函数把输入的数据包进行首尾字符的匹配
                       只用于单字节且不相同的首尾字符
                       如果找到匹配包则直接调用相应函数进行处理
                       该函数目前使用在厂家协议的匹配
                       该函数目前只使用在北向接口
  Calls: 
  Called By:
  Input:          pucData:         输入的数据
                  ulSize:            输入数据的长度
                  pstDataPack:  匹配数据包类型指针
                  ucMatch1,ucMatch2:      用于匹配的首尾字符
  Output:         pstDataPack:  匹配数据包类型指针
                                       含匹配后得到的数据和状态等信息
                  pfFun:   完整匹配包处理函数的指针
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
            //属于NO S NO E
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {                    
                //退出
                break;
            }
            //属于NO S ONLY E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //退出
                break;
            }
            //属于ONLY S NO E
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //防止S后面还有S
                *(pstDataPack->pucInPos) = ucMatch1;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                pstDataPack->ucState= DATAPACK_STATE_START;

                //处理剩余包
                ulSize = ulSize -ulStartPos -1;
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                
                continue;
            }
            //ulStartPos != FIND_NO_CHAR && ulEndPos != FIND_NO_CHAR
            //属于S E
            else
            {
                //防止S后面还有S
                *(pstDataPack->pucInPos) = ucMatch1;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                pstDataPack->ucState= DATAPACK_STATE_START;

                //处理剩余包
                ulSize = ulSize -ulStartPos -1;
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                
                continue;
            }
        }//if(pstDataPack.ucState== DATAPACK_STATE_NO)
        
        if(pstDataPack->ucState == DATAPACK_STATE_START)
        {
            //属于ONLY S NO E
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //确保数据包不溢出
                if((pstDataPack->pucInPos + ulSize) <= 
                        (pstDataPack->pucData + pstDataPack->ulDataPackMaxSize))
                {
                    memcpy(pstDataPack->pucInPos, pucData, ulSize);
                    pstDataPack->pucInPos = pstDataPack->pucInPos + ulSize;
                }
                else
                {
                    //前面收到的包无效
                    pstDataPack->ucState = DATAPACK_STATE_NO;                
                }
                //退出
                break;
            }
            //属于S E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //确保数据包不溢出
                if((pstDataPack->pucInPos + ulEndPos + 1) <= 
                        (pstDataPack->pucData + pstDataPack->ulDataPackMaxSize))
                {
                    memcpy(pstDataPack->pucInPos,pucData, ulEndPos + 1);
                    pstDataPack->pucInPos = pstDataPack->pucInPos + ulEndPos + 1;
                    //处理完整包
                    pfFun(pstDataPack);
                }

                pstDataPack->ucState = DATAPACK_STATE_NO;
                //因为没有S所以剩余包无效
                break;
            }
            //属于S S
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //指针复位
                pstDataPack->pucInPos = pstDataPack->pucData;
                //防止S后面还有S
                *(pstDataPack->pucInPos) = ucMatch1;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                pstDataPack->ucState= DATAPACK_STATE_START;

                //处理剩余包
                ulSize = ulSize -ulStartPos -1;
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                
                continue;
            }
            //ulStartPos != FIND_NO_CHAR && ulEndPos != FIND_NO_CHAR
            //属于S S E                    
            else if(ulStartPos < ulEndPos)
            {
                //前面收到的包S S无效
                //指针复位
                pstDataPack->pucInPos = pstDataPack->pucData;
                //防止S后面还有S
                *(pstDataPack->pucInPos) = ucMatch1;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                pstDataPack->ucState= DATAPACK_STATE_START;

                //处理剩余包
                ulSize = ulSize -ulStartPos -1;
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                
                continue;
            }
            //属于S E S
            //ulEndPos前面的包S E为有效包
            else if(ulStartPos > ulEndPos)
            {
                //确保数据包不溢出
                if((pstDataPack->pucInPos + ulEndPos + 1) <= 
                        (pstDataPack->pucData + pstDataPack->ulDataPackMaxSize))
                {
                    memcpy(pstDataPack->pucInPos, pucData, ulEndPos +1);
                    pstDataPack->pucInPos  = pstDataPack->pucInPos + ulEndPos + 1;
                    //处理完整包
                    pfFun(pstDataPack);
                }
                
                //E S为无效数据包

                //处理S后面的数据                
                //指针复位
                pstDataPack->pucInPos = pstDataPack->pucData;
                //防止S后面还有S
                *(pstDataPack->pucInPos) = ucMatch1;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                pstDataPack->ucState= DATAPACK_STATE_START;

                //处理剩余包
                ulSize = ulSize -ulStartPos -1;
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                
                continue;
            }
            //目前无此种情况
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
  Description:    本函数把输入的数据包进行首尾字符的匹配
                       只用于双字节且相同的首尾字符
                       如果找到匹配包则直接调用相应函数进行处理
                       该函数目前使用AT命令的匹配
                       该函数目前只使用在北向接口
  Calls: 
  Called By:
  Input:          pucData:      输入的数据
                  ulSize:       输入数据的长度
                  pstDataPack:  匹配数据包类型指针
                  ucMatch:      用于匹配的首尾字符
  Output:         pstDataPack:  匹配数据包类型指针
                                含匹配后得到的数据和状态等信息
                  pfFun:        完整匹配包处理函数的指针
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
            //属于NO S1
            if(ulStartPos1 == FIND_NO_CHAR)
            {                    
                //停止处理
                break;
            }
            //属于ONLY S1 NO S2
            else if(ulStartPos2 == FIND_NO_CHAR)
            {
                //S1在最后
                //以下表达方式避免连续出现S1
                if(pucData[ulSize -1] == ucMatch1)
                {
                    *(pstDataPack->pucInPos) = ucMatch1;
                    (pstDataPack->pucInPos)++;
                    pstDataPack->ucState = DATAPACK_STATE_START_1;
                }
                break;
            }
            //属于S1 S2连在一起
            else if(ulStartPos2 == ulStartPos1 + 1)
            {
                *(pstDataPack->pucInPos) = ucMatch1;
                (pstDataPack->pucInPos)++;
                *(pstDataPack->pucInPos) = ucMatch2;
                (pstDataPack->pucInPos)++;
                pstDataPack->ucState = DATAPACK_STATE_START;
                //如果S2不是最后一个则复制剩余包继续判断
                if(ulStartPos2 != ulSize -1)
                {
                    ulSize = ulSize -ulStartPos2 -1;
                    memcpy(pucData, pucData+ulStartPos2+1, ulSize);
                    continue;
                }
                break;
            }	
            //属于S1S2不连在一起且S1在前
            else if(ulStartPos1 < ulStartPos2)
            {
                //从S2前一个开始复制,因S2前面还可能有S1
                ulSize = ulSize - ulStartPos2 + 1;
                memcpy(pucData, pucData+ulStartPos2-1, ulSize);
                //pstDataPack->ucState = DATAPACK_STATE_NO;	
                continue;
            }	 
            //属于S1S2不连在一起且S2在前
            else
            {
                //从S1开始复制,因S1后面还可能有S2
                ulSize = ulSize - ulStartPos1;
                memcpy(pucData, pucData+ulStartPos1, ulSize);
                //pstDataPack->ucState = DATAPACK_STATE_NO;
                continue;
            }	
        }//pstDataPack->ucState == DATAPACK_STATE_NO

        else if(pstDataPack->ucState == DATAPACK_STATE_START_1)
        {
            //属于ONLY S1 NO S2
            if(ulStartPos2 == FIND_NO_CHAR)
            {                    
                //S1 在最后,和目前状态一致
                if(pucData[ulSize -1] == ucMatch1)
                {
                    break;                
                }
                //停止处理
                pstDataPack->ucState = DATAPACK_STATE_NO;                
                break;
            }
            //属于S1 S2连在一起
            else if(ulStartPos2 == 0)
            {
                *(pstDataPack->pucInPos) = ucMatch2;
                (pstDataPack->pucInPos)++;
                pstDataPack->ucState = DATAPACK_STATE_START;
                //如果S2不是最后一个则复制剩余包继续判断
                if(ulStartPos2 != ulSize -1)
                {
                    ulSize = ulSize -1;
                    memcpy(pucData, pucData+1, ulSize);
                    continue;
                }
                break;
            }
            //属于原有的S1和现在的S2不连在一起
            else
            {
                //从S2前一个开始复制,因S2前面还可能有S1
                ulSize = ulSize - ulStartPos2 + 1;
                memcpy(pucData, pucData+ulStartPos2-1, ulSize);
                pstDataPack->ucState = DATAPACK_STATE_NO;
                continue;
            }
        }//pstDataPack->ucState == DATAPACK_STATE_START_1

        else if(pstDataPack->ucState == DATAPACK_STATE_START)
        {
            //属于S1S2
            if((ulStartPos1 == FIND_NO_CHAR) && (ulStartPos2 == FIND_NO_CHAR))
            { 
                //确保数据包不溢出
                if((pstDataPack->pucInPos + ulSize) <= 
                        ((pstDataPack->pucData) + (pstDataPack->ulDataPackMaxSize)))
                {
                    //复制所有
                    memcpy(pstDataPack->pucInPos, pucData, ulSize);
                    pstDataPack->pucInPos = pstDataPack->pucInPos + ulSize;
                    break;
                }
                //数据溢出，恢复原状
                else
                {
                    pstDataPack->ucState= DATAPACK_STATE_NO;
                    break;
                }
            }
            //属于S1S2 E1
            else if((ulStartPos1 != FIND_NO_CHAR) && (ulStartPos2 == FIND_NO_CHAR))
            {
                //如果E1不是最后一个则非法
                if(ulStartPos1 != ulSize -1)
                {
                    //要避免连续S1的情况
                    //数据包最后一个不为S1                
                    if(pucData[ulSize -1] != ucMatch1)
                    {
                        pstDataPack->ucState = DATAPACK_STATE_NO;
                        break;
                    } 
                    //数据包最后一个为S1
                    else
                    {
                        //指针复位
                        pstDataPack->pucInPos = pstDataPack->pucData;
                        
                        *(pstDataPack->pucInPos) = ucMatch1;
                        (pstDataPack->pucInPos)++;                    
                        pstDataPack->ucState = DATAPACK_STATE_START_1;
                        break;
                    } 
                }
                //如果E1是最后一个
                else
                {
                    //确保数据包不溢出
                    if((pstDataPack->pucInPos + ulSize) <= 
                            ((pstDataPack->pucData) + (pstDataPack->ulDataPackMaxSize)))
                    {
                        //复制所有
                        memcpy(pstDataPack->pucInPos, pucData, ulSize);
                        pstDataPack->pucInPos = pstDataPack->pucInPos + ulSize;
                        pstDataPack->ucState = DATAPACK_STATE_END_1;
                        break;
                    }
                    //数据溢出，恢复原状
                    else
                    {
                        pstDataPack->ucState= DATAPACK_STATE_NO;
                        break;
                    }      
                }
                break;
            }
            //属于S1S2 E2
            else if((ulStartPos1 == FIND_NO_CHAR) && (ulStartPos2 != FIND_NO_CHAR))
            {
                pstDataPack->ucState = DATAPACK_STATE_NO;
                break;
            }
            //属于S1S2 E1E2
            else if(ulStartPos1 == ulStartPos2 - 1)
            {
                //确保数据包不溢出
                if((pstDataPack->pucInPos + ulSize) <= 
                        ((pstDataPack->pucData) + (pstDataPack->ulDataPackMaxSize)))
                {
                    //复制E2前面的包并处理完整包            
                    memcpy(pstDataPack->pucInPos, pucData, ulStartPos2+1); 
                    pstDataPack->pucInPos = pstDataPack->pucInPos + ulStartPos2+1;
                    pfFun(pstDataPack);
                }
                //数据溢出，恢复原状
                else
                {
                    pstDataPack->ucState= DATAPACK_STATE_NO;
                }

                //指针复位
                pstDataPack->pucInPos = pstDataPack->pucData;
                //尾保留
                *(pstDataPack->pucInPos) = ucMatch1;
                (pstDataPack->pucInPos)++;
                *(pstDataPack->pucInPos) = ucMatch2;
                (pstDataPack->pucInPos)++;
                pstDataPack->ucState = DATAPACK_STATE_START;
                //如果E2不是最后一个则复制剩余包继续判断
                if(ulStartPos2 != ulSize -1)
                {
                    ulSize = ulSize -ulStartPos2 -1;
                    memcpy(pucData, pucData+ulStartPos2+1, ulSize);
                    continue;
                }
                break;
            }
            //属于E1E2不连在一起且E1在前
            else if(ulStartPos1 < ulStartPos2)
            {
                //从E2前一个开始复制,因E2前面还可能有E1
                ulSize = ulSize - ulStartPos2 + 1;
                memcpy(pucData, pucData+ulStartPos2-1, ulSize);
                pstDataPack->ucState = DATAPACK_STATE_NO;
                continue;
            } 
            //属于E1E2不连在一起且E2在前
            else
            {
                //从E1开始复制,因E1后面还可能有E2
                ulSize = ulSize - ulStartPos1;
                memcpy(pucData, pucData+ulStartPos1, ulSize);
                pstDataPack->ucState = DATAPACK_STATE_NO;
                continue;
            }  
        }//pstDataPack->ucState == DATAPACK_STATE_START

        else if(pstDataPack->ucState == DATAPACK_STATE_END_1)
        {
            //属于ONLY S1S2E1 NO E2
            if(ulStartPos2 == FIND_NO_CHAR)
            { 
                //E1 在最后
                if(pucData[ulSize -1] == ucMatch1)
                {
                    //指针复位
                    pstDataPack->pucInPos = pstDataPack->pucData;                
                    *(pstDataPack->pucInPos) = ucMatch1;
                    (pstDataPack->pucInPos)++;
                    pstDataPack->ucState = DATAPACK_STATE_START_1;                
                    break;                
                }            
                //停止处理
                pstDataPack->ucState = DATAPACK_STATE_NO;                
                break;
            }
            //属于S1S2 E1E2
            else if(ulStartPos2 == 0)
            {
                //处理完整包            
                *(pstDataPack->pucInPos) = ucMatch2;
                (pstDataPack->pucInPos)++;
                pfFun(pstDataPack);

                //指针复位
                pstDataPack->pucInPos = pstDataPack->pucData;
                //尾保留
                *(pstDataPack->pucInPos) = ucMatch1;
                (pstDataPack->pucInPos)++;
                *(pstDataPack->pucInPos) = ucMatch2;
                (pstDataPack->pucInPos)++;
                pstDataPack->ucState = DATAPACK_STATE_START;
                
                //如果E2不是最后一个则复制剩余包继续判断
                if(ulStartPos2 != ulSize -1)
                {
                    ulSize = ulSize -ulStartPos2-1;
                    memcpy(pucData, pucData+ulStartPos2+1, ulSize);
                    continue;
                }
                break;
            }
            //属于原有的E1和现在的E2不连在一起
            else
            {
                //从E2前一个开始复制,因E2前面还可能有E1
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
  Description:    本函数把输入的数据包进行首尾字符的匹配
                       用于首字符为3个尾字符1个且字符相同的数据
                       如果找到匹配包则调用相应函数进行处理
                       该函数目前使用在主从协议的匹配
                       该函数目前在北向接口和南向接口中均使用
  Calls: 
  Called By:
  Input:          pucData:         输入的数据
                  ulSize:            输入数据的长度
                  pstDataPack:  匹配数据包类型指针
                  ucMatch:      用于匹配的首尾字符
  Output:     pstDataPack:  匹配数据包类型指针
                                       含匹配后得到的数据和状态等信息
                  pfFun:   完整匹配包处理函数的指针
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
            //属于NO S 
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {                    
                //停止处理
                break;
            }
            //属于NO S ONLY E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //首尾字符相同时这种情况是非法的              
                break;
            }
            //属于ONLY 1个S 
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //判断S是不是在最后
                if(ulStartPos == ulSize - 1)
                {
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                    pstDataPack->ucState= DATAPACK_STATE_START_1;
                }
                break;
            }
            //ulStartPos != FIND_NO_CHAR && ulEndPos != FIND_NO_CHAR
            //属于2个S且连在一起
            else if(ulStartPos == ulEndPos-1)
            {
                //写入2个S
                *(pstDataPack->pucInPos) = ucMatch;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                *(pstDataPack->pucInPos) = ucMatch;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;

                pstDataPack->ucState = DATAPACK_STATE_START_2;

                //调整剩余包长度
                ulSize = ulSize- ulEndPos-1;
                //读出剩余包数据                    
                memcpy(pucData, pucData+ulEndPos+1, ulSize);
                //继续处理剩余数据包
                continue;
            }
            //属于2个S且不连在一起
            else
            {
                //写入1个S
                *(pstDataPack->pucInPos) = ucMatch;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;

                pstDataPack->ucState = DATAPACK_STATE_START_1;

                //调整剩余包长度
                ulSize = ulSize- ulEndPos-1;
                //读出剩余包数据                    
                memcpy(pucData, pucData+ulEndPos+1, ulSize);
                //继续处理剩余数据包
                continue;
            }
        }//pstDataPack->ucState == DATAPACK_STATE_NO

        else if(pstDataPack->ucState == DATAPACK_STATE_START_1)
        {
            //属于NO S 
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {                    
                //有数据且没有找到则停止处理
                if(ulSize != 0)
                {
                    pstDataPack->ucState= DATAPACK_STATE_NO;
                }
                break;
            }
            //属于NO S ONLY E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //首尾字符相同时这种情况是非法的
                pstDataPack->ucState= DATAPACK_STATE_NO;
                break;
            }
            //属于ONLY 1个S 
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //判断S是不是在最前
                if(ulStartPos == 0)
                {
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                    pstDataPack->ucState= DATAPACK_STATE_START_2;

                    //调整剩余包长度
                    ulSize = ulSize- ulStartPos-1;
                    //读出剩余包数据                    
                    memcpy(pucData, pucData+ulStartPos+1, ulSize);
                    //继续处理剩余数据包
                    continue;                    
                }
                //判断S是不是在最后
                else if(ulStartPos == ulSize - 1)
                {
                    //指针复位
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
            //属于2个S且连在一起
            else if(ulStartPos == ulEndPos-1)
            {
                //S在开头
                if(ulStartPos == 0)
                {
                    //写入2个S
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;                    
                    pstDataPack->ucState= DATAPACK_STATE_START;

                    //调整剩余包长度
                    ulSize = ulSize- ulEndPos-1;
                    //读出剩余包数据                    
                    memcpy(pucData, pucData+ulEndPos+1, ulSize);
                    //继续处理剩余数据包
                    continue;                
                }
                //S不在开头且连在一起
                else
                {
                    //指针复位
                    pstDataPack->pucInPos = pstDataPack->pucData;                
                    //写入2个S
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;

                    pstDataPack->ucState = DATAPACK_STATE_START_2;

                    //调整剩余包长度
                    ulSize = ulSize- ulEndPos-1;
                    //读出剩余包数据                    
                    memcpy(pucData, pucData+ulEndPos+1, ulSize);
                    //继续处理剩余数据包
                    continue;
                }
            }
            //属于2个S且不连在一起
            else
            {
                //指针复位
                pstDataPack->pucInPos = pstDataPack->pucData;             
                //写入1个S
                *(pstDataPack->pucInPos) = ucMatch;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;

                pstDataPack->ucState = DATAPACK_STATE_START_1;

                //调整剩余包长度
                ulSize = ulSize- ulEndPos-1;
                //读出剩余包数据                    
                memcpy(pucData, pucData+ulEndPos+1, ulSize);
                //继续处理剩余数据包
                continue;
            }
        }//else if(pstDataPack->ucState == DATAPACK_STATE_NO)

        else if(pstDataPack->ucState == DATAPACK_STATE_START_2)
        {
            //属于NO S 
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {                    
                //有数据且没有找到则停止处理
                if(ulSize != 0)
                {
                    pstDataPack->ucState= DATAPACK_STATE_NO;
                }
                break;
            }
            //属于NO S ONLY E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //首尾字符相同时这种情况是非法的
                pstDataPack->ucState= DATAPACK_STATE_NO;
                break;
            }
            //属于ONLY 1个S 
            else if((ulStartPos != FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //判断S是不是在最前
                if(ulStartPos == 0)
                {
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                    pstDataPack->ucState= DATAPACK_STATE_START;

                    //调整剩余包长度
                    ulSize = ulSize- ulStartPos-1;
                    //读出剩余包数据                    
                    memcpy(pucData, pucData+ulStartPos+1, ulSize);
                    //继续处理剩余数据包
                    continue;                    
                }
                //判断S是不是在最后
                else if(ulStartPos == ulSize - 1)
                {
                    //指针复位
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
             //S在开头
            else if(ulStartPos == 0)
            {
                //写入1个S
                *(pstDataPack->pucInPos) = ucMatch;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                 
                pstDataPack->ucState= DATAPACK_STATE_START;

                //调整剩余包长度
                ulSize = ulSize- ulStartPos-1;
                //读出剩余包数据                    
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                //继续处理剩余数据包
                continue;                
            }
            //S不在开头
            else
            {
                    //指针复位
                    pstDataPack->pucInPos = pstDataPack->pucData;                
                    //写入1个S
                    *(pstDataPack->pucInPos) = ucMatch;
                    pstDataPack->pucInPos = pstDataPack->pucInPos + 1;

                    pstDataPack->ucState = DATAPACK_STATE_START_1;

                    //调整剩余包长度
                    ulSize = ulSize- ulStartPos-1;
                    //读出剩余包数据                    
                    memcpy(pucData, pucData+ulStartPos+1, ulSize);
                    //继续处理剩余数据包
                    continue;
            }
        }//else if(pstDataPack->ucState == DATAPACK_STATE_START_2)

        else if(pstDataPack->ucState == DATAPACK_STATE_START)
        {
            //属于NO S 
            if((ulStartPos == FIND_NO_CHAR) && (ulEndPos == FIND_NO_CHAR))
            {
                //确保数据包不溢出
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
            //属于NO S ONLY E
            else if((ulStartPos == FIND_NO_CHAR) && (ulEndPos != FIND_NO_CHAR))
            {
                //首尾字符相同时这种情况是非法的
                pstDataPack->ucState= DATAPACK_STATE_NO;
                break;
            }
            //属于ulStartPos != FIND_NO_CHAR
            //在E之前有其他数据
            else if((ulStartPos != 0)||(pstDataPack->pucInPos-pstDataPack->pucData>3))
            {
                //处理完整包
                //确保数据包不溢出
                if((pstDataPack->pucInPos + ulStartPos+1) <= 
                        ((pstDataPack->pucData) + (pstDataPack->ulDataPackMaxSize)))
                {                 
                    memcpy(pstDataPack->pucInPos,pucData,ulStartPos+1);
                    pstDataPack->pucInPos = pstDataPack->pucInPos + ulStartPos+1;
                    pfFun(pstDataPack);
                }

                //保留S,因为首尾字符相同
                //指针复位
                pstDataPack->pucInPos = pstDataPack->pucData;
                    
                *(pstDataPack->pucInPos) = ucMatch;
                pstDataPack->pucInPos = pstDataPack->pucInPos + 1;
                pstDataPack->ucState= DATAPACK_STATE_START_1;

                //调整剩余包长度
                ulSize = ulSize- ulStartPos-1;
                //读出剩余包数据                    
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                //继续处理剩余数据包
                continue;
            }            
            //在E之前没有其他数据,说明连续出现超过3个AA
            else
            {
                //状态不变，仍然为DATAPACK_STATE_START
                //调整剩余包长度
                ulSize = ulSize- ulStartPos-1;
                //读出剩余包数据                    
                memcpy(pucData, pucData+ulStartPos+1, ulSize);
                //继续处理剩余数据包
                continue;                
            }
        }//else if(pstDataPack->ucState == DATAPACK_STATE_START)        
        
    }//for(;;)
}

/*************************************************
  Function:       CHSthSendReady
  Description:    本函数判断通信接口是否空闲
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为通信接口的通信对象

  Output:         TRUE:空闲FALSE:忙     
  Return:         
  Others:
*************************************************/
BOOL CHSendReady(COMM_OBJ_ST *pstCOMMObj)
{
    //判断何种通信对象
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {    
        //判断南向串口是否还有数据未发送
        if(UsartSendReady ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev)))
        {
            //串口无数据发送则再根据接口类型做进一步判断
            switch(pstCOMMObj->ucInterfaceType)
            {
            //以下三种情况处理方式相同，因为南向接口不需要等待轮询后发送
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
  Description:    本函数处理南向接口收到的数据
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为南向接口的通信对象

  Output:     
  Return:         
  Others:
*************************************************/
void HndlCHSthRec(COMM_OBJ_ST *pstCOMMObj)
{
    UINT32  cpu_sr;
    ULONG ulCount = 0;
    UCHAR aucRecToMatch[USART_SIZE_BUFFER];//用于某种数据包匹配
    //判断何种通信对象
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {    
        //读南向串口
        if(UsartReadReady ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev)))
        {
            //notice启动接收超时定时器            
            StartGeneralTimer(&g_stTimerSthRecTimeOut, TIME_TIMEOUT_USARTREC/TIMER_UNIT_MS, NULL,NONPERIODIC);

            //读出数据
            OS_ENTER_CRITICAL();
            ulCount = UsartRead ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev), aucRecToMatch);
            OS_EXIT_CRITICAL();

            //无需根据接口类型对数据进行处理
            //因为南向接口只会收到主从协议

            //匹配主从协议             
            DPMatch31(&g_stDataPackSthMS, MS_START_END_FLAG, 
                    aucRecToMatch, ulCount, (DP_HNDL_FUNC)DPSthRecMSHndl);
        }
    }
}

/*************************************************
  Function:     HndlCHSthSend
  Description:    本函数处理南向接口发送的数据
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为南向接口的通信对象
                     pvPrmv:              接收到的消息指针
  Output:     
  Return:         
  Others:       先判断通信对象再判断通信接口最后判断原语类型
*************************************************/
void HndlCHSthSend(COMM_OBJ_ST *pstCOMMObj, void * pvPrmv)
{
    OHCH_SEND_DATA_REQ_ST *pstOHCHDataReq = (OHCH_SEND_DATA_REQ_ST *)pvPrmv;   
    MESSAGE_ST *pstMessage = (MESSAGE_ST *)pvPrmv;
    //判断何种通信对象
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {
        //如果是从设备且连接光端机(判断通信接口)
        if(pstCOMMObj->ucInterfaceType == USART_SLAVE_OPT ||
            pstCOMMObj->ucInterfaceType == USART_SLAVE_485)    
        {
            UCHAR aucData[USART_SIZE_BUFFER];
            ULONG ulSize;
            UCHAR ucFlag = 0;

            //写入标志位，目前含有通信状态信息
            //写入通信状态
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
           
            //根据原语类型进行处理
            switch(*((ULONG*)pvPrmv))
            {
            //由于是从设备所以对以下原语类型处理一样
            //用主从协议打包后直接发送
            case OHCH_SEND_DATA_REQ:
            case OHCH_DEBU_SEND_REQ:
            case OHCH_UNDE_SEND_REQ:                
                //写入消息数据
                memcpy(aucData,pstOHCHDataReq,sizeof(*pstOHCHDataReq));
                memcpy(aucData+sizeof(*pstOHCHDataReq), 
                        pstOHCHDataReq->pucData, pstOHCHDataReq->ulDataLen);
                ulSize = sizeof(*pstOHCHDataReq) + pstOHCHDataReq->ulDataLen;
                

                //对消息数据进行主从协议的编码,以广播方式发送
                ulSize = EncodeMSProtocol(MS_BROADCAST_DEVNUM, MS_CMD_TAKEOHTER,ucFlag, 
                        aucData, ulSize);
                
                //发送数据
                SthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                //notice回原语类型统一
                //pstOHCHDataReq->ulPrmvType = OHCH_SEND_DATA_CNF;
                
                //不要回确认信息
                MyFree(pvPrmv);
                pvPrmv = NULL;
                break;

            case ACCH_MESS_SEND_REQ:
                //不要回确认信息且不需要发送，因为这不是最后一个AC消息，发送了也没有意义
                //等于则说明是最后一个
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
                //是最后一条则进行发送(同OHCH_MESS_SEND_REQ处理)
                g_ulACCHSendMessageNum = 0;
            case OHCH_MESS_SEND_REQ:
            case MESS_QUERY_REQ:
            case MESS_QUERY_RSP:
            case MESS_SET_REQ:
            case MESS_SET_RSP:
            case MESS_ALARM_REP:
                //写入消息数据
                ulSize = pstMessage->usDataLen + 8 ;
                memcpy(aucData,pstMessage,ulSize);

                //对消息数据进行主从协议的编码,以广播方式发送
                ulSize = EncodeMSProtocol(MS_BROADCAST_DEVNUM, MS_CMD_TAKEOHTER,ucFlag, 
                        aucData, ulSize);
                
                //发送数据
                SthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                //不要回确认信息
                MyFree(pvPrmv);
                pvPrmv = NULL;
                break;
                
            default:
                //对于无法确认的释放内存
                MyFree(pvPrmv);
                pvPrmv = NULL;
                break;               
            }//switch(pstOHCHDataReq->ulPrmvType)
            
            //回确认包
            if(pvPrmv != NULL)
            {
                if(OSQPost(g_pstOHQue, pvPrmv) == OS_Q_FULL)
                {
                    MyFree(pvPrmv);
                }
            }
            
        }//if(pstCOMMObj->ucInterfaceType == USART_SLAVE)        

        //目前南向接口无这种接口
        else if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)
        {
        }//else if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)


    }//if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
}

/*************************************************
  Function:     HndlCHDebSend
  Description:    本函数处理南向接口发送的数据
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为南向接口的通信对象
                     pvPrmv:              接收到的消息指针
  Output:     
  Return:         
  Others:       先判断通信对象再判断通信接口最后判断原语类型
*************************************************/
void HndlCHDebSend(COMM_OBJ_ST *pstCOMMObj, void * pvPrmv)
{
    OHCH_SEND_DATA_REQ_ST *pstOHCHDataReq = (OHCH_SEND_DATA_REQ_ST *)pvPrmv;   
    //判断何种通信对象
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {            
        //根据原语类型进行处理
        switch(*((ULONG*)pvPrmv))
        {
        //需要增加原语类型
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
        //不需要回确认包
        MyFree(pvPrmv);
    }//if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
}


/*************************************************
  Function:       HndlCHNthRec
  Description:    本函数处理北向接口收到的数据
                  处理后的数据上交至相应消息队列
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为北向接口的通信对象

  Output:     
  Return:         
  Others:
*************************************************/
void HndlCHNthRec(COMM_OBJ_ST *pstCOMMObj)
{
    UINT32  cpu_sr;
    ULONG ulCount = 0;
    UCHAR aucRec[USART_SIZE_BUFFER];//用于保存读到的数据
    UCHAR aucRecToMatch[USART_SIZE_BUFFER];//用于某种数据包匹配
    //判断何种通信对象
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {    
        //读北向串口
        if(UsartReadReady ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev)))
        {
            //启动接收超时定时器
            StartGeneralTimer(&g_stTimerNthRecTimeOut, TIME_TIMEOUT_USARTREC/TIMER_UNIT_MS, NULL,NONPERIODIC);

            //读出数据
            OS_ENTER_CRITICAL();
            ulCount = UsartRead ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev), aucRec);
            
            //test把北向串口收到的数据从调试串口中发出，为了观察modem所发出的数据
            //UsartWrite ((USART_DEV_ST*)(g_stDebugInterface.pvCOMMDev),
              //      aucRec, ulCount);
            
            OS_EXIT_CRITICAL();

            //根据接口类型对数据进行处理
            switch(pstCOMMObj->ucInterfaceType)
            {
            case USART_MODEM_OR_DIRECT:
                //匹配AT命令和直连数据
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
            //对于读来说以下两种情况处理方式相同
            case USART_SLAVE_485:
            case USART_SLAVE_OPT:
                //匹配主从协议            
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
  Description:    本函数处理调试接口收到的数据
                  处理后的数据上交至相应消息队列
  Calls:
  Called By:
  Input:          pstCOMMObj:         通信对象

  Output:     
  Return:         
  Others:
*************************************************/
void HndlCHDebRec(COMM_OBJ_ST *pstCOMMObj)
{
    UINT32  cpu_sr;
    ULONG ulCount = 0;
    UCHAR aucRec[USART_SIZE_BUFFER];//用于保存读到的数据
    UCHAR aucRecToMatch[USART_SIZE_BUFFER];//用于某种数据包匹配
    //判断何种通信对象
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {    
        if(UsartReadReady ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev)))
        {
            //启动接收超时定时器
            StartGeneralTimer(&g_stTimerDebRecTimeOut, TIME_TIMEOUT_USARTREC/TIMER_UNIT_MS, NULL,NONPERIODIC);

            //读出数据
            OS_ENTER_CRITICAL();
            ulCount = UsartRead ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev), aucRec);
            OS_EXIT_CRITICAL();
            
            //只匹配直连数据
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
  Description:  本函数判断北向接口是否空闲
  Calls:
  Called By:
  Input:        pstCOMMObj:         作为北向接口的通信对象

  Output:       TRUE:空闲FALSE:忙     
  Return:         
  Others:
*************************************************/
/*
BOOL CHNthSendReady(COMM_OBJ_ST *pstCOMMObj)
{
    //判断何种通信对象
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {    
        //判断北向串口是否还有数据未发送
        if(UsartSendReady ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev)))
        {
            //串口无数据发送则再根据接口类型做进一步判断
            switch(pstCOMMObj->ucInterfaceType)
            {
            //以下两种情况处理方式相同，等到轮询则可发送
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
            //如果采用关光端机使能方式1，则3种情况均相同
            case USART_SLAVE_OPT:
                //test串口无数据时，关闭光端机使能,关使能方式2
                //目前采用关使能方式1
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
  Description:    本函数对数据进行主从协议的编码
                  本函数在原有内存上进行编码
  Calls:
  Called By:
  Input:          ucSNum:         从机编号
                  ucCMD:          命令单元
                  ucFlag:         标志单元
                  pucData,ulSize: 要编码的数据指针和大小
                  ulSize为0则说明没有数据单元
                  pucData输入时是要参与编码的数据单元指针
                  pucData输出时是编码后的数据指针
  Output:     
  Return:         编码后的数据长度        
  Others:
*************************************************/
ULONG EncodeMSProtocol(UCHAR ucSNum,UCHAR ucCMD,UCHAR ucFlag,UCHAR *pucData, ULONG ulSize)
{
    USHORT usCrc;
    //判断是否有数据单元
    if(ulSize != 0)
    {
        //搬移数据单元，从最后进行处理
        for(ULONG i=ulSize; i>0 ;i--)
        {
            //前面留出6字节用于存放3个AA,协议类型,从机编号,命令编号,标志单元
            *(pucData + i - 1 + 7) = *(pucData + i - 1);
        }
    }
    ///3个AA,协议类型,从机编号,命令编号,标志单元
    pucData[0] = MS_START_END_FLAG;
    pucData[1] = MS_START_END_FLAG;
    pucData[2] = MS_START_END_FLAG;
    pucData[3] = MS_PROTOCOL_TYPE;
    pucData[4] = ucSNum;
    pucData[5] = ucCMD;
    pucData[6] = ucFlag;    
    
    //crc校验
    //从协议类型开始
    usCrc = CalcuCRC(pucData+3, ulSize+4);
    SET_WORD(pucData+ulSize+7, usCrc);

    //转义
    ulSize = EncodeEscape(pucData+3, ulSize+6, USART_SIZE_BUFFER, MS_ESCAPE_CHAR, 
            g_aucMSEscaped, g_aucMSUnEscape, 2);

    //最后1个AA
    pucData[ulSize+3] = MS_START_END_FLAG;
    ulSize = ulSize + 4;
    return ulSize;
}

/*************************************************
  Function:       ResetModem
  Description:    本函数重启modem
  Calls:
  Called By:
  Input:          
  Output:     
  Return:            
  Others:
*************************************************/
void ResetModem(void)
{
    //先拉高引脚再延迟再拉低管脚
    PioOpen(&MODEMRESET_PIO_CTRL,MODEM_RESET_PIN,PIO_OUTPUT);
    PioWrite(&MODEMRESET_PIO_CTRL,MODEM_RESET_PIN,1);
    MyDelay(50);
    PioWrite(&MODEMRESET_PIO_CTRL,MODEM_RESET_PIN,0);
}

/*************************************************
  Function:       SthSendtoOPTor485
  Description:    本函数向南向接口发送光端机数据
                  本函数在已确定通信接口的情况下被调用
                  即本函数向主设备的南向接口发送数据
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为南向接口的通信对象
                  pucData,ulSize:      要发送的数据指针和大小
  Output:     
  Return:         
  Others:
*************************************************/
void SthSendtoOPTor485(COMM_OBJ_ST *pstCOMMObj, UCHAR *pucData, ULONG ulSize)
{
    UINT32 cpu_sr;
    //UCHAR ucState;

    //等待南向接口闲,关使能方式1  
    //ucState = pstCOMMObj->ucState;//保存当前通信状态
    //pstCOMMObj->ucState = COMM_STATE_SEND_IDLE;
    //test南向控制消息处理也放在sendready后，所以不需此段函数
    //test同时也不需要保存当前通信状态
    /*
    while(CHSthSendReady(pstCOMMObj)!=TRUE)
    {
        MyDelay(10);
        //继续接收
        //test还要处理南向接口的接收和北向接口的接收发送
        HndlCHSthRec(pstCOMMObj);
    }
    */
    //如果是485，则要打开发送使能
    if(pstCOMMObj->ucInterfaceType == USART_SLAVE_485)
    {
        MyDelay(20);
        PioWrite(&U485_PIO_CTRL,((USART_DEV_ST*)(pstCOMMObj->pvCOMMDev))->uiU485Enable,
            U485_ENABLE_ON);
    }
    //发送数据
    OS_ENTER_CRITICAL();
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            pucData, ulSize);
    OS_EXIT_CRITICAL();
    //如果是485，则要关闭发送使能
    if(pstCOMMObj->ucInterfaceType == USART_SLAVE_485)
    {
        //notice等待发送完毕关闭发送使能
        UCHAR ucState = pstCOMMObj->ucState;//保存当前通信状态
        pstCOMMObj->ucState = COMM_STATE_SEND_IDLE;
        while(CHSendReady(pstCOMMObj)!=TRUE)
        {
            MyDelay(10);
            //继续接收
            //notice还要处理南向接口的接收和北向接口的接收
            HndlCHNthRec(pstCOMMObj);
            HndlCHSthRec(&g_stSouthInterface);
        }
        pstCOMMObj->ucState = ucState;
        MyDelay(20);

        //关闭485发送使能
        PioWrite(&U485_PIO_CTRL,((USART_DEV_ST*)(pstCOMMObj->pvCOMMDev))->uiU485Enable,
            U485_ENABLE_OFF);
    }
    //恢复当前通信状态
    //pstCOMMObj->ucState = ucState;     

}

/*************************************************
  Function:       NthSendtoOPTor485
  Description:    本函数向北向接口发送光端机数据
                  本函数在已确定通信接口的情况下被调用
                  即本函数向主设备的北向接口发送数据
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为北向接口的通信对象
                  pucData,ulSize:      要发送的数据指针和大小
  Output:     
  Return:         
  Others:
*************************************************/
void NthSendtoOPTor485(COMM_OBJ_ST *pstCOMMObj, UCHAR *pucData, ULONG ulSize)
{
    UINT32 cpu_sr;
    UCHAR ucState;

    //如果是485，则要打开发送使能
    if(pstCOMMObj->ucInterfaceType == USART_SLAVE_485)
    {
        //打开发送使能
        PioWrite(&U485_PIO_CTRL,((USART_DEV_ST*)(pstCOMMObj->pvCOMMDev))->uiU485Enable,
            U485_ENABLE_ON);
             
        //延时
        MyDelay(20);            

        OS_ENTER_CRITICAL();
        UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
                pucData, ulSize);
        OS_EXIT_CRITICAL();
           
        //notice等待发送完毕关闭发送使能
        ucState = pstCOMMObj->ucState;//保存当前通信状态
        pstCOMMObj->ucState = COMM_STATE_SEND_IDLE;
        while(CHSendReady(pstCOMMObj)!=TRUE)
        {
            MyDelay(10);
            //继续接收
            //notice还要处理南向接口的接收发送和北向接口的接收
            HndlCHNthRec(pstCOMMObj);
            HndlCHSthRec(&g_stSouthInterface);
            CHSthReadyAndSend();
        }
        pstCOMMObj->ucState = ucState;
        MyDelay(20);
    
        //关闭发送使能
        PioWrite(&U485_PIO_CTRL,((USART_DEV_ST*)(pstCOMMObj->pvCOMMDev))->uiU485Enable,
            U485_ENABLE_OFF);
    }

    //光端机
    else
    {
        //notice光端机打开使能
        PioWrite(&OPT_PIO_CTRL,OPT_ENABLE,OPT_ENABLE_ON);
             
        //延时
        MyDelay(40);
            
        //加入00后发送
        UCHAR ucTemp=0;
        OS_ENTER_CRITICAL();
        UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
                &ucTemp, 1);    
        UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
                pucData, ulSize);
        OS_EXIT_CRITICAL();
           
        //notice等待发送完毕光端机关闭使能 ,关使能方式1
        ucState = pstCOMMObj->ucState;//保存当前通信状态
        pstCOMMObj->ucState = COMM_STATE_SEND_IDLE;
        while(CHSendReady(pstCOMMObj)!=TRUE)
        {
            MyDelay(10);
            //继续接收
            //notice还要处理南向接口的接收发送和北向接口的发送
            HndlCHNthRec(pstCOMMObj);
            HndlCHSthRec(&g_stSouthInterface);
            CHSthReadyAndSend();
        }
        pstCOMMObj->ucState = ucState;
        MyDelay(10);
    
        //notice光端机关闭使能
        PioWrite(&OPT_PIO_CTRL,OPT_ENABLE,OPT_ENABLE_OFF);
    }
}

/*************************************************
  Function:       NthSendtoMorD
  Description:    本函数向北向接口或者调试接口发送MODEM或者直连数据
                  本函数在已确定通信接口的情况下被调用
                  即本函数向主设备的北向或者调试接口发送数据
                  调试接口只会发CSD数据
  Calls:
  Called By:
  Input:           pstCOMMObj:         作为通信接口的通信对象
                   pvPrmv:              接收到的消息指针
  Output:     
  Return:         
  Others:
*************************************************/
LONG NthSendtoMorD(COMM_OBJ_ST *pstCOMMObj, void * pvPrmv)
{
    OHCH_SEND_DATA_REQ_ST *pstOHCHDataReq = (OHCH_SEND_DATA_REQ_ST *)pvPrmv;
    UINT32  cpu_sr;
    LONG lSucceeded = FAILED;

    //notice通信状态为GPRS而要发送短消息
    //有两种处理方式:直接上报失败或者切换方式来发送 

    //要发送短消息
    if(pstOHCHDataReq->ucCommMode == COMM_MODE_SMS)       
    {
        //notice
        if(g_ucCurrCommMode != COMM_MODE_SMS)
        {
            return FAILED;
        }
    
        //封装AT命令后发送
        SendSMSInPDUMode(pstCOMMObj,pstOHCHDataReq->acDstTelNum, pstOHCHDataReq->pucData,
                pstOHCHDataReq->ulDataLen, pstOHCHDataReq->ucDCS, pstOHCHDataReq->ucDA,NULL);

        //判断结果
        if(AtRtnIsOk(pstCOMMObj, TIME_WAIT_SMSSEND, CH_SENDAT_NORMAL) == TRUE)
        {
            lSucceeded = SUCCEEDED;
        }
        else
        {
            lSucceeded = FAILED;
            //notic为了短信压力
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

    //要发送直连数据
    else if(pstOHCHDataReq->ucCommMode == COMM_MODE_CSD)       
    {
        //notice
        if(g_ucCurrCommMode == COMM_MODE_GPRS)
        {
            return FAILED;
        }
        //发送数据
        OS_ENTER_CRITICAL();
        UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
                pstOHCHDataReq->pucData, pstOHCHDataReq->ulDataLen);
        OS_EXIT_CRITICAL();
        lSucceeded = SUCCEEDED;
    }

    //要发送GPRS数据
    else if(pstOHCHDataReq->ucCommMode == COMM_MODE_GPRS)
    {
        //如果通信状态不符合
        if(g_ucCurrCommMode != COMM_MODE_GPRS)
        {
            lSucceeded = FAILED;
        }
        else
        {
            //转义处理要申请内存，不能破坏原数据
            //因为原数据要回给OH层
            UCHAR *pucMem;
            ULONG ulSize = pstOHCHDataReq->ulDataLen;
            pucMem = (UCHAR *) MyMalloc(g_pstMemPool512);
            if(pucMem == (void*)NULL)
            {
                lSucceeded = FAILED;
            }
            else
            {
                //搬移数据
                memcpy((UCHAR*)pucMem,pstOHCHDataReq->pucData,ulSize);
                //进行转义
                ulSize = EncodeEscape((UCHAR*)pucMem, ulSize, USART_SIZE_BUFFER, 
                        GPRS_ESCAPE_CHAR, g_aucGPRSEscaped, g_aucGPRSUnEscape, 2);
                //发送数据
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
  Description:    本函数处理北向接口发送的数据
                  目前只处理CH层发来的数据或者
                  南向接口转发的数据(等同于CH层发来的数据)

  Calls:
  Called By:
  Input:          pstCOMMObj:         作为北向接口的通信对象
                  pvPrmv:             接收到的消息指针
  Output:     
  Return:         
  Others:         先判断通信对象再判断通信接口最后判断原语类型
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
    //判断何种通信对象
    if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
    {
        //如果是从设备且连接光端机(判断通信接口)
        if(pstCOMMObj->ucInterfaceType == USART_SLAVE_OPT ||
            pstCOMMObj->ucInterfaceType == USART_SLAVE_485)    
        {
            UCHAR aucData[USART_SIZE_BUFFER];
            ULONG ulSize;
            
            //根据原语类型进行处理
            switch(*((ULONG*)pvPrmv))
            {
            //由于是从设备所以对以下原语类型处理一样
            //用主从协议打包后直接发送
            case OHCH_SEND_DATA_REQ:
            case OHCH_UNDE_SEND_REQ:
            case OHCH_DEBU_SEND_REQ:
                //写入消息数据
                memcpy(aucData,pstOHCHDataReq,sizeof(*pstOHCHDataReq));
                memcpy(aucData+sizeof(*pstOHCHDataReq), 
                        pstOHCHDataReq->pucData, pstOHCHDataReq->ulDataLen);
                ulSize = sizeof(*pstOHCHDataReq) + pstOHCHDataReq->ulDataLen;
                //对消息数据进行主从协议的编码
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_TAKEOHTER,0, 
                        aucData, ulSize);
                //发送数据
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                //notice 发送数据结束后再改变原语类型
                pstOHCHDataReq->ulPrmvType = OHCH_SEND_DATA_CNF;
                break;

            case ACCH_MESS_SEND_REQ:
                //不要回确认信息且不需要发送，因为这不是最后一个AC消息，发送了也没有意义
                //等于则说明是最后一个
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
                //是最后一条则进行发送(同OHCH_MESS_SEND_REQ处理)
                g_ulACCHSendMessageNum = 0;
            case OHCH_MESS_SEND_REQ:
                //写入消息数据
                ulSize = pstMessage->usDataLen + 8;                
                memcpy(aucData,pstMessage,ulSize);

                //对消息数据进行主从协议的编码
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_TAKEOHTER,0, 
                        aucData, ulSize);
                //发送数据
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                //不要回确认信息
                MyFree(pvPrmv);
                pvPrmv = NULL;
                break;

            case OHCH_CONN_CENTER_REQ:
                pstOHCHConnCenterReq->ulPrmvType = OHCH_CONN_CENTER_CNF;
                //从机不支持发送GPRS连接
                pstOHCHConnCenterReq->lResult = RESULT_STOP_REQ;
                //回普通应答
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_ANS,0, 
                        aucData, 0);
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;
            case OHCH_GPRS_DISCONN_REQ:
                pstOHCHGprsDisconnReq->ulPrmvType = OHCH_GPRS_DISCONN_CNF;
                //从机不支持发送GPRS连接
                pstOHCHGprsDisconnReq->lResult = RESULT_STOP_REQ;
                //回普通应答
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_ANS,0, 
                        aucData, 0);
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;
            case OHCH_SLAVE_UNREG_REQ:
                pstOHCHSlaveUnRegReq->ulPrmvType = OHCH_SLAVE_UNREG_CNF;
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_UNREG,0, 
                        aucData, 0);
                //无需原语直接发送数据
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;
            case OHCH_R_SMC_ADDR_REQ:
            case OHCH_W_SMC_ADDR_REQ:
                pstOHCHSMCAddrReq->ulPrmvType = OHCH_RW_SMC_ADDR_CNF;
                //从机不支持短信中心号码的处理
                memset(pstOHCHSMCAddrReq->acTelNum,0,MAX_TEL_NUM_LEN);
                pstOHCHSMCAddrReq->lResult = SUCCEEDED;
                //回普通应答
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_ANS,0, 
                        aucData, 0);
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;
            default:
                //对于无法确认的释放内存
                MyFree(pvPrmv);
                pvPrmv = NULL;
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_ANS,0, 
                        aucData, 0);
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;               
            }//switch(pstOHCHDataReq->ulPrmvType)
      
            //回确认包
            if(pvPrmv != NULL)
            {
                if(OSQPost(g_pstOHQue, pvPrmv) == OS_Q_FULL)
                {
                    MyFree(pvPrmv);
                }
            }
            //设北向接口忙，因为主从采用一问一答的方式
            pstCOMMObj->ucState = COMM_STATE_SEND_BUSY;            
        }//if(pstCOMMObj->ucInterfaceType == USART_SLAVE)        

        //notice移到最前
        else if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)
        {
            //设北向接口忙
            pstCOMMObj->ucState = COMM_STATE_SEND_BUSY;
            
            //根据原语类型进行处理
            switch(*((ULONG*)pvPrmv))
            {
            //无论从设备转发还是主设备主动发均回应答
            //需要增加原语类型
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

            //notice注意在已打开连接下收到连接消息
            //以及在已断开连接条件下收到断开连接消息
            case OHCH_CONN_CENTER_REQ:

                MY_LOG_MSG_BIN((LS_TRACE, LL_INFO, (UCHAR*)pvPrmv,sizeof(pstOHCHConnCenterReq)));
            
                pstOHCHConnCenterReq->ulPrmvType = OHCH_CONN_CENTER_CNF;            
                if(ConnectGPRSandCenter(pstCOMMObj,pstOHCHConnCenterReq->aucIpAddr,
                        pstOHCHConnCenterReq->usPortNum)==SUCCEEDED)
                {
                    //置全局通信方式
                    g_ucCurrCommMode = COMM_MODE_GPRS;
                    pstOHCHConnCenterReq->lResult= SUCCEEDED;
                }
                else
                {
                    //连接GPRS如果失败则要尝试断开
                    //这是为了防止未断开GPRS而以为断开了
                    DisConnGPRS(pstCOMMObj);
                    g_ucCurrCommMode = COMM_NO_CONN;                    
                    pstOHCHConnCenterReq->lResult= FAILED;                
                }
                //test测试GPRS连接和断开，周期连接和断开
                  //              StartGeneralTimer(&g_stTimerZoneInfo, 
                    //    TIME_GET_ZONEINFO/TIMER_UNIT_MS, NULL);
                        
                break;//case OHCH_GPRS_CONN_REQ

            case OHCH_GPRS_DISCONN_REQ:

                MY_LOG_MSG_BIN((LS_TRACE, LL_INFO, (UCHAR*)pvPrmv,sizeof(pstOHCHGprsDisconnReq)));
            
                pstOHCHGprsDisconnReq->ulPrmvType = OHCH_GPRS_DISCONN_CNF;            
                if(DisConnGPRS(pstCOMMObj)==SUCCEEDED)
                {
                    //置全局通信方式
                    g_ucCurrCommMode = COMM_NO_CONN;
                    pstOHCHGprsDisconnReq->lResult= SUCCEEDED;
                }
                else
                {
                    //断开连接失败也改变全局通信方式
                    g_ucCurrCommMode = COMM_NO_CONN;
                    pstOHCHGprsDisconnReq->lResult= FAILED;                
                }
                //test测试GPRS连接和断开，周期连接和断开
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
                //鉴权回应判断是否摘机
                if(pstOHCHCSDConnRsp->lResult== SUCCEEDED)
                {
                    SendStrToUsart(pstCOMMObj, g_aucSAT_ATA, 
                           sizeof(g_aucSAT_ATA)-1);
                    //判断结果,无论成功与失败均退出，因为继续拨号还会收到CRING信息
                    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATACSDCONNECT,CH_SENDAT_ATA)==TRUE)
                    {
                        //置全局通信方式
                        g_ucCurrCommMode = COMM_MODE_CSD;
                    }
                }
                //挂机
                else
                {
                    SendStrToUsart(pstCOMMObj, g_aucSAT_ATH, 
                        sizeof(g_aucSAT_ATH)-1);
                    //无论成功与否均不在处理，因为不摘机就不会进入CSD模式
                    g_ucCurrCommMode = COMM_NO_CONN;
                    AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL);
                }
                //无需回应至OH,所以释放内存
                MyFree(pvPrmv);
                pvPrmv = NULL;
                break;//case OHCH_CSD_CONN_RSP
                
            default:
                //对于无法确认的释放内存
                MyFree(pvPrmv);
                pvPrmv = NULL;
                break;
            }//switch(pstOHCHDataReq->ulPrmvType)
            //回确认包
            if(pvPrmv != NULL)
            {
                if(OSQPost(g_pstOHQue, pvPrmv) == OS_Q_FULL)
                {
                    MyFree(pvPrmv);
                }
            }
            
            //设北向接口闲
            pstCOMMObj->ucState = COMM_STATE_SEND_IDLE;
        }//else if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)

    }//if(pstCOMMObj->ucType == COMM_OBJ_TYPE_USART)
}

/*************************************************
  Function:     HndlCHSthCtrlMsg
  Description:  本函数处理南向接口收到的控制消息
                本函数可以用来处理突发的控制消息
  Calls:
  Called By:
  Input:        pstCOMMObj:         作为南向接口的通信对象
  Output:     
  Return:         
  Others:
*************************************************/
void HndlCHSthCtrlMsg(COMM_OBJ_ST *pstCOMMObj)
{
    void *pvPrmv = NULL;
    
    //notice需要判断通信对象类型

        //处理南向接口控制消息,无消息则返回
        pvPrmv = OSQAccept(g_pstCHSthCtrlQue, &g_ucErr);
        
        if(pvPrmv == (void*)NULL)
        {
            return;
        }
        //有消息则处理
        if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)
        {
            //notice目前处理:扔掉所有控制消息直到空为止
        }
        else if(pstCOMMObj->ucInterfaceType == USART_SLAVE_OPT ||
            pstCOMMObj->ucInterfaceType == USART_SLAVE_485)
        {
            UCHAR aucData[25];//主从协议编码需要内存
            ULONG ulSize;
            UCHAR ucFlag = 0;

            //写入标志位，目前含有通信状态信息
            //写入通信状态
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
                
                //说明有新的从机编号被分配但还没有收到应答
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
                //notice可以延时20ms后再轮询，用来给上一个从机关闭使能的时间
                MyDelay(20);
                
                SthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                break;
            case CH_MG_MS_REGANS:
                //置目前轮询的从机编号为新的设备编号
                g_stYkppParamSet.stYkppCommParam.ucAskingSNum = MS_NEWS_DEVNUM;
                //notice这里应增加数据单元的编码
                aucData[0] = 0x03;
                aucData[1] = 0x00;
                aucData[2] = 0x01;
                aucData[3] = 0x80;
                aucData[4] = GetNewSNum();
                ulSize = EncodeMSProtocol(MS_NEWS_DEVNUM, MS_CMD_REGANS,ucFlag, 
                        aucData, 5);
                SthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                //启动从机注册保护定时器
                StartGeneralTimer(&g_stTimerSlaveRegProtect, 
                        TIME_SLAVEREG_PROTECT/TIMER_UNIT_MS, NULL,NONPERIODIC);
                break;

            case CH_MG_MS_UNREGANS:
                UCHAR ucSNum;
                ucSNum = ((CH_CTR_ST*)pvPrmv)->ucParam1;
                //先发送应答
                ulSize = EncodeMSProtocol(ucSNum, MS_CMD_UNREGANS,ucFlag, 
                        aucData, 0);
                SthSendtoOPTor485(pstCOMMObj, aucData, ulSize);
                //删除从机编号
                MSChangeBit(&g_stYkppParamSet.stYkppCommParam.usSBitmap, ucSNum, 0);
                //置目前轮询的从机编号
                g_stYkppParamSet.stYkppCommParam.ucAskingSNum = ucSNum;
                //保存flash
                SaveYkppParam();
                break;
            default:
                break;
            }//switch(((CH_CTR_ST*)pvPrmv)->ulPrmvType)
            
            //启动轮询超时定时器
            StartGeneralTimer(&g_stTimerSthAskTimeOut, TIME_TIMEOUT_ASKSLAVE/TIMER_UNIT_MS, NULL,NONPERIODIC);
            
        }//else if(pstCOMMObj->ucInterfaceType == USART_SLAVE)

        MyFree(pvPrmv);  
}


/*************************************************
  Function:     HndlCHNthCtrlMsg
  Description:  本函数处理北向接口收到的控制消息
                本函数可以用来处理突发的控制消息
  Calls:
  Called By:
  Input:        pstCOMMObj:         作为北向接口的通信对象
  Output:     
  Return:         
  Others:
*************************************************/
void HndlCHNthCtrlMsg(COMM_OBJ_ST *pstCOMMObj)
{
    void *pvPrmv = NULL;

    //notice需要判断通信对象类型

        //处理北向接口控制消息,无消息则返回
        pvPrmv = OSQAccept(g_pstCHNthCtrlQue, &g_ucErr);
        
        if(pvPrmv == (void*)NULL)
        {
            return;
        }
        //有消息则处理
        if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)
        {
            OS_Q_DATA stOSQData;
            //目前处理:modem自发命令的回应
            //对于一问一答的回应均在AtRtnIsOk中处理完毕
            switch(((CH_CTR_ST*)pvPrmv)->ulPrmvType)
            {
            case CH_MG_AT_ATA:
                SendStrToUsart(pstCOMMObj, g_aucSAT_ATA, 
                       sizeof(g_aucSAT_ATA)-1);
                //判断结果,无论成功与失败均退出，因为继续拨号还会收到CRING信息
                if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATACSDCONNECT,CH_SENDAT_ATA)!=TRUE)
                {
                    break;
                }
                //置全局通信方式
                g_ucCurrCommMode = COMM_MODE_CSD;

                MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "CHTASK ATA"));
                
                break;
            case CH_MG_AT_ATH:
                SendStrToUsart(pstCOMMObj, g_aucSAT_ATH, 
                       sizeof(g_aucSAT_ATH)-1);
                OHCH_CSD_DISCONN_IND_ST *pstOHCHCSDDisConnInd;
                //申请内存
                pstOHCHCSDDisConnInd = (OHCH_CSD_DISCONN_IND_ST *) MyMalloc(g_pstMemPool16);
                if(pstOHCHCSDDisConnInd == (void*)NULL)
                {
                    break;
                }
                //通知OH已挂断                
                pstOHCHCSDDisConnInd->ulPrmvType = OHCH_CSD_DISCONN_IND;
                pstOHCHCSDDisConnInd->lResult = FAILED;
                if(OSQPost(g_pstOHQue, pstOHCHCSDDisConnInd) == OS_Q_FULL)
                {
                    MyFree(pstOHCHCSDDisConnInd);            
                }
                //因为只有收到NO CARRIER或者语音呼叫的情况下才会发ATH
                //所以无论成功与失败均退出且置通信状态为无CSD
                if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
                {
                    //notice置全局通信方式
                    g_ucCurrCommMode = COMM_NO_CONN;
                    break;
                }
                //置全局通信方式
                g_ucCurrCommMode = COMM_NO_CONN;

                MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "CHTASK ATH"));
                
                break;
            case CH_MG_AT_CSD_CON:
                ULONG ulPos1,ulPos2;
                OHCH_CSD_CONN_IND_ST *pstOHCHCSDConnInd;
                
                //提取两个"间的电话号码
                ulPos1 = FindChar(g_aucCLIPTel, '"', 1, 15);
                ulPos2 = FindChar(g_aucCLIPTel, '"', 2, 40);
                if((ulPos1 == FIND_NO_CHAR) || (ulPos2 == FIND_NO_CHAR))
                {
                    break;
                }
                //写入电话号码
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

                //最后通知OH
                //发送消息，消息队列满则丢弃数据包
                if(OSQPost(g_pstOHQue, pstOHCHCSDConnInd) == OS_Q_FULL)
                {
                    MyFree(pstOHCHCSDConnInd);            
                }
                break;
            case CH_MG_GET_ZONEINFO:
                //判断北向队列是否忙
                OSQQuery(g_pstCHNthQue, &stOSQData);
                //不忙且通信无连接的情况下查询小区信息
                if((stOSQData.OSNMsgs<1)&&(g_ucCurrCommMode == COMM_NO_CONN))
                {
                    SendStrToUsart(pstCOMMObj, g_aucSAT_CCED, 
                           sizeof(g_aucSAT_CCED)-1);
                    //判断结果,无论成功与失败均退出，均重新启动定时器
                    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_CCED)==TRUE)
                    {                    
                        //处理小区信息
                        GetZoneInfo();
                    }
                }
                break;
            case CH_MG_AT_GPRS_SOCKETCLOSE:
                OHCH_GPRS_DISCONN_IND_ST *pstOHCHGPRSDisconnInd;
                
                //改变当前通信方式
                g_ucCurrCommMode = COMM_NO_CONN;
                
                pstOHCHGPRSDisconnInd = (OHCH_GPRS_DISCONN_IND_ST *) MyMalloc(g_pstMemPool16);
                if(pstOHCHGPRSDisconnInd == (void*)NULL)
                {
                    break;
                }
                pstOHCHGPRSDisconnInd->ulPrmvType = OHCH_GPRS_DISCONN_IND;
                pstOHCHGPRSDisconnInd->lResult = SUCCEEDED;

                //最后通知OH
                //发送消息，消息队列满则丢弃数据包
                if(OSQPost(g_pstOHQue, pstOHCHGPRSDisconnInd) == OS_Q_FULL)
                {
                    MyFree(pstOHCHGPRSDisconnInd);            
                }

                MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "REMOTE DISCONN GPRS"));
                
                break;
                
            case CH_MG_RESET:
                //判断北向队列是否忙
                OSQQuery(g_pstCHNthQue, &stOSQData);
                //不忙且通信无连接的情况下重启
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
            UCHAR aucData[25];//主从协议编码需要内存
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
                //先发送确认消息
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_UNREGCONF,0, 
                        aucData, 0);
                NthSendtoOPTor485(pstCOMMObj, aucData, ulSize);                
                //不用再改变从机编号以防止再次被注册
                //g_stYkppParamSet.stYkppCommParam.ucSNum = MS_NEWS_DEVNUM;

                //最后通知OH
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
                //发送消息，消息队列满则丢弃数据包
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
  Description:    本函数被北向接口函数所调用
                  用于获得modem的回应和判断超时
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为北向接口的通信对象
                     ulWaitTime:      等待回应的时间，单位ms
                     ulWaitTime的值应该大于ulWaitTime
                     ulATType:  发送AT命令的类型
  Output:     
  Return:         TRUE:OK
  Others:
*************************************************/
LONG AtRtnIsOk(COMM_OBJ_ST *pstCOMMObj,ULONG ulWaitTime,ULONG ulATType)
{
    void *pvPrmv = NULL;

    //启动定时器    
    StartGeneralTimer(&g_stTimerATRtnTimeOut, 
            ulWaitTime/TIMER_UNIT_MS, NULL,NONPERIODIC);
    for(;;)
    {
        //处理北向接口接收数据
        //notice还要处理南向接口的发送接收和北向接口的接收
        HndlCHNthRec(pstCOMMObj);
        HndlCHSthRec(&g_stSouthInterface);
        CHSthReadyAndSend();

        //处理北向接口控制消息
        //notice控制队列是否也要放在通信对象结构体中
        pvPrmv = OSQAccept(g_pstCHNthCtrlQue, &g_ucErr);

        //无消息继续等待
        if(pvPrmv == (void*)NULL)
        {
            //MYDELAY来允许其它任务运行
            MyDelay(CH_DELAY_TIME);
            continue;
        }
        //有消息则处理
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
            //把不能辨认的控制消息放回消息队列，由控制消息处理函数来处理
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
  Description:    本函数被北向接口函数所调用
                       用于发送AT命令
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为北向接口的通信对象

  Output:     
  Return:        
  Others:
*************************************************/
void SendStrToUsart(COMM_OBJ_ST *pstCOMMObj,UCHAR pucData[],ULONG ulSize)
{
    UINT32 cpu_sr;    
    //发送数据
    OS_ENTER_CRITICAL();
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            pucData, ulSize);
    OS_EXIT_CRITICAL();
}

/*************************************************
  Function:       InitNth
  Description:    本函数初始化北向接口
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为北向接口的通信对象

  Output:     
  Return:  SUCCEEDED:成功 FAILED:失败        
  Others:
*************************************************/
LONG InitNth(COMM_OBJ_ST *pstCOMMObj)
{
    //需要初始化modem
    if(pstCOMMObj->ucInterfaceType == USART_MODEM_OR_DIRECT)
    {
        //首先断开GPRS连接，这样才能成功初始化modem
        MyDelay(10);        
        DisConnGPRS(pstCOMMObj);

        //断开数据拨号
        SendStrToUsart(pstCOMMObj, g_aucSAT_ATH, 
               sizeof(g_aucSAT_ATH)-1);
        //等待结果，无论结果如何都继续执行
        AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL);

        //AT初始化命令,去掉回显连续发3次
        SendStrToUsart(pstCOMMObj, g_aucSAT_ATE0, 
               sizeof(g_aucSAT_ATE0)-1);
        //判断结果
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

        //AT初始化命令,设置PDU模式
        SendStrToUsart(pstCOMMObj, g_aucSAT_CMGF0, 
               sizeof(g_aucSAT_CMGF0)-1);
        //判断结果
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }

        //AT初始化命令,设置新短消息直接发送
        SendStrToUsart(pstCOMMObj, g_aucSAT_CNMI2, 
               sizeof(g_aucSAT_CNMI2)-1);
        //判断结果
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }

        //noticeAT初始化命令
        SendStrToUsart(pstCOMMObj, g_aucSAT_CSMP, 
               sizeof(g_aucSAT_CSMP)-1);
        //判断结果
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }

        //来电显示
        SendStrToUsart(pstCOMMObj, g_aucSAT_CLIP, 
               sizeof(g_aucSAT_CLIP)-1);
        //判断结果
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }

        //显示来电类型
        SendStrToUsart(pstCOMMObj, g_aucSAT_CRC,
               sizeof(g_aucSAT_CRC)-1);
        //判断结果
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }
        
        //notice锁定频段

        //保存设置
        SendStrToUsart(pstCOMMObj, g_aucSAT_ATANDW,
               sizeof(g_aucSAT_ATANDW)-1);
        //判断结果
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }

        //保存设置
        SendStrToUsart(pstCOMMObj, g_aucSAT_CSAS,
               sizeof(g_aucSAT_CSAS)-1);
        //判断结果
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }
        
        //notice处理sim卡里的短信

        //读出所有短信
        SendStrToUsart(pstCOMMObj, g_aucSAT_CMGL,
               sizeof(g_aucSAT_CMGL)-1);
        //判断结果
        if(AtRtnIsOk(pstCOMMObj,60000,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }
        //避免初始化时频繁删除短消息
        if(g_ucCurrSimSMSSta == SIM_STA_READED)
        {
            //删除所有短信
            SendStrToUsart(pstCOMMObj, g_aucSAT_CMGD,
                   sizeof(g_aucSAT_CMGD)-1);
            //判断结果
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
    //无modem且为从机直接返回成功，并置接口忙
    else if(pstCOMMObj->ucInterfaceType == USART_SLAVE_485 ||
            pstCOMMObj->ucInterfaceType == USART_SLAVE_OPT)
    {
        pstCOMMObj->ucState = COMM_STATE_SEND_BUSY;
        return SUCCEEDED;
    }    
    //其他则直接返回成功
    else
    {
        return SUCCEEDED;
    }
}

/*************************************************
  Function:       InitSth
  Description:    本函数初始化南向接口
                  主要是启动轮询操作
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为南向接口的通信对象

  Output:     
  Return:  SUCCEEDED:成功 FAILED:失败        
  Others:
*************************************************/
LONG InitSth(COMM_OBJ_ST *pstCOMMObj)
{
    if(pstCOMMObj->ucInterfaceType == USART_SLAVE_485 ||
            pstCOMMObj->ucInterfaceType == USART_SLAVE_OPT)
    {
        pstCOMMObj->ucState = COMM_STATE_SEND_IDLE;
        //notice启动轮询超时定时器
        g_stYkppParamSet.stYkppCommParam.ucAskingSNum = 0;
        //500ms后开始轮询
        StartGeneralTimer(&g_stTimerSthAskTimeOut, 500/TIMER_UNIT_MS, NULL,NONPERIODIC);
    }
    return SUCCEEDED;
}

/*************************************************
  Function:       GetZoneInfoPartValue
  Description:    本函数从2个逗号间获得数据，如果两个逗号间无数据则返回0
                  两个逗号间不能超过8字节，否则返回失败
                  本函数只被GetZoneInfo调用
                  本函数不破坏pucData指向的数据
  Calls:
  Called By:
  Input:          pucData:指向数据的指针
                  ulFirstNum:存放数据前的逗号是第几个逗号
                  pucValue:存放得到的数据值
                  ucISHex:1:转换为16进制数；0:转换为10进制数
  Output:     
  Return:  FAILED:失败        SUCCEEDED:成功
  Others:
*************************************************/
LONG GetZoneInfoPartValue(UCHAR* pucData,ULONG ulFirstNum,ULONG *pucValue,UCHAR ucISHex)
{
    ULONG ulPos1,ulPos2,ulSize;
    UCHAR aucTemp[8];
    //提取两个逗号间的数据
    ulPos1 = FindChar(pucData, ',', ulFirstNum, DATAPACK_ATFULL_DATA1_SIZE);
    ulPos2 = FindChar(pucData, ',', ulFirstNum+1, DATAPACK_ATFULL_DATA1_SIZE);
    ulSize = ulPos2 - ulPos1 - 1;

    if((ulPos2 == FIND_NO_CHAR)||(ulSize>8))
    {
        return FAILED;
    }
    
    (*pucValue) = 0;
    
    //两个逗号不是连在一起的则进行处理
    if(ulSize != 0)
    {
        //为奇数则在最高位添加0
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
        //把二字节合并后的数据转换为ULONG数据
        //CCED数据高位在前，低位在后
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
  Description:    本函数从全局变量中直接获取小区信息
                  并把小区信息放入CMCC中定义的存储区
  Calls:
  Called By:
  Input:                            
  Output:     
  Return:  SUCCEEDED:成功 FAILED:失败        
  Others:
*************************************************/
LONG GetZoneInfo(void)
{
    ULONG ulTemp;
    //第1，2逗号之间为运营商代码
    if(GetZoneInfoPartValue(g_aucZoneInfo, 1, &ulTemp, 0) != SUCCEEDED)
    {
        return FAILED;
    }
    //写入相应位置
    g_stRcParamSet.ucSsMnc = (UCHAR)ulTemp;

    //第2，3逗号之间为位置区编码(16进制)
    if(GetZoneInfoPartValue(g_aucZoneInfo, 2, &ulTemp, 1) != SUCCEEDED)
    {
        return FAILED;
    }
    g_stRcParamSet.usSsLac= (USHORT)ulTemp;

    //第3，4逗号之间为小区识别码(16进制)
    if(GetZoneInfoPartValue(g_aucZoneInfo, 3, &ulTemp, 1) != SUCCEEDED)
    {
        return FAILED;
    }
    g_stRcParamSet.usSsCi= (USHORT)ulTemp;

    //第4，5逗号之间为基站识别码
    if(GetZoneInfoPartValue(g_aucZoneInfo, 4, &ulTemp, 0) != SUCCEEDED)
    {
        return FAILED;
    }
    g_stRcParamSet.ucSsBsic = (UCHAR)ulTemp;

    //第5，6逗号之间为BCCH载频号
    if(GetZoneInfoPartValue(g_aucZoneInfo, 5, &ulTemp, 0) != SUCCEEDED)
    {
        return FAILED;
    }
    g_stRcParamSet.usSsBcch = (USHORT)ulTemp;

    //第6，7逗号之间为BCCH接收电平
    if(GetZoneInfoPartValue(g_aucZoneInfo, 6, &ulTemp, 0) != SUCCEEDED)
    {
        return FAILED;
    }
    //notice需要按照公式重新计算
    g_stRcParamSet.cSsBcchRxLev = (CHAR)ulTemp;    

    return SUCCEEDED;
}
/*************************************************
  Function:       GetSMCAddr
  Description:    本函数获得短信中心号码
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为北向接口的通信对象
                  pcTelNum:           存放电话号码的指针

  Output:     
  Return:  SUCCEEDED:成功 FAILED:失败        
  Others:
*************************************************/
LONG GetSMCAddr(COMM_OBJ_ST *pstCOMMObj,CHAR *pcTelNum)
{
    ULONG ulPos1,ulPos2;
    SendStrToUsart(pstCOMMObj, g_aucSAT_CSCAQ, 
           sizeof(g_aucSAT_CSCAQ)-1);
    memset(pcTelNum,0,MAX_TEL_NUM_LEN+1);
    //判断结果test时间
    //MCM-53_20070111_Zhonghw_begin
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCMD,CH_SENDAT_CSCA)!=TRUE)
    //MCM-53_20070111_Zhonghw_end
    {
        return FAILED;
    }
    //提取两个"间的电话号码
    ulPos1 = FindChar(g_aucSMCAddr, '"', 1, 15);
    ulPos2 = FindChar(g_aucSMCAddr, '"', 2, 40);
    if((ulPos1 == FIND_NO_CHAR) || (ulPos2 == FIND_NO_CHAR))
    {
        return FAILED;
    }
    //写入电话号码
    memcpy(pcTelNum,g_aucSMCAddr+ulPos1+1,
            ulPos2-ulPos1-1);
    return SUCCEEDED;
}

/*************************************************
  Function:       SetSMCAddr
  Description:    本函数设置短信中心号码
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为北向接口的通信对象
                  pcTelNum:           存放电话号码的指针

  Output:     
  Return:  SUCCEEDED:成功 FAILED:失败        
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
    //写入电话号码
    UsartWrite ((USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            (UCHAR*)pcTelNum, ulSize);
    //写入0D
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
  Description:    本函数设置IP地址和端口号
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为北向接口的通信对象

  Output:     
  Return:  SUCCEEDED:成功 FAILED:失败        
  Others:
*************************************************/
LONG SetIPandPort(COMM_OBJ_ST *pstCOMMObj,UCHAR aucIpAddr[],USHORT usPortNum)
{
    UCHAR aucData[35];
    ULONG cpu_sr,ulSize,ulCurrPos=0;
    
    //写入AT#TCPSERV=
    memcpy(aucData,g_aucSAT_GPRSIPSET,sizeof(g_aucSAT_GPRSIPSET)-1);
    ulCurrPos = ulCurrPos + sizeof(g_aucSAT_GPRSIPSET)-1;
    //写入"ipaddr"
    *(aucData + ulCurrPos) = '"';
    ulCurrPos++;
    for(ULONG i=0;i<4;i++)
    {
        ulSize = ULONGToStr(aucData+ulCurrPos, (ULONG)aucIpAddr[i]);
        ulCurrPos = ulCurrPos + ulSize;
        *(aucData + ulCurrPos) = '.';
        ulCurrPos++;
    }
    //把最后一个点改为分号
    *(aucData + ulCurrPos - 1) = '"';
    //写入0D
    *(aucData + ulCurrPos) = 0x0D;
    ulCurrPos++;
    ulSize = ulCurrPos;    
    //设置IP
    OS_ENTER_CRITICAL();
    UsartWrite ( (USART_DEV_ST*)(pstCOMMObj->pvCOMMDev),
            aucData, ulSize);
    OS_EXIT_CRITICAL();
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCONGPRS,CH_SENDAT_NORMAL)!=TRUE)
    {
        return FAILED;
    }

    ulCurrPos = 0;
    //写入AT#TCPPORT=
    memcpy(aucData,g_aucSAT_GPRSPORTSET,sizeof(g_aucSAT_GPRSPORTSET)-1);
    ulCurrPos = ulCurrPos + sizeof(g_aucSAT_GPRSPORTSET)-1;
    //写入PORT
    ulSize = ULONGToStr(aucData+ulCurrPos, (ULONG)usPortNum);
    ulCurrPos = ulCurrPos + ulSize;
    //写入0D
    *(aucData + ulCurrPos) = 0x0D;
    ulCurrPos++;
    ulSize = ulCurrPos;
    //设置IP
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
  Description:    本函数连接GPRS后根据IP地址和端口号再连接中心
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为北向接口的通信对象

  Output:     
  Return:  SUCCEEDED:成功 FAILED:失败        
  Others:
*************************************************/
LONG ConnectGPRSandCenter(COMM_OBJ_ST *pstCOMMObj,UCHAR aucIpAddr[],USHORT usPortNum)
{

    MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "CHTASK CONNECTING GPRS"));

    //notice可以考虑连接之前断开连接，即发送03

    //注册网络
    SendStrToUsart(pstCOMMObj, g_aucSAT_GPRSCGATT, 
           sizeof(g_aucSAT_GPRSCGATT)-1);    
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCONGPRS,CH_SENDAT_NORMAL)!=TRUE)
    {
        //失败有可能是收到"<"，所以不返回，继续连接网络
        //return FAILED;
    }
    //连接网络
    SendStrToUsart(pstCOMMObj, g_aucSAT_GPRSCONN, 
           sizeof(g_aucSAT_GPRSCONN)-1);    
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCONGPRS,CH_SENDAT_GPRSCONN)!=TRUE)
    {
        return FAILED;
    }

    //设置IP和PORT
    if(SetIPandPort(pstCOMMObj, aucIpAddr, usPortNum) != SUCCEEDED)
    {
        return FAILED;
    }

    //准备收发数据网络  
    SendStrToUsart(pstCOMMObj, g_aucSAT_GPRSOTCP, 
           sizeof(g_aucSAT_GPRSOTCP)-1);    
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATCONGPRS,CH_SENDAT_GPRSOTCP)!=TRUE)
    {
        //此时失败但可能已经连上，所以要确认是否连上
        //发送+++失败则说明未连上        
        SendStrToUsart(pstCOMMObj, g_aucSAT_ATOFFLINE, 
            sizeof(g_aucSAT_ATOFFLINE)-1);    
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATDISCONGPRS,CH_SENDAT_NORMAL)!=TRUE)
        {
            return FAILED;
        }
        //发送+++成功则说明连上，并要保证转换回数据模式:连续发10次
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
            //如果连续发送10次还不成功，则断开连接
            DisConnGPRS(pstCOMMObj);
            return FAILED;
        }
    }

    MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "CHTASK CONNECT GPRS SUCCEED"));

    return SUCCEEDED;
    
}

/*************************************************
  Function:     DisConnGPRS
  Description:    本函数断开GPRS连接
  Calls:
  Called By:
  Input:          pstCOMMObj:         作为北向接口的通信对象

  Output:     
  Return:  SUCCEEDED:成功 FAILED:失败        
  Others:
*************************************************/
LONG DisConnGPRS(COMM_OBJ_ST *pstCOMMObj)
{

    MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "CHTASK DISCONN GPRS"));

    MyDelay(500);//延时500ms，防止前面有数据会干扰
    //首先直接发送0x03来断开连接
    SendStrToUsart(pstCOMMObj, g_aucSAT_GPRSSOCKETCLOSE, 
           sizeof(g_aucSAT_GPRSSOCKETCLOSE)-1);    
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATDISCONGPRS,CH_SENDAT_SOCKETCLOSE) == TRUE)
    {
        return SUCCEEDED;
    }
        
    //切换到OFFLINE模式
    SendStrToUsart(pstCOMMObj, g_aucSAT_ATOFFLINE, 
           sizeof(g_aucSAT_ATOFFLINE)-1);    
    if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATDISCONGPRS,CH_SENDAT_NORMAL)!=TRUE)
    {
        //断开网络,防止目前已经是OFFLINE模式
        SendStrToUsart(pstCOMMObj, g_aucSAT_GPRSDISCONN, 
               sizeof(g_aucSAT_GPRSDISCONN)-1);    
        if(AtRtnIsOk(pstCOMMObj,TIME_WAIT_ATDISCONGPRS,CH_SENDAT_NORMAL)==TRUE)
        {
            return SUCCEEDED;
        }
    }
    //断开网络
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
  Description:    本函数等待南向接口准备好后处理发送数据
  Calls:
  Called By:
  Input:          
  Output:     
  Return:         
  Others:       先判断通信接口闲后在处理发送数据
*************************************************/
void CHSthReadyAndSend(void)
{
    void *pvPrmv = NULL;
    if(CHSendReady(&g_stSouthInterface) == TRUE)
    {
        //读出消息
        pvPrmv = OSQAccept(g_pstCHSthQue, &g_ucErr);
        if(pvPrmv != (void*)NULL)
        {
            HndlCHSthSend(&g_stSouthInterface, pvPrmv);
            //无论发送是否成功,原数据均返回给OH,故无需释放内存
            //MyFree(pvPrmv);
        }
        //没有消息则判断接口
        //notice如果需要轮询则处理控制消息
        else if(g_stSouthInterface.ucInterfaceType == USART_SLAVE_OPT ||
            g_stSouthInterface.ucInterfaceType == USART_SLAVE_485)
        {
            //处理南向接口控制消息
            HndlCHSthCtrlMsg(&g_stSouthInterface);
        }            
    }
}

/*************************************************
  Function:     HndlSimSMS
  Description:  本函数处理sim卡里的短消息                
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
    //北向接口空闲的时候再处理sim卡
    if(CHSendReady(pstCOMMObj) != TRUE)
    {
        return;
    }

    //判断北向队列是否忙
    OSQQuery(g_pstCHNthQue, &stOSQData);
    //不忙且通信无连接的情况下处理sim卡
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
            //读出所有短信
            SendStrToUsart(pstCOMMObj, g_aucSAT_CMGL,
                sizeof(g_aucSAT_CMGL)-1);
            //判断结果
            if(AtRtnIsOk(pstCOMMObj,60000,CH_SENDAT_NORMAL) == TRUE)
            {
                //删除所有短信
                SendStrToUsart(pstCOMMObj, g_aucSAT_CMGD,
                    sizeof(g_aucSAT_CMGD)-1);
                //判断结果
                if(AtRtnIsOk(pstCOMMObj,8000,CH_SENDAT_NORMAL) == TRUE)
                {
                    //改变短消息状态
                    g_ucCurrSimSMSSta = SIM_STA_NORMAL;
                }
                //删除短消息失败
                else
                {
                    //改变短消息状态
                    g_ucCurrSimSMSSta = SIM_STA_READED;
                }
            }
            //读所有短信失败则保持原状态，但此时已成功变成CNMI=22000，所以不用担心有短信会漏掉
            else
            {
                //do nothing
            }
        }
    }
    else if(g_ucCurrSimSMSSta == SIM_STA_READED)
    {
        //删除所有短信
        SendStrToUsart(pstCOMMObj, g_aucSAT_CMGD,
            sizeof(g_aucSAT_CMGD)-1);
        //判断结果
        if(AtRtnIsOk(pstCOMMObj,8000,CH_SENDAT_NORMAL) == TRUE)
        {
            //改变短消息状态
            g_ucCurrSimSMSSta = SIM_STA_NORMAL;
        }
        //删除短消息失败
        else
        {
            //改变短消息状态
            g_ucCurrSimSMSSta = SIM_STA_READED;
        }
    }
}

void CHTask(void *pv)
{
    void *pvPrmv = NULL;

    //任务初始化

    //APAC数据包初始化,首尾单字符(部分不初始化)
    UCHAR aucData1[DATAPACK_CMCC_APAC_SIZE];//分配内存至APAC数据包
    g_stDataPackCMCCAPAC.ulDataPackMaxSize = DATAPACK_CMCC_APAC_SIZE;
    g_stDataPackCMCCAPAC.pucData = aucData1;
    g_stDataPackCMCCAPAC.ucState = DATAPACK_STATE_NO;
    g_stDataPackCMCCAPAC.pucInPos = g_stDataPackCMCCAPAC.pucData;

    //APB数据包初始化,首尾单字符(部分不初始化)
    UCHAR aucData2[DATAPACK_CMCC_APB_SIZE];//分配内存至APB数据包
    g_stDataPackCMCCAPB.ulDataPackMaxSize = DATAPACK_CMCC_APB_SIZE;
    g_stDataPackCMCCAPB.pucData = aucData2;
    g_stDataPackCMCCAPB.ucState = DATAPACK_STATE_NO;
    g_stDataPackCMCCAPB.pucInPos = g_stDataPackCMCCAPB.pucData;

    //AT命令数据包初始化,首尾双字符(部分不初始化)
    UCHAR aucData3[DATAPACK_ATCMD_SIZE];//分配内存至ATCMD数据包
    g_stDataPackAT.ulDataPackMaxSize = DATAPACK_ATCMD_SIZE;
    g_stDataPackAT.pucData = aucData3;
    g_stDataPackAT.ucState = DATAPACK_STATE_NO;
    g_stDataPackAT.pucInPos = g_stDataPackAT.pucData;

    //AT完整数据包初始化
    UCHAR aucData4[DATAPACK_ATFULL_DATA1_SIZE];//分配内存至ATFULL数据包1
    UCHAR aucData5[256];//分配内存至ATFULL数据包2
    g_stDataPackATFull.ulDataPack1MaxSize= DATAPACK_ATFULL_DATA1_SIZE;
    g_stDataPackATFull.pucData1 = aucData4;
    g_stDataPackATFull.ulDataPack2MaxSize= 256;
    g_stDataPackATFull.pucData2 = aucData5;    
    g_stDataPackAT.ucState = DATAPACK_STATE_NO;

    //厂家协议数据包初始化
    UCHAR aucData6[DATAPACK_PRIVATE_SIZE];//分配内存至厂家协议数据包
    g_stDataPackPrivate.ulDataPackMaxSize = DATAPACK_PRIVATE_SIZE;
    g_stDataPackPrivate.pucData = aucData6;
    g_stDataPackPrivate.ucState = DATAPACK_STATE_NO;
    g_stDataPackPrivate.pucInPos = g_stDataPackPrivate.pucData;

    //北向主从协议数据包初始化
    UCHAR aucData7[DATAPACK_MS_SIZE];//分配内存
    g_stDataPackNthMS.ulDataPackMaxSize = DATAPACK_MS_SIZE;
    g_stDataPackNthMS.pucData = aucData7;
    g_stDataPackNthMS.ucState = DATAPACK_STATE_NO;
    g_stDataPackNthMS.pucInPos = g_stDataPackNthMS.pucData;

    //南向主从协议数据包初始化
    UCHAR aucData8[DATAPACK_MS_SIZE];//分配内存
    g_stDataPackSthMS.ulDataPackMaxSize = DATAPACK_MS_SIZE;
    g_stDataPackSthMS.pucData = aucData8;
    g_stDataPackSthMS.ucState = DATAPACK_STATE_NO;
    g_stDataPackSthMS.pucInPos = g_stDataPackSthMS.pucData;

    //调试厂家协议数据包初始化
    UCHAR aucData9[DATAPACK_PRIVATE_SIZE];//分配内存至厂家协议数据包
    g_stDataPackDebPrivate.ulDataPackMaxSize = DATAPACK_PRIVATE_SIZE;
    g_stDataPackDebPrivate.pucData = aucData9;
    g_stDataPackDebPrivate.ucState = DATAPACK_STATE_NO;
    g_stDataPackDebPrivate.pucInPos = g_stDataPackPrivate.pucData;

    //调试APAC协议数据包初始化
    UCHAR aucData10[DATAPACK_PRIVATE_SIZE];//分配内存至厂家协议数据包
    g_stDataPackDebAPAC.ulDataPackMaxSize = DATAPACK_CMCC_APAC_SIZE;
    g_stDataPackDebAPAC.pucData = aucData10;
    g_stDataPackDebAPAC.ucState = DATAPACK_STATE_NO;
    g_stDataPackDebAPAC.pucInPos = g_stDataPackPrivate.pucData;
    
    //test
    /*
    g_stNorthInterface.pvCOMMDev = &g_stUsart0;//232(调试时使用该串口)
    g_stSouthInterface.pvCOMMDev = &g_stUsart2;//485
    g_stDebugInterface.pvCOMMDev = &g_stUsart1;//232(暂不使用调试串口)
    g_stNorthInterface.ucInterfaceType = USART_SLAVE_OPT;
    g_stSouthInterface.ucInterfaceType = USART_SLAVE_485;
    g_stDebugInterface.ucInterfaceType = USART_MODEM_OR_DIRECT;
    
    g_stNmParamSet.ucDevNum = 0;              //设备编号
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
   
        
    //初始化北向接口
    InitNth(&g_stNorthInterface);

    //初始化南向接口
    //notice需要等北向初始化完毕后，再初始化南向
    InitSth(&g_stSouthInterface);
    
    //启动查询小区信息定时器和重启定时器
    if(g_stNorthInterface.ucInterfaceType == USART_MODEM_OR_DIRECT)
    {
        StartGeneralTimer(&g_stTimerZoneInfo, TIME_GET_ZONEINFO/TIMER_UNIT_MS, NULL,PERIODIC);
        StartGeneralTimer(&g_stTimerReset, TIME_TIMEOUT_RESET/TIMER_UNIT_MS, NULL,PERIODIC);
    }

    //无限循环
    while(1)
    {
        //MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "CHTASK START RUN"));
        //MY_LOG_MSG_BIN((LS_TRACE, LL_INFO, (UCHAR*)&g_ulTest,4));
        //g_ulTest++;
        //MY_TRACE((LS_TRACE, LL_INFO, MID_CH, 0xfe));
        
        //处理北向接口接收数据
        HndlCHNthRec(&g_stNorthInterface);
        
        //处理南向接口接收数据
        HndlCHSthRec(&g_stSouthInterface);
        
        //处理调试接口接收数据
        HndlCHDebRec(&g_stDebugInterface);


        //处理北向接口控制消息
        HndlCHNthCtrlMsg(&g_stNorthInterface);

        //处理sim卡上短消息
        HndlSimSMS();

        //处理调试接口控制消息


        //处理北向接口发送数据
        if(CHSendReady(&g_stNorthInterface) == TRUE)
        {
            //读出消息
            pvPrmv = OSQAccept(g_pstCHNthQue, &g_ucErr);
            if(pvPrmv != (void*)NULL)
            {
                HndlCHNthSend(&g_stNorthInterface, pvPrmv);
                //无论发送是否成功,原数据均返回给OH,故无需释放内存
                //MyFree(pvPrmv);
            }
            //没有消息从机要发轮询回应
            else if(g_stNorthInterface.ucInterfaceType == USART_SLAVE_OPT ||
                g_stNorthInterface.ucInterfaceType == USART_SLAVE_485)
            {
                UCHAR aucData[20];
                ULONG ulSize;
                ulSize = EncodeMSProtocol(g_stYkppParamSet.stYkppCommParam.ucSNum, SM_CMD_ANS,0, 
                        aucData, 0);
                NthSendtoOPTor485(&g_stNorthInterface, aucData, ulSize);
                //回应轮询包后要置通信接口忙
                g_stNorthInterface.ucState = COMM_STATE_SEND_BUSY;
            }            
        }

        //处理南向接口发送数据
        CHSthReadyAndSend();
        
        //处理调试接口发送数据
        //读出消息
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
  Description:   本函数设置短消息中地址类型单元的值，如果是被动应答的包，
                      地址类型采用短信解码时保存下来的地址值，并且不对电话号码
                      进行编辑。如果是主动发起的包，则根据以下组合对PDU中电话号码
                      相关参数进行配置
                      1）0x91 ＋ "86" ＋ 手机号码
                      2）0xA1 ＋ SP号码
  Calls:
  Called By:
  Input:          pcTelNum:发送号码
                     aucPDUData:短消息PDU包
                     ulCurrIndex:指向aucPDUData当前编码单元
                     ucDA:短消息地址类型
  Output:     
  Return:         
  Others:
*************************************************/
void SetDA(CHAR *pcTelNum, UCHAR aucPDUData[], ULONG &ulCurrIndex,UCHAR ucDA)
{
    UCHAR ucDALen;
    UCHAR i , j ;
    UCHAR ucCompLen;
    
    //写入DALEN
    ucDALen = (UCHAR)FindChar((UCHAR*)pcTelNum, 0, 1, MAX_TEL_NUM_LEN+1);

    //防止电话号码有问题
    if(ucDALen == FIND_NO_CHAR)
    {
        ucDALen = MAX_TEL_NUM_LEN;
    }

    //防止出现+
    if(pcTelNum[0] == '+')
    {
        for(i = 0; i < ucDALen-1 ; i++)
        {
            pcTelNum[i] = pcTelNum[i+1];
        }
        ucDALen--;
        pcTelNum[ucDALen] = 0x00;
    }

    if((ucDA == 0xA1)||(ucDA == 0x91))//被动应答的包
    {
        aucPDUData[ulCurrIndex] = ucDALen;
        ulCurrIndex++;
        
        aucPDUData[ulCurrIndex] = ucDA;
    }
    else//主动发起的包
    {
        for(i = 0 ; i < MAX_SMS_ADDR_PREFIX_NUM ; i++)
        {
            ucCompLen = g_stYkppParamSet.stYkppSmsAddrParam.astSmsAddrPrefix[i].ucLen;
            if((ucCompLen == 0)||(ucCompLen >ucDALen))//小于前缀的长度
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
    
            if(j >= ucCompLen)//匹配成功
            {
                 //在号码前面增加"86"
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

        //写入TypeOfAddr，如果前缀是86则是国际号码，否则为国内号码
        if((pcTelNum[0] == '8') && (pcTelNum[1] == '6'))
        {
            aucPDUData[ulCurrIndex] = 0x91;//91表示国际号码
        }
        else
        {
            aucPDUData[ulCurrIndex] = 0xA1;//A1表示国内号码
        }
    }
    ulCurrIndex++;

    //写入电话号码
    ucDALen = WriteTelNumToPDU(aucPDUData+ulCurrIndex, ucDALen, pcTelNum);
    ulCurrIndex = ulCurrIndex + ucDALen;
}
