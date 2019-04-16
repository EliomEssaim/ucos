/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   CMCC_1_Ext.h
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
       描述:  修改问题MCM-11，
              增加机型配置表，在程序初始化时根据机型来给机型配置表
              中的各个项进行配置。
    3. 日期:  2006/11/01
       作者:  章杰
       描述:  修改问题MCM-5，
              增加上报通信方式的值的宏定义，这样和通信方式的值的宏定义就有所区别了
    4. 日期:  2006/11/3
       作者:  钟华文
       描述:  修改问题MCM-6,修改设置参数中ID"0x045A-0x045E"被两个监控参数共用的情况
    5. 日期:  2006/11/06
       作者:  林雨
       描述:  修改问题MCM-17，
              将所有带通道号的通道1的参数都映射对应的到不带通道号的参数上，即当设备
              是多通道时，查询通道1的参数实际访问的是对应的不带通道号的参数的地址。
    6. 日期:  2006/11/7
       作者:  章杰
       描述:  修改问题MCM-21，
              修改笔误，把宏定义中的FIRELESSCOUPL 改为WIRELESSCOUPL
    7. 日期:  2006/11/8
       作者:  章杰
       描述:  修改问题MCM-23，
              增加参量列表全局变量中可以容纳参量的个数，因为测试机型的参量数已超过其原有容纳数
    8. 日期:  2006/11/8
       作者:  钟华文
       描述:  修改问题MCM-25，
              调整告警参数结构体中新增加成员的位置，使所有告警项的参数都根据ID从小到大的顺序排列。
    9. 日期:  2006/11/29
       作者:  林玮
       描述:  修改问题MCM-42，
              增加通道二的下行输出和驻波处理
    10.日期:  2007/03/15
       作者:  钟华文
       描述:  修改问题MCM-63,
              增加剩余未定义2g机型的宏定义
    11. 日期:  2007/11/20
        作者:  章杰
        描述:  修改问题MCM-80，              
               根据设备类型选择“太阳能蓄电池电压”的检测方式。
---------------------------------------------------------------------------*/
#ifndef Cmcc_1_ExtH
#define Cmcc_1_ExtH

#include "UserType.h"

#ifdef BIG_ENDIAN //默认为Little Endian
#define ADJUST_WORD(word)  ((word) >> 8 | ((word) & 0xFF) << 8) //高低字节对调
#define ADJUST_DWORD(dword) ((((dword) >> 24) & 0xFF) | (((dword) << 24) & 0xFF000000) \
                           | (((dword) >> 8) & 0xFF00) | (((dword) << 8) & 0xFF0000)) //1、4字节对调，2、3字节对调

#define GET_WORD_1ST_BYTE(word)     (((word) >> 8) & 0xFF)
#define GET_WORD_2ND_BYTE(word)     ((word) & 0xFF)
#define GET_DWORD_1ST_BYTE(dword)   (((dword) >> 24) & 0xFF)
#define GET_DWORD_2ND_BYTE(dword)   (((dword) >> 16) & 0xFF)
#define GET_DWORD_3RD_BYTE(dword)   (((dword) >> 8) & 0xFF)
#define GET_DWORD_4TH_BYTE(dword)   ((dword) & 0xFF)

#define GET_WORD(ptr)   ((*(UCHAR *)(ptr) << 8) + *((UCHAR *)(ptr) + 1))
#define SET_WORD(ptr, word)  (*(UCHAR *)(ptr) = GET_WORD_2ND_BYTE((word)), \
                              *((UCHAR *)(ptr) + 1) = GET_WORD_1ST_BYTE((word)))

#define GET_DWORD(ptr) ((*(UCHAR *)(ptr) << 24) \
                      + (*((UCHAR *)(ptr) + 1) << 16) \
                      + (*((UCHAR *)(ptr) + 2) << 8) \
                      +  *((UCHAR *)(ptr) + 3))

#define SET_DWORD(ptr, dword)   (*(UCHAR *)(ptr) = GET_DWORD_4TH_BYTE((dword)), \
                                 *((UCHAR *)(ptr) + 1) = GET_DWORD_3RD_BYTE((dword)), \
                                 *((UCHAR *)(ptr) + 2) = GET_DWORD_2ND_BYTE((dword)), \
                                 *((UCHAR *)(ptr) + 3) = GET_DWORD_1ST_BYTE((dword)))


#else
#define ADJUST_WORD(word)  (word)
#define ADJUST_DWORD(dword) (dword)

#define GET_WORD_1ST_BYTE(word)     ((word) & 0xFF)
#define GET_WORD_2ND_BYTE(word)     (((word) >> 8) & 0xFF)
#define GET_DWORD_1ST_BYTE(dword)   ((dword) & 0xFF)
#define GET_DWORD_2ND_BYTE(dword)   (((dword) >> 8) & 0xFF)
#define GET_DWORD_3RD_BYTE(dword)   (((dword) >> 16) & 0xFF)
#define GET_DWORD_4TH_BYTE(dword)   (((dword) >> 24) & 0xFF)

#define GET_WORD(ptr)  (*(UCHAR *)(ptr) + (*((UCHAR *)(ptr) + 1) << 8))
#define SET_WORD(ptr, word)  (*(UCHAR *)(ptr) = GET_WORD_1ST_BYTE((word)), \
                              *((UCHAR *)(ptr) + 1) = GET_WORD_2ND_BYTE((word)))
#define GET_DWORD(ptr) (*(UCHAR *)(ptr) \
                     + (*((UCHAR *)(ptr) + 1) << 8) \
                     + (*((UCHAR *)(ptr) + 2) << 16) \
                     + (*((UCHAR *)(ptr) + 3) << 24))

#define SET_DWORD(ptr, dword)   (*(UCHAR *)(ptr) = GET_DWORD_1ST_BYTE((dword)), \
                                 *((UCHAR *)(ptr) + 1) = GET_DWORD_2ND_BYTE((dword)), \
                                 *((UCHAR *)(ptr) + 2) = GET_DWORD_3RD_BYTE((dword)), \
                                 *((UCHAR *)(ptr) + 3) = GET_DWORD_4TH_BYTE((dword)))

#endif


#define IGNORE              FAILED
#define FORWARD_UNDESTATE   1
#define SUCCEEDED_DEBUSTATE 2  
#define NEED_TO_FORWARD     3
#define NEED_MORE_OP        4


//AP层起始结束标志
#define APAC_FLAG       0x7E    //APA,APC起始结束标志 ~
#define APB_FLAG        0x21    //APB起始结束标志 !

//MCM-5_20061101_zhangjie_begin
//通信方式
#define COMM_NO_CONN                1 //当不处于CSD或者GPRS方式时认为处于该方式，此时任何上报都可发送
#define COMM_MODE_SMS               1
#define COMM_MODE_GPRS              2
#define COMM_MODE_CSD               0xEE //自定义通信方式

//上报通信方式
#define COMM_REPORT_NO_CONN                1 //2G方式下使用
#define COMM_REPORT_MODE_SMS               1
#define COMM_REPORT_MODE_GPRS              3
#define COMM_REPORT_MODE_CSD               2 
//MCM-5_20061101_zhangjie_end

//常数定义
#define STR_LEN         20
#define LONG_STR_LEN    40
#define DEV_MODEL_LEN   STR_LEN
#define DEV_SN_LEN      STR_LEN
#define LONGITUDE_LEN   STR_LEN
#define LATITUDE_LEN    STR_LEN
//#define MOBJ_TBL_LEN 314*2
#define SFWR_VER_LEN    STR_LEN
#define TEL_NUM_LEN     STR_LEN
#define IP_LEN          4
#define DATE_TIME_LEN   7
#define CRC_LEN         2 //CRC校验码长度
//#define STA_NUM_LEN     4
//#define DEV_NUM_LEN     1
#define IP_ADDR_LEN     4


//***************监控对象标识***************
//设备信息
#define MOBJ_ID_DEV_INFO_SET        0       //参数ID的高字节
#define MOBJ_ID_DI_MNFT_ID          0x02    //设备厂商代码 
#define MOBJ_ID_DI_DEV_TYPE         0x03    //设备类别
#define MOBJ_ID_DI_DEV_MODEL        0x04    //设备型号
#define MOBJ_ID_DI_DEV_SN           0x05    //设备生产序列号
#define MOBJ_ID_DI_CH_COUNT         0x06    //设备的实际信道总数
#define MOBJ_ID_DI_LONGITUDE        0x07    //经度
#define MOBJ_ID_DI_LATITUDE         0x08    //纬度
#define MOBJ_ID_DI_MOBJ_TABLE       0x09    //设备的监控参量列表
#define MOBJ_ID_DI_SFWR_VER         0x0A    //监控版本信息
#define MOBJ_ID_DI_DEV_MODEL2       0x0B    //3G:设备型号
#define MOBJ_ID_DI_RUNNING_MODE     0x10    //3G:嵌入式软件运行模式
#define MOBJ_ID_DI_MAX_APC_LEN      0x11    //3G:可支持AP：C协议的最大长度
#define MOBJ_ID_DI_MCPB_MODE        0x12    //3G:MCP：B采用的交互机制
#define MOBJ_ID_DI_NC               0x13    //3G:连发系数（NC）
#define MOBJ_ID_DI_T1               0x14    //3G:设备响应超时（TOT1）
#define MOBJ_ID_DI_TG               0x15    //3G:发送间隔时间（TG）（此数据在NC＞1时才有意义）
#define MOBJ_ID_DI_TP               0x16    //3G:暂停传输等待时间（TP）
#define MOBJ_ID_DI_TIME_TO_SWITCH   0x17    //3G:转换到软件升级模式时，OMC需要等待的时间
#define MOBJ_ID_DI_UPGRADE_RESULT   0x18    //3G:设备执行软件升级的结果
#define MOBJ_ID_DI_UPGRADE_MODE     0x20    //3G:设备使用的远程升级方式

//0x0030～0x009F    系统保留
//0x00A0～0x00FF    厂家自定义

//------------------------------以下为太阳能控制器新增参数-----------------------------------
//Solar Energy Controller
#define MOBJ_ID_DI_SEC_MNFT_ID          0xC0 //太阳能设备厂商代码
#define MOBJ_ID_DI_SEC_DEV_TYPE         0xC1 //太阳能设备类别
#define MOBJ_ID_DI_SEC_DEV_MODEL        0xC2 //太阳能设备型号
#define MOBJ_ID_DI_SEC_SFWR_VER         0xC3 //太阳能监控版本信息
#define MOBJ_ID_DI_SEC_DEV_SN           0xC4 //太阳能设备生产序列号
#define MOBJ_ID_DI_SEC_SB_MODEL         0xC5 //硅板型号
#define MOBJ_ID_DI_SEC_SB_SPEC          0xC6 //硅板规格
#define MOBJ_ID_DI_SEC_SB_CNT           0xC7 //硅板数量
#define MOBJ_ID_DI_SEC_SB_DIR           0xC8 //硅板面方位角
#define MOBJ_ID_DI_SEC_SB_PITCH_ANGLE   0xC9 //硅板面俯仰角
#define MOBJ_ID_DI_SEC_ST_BAT_MODEL     0xCA //蓄电池型号
#define MOBJ_ID_DI_SEC_ST_BAT_SPEC      0xCB //蓄电池规格
#define MOBJ_ID_DI_SEC_ST_BAT_CNT       0xCC //蓄电池数量
#define MOBJ_ID_DI_SEC_STD_SLMT_PER_DAY 0xCD //日均标准光照时间standar sunlight mean time per day
#define MOBJ_ID_DI_SEC_LONGITUDE        0xCE //太阳能电源站点经度
#define MOBJ_ID_DI_SEC_LATITUDE         0xCF //太阳能电源站点纬度
#define MOBJ_ID_DI_SEC_ALTITUDE         0xD0 //太阳能电源站点海拔
//------------------------------以上为太阳能控制器新增参数-----------------------------------


//设备信息扩展参数，可写，参数和设备信息同
#define MOBJ_ID_DEV_INFO_SET_EX     0x0A    //参数ID的高字节


//网管参数
#define MOBJ_ID_NM_PARAM_SET        0x01    //参数ID的高字节
#define MOBJ_ID_NM_STA_NUM          0x01    //站点编号
#define MOBJ_ID_NM_DEV_NUM          0x02    //设备编号
#define MOBJ_ID_NM_SMC_ADDR         0x10    //短信服务中心号码
#define MOBJ_ID_NM_QNS_TEL_NUM_1    0x11    //查询/设置电话号码1～5
#define MOBJ_ID_NM_QNS_TEL_NUM_2    0x12
#define MOBJ_ID_NM_QNS_TEL_NUM_3    0x13
#define MOBJ_ID_NM_QNS_TEL_NUM_4    0x14
#define MOBJ_ID_NM_QNS_TEL_NUM_5    0x15
#define MOBJ_ID_NM_REPORT_TEL_NUM   0x20    //上报号码
#define MOBJ_ID_NM_EMS_IP_ADDR      0x30    //监控中心IP地址（IP v4）
#define MOBJ_ID_NM_EMS_PORT_NUM     0x31    //监控中心IP地址端口号
#define MOBJ_ID_NM_USE_GPRS         0x32    //3G:设备是否使用GPRS方式
#define MOBJ_ID_NM_GPRS_APN         0x33    //3G:GPRS接入点名称（即：APN）
#define MOBJ_ID_NM_HEARTBEAT_PERIOD 0x34    //3G:设备的心跳包间隔时间
#define MOBJ_ID_NM_DEV_TEL_NUM      0x35    //3G:设备的电话号码
#define MOBJ_ID_NM_GPRS_USER_ID     0x36    //3G:GPRS参数：用户标识
#define MOBJ_ID_NM_GPRS_PWD         0x37    //3G:GPRS参数：口令
#define MOBJ_ID_NM_REPORT_COMM_MODE 0x40    //上报通信方式
#define MOBJ_ID_NM_REPORT_TYPE      0x41    //上报类型
#define MOBJ_ID_NM_COMM_MODE        0x42    //通信方式                         //规范20060810
#define MOBJ_ID_NM_DATE_TIME        0x50    //日期、时间
#define MOBJ_ID_NM_FTP_SERVER_IP_ADDR   0x60 //3G:FTP服务器IP地址（IP v4）
#define MOBJ_ID_NM_FTP_SERVER_PORT_NUM  0x61 //3G:FTP服务器IP地址端口号
#define MOBJ_ID_NM_FTP_USER_ID      0x62    //3G:FTP升级操作的用户名
#define MOBJ_ID_NM_FTP_PWD          0x63    //3G:FTP升级操作的口令
#define MOBJ_ID_NM_FTP_PATH         0x64    //3G:相对路径
#define MOBJ_ID_NM_FILE_NAME        0x65    //3G:文件名

//0x0170～0x019F    系统保留    
//0x01A0～0x01FF    厂家自定义 
//------------------------------以下为太阳能控制器新增参数-----------------------------------
#define MOBJ_ID_NM_SEC_STA_NUM      0xCO

//------------------------------以上为太阳能控制器新增参数-----------------------------------


//告警使能和告警状态
#define MOBJ_ID_ALARM_ENA_SET       0x02    //参数ID的高字节
#define MOBJ_ID_ALARM_STATUS_SET    0x03    //参数ID的高字节

#define MOBJ_ID_A_PWR_LOST          0x01    //电源掉电告警
#define MOBJ_ID_A_PWR_FAULT         0x02    //电源故障告警
#define MOBJ_ID_A_SOLAR_BAT_LOW_PWR 0x03    //太阳能电池低电压告警
#define MOBJ_ID_A_BAT_FAULT         0x04    //监控模块电池故障告警
#define MOBJ_ID_A_POSITION          0x05    //位置告警
#define MOBJ_ID_A_PA_OVERHEAT       0x06    //功放过温告警
#define MOBJ_ID_A_SRC_SIG_CHANGED   0x07    //信源变化告警
#define MOBJ_ID_A_OTHER_MODULE      0x08    //其它模块告警
#define MOBJ_ID_A_OSC_UNLOCKED      0x09    //本振失锁告警
#define MOBJ_ID_A_UL_LNA            0x0A    //上行低噪放故障告警
#define MOBJ_ID_A_DL_LNA            0x0B    //下行低噪放故障告警
#define MOBJ_ID_A_UL_PA             0x0C    //上行功放告警
#define MOBJ_ID_A_DL_PA             0x0D    //下行功放告警
#define MOBJ_ID_A_OPT_MODULE        0x0E    //光收发模块故障告警
#define MOBJ_ID_A_MS_LINK           0x0F    //主从监控链路告警
#define MOBJ_ID_A_DL_IN_OVER_PWR    0x10    //下行输入过功率告警
#define MOBJ_ID_A_DL_IN_UNDER_PWR   0x11    //下行输入欠功率告警
#define MOBJ_ID_A_DL_OUT_OVER_PWR   0x12    //下行输出过功率告警
#define MOBJ_ID_A_DL_OUT_UNDER_PWR  0x13    //下行输出欠功率告警
#define MOBJ_ID_A_DL_SWR            0x14    //下行驻波比告警
#define MOBJ_ID_A_UL_OUT_OVER_PWR   0x15    //上行输出过功率告警
#define MOBJ_ID_A_UL_OUT_UNDER_PWR  0x16    //上行输出欠功率告警//规范20060810
#define MOBJ_ID_A_UL_SWR            0x17    //上行驻波比告警    //规范20060810
#define MOBJ_ID_A_EXT_1             0x20    //外部告警1～8
#define MOBJ_ID_A_EXT_2             0x21
#define MOBJ_ID_A_EXT_3             0x22
#define MOBJ_ID_A_EXT_4             0x23
#define MOBJ_ID_A_EXT_5             0x24
#define MOBJ_ID_A_EXT_6             0x25
#define MOBJ_ID_A_EXT_7             0x26
#define MOBJ_ID_A_EXT_8             0x27

#define MOBJ_ID_A_DOOR              0x28    //门禁告警
#define MOBJ_ID_A_SELF_OSC          0x29    //自激告警
#define MOBJ_ID_A_GPRS_LOGIN_FAILED 0x2A    //3G:GPRS登录失败告警

#define MOBJ_ID_A_DL_IN_OVER_PWR_1      0x30    //下行输入过功率告警（通道1）  
#define MOBJ_ID_A_DL_IN_UNDER_PWR_1     0x31    //下行输入欠功率告警（通道1）
#define MOBJ_ID_A_DL_OUT_OVER_PWR_1     0x32    //下行输出过功率告警（通道1）
#define MOBJ_ID_A_DL_OUT_UNDER_PWR_1    0x33    //下行输出欠功率告警（通道1）
#define MOBJ_ID_A_UL_OUT_OVER_PWR_1     0x34    //上行输出过功率告警（通道1）
#define MOBJ_ID_A_DL_SWR_1              0x35    //下行驻波比告警    （通道1） 

#define MOBJ_ID_A_DL_IN_OVER_PWR_2      0x36    //下行输入过功率告警（通道2）  
#define MOBJ_ID_A_DL_IN_UNDER_PWR_2     0x37    //下行输入欠功率告警（通道2）
#define MOBJ_ID_A_DL_OUT_OVER_PWR_2     0x38    //下行输出过功率告警（通道2）
#define MOBJ_ID_A_DL_OUT_UNDER_PWR_2    0x39    //下行输出欠功率告警（通道2）
#define MOBJ_ID_A_UL_OUT_OVER_PWR_2     0x3A    //上行输出过功率告警（通道2）
#define MOBJ_ID_A_DL_SWR_2              0x3B    //下行驻波比告警    （通道2） 

#define MOBJ_ID_A_DL_IN_OVER_PWR_3      0x3C    //下行输入过功率告警（通道3）  
#define MOBJ_ID_A_DL_IN_UNDER_PWR_3     0x3D    //下行输入欠功率告警（通道3）
#define MOBJ_ID_A_DL_OUT_OVER_PWR_3     0x3E    //下行输出过功率告警（通道3）
#define MOBJ_ID_A_DL_OUT_UNDER_PWR_3    0x3F    //下行输出欠功率告警（通道3）
#define MOBJ_ID_A_UL_OUT_OVER_PWR_3     0x40    //上行输出过功率告警（通道3）
#define MOBJ_ID_A_DL_SWR_3              0x41    //下行驻波比告警    （通道3） 

#define MOBJ_ID_A_DL_IN_OVER_PWR_4      0x42    //下行输入过功率告警（通道4）  
#define MOBJ_ID_A_DL_IN_UNDER_PWR_4     0x43    //下行输入欠功率告警（通道4）
#define MOBJ_ID_A_DL_OUT_OVER_PWR_4     0x44    //下行输出过功率告警（通道4）
#define MOBJ_ID_A_DL_OUT_UNDER_PWR_4    0x45    //下行输出欠功率告警（通道4）
#define MOBJ_ID_A_UL_OUT_OVER_PWR_4     0x46    //上行输出过功率告警（通道4）
#define MOBJ_ID_A_DL_SWR_4              0x47    //下行驻波比告警    （通道4） 

//MCM-25_20061108_zhonghw_begin
#define MOBJ_ID_A_UL_OUT_UNDER_PWR_1    0x48    //上行输出欠功率告警（通道1)//规范20060810
#define MOBJ_ID_A_UL_OUT_UNDER_PWR_2    0x49    //上行输出欠功率告警（通道2)//规范20060810
#define MOBJ_ID_A_UL_OUT_UNDER_PWR_3    0x4A    //上行输出欠功率告警（通道3)//规范20060810
#define MOBJ_ID_A_UL_OUT_UNDER_PWR_4    0x4B    //上行输出欠功率告警（通道4)//规范20060810
#define MOBJ_ID_A_UL_SWR_1              0x4C    //上行驻波比告警    （通道1)//规范20060810
#define MOBJ_ID_A_UL_SWR_2              0x4D    //上行驻波比告警    （通道2)//规范20060810
#define MOBJ_ID_A_UL_SWR_3              0x4E    //上行驻波比告警    （通道3)//规范20060810
#define MOBJ_ID_A_UL_SWR_4              0x4F    //上行驻波比告警    （通道4)//规范20060810
//MCM-25_20061108_zhonghw_end

#define MOBJ_ID_A_TTA_1             0x60    //塔顶放大器告警使能1～8
#define MOBJ_ID_A_TTA_2             0x61
#define MOBJ_ID_A_TTA_3             0x62
#define MOBJ_ID_A_TTA_4             0x63
#define MOBJ_ID_A_TTA_5             0x64
#define MOBJ_ID_A_TTA_6             0x65
#define MOBJ_ID_A_TTA_7             0x66
#define MOBJ_ID_A_TTA_8             0x67

#define MOBJ_ID_A_PA_BYPASS_1       0x68    //功放旁路告警使能1～8
#define MOBJ_ID_A_PA_BYPASS_2       0x69
#define MOBJ_ID_A_PA_BYPASS_3       0x6A
#define MOBJ_ID_A_PA_BYPASS_4       0x6B
#define MOBJ_ID_A_PA_BYPASS_5       0x6C
#define MOBJ_ID_A_PA_BYPASS_6       0x6D
#define MOBJ_ID_A_PA_BYPASS_7       0x6E
#define MOBJ_ID_A_PA_BYPASS_8       0x6F

#define MOBJ_ID_A_SELF_OSC_SHUTDOWN 0x70    //3G:自激关机保护告警
#define MOBJ_ID_A_SELF_OSC_REDUCE_ATT 0x71  //3G:自激降增益保护告警

//0x0280～0x029F    系统保留    
//0x02A0～0x02FF    厂家自定义
//0x0380～0x039F    系统保留    
//0x03A0～0x03FF    厂家自定义 
#define MOBJ_ID_A_RESERVED_BEGIN    0x80
#define MOBJ_ID_A_RESERVED_END      0x9F
#define MOBJ_ID_A_USER_DEFINE_BEGIN 0xA0
#define MOBJ_ID_A_USER_DEFINE_END   0xFF


//------------------------------以下为POI新增参数-----------------------------------

#define MOBJ_ID_A_CDMA800_IN_OVER_PWR   0xA0    //CDMA800输入过功率告警
#define MOBJ_ID_A_CDMA800_IN_UNDER_PWR  0xA1    //CDMA800输入欠功率告警
#define MOBJ_ID_A_MB_GSM_IN_OVER_PWR    0xA2    //移动GSM输入过功率告警
#define MOBJ_ID_A_MB_GSM_IN_UNDER_PWR   0xA3    //移动GSM输入欠功率告警
#define MOBJ_ID_A_UC_GSM_IN_OVER_PWR    0xA4    //联通GSM输入过功率告警
#define MOBJ_ID_A_UC_GSM_IN_UNDER_PWR   0xA5    //联通GSM输入欠功率告警
#define MOBJ_ID_A_MB_DCS_IN_OVER_PWR    0xA6    //移动DCS输入过功率告警
#define MOBJ_ID_A_MB_DCS_IN_UNDER_PWR   0xA7    //移动DCS输入欠功率告警
#define MOBJ_ID_A_UC_DCS_IN_OVER_PWR    0xA8    //联通DCS输入过功率告警
#define MOBJ_ID_A_UC_DCS_IN_UNDER_PWR   0xA9    //联通DCS输入欠功率告警
#define MOBJ_ID_A_3G1_FDD_IN_OVER_PWR   0xAA    //3G1-FDD输入过功率告警
#define MOBJ_ID_A_3G1_FDD_IN_UNDER_PWR  0xAB    //3G1-FDD输入欠功率告警
#define MOBJ_ID_A_3G2_FDD_IN_OVER_PWR   0xAC    //3G2-FDD输入过功率告警
#define MOBJ_ID_A_3G2_FDD_IN_UNDER_PWR  0xAD    //3G2-FDD输入欠功率告警
#define MOBJ_ID_A_3G3_TDD_IN_OVER_PWR   0xAE    //3G3-TDD输入过功率告警
#define MOBJ_ID_A_3G3_TDD_IN_UNDER_PWR  0xAF    //3G3-TDD输入欠功率告警
#define MOBJ_ID_A_TRUNK_IN_OVER_PWR     0xB0    //集群系统输入过功率告警
#define MOBJ_ID_A_TRUNK_IN_UNDER_PWR    0xB1    //集群系统输入欠功率告警

//------------------------------以上为POI新增参数-----------------------------------

//------------------------------以下为太阳能控制器新增参数-----------------------------------
#define MOBJ_ID_A_SEC_ST_BAT_BLOWOUT    0xC0    //蓄电池保险丝断
#define MOBJ_ID_A_SEC_SELF_RESUME_BLOWOUT 0xC1  //输出自恢复保险丝断
#define MOBJ_ID_A_SEC_OUTPUT_OVER_CUR   0xC2    //输出过流, over current
#define MOBJ_ID_A_SEC_CIRCUIT_FAULT     0xC3    //控制电路故障
#define MOBJ_ID_A_SEC_VOL_24_FAULT      0xC4    //24V负载故障
#define MOBJ_ID_A_SEC_VOL_12_FAULT      0xC5    //12V负载故障
#define MOBJ_ID_A_SEC_VOL_5_FAULT       0xC6    //5V负载故障
#define MOBJ_ID_A_SEC_USE_UNDER_VOL     0xC7    //欠压使用提示
#define MOBJ_ID_A_SEC_OVER_VOL          0xC8    //蓄电池过压告警
#define MOBJ_ID_A_SEC_OVER_DISCHARGING  0xC9    //蓄电池预过放告警
#define MOBJ_ID_A_SEC_OVER_DISCHARGED   0xCA    //蓄电池过放告警
#define MOBJ_ID_A_SEC_ST_BAT_FAULT      0xCB    //蓄电池故障
#define MOBJ_ID_A_SEC_SB_FAULT          0xCC    //硅板故障
#define MOBJ_ID_A_SEC_DOOR              0xCD    //门禁告警
#define MOBJ_ID_A_SEC_ALEAK             0xCE    //水漏告警
//------------------------------以上为太阳能控制器新增参数-----------------------------------

//最大的告警项ID的低字节，用于判断告警监控参量的合法性，在增加告警项时需要更新
#define MAX_ALARM_ITEM_ID               MOBJ_ID_A_SEC_ALEAK


//设置参数
#define MOBJ_ID_SETTING_PARAM_SET   0x04    //参数ID的高字节

#define MOBJ_ID_SP_RF_SW            0x01    //射频信号开关状态
#define MOBJ_ID_SP_PA_SW_1          0x02    //功放开关状态1～8 
#define MOBJ_ID_SP_PA_SW_2          0x03
#define MOBJ_ID_SP_PA_SW_3          0x04
#define MOBJ_ID_SP_PA_SW_4          0x05
#define MOBJ_ID_SP_PA_SW_5          0x06
#define MOBJ_ID_SP_PA_SW_6          0x07
#define MOBJ_ID_SP_PA_SW_7          0x08
#define MOBJ_ID_SP_PA_SW_8          0x09

#define MOBJ_ID_SP_WORK_CH_NUM_1    0x10    //工作信道号1～16
#define MOBJ_ID_SP_WORK_CH_NUM_2    0x11
#define MOBJ_ID_SP_WORK_CH_NUM_3    0x12
#define MOBJ_ID_SP_WORK_CH_NUM_4    0x13
#define MOBJ_ID_SP_WORK_CH_NUM_5    0x14
#define MOBJ_ID_SP_WORK_CH_NUM_6    0x15
#define MOBJ_ID_SP_WORK_CH_NUM_7    0x16
#define MOBJ_ID_SP_WORK_CH_NUM_8    0x17
#define MOBJ_ID_SP_WORK_CH_NUM_9    0x18    
#define MOBJ_ID_SP_WORK_CH_NUM_10   0x19
#define MOBJ_ID_SP_WORK_CH_NUM_11   0x1A
#define MOBJ_ID_SP_WORK_CH_NUM_12   0x1B
#define MOBJ_ID_SP_WORK_CH_NUM_13   0x1C
#define MOBJ_ID_SP_WORK_CH_NUM_14   0x1D
#define MOBJ_ID_SP_WORK_CH_NUM_15   0x1E
#define MOBJ_ID_SP_WORK_CH_NUM_16   0x1F

#define MOBJ_ID_SP_SHF_CH_NUM_1     0x20   //移频信道号1～16 
#define MOBJ_ID_SP_SHF_CH_NUM_2     0x21
#define MOBJ_ID_SP_SHF_CH_NUM_3     0x22
#define MOBJ_ID_SP_SHF_CH_NUM_4     0x23
#define MOBJ_ID_SP_SHF_CH_NUM_5     0x24
#define MOBJ_ID_SP_SHF_CH_NUM_6     0x25
#define MOBJ_ID_SP_SHF_CH_NUM_7     0x26
#define MOBJ_ID_SP_SHF_CH_NUM_8     0x27
#define MOBJ_ID_SP_SHF_CH_NUM_9     0x28
#define MOBJ_ID_SP_SHF_CH_NUM_10    0x29
#define MOBJ_ID_SP_SHF_CH_NUM_11    0x2A
#define MOBJ_ID_SP_SHF_CH_NUM_12    0x2B
#define MOBJ_ID_SP_SHF_CH_NUM_13    0x2C
#define MOBJ_ID_SP_SHF_CH_NUM_14    0x2D
#define MOBJ_ID_SP_SHF_CH_NUM_15    0x2E
#define MOBJ_ID_SP_SHF_CH_NUM_16    0x2F

#define MOBJ_ID_SP_WORK_UB_CH_NUM   0x30    //工作频带的上边带信道号
#define MOBJ_ID_SP_WORK_LB_CH_NUM   0x31    //工作频带的下边带信道号
#define MOBJ_ID_SP_SHF_UB_CH_NUM    0x32    //移频频带的上边带信道号
#define MOBJ_ID_SP_SHF_LB_CH_NUM    0x33    //移频频带的下边带信道号

#define MOBJ_ID_SP_WORK_UB_CH_NUM_1   0x34  //工作频带的上边带信道号（通道1）
#define MOBJ_ID_SP_WORK_LB_CH_NUM_1   0x35  //工作频带的下边带信道号（通道1）
#define MOBJ_ID_SP_WORK_UB_CH_NUM_2   0x36  //工作频带的上边带信道号（通道2）
#define MOBJ_ID_SP_WORK_LB_CH_NUM_2   0x37  //工作频带的下边带信道号（通道2）
#define MOBJ_ID_SP_WORK_UB_CH_NUM_3   0x38  //工作频带的上边带信道号（通道3）
#define MOBJ_ID_SP_WORK_LB_CH_NUM_3   0x39  //工作频带的下边带信道号（通道3）
#define MOBJ_ID_SP_WORK_UB_CH_NUM_4   0x3A  //工作频带的上边带信道号（通道4）
#define MOBJ_ID_SP_WORK_LB_CH_NUM_4   0x3B  //工作频带的下边带信道号（通道4）

#define MOBJ_ID_SP_UL_ATT          0x40     //上行衰减值
#define MOBJ_ID_SP_DL_ATT          0x41     //下行衰减值
#define MOBJ_ID_SP_UL_ATT_1        0x42     //上行衰减值（通道1）
#define MOBJ_ID_SP_DL_ATT_1        0x43     //下行衰减值（通道1）
#define MOBJ_ID_SP_UL_ATT_2        0x44     //上行衰减值（通道2）
#define MOBJ_ID_SP_DL_ATT_2        0x45     //下行衰减值（通道2）
#define MOBJ_ID_SP_UL_ATT_3        0x46     //上行衰减值（通道3）
#define MOBJ_ID_SP_DL_ATT_3        0x47     //下行衰减值（通道3）
#define MOBJ_ID_SP_UL_ATT_4        0x48     //上行衰减值（通道4）
#define MOBJ_ID_SP_DL_ATT_4        0x49     //下行衰减值（通道4）

#define MOBJ_ID_SP_DL_SWR_THR      0x50     //下行驻波比门限
#define MOBJ_ID_SP_PA_OVERHEAT_THR 0x51     //功放过温度告警门限
#define MOBJ_ID_SP_SRC_CELL_ID     0x52     //信源小区识别码参照值

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR  0x53 //下行输入功率欠功率门限
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR   0x54 //下行输入功率过功率门限
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR 0x55 //下行输出功率欠功率门限
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR  0x56 //下行输出功率过功率门限
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR  0x57 //上行输出功率过功率门限

#define MOBJ_ID_SP_SHF_UB_CH_NUM_1 0x58     //移频频带的上边带信道号（通道1）
#define MOBJ_ID_SP_SHF_LB_CH_NUM_1 0x59     //移频频带的下边带信道号（通道1）
#define MOBJ_ID_SP_SHF_UB_CH_NUM_2 0x5A     //移频频带的上边带信道号（通道2）
#define MOBJ_ID_SP_SHF_LB_CH_NUM_2 0x5B     //移频频带的下边带信道号（通道2）
#define MOBJ_ID_SP_SHF_UB_CH_NUM_3 0x5C     //移频频带的上边带信道号（通道3）
#define MOBJ_ID_SP_SHF_LB_CH_NUM_3 0x5D     //移频频带的下边带信道号（通道3）
#define MOBJ_ID_SP_SHF_UB_CH_NUM_4 0x5E     //移频频带的上边带信道号（通道4）
#define MOBJ_ID_SP_SHF_LB_CH_NUM_4 0x5F     //移频频带的下边带信道号（通道4）

#define MOBJ_ID_SP_RF_SW_1         0x60     //射频切换开关状态1～6 （载波池）
#define MOBJ_ID_SP_RF_SW_2         0x61
#define MOBJ_ID_SP_RF_SW_3         0x62
#define MOBJ_ID_SP_RF_SW_4         0x63
#define MOBJ_ID_SP_RF_SW_5         0x64
#define MOBJ_ID_SP_RF_SW_6         0x65

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_1  0x66 //下行输入功率欠功率门限（通道1）
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_1   0x67 //下行输入功率过功率门限（通道1）
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_1 0x68 //下行输出功率欠功率门限（通道1）
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_1  0x69 //下行输出功率过功率门限（通道1）
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_1  0x6A //上行输出功率过功率门限（通道1）
#define MOBJ_ID_SP_DL_SWR_THR_1           0x6B //下行驻波比门限        （通道1）
#define MOBJ_ID_SP_UL_SWR_THR_1           0x4A //上行驻波比门限        （通道1）//规范20060810

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_2  0x6C //下行输入功率欠功率门限（通道2）
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_2   0x6D //下行输入功率过功率门限（通道2）
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_2 0x6E //下行输出功率欠功率门限（通道2）
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_2  0x6F //下行输出功率过功率门限（通道2）
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_2  0x70 //上行输出功率过功率门限（通道2）
#define MOBJ_ID_SP_DL_SWR_THR_2           0x71 //下行驻波比门限        （通道2）
#define MOBJ_ID_SP_UL_SWR_THR_2           0x4B //上行驻波比门限        （通道2）//规范20060810

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_3  0x72 //下行输入功率欠功率门限（通道3）
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_3   0x73 //下行输入功率过功率门限（通道3）
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_3 0x74 //下行输出功率欠功率门限（通道3）
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_3  0x75 //下行输出功率过功率门限（通道3）
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_3  0x76 //上行输出功率过功率门限（通道3）
#define MOBJ_ID_SP_DL_SWR_THR_3           0x77 //下行驻波比门限        （通道3）
#define MOBJ_ID_SP_UL_SWR_THR_3           0x4C//上行驻波比门限         （通道3）//规范20060810

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_4  0x78 //下行输入功率欠功率门限（通道4）
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_4   0x79 //下行输入功率过功率门限（通道4）
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_4 0x7A //下行输出功率欠功率门限（通道4）
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_4  0x7B //下行输出功率过功率门限（通道4）
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_4  0x7C //上行输出功率过功率门限（通道4）
#define MOBJ_ID_SP_DL_SWR_THR_4           0x7D //下行驻波比门限        （通道4）
#define MOBJ_ID_SP_UL_SWR_THR_4           0x4D //上行驻波比门限        （通道4）//规范20060810

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX    0x80 //3G:下行输入功率欠功率门限，sint2型，单位为dBm，比例为10，以下同
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX     0x81 //3G:下行输入功率过功率门限
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX   0x82 //3G:下行输出功率欠功率门限
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX    0x83 //3G:下行输出功率过功率门限
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX    0x84 //3G:上行输出功率过功率门限
#define MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX   0x85 //3G:上行输出功率欠功率门限//规范20060810
#define MOBJ_ID_SP_UL_SWR_THR_EX             0x86 //上行驻波比门限           //规范20060810

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_1  0x88 //3G:下行输入功率欠功率门限（通道1）
#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_2  0x89 //3G:下行输入功率欠功率门限（通道2）
#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_3  0x8A //3G:下行输入功率欠功率门限（通道3）
#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_4  0x8B //3G:下行输入功率欠功率门限（通道4）

#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_1   0x8C //3G:下行输入功率过功率门限（通道1）
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_2   0x8D //3G:下行输入功率过功率门限（通道2）
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_3   0x8E //3G:下行输入功率过功率门限（通道3）
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_4   0x8F //3G:下行输入功率过功率门限（通道4）

#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_1 0x90 //3G:下行输出功率欠功率门限（通道1）
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_2 0x91 //3G:下行输出功率欠功率门限（通道2）
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_3 0x92 //3G:下行输出功率欠功率门限（通道3）
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_4 0x93 //3G:下行输出功率欠功率门限（通道4）

#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_1  0x94 //3G:下行输出功率过功率门限（通道1）
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_2  0x95 //3G:下行输出功率过功率门限（通道2）
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_3  0x96 //3G:下行输出功率过功率门限（通道3）
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_4  0x97 //3G:下行输出功率过功率门限（通道4）

#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_1  0x98 //3G:上行输出功率过功率门限（通道1）
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_2  0x99 //3G:上行输出功率过功率门限（通道2）
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_3  0x9A //3G:上行输出功率过功率门限（通道3）
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_4  0x9B //3G:上行输出功率过功率门限（通道4）

#define MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_1 0x9C //3G:上行输出功率欠功率门限 (通道1)//规范20060810
#define MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_2 0x9D //3G:上行输出功率欠功率门限 (通道2)//规范20060810
#define MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_3 0x9E //3G:上行输出功率欠功率门限 (通道3)//规范20060810
#define MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_4 0x9F //3G:上行输出功率欠功率门限 (通道4)//规范20060810

//0x049C～0x049F    系统保留    
//0x04A0～0x04FF    厂家自定义
//------------------------------以下为POI新增参数-----------------------------------

#define MOBJ_ID_SP_CDMA800_IN_OVER_PWR_THR  0xA0    //CDMA800输入过功率门限
#define MOBJ_ID_SP_CDMA800_IN_UNDER_PWR_THR 0xA1    //CDMA800输入欠功率门限

#define MOBJ_ID_SP_MB_GSM_IN_OVER_PWR_THR   0xA2    //移动GSM输入过功率门限
#define MOBJ_ID_SP_MB_GSM_IN_UNDER_PWR_THR  0xA3    //移动GSM输入欠功率门限

#define MOBJ_ID_SP_UC_GSM_IN_OVER_PWR_THR   0xA4    //联通GSM输入过功率门限
#define MOBJ_ID_SP_UC_GSM_IN_UNDER_PWR_THR  0xA5    //联通GSM输入欠功率门限

#define MOBJ_ID_SP_MB_DCS_IN_OVER_PWR_THR   0xA6    //移动DCS输入过功率门限
#define MOBJ_ID_SP_MB_DCS_IN_UNDER_PWR_THR  0xA7    //移动DCS输入欠功率门限

#define MOBJ_ID_SP_UC_DCS_IN_OVER_PWR_THR   0xA8    //联通DCS输入过功率门限
#define MOBJ_ID_SP_UC_DCS_IN_UNDER_PWR_THR  0xA9    //联通DCS输入欠功率门限

#define MOBJ_ID_SP_3G1_FDD_IN_OVER_PWR_THR  0xAA    //3G1-FDD输入过功率门限
#define MOBJ_ID_SP_3G1_FDD_IN_UNDER_PWR_THR 0xAB    //3G1-FDD输入欠功率门限

#define MOBJ_ID_SP_3G2_FDD_IN_OVER_PWR_THR  0xAC    //3G2-FDD输入过功率门限
#define MOBJ_ID_SP_3G2_FDD_IN_UNDER_PWR_THR 0xAD    //3G2-FDD输入欠功率门限

#define MOBJ_ID_SP_3G3_TDD_IN_OVER_PWR_THR  0xAE    //3G3-TDD输入过功率门限
#define MOBJ_ID_SP_3G3_TDD_IN_UNDER_PWR_THR 0xAF    //3G3-TDD输入欠功率门限

#define MOBJ_ID_SP_TRUNK_IN_OVER_PWR_THR    0xB0    //集群系统输入过功率门限
#define MOBJ_ID_SP_TRUNK_IN_UNDER_PWR_THR   0xB1    //集群系统输入欠功率门限

//------------------------------以上为POI新增参数-----------------------------------

//------------------------------以下为太阳能控制器新增参数-----------------------------------
#define MOBJ_ID_SP_SEC_OVER_CHARGED_THR             0xC0    //过充电压门限
#define MOBJ_ID_SP_SEC_EVEN_CHARGE_THR              0xC1    //均衡充电电压门限
#define MOBJ_ID_SP_SEC_OVER_CHARGED_RESUME_THR      0xC2    //过充恢复电压门限
#define MOBJ_ID_SP_SEC_FC_TO_PC_VOL_THR             0xC3    //强充转脉充电压门限
#define MOBJ_ID_SP_SEC_OVER_DISCHARGING_THR         0xC4    //预过放电压门限
#define MOBJ_ID_SP_SEC_OVER_DISCHARGED_THR          0xC5    //过放电压门限
#define MOBJ_ID_SP_SEC_OVER_DISCHARGED_RESUME_THR   0xC6    //过放恢复电压门限
#define MOBJ_ID_SP_SEC_PC_TO_FC_CUR_THR             0xC7    //脉充转强充电流门限
#define MOBJ_ID_SP_SEC_OUTPUT_OVER_CUR_THR          0xC8    //输出过流门限
#define MOBJ_ID_SP_SEC_RESET_SW                     0xC9    //复位开关
#define MOBJ_ID_SP_SEC_CHARGE_SW                    0xCA    //充电开关
#define MOBJ_ID_SP_SEC_LOAD_SW                      0xCB    //负载开关
#define MOBJ_ID_SP_SEC_STARTUP_UNDER_VOL_SW         0xCC    //负载欠压启动开关
//------------------------------以上为太阳能控制器新增参数-----------------------------------

//实时采样参数
#define MOBJ_ID_RT_COLL_PARAM_SET  0x05     //参数ID的高字节
#define MOBJ_ID_RC_PA_TEMP         0x01     //功放温度值
#define MOBJ_ID_RC_DL_IN_PWR       0x02     //下行输入功率电平
#define MOBJ_ID_RC_DL_OUT_PWR      0x03     //下行输出功率电平
#define MOBJ_ID_RC_UL_THE_GAIN     0x04     //上行理论增益
#define MOBJ_ID_RC_DL_ACT_GAIN     0x05     //下行实际增益
#define MOBJ_ID_RC_DL_SWR          0x06     //下行驻波比值
#define MOBJ_ID_RC_SS_MNC          0x07     //信源信息：运营商代码
#define MOBJ_ID_RC_SS_LAC          0x08     //信源信息：位置区编码
#define MOBJ_ID_RC_SS_BSIC         0x09     //信源信息：基站识别码
#define MOBJ_ID_RC_SS_BCCH         0x0A     //信源信息：BCCH绝对载频号
#define MOBJ_ID_RC_SS_BCCH_RX_LEV  0x0B     //信源信息：BCCH接收电平
#define MOBJ_ID_RC_SS_CI           0x0C     //信源信息：小区识别码实时值
#define MOBJ_ID_RC_UL_OUT_PWR      0x0D     //上行输出功率电平
#define MOBJ_ID_RC_OPT_RX_PWR      0x0E     //光收功率
#define MOBJ_ID_RC_OPT_TX_PWR      0x0F     //光发功率

#define MOBJ_ID_RC_DL_IN_PWR_1     0x10     //下行输入功率电平（通道1）
#define MOBJ_ID_RC_DL_OUT_PWR_1    0x11     //下行输出功率电平（通道1）
#define MOBJ_ID_RC_UL_THE_GAIN_1   0x12     //上行理论增益（通道1）
#define MOBJ_ID_RC_DL_ACT_GAIN_1   0x13     //下行实际增益（通道1）
#define MOBJ_ID_RC_UL_OUT_PWR_1    0x14     //上行输出功率电平（通道1）
#define MOBJ_ID_RC_DL_SWR_1        0x15     //下行驻波比值（通道1）

#define MOBJ_ID_RC_DL_IN_PWR_2     0x16     //下行输入功率电平（通道2）
#define MOBJ_ID_RC_DL_OUT_PWR_2    0x17     //下行输出功率电平（通道2）
#define MOBJ_ID_RC_UL_THE_GAIN_2   0x18     //上行理论增益（通道2）
#define MOBJ_ID_RC_DL_ACT_GAIN_2   0x19     //下行实际增益（通道2）
#define MOBJ_ID_RC_UL_OUT_PWR_2    0x1A     //上行输出功率电平（通道2）
#define MOBJ_ID_RC_DL_SWR_2        0x1B     //下行驻波比值（通道2）

#define MOBJ_ID_RC_DL_IN_PWR_3     0x1C     //下行输入功率电平（通道3）
#define MOBJ_ID_RC_DL_OUT_PWR_3    0x1D     //下行输出功率电平（通道3）
#define MOBJ_ID_RC_UL_THE_GAIN_3   0x1E     //上行理论增益（通道3）
#define MOBJ_ID_RC_DL_ACT_GAIN_3   0x1F     //下行实际增益（通道3）
#define MOBJ_ID_RC_UL_OUT_PWR_3    0x20     //上行输出功率电平（通道3）
#define MOBJ_ID_RC_DL_SWR_3        0x21     //下行驻波比值（通道3）

#define MOBJ_ID_RC_DL_IN_PWR_4     0x22     //下行输入功率电平（通道4）
#define MOBJ_ID_RC_DL_OUT_PWR_4    0x23     //下行输出功率电平（通道4）
#define MOBJ_ID_RC_UL_THE_GAIN_4   0x24     //上行理论增益（通道4）
#define MOBJ_ID_RC_DL_ACT_GAIN_4   0x25     //下行实际增益（通道4）
#define MOBJ_ID_RC_UL_OUT_PWR_4    0x26     //上行输出功率电平（通道4）
#define MOBJ_ID_RC_DL_SWR_4        0x27     //下行驻波比值（通道4）

#define MOBJ_ID_RC_UL_BYPASS_STATUS  0x28   //上行信号旁路状态
#define MOBJ_ID_RC_DL_BYPASS_STATUS  0x29   //下行信号旁路状态

#define MOBJ_ID_RC_DL_IN_PWR_EX    0x30     //3G:下行输入功率电平，sint2型，单位为dBm，比例为10，以下同
#define MOBJ_ID_RC_DL_OUT_PWR_EX   0x31     //3G:下行输出功率电平
#define MOBJ_ID_RC_UL_OUT_PWR_EX   0x32     //3G:上行输出功率电平
#define MOBJ_ID_RC_UL_SWR          0x33     //上行驻波比值           //规范20060810

#define MOBJ_ID_RC_DL_IN_PWR_EX_1  0x34     //3G:下行输入功率电平（通道1）
#define MOBJ_ID_RC_DL_IN_PWR_EX_2  0x35     //3G:下行输入功率电平（通道2）
#define MOBJ_ID_RC_DL_IN_PWR_EX_3  0x36     //3G:下行输入功率电平（通道3）
#define MOBJ_ID_RC_DL_IN_PWR_EX_4  0x37     //3G:下行输入功率电平（通道4）

#define MOBJ_ID_RC_DL_OUT_PWR_EX_1 0x38     //3G:下行输出功率电平（通道1）
#define MOBJ_ID_RC_DL_OUT_PWR_EX_2 0x39     //3G:下行输出功率电平（通道2）
#define MOBJ_ID_RC_DL_OUT_PWR_EX_3 0x3A     //3G:下行输出功率电平（通道3）
#define MOBJ_ID_RC_DL_OUT_PWR_EX_4 0x3B     //3G:下行输出功率电平（通道4）

#define MOBJ_ID_RC_UL_OUT_PWR_EX_1 0x3C     //3G:上行输出功率电平（通道1）
#define MOBJ_ID_RC_UL_OUT_PWR_EX_2 0x3D     //3G:上行输出功率电平（通道2）
#define MOBJ_ID_RC_UL_OUT_PWR_EX_3 0x3E     //3G:上行输出功率电平（通道3）
#define MOBJ_ID_RC_UL_OUT_PWR_EX_4 0x3F     //3G:上行输出功率电平（通道4）

//0x049C～0x049F    信源信息（WCDMA预留）

#define MOBJ_ID_RC_DONOR_CPICH_LEV     0x50 //施主端口CPICH功率电平
#define MOBJ_ID_RC_SERVICE_CPICH_LEV   0x51 //用户端口CPICH功率电平

//0x0560～0x059F    系统保留    
//0x05A0～0x05FF    厂家自定义 
//------------------------------以下为POI新增参数-----------------------------------

#define MOBJ_ID_RC_CDMA800_IN_PWR   0xA0    //CDMA800输入功率电平值
#define MOBJ_ID_RC_MB_GSM_IN_PWR    0xA1    //移动GSM输入功率电平值
#define MOBJ_ID_RC_UC_GSM_IN_PWR    0xA2    //联通GSM输入功率电平值
#define MOBJ_ID_RC_MB_DCS_IN_PWR    0xA3    //移动DCS输入功率电平值
#define MOBJ_ID_RC_UC_DCS_IN_PWR    0xA4    //联通DCS输入功率电平值
#define MOBJ_ID_RC_3G1_FDD_IN_PWR   0xA5    //3G1-FDD输入功率电平值
#define MOBJ_ID_RC_3G2_FDD_IN_PWR   0xA6    //3G2-FDD输入功率电平值
#define MOBJ_ID_RC_3G3_TDD_IN_PWR   0xA7    //3G3-FDD输入功率电平值
#define MOBJ_ID_RC_TRUNK_IN_PWR     0xA8    //集群系统输入功率电平值

//------------------------------以上为POI新增参数-----------------------------------

//------------------------------以下为太阳能控制器新增参数-----------------------------------
#define MOBJ_ID_RC_SEC_ENV_TEMP         0xC0    //蓄电池环境温度
#define MOBJ_ID_RC_SEC_SB_VOL           0xC1    //硅板电压
#define MOBJ_ID_RC_SEC_ST_BAT_VOL       0xC2    //蓄电池电压
#define MOBJ_ID_RC_SEC_VOL_24           0xC3    //输出24V电压
#define MOBJ_ID_RC_SEC_VOL_12           0xC4    //输出12V电压
#define MOBJ_ID_RC_SEC_VOL_5            0xC5    //输出5V电压
#define MOBJ_ID_RC_SEC_SB_CUR           0xC6    //硅板电流
#define MOBJ_ID_RC_SEC_LOAD_CUR         0xC7    //负载电流
#define MOBJ_ID_RC_SEC_CHARGE_CUR       0xC8    //充电电流
#define MOBJ_ID_RC_SEC_WIRE_VOL         0xC9    //蓄电池连线压降
#define MOBJ_ID_RC_SEC_CAPACITY         0xCA    //蓄电池荷电量
#define MOBJ_ID_RC_SEC_CHARGE_CAPACITY  0xCB    //当天充入的电量
//------------------------------以上为太阳能控制器新增参数-----------------------------------

//MCP:B的监控参量
//设备远程升级信息
#define MOBJ_ID_UPGRADE_INFO_SET        0x02     //参数ID的高字节
#define MOBJ_ID_UI_RUNNING_MODE         0x01     //嵌入式软件运行模式
#define MOBJ_ID_UI_NEXT_BLOCK_SN        0x02     //下一个文件数据块序号
#define MOBJ_ID_UI_MAX_BLOCK_SIZE       0x03     //支持的数据块长度

//远程升级文件信息
#define MOBJ_ID_UPGRADE_FILE_INFO_SET   0x03     //参数ID的高字节
#define MOBJ_ID_UF_FILE_ID              0x01     //文件标识码
#define MOBJ_ID_UF_TRANS_CTRL_BYTE      0x02     //文件传输控制
#define MOBJ_ID_UF_TRANS_RSP_FLAG       0x03     //文件数据包应答
#define MOBJ_ID_UF_CURRENT_BLOCK_SN     0x04     //文件数据块序号
#define MOBJ_ID_UF_FILE_BLOCK           0x05     //文件数据块

//监控参量列表，保存在NV Memory中
//MCM-23_20061108_zhangjie_begin
#define MAX_PARAM_COUNT    500
//MCM-23_20061108_zhangjie_end
typedef struct
{
    USHORT usParamCount;     //参量的个数
    USHORT ausParamID[MAX_PARAM_COUNT];
}MOBJ_ID_TBL_ST;

//设备信息
typedef struct
{
    UCHAR  ucMnftId;                    //设备厂商代码                                    
    UCHAR  ucDevType;                   //设备类别                                        
    CHAR   acDevModel[STR_LEN];         //设备型号                                        
    CHAR   acDevSn[STR_LEN];            //设备生产序列号                                  
    UCHAR  ucChCount;                   //设备的实际信道总数                              
    CHAR   acLongitude[STR_LEN];        //经度                                            
    CHAR   acLatitude[STR_LEN];         //纬度                                            
    CHAR   acSfwrVer[STR_LEN];          //监控版本信息                                       
    CHAR   acDevModel2[LONG_STR_LEN];   //3G:设备型号
    UCHAR  ucRunningMode;               //3G:嵌入式软件运行模式，只反应当时的状态，不用保存
    USHORT usMaxApcLen;                 //3G:可支持AP：C协议的最大长度                       
    UCHAR  ucMcpbMode;                  //3G:MCP：B采用的交互机制                            
    UCHAR  ucNc;                        //3G:连发系数（NC）                                  
    UCHAR  ucT1;                        //3G:设备响应超时（TOT1）                            
    USHORT usTg;                        //3G:发送间隔时间（TG）（此数据在NC＞1时才有意义）   
    UCHAR  ucTp;                        //3G:暂停传输等待时间（TP）                          
    USHORT usTimeToSwitch;              //3G:转换到软件升级模式时，OMC需要等待的时间         
    UCHAR  ucUpgradeResult;             //3G:设备执行软件升级的结果，不用保存                          
    UCHAR  ucUpgradeMode;               //3G:设备使用的远程升级方式                          
}DEV_INFO_SET_ST;

//网管参数
typedef struct
{
    ULONG  ulStaNum;                    //站点编号                          
    UCHAR  ucDevNum;                    //设备编号                    
    CHAR   acSmcAddr[TEL_NUM_LEN];      //短信服务中心号码              
    CHAR   acQnsTelNum1[TEL_NUM_LEN];   //查询/设置电话号码1～5       
    CHAR   acQnsTelNum2[TEL_NUM_LEN];                              
    CHAR   acQnsTelNum3[TEL_NUM_LEN];                              
    CHAR   acQnsTelNum4[TEL_NUM_LEN];                              
    CHAR   acQnsTelNum5[TEL_NUM_LEN];                              
    CHAR   acReportTelNum[TEL_NUM_LEN]; //上报号码                    
    UCHAR  aucEmsIpAddr[IP_ADDR_LEN];   //监控中心IP地址（IP v4）     
    USHORT usEmsPortNum;                //监控中心IP地址端口号        
    UCHAR  ucUseGprs;                   //3G:设备是否使用GPRS方式，已经被删除
    CHAR   acGprsApn[STR_LEN];          //3G:GPRS接入点名称（即：APN）
    USHORT usHeartbeatPeriod;           //3G:设备的心跳包间隔时间     
    CHAR   acDevTelNum[TEL_NUM_LEN];    //3G:设备的电话号码            
    CHAR   acGprsUserId[STR_LEN];       //3G:GPRS参数：用户标识       
    CHAR   acGprsPwd[STR_LEN];          //3G:GPRS参数：口令           
    UCHAR  ucReportCommMode;            //上报通信方式
    UCHAR  ucCommMode;                  //通信方式                                  //规范20060810
    UCHAR  ucReportType;                //上报类型，不用保存
    UCHAR  aucDateTime[DATE_TIME_LEN];  //日期、时间                       
    UCHAR  aucFtpServerIpAddr[IP_ADDR_LEN];   //3G:FTP服务器IP地址（IP v4），不用保存
    USHORT usFtpServerPortNum;          //3G:FTP服务器IP地址端口号，不用保存
    CHAR   acFtpUserId[STR_LEN];        //3G:FTP升级操作的用户名，不用保存
    CHAR   acFtpPwd[STR_LEN];           //3G:FTP升级操作的口令，不用保存
    CHAR   acFtpPath[LONG_STR_LEN];     //3G:相对路径，不用保存
    CHAR   acFileName[LONG_STR_LEN];    //3G:文件名，不用保存
}NM_PARAM_SET_ST;   

//设置参数
typedef struct
{
    UCHAR ucRfSw;           //射频信号开关状态

    UCHAR ucPaSw1;          //功放开关状态1～8    bit型
    UCHAR ucPaSw2;          
    UCHAR ucPaSw3;          
    UCHAR ucPaSw4;          
    UCHAR ucPaSw5;          
    UCHAR ucPaSw6;          
    UCHAR ucPaSw7;          
    UCHAR ucPaSw8;          

    USHORT usWorkChNum1;    //工作信道号1～16 uint2型    
    USHORT usWorkChNum2;    
    USHORT usWorkChNum3;    
    USHORT usWorkChNum4;    
    USHORT usWorkChNum5;    
    USHORT usWorkChNum6;    
    USHORT usWorkChNum7;    
    USHORT usWorkChNum8;    
    USHORT usWorkChNum9;    
    USHORT usWorkChNum10;   
    USHORT usWorkChNum11;   
    USHORT usWorkChNum12;   
    USHORT usWorkChNum13;   
    USHORT usWorkChNum14;   
    USHORT usWorkChNum15;   
    USHORT usWorkChNum16;   

    USHORT usShfChNum1;     //移频信道号1～16 uint2型     
    USHORT usShfChNum2;     
    USHORT usShfChNum3;     
    USHORT usShfChNum4;     
    USHORT usShfChNum5;     
    USHORT usShfChNum6;     
    USHORT usShfChNum7;     
    USHORT usShfChNum8;     
    USHORT usShfChNum9;     
    USHORT usShfChNum10;    
    USHORT usShfChNum11;    
    USHORT usShfChNum12;    
    USHORT usShfChNum13;    
    USHORT usShfChNum14;    
    USHORT usShfChNum15;    
    USHORT usShfChNum16;    

    USHORT usWorkUbChNum;   //工作频带的上边带信道号         
    USHORT usWorkLbChNum;   //工作频带的下边带信道号         
    USHORT usShfUbChNum;    //移频频带的上边带信道号         
    USHORT usShfLbChNum;    //移频频带的下边带信道号         
                                 
    USHORT usWorkUbChNum1;  //工作频带的上边带信道号（通道1） 
    USHORT usWorkLbChNum1;  //工作频带的下边带信道号（通道1） 
    USHORT usWorkUbChNum2;  //工作频带的上边带信道号（通道2） 
    USHORT usWorkLbChNum2;  //工作频带的下边带信道号（通道2） 
    USHORT usWorkUbChNum3;  //工作频带的上边带信道号（通道3） 
    USHORT usWorkLbChNum3;  //工作频带的下边带信道号（通道3） 
    USHORT usWorkUbChNum4;  //工作频带的上边带信道号（通道4） 
    USHORT usWorkLbChNum4;  //工作频带的下边带信道号（通道4）
                                 
    UCHAR  ucUlAtt;         //上行衰减值
    UCHAR  ucDlAtt;         //下行衰减值
    UCHAR  ucUlAtt1;        //上行衰减值（通道1），目前驱动层将通道1的参数映射到不带通道号的对应的参数上            
    UCHAR  ucDlAtt1;        //下行衰减值（通道1），目前驱动层将通道1的参数映射到不带通道号的对应的参数上            
    UCHAR  ucUlAtt2;        //上行衰减值（通道2）            
    UCHAR  ucDlAtt2;        //下行衰减值（通道2）            
    UCHAR  ucUlAtt3;        //上行衰减值（通道3）            
    UCHAR  ucDlAtt3;        //下行衰减值（通道3）            
    UCHAR  ucUlAtt4;        //上行衰减值（通道4）            
    UCHAR  ucDlAtt4;        //下行衰减值（通道4）            
                                 
    UCHAR  ucDlSwrThr;      //下行驻波比门限                 
    CHAR   cPaOverheatThr;  //功放过温度告警门限             
    USHORT usSrcCellId;     //信源小区识别码参照值           

    CHAR   cDlInUnderPwrThr;  //下行输入功率欠功率门限
    CHAR   cDlInOverPwrThr;   //下行输入功率过功率门限
    CHAR   cDlOutUnderPwrThr; //下行输出功率欠功率门限
    CHAR   cDlOutOverPwrThr;  //下行输出功率过功率门限
    CHAR   cUlOutOverPwrThr;  //上行输出功率过功率门限

    USHORT usShfUbChNum1;   //移频频带的上边带信道号（通道1）
    USHORT usShfLbChNum1;   //移频频带的下边带信道号（通道1）
    USHORT usShfUbChNum2;   //移频频带的上边带信道号（通道2）
    USHORT usShfLbChNum2;   //移频频带的下边带信道号（通道2）
    USHORT usShfUbChNum3;   //移频频带的上边带信道号（通道3）
    USHORT usShfLbChNum3;   //移频频带的下边带信道号（通道3）
    USHORT usShfUbChNum4;   //移频频带的上边带信道号（通道4）
    USHORT usShfLbChNum4;   //移频频带的下边带信道号（通道4）
                                
    UCHAR  ucRfSw1;         //射频切换开关状态1～6 （载波池）
    UCHAR  ucRfSw2;         
    UCHAR  ucRfSw3;         
    UCHAR  ucRfSw4;         
    UCHAR  ucRfSw5;         
    UCHAR  ucRfSw6;         

    CHAR   cDlInUnderPwrThr1;   //下行输入功率欠功率门限（通道1），目前驱动层将通道1的参数映射到不带通道号的对应的参数上 
    CHAR   cDlInOverPwrThr1;    //下行输入功率过功率门限（通道1）
    CHAR   cDlOutUnderPwrThr1;  //下行输出功率欠功率门限（通道1）
    CHAR   cDlOutOverPwrThr1;   //下行输出功率过功率门限（通道1）
    CHAR   cUlOutOverPwrThr1;   //上行输出功率过功率门限（通道1）
    UCHAR  ucDlSwrThr1;         //下行驻波比门限（通道1）
    UCHAR  ucUlSwrThr1;         //上行驻波比门限（通道1）          //规范20060810
    
    CHAR   cDlInUnderPwrThr2;   //下行输入功率欠功率门限（通道2）
    CHAR   cDlInOverPwrThr2;    //下行输入功率过功率门限（通道2）
    CHAR   cDlOutUnderPwrThr2;  //下行输出功率欠功率门限（通道2）
    CHAR   cDlOutOverPwrThr2;   //下行输出功率过功率门限（通道2）
    CHAR   cUlOutOverPwrThr2;   //上行输出功率过功率门限（通道2）
    UCHAR  ucDlSwrThr2;         //下行驻波比门限（通道2）      
    UCHAR  ucUlSwrThr2;         //上行驻波比门限（通道2）          //规范20060810
                                                                 
    CHAR   cDlInUnderPwrThr3;   //下行输入功率欠功率门限（通道3）
    CHAR   cDlInOverPwrThr3;    //下行输入功率过功率门限（通道3）
    CHAR   cDlOutUnderPwrThr3;  //下行输出功率欠功率门限（通道3）
    CHAR   cDlOutOverPwrThr3;   //下行输出功率过功率门限（通道3）
    CHAR   cUlOutOverPwrThr3;   //上行输出功率过功率门限（通道3）
    UCHAR  ucDlSwrThr3;         //下行驻波比门限（通道3）   
    UCHAR  ucUlSwrThr3;         //上行驻波比门限（通道3）          //规范20060810 
                                                                 
    CHAR   cDlInUnderPwrThr4;   //下行输入功率欠功率门限（通道4）
    CHAR   cDlInOverPwrThr4;    //下行输入功率过功率门限（通道4）
    CHAR   cDlOutUnderPwrThr4;  //下行输出功率欠功率门限（通道4）
    CHAR   cDlOutOverPwrThr4;   //下行输出功率过功率门限（通道4）
    CHAR   cUlOutOverPwrThr4;   //上行输出功率过功率门限（通道4）
    UCHAR  ucDlSwrThr4;         //下行驻波比门限（通道4）
    UCHAR  ucUlSwrThr4;         //上行驻波比门限（通道4）          //规范20060810

    SHORT  sDlInUnderPwrThr;    //3G:下行输入功率欠功率门限，sint2型，单位为dBm，比例为10，以下同
    SHORT  sDlInOverPwrThr;     //3G:下行输入功率过功率门限                                      
    SHORT  sDlOutUnderPwrThr;   //3G:下行输出功率欠功率门限                                        
    SHORT  sDlOutOverPwrThr;    //3G:下行输出功率过功率门限                                      
    SHORT  sUlOutOverPwrThr;    //3G:上行输出功率过功率门限
    SHORT  sUlOutUnderPwrThr;   //3G:上行输出功率欠功率门限        //规范20060810
    UCHAR  ucUlSwrThr;          //3G:上行驻波比门限                //规范20060810
    
                               
    SHORT  sDlInUnderPwrThr1;   //3G:下行输入功率欠功率门限（通道1）
    SHORT  sDlInUnderPwrThr2;   //3G:下行输入功率欠功率门限（通道2）
    SHORT  sDlInUnderPwrThr3;   //3G:下行输入功率欠功率门限（通道3）
    SHORT  sDlInUnderPwrThr4;   //3G:下行输入功率欠功率门限（通道4）
                               
    SHORT  sDlInOverPwrThr1;    //3G:下行输入功率过功率门限（通道1）
    SHORT  sDlInOverPwrThr2;    //3G:下行输入功率过功率门限（通道2）
    SHORT  sDlInOverPwrThr3;    //3G:下行输入功率过功率门限（通道3）
    SHORT  sDlInOverPwrThr4;    //3G:下行输入功率过功率门限（通道4）
                               
    SHORT  sDlOutUnderPwrThr1;  //3G:下行输出功率欠功率门限（通道1）
    SHORT  sDlOutUnderPwrThr2;  //3G:下行输出功率欠功率门限（通道2）
    SHORT  sDlOutUnderPwrThr3;  //3G:下行输出功率欠功率门限（通道3）
    SHORT  sDlOutUnderPwrThr4;  //3G:下行输出功率欠功率门限（通道4）
                               
    SHORT  sDlOutOverPwrThr1;   //3G:下行输出功率过功率门限（通道1）
    SHORT  sDlOutOverPwrThr2;   //3G:下行输出功率过功率门限（通道2）
    SHORT  sDlOutOverPwrThr3;   //3G:下行输出功率过功率门限（通道3）
    SHORT  sDlOutOverPwrThr4;   //3G:下行输出功率过功率门限（通道4）
                               
    SHORT  sUlOutOverPwrThr1;   //3G:上行输出功率过功率门限（通道1）
    SHORT  sUlOutOverPwrThr2;   //3G:上行输出功率过功率门限（通道2）
    SHORT  sUlOutOverPwrThr3;   //3G:上行输出功率过功率门限（通道3）
    SHORT  sUlOutOverPwrThr4;   //3G:上行输出功率过功率门限（通道4）

    SHORT  sUlOutUnderPwrThr1;   //3G:上行输出功率欠功率门限（通道1）    //规范20060810
    SHORT  sUlOutUnderPwrThr2;   //3G:上行输出功率欠功率门限（通道2）    //规范20060810
    SHORT  sUlOutUnderPwrThr3;   //3G:上行输出功率欠功率门限（通道3）    //规范20060810
    SHORT  sUlOutUnderPwrThr4;   //3G:上行输出功率欠功率门限（通道4）    //规范20060810

//------------------------------以下为POI新增参数-----------------------------------

    CHAR  cCdma800InUnderPwrThr;// CDMA800输入功率欠功率门限
    CHAR  cCdma800InOverPwrThr; // CDMA800输入功率过功率门限

    CHAR  cMbGsmInUnderPwrThr;  // 移动GSM输入功率欠功率门限
    CHAR  cMbGsmInOverPwrThr;   // 移动GSM输入功率过功率门限

    CHAR  cUcGsmInUnderPwrThr;  // 联通GSM输入功率欠功率门限
    CHAR  cUcGsmInOverPwrThr;   // 联通GSM输入功率过功率门限

    CHAR  cMbDcsInUnderPwrThr;  //移动DCS输入功率欠功率门限
    CHAR  cMbDcsInOverPwrThr;   // 移动DCS输入功率过功率门限

    CHAR  cUcDcsInUnderPwrThr;  // 联通DCS输入功率欠功率门限
    CHAR  cUcDcsInOverPwrThr;   // 联通DCS输入功率过功率门限

    CHAR  c3G1FDDInUnderPwrThr; // 3G1-FDD输入功率欠功率门限
    CHAR  c3G1FDDInOverPwrThr;  // 3G1-FDD输入功率过功率门限

    CHAR  c3G2FDDInUnderPwrThr; // 3G2-FDD输入功率欠功率门限
    CHAR  c3G2FDDInOverPwrThr;  // 3G2-FDD输入功率过功率门限

    CHAR  c3G3TDDInUnderPwrThr; // 3G3-TDD输入功率欠功率门限
    CHAR  c3G3TDDInOverPwrThr;  // 3G3-TDD输入功率过功率门限

    CHAR  cTrunkInUnderPwrThr;  // 集群系统输入功率欠功率门限
    CHAR  cTrunkInOverPwrThr;   // 集群系统输入功率过功率门限

//------------------------------以上为POI新增参数-----------------------------------
    
}SETTING_PARAM_SET_ST;                   
                   
//实时采样参数，带通道的有超出范围标志的参数暂时不考虑，如果需要今后扩充
typedef struct
{
    CHAR   cPaTemp;             //功放温度值    sint1型，单位为℃              
    CHAR   cDlInPwr;            //下行输入功率电平  sint1型，单位为dBm
    UCHAR  ucDlInPwrFlag;       //下行输入功率电平超出范围的标志
    CHAR   cDlOutPwr;           //下行输出功率电平  sint1型，单位为dBm
    UCHAR  ucDlOutPwrFlag;      //下行输出功率电平超出范围的标志
    CHAR   cUlTheGain;          //上行理论增益  sint1型，单位为dB              
    CHAR   cDlActGain;          //下行实际增益  sint1型，单位为dB              
    UCHAR  ucDlActGainFlag;     //下行实际增益超出范围的标志
    UCHAR  ucDlSwr;             //下行驻波比值  UCHAR型，比例为10  
    UCHAR  ucDlSwrFlag;         //下行驻波比值超出范围的标志
    UCHAR  ucUlSwr;             //上行驻波比值  UCHAR型，比例为10          //规范20060810
    UCHAR  ucUlSwrFlag;         //上行驻波比值超出范围的标志               //规范20060810
    UCHAR  ucSsMnc;             //信源信息：运营商代码  UCHAR型                
    USHORT usSsLac;             //信源信息：位置区编码  uint2型                
    UCHAR  ucSsBsic;            //信源信息：基站识别码  UCHAR型                
    USHORT usSsBcch;            //信源信息：BCCH绝对载频号  uint2型            
    CHAR   cSsBcchRxLev;        //信源信息：BCCH接收电平    sint1型，单位为dBm 
    UCHAR  ucSsBcchRxLevFlag;   //BCCH接收电平超出范围的标志
    USHORT usSsCi;              //信源信息：小区识别码实时值    uint2型        
    CHAR   cUlOutPwr;           //上行输出功率电平  sint1型，单位为dBm         
    UCHAR  ucUlOutPwrFlag;      //上行输出功率电平超出范围的标志
    CHAR   cOptRxPwr;           //光收功率  sint1型，单位为dBm                 
    CHAR   cOptTxPwr;           //光发功率  sint1型，单位为dBm

//由于目前没有使用带通道的参数，因此这些参数的超出范围标志都没有定义

    CHAR   cDlInPwr1;           //下行输入功率电平（通道1） sint1型，单位为dBm 
    CHAR   cDlOutPwr1;          //下行输出功率电平（通道1） sint1型，单位为dBm 
    CHAR   cUlTheGain1;         //上行理论增益（通道1） sint1型，单位为dB      
    CHAR   cDlActGain1;         //下行实际增益（通道1） sint1型，单位为dB      
    CHAR   cUlOutPwr1;          //上行输出功率电平（通道1） sint1型，单位为dBm 
    UCHAR  ucDlSwr1;            //下行驻波比值（通道1） UCHAR型，比例为10      
                                               
    CHAR   cDlInPwr2;           //下行输入功率电平（通道2） sint1型，单位为dBm 
    UCHAR  ucDlInPwrFlag2;      //下行输入功率电平（通道2） 超出范围的标志
    CHAR   cDlOutPwr2;          //下行输出功率电平（通道2） sint1型，单位为dBm 
    UCHAR  ucDlOutPwrFlag2;     //下行输出功率电平（通道2） 超出范围的标志
    CHAR   cUlTheGain2;         //上行理论增益（通道2） sint1型，单位为dB      
    CHAR   cDlActGain2;         //下行实际增益（通道2） sint1型，单位为dB      
    UCHAR  ucDlActGainFlag2;    //下行实际增益（通道2） 超出范围的标志
    CHAR   cUlOutPwr2;          //上行输出功率电平（通道2） sint1型，单位为dBm 
    UCHAR  ucUlOutPwrFlag2;     //上行输出功率电平（通道2） 超出范围的标志
    UCHAR  ucDlSwr2;            //下行驻波比值（通道2） UCHAR型，比例为10      
    UCHAR  ucDlSwrFlag2;        //下行驻波比值（通道2） 超出范围的标志
                                               
    CHAR   cDlInPwr3;           //下行输入功率电平（通道3） sint1型，单位为dBm 
    CHAR   cDlOutPwr3;          //下行输出功率电平（通道3） sint1型，单位为dBm 
    CHAR   cUlTheGain3;         //上行理论增益（通道3） sint1型，单位为dB      
    CHAR   cDlActGain3;         //下行实际增益（通道3） sint1型，单位为dB      
    CHAR   cUlOutPwr3;          //上行输出功率电平（通道3） sint1型，单位为dBm 
    UCHAR  ucDlSwr3;            //下行驻波比值（通道3） UCHAR型，比例为10      
                                               
    CHAR   cDlInPwr4;           //下行输入功率电平（通道4） sint1型，单位为dBm 
    CHAR   cDlOutPwr4;          //下行输出功率电平（通道4） sint1型，单位为dBm 
    CHAR   cUlTheGain4;         //上行理论增益（通道4） sint1型，单位为dB      
    CHAR   cDlActGain4;         //下行实际增益（通道4） sint1型，单位为dB      
    CHAR   cUlOutPwr4;          //上行输出功率电平（通道4） sint1型，单位为dBm 
    UCHAR  ucDlSwr4;            //下行驻波比值（通道4） UCHAR型，比例为10      
                                               
    UCHAR  ucUlBypassStatus;    //上行信号旁路状态  bit型，0表示正常，1表示旁路
    UCHAR  ucDlBypassStatus;    //下行信号旁路状态  bit型，0表示正常，1表示旁路 

    SHORT  sDlInPwr;            //3G:下行输入功率电平，sint2型，单位为dBm，比例为10，以下同
    UCHAR  uc3GDlInPwrFlag;     //下行输入功率电平超出范围的标志
    SHORT  sDlOutPwr;           //3G:下行输出功率电平
    UCHAR  uc3GDlOutPwrFlag;    //下行输出功率电平超出范围的标志
    SHORT  sUlOutPwr;           //3G:上行输出功率电平
    UCHAR  uc3GUlOutPwrFlag;    //上行输出功率电平超出范围的标志

//由于目前没有使用带通道的参数，因此这些参数的超出范围标志都没有定义

    SHORT  sDlInPwr1;           //3G:下行输入功率电平（通道1～通道4）
    SHORT  sDlInPwr2;
    SHORT  sDlInPwr3;
    SHORT  sDlInPwr4;

    SHORT  sDlOutPwr1;          //3G:下行输出功率电平（通道1～通道4）
    SHORT  sDlOutPwr2;
    UCHAR  uc3GDlOutPwrFlag2;   //3G:下行输出功率电平（通道2）超出范围的标志
    SHORT  sDlOutPwr3;
    SHORT  sDlOutPwr4;

    SHORT  sUlOutPwr1;          //3G:上行输出功率电平（通道1～通道4）
    SHORT  sUlOutPwr2;
    SHORT  sUlOutPwr3;
    SHORT  sUlOutPwr4;

    SHORT  sDonorCpichLev;      //3G:施主端口CPICH功率电平
    SHORT  sServiceCpichLev;    //3G:用户端口CPICH功率电平
    
    //--------- 以下为林玮添加 ---------
    USHORT usModuleBatValue;    //监控模块电池电压，没有对应的参量ID
    //--------- 以上为林玮添加 --------- 
    //zhangjie 20071120 太阳能蓄电池电压
    USHORT usSecBatVol;
    //zhangjie 20071120 太阳能蓄电池电压

//------------------------------以下为POI新增参数-----------------------------------

    CHAR  cCdma800InPwr;        //CDMA800输入功率电平值
    UCHAR ucCdma800InPwrFlag;   //超出范围的标志
    CHAR  cMbGsmInPwr;          //移动GSM输入功率电平值
    UCHAR uccMbGsmInPwrFlag;    //超出范围的标志
    CHAR  cUcGsmInPwr;          //联通GSM输入功率电平值
    UCHAR uccUcGsmInPwrFlag;    //超出范围的标志
    CHAR  cMbDcsInPwr;          //移动DCS输入功率电平值
    UCHAR uccMbDcsInPwrFlag;    //超出范围的标志
    CHAR  cUcDcsInPwr;          //联通DCS输入功率电平值
    UCHAR uccUcDcsInPwrFlag;    //超出范围的标志
    CHAR  c3G1FDDInPwr;         //3G1-FDD输入功率电平值
    UCHAR ucc3G1FDDInPwrFlag;   //超出范围的标志
    CHAR  c3G2FDDInPwr;         //3G2-FDD输入功率电平值
    UCHAR ucc3G2FDDInPwrFlag;   //超出范围的标志
    CHAR  c3G3TDDInPwr;         //3G3-TDD输入功率电平值
    UCHAR ucc3G3TDDInPwrFlag;   //超出范围的标志
    CHAR  cTrunkInPwr;          //集群系统输入功率电平值
    UCHAR uccTrunkInPwrFlag;    //超出范围的标志

//------------------------------以上为POI新增参数-----------------------------------
    
}RC_PARAM_SET_ST;                   


//告警使能
typedef struct   
{                               
    UCHAR ucPwrLost;           //电源掉电告警                
    UCHAR ucPwrFault;          //电源故障告警                
    UCHAR ucSolarBatLowPwr;    //太阳能电池低电压告警        
    UCHAR ucBatFalut;          //监控模块电池故障告警        
    UCHAR ucPosition;          //位置告警                    
    UCHAR ucPaOverheat;        //功放过温告警                
    UCHAR ucSrcSigChanged;     //信源变化告警                
    UCHAR ucOtherModule;       //其它模块告警                
    UCHAR ucOscUnlocked;       //本振失锁告警                
    UCHAR ucUlLna;             //上行低噪放故障告警          
    UCHAR ucDlLna;             //下行低噪放故障告警          
    UCHAR ucUlPa;              //上行功放告警                
    UCHAR ucDlPa;              //下行功放告警                
    UCHAR ucOptModule;         //光收发模块故障告警          
    UCHAR ucMsLink;            //主从监控链路告警            
    UCHAR ucDlInOverPwr;       //下行输入过功率告警          
    UCHAR ucDlInUnderPwr;      //下行输入欠功率告警          
    UCHAR ucDlOutOverPwr;      //下行输出过功率告警          
    UCHAR ucDlOutUnderPwr;     //下行输出欠功率告警          
    UCHAR ucDlSwr;             //下行驻波比告警              
    UCHAR ucUlOutOverPwr;      //上行输出过功率告警
    UCHAR ucUlOutUnderPwr;     //上行输出欠功率告警 //规范20060810
    UCHAR ucUlSwr;             //上行驻波比告警    //规范20060810
    UCHAR ucExt1;              //外部告警1～8                
    UCHAR ucExt2;                                            
    UCHAR ucExt3;                                            
    UCHAR ucExt4;                                            
    UCHAR ucExt5;                                            
    UCHAR ucExt6;                                            
    UCHAR ucExt7;                                            
    UCHAR ucExt8;                                            
    UCHAR ucDoor;              //门禁告警                    
    UCHAR ucSelfOsc;           //自激告警                    
    UCHAR ucGprsLoginFailed;   //3G:GPRS登录失败告警，已经被删除

    UCHAR ucDlInOverPwr1;      //下行输入过功率告警（通道1） 
    UCHAR ucDlInUnderPwr1;     //下行输入欠功率告警（通道1） 
    UCHAR ucDlOutOverPwr1;     //下行输出过功率告警（通道1） 
    UCHAR ucDlOutUnderPwr1;    //下行输出欠功率告警（通道1） 
    UCHAR ucUlOutOverPwr1;     //上行输出过功率告警（通道1） 
    UCHAR ucDlSwr1;            //下行驻波比告警    （通道1）     

    UCHAR ucDlInOverPwr2;      //下行输入过功率告警（通道2） 
    UCHAR ucDlInUnderPwr2;     //下行输入欠功率告警（通道2） 
    UCHAR ucDlOutOverPwr2;     //下行输出过功率告警（通道2） 
    UCHAR ucDlOutUnderPwr2;    //下行输出欠功率告警（通道2） 
    UCHAR ucUlOutOverPwr2;     //上行输出过功率告警（通道2） 
    UCHAR ucDlSwr2;            //下行驻波比告警    （通道2）     

    UCHAR ucDlInOverPwr3;      //下行输入过功率告警（通道3） 
    UCHAR ucDlInUnderPwr3;     //下行输入欠功率告警（通道3） 
    UCHAR ucDlOutOverPwr3;     //下行输出过功率告警（通道3） 
    UCHAR ucDlOutUnderPwr3;    //下行输出欠功率告警（通道3） 
    UCHAR ucUlOutOverPwr3;     //上行输出过功率告警（通道3） 
    UCHAR ucDlSwr3;            //下行驻波比告警    （通道3）     

    UCHAR ucDlInOverPwr4;      //下行输入过功率告警（通道4） 
    UCHAR ucDlInUnderPwr4;     //下行输入欠功率告警（通道4） 
    UCHAR ucDlOutOverPwr4;     //下行输出过功率告警（通道4） 
    UCHAR ucDlOutUnderPwr4;    //下行输出欠功率告警（通道4） 
    UCHAR ucUlOutOverPwr4;     //上行输出过功率告警（通道4） 
    UCHAR ucDlSwr4;            //下行驻波比告警    （通道4）     

//MCM-25_20061108_zhonghw_begin
    UCHAR ucUlOutUnderPwr1;    //上行输出欠功率告警（通道1）      //规范20060810
    UCHAR ucUlOutUnderPwr2;    //上行输出欠功率告警（通道2）      //规范20060810
    UCHAR ucUlOutUnderPwr3;    //上行输出欠功率告警（通道3）      //规范20060810  
    UCHAR ucUlOutUnderPwr4;    //上行输出欠功率告警（通道4）     //规范20060810
    UCHAR ucUlSwr1;            //上行驻波比告警    （通道1）      //规范20060810
    UCHAR ucUlSwr2;            //上行驻波比告警    （通道2）      //规范20060810      
    UCHAR ucUlSwr3;            //上行驻波比告警    （通道3）      //规范20060810          
    UCHAR ucUlSwr4;            //上行驻波比告警    （通道4）     //规范20060810     
//MCM-25_20061108_zhonghw_end

    UCHAR ucTta1;              //塔顶放大器告警使能1～8      
    UCHAR ucTta2;                                            
    UCHAR ucTta3;                                            
    UCHAR ucTta4;                                            
    UCHAR ucTta5;                                            
    UCHAR ucTta6;             
    UCHAR ucTta7;                                    
    UCHAR ucTta8;                                    
    UCHAR ucPaBypass1;         //功放旁路告警使能1～8
    UCHAR ucPaBypass2;                               
    UCHAR ucPaBypass3;                               
    UCHAR ucPaBypass4;                               
    UCHAR ucPaBypass5;                               
    UCHAR ucPaBypass6;                               
    UCHAR ucPaBypass7;                               
    UCHAR ucPaBypass8;                               
    UCHAR ucSelfOscShutdown;   //3G:自激关机保护告警    
    UCHAR ucSelfOscReduceAtt;  //3G:自激降增益保护告警  

//以上共74项
//------------------------------以下为POI新增参数，共15项-----------------------------------

    UCHAR  ucCdma800InOverPwr;  //CDMA800输入过功率告警
    UCHAR  ucCdma800InUnderPwr; //CDMA800输入欠功率告警

    UCHAR  ucMbGsmInOverPwr;    //移动GSM输入过功率告警
    UCHAR  ucMbGsmInUnderPwr;   //移动GSM输入欠功率告警

    UCHAR  ucUcGsmInOverPwr;    //联通GSM输入过功率告警
    UCHAR  ucUcGsmInUnderPwr;   //联通GSM/输入欠功率告警

    UCHAR  ucMbDcsInOverPwr;    //移动DCS输入过功率告警
    UCHAR  ucMbDcsInUnderPwr;   //移动DCS输入欠功率告警

    UCHAR  ucUcDcsInOverPwr;    //联通DCS输入过功率告警
    UCHAR  ucUcDcsInUnderPwr;   //联通DCS输入欠功率告警

    UCHAR  uc3G1FDDInOverPwr;   //3G1-FDD输入过功率告警
    UCHAR  uc3G1FDDInUnderPwr;  //3G1-FDD输入欠功率告警

    UCHAR  uc3G2FDDInOverPwr;   //3G2-FDD输入过功率告警
    UCHAR  uc3G2FDDInUnderPwr;  //3G2-FDD输入欠功率告警

    UCHAR  uc3G3TDDInOverPwr;   //3G3-TDD输入过功率告警
    UCHAR  uc3G3TDDInUnderPwr;  //3G3-TDD输入欠功率告警

    UCHAR  ucTrunkInOverPwr;    //集群系统输入过功率告警
    UCHAR  ucTrunkInUnderPwr;   //集群系统输入欠功率告警

//------------------------------以上为POI新增参数，共15项-----------------------------------

//------------------------------以下为太阳能控制器新增参数，共15项----------------------------

    UCHAR ucSecStBatBlowout;   //蓄电池保险丝断
    UCHAR ucSecSelfResumeBlowout;//输出自恢复保险丝断
    UCHAR ucSecOutputOverCur;  //输出过流, over current
    UCHAR ucSecCircuitFault;   //控制电路故障
    UCHAR ucSecVol24fault;     //24V负载故障
    UCHAR ucSecVol12Fault;     //12V负载故障
    UCHAR ucSecVol5Fault;      //5V负载故障
    UCHAR ucSecUseUnderVol;    //欠压使用提示
    UCHAR ucSecOverVol;        //蓄电池过压告警
    UCHAR ucSecOverDischarging;//蓄电池预过放告警
    UCHAR ucSecOverDischarged; //蓄电池过放告警
    UCHAR ucSecStBatFault;     //蓄电池故障
    UCHAR ucSecSBFault;        //硅板故障
    UCHAR ucSecDoor;           //门禁告警
    UCHAR ucSecAleak;          //水漏告警

//------------------------------以上为太阳能控制器新增参数，共15项----------------------------


//--------- 以下两项内部使用，用于产生自激降增益告警和关机保护告警，因此必须放在最后，否则会导致ID错位
    UCHAR ucUlSelfOsc;         //上行自激告警，没有对应的参量ID
    UCHAR ucDlSelfOsc;         //下行自激告警，没有对应的参量ID

}ALARM_ITEM_BYTE_ST; 

typedef union
{
    UCHAR aucAlarmEnableArray[1];
    ALARM_ITEM_BYTE_ST  stAlarmEnableStruct;
}ALARM_ENABLE_UN;

typedef union
{
    UCHAR aucAlarmStatusArray[1];
    ALARM_ITEM_BYTE_ST stAlarmStatusStruct;
}ALARM_STATUS_UN;

//每个告警项包含以下信息
typedef struct
{
    UCHAR ucIDLoByte;

    UCHAR ucSupportedBit : 1; //表示是否支持该告警项，0：否，1：是，由设备类型决定
  //UCHAR ucEnabledBit   : 1; //表示使能状态，0:使能关，1:使能开
    UCHAR ucEffectiveBit : 1; //表示是否有效，当使能开且未被屏蔽时有效，否则无效
  //UCHAR ucCenterStatus : 1; //表示中心告警状态，0:正常,1:告警
    UCHAR ucLocalStatus  : 1; //表示本地告警状态，0:正常,1:告警，这是查询时获得的告警状态
    UCHAR ucRcStatus     : 1; //告警实时采集状态，0:正常,1:告警
    UCHAR ucIsInstant    : 1; //表示立即告警，不需要进行告警计数

    UCHAR ucTotalCount;   //本告警项已经采集的次数
    UCHAR ucChangedCount; //本告警项相对于本地告警状态变化的次数
}ALARM_ITEM_INFO_ST;


//告警状态
typedef struct   
{                               
    ALARM_ITEM_INFO_ST stPwrLost;           //电源掉电告警                
    ALARM_ITEM_INFO_ST stPwrFault;          //电源故障告警                
    ALARM_ITEM_INFO_ST stSolarBatLowPwr;    //太阳能电池低电压告警        
    ALARM_ITEM_INFO_ST stBatFalut;          //监控模块电池故障告警        
    ALARM_ITEM_INFO_ST stPosition;          //位置告警                    
    ALARM_ITEM_INFO_ST stPaOverheat;        //功放过温告警                
    ALARM_ITEM_INFO_ST stSrcSigChanged;     //信源变化告警                
    ALARM_ITEM_INFO_ST stOtherModule;       //其它模块告警                
    ALARM_ITEM_INFO_ST stOscUnlocked;       //本振失锁告警                
    ALARM_ITEM_INFO_ST stUlLna;             //上行低噪放故障告警          
    ALARM_ITEM_INFO_ST stDlLna;             //下行低噪放故障告警          
    ALARM_ITEM_INFO_ST stUlPa;              //上行功放告警                
    ALARM_ITEM_INFO_ST stDlPa;              //下行功放告警                
    ALARM_ITEM_INFO_ST stOptModule;         //光收发模块故障告警          
    ALARM_ITEM_INFO_ST stMsLink;            //主从监控链路告警            
    ALARM_ITEM_INFO_ST stDlInOverPwr;       //下行输入过功率告警          
    ALARM_ITEM_INFO_ST stDlInUnderPwr;      //下行输入欠功率告警          
    ALARM_ITEM_INFO_ST stDlOutOverPwr;      //下行输出过功率告警          
    ALARM_ITEM_INFO_ST stDlOutUnderPwr;     //下行输出欠功率告警          
    ALARM_ITEM_INFO_ST stDlSwr;             //下行驻波比告警              
    ALARM_ITEM_INFO_ST stUlOutOverPwr;      //上行输出过功率告警          
    ALARM_ITEM_INFO_ST stUlOutUnderPwr;     //上行输出欠功率告警                //规范20060810
    ALARM_ITEM_INFO_ST stUlSwr;             //上行驻波比告警                    //规范20060810
    ALARM_ITEM_INFO_ST stExt1;              //外部告警1～8                
    ALARM_ITEM_INFO_ST stExt2;                                            
    ALARM_ITEM_INFO_ST stExt3;                                            
    ALARM_ITEM_INFO_ST stExt4;                                            
    ALARM_ITEM_INFO_ST stExt5;                                            
    ALARM_ITEM_INFO_ST stExt6;                                            
    ALARM_ITEM_INFO_ST stExt7;                                            
    ALARM_ITEM_INFO_ST stExt8;                                            
    ALARM_ITEM_INFO_ST stDoor;              //门禁告警                    
    ALARM_ITEM_INFO_ST stSelfOsc;           //自激告警                    
    ALARM_ITEM_INFO_ST stGprsLoginFailed;   //3G:GPRS登录失败告警，已经被删除

    ALARM_ITEM_INFO_ST stDlInOverPwr1;      //下行输入过功率告警 （通道1） 
    ALARM_ITEM_INFO_ST stDlInUnderPwr1;     //下行输入欠功率告警 （通道1） 
    ALARM_ITEM_INFO_ST stDlOutOverPwr1;     //下行输出过功率告警 （通道1） 
    ALARM_ITEM_INFO_ST stDlOutUnderPwr1;    //下行输出欠功率告警 （通道1） 
    ALARM_ITEM_INFO_ST stUlOutOverPwr1;     //上行输出过功率告警 （通道1） 
    ALARM_ITEM_INFO_ST stDlSwr1;            //下行驻波比告警     （通道1）     

    ALARM_ITEM_INFO_ST stDlInOverPwr2;      //下行输入过功率告警 （通道2） 
    ALARM_ITEM_INFO_ST stDlInUnderPwr2;     //下行输入欠功率告警 （通道2） 
    ALARM_ITEM_INFO_ST stDlOutOverPwr2;     //下行输出过功率告警 （通道2） 
    ALARM_ITEM_INFO_ST stDlOutUnderPwr2;    //下行输出欠功率告警 （通道2） 
    ALARM_ITEM_INFO_ST stUlOutOverPwr2;     //上行输出过功率告警 （通道2） 
    ALARM_ITEM_INFO_ST stDlSwr2;            //下行驻波比告警     （通道2）     

    ALARM_ITEM_INFO_ST stDlInOverPwr3;      //下行输入过功率告警 （通道3） 
    ALARM_ITEM_INFO_ST stDlInUnderPwr3;     //下行输入欠功率告警 （通道3） 
    ALARM_ITEM_INFO_ST stDlOutOverPwr3;     //下行输出过功率告警 （通道3） 
    ALARM_ITEM_INFO_ST stDlOutUnderPwr3;    //下行输出欠功率告警 （通道3） 
    ALARM_ITEM_INFO_ST stUlOutOverPwr3;     //上行输出过功率告警 （通道3） 
    ALARM_ITEM_INFO_ST stDlSwr3;            //下行驻波比告警     （通道3）     

    ALARM_ITEM_INFO_ST stDlInOverPwr4;      //下行输入过功率告警 （通道4） 
    ALARM_ITEM_INFO_ST stDlInUnderPwr4;     //下行输入欠功率告警 （通道4） 
    ALARM_ITEM_INFO_ST stDlOutOverPwr4;     //下行输出过功率告警 （通道4） 
    ALARM_ITEM_INFO_ST stDlOutUnderPwr4;    //下行输出欠功率告警 （通道4） 
    ALARM_ITEM_INFO_ST stUlOutOverPwr4;     //上行输出过功率告警 （通道4） 
    ALARM_ITEM_INFO_ST stDlSwr4;            //下行驻波比告警     （通道4）     
//MCM-25_20061108_zhonghw_begin
    ALARM_ITEM_INFO_ST stUlOutUnderPwr1;    //上行输出欠功率告警（通道1）        //规范20060810
    ALARM_ITEM_INFO_ST stUlOutUnderPwr2;    //上行输出欠功率告警（通道2）        //规范20060810
    ALARM_ITEM_INFO_ST stUlOutUnderPwr3;    //上行输出欠功率告警（通道3）        //规范20060810    
    ALARM_ITEM_INFO_ST stUlOutUnderPwr4;     //上行输出欠功率告警（通道4）       //规范20060810
    ALARM_ITEM_INFO_ST stUlSwr1;            //上行驻波比告警    （通道1）        //规范20060810    
    ALARM_ITEM_INFO_ST stUlSwr2;            //上行驻波比告警    （通道2）        //规范20060810
    ALARM_ITEM_INFO_ST stUlSwr3;            //上行驻波比告警    （通道3）        //规范20060810    
    ALARM_ITEM_INFO_ST stUlSwr4;             //上行驻波比告警    （通道4）       //规范20060810
//MCM-25_20061108_zhonghw_end

    ALARM_ITEM_INFO_ST stTta1;              //塔顶放大器告警使能1～8      
    ALARM_ITEM_INFO_ST stTta2;                                            
    ALARM_ITEM_INFO_ST stTta3;                                            
    ALARM_ITEM_INFO_ST stTta4;                                            
    ALARM_ITEM_INFO_ST stTta5;                                            
    ALARM_ITEM_INFO_ST stTta6;             
    ALARM_ITEM_INFO_ST stTta7;                                    
    ALARM_ITEM_INFO_ST stTta8;                                    
    ALARM_ITEM_INFO_ST stPaBypass1;         //功放旁路告警使能1～8
    ALARM_ITEM_INFO_ST stPaBypass2;                               
    ALARM_ITEM_INFO_ST stPaBypass3;                               
    ALARM_ITEM_INFO_ST stPaBypass4;                               
    ALARM_ITEM_INFO_ST stPaBypass5;                               
    ALARM_ITEM_INFO_ST stPaBypass6;                               
    ALARM_ITEM_INFO_ST stPaBypass7;                               
    ALARM_ITEM_INFO_ST stPaBypass8;                               
    ALARM_ITEM_INFO_ST stSelfOscShutdown;   //3G:自激关机保护告警    
    ALARM_ITEM_INFO_ST stSelfOscReduceAtt;  //3G:自激降增益保护告警  

//以上共74项
//------------------------------以下为POI新增参数，共18项-----------------------------------

    ALARM_ITEM_INFO_ST stCdma800InOverPwr;  // CDMA800输入过功率告警
    ALARM_ITEM_INFO_ST stCdma800InUnderPwr; // CDMA800输入欠功率告警

    ALARM_ITEM_INFO_ST stMbGsmInOverPwr;    // 移动GSM输入过功率告警
    ALARM_ITEM_INFO_ST stMbGsmInUnderPwr;   // 移动GSM输入欠功率告警

    ALARM_ITEM_INFO_ST stUcGsmInOverPwr;    // 联通GSM输入过功率告警
    ALARM_ITEM_INFO_ST stUcGsmInUnderPwr;   // 联通GSM输入欠功率告警

    ALARM_ITEM_INFO_ST stMbDcsInOverPwr;    // 移动DCS输入过功率告警
    ALARM_ITEM_INFO_ST stMbDcsInUnderPwr;   // 移动DCS输入欠功率告警

    ALARM_ITEM_INFO_ST stUcDcsInOverPwr;    // 联通DCS输入过功率告警
    ALARM_ITEM_INFO_ST stUcDcsInUnderPwr;   // 联通DCS输入欠功率告警

    ALARM_ITEM_INFO_ST st3G1FDDInOverPwr;   // 3G1-FDD输入过功率告警
    ALARM_ITEM_INFO_ST st3G1FDDInUnderPwr;  // 3G1-FDD输入欠功率告警

    ALARM_ITEM_INFO_ST st3G2FDDInOverPwr;   // 3G2-FDD输入过功率告警
    ALARM_ITEM_INFO_ST st3G2FDDInUnderPwr;  // 3G2-FDD输入欠功率告警

    ALARM_ITEM_INFO_ST st3G3TDDInOverPwr;   // 3G3-TDD输入过功率告警
    ALARM_ITEM_INFO_ST st3G3TDDInUnderPwr;  // 3G3-TDD输入欠功率告警

    ALARM_ITEM_INFO_ST stTrunkInOverPwr;    // 集群系统输入过功率告警
    ALARM_ITEM_INFO_ST stTrunkInUnderPwr;   // 集群系统输入欠功率告警

//------------------------------以上为POI新增参数，共18项-----------------------------------

//------------------------------以下为太阳能控制器新增参数，共15项----------------------------

    ALARM_ITEM_INFO_ST stSecStBatBlowout;   //蓄电池保险丝断
    ALARM_ITEM_INFO_ST stSecSelfResumeBlowout;//输出自恢复保险丝断
    ALARM_ITEM_INFO_ST stSecOutputOverCur;  //输出过流, over current
    ALARM_ITEM_INFO_ST stSecCircuitFault;   //控制电路故障
    ALARM_ITEM_INFO_ST stSecVol24fault;     //24V负载故障
    ALARM_ITEM_INFO_ST stSecVol12Fault;     //12V负载故障
    ALARM_ITEM_INFO_ST stSecVol5Fault;      //5V负载故障
    ALARM_ITEM_INFO_ST stSecUseUnderVol;    //欠压使用提示
    ALARM_ITEM_INFO_ST stSecOverVol;        //蓄电池过压告警
    ALARM_ITEM_INFO_ST stSecOverDischarging;//蓄电池预过放告警
    ALARM_ITEM_INFO_ST stSecOverDischarged; //蓄电池过放告警
    ALARM_ITEM_INFO_ST stSecStBatFault;     //蓄电池故障
    ALARM_ITEM_INFO_ST stSecSBFault;        //硅板故障
    ALARM_ITEM_INFO_ST stSecDoor;           //门禁告警
    ALARM_ITEM_INFO_ST stSecAleak;          //水漏告警

//------------------------------以上为太阳能控制器新增参数，共15项----------------------------

//--------- 以下两项内部使用，用于产生自激降增益告警和关机保护告警，因此必须放在最后，否则会导致ID错位
    ALARM_ITEM_INFO_ST stUlSelfOsc;         //上行自激告警，没有对应的参量ID
    ALARM_ITEM_INFO_ST stDlSelfOsc;         //下行自激告警，没有对应的参量ID
    
}ALARM_ITEM_SET_ST;

#define MAX_ALARM_ITEMS                 ((sizeof(ALARM_ITEM_SET_ST)) / sizeof(ALARM_ITEM_INFO_ST))


//程序开始运行时将首先初始化所有告警项的ID和状态，如果需要遍历告警项，
//则使用数组方式访问，如果需要针对某项告警操作，则使用结构方式访问。
//编码时可以直接获得告警项的ID，解码时可以通过二分法查找对应ID的告警项。
//在数组方式访问到告警项时，可以使用下面的结构访问告警项的内容。
typedef union
{
    ALARM_ITEM_INFO_ST astAlarmItemArray[1];
    ALARM_ITEM_SET_ST  stAlarmItemStruct;
}ALARM_ITEM_UN;

//需要保存到NV Memory中的监控信息
#ifndef M3
#define ADDR_CFG_FILE_BASE       (0x1180000)
#else
#define ADDR_CFG_FILE_BASE       (0)
#endif

//只读参数放在一个sector中
#ifndef M3
#define ADDR_MOBJ_ID_TBL         (ADDR_CFG_FILE_BASE + 0)
#define ADDR_DEV_INFO_SET        (ADDR_CFG_FILE_BASE + 0x800)
#else
#define ADDR_MOBJ_ID_TBL         ((ULONG)&g_stMObjIdTbl)
#define ADDR_DEV_INFO_SET        ((ULONG)&g_stDevInfoSet)
#endif

//可写参数放在一个sector中
#ifndef M3
#define ADDR_NM_PARAM_SET        (ADDR_CFG_FILE_BASE + 0x1000)
#define ADDR_ALARM_ENABLE_SET    (ADDR_CFG_FILE_BASE + 0x1400)
#define ADDR_SETTING_PARAM_SET   (ADDR_CFG_FILE_BASE + 0x1800)
#else
#define ADDR_NM_PARAM_SET        ((ULONG)&g_stNmParamSet)
#define ADDR_ALARM_ENABLE_SET    ((ULONG)&g_uAlarmEnable)
#define ADDR_SETTING_PARAM_SET   ((ULONG)&g_stSettingParamSet)
#endif

//告警中心状态单独放在一个sector中
#ifndef M3
#define ADDR_CENTER_STATUS_SET   (ADDR_CFG_FILE_BASE + 0x2000)
#else
#define ADDR_CENTER_STATUS_SET   ((ULONG)&g_uCenterStatus)
#endif

//和远程升级相关的放在一个sector中
#ifndef M3
#define ADDR_UPGRADE_INFO_SET    (ADDR_CFG_FILE_BASE + 0x3000)
#define ADDR_FILE_PART_TBL       (ADDR_CFG_FILE_BASE + 0x3800)
#else
#define ADDR_UPGRADE_INFO_SET    ((ULONG)&g_stUpgradeCtx)
#define ADDR_FILE_PART_TBL       ((ULONG)&g_astFilePartTbl)
#endif

//厂家私有参数放在一个sector中
#ifndef M3
#define ADDR_YKPP_PARAM_SET   (ADDR_CFG_FILE_BASE + 0x4000)
#else
#define ADDR_YKPP_PARAM_SET   ((ULONG)&g_stYkppParamSet)
#endif

//GPRS通信方式时本地和中心的连接状态
typedef struct
{
    UCHAR ucLinkStatus;
    UCHAR ucLoginStatus;
    UCHAR ucConnAttempCnt;      //连接尝试建立连接的次数，每失败3次就上报通信链路故障
    UCHAR ucLoginTimeoutCnt;    //登录超时的次数，超时2次即断开连接重新登录
    UCHAR ucNoRspCnt;           //心跳超时的次数，超时2次即断开连接重新登录    
}CENTER_CONN_STATUS_ST;

#define COMM_STATUS_DISCONNECTED    0 //未连接，表示GPRS未连接，或者TCP未连接
#define COMM_STATUS_CONNECTED       1 //已连接，表示GPRS连接成功，且TCP连接成功

#define COMM_STATUS_LOGOUT          2 //未登录
#define COMM_STATUS_LOGIN           3 //已登录

#define MAX_CONN_ATTEMP_CNT         3
#define MAX_LOGIN_TO_CNT            2
#define MAX_HEARTBEAT_TO_CNT        2

//***************************3G机型定义******************  
#define WIDE_BAND                              51    //宽带直放站
#define WIRELESS_FRESELT                       52    //选频直放站
#define OPTICAL_DIRECOUPL_LOCAL                53    //光纤直放站直接耦合近端机
#define OPTICAL_WIDEBAND_FAR                   54    //光纤直放站宽带远端机
#define TRUNK_AMPLIFIER                        55    //干线放大器 
#define FRESHIFT_FRESELT_FAR                   56    //移频直放站选频远端机
#define FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL      57    //移频直放站直接耦合宽带近端机
#define FRESHIFT_WIRELESS_WIDEBAND_LOCAL       62    //移频直放站无线耦合宽带近端机
#define OPTICAL_FRESELT_FAR                    63    //光纤直放站选频远端机
#define OPTICAL_CARRIER_WIDEBAND_LOCAL         64    //光纤载波池宽带基站端机
#define OPTICAL_CARRIER_FRESELT_LOCAL          65    //光纤载波池选频基站端机
#define OPTICAL_CARRIER_WIDEBAND_FAR           66    //光纤载波池宽带远端机
#define OPTICAL_CARRIER_FRESELT_FAR            67    //光纤载波池选频远端机
#define OPTICAL_WIRELESSCOUPL_LOCAL            68    //光纤直放站无线耦合近端机
#define FRESHIFT_WIDEBAND_FAR                  69    //移频直放站宽带远端机
#define FRESHIFT_DIRECOUPL_FRESELT_LOCAL       70    //移频直放站直接耦合选频近端机
#define FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL   71    //移频直放站无线耦合选频近端机
#define BS_AMPLIFIER                           72    //基站放大器 
#define POI_STAKEOUT                           90    //POI专用设备类型

#define DEV_WITH_SEC                           100   //附带太阳能控制器
//***************************3G机型定义******************
//***************************2G机型定义******************
#define WIDE_BAND_2G                               1    //宽带直放站
#define WIRELESS_FRESELT_2G                        2    //选频直放站
#define OPTICAL_DIRECOUPL_LOCAL_2G                 3    //光纤直放站直接耦合近端机
#define OPTICAL_WIDEBAND_FAR_2G                    4    //光纤直放站宽带远端机
#define TRUNK_AMPLIFIER_2G                         5    //干线放大器
#define FRESHIFT_FRESELT_FAR_2G                    6    //移频直放站选频远端机
#define FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL_2G       7    //移频直放站直接耦合宽带近端机

//MCM-63_20070315_Zhonghw_begin
#define BI_FRE_OPTICAL_FAR_2G                      8    //双频光纤直放站远端机
#define BI_FRE_OPTICAL_DIRECOUPL_LOCAL_2G          9    //双频光纤直放站直接耦合近端机
#define BI_DIRECT_WIDE_BAND_TOWER_AMP_2G          10    //双向宽带塔放
#define BI_DIRECT_FRE_SELECT_TOWER_AMP_2G         11    //双向选频塔放
//MCM-63_20070315_Zhonghw_end
#define FRESHIFT_WIRELESS_WIDEBAND_LOCAL_2G       12    //移频直放站无线耦合宽带近端机
#define OPTICAL_FRESELT_FAR_2G                    13    //光纤直放站选频远端机
#define OPTICAL_CARRIER_WIDEBAND_LOCAL_2G         14    //光纤载波池宽带基站端机
#define OPTICAL_CARRIER_FRESELT_LOCAL_2G          15    //光纤载波池选频基站端机
#define OPTICAL_CARRIER_WIDEBAND_FAR_2G           16    //光纤载波池宽带远端机
#define OPTICAL_CARRIER_FRESELT_FAR_2G            17    //光纤载波池选频远端机
#define OPTICAL_WIRELESSCOUPL_LOCAL_2G            18    //光纤直放站无线耦合近端机
#define FRESHIFT_WIDEBAND_FAR_2G                  19    //移频直放站宽带远端机
#define FRESHIFT_DIRECOUPL_FRESELT_LOCAL_2G       20    //移频直放站直接耦合选频近端机
#define FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL_2G   21    //移频直放站无线耦合选频近端机
#define BS_AMPLIFIER_2G                           22    //基站放大器 
//***************************2G机型定义******************
//****************************测试机型*******************
#define MACHINE_FOR_TEST                          99
//****************************测试机型*******************

//MCM-11_20061101_zhangjie_begin
//机型中"选频类型"定义
#define FREQTYPE_WIDEBAND           0   //宽带型
#define FREQTYPE_FRESELT            1   //选频型
#define FREQTYPE_FRESHIFT           2   //移频型
#define FREQTYPE_FRESHIFT_WIDEBAND  3   //移频宽带型

//机型中"2G3G"定义
#define GEN_TYPE_2G         0           //2G机型
#define GEN_TYPE_3G         1           //3G机型

//机型中"通信主从机"定义
#define COMM_ROLE_MASTER    0           //通信主机
#define COMM_ROLE_SLAVE     1           //通信从机

//机型中"射频主从机"定义
#define RF_MASTER    0
#define RF_SLAVE     1

//机型中"信道号类型"定义
#define CH_SHIFT_NOTHING    0           //工作和移频信道号都没有
#define ONLY_CH             1           //只有工作信道号
#define ONLY_SHIFT          2           //只有移频信道号
#define CH_SHIFT            3           //工作和移频信道号都有

//机型中"光纤机类型"定义
#define OPTICAL_NO          0           //非光纤机
#define OPTICAL_LOCAL       1           //光纤近端
#define OPTICAL_REMOTE      2           //光纤远端

//机型中"显示器采用的串口类型"定义
#define UI_USART_0          0           //显示器接串口0
#define UI_USART_1          1           //显示器接串口1
#define UI_USART_2          2           //显示器接串口2

typedef struct
{
    UCHAR  ucSetFreqType;               //选频类型
    UCHAR  ucGenType;                   //2G或者3G
    UCHAR  ucCommRoleType;              //通信主从机
    UCHAR  ucRFRoleType;                //射频主从机
    UCHAR  ucChannelType;               //信道号类型
    UCHAR  ucOpticalType;               //光纤机类型
    UCHAR  ucIsCoupling;                //直接耦合机型
    UCHAR  ucUIUSARTType;               //显示器采用的串口类型
}DEV_TYPE_TABLE_ST;
//MCM-11_20061101_zhangjie_end

#endif
