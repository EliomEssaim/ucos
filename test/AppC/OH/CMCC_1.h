/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   CMCC_1.h
    作者:     林雨
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  2006/02/16
       作者:  林雨
       描述:  编码完成
    2. 日期:  2006/11/14
       作者:  钟华文
       描述:  修改问题MCM-32，
              接到切换监控版本的命令时根据程序运行情况配置相应的标志位，
              使此时未运行的程序被bootloader下次启动时选择运行。在协议
              层数据处理完毕后的外层，调用重启函数使设备重启。
    3. 日期:  2007/01/05
       作者:  钟华文
       描述:  修改问题MCM-50,
              在文件升级上下文中增加参量“ulUnSaveBlocks”。              
---------------------------------------------------------------------------*/
#ifndef CMCC_1_H
#define CMCC_1_H

#include "UserType.h"
#include "CMCC_1_Ext.h"
#include "../AppMain.h"

//协议类型定义
#define AP_A    1
#define AP_B    2
#define AP_C    3
#define VP_A    1 //即NP_A
#define MCP_A   1
#define MCP_B   2

//各种协议数据的最大长度
#define MAX_APA_LEN     256 //256 Bytes（指从"协议类型"到"校验单元"最后一个字节，在发送过程转义处理之前的字节数）
#define MAX_APB_LEN     70  //协议包最大长度是指从"起始标志"到"结束标志"，在发送过程ASCII码拆分处理之后的字节数
                            //当使用MODEM（SMS）方式通信时，协议包的最大长度：140 Bytes
#define MAX_APC_LEN     4608//协议包的最大长度：4096 Bytes（指从"协议类型"到"校验单元"最后一个字节，在发送过程转义处理之前的字节数，转义之后可能大于4096，所以使用4608）



//#define APAC_START_FLAG     0x7E    //APA,APC起始标志 ~
//#define APAC_END_FLAG       0x7E    //APA,APC结束标志 ~
//#define APB_START_FLAG      0x21    //APB起始标志 !
//#define APB_END_FLAG        0x21    //APB结束标志 !

//VP层交互标志
#define VP_FLAG_SUCCEEDED           0
#define VP_FLAG_FAILED              1
#define VP_FLAG_COMMAND             0x80

//MCP命令标识
#define MCPA_CMD_ID_REPORT              1
#define MCPA_CMD_ID_QUERY               2
#define MCPA_CMD_ID_SET                 3
#define MCPA_CMD_ID_SWITCH_TO_UPGRADE   0x10

//MCM-32_20061114_zhonghw_begin
#define MCPA_CMD_ID_SWITCH_FILE_VER     0x11
//MCM-32_20061114_zhonghw_end

#define MCPA_CMD_ID_EXT_SET             0x83

#define MCPB_CMD_ID_READ            2
#define MCPB_CMD_ID_WRITE           3

//MCP应答标志
#define RSP_FLAG_SUCCEEDED          0   //成功
#define RSP_FLAG_PARTIALLY_DONE     1   //命令被有条件执行
#define RSP_FLAG_CMD_ID_ERR         2   //命令编号错
#define RSP_FLAG_LEN_ERR            3   //长度错
#define RSP_FLAG_CRC_ERR            4   //CRC校验错
#define RSP_FLAG_OTHER_ERR          0xFE//其它错误
#define RSP_FLAG_CMD                0xFF//命令

//监控对象错误代码
#define MOBJ_CORRECT                0   //正确
#define MOBJ_ID_UNRECOGNIZED        1   //监控数据标识无法识别
#define MOBJ_VALUE_OUT_OF_BOUND     2   //监控数据的设置值超出范围
#define MOBJ_VALUE_NOT_MATCH        3   //监控数据标识与监控数据的值不符合要求
#define MOBJ_LEN_NOT_MATCH          4   //监控数据标识与监控数据长度不匹配
#define MOBJ_VALUE_TOO_LOW          5   //监控数据的检测值低于工作范围
#define MOBJ_VALUE_TOO_HIGH         6   //监控数据的检测值高于工作范围
#define MOBJ_OTHER_ERR              9   //未列出的其它错误

//运行模式
#define RUNNING_MODE_MONITOR        0
#define RUNNING_MODE_UPGRADE        1

//MCP:B交互机制
#define COMFIRM_PER_PACKET          0   //采用每包确认方式
#define MULTI_PACKET_ONE_COMFIRM    1   //采用多问一答方式

//升级的结果
#define UPGRADE_DONE                 0   //已经成功完成升级
#define UPGRADE_ABORTED              1   //设备终止软件升级
#define UPGRADE_CANCELLED            2   //OMC取消软件升级
#define UPGRADE_VALID_FILE_FAILED    3   //文件检查失败
#define UPGRADE_SAVE_FILE_FAILED     4   //保存文件失败
#define UPGRADE_COMM_FAILED          5   //远程通信链路异常
#define UPGRADE_COMM_TIMEOUT         6   //远程通信超时
#define UPGRADE_FTP_DOWNLOAD_TIMEOUT 7   //文件FTP下载超时
#define UPGRADE_FTP_LOGIN_FAILED     8   //文件登录FTP服务器失败
#define UPGRADE_OTHER_ERR            17  //其它异常中断软件升级
#define UPGRADE_NOT_OCCURED          0xFF//没有发生过升级

//是否使用GPRS
#define USE_GPRS                    0
#define NOT_USE_GPRS                1

//上报类型
#define REPORT_TYPE_ALARM               1
#define REPORT_TYPE_STA_INIT            2
#define REPORT_TYPE_INSPECTION          3
#define REPORT_TYPE_REPAIR_CONFIRMED    4
#define REPORT_TYPE_CONFIG_CHANGED      5
#define REPORT_TYPE_LOGIN               6
#define REPORT_TYPE_HEARTBEAT           7
#define REPORT_TYPE_UPGRADE             8
#define REPORT_TYPE_COMM_LINK_FAULT     9

#define OTHER_REPORT_COUNT          (REPORT_TYPE_UPGRADE - REPORT_TYPE_ALARM)
#define OTHER_REPORT_BASE           REPORT_TYPE_STA_INIT

//文件传输控制
#define TRANS_CTRL_START            1//20060417zhw
#define TRANS_CTRL_END              2
#define TRANS_CTRL_CANCEL           3
#define TRANS_CTRL_UPGRADE_COMPLETE 4

#define TRANS_CTRL_INIT             0xFF//厂家定义，当转入升级模式时进入该状态
#define TRANS_CTRL_TRANSFERRING     0xFE//厂家定义，当收到第一个数据包后进入该状态
#define TRANS_CTRL_ABORT            0xFD//厂家定义，当升级过程需要中断退出时进入该状态

//文件数据包应答
#define TRANS_RSP_OK                0   //成功接收，可以继续接收后续数据包
#define TRANS_RSP_RETRANS_REQ       1   //请求OMC重发数据包（前提，之前的包成功接收）
#define TRANS_RSP_DELAY_REQ         2   //请求OMC延时TP后继续发送数据包（前提，此包成功接收）
#define TRANS_RSP_CANCEL_REQ        3   //请求OMC取消软件升级
#define TRANS_RSP_MNFT_ID_ERROR     4   //由于文件中的厂家标识错误，设备终止软件升级
#define TRANS_RSP_DEV_ID_ERROR      5   //由于文件中的设备标识错误，设备终止软件升级
#define TRANS_RSP_OTHER_ERROR       6   //由于文件中的其它错误，设备终止软件升级

//判断监控对象正确性，分为UCHAR/uint2/str/dec分别判断
#define UINT_1 1
#define SINT_1 1
#define UINT_2 2
#define SINT_2 2
#define UINT_4 4
#define STR_10 10
#define STR_20 20
#define STR_40 40

#define TWO_BYTES       2
#define LO_BYTE         0 //二字节数据的第一个字节
#define HI_BYTE         1 //二字节数据的第二个字节

//目前使用多问一答且连发系数为1，因此不使用T4，只使用T5，以后如果使用多问一答且连发系数大于1时再使用T4
#define TIMER_TRANS_CTRL_INTV       (30000 / TIMER_UNIT_MS) //10 seconds
#define TIMER_TRANS_BLK_INTV        (30000 / TIMER_UNIT_MS) //10 seconds

#define TIMER_ALARM_SHORT_INTV      (180000 / TIMER_UNIT_MS)   //3 mins
#define TIMER_ALARM_LONG_INTV       (10800000 / TIMER_UNIT_MS) //3 hours

#define TIMER_HEARTBEAT_TO_INTV     (15000 / TIMER_UNIT_MS)  //暂定15秒
#define TIMER_HEARTBEAT_PERIOD_INTV (60000 / TIMER_UNIT_MS)  //协议规定默认值为1分钟

#define TIMER_LOGIN_TO_INTV         (15000 / TIMER_UNIT_MS)  //暂定15秒
#define TIMER_LOGIN_PERIOD_INTV     (180000 / TIMER_UNIT_MS) //不小于3分钟


//***************协议栈定义***************
#if defined(__BORLANDC__) || defined(_MSC_VER)
#pragma pack(1) //所有命令数据都采用1字节对齐
#define __attribute__(a)
#endif

//AP:A,AP:B,AP_C，本层的数据开销为4字节，包含AP协议类型，PDU类型和CRC校验码，不包含起始结束标志
#define AP_OVERHEAD 4
typedef struct
{
    UCHAR ucApType;
    UCHAR ucPduType;
    UCHAR aucPdu[1];
}__attribute__((packed)) AP_LAYER_ST;

//VP:A，本层的数据开销为9字节
#define VP_OVERHEAD         9
#define MCP_MIN_LEN         2 //包含命令单元和应答标志
typedef struct
{
    ULONG  ulStaNum;
    UCHAR  ucDevNum;
    USHORT usPacketID;
    UCHAR  ucVpFlag;
    UCHAR  ucApId;
    UCHAR  aucPdu[MCP_MIN_LEN]; //至少要包含命令标识和应答标志，两个字节
}__attribute__((packed)) VPA_LAYER_ST;

//MCP:A和MCP:B，命令单元开销为2字节
#define MCPA_MOBJ_MIN_LEN   3 //包含对象长度和对象标识
#define MCPB_MOBJ_MIN_LEN   4
//MCM-32_20061114_zhonghw_begin
#define MCPA_DATA_SWITCH_VER_LEN      10 //切换监控软件版本MCP_A数据单元长度,包含命令单元和两个ID
#define MCPA_MOBJ_SWITCH_VER_MIN_LEN  4  //切换监控软件版本单个监控数据单元长度
//MCM-32_20061114_zhonghw_end
typedef struct
{
    UCHAR ucCmdId;
    UCHAR ucRspFlag;
    UCHAR aucContent[1];
}__attribute__((packed)) MCP_LAYER_ST;

typedef struct
{
    UCHAR ucMObjLen;
    UCHAR aucMObjId[TWO_BYTES];
    UCHAR aucMObjContent[1];
}__attribute__((packed)) MCPA_MOBJ_ST;

typedef struct
{
    USHORT usMObjLen;
    UCHAR  aucMObjId[TWO_BYTES];
    UCHAR  aucMObjContent[1];
}__attribute__((packed)) MCPB_MOBJ_ST;


#if defined(__BORLANDC__) || defined(_MSC_VER)
#pragma pack()
#endif









//***************监控对象结构体定义***************
//MCP:B的监控参量，保存在NV Memory中
typedef struct
{
    ULONG  ulNextBlockSn;       //下一个文件数据块序号
    ULONG  ulMaxBlockSize;      //支持的数据块长度
    ULONG  ulSavedFileSize;     //已保存的文件长度，不是协议要求的数据
    LONG   lUpgradeResult;      //升级的结果
    //UCHAR  aucFileId[STR_LEN];  //文件标识码，保存在不同文件中，不保存在此处
    //UCHAR  aucFile[1];          //文件缓存区，保存在不同的文件中，不保存在此处
}UPGRADE_INFO_SET_ST;




//以下被注释的内容原来用于为使用结构位域定义时的情况
//#define LOCAL_ALARM_BIT     0
//#define CENTER_ALARM_BIT    2
//#define ENABLE_BIT          5
//#define SUPPORT_BIT         7

//#define GetBit(obj, bit)  (((obj) >> (bit)) & 1)
//#define SetBit(obj, bit, value) ((obj) &= (~(1 << (bit))), (obj) |= ((value & 1) << bit))

//#define IsAlarmItemSupported(ucInfo)  GetBit((ucInfo), SUPPORT_BIT)
//#define IsAlarmItemEnabled(ucInfo)    GetBit((ucInfo), ENABLE_BIT)

//#define SetAlarmItemSupportedBit(ucInfo, ucValue)   SetBit((ucInfo), SUPPORT_BIT, (ucValue))
//#define SetAlarmItemEnabledBit(ucInfo, ucValue)     SetBit((ucInfo), ENABLE_BIT, (ucValue))

//#define GetLocalAlarmStatus(ucInfo)     GetBit((ucInfo), LOCAL_ALARM_BIT)
//#define GetCenterAlarmStatus(ucInfo)    GetBit((ucInfo), CENTER_ALARM_BIT)

//#define SetLocalAlarmStatus(ucInfo, ucStatus)   SetBit((ucInfo), LOCAL_ALARM_BIT, (ucStatus))
//#define SetCenterAlarmStatus(ucInfo, ucStatus)  SetBit((ucInfo), LOCAL_ALARM_BIT, (ucStatus))


//已发送的告警项缓冲区，当一条告警上报被发送后，它所包含的告警项就被       
//放入已发送告警项缓冲区中，且按照ID从小到大的顺序排列。告警上下文中的
//ulAlarmItemBegin即指向缓冲区中的某一位置。当收到告警应答后，将使用
//已发送告警缓冲区中将其对应的告警项更新中心告警状态。

//告警发送缓冲区中的每一个告警项可以按照下面的结构来访问
typedef struct
{
    UCHAR ucIDLoByte;   //告警项ID的低字节
    UCHAR ucUsed   : 1; //该告警项是否有效
    UCHAR ucStatus : 1; //告警项状态
}ALARM_ITEM_IN_BUF_ST;

//#define MAX_ALARM_ITEMS                 ((sizeof(ALARM_ITEM_SET_ST)) / sizeof(ALARM_ITEM_INFO_ST))
#define MOBJ_ALARM_LEN                  sizeof(MCPA_MOBJ_ST)

//告警发送缓冲区
typedef struct
{
    ULONG ulAlarmItemCount;
    ALARM_ITEM_IN_BUF_ST astAlarmItems[MAX_ALARM_ITEMS];
}ALARM_ITEM_BUF_ST;

//申明定时器类
class Timer;

//告警上报操作的上下文
typedef struct
{
    Timer  *pTimer;          //定时器引用
    ULONG  ulTimerMagicNum;  //定时器的magic number
    USHORT usPacketID;       //本次上报的序号，用于对应告警上报应答
    UCHAR  ucCommState;      //通信状态，分为IDLE,SEND_DATA_PENDING,WAIT_FOR_RSP
    UCHAR  ucResendState;    //告警重发的状态
    UCHAR  ucAlarmItemCount; //本次上报中告警项的个数
    UCHAR  ucAlarmItemBegin; //告警项在已发送告警缓存区中的起始偏移
}ALARM_REPORT_CTX_ST;


//其它上报的上下文
typedef struct
{
    Timer  *pTimer;              //定时器引用
    ULONG  ulTimerMagicNum;      //定时器的magic number
    USHORT usPacketID;           //本次上报的序号，用于对应上报应答
    UCHAR  ucCommState;          //通信状态
}OTHER_REPORT_CTX_ST;


//升级文件的缓存区
#define FLASH_SECTOR_SIZE 0x1000 //4k，对应于一个FLASH的SECTOR
typedef struct
{
    ULONG ulUsedSize;
    UCHAR aucBuf[FLASH_SECTOR_SIZE];
}FLASH_BUF_ST;

//升级文件的上下文
typedef struct
{
    ULONG ulNextBlockSn;        //下一个数据块的序列号，文件下载校验成功后需更新NV Memory
    ULONG ulMaxBlockSize;       //每包最大长度，初始化时从NV Memory中读出
    ULONG ulSavedFileSize;      //已保存的升级文件大小，写入NV Memory后才更新SN
    LONG  lUpgradeResult;       //升级的结果，需要保存到NV Memory中    

    //MCM-50_20070105_zhonghw_begin
    ULONG ulUnSaveBlocks;       //已经接收但是未保存数据块，范围在0~(缓存大小/包大小)
    //MCM-50_20070105_zhonghw_end    
    ULONG ulUpgradePartIdx;     //存放升级文件的分区下标
    ULONG ulUpgradeProgress;    //升级的进程
    Timer *pTimer;              //定时器引用
    ULONG ulTimerMagicNum;      //定时器的magic number
    FLASH_BUF_ST stFileBuf;     //升级文件的缓存区
}UPGRADE_CTX_ST;

//监控参量的处理函数类型
typedef LONG (*MOBJ_HNDLR)(MCPA_MOBJ_ST *pstMcpA, ULONG ulParamAddr, UCHAR ucDataType, 
                           UCHAR ucIsSupported, UCHAR ucOperation);

#define PARAM_NOT_SUPPORTED     0
#define PARAM_SUPPORTED         1

//监控参量处理表，需要在启动时进行初始化，以该结构数组的下标作为参量ID的低字节
typedef struct
{
    ULONG ulAddr;            //参量的存储地址，对应于存储结构中的位置 
    MOBJ_HNDLR pfHandler;    //操作监控参量的函数，由于告警项处理比较统一，起始可以不需要不同的操作函数
    UCHAR ucDataType;        //参量的数据类型 
    UCHAR ucIsSupported;     //该参量是否被支持，在初始化时由参量列表决定
    //UCHAR ucIsWritable;      //该参量可写
}MOBJ_HNDLR_TBL_ST;

//告警使能和告警状态参量处理表，需要在启动时进行初始化，与上面结构不同的是参量ID的低字节放在结构体中
typedef struct
{
    ULONG ulAddr;            //参量的存储地址，对应于存储结构中的位置 
    MOBJ_HNDLR pfHandler;    //操作监控参量的函数，由于告警项处理比较统一，起始可以不需要不同的操作函数
    UCHAR ucDataType;        //参量的数据类型 
    UCHAR ucIsSupported;     //该参量是否被支持，在初始化时由参量列表决定
    //UCHAR ucIsWritable;      //该参量可写
    UCHAR ucIDLoByte;        //告警项ID的低字节
}ALARM_ITEM_HNDLR_TBL_ST;


//注意!!!!!!!除告警以外，使用每一组参量的最后一个的ID作为数组的大小
#define DEV_INFO_HNDLR_TBL_SIZE         (MOBJ_ID_DI_SEC_ALTITUDE + 1)
#define NM_PARAMS_HNDLR_TBL_SIZE        (MOBJ_ID_NM_FILE_NAME + 1)
#define ALARM_ITEMS_HNDLR_TBL_SIZE      (MAX_ALARM_ITEMS) //因为告警项采用紧缩排列，因此使用告警项个数作为数组大小
#define SETTING_PARAMS_HNDLR_TBL_SIZE   (MOBJ_ID_SP_SEC_STARTUP_UNDER_VOL_SW + 1)
#define RT_COLL_PARAMS_HNDLR_TBL_SIZE   (MOBJ_ID_RC_SEC_CHARGE_CAPACITY + 1)

//notice 均是重发次数，非总的发送次数
#define RESEND_TIME_DATATYPE_RSP            9   //应答重发的次数
#define RESEND_TIME_DATATYPE_ALARM          2   //告警上报重发的次数
#define RESEND_TIME_DATATYPE_REPORT         2   //上报重发的次数
#define RESEND_TIME_GPRS_LINKFAULT          0   //GPRS连接失败后短信上报重发的次数

                  
#endif                   
                   
                   
                   
           
                   
                   
