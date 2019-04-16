/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   YKPP.h
    作者:     林雨
    文件说明: 本文件包含实现定时器机制的代码
    其它:
    函数列表:

    历史记录:
    1. 日期:  2006/02/16
       作者:  林雨
       描述:  编码完成
    2. 日期:  2006/11/3
       作者:  钟华文
       描述:  修改问题MCM-10,修改多处判断操作对象的异步操作处理方式
    3. 日期:  2006/11/29
       作者:  林玮
       描述:  修改问题MCM-42，
              增加通道二的下行输出和驻波处理
    6. 日期:  2007/01/11
       作者:  钟华文
       描述:  修改问题MCM-54，
              增加操作结果错误类型"YKPP_ERR_ASYN_BUF_FULL".
    7. 日期:  2007/03/15
       作者:  钟华文
       描述:  修改问题MCM-64，
              增加通信主从机ID:0x0211；
              在厂家协议控制参数中增加"通信主从机参数"；
---------------------------------------------------------------------------*/
#ifndef YKPP_H
#define YKPP_H

#include "UserType.h"
#include "../Log/Log.h"

//邮科厂家协议定义

#define YKPP_BEGIN_FLAG '#' //厂家协议起始标志
#define YKPP_END_FLAG   '*' //厂家协议结束标志

//编码方案
#define SCHM_A 'A'
#define SCHM_B 'B'
#define SCHM_C 'C'

//操作类型
#define YKPP_READ_REQ  'R'
#define YKPP_READ_RSP  (YKPP_READ_REQ + 1)
#define YKPP_WIRTE_REQ 'W'
#define YKPP_WIRTE_RSP (YKPP_WIRTE_REQ + 1)
#define YKPP_EXE_REQ   'E'
#define YKPP_EXE_RSP   (YKPP_EXE_REQ + 1)

//异步操作的操作类型,与移动协议的操作类型相对应
#define NEED_MORE_OP_READ       2
#define NEED_MORE_OP_WRITE      3

#define FI_CHECKSUM_NEEDED      (1 << 0)
#define FI_DEV_NUM_AVAILABLE    (1 << 1)
#define FI_AVAILABLE            (1 << 7)

#define YKPP_ERR_CHECKSUM        1          //校验码错误
#define YKPP_ERR_OPER_TYPE       2          //操作类型错误
#define YKPP_ERR_PARAM_ID        3          //包含错误的参数ID
#define YKPP_ERR_OPER_FAILED     4          //操作失败
#define YKPP_ERR_ASYN_TIMEOUT    5          //异步操作超时
//MCM-54_20070111_Zhonghw_begin
#define YKPP_ERR_ASYN_BUF_FULL   6          //异步超作上下文满(为了信息完整增加此错误类型)
//MCM-54_20070111_Zhonghw_end

#define FI_DEV_NUN_AVAILABLE_EN    0X02       //增加编码方案中的设备编号鉴权
#define FI_DEV_NUN_AVAILABLE_DIS   0XFD       //去除编码方案中的设备编号鉴权

#define DLIN_MODEN_MEASURE      0          //由无线MODEN来测量下行输入功率
#define DLIN_MODULE_MEASURE     1          //由功率检测模块来测量下行输入功率

typedef struct
{
    UCHAR ucSchm;
    UCHAR ucOperType;
    UCHAR ucResult;
    UCHAR ucFieldIndicator;
    UCHAR ucDevNum;
    UCHAR aucParamTbl[1];
}__attribute__((packed)) YKPP_PDU_ST; //不包含起始结束标志和校验单元


typedef struct
{
    UCHAR  ucUlRfSw;        //上行射频信号开关
    UCHAR  ucDlRfSw;        //下行射频信号开关
    CHAR   cUlOutMax;      //上行输出功率最大值
    UCHAR  ucDlOutMax;      //下行输出功率最大值
    SHORT  sUlOutAdjust;    //上行输出功率校准值
    SHORT  sDlOutAdjust;    //下行输出功率校准值
    SHORT  sDlReflAdjust;   //下行反射功率校准值
    UCHAR  ucUlGainMax;     //上行最大增益
    UCHAR  ucDlInCoupler;   //下行输入耦合器参数

//------------------------------以下为POI新增参数-----------------------------------

    CHAR  cCdma800InPwrAdjust;  //CDMA800输入功率电平校准值
    CHAR  cMbGsmInPwrAdjust;    //移动GSM输入功率电平校准值
    CHAR  cUcGsmInPwrAdjust;    //联通GSM输入功率电平校准值
    CHAR  cMbDcsInPwrAdjust;    //移动DCS输入功率电平校准值
    CHAR  cUcDcsInPwrAdjust;    //联通DCS输入功率电平校准值
    CHAR  c3G1FDDInPwrAdjust;   //3G1-FDD输入功率电平校准值
    CHAR  c3G2FDDInPwrAdjust;   //3G2-FDD输入功率电平校准值
    CHAR  c3G3TDDInPwrAdjust;   //3G3-TDD输入功率电平校准值
    CHAR  cTrunkInPwrAdjust;    //集群系统输入功率电平校准值
    CHAR  cOutPwrAdjust1;       //第一路输出功率电平校准值
    CHAR  cOutPwrAdjust2;       //第二路输出功率电平校准值
    CHAR  cReflectPwrAdjust1;   //第一路反射功率电平校准值
    CHAR  cReflectPwrAdjust2;   //第二路反射功率电平校准值

//------------------------------以上为POI新增参数-----------------------------------

    //MCM-42_20061129_linwei_begin
    UCHAR  ucDlOutMax2;     //下行输出功率最大值(通道二)
    SHORT  sDlOutAdjust2;   //下行输出功率校准值(通道二)
    SHORT  sDlReflAdjust2;  //下行反射功率校准值(通道二)
//------------------------------以上为新增参数-----------------------------------
    //MCM-42_20061129_linwei_end
    
}YKPP_RF_PARAM_ST;

typedef struct
{
    UCHAR  ucRebootSw;              //重启开关，暂时没有使用
    UCHAR  ucTelNumAuthSw;          //电话号码鉴权开关
    struct
    {
        UCHAR ucOnOff;              //开关
        UCHAR ucMaxAlarmPerHour;    //每小时告警上报次数上限，当开关状态为开启时有效
    }stAlarmLimitSw;                //频繁告警限制开关，暂时没有使用
    
    UCHAR  ucAddrAuthSw;            //地址鉴权开关
    UCHAR  ucYkppRspSw;             //厂家命令应答开关
    struct
    {
        UCHAR ucTotal;              //一个判断周期的总次数
        UCHAR ucOccurLimit;         //一个判断周期中告警产生门限次数
        UCHAR ucEliminateLimit;     //一个判断周期中告警恢复门限次数
    }stAlarmJudgeParam;             //告警判断参数???
    
    UCHAR ucUpdateMode;             //在线升级模式，暂时没有使用
    UCHAR ucDebugMode;              //调试模式???
    USHORT usBattJudgeStartVoltage; //模块电池故障判断起始点电池电压的100倍
    USHORT usBattJudgeStopVoltage;  //模块电池故障判断结束点电池电压的100倍
    LOG_MGMT_PARAM_ST stLogMgmtParam;
   	UCHAR ucDlInAcqMode;            //下行输入功率检测方式
   	//MCM-64_20070315_Zhonghw_begin
    UCHAR ucCommRoleType;            //通信主从机:0表示通信主机；1表示通信从机
   	//MCM-64_20070315_Zhonghw_end
}YKPP_CTRL_PARAM_ST;


//邮科主从协议相关参数结构
typedef struct
{
    UCHAR ucSNum;       //从机编号
    UCHAR ucAskingSNum; //正在轮询的从机编号
    UCHAR ucNewSNum;    //正在注册的新从机编号,0表示无新设备正在注册
    USHORT usSBitmap;   //从机位图
    USHORT usCommAlarm; //主从通信告警标识，16位表示16个从机，与从机位图对应，1:告警，0:正常
}YKPP_COMM_PARAM_ST;

//MCM-20080329_Zhonghw_begin
#define MAX_SMS_ADDR_PREFIX_NUM    20  //前缀的最大个数
#define MAX_SMS_ADDR_PREFIX_LEN     4   //单个前缀的最大长度

typedef struct
{
UCHAR ucLen;         //前缀的长度，0表示无前缀，非0表示要匹配的长度
CHAR aucPrefix[MAX_SMS_ADDR_PREFIX_LEN]; //前缀号码字符
}SMS_ADDR_PREFIX_ST;

typedef struct
{
    //UCHAR ucIsUseNationalSN;  //是否使用国家代码，0XF0表示手机号码使用86，0X0F表示SP号码使用86
    SMS_ADDR_PREFIX_ST astSmsAddrPrefix[MAX_SMS_ADDR_PREFIX_NUM];
}YKPP_SMS_ADDR_PREFIX_ST;
//MCM-20080329_Zhonghw_begin

//总的厂家协议参数数据集，可以保存在一个FLASH SECTOR中
typedef struct
{
    YKPP_RF_PARAM_ST    stYkppRfParam;
    YKPP_CTRL_PARAM_ST  stYkppCtrlParam;
    YKPP_COMM_PARAM_ST  stYkppCommParam;
    //MCM-20080329_Zhonghw_begin
    YKPP_SMS_ADDR_PREFIX_ST stYkppSmsAddrParam; //考虑到升级后对以前的参数不影响，放置在最后
    //MCM-20080329_Zhonghw_end
}YKPP_PARAM_SET_ST;


//***************厂家协议监控对象标识***************
//射频参数
#define YKPP_RF_PARAM_SET                           1        //参数ID的高字节
#define YKPP_RF_PARAM_ULRFSW                        1        //上行射频信号开关   
#define YKPP_RF_PARAM_DLRFSW                        2        //下行射频信号开关
#define YKPP_RF_PARAM_ULOUTMAX                      3        //上行输出功率最大值
#define YKPP_RF_PARAM_DLOUTMAX                      4        //下行输出功率最大值
#define YKPP_RF_PARAM_ULOUTADJUST                   5        //上行输出功率校准值 
#define YKPP_RF_PARAM_DLOUTADJUST                   6        //下行输出功率校准值  
#define YKPP_RF_PARAM_DLREFLADJUST                  7        //下行反射功率校准值
#define YKPP_RF_PARAM_ULGAINMAX                     8        //上行最大增益
#define YKPP_RF_PARAM_DLINCOUPLER                   9        //下行输入耦合器参数

//------------------------------以下为POI新增参数-----------------------------------

#define YKPP_RF_PARAM_CDMA800_IN_PWR                0x0A    //CDMA800输入功率电平校准值
#define YKPP_RF_PARAM_MB_GSM_IN_PWR                 0x0B    //移动GSM输入功率电平校准值
#define YKPP_RF_PARAM_UC_GSM_IN_PWR                 0x0C    //联通GSM输入功率电平校准值
#define YKPP_RF_PARAM_MB_DCS_IN_PWR                 0x0D    //移动DCS输入功率电平校准值
#define YKPP_RF_PARAM_UC_DCS_IN_PWR                 0x0E    //联通DCS输入功率电平校准值
#define YKPP_RF_PARAM_3G1_FDD_IN_PWR                0x0F    //3G1-FDD输入功率电平校准值
#define YKPP_RF_PARAM_3G2_FDD_IN_PWR                0x10    //3G2-FDD输入功率电平校准值
#define YKPP_RF_PARAM_3G3_TDD_IN_PWR                0x11    //3G3-TDD输入功率电平校准值
#define YKPP_RF_PARAM_TRUNK_IN_PWR                  0x12    //集群系统输入功率电平校准值
#define YKPP_RF_PARAM_OUT_PWR_ADJUST1               0x13    //第一路输出功率电平校准值
#define YKPP_RF_PARAM_OUT_PWR_ADJUST2               0x14    //第二路输出功率电平校准值
#define YKPP_RF_PARAM_REFL_PWR_ADJUST1              0x15    //第一路反射功率电平校准值
#define YKPP_RF_PARAM_REFL_PWR_ADJUST2              0x16    //第二路反射功率电平校准值

//------------------------------以上为POI新增参数-----------------------------------

//控制参数
#define YKPP_CTRL_PARAM_SET                         2       //参数ID的高字节
#define YKPP_CTRL_PARAM_REBOOT_SW                   1       //重启开关                          //
#define YKPP_CTRL_PARAM_TELNUM_AUTH_SW              2       //电话号码鉴权开关
#define YKPP_CTRL_PARAM_ALARM_LIMIT_SW              3       //频繁告警限制开关
#define YKPP_CTRL_PARAM_ADDR_AUTH_SW                4       //地址鉴权开关
#define YKPP_CTRL_PARAM_YKPP_RS_PSW                 5       //厂家命令应答开关
#define YKPP_CTRL_PARAM_ALARM_JUDGE_PARAM           6       //告警判断参数
#define YKPP_CTRL_PARAM_UPDATE_MODE                 7       //在线升级模式
#define YKPP_CTRL_PARAM_DEBUG_MODE                  8       //调试模式
#define YKPP_CTRL_PARAM_BATT_JUDGE_START_VOLTAGE    9       //模块电池故障判断起始点电池电压的100倍
#define YKPP_CTRL_PARAM_BATT_JUDGE_STOP_VOLTAGE     10      //模块电池故障判断结束点电池电压的100倍
#define YKPP_CTRL_PARAM_LOG_SW                      11      //程序日志开关
#define YKPP_CTRL_PARAM_LOG_OUTPUT                  12      //日志输出
#define YKPP_CTRL_PARAM_LOGSRCMASK                  13      //日志源掩码
#define YKPP_CTRL_PARAM_LEVELMASK                   14      //日志级别掩码
#define YKPP_CTRL_PARAM_MIDMASK                     15      //模块ID掩码
#define YKPP_CTRL_PARAM_DLINACQ_MODE                16      //下行输入功率检测方式 

//MCM-64_20070315_Zhonghw_begin
#define YKPP_CTRL_PARAM_COMM_ROLE_TYPE                  17       //通信主从机
//MCM-64_20070315_Zhonghw_end

//通信参数
#define YKPP_COMM_PARAM_SET                         3        //参数ID的高字节
#define YKPP_COMM_PARAM_SLAVE_BITMAP                1        //从机位图
#define YKPP_COMM_PARAM_SLAVE_NUM                   2        //厂家从机编号
#define YKPP_COMM_PARAM_SMC_ADDR                    3        //短信中心号码

//存储器参数
#define YKPP_MEM_PARAM_SET                          4        //参数ID的高字节
#define YKPP_MEM_PARAM_EEPROMDATA                   1        //EEPROM数据
#define YKPP_MEM_PARAM_ADDRBUSDATA                  2        //FLASH或RAM数据

//协议相关参数
#define YKPP_CORRE_PARAM_SET                        5        //参数ID的高字节
#define YKPP_CORRE_PARAM_STAADDR                    1        //地址（站点编号和设备编号）

//执行参数
#define YKPP_EXE_PARAM_SET                          ':'      //参数ID的高字节
#define YKPP_EXE_PARAM_TOPOLOGY                     1
#define YKPP_EXE_PARAM_REPORT_TYPE                  2        //触发上报
#define YKPP_EXE_PARAM_AT_CMD                       'D'      //AT命令
#define YKPP_EXE_PARAM_ALL_ALARM_ENABLE_SW          3        //所有告警使能开关
#define YKPP_EXE_PARAM_RESET                        4        //恢复设置，0为出厂设置，1为调试设置

//调试参数
#define YKPP_DEBUG_PARAM_SET                        6        //参数ID的高字节
#define YKPP_DEBUG_PARAM_TIMER_INFO                 1        //定时器信息
#define YKPP_DEBUG_PARAM_STACK_INFO                 2        //堆栈信息
#define YKPP_DEBUG_PARAM_MEM_INFO                   3        //内存信息
#define YKPP_DEBUG_PARAM_QUE_INFO                   4        //队列信息


//FLASH地址范围
#define ADDR_FLASH_BEGIN              0x1000000  //起始地址
#define ADDR_FLASH_END                0x11FFFFF  //结束地址

//触发上报类型
#define REPORT_TYPE_YKPP_STA_INIT            2
#define REPORT_TYPE_YKPP_INSPECTION          3
#define REPORT_TYPE_YKPP_REPAIR_CONFIRMED    4
#define REPORT_TYPE_YKPP_CONFIG_CHANGED      5

#define YKPP_RSP_SW_OFF                      0       //表示厂家协议应答开关关闭 
#define YKPP_NEED_NO_RSP                     1       //厂家协议无须回应 
#define YKPP_NEED_TO_RSP                     0       //厂家协议必须回应    



#endif









