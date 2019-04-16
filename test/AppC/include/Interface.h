/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   Interface.h
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
              将读取短信中心号码的原语结构独立出来增加消息序列号，
              用于操作返回时作为查找上下文作参考。
    3. 日期:  2006/11/07
       作者:  林玮
       描述:  修改问题MCM-20，
              增加按键触发上报功能
---------------------------------------------------------------------------*/
#ifndef InterfaceH
#define InterfaceH

#include "UserType.h"

//原语类型定义
#define OHCH_SEND_DATA_REQ          0//OH请求CH发送数据或CH请求OH转发数据（目前只有南向CH会要求转发）
#define OHCH_SEND_DATA_CNF          1
#define OHCH_RECV_DATA_IND          2
#define OHCH_R_SMC_ADDR_REQ         3
#define OHCH_W_SMC_ADDR_REQ         4
#define OHCH_RW_SMC_ADDR_CNF        5

//#define OHCH_GPRS_CONN_REQ          0x10
//#define OHCH_GPRS_CONN_CNF          0x11
#define OHCH_GPRS_DISCONN_REQ       0x12
#define OHCH_GPRS_DISCONN_CNF       0x13
#define OHCH_GPRS_DISCONN_IND       0x14
#define OHCH_SLAVE_UNREG_REQ        0x15
#define OHCH_SLAVE_UNREG_CNF        0x16
#define OHCH_CSD_CONN_IND           0x17 //CH请求鉴权
#define OHCH_CSD_CONN_RSP           0x18 //OH对鉴权的回应
#define OHCH_CSD_DISCONN_IND        0x19 //CH通知断线
#define OHCH_CONN_CENTER_REQ        0x1A //不仅建立GPRS连接，还要建立TCP连接
#define OHCH_CONN_CENTER_CNF        0x1B

#define MESS_QUERY_REQ              0x00010001 //查询，设备到器件
#define MESS_QUERY_RSP              0x00010002 //查询应答，器件到设备
#define MESS_SET_REQ                0x00010003 //设置，设备到器件
#define MESS_SET_RSP                0x00010004 //设置应答，器件到设备
#define MESS_ALARM_REP              0x00010005 //主动上报，器件到设备

#define ACCH_MESS_SEND_REQ          0x1C //AC请求CH发送消息，目前只有射频信息
#define ACCH_MESS_RECV_IND          0x1D //CH收到消息后通知AC，目前只有射频信息
#define OHCH_MESS_SEND_REQ          0x1E //OH请求CH发送消息
#define OHCH_MESS_RECV_IND          0x1F //CH收到消息后通知OH

#define OHCH_UNDE_SEND_REQ          0x20 //OH请求CH发送未调数据
#define OHCH_UNDE_RECV_IND          0x21 //CH收到未调数据后通知OH
#define OHCH_DEBU_SEND_REQ          0x22 //OH请求CH发送已调数据或CH请求OH转发已调数据

#define TIMEOUT_EVENT               0x28
#define ALARM_STATUS_CHANGED_EVENT  0x29
#define KEY_DOWN_EVENT              0x2A

#define LOG_MSG_REQ                 0x30 //日志请求

#define MAX_TEL_NUM_LEN     20
#define MAX_DATA_LEN        300

#define COMM_IF_SOUTH       0 //南向接口
#define COMM_IF_NORTH       1 //北向接口
#define COMM_IF_DEBUG       2 //调试接口
#define DATA_TYPE_RSP       0xFF //数据的类型为应答，上报直接采用协议中规定的上报类型
//#define DATA_TYPE_REPORT    1 //数据的类型为主动上报

typedef struct
{
    ULONG ulPrmvType;   //发送模块+接收模块+子类型
    ULONG ulMagicNum;   //用于将请求和确认对应起来
    UCHAR ucResendTime; //记录重发次数
    CHAR  cResult;      //当确认发送时有效
    UCHAR ucDstIf;      //发送数据的目的接口，北向或南向
    UCHAR ucCommMode;   //通信方式
    UCHAR ucDCS;        //如果是短消息传输则需要编码方式，其中0：GSM7Bit，8：UCS2
    UCHAR ucDA;         //地址类型
    CHAR  acDstTelNum[MAX_TEL_NUM_LEN + 1]; //电话号码以数值0结尾
    UCHAR ucDataType;   //数据的类型，0表示主动上报，1表示被动应答，用于下层原封不动地返回供上层判断
    ULONG ulDataLen;    //发送的数据长度
    UCHAR *pucData;     //发送的数据
}OHCH_SEND_DATA_REQ_ST, OHCH_SEND_DATA_CNF_ST;

#define DATA_START_POS  sizeof(OHCH_SEND_DATA_REQ_ST)  //原语中数据流开始的位置
/*
typedef struct
{
    ULONG ulPrmvType;   //发送模块+接收模块+子类型
    ULONG ulMagicNum;   //对应于发送请求
    UCHAR ucDataType;   //与其对应的SEND_DATA_REQ中的DataType取值相同
    UCHAR ucResult;     //0表示成功，非0表示失败的原因
}OHCH_SEND_DATA_CNF_ST;
*/
typedef struct
{
    ULONG ulPrmvType;   //发送模块+接收模块+子类型
    //ULONG ulPadding1;   //用于占位
    ULONG ulDataLenMax; //由原语申请方填写，表示后面的data空间可用的最大长度，而ulDataLen表示的是实际长度
    UCHAR ucPadding2;   //用于占位
    UCHAR ucPadding3;   //用于占位
    UCHAR ucSrcIf;      //接收数据的源接口，北向或南向
    UCHAR ucCommMode;
    UCHAR ucDCS;        //如果是短消息传输则需要编码方式，其中0：GSM7Bit，8：UCS2
    UCHAR ucDA;         //地址类型
    CHAR  acSrcTelNum[MAX_TEL_NUM_LEN + 1]; //发送方的电话号码
    UCHAR ucPadding4;   //用于占位
    ULONG ulDataLen;
    UCHAR *pucData;
}OHCH_RECV_DATA_IND_ST;

typedef struct
{
    ULONG ulPrmvType;       //发送模块+接收模块+子类型
    union                   //用于保存在创建定时器时设置的参数
    {
        UCHAR aucByte[4];   //字节数组
        ULONG ulDoubleWord; //双字
        void *pvPtr;        //指针
    };
}TIMEOUT_EVENT_ST;

typedef struct
{
    ULONG ulPrmvType;       //发送模块+接收模块+子类型
}ALARM_STATUS_CHANGED_EVENT_ST;

//MCM-20_20061107_Linwei_begin
typedef struct
{
    ULONG ulPrmvType;
    USHORT usKey;      //按键的值
    UCHAR ucKeyType;  //按键类型
}KEY_EVENT_ST;
//MCM-20_20061107_Linwei_end

//MCM-10_20061105_Zhonghw_begin
//用于请求读写短信中心号码
typedef struct
{
    ULONG ulPrmvType;
    UCHAR ucMsgSn;    //消息序列号，和操作对象两个字节正好可以对应到移动协议的包标识字段上，互相映射    
    LONG  lResult;
    CHAR  acTelNum[MAX_TEL_NUM_LEN + 1]; //号码为字符串，以NULL结尾
}OHCH_RW_SMC_ADDR_REQ_ST, OHCH_RW_SMC_ADDR_CNF_ST;

typedef struct
{
    ULONG ulPrmvType;
    LONG  lResult;
    CHAR  acTelNum[MAX_TEL_NUM_LEN + 1]; //号码为字符串，以NULL结尾
}OHCH_CSD_CONN_IND_ST, OHCH_CSD_CONN_RSP_ST;
//MCM-10_20061105_Zhonghw_end

#if defined(__BORLANDC__) || defined(_MSC_VER)
#pragma pack(1) //所有命令数据都采用1字节对齐
#define __attribute__(a)
#endif

//用于请求CH将远端或者近端发出的消息通知到对端，目前主要是射频信息和太阳能控制器中的数据
typedef struct
{
    ULONG  ulMessType; //消息ID
    UCHAR  ucObj;      //操作对象
    UCHAR  ucMsgSn;    //消息序列号，和操作对象两个字节正好可以对应到移动协议的包标识字段上，互相映射
    USHORT usDataLen;  //数据长度
    UCHAR  aucData[1];
}__attribute__((packed)) MESSAGE_ST;

#if defined(__BORLANDC__) || defined(_MSC_VER)
#pragma pack()
#endif

//被请求方通知请求方停止请求(目前用于从机请求GPRS连接)
#define RESULT_STOP_REQ 0xF1
typedef struct
{
    ULONG  ulPrmvType;
    LONG   lResult;
    UCHAR  aucIpAddr[4]; //当请求时有效
    USHORT usPortNum;    //当请求时有效
}OHCH_CONN_CENTER_REQ_ST, OHCH_CONN_CENTER_CNF_ST;


typedef struct
{
    ULONG ulPrmvType;
    LONG  lResult;
}OHCH_GPRS_DISCONN_REQ_ST, OHCH_GPRS_DISCONN_IND_ST, OHCH_CSD_DISCONN_IND_ST,
OHCH_SLAVE_UNREG_REQ_ST, OHCH_SLAVE_UNREG_CNF_ST;

typedef struct
{
    ULONG  ulPrmvType;
    UCHAR  ucLogSrc;
    UCHAR  ucLevel;
    USHORT usMsgLen;
    UCHAR  aucMsg[1];
}LOG_MSG_REQ_ST;


#endif
 
