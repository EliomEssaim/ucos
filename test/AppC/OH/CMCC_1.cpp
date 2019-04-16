/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   CMCC_1.cpp
    作者:     林雨
    文件说明: 本文件的实现中国移动监控规范南向接口协议的定义
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
              把原代码中涉及机型判断的地方全部改为对机型配置表中某
              项或者多项值的判断。
    3. 日期:  2006/11/01
       作者:  章杰
       描述:  修改问题MCM-5，
              在程序中所有判断通信方式或者上报通信方式时只使用
              "通信方式"这个ID，并当设置上报通信时，均把设置上
              报通信的值映射到通信方式上，这样就使得2G和3G设备
              在判断通信方式上得到了统一。 
    4. 日期:  2006/11/3
       作者:  钟华文
       描述:  修改问题MCM-10
              在处理缓存完所有异步操作的参数后，为操作请求申请上
              下文,并注册不同的操作对象的后续处理、超时处理函数。\
    5. 日期:  2006/11/7
       作者:  章杰
       描述:  修改问题MCM-21，
              修改笔误，把宏定义中的FIRELESSCOUPL 改为WIRELESSCOUPL 
    6. 日期:  2006/11/8
       作者:  章杰
       描述:  修改问题MCM-24，
              在修改机型后的初始化参量列表时，对要写入的参量列表做内存的越界判断
    7. 日期:  2006/11/8
       作者:  林雨
       描述:  修改问题MCM-26，
              初始化时调用监控参量列表长度计算函数并更新长度指示单元的值
    8. 日期:  2006/11/9
       作者:  钟华文
       描述:  修改问题MCM-28，
              将厂家协议处理相关函数从CMCC.cpp移至YKPP.cpp
    9. 日期:  2006/11/10
       作者:  章杰
       描述:  修改问题MCM-29，
              在初始化时给下行输入一个正常的值
    10. 日期:  2006/11/14
        作者:  钟华文
        描述:  修改问题MCM-32，
               接到切换监控版本的命令时根据程序运行情况配置相应的标志位，
               使此时未运行的程序被bootloader下次启动时选择运行。在协议
               层数据处理完毕后的外层，调用重启函数使设备重启。
    13. 日期:  2007/01/05
        作者:  钟华文
        描述:  修改问题MCM-50，
               当由于异常引起文件升级中断时，根据实际情况修正文件升级信息；
               当写一个FLASH的SECTOR成功后，及时保存文件升级信息。
    14. 日期:  2007/03/15
        作者:  钟华文
        描述:  修改问题MCM-63
               增加参数处理支持2g所有设备类型               
    15. 日期:  2007/03/15
        作者:  钟华文
        描述:  修改问题MCM-64，              
               在"DevTypeTableInit"中引用厂家参数中的"通信主从机"参数对
               设备类型配置表中的通信主从机标志作相应的修改。
    16. 日期:  2007/04/03
        作者:  钟华文
        描述:  修改问题MCM-72
               上电初始化过程中如果发现监控板未被初始化，则先初始化站点编号和设备编号为0
    17. 日期:  2007/06/14
        作者:  钟华文
        描述:  修改问题MCM-74，              
               修改设备类型后更新配置时，将"设备的实际信道总数"设置成2
    18. 日期:  2007/11/20
        作者:  章杰
        描述:  修改问题MCM-80，              
               根据设备类型选择“太阳能蓄电池电压”的检测方式。
---------------------------------------------------------------------------*/

//****************************包含头文件**************************//
#ifdef M3
#include "Queue.h" //M3中使用CheckAlarmStatus()中的队列操作
#endif

#include <string.h>
#include "../../Periph/flash/flash.h"
#include "../../include/boot.h"
#include "../MM/MemMgmt.h"
#include "../Util/Util.h"
#include "../include/Interface.h"
#include "../Timer/Timer.h"
#include "../AppMain.h"
#include "OH.h"
#include "YKPP.h"
#include "CMCC_DevConfig.cpp"
#include "CMCC_1.h"

#ifdef UNIT_TEST
#include "../../UT/Stub/Stub.h"
#endif

//*****************************宏定义*****************************//

/*使用AP:A，则除去起始结束单元的原始数据最大为256字节，除去各层开销，数据单元长
  度最大为256-15=241字节，扣除上报类型，还剩239字节
  使用AP:B(SMS)，则除去起始结束单元的原始数据最大为(140-2)/2=69字节，除去各层开
  销，数据单元长度最大为69-15=54字节，扣除上报类型，还剩50字节
  每个告警对象4字节，则前者最多可以包含59个告警，后者最多可以包含12个告警
*/
#define REPORT_LEN_EXCEPT_MOBJ_CONTENT  17
#define MAX_ALARM_ITEM_COUNT_BY_APB     12
#define MAX_ALARM_ITEM_COUNT_BY_APA     59
#define MAX_MOBJ_ID_COUNT_BY_APB        24
#define MAX_MOBJ_ID_COUNT_BY_APA        119
#define MAX_MOBJ_ID_COUNT_BY_APC        200 //???需要通过支持的最大的AP:C长度来决定


#define SWAP_WORD(word)     ((word) = ADJUST_WORD((word)))
#define SWAP_DWORD(dword)   ((dword)= ADJUST_DWORD((dword)))


//设置MCP层监控参量的错误代码
#define SetErrCode(ucMObjIdHiByte, ucErr, ucFlag) \
    ((ucMObjIdHiByte) = (UCHAR)(((ucMObjIdHiByte) & 0x0F) | (ucErr << 4))), (ucFlag) = RSP_FLAG_PARTIALLY_DONE

//通信包标识只循环计数低字节，这样高字节可以灵活作为其它用途，从而保证标识不会重
//复，例如如果高字节如果为1则表明是器件产生的告警
#define GEN_PACKET_ID() (++g_ucPacketIdLowByte + 0x8000)
#define GET_PACKET_ID() (g_ucPacketIdLowByte + 0x8000)

//用于在设置时置标志和判断标志
USHORT g_usSettingParamBitmap = 0;
#define BITMAP_RF_SW                (1 << 0)
#define BITMAP_CH_NUM               (1 << 1)
#define BITMAP_ATT                  (1 << 2)
#define BITMAP_SMC_ADDR             (1 << 3)
#define BITMAP_MODIFY_IP_ADDR       (1 << 4)
#define BITMAP_MODIFY_PORT_NUM      (1 << 5)
#define BITMAP_MODIFY_DEV_TYPE      (1 << 6)
#define BITMAP_MODIFY_MOB_LIST      (1 << 7)
#define BITMAP_MODIFY_COMM_MODE     (1 << 8)
#define BITMAP_MODIFY_CH_COUNT      (1 << 9)
//MCM-32_20061114_zhonghw_begin
#define BITMAP_SWITCH_FILE_VER      (1 << 10)
//MCM-32_20061114_zhonghw_end

#define FIRST_NO_TIMEOUT                0    //从未超时
#define FIRST_1_SHORT_TIMEOUT           1    //第一轮第1次短超时
#define FIRST_2_SHORT_TIMEOUT           2    //第一轮第2次短超时
#define FIRST_WAIT_FOR_LONG_TIMEOUT     0xF  //等待第一轮长超时
#define SECOND_NO_TIMEOUT               0x10 //第一轮长超时
#define SECOND_1_SHORT_TIMEOUT          0x11 //第二轮第1次短超时
#define SECOND_2_SHORT_TIMEOUT          0x12 //第二轮第2次短超时
#define SECOND_WAIT_FOR_LONG_TIMEOUT    0x1F //等待第二轮长超时
#define THIRD_NO_TIMEOUT                0x20 //第二轮长超时
#define THIRD_1_SHORT_TIMEOUT           0x21 //第三轮第1次短超时
#define THIRD_2_SHORT_TIMEOUT           0x22 //第三轮第2次短超时

//**************************全局变量定义**************************//

//产生通信包标识，0x8000～0x8FFF则用于由设备发起的通信
UCHAR g_ucPacketIdLowByte = 0; //通信包标识的低字节

//监控参量列表
MOBJ_ID_TBL_ST g_stMObjIdTbl;

//对应的全局变量
DEV_INFO_SET_ST      g_stDevInfoSet;
NM_PARAM_SET_ST      g_stNmParamSet;
SETTING_PARAM_SET_ST g_stSettingParamSet;

//实时采样参数，不需要掉电保存
RC_PARAM_SET_ST g_stRcParamSet;


//告警使能
ALARM_ENABLE_UN g_uAlarmEnable;

//中心告警状态
ALARM_STATUS_UN g_uCenterStatus;

//所有告警信息
ALARM_ITEM_UN g_uAlarmItems;

//告警发送缓冲区
ALARM_ITEM_BUF_ST g_stAlarmItemBuf;


//告警上报操作的上下文
#define MAX_ALARM_REPORTS  6
ALARM_REPORT_CTX_ST g_astAlarmReportCtx[MAX_ALARM_REPORTS];

//其它上报的上下文
OTHER_REPORT_CTX_ST g_astOtherReportCtx[OTHER_REPORT_COUNT];


//用于程序升级的全局变量，需要保存在NV Memory中
FILE_PART_DESC_ST g_astFilePartTbl[FPD_COUNT]; //前两个用于程序文件，最后一个用于数据文件

//升级文件的上下文
UPGRADE_CTX_ST g_stUpgradeCtx;

//监控参量处理表
MOBJ_HNDLR_TBL_ST g_astDevInfoHndlrTbl[DEV_INFO_HNDLR_TBL_SIZE];
MOBJ_HNDLR_TBL_ST g_astDevInfoExHndlrTbl[DEV_INFO_HNDLR_TBL_SIZE];
MOBJ_HNDLR_TBL_ST g_astNmParamHndlrTbl[NM_PARAMS_HNDLR_TBL_SIZE];
ALARM_ITEM_HNDLR_TBL_ST g_astAlarmEnaHndlrTbl[MAX_ALARM_ITEMS];
ALARM_ITEM_HNDLR_TBL_ST g_astAlarmStatusHndlrTbl[MAX_ALARM_ITEMS];
MOBJ_HNDLR_TBL_ST g_astSettingParamHndlrTbl[SETTING_PARAMS_HNDLR_TBL_SIZE];
MOBJ_HNDLR_TBL_ST g_astRtCollParamHndlrTbl[RT_COLL_PARAMS_HNDLR_TBL_SIZE];

//GPRS通信方式时本地和中心的连接状态
CENTER_CONN_STATUS_ST g_stCenterConnStatus;


//用于AP:A和AP:C转义的字符集
typedef struct
{
    UCHAR ucEscape;
    UCHAR aucBeforeEscape[2];
    UCHAR aucAfterEscape[2];
}APAC_ESCAPE_ST;

const APAC_ESCAPE_ST g_stApacEscape = 
{
    0x5E,
    {0x5E, 0x7E},
    {0x5D, 0x7D}
};

//机型配置表
DEV_TYPE_TABLE_ST g_stDevTypeTable;

//**************************全局变量申明**************************//
#ifndef M3
extern const FLASH_DEV_ST g_stNorFlash0; //用于操作FLASH
extern UCHAR g_ucCurrCommMode; //当前的通信状态，需要CH来初始化
#else
const FLASH_DEV_ST g_stNorFlash0 = {0};
UCHAR g_ucCurrCommMode; //当前的通信状态，需要CH来初始化
#include "../../Stub/M3Stub.h"
#endif
extern YKPP_PARAM_SET_ST g_stYkppParamSet;


//用于异步操作的上下文
extern ASYN_OP_CTX_ST g_astAsynOpCtx[ASYN_OP_CTX_CNT];
extern UCHAR g_ucAsynOpMsgID; //用于发起方分配消息ID，只取低7位，最高位固定为0

extern ASYN_OP_BUF_ST g_stSECParam;
extern ASYN_OP_BUF_ST g_stTDParam;

//****************************函数申明****************************//
Timer* OHCreateTimer(void *pvCtx, ULONG ulInterval, ULONG *pulMagicNum);
void OHRemoveTimer(Timer *pTimer, ULONG ulMagicNum);
void InitMObjHndlrTbl();

extern "C" void Restart(void);
extern "C" void SetAtt(void);
extern "C" void SetRfSw(void);
extern "C" void SetFreq(void);
extern "C" void RtcGetDateTime(UCHAR *);
extern "C" LONG RtcSetDateTime(UCHAR *);

extern LONG BuildYkppPdu(YKPP_PDU_ST * pstPdu, ULONG ulPduLen);
LONG HandleYkppData(UCHAR *pucData, ULONG &ulDataLen, UCHAR ucCommMode, UCHAR &ucDir);
LONG ExtractYkppData(UCHAR *pucInput, ULONG ulInputLen, UCHAR *&pucOutput, ULONG &ulOutputLen);
LONG HandleMcpAMObjListWrite(MCPA_MOBJ_ST * pstMObj);
void SaveYkppParam();
//void SaveFileParam();
void ClearAsynOpCtx(UCHAR ucIdx);
void SetReportLampStatus(UCHAR ucLampSta);
void InitYkppDefaultParam();

LONG GetAsynOpCtx(UCHAR &ucIdx);
void DoAsynOpTD(void *pvCtx);
void DoAsynOpSEC(void *pvCtx);

void AsynOpToCommon(void *pvCtx);
//MCM-28_20061109_zhonghw_begin
LONG EncodeAndResponseYkppBuf(OHCH_RECV_DATA_IND_ST * pstPrmv);
//MCM-28_20061109_zhonghw_end

//MCM-32_20061114_zhonghw_begin
LONG SwitchFileVer(void);
LONG HandleSwitchSofVer(MCP_LAYER_ST * pstMcp,LONG);
//MCM-32_20061114_zhonghw_end
/*************************************************
  Function:
  Description:    通过指示灯向用户显示上报(开站、变更等)的结果
  Calls:
  Called By:
  Input:          ucEvent:上报的事件
                  lResult:上报的结果
  Output:         
  Return:         
  Others:         
*************************************************/
void NotifyReportResult(UCHAR ucEvent, LONG lResult) 
{
    UCHAR uclampsta = 0;
    switch(ucEvent)
    {
    case REPORT_TYPE_STA_INIT:
    case REPORT_TYPE_INSPECTION:
    case REPORT_TYPE_REPAIR_CONFIRMED:
    case REPORT_TYPE_CONFIG_CHANGED:
        if(lResult != SUCCEEDED)       //因为亮灯函数接口为0灭1亮
        {
            uclampsta = 1;
        }
        SetReportLampStatus(uclampsta);
        break;
    }

}

/*************************************************
  Function:
  Description:    本函数启动升级过程的定时器
  Calls:
  Called By:
  Input:          ucTimerID:  定时器ID
                  ulInterval: 定时时长
                  g_stUpgradeCtx.pTimer: 升级过程使用的定时器
  Output:         
  Return:         
  Others:         
*************************************************/
LONG StartUpgradeTimer(UCHAR ucTimerID, ULONG ulInterval)
{
    OH_TIMER_CTX_UN uCtx;
    uCtx.stParam.ucTimerID = ucTimerID;
    
    if(g_stUpgradeCtx.pTimer) //如果原来有定时器需要先停止
    {
        OHRemoveTimer(g_stUpgradeCtx.pTimer, g_stUpgradeCtx.ulTimerMagicNum);
        g_stUpgradeCtx.pTimer = 0;
        g_stUpgradeCtx.ulTimerMagicNum = 0;
    }

    g_stUpgradeCtx.pTimer = OHCreateTimer((void *)uCtx.ulParam, ulInterval, 
                                          &g_stUpgradeCtx.ulTimerMagicNum);
    if(g_stUpgradeCtx.pTimer == 0)
    {
        return FAILED;
    }
    else
    {
        return SUCCEEDED;
    }
}

/*************************************************
  Function:
  Description:    本函数停止升级过程的定时器
  Calls:
  Called By:
  Input:          g_stUpgradeCtx.pTimer: 升级过程使用的定时器
  Output:         
  Return:         
  Others:         
*************************************************/
LONG StopUpgradeTimer()
{
    if(g_stUpgradeCtx.pTimer)
    {
        OHRemoveTimer(g_stUpgradeCtx.pTimer, g_stUpgradeCtx.ulTimerMagicNum);
        g_stUpgradeCtx.pTimer = 0;
        g_stUpgradeCtx.ulTimerMagicNum = 0;
    }
    
    return SUCCEEDED;
}

//以下封装FLASH的操作函数，使调用更方便
void SimpleFlashRead(ULONG ulFlashAddr, void *pvData, ULONG ulSize)
{
    FlashRead(g_stNorFlash0.pfwBaseAddr, (UCHAR *)ulFlashAddr, (UCHAR *)pvData, ulSize);
}

LONG SimpleFlashWrite(ULONG ulFlashAddr, void *pvData, ULONG ulSize)
{
    return FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR *)ulFlashAddr, (UCHAR *)pvData, ulSize);
}

LONG SimpleFlashEraseSector(ULONG ulFlashAddr)
{
    return FlashEraseSector(g_stNorFlash0.pfwBaseAddr, (UCHAR *)ulFlashAddr);
}

/*************************************************
  Function:
  Description:    本函数用于比较FLASH中的数据块和RAM中的
                  数据块是否一致，主要用于在写FLASH之前进
                  行判断，如果两者一致则无需写FLASH
  Calls:
  Called By:
  Input:          ulFlashAddr: FLASH的起始地址
                  pvData:      RAM中的起始地址
                  ulSize:      数据块长度
  Output:         
  Return:         SUCCEEDED:   两者一致
                  FAILED:      两者不一致
  Others:         
*************************************************/
LONG CmpData(ULONG ulFlashAddr, void *pvData, ULONG ulSize)
{
    ULONG i = 0;
    UCHAR ucBuf = 0;

    for(i = 0; i < ulSize; i++)
    {
        SimpleFlashRead(ulFlashAddr + i, &ucBuf, 1);
        if(ucBuf != *((UCHAR *)pvData + i))
        {
            return FAILED;
        }        
    }

    return SUCCEEDED;
}


/*************************************************
  Function:
  Description:    本函数用于保存只读参数，包括监控参量
                  列表和设备信息，由于FLASH的最小写单位
                  是4k，而这两组参数放在同一个sector中，
                  因此需要同时更新
  Calls:
  Called By:
  Input:          g_stMObjIdTbl:  监控参量列表
                  g_stDevInfoSet: 设备信息
  Output:         
  Return:         
  Others:         只读参数只能通过扩展命令和厂家命令写入
*************************************************/
void SaveReadOnlyParam()
{
    while(1)
    {
        if(CmpData(ADDR_MOBJ_ID_TBL, &g_stMObjIdTbl, sizeof(g_stMObjIdTbl)) != SUCCEEDED)
        {
            break;
        }

        if(CmpData(ADDR_DEV_INFO_SET, &g_stDevInfoSet, sizeof(g_stDevInfoSet)) != SUCCEEDED)
        {
            break;
        }
        return; //内容不变则直接返回
    }

    if(SimpleFlashEraseSector(ADDR_MOBJ_ID_TBL) != SUCCEEDED)
    {
        return;
    }

    SimpleFlashWrite(ADDR_MOBJ_ID_TBL, &g_stMObjIdTbl, sizeof(g_stMObjIdTbl));
    SimpleFlashWrite(ADDR_DEV_INFO_SET, &g_stDevInfoSet, sizeof(g_stDevInfoSet));
}

/*************************************************
  Function:
  Description:    本函数用于保存中心告警状态
  Calls:
  Called By:
  Input:          g_uCenterStatus: 中心告警状态
  Output:         
  Return:         
  Others:         
*************************************************/
void SaveAlarmCenterStatus()
{
    if(CmpData(ADDR_CENTER_STATUS_SET, &g_uCenterStatus, sizeof(g_uCenterStatus)) == SUCCEEDED)
    {
        return; //内容不变则直接返回
    }
    
    if(SimpleFlashEraseSector(ADDR_CENTER_STATUS_SET) != SUCCEEDED)
    {
        return;
    }

    SimpleFlashWrite(ADDR_CENTER_STATUS_SET, &g_uCenterStatus, sizeof(g_uCenterStatus));
}

/*************************************************
  Function:
  Description:    本函数用于保存可写的监控参量，包括网管
                  参数，设置参数，告警使能参数。这三组参
                  数放在同一个sector中，因此需要同时更新
  Calls:
  Called By:
  Input:          g_stNmParamSet:      网管参数
                  g_stSettingParamSet: 设置参数
                  g_uAlarmEnable:      告警使能参数
  Output:         
  Return:         
  Others:         
*************************************************/
void SaveWritableParam()
{
    while(1)
    {
        if(CmpData(ADDR_NM_PARAM_SET, &g_stNmParamSet, sizeof(g_stNmParamSet)) != SUCCEEDED)
        {
            break;
        }
        if(CmpData(ADDR_SETTING_PARAM_SET, &g_stSettingParamSet, sizeof(g_stSettingParamSet)) != SUCCEEDED)
        {
            break;
        }
        if(CmpData(ADDR_ALARM_ENABLE_SET, &g_uAlarmEnable, sizeof(g_uAlarmEnable)) != SUCCEEDED)
        {
            break;
        }
        return; //内容不变则直接返回
    }

    if(SimpleFlashEraseSector(ADDR_NM_PARAM_SET) != SUCCEEDED)
    {
        return;
    }

    SimpleFlashWrite(ADDR_NM_PARAM_SET, &g_stNmParamSet, sizeof(g_stNmParamSet));
    SimpleFlashWrite(ADDR_SETTING_PARAM_SET, &g_stSettingParamSet, sizeof(g_stSettingParamSet));
    SimpleFlashWrite(ADDR_ALARM_ENABLE_SET, &g_uAlarmEnable, sizeof(g_uAlarmEnable));
}

/*************************************************
  Function:
  Description:    本函数用于保存与升级有关的参数，包
                  括升级信息和文件分区表。这两组参数
                  放在同一个sector中，因此需要同时更新
  Calls:
  Called By:
  Input:          g_stUpgradeCtx:  升级信息
                  g_astFilePartTbl:文件分区表
  Output:         
  Return:         
  Others:         
*************************************************/
void SaveUpgradeInfo()
{
    while(1)
    {
        if(CmpData(ADDR_UPGRADE_INFO_SET, &g_stUpgradeCtx, sizeof(UPGRADE_INFO_SET_ST)) != SUCCEEDED) //注意这两个结构大小不同
        {
            break;
        }
        if(CmpData(ADDR_FILE_PART_TBL, &g_astFilePartTbl, sizeof(g_astFilePartTbl)) != SUCCEEDED)
        {
            break;
        }
        return; //内容不变则直接返回
    }

    if(SimpleFlashEraseSector(ADDR_UPGRADE_INFO_SET) != SUCCEEDED)
    {
        return;
    }

    SimpleFlashWrite(ADDR_UPGRADE_INFO_SET, &g_stUpgradeCtx, sizeof(UPGRADE_INFO_SET_ST)); //注意这两个结构大小不同
    SimpleFlashWrite(ADDR_FILE_PART_TBL, &g_astFilePartTbl, sizeof(g_astFilePartTbl));
}

//调整高低字节顺序，在读出和写入时完成，除非处理这些参数的两个程序使用不同的字节序
void AdjustEndianOfDataFromNVMemory()
{
    SWAP_WORD(g_stDevInfoSet.usMaxApcLen);
    SWAP_WORD(g_stDevInfoSet.usTg);
    SWAP_WORD(g_stDevInfoSet.usTimeToSwitch);
    
    SWAP_WORD(g_stNmParamSet.usEmsPortNum);
    SWAP_WORD(g_stNmParamSet.usFtpServerPortNum);
    SWAP_WORD(g_stNmParamSet.usHeartbeatPeriod);
    SWAP_DWORD(g_stNmParamSet.ulStaNum);

    SWAP_WORD(g_stSettingParamSet.usShfUbChNum);
    SWAP_WORD(g_stSettingParamSet.usShfUbChNum1);
    SWAP_WORD(g_stSettingParamSet.usShfUbChNum2);
    SWAP_WORD(g_stSettingParamSet.usShfUbChNum3);
    SWAP_WORD(g_stSettingParamSet.usShfUbChNum4);

    SWAP_WORD(g_stSettingParamSet.usShfLbChNum);
    SWAP_WORD(g_stSettingParamSet.usShfLbChNum1);
    SWAP_WORD(g_stSettingParamSet.usShfLbChNum2);
    SWAP_WORD(g_stSettingParamSet.usShfLbChNum3);
    SWAP_WORD(g_stSettingParamSet.usShfLbChNum4);

    SWAP_WORD(g_stSettingParamSet.usWorkUbChNum);     
    SWAP_WORD(g_stSettingParamSet.usWorkUbChNum1);
    SWAP_WORD(g_stSettingParamSet.usWorkUbChNum2);
    SWAP_WORD(g_stSettingParamSet.usWorkUbChNum3);
    SWAP_WORD(g_stSettingParamSet.usWorkUbChNum4);

    SWAP_WORD(g_stSettingParamSet.usWorkLbChNum);
    SWAP_WORD(g_stSettingParamSet.usWorkLbChNum1);
    SWAP_WORD(g_stSettingParamSet.usWorkLbChNum2);
    SWAP_WORD(g_stSettingParamSet.usWorkLbChNum3);
    SWAP_WORD(g_stSettingParamSet.usWorkLbChNum4);

    SWAP_WORD(g_stSettingParamSet.usWorkChNum1);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum2);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum3);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum4);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum5);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum6);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum7);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum8);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum9);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum10);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum11);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum12);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum13);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum14);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum15);
    SWAP_WORD(g_stSettingParamSet.usWorkChNum16);

    SWAP_WORD(g_stSettingParamSet.usShfChNum1);
    SWAP_WORD(g_stSettingParamSet.usShfChNum2);
    SWAP_WORD(g_stSettingParamSet.usShfChNum3);
    SWAP_WORD(g_stSettingParamSet.usShfChNum4);
    SWAP_WORD(g_stSettingParamSet.usShfChNum5);
    SWAP_WORD(g_stSettingParamSet.usShfChNum6);
    SWAP_WORD(g_stSettingParamSet.usShfChNum7);
    SWAP_WORD(g_stSettingParamSet.usShfChNum8);
    SWAP_WORD(g_stSettingParamSet.usShfChNum9);
    SWAP_WORD(g_stSettingParamSet.usShfChNum10);
    SWAP_WORD(g_stSettingParamSet.usShfChNum11);
    SWAP_WORD(g_stSettingParamSet.usShfChNum12);
    SWAP_WORD(g_stSettingParamSet.usShfChNum13);
    SWAP_WORD(g_stSettingParamSet.usShfChNum14);
    SWAP_WORD(g_stSettingParamSet.usShfChNum15);
    SWAP_WORD(g_stSettingParamSet.usShfChNum16);

    SWAP_WORD(g_stSettingParamSet.usSrcCellId);
    
    SWAP_WORD(g_stSettingParamSet.sDlInOverPwrThr);
    SWAP_WORD(g_stSettingParamSet.sDlInOverPwrThr1);
    SWAP_WORD(g_stSettingParamSet.sDlInOverPwrThr2);
    SWAP_WORD(g_stSettingParamSet.sDlInOverPwrThr3);
    SWAP_WORD(g_stSettingParamSet.sDlInOverPwrThr4);

    SWAP_WORD(g_stSettingParamSet.sDlInUnderPwrThr);
    SWAP_WORD(g_stSettingParamSet.sDlInUnderPwrThr1);
    SWAP_WORD(g_stSettingParamSet.sDlInUnderPwrThr2);
    SWAP_WORD(g_stSettingParamSet.sDlInUnderPwrThr3);
    SWAP_WORD(g_stSettingParamSet.sDlInUnderPwrThr4);

    SWAP_WORD(g_stSettingParamSet.sDlOutOverPwrThr);
    SWAP_WORD(g_stSettingParamSet.sDlOutOverPwrThr1);
    SWAP_WORD(g_stSettingParamSet.sDlOutOverPwrThr2);
    SWAP_WORD(g_stSettingParamSet.sDlOutOverPwrThr3);
    SWAP_WORD(g_stSettingParamSet.sDlOutOverPwrThr4);

    SWAP_WORD(g_stSettingParamSet.sDlOutUnderPwrThr);
    SWAP_WORD(g_stSettingParamSet.sDlOutUnderPwrThr1);
    SWAP_WORD(g_stSettingParamSet.sDlOutUnderPwrThr2);
    SWAP_WORD(g_stSettingParamSet.sDlOutUnderPwrThr3);
    SWAP_WORD(g_stSettingParamSet.sDlOutUnderPwrThr4);

    SWAP_WORD(g_stSettingParamSet.sUlOutOverPwrThr);
    SWAP_WORD(g_stSettingParamSet.sUlOutOverPwrThr1);
    SWAP_WORD(g_stSettingParamSet.sUlOutOverPwrThr2);
    SWAP_WORD(g_stSettingParamSet.sUlOutOverPwrThr3);
    SWAP_WORD(g_stSettingParamSet.sUlOutOverPwrThr4);
}

/*************************************************
  Function:
  Description:    本函数初始化出厂默认参数,系统监测到设备从来没有被初始化 
                  (通过检查FLASH中相应的初始化位)时调用此函数;或者改变设备
                  类别后会调用此函数，并根据机型配置相应的特殊参数。
  Calls:
  Called By:
  Input:          g_astFilePartTbl: 文件分区表
  Output:
  Return:         
  Others:         本函数供外部调用                  

*************************************************/
void InitCmccDefaultParam()
{
    //设备信息
    g_stDevInfoSet.ucMnftId       = 10;       //厂商代码
    g_stDevInfoSet.usMaxApcLen    = 4096;     //3G:可支持AP：C协议的最大长度
    g_stDevInfoSet.ucMcpbMode     = 1;        //3G:MCP：B采用的交互机制
    g_stDevInfoSet.ucNc           = 1;        //3G:连发系数（NC）
    g_stDevInfoSet.ucT1           = 30;       //3G:设备响应超时（TOT1）
    g_stDevInfoSet.usTg           = 0;        //3G:发送间隔时间（TG）（此数据在NC＞1时才有意义）
    g_stDevInfoSet.ucTp           = 5;        //3G:暂停传输等待时间（TP）
    g_stDevInfoSet.usTimeToSwitch = 30;       //3G:转换到软件升级模式时，OMC需要等待的时间
    g_stDevInfoSet.ucUpgradeMode  = 1;        //3G:设备使用的远程升级方式


    //设置参数(需要渐渐总结)
    g_stSettingParamSet.ucRfSw           = 1;    //射频信号开关
    g_stSettingParamSet.ucDlSwrThr       = 30;   //下行驻波比门限                 
    g_stSettingParamSet.cPaOverheatThr   = 60;   //功放过温度告警门限
    g_stSettingParamSet.cDlInUnderPwrThr = -100; //下行输入功率欠功率门限
    g_stSettingParamSet.cDlInOverPwrThr  = 100;  //下行输入功率过功率门限
    g_stSettingParamSet.cDlOutUnderPwrThr= -100; //下行输出功率欠功率门限
    g_stSettingParamSet.cDlOutOverPwrThr = 100;  //下行输出功率过功率门限
    g_stSettingParamSet.cUlOutOverPwrThr = 100;  //上行输出功率过功率门限

    g_stSettingParamSet.sDlInUnderPwrThr = -400;  //3G:下行输入功率欠功率门限
    g_stSettingParamSet.sDlInOverPwrThr  = 400;   //3G:下行输入功率过功率门限
    g_stSettingParamSet.sDlOutUnderPwrThr= -400;  //3G:下行输出功率欠功率门限
    g_stSettingParamSet.sDlOutOverPwrThr = 400;   //3G:下行输出功率过功率门限
    g_stSettingParamSet.sUlOutOverPwrThr = 400;   //3G:上行输出功率过功率门限
    g_stSettingParamSet.sUlOutUnderPwrThr= -400;  //3G:上行输出功率欠功率门限
    g_stSettingParamSet.ucUlSwrThr       = 30  ;  //3G:上行驻波比门限      

    //MCM-74_20070614_Zhonghw_begin
    g_stDevInfoSet.ucChCount = 2;    //默认为2
    //MCM-74_20070614_Zhonghw_end
    

    //notice告警使能全开
    memset(&g_uAlarmEnable, 1, sizeof(g_uAlarmEnable));

    //中心告警状态全为正常
    memset(&g_uCenterStatus, 0, sizeof(g_uCenterStatus));

    //升级文件的上下文
    memset(&g_stUpgradeCtx, 0, sizeof(g_stUpgradeCtx));
    g_stUpgradeCtx.ulNextBlockSn  = 0;
    //MCM-50_20070105_Zhonghw_begin
    g_stUpgradeCtx.ulUnSaveBlocks = 0;
    //MCM-50_20070105_Zhonghw_end       
    g_stUpgradeCtx.ulMaxBlockSize = 4096;
    g_stUpgradeCtx.ulSavedFileSize= 0;
    g_stUpgradeCtx.lUpgradeResult = 0;


}

void InitAlarmInfo()
{
    ULONG i;

    //初始化g_uAlarmItems中的告警状态，本地状态和中心状态保持一致
    for(i = 0; i < MAX_ALARM_ITEMS; i++)
    {
        if(g_uAlarmEnable.aucAlarmEnableArray[i] > 1)
        {
            g_uAlarmEnable.aucAlarmEnableArray[i] = 0; //将未初始化的使能清零，防止FLASH中的数据有可能为0xFF
        }
        
        if(g_uCenterStatus.aucAlarmStatusArray[i] > 1) //将未初始化的中心状态清零，防止FLASH中的数据有可能为0xFF
        {
            g_uCenterStatus.aucAlarmStatusArray[i] = 0;
        }

        //初始化g_uAlarmItems中的告警状态，本地状态和中心状态保持一致
        g_uAlarmItems.astAlarmItemArray[i].ucLocalStatus = g_uCenterStatus.aucAlarmStatusArray[i];
    
        g_uAlarmItems.astAlarmItemArray[i].ucTotalCount = 0;
        g_uAlarmItems.astAlarmItemArray[i].ucChangedCount = 0;

        //将所有告警项初始化为非立即告警，立即告警将在随后的初始化中置位
        g_uAlarmItems.astAlarmItemArray[i].ucIsInstant = 0;

        //是否支持该参数
        g_uAlarmItems.astAlarmItemArray[i].ucSupportedBit = g_astAlarmStatusHndlrTbl[i].ucIsSupported;

        //初始化告警参数ID低字节
        g_uAlarmItems.astAlarmItemArray[i].ucIDLoByte = g_astAlarmStatusHndlrTbl[i].ucIDLoByte;
    }

    //位置告警、门禁告警和外部告警为立即告警
    g_uAlarmItems.stAlarmItemStruct.stPosition.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stDoor.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt1.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt2.ucIsInstant = 1;    
    g_uAlarmItems.stAlarmItemStruct.stExt3.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt4.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt5.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt6.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt7.ucIsInstant = 1;
    g_uAlarmItems.stAlarmItemStruct.stExt8.ucIsInstant = 1;

    //所有来自太阳能控制器的告警都被视为立即告警
    g_uAlarmItems.stAlarmItemStruct.stSecStBatBlowout.ucIsInstant       = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecSelfResumeBlowout.ucIsInstant  = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecOutputOverCur.ucIsInstant      = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecCircuitFault.ucIsInstant       = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecVol24fault.ucIsInstant         = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecVol12Fault.ucIsInstant         = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecVol5Fault.ucIsInstant          = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecUseUnderVol.ucIsInstant        = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecOverVol.ucIsInstant            = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecOverDischarging.ucIsInstant    = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecOverDischarged.ucIsInstant     = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecStBatFault.ucIsInstant         = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecSBFault.ucIsInstant            = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecDoor.ucIsInstant               = 1;
    g_uAlarmItems.stAlarmItemStruct.stSecAleak.ucIsInstant              = 1;

    //所有来自太阳能控制器的实时告警状态都被设为本地状态
    //太阳能实时状态无法在本机采集到，只能取本地状态为实
    //时状态，以免实时状态和本地状态不一致导致的误上报
    g_uAlarmItems.stAlarmItemStruct.stSecStBatBlowout.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecStBatBlowout.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecSelfResumeBlowout.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecSelfResumeBlowout.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecOutputOverCur.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecOutputOverCur.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecCircuitFault.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecCircuitFault.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecVol24fault.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecVol24fault.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecVol12Fault.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecVol12Fault.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecVol5Fault.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecVol5Fault.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecUseUnderVol.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecUseUnderVol.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecOverVol.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecOverVol.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecOverDischarging.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecOverDischarging.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecOverDischarged.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecOverDischarged.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecStBatFault.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecStBatFault.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecSBFault.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecSBFault.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecDoor.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecDoor.ucLocalStatus;
    g_uAlarmItems.stAlarmItemStruct.stSecAleak.ucRcStatus = 
    g_uAlarmItems.stAlarmItemStruct.stSecAleak.ucLocalStatus;

    //所有来自太阳能控制器的告警使能都被打开
    g_uAlarmEnable.stAlarmEnableStruct.ucSecStBatBlowout       = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecSelfResumeBlowout  = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecOutputOverCur      = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecCircuitFault       = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecVol24fault         = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecVol12Fault         = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecVol5Fault          = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecUseUnderVol        = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecOverVol            = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecOverDischarging    = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecOverDischarged     = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecStBatFault         = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecSBFault            = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecDoor               = 1;
    g_uAlarmEnable.stAlarmEnableStruct.ucSecAleak              = 1;
}

/*************************************************
  Function:
  Description:    本函数对机型配置表进行初始化
  Calls:
  Called By:
  Input:          
  Output:
  Return:         
  Others:         本函数供外部调用
*************************************************/
void DevTypeTableInit(void)
{
    UCHAR ucDevtype = g_stDevInfoSet.ucDevType;
    if(ucDevtype > DEV_WITH_SEC)  //附带太阳能控制器
    {
        ucDevtype = ucDevtype - DEV_WITH_SEC;
    }

    if(ucDevtype < 50)
    {
        g_stDevTypeTable.ucGenType = GEN_TYPE_2G;
    }
    else
    {
        g_stDevTypeTable.ucGenType = GEN_TYPE_3G;
    }

    switch(ucDevtype)
    {
    case WIDE_BAND_2G:
    case OPTICAL_WIDEBAND_FAR_2G:
    //MCM-63_20070315_Zhonghw_begin
    case BI_DIRECT_WIDE_BAND_TOWER_AMP_2G:
    //MCM-63_20070315_Zhonghw_begin
    case OPTICAL_CARRIER_WIDEBAND_LOCAL_2G:
    case OPTICAL_CARRIER_WIDEBAND_FAR_2G:
    case WIDE_BAND:
    case OPTICAL_WIDEBAND_FAR:
    case OPTICAL_CARRIER_WIDEBAND_LOCAL:
    case OPTICAL_CARRIER_WIDEBAND_FAR:
        g_stDevTypeTable.ucSetFreqType = FREQTYPE_WIDEBAND;
        break;

    case WIRELESS_FRESELT_2G:
    //MCM-63_20070315_Zhonghw_begin
    case BI_DIRECT_FRE_SELECT_TOWER_AMP_2G:
    //MCM-63_20070315_Zhonghw_begin        
    case OPTICAL_FRESELT_FAR_2G:
    case OPTICAL_CARRIER_FRESELT_LOCAL_2G:
    case OPTICAL_CARRIER_FRESELT_FAR_2G:
    case WIRELESS_FRESELT:
    case OPTICAL_FRESELT_FAR:
    case OPTICAL_CARRIER_FRESELT_LOCAL:
    case OPTICAL_CARRIER_FRESELT_FAR:
    case FRESHIFT_FRESELT_FAR_2G:
        g_stDevTypeTable.ucSetFreqType = FREQTYPE_FRESELT;
        break;

    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL_2G:
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL_2G:
    case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL_2G:
    case FRESHIFT_FRESELT_FAR:
    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL:
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL:
    case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL:
        g_stDevTypeTable.ucSetFreqType = FREQTYPE_FRESHIFT;
        break;

    case FRESHIFT_WIRELESS_WIDEBAND_LOCAL_2G:
    case FRESHIFT_WIDEBAND_FAR_2G:
    case FRESHIFT_WIRELESS_WIDEBAND_LOCAL:
    case FRESHIFT_WIDEBAND_FAR:
        g_stDevTypeTable.ucSetFreqType = FREQTYPE_FRESHIFT_WIDEBAND;
        break;

    default:
        g_stDevTypeTable.ucSetFreqType = FREQTYPE_WIDEBAND;
        break;
    }


    switch(ucDevtype)
    {
    case OPTICAL_WIDEBAND_FAR_2G:
    case OPTICAL_FRESELT_FAR_2G:
    case OPTICAL_CARRIER_WIDEBAND_FAR_2G:
    case OPTICAL_CARRIER_FRESELT_FAR_2G:
    case OPTICAL_WIDEBAND_FAR:
    case OPTICAL_FRESELT_FAR:
    case OPTICAL_CARRIER_WIDEBAND_FAR:
    case OPTICAL_CARRIER_FRESELT_FAR:
    //MCM-63_20070315_Zhonghw_begin
    case BI_FRE_OPTICAL_FAR_2G:
    //MCM-63_20070315_Zhonghw_begin            
    case 58:
        g_stDevTypeTable.ucOpticalType = OPTICAL_REMOTE;
        g_stDevTypeTable.ucCommRoleType = COMM_ROLE_SLAVE;
        g_stDevTypeTable.ucRFRoleType = RF_SLAVE;
        break;

    case OPTICAL_DIRECOUPL_LOCAL_2G:
    case OPTICAL_CARRIER_WIDEBAND_LOCAL_2G:
    case OPTICAL_CARRIER_FRESELT_LOCAL_2G:
    case OPTICAL_WIRELESSCOUPL_LOCAL_2G:
    case OPTICAL_DIRECOUPL_LOCAL:
    case OPTICAL_CARRIER_WIDEBAND_LOCAL:
    case OPTICAL_CARRIER_FRESELT_LOCAL:
    case OPTICAL_WIRELESSCOUPL_LOCAL:
    //MCM-63_20070315_Zhonghw_begin
    case BI_FRE_OPTICAL_DIRECOUPL_LOCAL_2G:
    //MCM-63_20070315_Zhonghw_begin                    
    case 59:
        g_stDevTypeTable.ucOpticalType = OPTICAL_LOCAL;
        g_stDevTypeTable.ucCommRoleType = COMM_ROLE_MASTER;
        g_stDevTypeTable.ucRFRoleType = RF_MASTER;
        break;

    case TRUNK_AMPLIFIER_2G:
    case TRUNK_AMPLIFIER:
        g_stDevTypeTable.ucOpticalType = OPTICAL_NO;
        g_stDevTypeTable.ucCommRoleType = COMM_ROLE_SLAVE;
        g_stDevTypeTable.ucRFRoleType = RF_MASTER;
        break;

    default:
        g_stDevTypeTable.ucOpticalType = OPTICAL_NO;
        g_stDevTypeTable.ucCommRoleType = COMM_ROLE_MASTER;
        g_stDevTypeTable.ucRFRoleType = RF_MASTER;
        break;
    }

    switch(ucDevtype)
    {    
    case WIRELESS_FRESELT:
    case OPTICAL_WIRELESSCOUPL_LOCAL:
    case OPTICAL_FRESELT_FAR:
    case OPTICAL_CARRIER_FRESELT_LOCAL:
    case OPTICAL_CARRIER_FRESELT_FAR:
    case WIRELESS_FRESELT_2G:
    case OPTICAL_WIRELESSCOUPL_LOCAL_2G:
    case OPTICAL_FRESELT_FAR_2G:
    case OPTICAL_CARRIER_FRESELT_LOCAL_2G:
    case OPTICAL_CARRIER_FRESELT_FAR_2G:
    //MCM-63_20070315_Zhonghw_begin
    case BI_FRE_OPTICAL_FAR_2G:
    case BI_DIRECT_FRE_SELECT_TOWER_AMP_2G:
    //MCM-63_20070315_Zhonghw_begin                    
    
        g_stDevTypeTable.ucChannelType = ONLY_CH;        
        break;
        
    case FRESHIFT_FRESELT_FAR:
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL:
    case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL:
    case FRESHIFT_FRESELT_FAR_2G:
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL_2G:
    case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL_2G:
        g_stDevTypeTable.ucChannelType = CH_SHIFT;
        break;
            
    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL:
        g_stDevTypeTable.ucChannelType = ONLY_SHIFT;
        break;
        
    default:
        g_stDevTypeTable.ucChannelType = CH_SHIFT_NOTHING;
        break;
    }

    switch(ucDevtype)
    {
    case OPTICAL_DIRECOUPL_LOCAL_2G:
    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL_2G:
    case OPTICAL_CARRIER_WIDEBAND_LOCAL_2G:
    case OPTICAL_CARRIER_FRESELT_LOCAL_2G:
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL_2G:
    case OPTICAL_DIRECOUPL_LOCAL:
    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL:
    case OPTICAL_CARRIER_WIDEBAND_LOCAL:
    case OPTICAL_CARRIER_FRESELT_LOCAL:
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL:
    //MCM-63_20070315_Zhonghw_begin
    case BI_FRE_OPTICAL_DIRECOUPL_LOCAL_2G:
    //MCM-63_20070315_Zhonghw_begin                    
    case 59:
        g_stDevTypeTable.ucIsCoupling= TRUE;
        break;
        
    default:
        g_stDevTypeTable.ucIsCoupling = FALSE;
        break;
    }

    //目前显示器都接在串口0上
    g_stDevTypeTable.ucUIUSARTType = UI_USART_0;

    //MCM-64_20070315_Zhonghw_begin
    if(g_stYkppParamSet.stYkppCtrlParam.ucCommRoleType== COMM_ROLE_MASTER)
    {
        g_stDevTypeTable.ucCommRoleType = COMM_ROLE_MASTER;
    }
    //MCM-64_20070315_Zhonghw_end
}

/*************************************************
  Function:
  Description:    本函数在系统启动后对全局变量进行初始化
  Calls:
  Called By:
  Input:          g_astFilePartTbl: 文件分区表
  Output:
  Return:         
  Others:         本函数供外部调用
*************************************************/
void CmccInit()
{
    //MCM-26_20061108_linyu_begin
    //初始化监控参量列表
    InitDevCfgTable();
    //MCM-26_20061108_linyu_end
    //从NV Memory读出参数集
    SimpleFlashRead(ADDR_MOBJ_ID_TBL, &g_stMObjIdTbl, sizeof(g_stMObjIdTbl));    
    SimpleFlashRead(ADDR_DEV_INFO_SET, &g_stDevInfoSet, sizeof(g_stDevInfoSet));    
    SimpleFlashRead(ADDR_NM_PARAM_SET, &g_stNmParamSet, sizeof(g_stNmParamSet));
    SimpleFlashRead(ADDR_SETTING_PARAM_SET, &g_stSettingParamSet, sizeof(g_stSettingParamSet));
    SimpleFlashRead(ADDR_ALARM_ENABLE_SET, &g_uAlarmEnable, sizeof(g_uAlarmEnable));
    SimpleFlashRead(ADDR_CENTER_STATUS_SET, &g_uCenterStatus, sizeof(g_uCenterStatus));
    SimpleFlashRead(ADDR_FILE_PART_TBL, &g_astFilePartTbl, sizeof(g_astFilePartTbl));

    //初始化机型配置表
    DevTypeTableInit();

    //如果是2G机型需要把上报通信方式的设置映射到通信方式上来
    if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)
    {
        switch(g_stNmParamSet.ucReportCommMode)
        {
        case COMM_REPORT_MODE_GPRS:
            g_stNmParamSet.ucCommMode = COMM_MODE_GPRS;
            break;
        case COMM_REPORT_MODE_SMS:
            g_stNmParamSet.ucCommMode = COMM_MODE_SMS;
            break;
        case COMM_REPORT_MODE_CSD:
            g_stNmParamSet.ucCommMode = COMM_MODE_CSD;
            break;
        default:
            break;
        }
    }

#ifdef BIG_ENDIAN
    //AdjustEndianOfDataFromNVMemory();
#endif //ifdef BIG_ENDIAN

    //初始化和升级相关的信息
    SimpleFlashRead(ADDR_UPGRADE_INFO_SET, &g_stUpgradeCtx, sizeof(UPGRADE_INFO_SET_ST)); //注意这两个结构大小不同
    memset((UCHAR *)&g_stUpgradeCtx + sizeof(UPGRADE_INFO_SET_ST), 0, sizeof(g_stUpgradeCtx) - sizeof(UPGRADE_INFO_SET_ST));

    //无需存入NV Memory的全局变量清零    
    memset(&g_stRcParamSet, 0, sizeof(g_stRcParamSet));
    
    memset(&g_stAlarmItemBuf, 0, sizeof(g_stAlarmItemBuf));
    memset(&g_astAlarmReportCtx, 0, sizeof(g_astAlarmReportCtx));
    memset(&g_astOtherReportCtx, 0, sizeof(g_astOtherReportCtx));

    //notice 监控参量个数过大则设置为0，此时通过设置机型可初始化参量列表（扩展参数不计在参量列表中）
    if(g_stMObjIdTbl.usParamCount > 2048)
    {
        g_stMObjIdTbl.usParamCount = 0;
    }
    //初始化MCP:A监控参量处理表
    InitMObjHndlrTbl();

    //初始化和告警相关的信息，必须在监控参量处理表初始化完成后进行
    InitAlarmInfo();

    //将版本号从文件分区表读出初始化
    if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucIsRunning)
    {
        memcpy(g_stDevInfoSet.acSfwrVer, 
               g_astFilePartTbl[PROGRAM_PART_1_IDX].acVersion,
               sizeof(g_stDevInfoSet.acSfwrVer));
    }
    else if(g_astFilePartTbl[PROGRAM_PART_2_IDX].ucIsRunning)
    {
        memcpy(g_stDevInfoSet.acSfwrVer, 
               g_astFilePartTbl[PROGRAM_PART_2_IDX].acVersion,
               sizeof(g_stDevInfoSet.acSfwrVer));
    }
    else
    {
        memset(g_stDevInfoSet.acSfwrVer, '?', sizeof(g_stDevInfoSet.acSfwrVer));
    }
    

    //进入监控状态
    g_stDevInfoSet.ucRunningMode = RUNNING_MODE_MONITOR;

    //MCM-72_20070403_Zhonghw_begin
    //为了简化生产流程，方便沟通理解，在此先初始化站点编号和设备编号
    if((g_stNmParamSet.ulStaNum == 0xFFFFFFFF)&&(g_stNmParamSet.ucDevNum == 0xFF))
    {
        g_stNmParamSet.ulStaNum = 0;
        g_stNmParamSet.ucDevNum = 0; 
    }
    //MCM-72_20070403_Zhonghw_end
    
    //将参数设置到硬件以生效，由驱动层完成
    SetAtt();
    SetRfSw();
    SetFreq();

    //初始化和GPRS登录相关的参数
    g_stCenterConnStatus.ucLinkStatus           = COMM_STATUS_DISCONNECTED;
    g_stCenterConnStatus.ucLoginStatus          = COMM_STATUS_LOGOUT;
    g_stCenterConnStatus.ucConnAttempCnt        = 0;
    g_stCenterConnStatus.ucNoRspCnt             = 0;
    g_stCenterConnStatus.ucLoginTimeoutCnt      = 0;
    
    //MCM-29_20061110_zhangjie_begin
    g_stRcParamSet.cDlInPwr = (CHAR)((30+80)/2 - g_stYkppParamSet.stYkppRfParam.ucDlInCoupler);
    //MCM-29_20061110_zhangjie_end
}


/*************************************************
  Function:
  Description:    本函数在输入数据中查找完整的协议数据包，如果找到
                  就返回（包含起始标志）
  Calls:
  Called By:
  Input:          ucFlag:       待扫描的起始结束标志
                  pucInput:     待扫描的数据
                  ulInputLen:   待扫描的数据长度，包含起始结束标志
  Output:         pulOutputLen: 0表示没有查到完整数据包，否则表示完整数据包的长度
                  usState:      返回扫描的状态
  Return:         指向完整的数据包头
  Others:         
*************************************************/
UCHAR *ScanAP(UCHAR ucFlag, UCHAR *pucInput, ULONG ulInputLen, ULONG *pulOutputLen, USHORT *pusState)
{
    ULONG ulOffset = 0;
    UCHAR *pucStartFlag = NULL;
    
    *pulOutputLen = 0;
    *pusState = NO_FLAG_FOUND;

    while(ulOffset < ulInputLen)
    {
        switch(*pusState) //根据状态处理输入字符
        {
        case NO_FLAG_FOUND:
            if(pucInput[ulOffset] == ucFlag)
            {
                *pusState = AP_START_FLAG_FOUND;
                pucStartFlag = pucInput + ulOffset;
            }
            break;
        case AP_START_FLAG_FOUND:
            if(pucInput[ulOffset] == ucFlag)
            {
                *pusState = AP_END_FLAG_FOUND;
                *pulOutputLen = pucInput + ulOffset - pucStartFlag + 1;
            }
            break;
        default:
            break; //不会进入这个分支，否则程序有误
        }

        ulOffset++;

        //返回有效数据，此时状态为APAC_END_FLAG_FOUND或APB_END_FLAG_FOUND
        if(*pulOutputLen != 0)
        {
            *pusState += ucFlag;
            return pucStartFlag;
        }
    }

    //没有找到协议数据包，此时状态为NO_FLAG_FOUND或APAC_START_FLAG_FOUND或APB_START_FLAG_FOUND
    if(*pusState != NO_FLAG_FOUND)
    {
        *pusState += ucFlag;
    }
    return pucStartFlag;
}

/*************************************************
  Function:
  Description:    本函数调用ScanAP()对接收到的协议数据进行扫描，并将有效的协议数据
                  提取出来，输入的数据只可能包含一个协议数据包，这是由下层保证的，
                  但是为了处理的完备，仍然考虑可能收到包含各种AP协议
  Calls:
  Called By:
  Input:          pucInput:     待扫描的数据
                  ulInputLen:   待扫描的数据长度，包含起始结束标志
  Output:         ppucOutput:   抽取出的完整的数据包（包含起始结束标志），如果无完整数据包则无效
                  pulOutputLen: 0表示没有查到完整数据包，否则表示完整数据包的长度
  Return:         处理的结果
  Others:         
*************************************************/
LONG ExtractCMCCProtoData(UCHAR *pucInput, ULONG ulInputLen, UCHAR **ppucOutput, ULONG *pulOutputLen)
{
    UCHAR *pucData  = pucInput;
    ULONG ulDataLen = ulInputLen;
    USHORT usState   = 0;

    UCHAR *pucApac = NULL;
    ULONG ulApacLen= 0;
    UCHAR *pucApb  = NULL;
    ULONG ulApbLen = 0;

    //扫描AP:A或AP:C数据包
    UCHAR *pucStart = ScanAP(APAC_FLAG, pucData, ulDataLen, &ulApacLen, &usState);
    switch(usState)
    {
    case APAC_END_FLAG_FOUND:
        pucApac = pucStart;        
        break;
    case APAC_START_FLAG_FOUND:
    case NO_FLAG_FOUND:
        break;
    }

    //扫描AP:B数据包
    pucStart = ScanAP(APB_FLAG, pucData, ulDataLen, &ulApbLen, &usState);

    switch(usState)
    {
    case APB_END_FLAG_FOUND:
        pucApb = pucStart;        
        break;
    case APB_START_FLAG_FOUND:
    case NO_FLAG_FOUND:
        break;
    }
    
    if((ulApacLen > 0) && (ulApbLen > 0)) //如果包含两种协议数据
    {
        if(pucApac < pucApb) //AP:AC在前
        {
            *ppucOutput = pucApac;
            *pulOutputLen = ulApacLen;
        }
        else
        {
            *ppucOutput = pucApb;
            *pulOutputLen = ulApbLen;
        }
        return CMCC_PROTO_DATA_FOUND;
    }
    else if(ulApacLen > 0) //只包含AP:AC
    {
        *ppucOutput = pucApac;
        *pulOutputLen = ulApacLen;
        return CMCC_PROTO_DATA_FOUND;
    }
    else if(ulApbLen > 0) //只包含AP:B
    {
        *ppucOutput = pucApb;
        *pulOutputLen = ulApbLen;
        return CMCC_PROTO_DATA_FOUND;
    }

    return NO_CMCC_PROTO_DATA_FOUND;
}

/*************************************************
  Function:
  Description:    本函数对AP层协议数据进行解码，根据不同AP协议类型进行二字节合并
                  和CRC校验，并获取NP层PDU和长度
  Calls:
  Called By:
  Input:          pucApData:       AP层数据指针，指向起始标志之后的第一个字节
                  ulApDataLen:     AP层数据长度，不包含起始结束标志
  Output:         pulRawApDataLen: AP层的原始数据长度，包含CRC，不包含起始结束标志
                  ppucVpData:      VP层的数据
                  *ulVpDataLen:    VP层的PDU长度
  Return:         AP_A:            AP_A类型的数据
                  AP_B:            AP_B类型的数据
                  AP_C:            AP_C类型的数据
                  FAILED:          可以忽略该数据包
  Others:         
*************************************************/
LONG DecodeApData(UCHAR *pucApData, ULONG ulApDataLen, ULONG *pulRawApDataLen, 
                  UCHAR **ppucVpData, ULONG *ulVpDataLen)
{
    USHORT usCrc = 0;
    LONG lApType = FAILED;

    //判断类型
    switch(*pucApData)
    {
    case AP_A: //需要转义
        ulApDataLen = DecodeEscape(pucApData, ulApDataLen, g_stApacEscape.ucEscape,
                                   g_stApacEscape.aucAfterEscape,
                                   g_stApacEscape.aucBeforeEscape, 
                                   sizeof(g_stApacEscape.aucAfterEscape));
        lApType = AP_A;
        break;
    case AP_C:
        ulApDataLen = DecodeEscape(pucApData, ulApDataLen, g_stApacEscape.ucEscape,
                                   g_stApacEscape.aucAfterEscape,
                                   g_stApacEscape.aucBeforeEscape, 
                                   sizeof(g_stApacEscape.aucAfterEscape));
        lApType = AP_C;
        break;
    case '0': //AP:B拆分后协议类型为“02”，先判断第一个字节是否正确
        if(*(pucApData + 1) == '2')
        {
            ulApDataLen = Merge2Bytes(pucApData, ulApDataLen); //进行二字节合并
            lApType = AP_B;
        }
        else
        {
            return FAILED;
        }        
        break;
    default: //忽略不处理
        return FAILED;
    }

    //判断AP层的有效长度
    if(ulApDataLen < AP_OVERHEAD)
    {
        return FAILED;
    }

    //CRC校验
    usCrc = CalcuCRC(pucApData, ulApDataLen - CRC_LEN);
    //if(*((USHORT*)(pucApData + ulApDataLen - CRC_LEN)) != ADJUST_WORD(usCrc))
    if(GET_WORD(pucApData + ulApDataLen - CRC_LEN) != usCrc)
    {
        return FAILED; //校验错误
    }

    *pulRawApDataLen = ulApDataLen; //返回AP层原始数据长度

    //获取VP层PDU
    *ulVpDataLen = ulApDataLen - 1 - 1 - CRC_LEN; //减去协议类型、承载协议类型和CRC单元
    *ppucVpData = ((AP_LAYER_ST *)pucApData)->aucPdu;

    return lApType;
}

/*************************************************
  Function:
  Description:    本函数对AP层协议数据进行编码，根据不同AP协议类型进行二字节拆分
                  和CRC校验，并返回编码后的AP数据和长度，所有的操作都在传入的数
                  据块中进行，调用方必须保证数据块包含足够的编码空间；通常传入的
                  数据块就是存放接收AP层数据的地方
  Calls:
  Called By:
  Input:          pucRawApData:   AP层的原始数据指针，指向起始标志后的第一个字节，
                                  同时需要保证该指针的前一个字节有效（用来存放起始标志）
                  ulRawApDataLen: AP层的原始数据长度，不包含起始结束标志和校验单元
                  ucApType:       AP协议类型
                  ulBufSize:      可用于编码的内存块大小
  Output:         
  Return:         0:              编码失败
                  非0:            编码后的数据长度，包含起始结束标志和校验单元
  Others:
*************************************************/
ULONG EncodeApData(UCHAR *pucRawApData, ULONG ulRawApDataLen, UCHAR ucApType, ULONG ulBufSize)
{
    USHORT usCrc = 0;
    ULONG ulApDataLen = 0;
    
    AP_LAYER_ST *pstApData = (AP_LAYER_ST *)pucRawApData;
    pstApData->ucApType    = ucApType;
    pstApData->ucPduType   = VP_A; //承载协议类型固定为VP:A

    //先进行CRC校验
    usCrc = CalcuCRC(pucRawApData, ulRawApDataLen);
    //*((USHORT *)(pucRawApData + ulRawApDataLen)) = ADJUST_WORD(usCrc); //调整为低字节在前，高字节在后
    *(pucRawApData + ulRawApDataLen) = GET_WORD_1ST_BYTE(usCrc);
    *(pucRawApData + ulRawApDataLen + 1) = GET_WORD_2ND_BYTE(usCrc);

    //判断类型
    if(ucApType == AP_B)
    {
        ulApDataLen = SplitInto2Bytes(pucRawApData, ulRawApDataLen + CRC_LEN); //进行二字节拆分
        if(ulApDataLen == 0)
        {
            return 0;
        }
        *(pucRawApData - 1) = APB_FLAG; //在数据最前端添加起始标志，传入参数保证前一个字节是有效的
        *(pucRawApData + ulApDataLen) = APB_FLAG;
        return ulApDataLen + 2;
    }
    else
    {
        ulApDataLen = EncodeEscape(pucRawApData, ulRawApDataLen + CRC_LEN,
                                   ulBufSize,
                                   g_stApacEscape.ucEscape,
                                   g_stApacEscape.aucAfterEscape,
                                   g_stApacEscape.aucBeforeEscape,
                                   sizeof(g_stApacEscape.aucAfterEscape)); //进行转义处理
        if(ulApDataLen == 0)
        {
            return 0;
        }
        *(pucRawApData - 1) = APAC_FLAG; //在数据最前端添加起始标志，传入参数保证前一个字节是有效的
        *(pucRawApData + ulApDataLen) = APAC_FLAG;
        return ulApDataLen + 2;
    }
}

/*************************************************
  Function:
  Description:    本函数用于重启心跳周期定时器，当完成一次心跳，或者
                  监控模块和监控中心有数据交互时心跳周期定时器需要被
                  重启，如果正在等待心跳应答则不重启
  Calls:
  Called By:
  Input:          
  Output:
  Return:         
  Others:         
*************************************************/
void RestartHeartbeatPeriodTimer()
{
    OH_TIMER_CTX_UN uCtx;
    ULONG ulIdx = REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE;

//    if(g_astOtherReportCtx[ulIdx].ucCommState != OH_IDLE)
//    {
//        return; //当已经发送了心跳包时不再重启心跳周期定时器
//    }
    
    if(g_astOtherReportCtx[ulIdx].pTimer) //如果原来有定时器需要先停止
    {
        if(g_astOtherReportCtx[ulIdx].ucCommState == OH_IDLE) //如果是心跳周期定时器则需要重启
        {
            OHRemoveTimer(g_astOtherReportCtx[ulIdx].pTimer, 
                          g_astOtherReportCtx[ulIdx].ulTimerMagicNum);
            g_astOtherReportCtx[ulIdx].pTimer = 0;
            g_astOtherReportCtx[ulIdx].ulTimerMagicNum = 0;              
        }
        else
        {
            return;
        }
    }

    //启动定时器
    uCtx.stParam.ucTimerID = TIMER_HEARTBEAT_PERIOD_ID;
    uCtx.stParam.ucParam1  = REPORT_TYPE_HEARTBEAT; 

    g_astOtherReportCtx[ulIdx].pTimer = OHCreateTimer((void *)uCtx.ulParam, TIMER_HEARTBEAT_PERIOD_INTV, 
                                                     &(g_astOtherReportCtx[ulIdx].ulTimerMagicNum));
} 

/*************************************************
  Function:
  Description:    本函数在修复确认上报事件后进行，处理只有故障修复后才
                  能恢复的告警，将它们的本地告警状态置为正常
  Calls:
  Called By:
  Input:          
  Output:
  Return:         
  Others:         本函数供外部调用
*************************************************/
void ActionAfterRepairConfirmReport()
{
    //自激告警
    g_uAlarmItems.stAlarmItemStruct.stSelfOsc.ucLocalStatus = 0;
    g_uAlarmItems.stAlarmItemStruct.stSelfOscShutdown.ucLocalStatus = 0;
    g_uAlarmItems.stAlarmItemStruct.stSelfOscReduceAtt.ucLocalStatus= 0;

    //监控电池故障告警
    g_uAlarmItems.stAlarmItemStruct.stBatFalut.ucLocalStatus = 0;    
}

/*************************************************
  Function:
  Description:    本函数处理收到的上报应答
  Calls:
  Called By:
  Input:          usPackeID: 数据包ID
                  ucRspFlag: 应答标志
  Output:         
  Return:         IGNORE:    不再进行后续处理
  Others:         
*************************************************/
LONG HandleReportRsp(USHORT usPacketID, UCHAR ucRspFlag)
{
    UCHAR ucCtxIdx = 0;
    ULONG i = 0;
    ULONG j = 0;
    OH_TIMER_CTX_UN uCtx;
    
    //告警上下文中查找
    for(ucCtxIdx = 0; ucCtxIdx < MAX_ALARM_REPORTS; ucCtxIdx++)
    {
        if(g_astAlarmReportCtx[ucCtxIdx].usPacketID == usPacketID)
        {
            //停止定时器
            if(g_astAlarmReportCtx[ucCtxIdx].pTimer)
            {
                OHRemoveTimer(g_astAlarmReportCtx[ucCtxIdx].pTimer, g_astAlarmReportCtx[ucCtxIdx].ulTimerMagicNum);
                g_astAlarmReportCtx[ucCtxIdx].pTimer = 0;
                g_astAlarmReportCtx[ucCtxIdx].ulTimerMagicNum = 0;
            }
            g_astAlarmReportCtx[ucCtxIdx].usPacketID  = 0;
            g_astAlarmReportCtx[ucCtxIdx].ucCommState = OH_IDLE;

            g_stAlarmItemBuf.ulAlarmItemCount -= g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemCount;

            //如果应答标志不为0，则不能更新监控中心告警状态
            if(ucRspFlag == RSP_FLAG_SUCCEEDED)
            {
                //查找告警发送缓存和中心状态中对应的告警项，这里应该可以使用二分法提高效率
                for(i = g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemBegin; i < g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemCount; i++)
                {
                    for(j = 0; j < MAX_ALARM_ITEMS; j++)
                    {
                        if(g_uAlarmItems.astAlarmItemArray[j].ucIDLoByte
                        == g_stAlarmItemBuf.astAlarmItems[i].ucIDLoByte)
                        {
                            g_uCenterStatus.aucAlarmStatusArray[j]  //更新中心状态，需要保存到NV Memory中
                                    = g_stAlarmItemBuf.astAlarmItems[i].ucStatus;
                            j++;
                            break;
                        }
                    }

                    //清空告警发送缓存
                    g_stAlarmItemBuf.astAlarmItems[i].ucUsed     = 0;
                    g_stAlarmItemBuf.astAlarmItems[i].ucStatus   = 0;
                    g_stAlarmItemBuf.astAlarmItems[i].ucIDLoByte = 0;
                    
                    if(j == MAX_ALARM_ITEMS)
                    {
                        break;
                    }
                }

                SaveAlarmCenterStatus();
            }

            return IGNORE;
        }
    }

    //在其它上报的上下文中查找
    for(i = 0; i < OTHER_REPORT_COUNT; i++)
    {
        if(g_astOtherReportCtx[i].usPacketID == usPacketID)
        {
            //停止定时器
            if(g_astOtherReportCtx[i].pTimer)
            {
                OHRemoveTimer(g_astOtherReportCtx[i].pTimer, g_astOtherReportCtx[i].ulTimerMagicNum);
                g_astOtherReportCtx[i].pTimer = 0;
                g_astOtherReportCtx[i].ulTimerMagicNum = 0;
            }
            
            g_astOtherReportCtx[i].ucCommState = OH_IDLE;

            if(i == REPORT_TYPE_LOGIN - OTHER_REPORT_BASE) //登录的应答
            {
                if(ucRspFlag == RSP_FLAG_SUCCEEDED) //登录成功
                {
                    g_stCenterConnStatus.ucLoginStatus = COMM_STATUS_LOGIN;

                    //RestartHeartbeatPeriodTimer(); //开始启动心跳周期定时器，在最外层统一处理
                }
                else //间隔一段时间重新登录
                {
                    uCtx.stParam.ucTimerID = TIMER_LOGIN_PERIOD_ID;    
                    uCtx.stParam.ucParam1  = REPORT_TYPE_LOGIN;

                    g_astOtherReportCtx[i].pTimer = OHCreateTimer((void *)uCtx.ulParam, TIMER_LOGIN_PERIOD_INTV, 
                                                                 &(g_astOtherReportCtx[i].ulTimerMagicNum));
                }
            }
            else if(i == REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE) //心跳的应答
            {
                //间隔一段时间重发心跳，因为和中心的数据交互都会导致心跳周期定时器重启，因此放在接收数据时执行
            }
            else
            {
                //通知上报的结果
                NotifyReportResult((UCHAR)i + OTHER_REPORT_BASE, ucRspFlag);

                if((i == REPORT_TYPE_STA_INIT - OTHER_REPORT_BASE) 
                && (ucRspFlag == RSP_FLAG_SUCCEEDED) && (g_stNmParamSet.ucCommMode == COMM_MODE_GPRS))
                {
                    //如果之前未登录成功，则此时不再登录，而认为已经登录成功
                    g_stCenterConnStatus.ucLoginStatus = COMM_STATUS_LOGIN;

                    //RestartHeartbeatPeriodTimer(); //开始启动心跳周期定时器，在最外层统一处理
                }
                
                //修复确认后需要更新某些告警状态
                if((i == REPORT_TYPE_REPAIR_CONFIRMED - OTHER_REPORT_BASE)
                 &&(ucRspFlag == RSP_FLAG_SUCCEEDED))
                {
                    ActionAfterRepairConfirmReport();
                }
                break;
            }
        }
    }
    
    return IGNORE;
}

/*************************************************
  Function:
  Description:    本函数对VP层议数据进行解码
  Calls:
  Called By:
  Input:          pucVpData:     VP层数据指针
                  ulVpDataLen:   VP层数据长度
  Output:         ppucMcpData:   MCP层的PDU
                  pulMcpDataLen: MCP层的PDU长度
  Return:         SUCCEEDED:     可以进行后续处理
                  IGNORE:        可以忽略该数据包，当数据包不正确或者是应答包时
                  NEED_TO_FORWARD: 需要转发该数据包
                  
  Others:         
*************************************************/
LONG DecodeVpData(UCHAR *pucVpData, ULONG ulVpDataLen,
                  UCHAR **ppucMcpData, ULONG *pulMcpDataLen)
{
    VPA_LAYER_ST *pstVpData = (VPA_LAYER_ST *)pucVpData;
    
    //1.判断VP层数据的有效长度
    if(ulVpDataLen < sizeof(VPA_LAYER_ST))
    {
        return IGNORE;
    }
    
    //2.站点编号和设备编号判断
    if(g_stYkppParamSet.stYkppCtrlParam.ucAddrAuthSw != 0) //判断是否需要对地址进行鉴权
    {
        if((ADJUST_DWORD(pstVpData->ulStaNum) != g_stNmParamSet.ulStaNum)||(pstVpData->ucDevNum != g_stNmParamSet.ucDevNum))
        {
            return NEED_TO_FORWARD; //系统编号不同需要转发
        }
    }
    
    //3.VP层交互标志
    switch(pstVpData->ucVpFlag)
    {
    case VP_FLAG_COMMAND:   //请求，不处理通信包标识
        pstVpData->ucVpFlag = VP_FLAG_SUCCEEDED; //应答的VP层交互标志
        *ppucMcpData   = pstVpData->aucPdu;
        *pulMcpDataLen = ulVpDataLen - sizeof(VPA_LAYER_ST) + MCP_MIN_LEN;        
        return SUCCEEDED;

    case VP_FLAG_SUCCEEDED: //应答
        //根据协议规定可知，当VP包为应答时只要使用通信包标识号匹配即可知道对应的上报类型
        return HandleReportRsp(ADJUST_WORD(pstVpData->usPacketID),          //调整字节序
                               ((MCP_LAYER_ST *)pstVpData->aucPdu)->ucRspFlag);

    case VP_FLAG_FAILED: //协议规定设备忙无法处理，因此监控中心不会回这个标志
    default: //保留和厂家自定义，暂不处理
        return IGNORE;
    }
}

/*********************************************************************************
  Function:
  Description:    本函数处理MCP:A中的监控参量列表查询
  Calls:
  Called By:
  Input:          pstMObj:    MCP层数据指针  
                  ucApType:   AP数据类型
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpAMObjList(MCPA_MOBJ_ST *pstMObj, UCHAR ucApType)
{
    LONG lRspFlag = MOBJ_CORRECT;
    UCHAR ucMaxMObjIDCountPerQuery = 0;
    UCHAR ucTotalQueryCount        = 0;
    UCHAR ucQueriedMObjIDLen       = 0;

    //判断“监控对象标号与监控对象长度不匹配”
    if(pstMObj->ucMObjLen != 5)
    {
        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_VALUE_OUT_OF_BOUND, lRspFlag);
        return lRspFlag; 
    }

    if(ucApType == AP_A)
    {
        ucMaxMObjIDCountPerQuery = MAX_MOBJ_ID_COUNT_BY_APA;
    }
    else if(ucApType == AP_B)
    {
        ucMaxMObjIDCountPerQuery = MAX_MOBJ_ID_COUNT_BY_APB;
    }
    else //AP_C
    {
        ucMaxMObjIDCountPerQuery = MAX_MOBJ_ID_COUNT_BY_APC;
    }

    //查询本设备支持的监控参量总个数
    if(g_stMObjIdTbl.usParamCount % ucMaxMObjIDCountPerQuery > 0)
    {
        ucTotalQueryCount = g_stMObjIdTbl.usParamCount / ucMaxMObjIDCountPerQuery + 1;
    }
    else
    {
        ucTotalQueryCount = g_stMObjIdTbl.usParamCount / ucMaxMObjIDCountPerQuery;
    }

    if((pstMObj->aucMObjContent[0] > ucTotalQueryCount)
     ||(pstMObj->aucMObjContent[1] > ucTotalQueryCount)
     ||(pstMObj->aucMObjContent[0] == 0)
     ||(pstMObj->aucMObjContent[1] == 0))
    {
        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_VALUE_OUT_OF_BOUND, lRspFlag); //“监控对象内容超出范围”
    
        //当此处的错误代码为2时，监控对象内容中为设备返回的实际值(接口规范8.6)
        //返回列表的第一个部分，当作第一次收到查询的情况处理
        pstMObj->aucMObjContent[0] = ucTotalQueryCount;
        pstMObj->aucMObjContent[1] = 1;

        if(ucTotalQueryCount > pstMObj->aucMObjContent[1])
        {
            ucQueriedMObjIDLen = ucMaxMObjIDCountPerQuery * UINT_2;                    
        }
        else
        {
            ucQueriedMObjIDLen = g_stMObjIdTbl.usParamCount * UINT_2;                    
        }

        memcpy(&pstMObj->aucMObjContent[2], g_stMObjIdTbl.ausParamID, ucQueriedMObjIDLen);
    }
    else if(pstMObj->aucMObjContent[0] == 1) //第一次查询，此处不判断当前查询编号
    {
        pstMObj->aucMObjContent[0] = ucTotalQueryCount;
        pstMObj->aucMObjContent[1] = 1;

        if(ucTotalQueryCount > pstMObj->aucMObjContent[1])
        {
            ucQueriedMObjIDLen = ucMaxMObjIDCountPerQuery * UINT_2; 
        }
        else
        {
            ucQueriedMObjIDLen = g_stMObjIdTbl.usParamCount * UINT_2; 
        }

        memcpy(&pstMObj->aucMObjContent[2], g_stMObjIdTbl.ausParamID, ucQueriedMObjIDLen);
    }
    else //根据pstMObj->aucMObjContent决定第几次查询
    {
        if(ucTotalQueryCount > pstMObj->aucMObjContent[1])
        {
            ucQueriedMObjIDLen = ucMaxMObjIDCountPerQuery * UINT_2;                    
        }
        else
        {
            ucQueriedMObjIDLen = (g_stMObjIdTbl.usParamCount - (ucTotalQueryCount - 1) * ucMaxMObjIDCountPerQuery) * UINT_2;
        }

        memcpy(&pstMObj->aucMObjContent[2],
               (UCHAR *)&g_stMObjIdTbl.ausParamID + (pstMObj->aucMObjContent[1] - 1) * ucMaxMObjIDCountPerQuery * UINT_2,
               ucQueriedMObjIDLen);
    }

    pstMObj->ucMObjLen += ucQueriedMObjIDLen;
    return lRspFlag;
}

/*************************************************
  Function:
  Description:    本函数在切换到升级模式时调用，选择升级文件
                  存放的分区，并初始化升级过程上下文
  Calls:
  Called By:
  Input:          g_astFilePartTbl: 文件分区表
  Output:
  Return:         
  Others:
*************************************************/
void SwitchToUpgradeMode()
{
    //选择下载文件存放的分区
    if((g_astFilePartTbl[PROGRAM_PART_1_IDX].ucStatus == PROGRAM_STATUS_RUNNABLE)
    && (g_astFilePartTbl[PROGRAM_PART_2_IDX].ucStatus == PROGRAM_STATUS_RUNNABLE))
    {
        if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucNew == FILE_OLD)
        {
            g_stUpgradeCtx.ulUpgradePartIdx = PROGRAM_PART_1_IDX;
        }
        else
        {
            g_stUpgradeCtx.ulUpgradePartIdx = PROGRAM_PART_2_IDX;
        }        
    }
    else if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucStatus == PROGRAM_STATUS_RUNNABLE)
    {
        g_stUpgradeCtx.ulUpgradePartIdx = PROGRAM_PART_2_IDX;
    }
    else if(g_astFilePartTbl[PROGRAM_PART_2_IDX].ucStatus == PROGRAM_STATUS_RUNNABLE)
    {
        g_stUpgradeCtx.ulUpgradePartIdx = PROGRAM_PART_1_IDX;
    }
    else
    {
        //待考虑???
        if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucNew == FILE_OLD)
        {
            g_stUpgradeCtx.ulUpgradePartIdx = PROGRAM_PART_1_IDX;
        }
        else
        {
            g_stUpgradeCtx.ulUpgradePartIdx = PROGRAM_PART_2_IDX;
        }          
    }

    //初始化升级过程的上下文，其它部分在CmccInit()中完成
    g_stUpgradeCtx.ulUpgradeProgress= TRANS_CTRL_INIT;

    //切换到升级模式
    g_stDevInfoSet.ucRunningMode = RUNNING_MODE_UPGRADE;
    
    //启动定时器
    StartUpgradeTimer(TIMER_TRANS_CTRL_ID, TIMER_TRANS_CTRL_INTV);
}

/*************************************************
  Function:
  Description:    本函数处理对MCP:A的监控参量的查询和设置，其中查询监控参量
                  特殊处理，其余的均使用处理表中的回调函数处理
  Calls:
  Called By:
  Input:          pucMcpData:    MCP层数据指针
                  pulMcpDataLen: MCP层数据长度
                  ucApType:      AP协议类型
                  usPacketID:    包序号
  Output:         pulMcpDataLen: 处理后的MCP层数据长度，查询监控参量列表时长度会改变
  Return:         SUCCEEDED:     可以进行后续处理
                  IGNORE:        可以忽略该数据包
  Others:         
*************************************************/
LONG HandleMcpA(UCHAR* pucMcpData, ULONG *pulMcpDataLen, UCHAR ucApType, USHORT usPacketID)
{
    MCP_LAYER_ST *pstMcp  = (MCP_LAYER_ST *)pucMcpData;
    LONG lMcpDataLen      = (LONG)*pulMcpDataLen;
    ULONG ulActualLen     = 0; //用于返回处理后的实际长度
    MCPA_MOBJ_ST *pstMObj = NULL;
    MOBJ_HNDLR pfHndlr    = NULL;
    LONG lRspFlag         = 0;
    LONG j                = 0;

    if(pstMcp->ucRspFlag != RSP_FLAG_CMD)
    {
        return IGNORE; //如果应答标志不正确则不应答
    }

    pstMcp->ucRspFlag = RSP_FLAG_SUCCEEDED;
    
    switch(pstMcp->ucCmdId)
    {
    case MCPA_CMD_ID_QUERY:
    case MCPA_CMD_ID_SET:
        break;
    case MCPA_CMD_ID_SWITCH_TO_UPGRADE: 
        SwitchToUpgradeMode();
        return SUCCEEDED;
    //MCM-32_20061114_zhonghw_begin
    case MCPA_CMD_ID_SWITCH_FILE_VER:
        if(HandleSwitchSofVer(pstMcp,lMcpDataLen) != SUCCEEDED)
        {
            pstMcp->ucRspFlag = RSP_FLAG_OTHER_ERR;
        }
        return SUCCEEDED;
    //MCM-32_20061114_zhonghw_end        
        
    default: 
        pstMcp->ucRspFlag = RSP_FLAG_CMD_ID_ERR;
        return SUCCEEDED;
    }

    pstMObj = (MCPA_MOBJ_ST *)(pstMcp->aucContent);
    lMcpDataLen -= 2; //扣除命令单元长度
    ulActualLen += 2;
    while(lMcpDataLen >= MCPA_MOBJ_MIN_LEN) //L和T各占2字节
    {
        if(pstMObj->ucMObjLen < MCPA_MOBJ_MIN_LEN) //监控对象长度不够，之后的数据就不再处理
        {
            pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR;
            return SUCCEEDED;
        }

        if(pstMObj->ucMObjLen > lMcpDataLen) //超出实际数据长度
        {
            pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR;
            return SUCCEEDED;
        }

        switch(pstMObj->aucMObjId[HI_BYTE] & 0xF) //高字节表示参数分类，只取低四位
        {
        case MOBJ_ID_DEV_INFO_SET:
            if(pstMObj->aucMObjId[LO_BYTE] == MOBJ_ID_DI_MOBJ_TABLE) //查询监控参量列表做特殊处理
            {
                pstMcp->ucRspFlag = (UCHAR)HandleMcpAMObjList(pstMObj, ucApType);
                ulActualLen += pstMObj->ucMObjLen;
                *pulMcpDataLen = ulActualLen;

                return SUCCEEDED; //后续的就不再处理，查询监控参量列表不应该有其它查询项
            }
            if(pstMObj->aucMObjId[LO_BYTE] < DEV_INFO_HNDLR_TBL_SIZE)
            {
                pfHndlr = g_astDevInfoHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].pfHandler;
                if(pfHndlr)
                {
                    lRspFlag = pfHndlr(pstMObj, 
                                       g_astDevInfoHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ulAddr,
                                       g_astDevInfoHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucDataType,
                                       g_astDevInfoHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucIsSupported,
                                       pstMcp->ucCmdId);
                    break;
                }
            }

            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        case MOBJ_ID_NM_PARAM_SET:
            if(pstMObj->aucMObjId[LO_BYTE] < NM_PARAMS_HNDLR_TBL_SIZE)
            {
                pfHndlr = g_astNmParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].pfHandler;
                if(pfHndlr)
                {
                    lRspFlag = pfHndlr(pstMObj, 
                                       g_astNmParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ulAddr,
                                       g_astNmParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucDataType,
                                       g_astNmParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucIsSupported,
                                       pstMcp->ucCmdId);
                    break;
                }
            }

            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        case MOBJ_ID_ALARM_ENA_SET:
            if(pstMObj->aucMObjId[LO_BYTE] <= MAX_ALARM_ITEM_ID)
            {
                for(j = 0; j < pstMObj->aucMObjId[LO_BYTE]; j++) //因为告警项采用紧缩排列，因此ucIDLoByte肯定大等于对应的下标
                {
                    if(g_astAlarmEnaHndlrTbl[j].ucIDLoByte == pstMObj->aucMObjId[LO_BYTE])
                    {
                        pfHndlr = g_astAlarmEnaHndlrTbl[j].pfHandler;
                        if(pfHndlr)
                        {
                            lRspFlag = pfHndlr(pstMObj, 
                                               g_astAlarmEnaHndlrTbl[j].ulAddr,
                                               g_astAlarmEnaHndlrTbl[j].ucDataType,
                                               g_astAlarmEnaHndlrTbl[j].ucIsSupported,
                                               pstMcp->ucCmdId);
                        }
                        else
                        {
                            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                        }
                        break;
                    }
                }
                if(j == pstMObj->aucMObjId[LO_BYTE])
                {
                    SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                }
            }                       
            else
            {
                SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            }
            break;
        case MOBJ_ID_ALARM_STATUS_SET:
            if(pstMObj->aucMObjId[LO_BYTE] <= MAX_ALARM_ITEM_ID)
            {
                for(j = 0; j < pstMObj->aucMObjId[LO_BYTE]; j++) //因为告警项采用紧缩排列，因此ucIDLoByte肯定大等于对应的下标
                {
                    if(g_astAlarmStatusHndlrTbl[j].ucIDLoByte == pstMObj->aucMObjId[LO_BYTE])
                    {
                        pfHndlr = g_astAlarmStatusHndlrTbl[j].pfHandler;
                        if(pfHndlr)
                        {
                            lRspFlag = pfHndlr(pstMObj, 
                                               g_astAlarmStatusHndlrTbl[j].ulAddr,
                                               g_astAlarmStatusHndlrTbl[j].ucDataType,
                                               g_astAlarmStatusHndlrTbl[j].ucIsSupported,
                                               pstMcp->ucCmdId);
                        }
                        else
                        {
                            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                        }
                        break;
                    }
                }
                if(j == pstMObj->aucMObjId[LO_BYTE])
                {
                    SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                }
            }                       
            else
            {
                SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            }
            break;
        case MOBJ_ID_SETTING_PARAM_SET:
            if(pstMObj->aucMObjId[LO_BYTE] < SETTING_PARAMS_HNDLR_TBL_SIZE)
            {
                pfHndlr = g_astSettingParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].pfHandler;
                if(pfHndlr)
                {
                    lRspFlag = pfHndlr(pstMObj, 
                                       g_astSettingParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ulAddr,
                                       g_astSettingParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucDataType,
                                       g_astSettingParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucIsSupported,
                                       pstMcp->ucCmdId);
                    break;
                }
            }

            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        case MOBJ_ID_RT_COLL_PARAM_SET:
            if(pstMObj->aucMObjId[LO_BYTE] < RT_COLL_PARAMS_HNDLR_TBL_SIZE)
            {
                pfHndlr = g_astRtCollParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].pfHandler;
                if(pfHndlr)
                {
                    lRspFlag = pfHndlr(pstMObj, 
                                       g_astRtCollParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ulAddr,
                                       g_astRtCollParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucDataType,
                                       g_astRtCollParamHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucIsSupported,
                                       pstMcp->ucCmdId);
                    break;
                }
            }

            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        case MOBJ_ID_DEV_INFO_SET_EX:
            if(pstMObj->aucMObjId[LO_BYTE] == MOBJ_ID_DI_MOBJ_TABLE) //查询监控参量列表做特殊处理
            {
                pstMcp->ucRspFlag = (UCHAR)HandleMcpAMObjListWrite(pstMObj);
                
                break;
            }            
            if(pstMObj->aucMObjId[LO_BYTE] < DEV_INFO_HNDLR_TBL_SIZE)
            {
                pfHndlr = g_astDevInfoExHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].pfHandler;
                if(pfHndlr)
                {
                    lRspFlag = pfHndlr(pstMObj,
                                       g_astDevInfoExHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ulAddr,
                                       g_astDevInfoExHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucDataType,
                                       g_astDevInfoExHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucIsSupported,
                                       pstMcp->ucCmdId);
                    break;
                }
            }

            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        default: //系统保留或厂家自定义
            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        }

        if(lRspFlag != RSP_FLAG_SUCCEEDED)
        {
            pstMcp->ucRspFlag = (UCHAR)lRspFlag;
        }

        //进入下一个监控对象
        lMcpDataLen -= pstMObj->ucMObjLen;
        ulActualLen  += pstMObj->ucMObjLen;
        pstMObj = (MCPA_MOBJ_ST *)(((UCHAR *)pstMObj) + pstMObj->ucMObjLen);     
    }
    
    *pulMcpDataLen = ulActualLen;

    //将参数保存到NV Memory中
    SaveReadOnlyParam();
    SaveWritableParam();
    SaveAlarmCenterStatus();

    return SUCCEEDED;
}

/*************************************************
  Function:
  Description:    本函数初始化升级过程的上下文中的状态，并
                  准备接收后续的文件数据包，任何状态下收到
                  开始传输的指示都会重新准备传输
  Calls:
  Called By:
  Input:          g_astFilePartTbl: 程序文件分区                  
  Output:         g_stUpgradeCtx:   升级文件用到的上下文
  Return:         
  Others:         
*************************************************/
void StartFileTransmission()
{
    //初始化升级过程的上下文的状态
    g_stUpgradeCtx.stFileBuf.ulUsedSize = 0; //缓存区清空
    g_stUpgradeCtx.ulUpgradeProgress    = TRANS_CTRL_START;
    g_stUpgradeCtx.lUpgradeResult       = UPGRADE_DONE;

    //MCM-50_20070105_zhonghw_begin
    g_stUpgradeCtx.ulUnSaveBlocks = 0;      
    //MCM-50_20070105_zhonghw_end
    //启动定时器
    StartUpgradeTimer(TIMER_TRANS_CTRL_ID, TIMER_TRANS_CTRL_INTV);
}

/*************************************************
  Function:
  Description:    本函数将文件缓存区中的数据写入FLASH，每次都写一个sector
  Calls:
  Called By:
  Input:          g_astFilePartTbl: 程序文件分区
                  g_stUpgradeCtx: 升级文件用到的上下文
  Output:         
  Return:         SUCCEEDED表示成功，其它表示失败
  Others:         
*************************************************/
LONG WriteFileBlockToFlash()
#ifndef M3
{
    if(SimpleFlashEraseSector(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr
                            + g_stUpgradeCtx.ulSavedFileSize) != SUCCEEDED)
    {
        return FAILED;
    }

    if(SimpleFlashWrite(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr
                      + g_stUpgradeCtx.ulSavedFileSize,
                        g_stUpgradeCtx.stFileBuf.aucBuf,
                        g_stUpgradeCtx.stFileBuf.ulUsedSize) != SUCCEEDED)
    {
        return FAILED;
    }
    return  SUCCEEDED;
}
#else
{
    return SaveProgramFile(g_stUpgradeCtx.ulUpgradePartIdx,
                           g_stUpgradeCtx.ulSavedFileSize,
                           g_stUpgradeCtx.stFileBuf.aucBuf,
                           g_stUpgradeCtx.stFileBuf.ulUsedSize);
}
#endif

/*************************************************
  Function:
  Description:    本函数保存下载的文件数据块，当存满文件缓存区时就写入FLASH
                  否则等待下一个数据块，接收文件数据块时可以使用应答返回错误
  Calls:
  Called By:
  Input:          pucData:   文件数据块
                  ulDataLen: 数据块长度，这里是文件块的长度，不包含前面的L和T，由
                             调用方保证文件块长度不超过最大长度
                  ulSn:      当前数据块的SN
  Output:         g_stUpgradeCtx: 升级过程的上下文
  Return:         TRANS_RSP_OK:   成功的应答
                  TRANS_RSP_CANCEL_REQ: 请求取消升级
  Others:         FLASH_SECTOR_SIZE肯定是数据包长度ulDataLen的整数倍
*************************************************/
LONG RecvFileBlock(UCHAR *pucData, ULONG ulDataLen, ULONG ulSn)
{
    //判断状态
    if(g_stUpgradeCtx.ulUpgradeProgress == TRANS_CTRL_START)
    {
        g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_TRANSFERRING;

    }

    //停止定时器
    StopUpgradeTimer();
    if(g_stUpgradeCtx.ulUpgradeProgress != TRANS_CTRL_TRANSFERRING)
    {
        //其它状态下收到该数据包均在应答中请求取消升级，但是其它状态都不变
        //g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_ABORT;
        //g_stUpgradeCtx.lUpgradeResult    = UPGRADE_ABORTED;

        //MCM-50_20070105_zhonghw_begin
        //回退数据包
        g_stUpgradeCtx.ulNextBlockSn -= g_stUpgradeCtx.ulUnSaveBlocks;
        g_stUpgradeCtx.ulUnSaveBlocks = 0;                        
        //MCM-50_20070105_zhonghw_end
        
    return TRANS_RSP_CANCEL_REQ;
    }

    if(ulSn == 0) //SN从0开始
    {
        //如果重新开始下载，则初始化Next SN和SavedFileSize
        g_stUpgradeCtx.ulNextBlockSn = 0;
        g_stUpgradeCtx.ulSavedFileSize = 0;
        
        //MCM-50_20070105_zhonghw_begin
        g_stUpgradeCtx.ulUnSaveBlocks = 0;
        //MCM-50_20070105_zhonghw_end
    }
    
    //如果数据包长度不足，说明是最后一个包，先将其放入缓存，再收到文件传输正常结束的时候将其写入flash
    

    //将数据放入文件缓存区
    memcpy(g_stUpgradeCtx.stFileBuf.aucBuf + g_stUpgradeCtx.stFileBuf.ulUsedSize, 
           pucData, ulDataLen);
    g_stUpgradeCtx.stFileBuf.ulUsedSize += ulDataLen;

    g_stUpgradeCtx.ulNextBlockSn++;
    
    //MCM-50_20070105_zhonghw_begin
    g_stUpgradeCtx.ulUnSaveBlocks++;
    //MCM-50_20070105_zhonghw_end

    if(g_stUpgradeCtx.stFileBuf.ulUsedSize >= FLASH_SECTOR_SIZE)
    {
        //缓存满，写入FLASH
        if(WriteFileBlockToFlash() != SUCCEEDED)
        {
            //g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_ABORT;
            g_stUpgradeCtx.lUpgradeResult = UPGRADE_SAVE_FILE_FAILED;
            
            //MCM-50_20070105_zhonghw_begin
            //回退数据包
            g_stUpgradeCtx.ulNextBlockSn -= g_stUpgradeCtx.ulUnSaveBlocks;
            g_stUpgradeCtx.ulUnSaveBlocks = 0;                        
            //MCM-50_20070105_zhonghw_end
            
            return TRANS_RSP_CANCEL_REQ;
        }

        //MCM-50_20070105_zhonghw_begin
        g_stUpgradeCtx.ulUnSaveBlocks = 0;
        //MCM-50_20070105_zhonghw_end
        
        g_stUpgradeCtx.stFileBuf.ulUsedSize = 0; //缓存区清空

        g_stUpgradeCtx.ulSavedFileSize += FLASH_SECTOR_SIZE; //更新已写入FLASH的文件大小

        //MCM-50_20070105_Zhonghw_begin
        //保存升级信息，防止过程中掉电导致升级信息未被保存
        SaveUpgradeInfo();
        //MCM-50_20070105_Zhonghw_end        
    }

    //重新启动定时器
    StartUpgradeTimer(TIMER_TRANS_BLK_ID, TIMER_TRANS_BLK_INTV);

    return TRANS_RSP_OK; 
}

/*************************************************
  Function:
  Description:    本函数在文件下载完毕后调用，将文件数据从FLASH中
                  读出，并计算校验码，用于和文件标识中的校验码进行
                  比较
  Calls:
  Called By:
  Input:          g_astFilePartTbl: 文件分区表
  Output:         g_stUpgradeCtx:   升级过程的上下文
  Return:         SUCCEEDED:        校验成功
                  FAILED:           校验失败
  Others:         
*************************************************/
LONG ValidateFile()
{
/*
    //文件标识码中的前两字节时文件CRC校验码，低字节在前，高字节在后
    USHORT usCrcInFileID = g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].aucFileId[0] 
                        + (g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].aucFileId[1] << 8);

    //将文件从FLASH中读出并分段进行校验，直接利用下载文件时的缓冲区
    ULONG ulCheckedFileSize = 0;
    USHORT usCrc = 0;

    //一个一个sector数据的读取，计算校验值
    while((LONG)(g_stUpgradeCtx.ulSavedFileSize - ulCheckedFileSize) >= FLASH_SECTOR_SIZE)
    {
        SimpleFlashRead(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr + ulCheckedFileSize,
                        g_stUpgradeCtx.stFileBuf.aucBuf, 
                        FLASH_SECTOR_SIZE);

        usCrc = CalcuCRC(g_stUpgradeCtx.stFileBuf.aucBuf, FLASH_SECTOR_SIZE, usCrc);
        ulCheckedFileSize += FLASH_SECTOR_SIZE;
    }

    //不足一个sector数据的读取，计算校验值
    if(g_stUpgradeCtx.ulSavedFileSize > ulCheckedFileSize)
    {
        SimpleFlashRead(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr + ulCheckedFileSize,
                        g_stUpgradeCtx.stFileBuf.aucBuf, 
                        g_stUpgradeCtx.ulSavedFileSize - ulCheckedFileSize);

        usCrc = CalcuCRC(g_stUpgradeCtx.stFileBuf.aucBuf, FLASH_SECTOR_SIZE, usCrc);
    }

    //比较文件校验码
    if(usCrcInFileID == usCrc)
    {
        return SUCCEEDED;
    }
    else
    {
        return FAILED;
    }
*/
//test
return SUCCEEDED;
}

/*************************************************
  Function:
  Description:    本函数结束文件下载过程，将文件尾写入FLASH，同时校验文件，
                  如果出现错误无法在应答中返回，只能等到升级完成后上报升级
                  结果
  Calls:
  Called By:
  Input:          g_astFilePartTbl: 文件分区表
                  g_stUpgradeCtx:   升级过程的上下文
                  
  Output:         
  Return:         
  Others:         FLASH_SECTOR_SIZE肯定是数据包长度ulDataLen的整数倍
*************************************************/
void EndFileTransmission()
{
    //除了UPGRADE_COMPLETE,ABORT,CANCEL外的状态下接收到END都可以处理，而前三个状态都是暂态
    //停止定时器
    StopUpgradeTimer();

    //更新下载过程指示
    g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_END;

    //将文件尾写入FLASH
    if(WriteFileBlockToFlash() != SUCCEEDED)
    {
        g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_ABORT;
        g_stUpgradeCtx.lUpgradeResult    = UPGRADE_SAVE_FILE_FAILED;
        return;
    }

    g_stUpgradeCtx.ulSavedFileSize += g_stUpgradeCtx.stFileBuf.ulUsedSize;

    //校验文件
    if(ValidateFile() != SUCCEEDED)
    {
        g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_ABORT;
        g_stUpgradeCtx.lUpgradeResult    = UPGRADE_VALID_FILE_FAILED;
        return;
    }

    //将ulNextBlockSn, ulSavedFileSize, lUpgradeResult保存到NV Memory中
    //SimpleFlashWrite(ADDR_UPGRADE_INFO_SET, &g_stUpgradeCtx, sizeof(UPGRADE_INFO_SET_ST));

    //启动定时器
    StartUpgradeTimer(TIMER_TRANS_CTRL_ID, TIMER_TRANS_CTRL_INTV);
}

/*************************************************
  Function:
  Description:    本函数在收到OMC取消升级时调用，将取消前已下载的部分
                  保存到NV Memory，同时返回监控模式
  Calls:
  Called By:
  Input:          g_stUpgradeCtx: 升级过程的上下文
  Output:         
  Return:         
  Others:         
*************************************************/
void CancelFileTransmission()
{
    //停止定时器
    StopUpgradeTimer();
    
    //更新状态和结果，准备返回监控模式
    g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_CANCEL;
    g_stUpgradeCtx.lUpgradeResult    = UPGRADE_CANCELLED;

    //MCM-50_20070105_zhonghw_begin
    //回退数据包(适应OMC主动取消的情况)
    g_stUpgradeCtx.ulNextBlockSn -= g_stUpgradeCtx.ulUnSaveBlocks;
    g_stUpgradeCtx.ulUnSaveBlocks = 0;                        
    //MCM-50_20070105_zhonghw_end
    
    //将ulNextBlockSn, ulSavedFileSize, lUpgradeResult保存到NV Memory中
    //SimpleFlashWrite(ADDR_UPGRADE_INFO_SET, &g_stUpgradeCtx, sizeof(UPGRADE_INFO_SET_ST));
}

/*************************************************
  Function:
  Description:    本函数在收到升级结束标志都调用
  Calls:
  Called By:
  Input:          g_stUpgradeCtx: 升级过程的上下文
  Output:         
  Return:         
  Others:         
*************************************************/
void RecvUpgradeComplete()
{
    //停止定时器
    StopUpgradeTimer();
    
    if(g_stUpgradeCtx.ulUpgradeProgress != TRANS_CTRL_END)
    {
        //其它状态下收到该数据包均中断
        g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_UPGRADE_COMPLETE;
        g_stUpgradeCtx.lUpgradeResult    = UPGRADE_ABORTED;

        //MCM-50_20070105_zhonghw_begin
        //回退数据包(适应OMC误结束)
        g_stUpgradeCtx.ulNextBlockSn -= g_stUpgradeCtx.ulUnSaveBlocks;
        g_stUpgradeCtx.ulUnSaveBlocks = 0;                        
        //MCM-50_20070105_zhonghw_end         
        return;
    }

    g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_UPGRADE_COMPLETE;

    //将lUpgradeResult保存到NV Memory中，升级结果的上报在升级完成应答之后，由最外层处理
    //SimpleFlashWrite(ADDR_UPGRADE_INFO_SET, &g_stUpgradeCtx, sizeof(UPGRADE_INFO_SET_ST));
}

/*************************************************
  Function:
  Description:    本函数检查不同长度的整型监控量的长度是否正确
  Calls:
  Called By:
  Input:          pstMObj:       监控对象
                  ulExpectedLen: 定义的长度
  Output:         ucFlag:        该监控量是否错误，用于标识协议处理部分错误
  Return:         SUCCEEDED:     处理成功
                  FAILED:        处理失败
  Others:
*************************************************/
LONG McpBCheckInt(MCPB_MOBJ_ST *pstMObj, ULONG ulExpectedLen, UCHAR *ucFlag)
{
    if(pstMObj->usMObjLen != ulExpectedLen + 4) //加4表示长度字段和对象标识字段的总长度
    {
        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_LEN_NOT_MATCH, *ucFlag);
        return FAILED;
    }
    return SUCCEEDED;
}

/*************************************************
  Function:
  Description:    本函数构造文件传输的应答包，包含应答标志和SN
  Calls:
  Called By:
  Input:          pucData:   应答包开始的位置
                  ucRspFlag: 应答标志
                  ulSn:      当前文件数据块的SN
  Output:         
  Return:         应答包的长度
  Others:
*************************************************/
ULONG EncodeFileTransRsp(UCHAR *pucData, UCHAR ucRspFlag, ULONG ulSn)
{
    ULONG ulLen = 0;
    MCPB_MOBJ_ST *pstMObj = (MCPB_MOBJ_ST *)pucData;
    
    pstMObj->usMObjLen = MCPB_MOBJ_MIN_LEN + 1; //LT各占两个字节
    pstMObj->aucMObjId[LO_BYTE] = MOBJ_ID_UF_TRANS_RSP_FLAG;
    pstMObj->aucMObjId[HI_BYTE] = MOBJ_ID_UPGRADE_FILE_INFO_SET;
    pstMObj->aucMObjContent[0]  = ucRspFlag; 
    
    ulLen += pstMObj->usMObjLen;
    pstMObj = (MCPB_MOBJ_ST *)(pucData + ulLen);

    pstMObj->usMObjLen = MCPB_MOBJ_MIN_LEN + 4; //LT各占两个字节
    pstMObj->aucMObjId[LO_BYTE] = MOBJ_ID_UF_CURRENT_BLOCK_SN;
    pstMObj->aucMObjId[HI_BYTE] = MOBJ_ID_UPGRADE_FILE_INFO_SET;
    //*(ULONG *)pstMObj->aucMObjContent = ulSn;
    SET_DWORD(pstMObj->aucMObjContent, ulSn);
    ulLen += pstMObj->usMObjLen;

    return ulLen + 2;//20060421zhw
}

/*************************************************
  Function:
  Description:    本函数处理对监控对象的查询，外层函数保证MAP层数据的长度大于3，
                  如果处理失败会填写错误编码，所有的处理都是直接在传入的数据上
                  进行的
  Calls:
  Called By:
  Input:          pucMcpData:    MAP层数据单元指针
                  pulMcpDataLen: MAP层数据单元长度
  Output:         pulMcpDataLen: 返回处理后的MAP层数据长度
  Return:         SUCCEEDED表示可以进行后续处理，FAILED表示忽略该数据包
  Others:         如果是总的长度错误，则返回应答标志为RSP_FLAG_LEN_ERR，此时不处
                  理监控对象标号中的错误代码；如果是监控对象的长度错误，则填写错
                  误代码，且返回应答标志为RSP_FLAG_PARTIALLY_DONE
*************************************************/
LONG HandleMcpB(UCHAR* pucMcpData, ULONG *pulMcpDataLen)
{
    MCP_LAYER_ST *pstMcp  = (MCP_LAYER_ST *)pucMcpData;
    LONG lMcpDataLen      = (LONG)*pulMcpDataLen;
    ULONG ulActualLen     = 0; //用于返回处理后的实际长度
    MCPB_MOBJ_ST *pstMObj = NULL;
    ULONG ulCurrentSn     = 0; //用于记录当前文件数据块SN
    BOOL IsSameFileID     = TRUE; //当FILE ID相同时可以返回记录的断点信息

    if(pstMcp->ucRspFlag != RSP_FLAG_CMD)
    {
        return FAILED; //如果应答标志不正确则不应答
    }

    if((pstMcp->ucCmdId != MCPB_CMD_ID_READ) && (pstMcp->ucCmdId != MCPB_CMD_ID_WRITE))
    {
        pstMcp->ucRspFlag = RSP_FLAG_CMD_ID_ERR;
        return SUCCEEDED;
    }

    pstMcp->ucRspFlag = RSP_FLAG_SUCCEEDED;

    pstMObj = (MCPB_MOBJ_ST *)(pstMcp->aucContent);
    lMcpDataLen -= 2; //扣除命令单元长度
    ulActualLen += 2;
    while(lMcpDataLen >= MCPB_MOBJ_MIN_LEN) //L和T各占2字节
    {
        if(pstMObj->usMObjLen < MCPB_MOBJ_MIN_LEN) //监控对象长度不够，之后的数据就不再处理
        {
            pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR;
            return SUCCEEDED;
        }

        if(pstMObj->usMObjLen > lMcpDataLen) //超出实际数据长度
        {
            pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR;
            return SUCCEEDED;
        }

        switch(pstMObj->aucMObjId[HI_BYTE] & 0xF) //高字节表示参数分类，只取低四位
        {
        case MOBJ_ID_UPGRADE_INFO_SET: //进入这个分支的都当成读操作处理
            switch(pstMObj->aucMObjId[LO_BYTE])
            {
            case MOBJ_ID_UI_RUNNING_MODE: //可读，不可写
                if(McpBCheckInt(pstMObj, UINT_1, &pstMcp->ucRspFlag) == SUCCEEDED)
                {
                    pstMObj->aucMObjContent[0] = g_stDevInfoSet.ucRunningMode;
                }
                break;
            case MOBJ_ID_UI_NEXT_BLOCK_SN: //可读，不可写
                if(McpBCheckInt(pstMObj, UINT_4, &pstMcp->ucRspFlag) == SUCCEEDED)
                {
                    if(IsSameFileID)
                    {
                        //*(ULONG *)(pstMObj->aucMObjContent) = ADJUST_DWORD(g_stUpgradeCtx.ulNextBlockSn);
                        SET_DWORD(pstMObj->aucMObjContent, g_stUpgradeCtx.ulNextBlockSn);
                    }
                    else
                    {
                        //*(ULONG *)(pstMObj->aucMObjContent) = ADJUST_DWORD(1); //如果文件ID不同，则需要重新发送
                        SET_DWORD(pstMObj->aucMObjContent, 0);
                    }
                }
                break;
            case MOBJ_ID_UI_MAX_BLOCK_SIZE: //可读，不可写
                if(McpBCheckInt(pstMObj, UINT_2, &pstMcp->ucRspFlag) == SUCCEEDED)
                {
                    //*(USHORT *)(pstMObj->aucMObjContent) = (USHORT)g_stUpgradeCtx.ulMaxBlockSize;
                    SET_WORD(pstMObj->aucMObjContent, (USHORT)g_stUpgradeCtx.ulMaxBlockSize);
                }

                //由于获取断点信息固定包含0x0301,0x0202,0x0203，因此在此启动定时器
                StartUpgradeTimer(TIMER_TRANS_CTRL_ID, TIMER_TRANS_CTRL_INTV);
                break;
            default:
                SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                break;
            }
            break;
        case MOBJ_ID_UPGRADE_FILE_INFO_SET:
            switch(pstMObj->aucMObjId[LO_BYTE])
            {
            case MOBJ_ID_UF_FILE_ID: //可读可写，但是南向接口规范中表9-55中规定是写命令
                if(McpBCheckInt(pstMObj, STR_20, &pstMcp->ucRspFlag) == SUCCEEDED)
                {
                    if(pstMcp->ucCmdId == MCPB_CMD_ID_READ)
                    {
                        memcpy(pstMObj->aucMObjContent, 
                               g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].aucFileId,
                               STR_20);
                    }
                    else
                    {
                        //比较是否和保存的相同
                        for(LONG i = 0; i < STR_20; i++)
                        {
                            if(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].aucFileId[i]
                            != pstMObj->aucMObjContent[i])
                            {
                                IsSameFileID = FALSE;

                                //将新的文件ID写入
                                memcpy(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].aucFileId,
                                       pstMObj->aucMObjContent, STR_20);
                                break;
                            }
                        }

                        //IsSameFileID = TRUE;
                    }
                }
                break;
            case MOBJ_ID_UF_TRANS_CTRL_BYTE: //存在于在写命令中，读没有意义，因此不需要保存
                if(McpBCheckInt(pstMObj, UINT_1, &pstMcp->ucRspFlag) == SUCCEEDED)
                {
                    switch(pstMObj->aucMObjContent[0])//20060417zhw
                    {
                    case TRANS_CTRL_START: //文件传输开始
                        StartFileTransmission();
                        break;
                    case TRANS_CTRL_END: //文件传输结束
                        EndFileTransmission();
                        break;
                    case TRANS_CTRL_CANCEL: //取消文件传输，需要返回监控模式
                        CancelFileTransmission();
                        break;
                    case TRANS_CTRL_UPGRADE_COMPLETE: //升级完成，可以（重启）返回监控模式，上报升级结果
                        RecvUpgradeComplete();
                        break;
                    default:
                        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_VALUE_OUT_OF_BOUND, pstMcp->ucRspFlag);
                        break;
                    }
                }
                break;
            case MOBJ_ID_UF_TRANS_RSP_FLAG: //存在于写命令中，由下位机填写，读没有意义，因此不需要保存
                McpBCheckInt(pstMObj, UINT_1, &pstMcp->ucRspFlag);                
                break;
            case MOBJ_ID_UF_CURRENT_BLOCK_SN: //存在于写命令中，读没有意义，因此不需要保存
                if(McpBCheckInt(pstMObj, UINT_4, &pstMcp->ucRspFlag) == SUCCEEDED)
                {
                    //ulCurrentSn = ADJUST_DWORD(*(ULONG *)(pstMObj->aucMObjContent));
                    ulCurrentSn = GET_DWORD(pstMObj->aucMObjContent);
                    
                    //判断是否和下一数据序号相同，或者是否是第一包
                    if((ulCurrentSn != 0) && (ulCurrentSn != g_stUpgradeCtx.ulNextBlockSn))
                    {
                        //不进行后续处理，直接返回应答包
                        *pulMcpDataLen = EncodeFileTransRsp(pstMcp->aucContent, 
                                                            TRANS_RSP_CANCEL_REQ, //取消本次升级
                                                            ulCurrentSn);
                        //MCM-50_20070105_zhonghw_begin
                        //回退数据包(适应OMC误发送)
                        g_stUpgradeCtx.ulNextBlockSn -= g_stUpgradeCtx.ulUnSaveBlocks;
                        g_stUpgradeCtx.ulUnSaveBlocks = 0;                        
                        //MCM-50_20070105_zhonghw_end                        

                        return SUCCEEDED;
                    }
                }               
                break;
            case MOBJ_ID_UF_FILE_BLOCK: //存在于写命令中，读没有意义，因此不需要保存
                if(pstMObj->usMObjLen <= MCPB_MOBJ_MIN_LEN + g_stUpgradeCtx.ulMaxBlockSize) //LT各占两个字节
                {
                    UCHAR ucRspFlag = (UCHAR)RecvFileBlock(pstMObj->aucMObjContent, pstMObj->usMObjLen - MCPB_MOBJ_MIN_LEN, ulCurrentSn);
                    *pulMcpDataLen = EncodeFileTransRsp(pstMcp->aucContent, ucRspFlag, ulCurrentSn);
                    return SUCCEEDED;
                }
                else
                {
                    SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_LEN_NOT_MATCH, pstMcp->ucRspFlag);
                }
                
                break;
            default:
                SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                break;
            }
            break;
        default: //系统保留或厂家自定义
            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            break;
        }

        //进入下一个监控对象
        lMcpDataLen -= pstMObj->usMObjLen;
        ulActualLen  += pstMObj->usMObjLen;
        pstMObj = (MCPB_MOBJ_ST *)(((UCHAR *)pstMObj) + pstMObj->usMObjLen);     
    }   

    *pulMcpDataLen = ulActualLen;

    //需要启动定时器，等待下一个数据包的到来，目前定时器都在处理传输控制和数据块的函数中启动

    //将参数保存到NV Memory中
    SaveUpgradeInfo();
    return SUCCEEDED;
}

/*************************************************
  Function:       
  Description:    本函数处理经过起始结束标志判断后的完整的数据包，包含
                  起始结束标志
  Calls:          
  Called By:      
  Input:          pucApData:  数据指针，指向起始标志
                  pulDataLen: 数据长度
                  ulDataLenMax: 数据的最大长度，由外层申请的内存大小决定
                  ucCommMode: 通信模式，目前暂时不用
  Output:         pulDataLen: 将更新后的数据长度传出
                  pucDir:     发送数据的方向，北向或南向
  Return:         FORWARD_UNDESTATE表示转发
                  SUCCEEDED_DUBUSTATE表示处理成功
                  FAILED表示处理失败
                  NEED_MORE_OP表示需要进行后续处理
  Others:         
*************************************************/
LONG HandleCMCCProtoData(UCHAR *pucApData, ULONG *pulDataLen, ULONG ulDataLenMax, UCHAR ucCommMode, UCHAR *pucDir)
{
    LONG lApType           = FAILED;
    LONG lResult           = SUCCEEDED;
    ULONG ulDataLen        = *pulDataLen;
    ULONG ulRawApDataLen   = 0;
    UCHAR *pucVpData       = NULL;
    ULONG ulVpDataLen      = 0;
    UCHAR *pucMcpData      = NULL;
    ULONG ulMcpDataLen     = 0;
    VPA_LAYER_ST *pstVpData= NULL;
    
    //AP层数据解码
    lApType = DecodeApData(pucApData + 1, ulDataLen - 2, //跳过起始标志，扣除起始结束标志的长度
                           &ulRawApDataLen, &pucVpData, &ulVpDataLen);
    if(lApType == FAILED)
    {
        return FAILED;
    }

    //VP层数据解码
    lResult = DecodeVpData(pucVpData, ulVpDataLen, &pucMcpData, &ulMcpDataLen);
    if(lResult == NEED_TO_FORWARD) //需要向南向接口转发
    {
        ulDataLen = EncodeApData(pucApData + 1, ulRawApDataLen - CRC_LEN, 
                                 ((AP_LAYER_ST *)(pucApData + 1))->ucApType, ulDataLenMax);
        if(ulDataLen == 0)
        {
            return FAILED;
        }
        else
        {
            *pulDataLen = ulDataLen;
            *pucDir = COMM_IF_SOUTH;
            return FORWARD_UNDESTATE;     //调试模式表示未调进来未调返回
                                          //UNDESTATE不能与FAILED值相同
                                          //在移动模式下表示需要转发
                                          //在调试模式下表示未调试返回
        }
    }
    else if(lResult == IGNORE)
    {
        return FAILED;
    }

    //处理MCP层数据
    pstVpData = (VPA_LAYER_ST *)pucVpData;
    if(g_stDevInfoSet.ucRunningMode == RUNNING_MODE_MONITOR)
    {
        if(pstVpData->ucApId == MCP_A)
        {
            if(HandleMcpA(pucMcpData, &ulMcpDataLen, (UCHAR)lApType, pstVpData->usPacketID) == IGNORE)
            {
                return FAILED;
            }
            
            //判断是否需要进行异步操作
            if((g_stTDParam.usParamLen != 0) || (g_stSECParam.usParamLen != 0))
            {
                UCHAR ucIdx = 0;
                if(GetAsynOpCtx(ucIdx) == SUCCEEDED) //如果上下文满则直接组包返回失败的应答
                {
                    //预占上下文
                    g_astAsynOpCtx[ucIdx].ucState = OH_SEND_DATA_PENDING;

                    //注册异步操作的处理函数和变量
                    if(g_stTDParam.usParamLen != 0)
                    {
                        g_astAsynOpCtx[ucIdx].astOp[g_stTDParam.ucOpObj - 1].pfOp = DoAsynOpTD;
                        g_astAsynOpCtx[ucIdx].astOp[g_stTDParam.ucOpObj - 1].pvCtx= &g_astAsynOpCtx[ucIdx];
                        g_astAsynOpCtx[ucIdx].pfTo = AsynOpToCommon;
                    }

                    if(g_stSECParam.usParamLen != 0)
                    {
                        g_astAsynOpCtx[ucIdx].astOp[g_stSECParam.ucOpObj - 1].pfOp = DoAsynOpSEC;
                        g_astAsynOpCtx[ucIdx].astOp[g_stSECParam.ucOpObj - 1].pvCtx= &g_astAsynOpCtx[ucIdx];
                        g_astAsynOpCtx[ucIdx].pfTo = AsynOpToCommon;
                    }

                    *pulDataLen = ulRawApDataLen + 2; //AP层数据长度加上起始结束标志
                    return NEED_MORE_OP; //需要进行异步操作的暂时不应答
                }
            }
        }
        else
        {
            return FAILED; //忽略不处理
        }
    }    
    else
    {
        if(pstVpData->ucApId == MCP_B)
        {
            if(HandleMcpB(pucMcpData, &ulMcpDataLen) == FAILED)
            {
                return FAILED;
            }
        }
        else
        {
            return FAILED; //忽略不处理
        }
    }
    
    //VP层数据编码，主要针对设置站点编号和设备编号的情况，需要更新相应字段
    pstVpData->ulStaNum = g_stNmParamSet.ulStaNum;
    pstVpData->ucDevNum = g_stNmParamSet.ucDevNum;

    //AP层数据编码
    ulRawApDataLen = ulMcpDataLen + AP_OVERHEAD + VP_OVERHEAD - CRC_LEN; //4和9分别是AP和NP层的开销，要扣除CRC长度
    
    //pucData指向起始标志
    ulDataLen = EncodeApData(pucApData + 1, ulRawApDataLen, 
                             ((AP_LAYER_ST *)(pucApData + 1))->ucApType, ulDataLenMax);

    if(ulDataLen == 0)
    {
        return FAILED;
    }

    //向北向接口发送协议数据
    *pulDataLen = ulDataLen;
    *pucDir = COMM_IF_NORTH;

     return SUCCEEDED_DEBUSTATE;          //调试模式表示未调进来已调返回
                                          //UNDESTATE不能与FAILED，FORWARD_UNDESTATE值相同
                                          //在移动模式下表示成功处理
                                          //在调试模式下表示已调返回
}

//本函数比较两个电话号码，比较时要去掉电话号码的前缀，SUCCEEDED表示合法，否则表示不合法
LONG ValidateTelNum(CHAR *pcTelNum1, CHAR *pcTelNum2)
{
    ULONG ulLen1,ulLen2,ulLen;
    ulLen1 = FindChar((UCHAR*)pcTelNum1, 0, 1, TEL_NUM_LEN);
    ulLen2 = FindChar((UCHAR*)pcTelNum2, 0, 1, TEL_NUM_LEN);
    
    if((ulLen1 == FIND_NO_CHAR) || (ulLen2 == FIND_NO_CHAR))
    {
        return FAILED;
    }

    if((ulLen1 == 0) || (ulLen2 == 0))
    {
        return FAILED;
    }
    //取长度较小的
    if(ulLen1 >= ulLen2)
    {
        ulLen = ulLen2;
    }
    else
    {
        ulLen = ulLen1;
    }
    //从后往前比较
    ULONG i1 = ulLen1;
    ULONG i2 = ulLen2;
    for(ULONG i = ulLen ; i > 0; i--,i1--,i2--)
    {
        if(pcTelNum1[i1-1] != pcTelNum2[i2-1])
        {
            return FAILED;
        }
    }
    
    return SUCCEEDED;
}
/*************************************************
  Function:       
  Description:    本函数处理从CH接收到的已调数据
  Calls:          
  Called By:      
  Input:          pstRecv: 接收数据的原语
  Output:         
  Return:         SUCCEEDED表示成功，FAILED表示失败
  Others:         本函数供外部调用
*************************************************/
LONG HndlOHCHDebgDataReq(OHCH_RECV_DATA_IND_ST *pstRecv)
{
    OHCH_SEND_DATA_REQ_ST *pstSend = (OHCH_SEND_DATA_REQ_ST *)pstRecv;
    OHCH_SEND_DATA_REQ_ST *pstSendScnd;
    if(g_stYkppParamSet.stYkppCtrlParam.ucDebugMode == TRUE)      //调试模式
    {
        pstSend->ucDstIf = COMM_IF_DEBUG;
        if(OSQPost(g_pstCHDebQue, pstSend)!=OS_NO_ERR)
        {
            MyFree((void*)pstSend);  //释放内存
            return FAILED;
        }
        return SUCCEEDED;
    }
    else
    {
        if(pstRecv->ucSrcIf == COMM_IF_NORTH)
        {
            pstSend->ucDstIf = COMM_IF_SOUTH; 
            if(OSQPost(g_pstCHSthQue, pstSend)!=OS_NO_ERR)
            {
                MyFree((void*)pstSend);
                return FAILED;
            }
            return SUCCEEDED;
        }
        else      //来自南向接口
        {
            //先拷贝数据
            pstSendScnd = (OHCH_SEND_DATA_REQ_ST *)MyMalloc(g_pstMemPool256);
            if(pstSendScnd != NULL)
            {
                memcpy(pstSendScnd,pstSend,sizeof(*pstSend)+pstSend->ulDataLen);
            }
            
            pstSend->ucDstIf = COMM_IF_SOUTH;         
            if(OSQPost(g_pstCHSthQue, pstSend)!=OS_NO_ERR)
            {
                MyFree((void*)pstSend); 
            }
            //发往北向接口            
            if(pstSendScnd != NULL)
            {
                pstSendScnd->ucDstIf = COMM_IF_NORTH;         
                if(OSQPost(g_pstCHNthQue, pstSendScnd)!=OS_NO_ERR)
                {
                    MyFree((void*)pstSendScnd); //写队列失败释放新申请的内存
                    return FAILED;
                }             
            }             
            return SUCCEEDED;
        }
    }
}

/*************************************************
  Function:       
  Description:    本函数处理从CH接收到的未调数据
  Calls:          
  Called By:      
  Input:          pstRecv: 接收数据的原语
  Output:         
  Return:         SUCCEEDED表示成功，FAILED表示失败
  Others:         本函数供外部调用
*************************************************/
LONG HndlOHCHDebgDataInd(OHCH_RECV_DATA_IND_ST *pstRecv)
{
    UCHAR ucDir = 0;
    UCHAR ucSrcIf = pstRecv->ucSrcIf;  //记录数据来源通信口
    OHCH_SEND_DATA_REQ_ST *pstSend = (OHCH_SEND_DATA_REQ_ST *)pstRecv;
    OHCH_SEND_DATA_REQ_ST *pstSendScnd; //需要发往两个队列时copy数据
    UCHAR *pucData = NULL;
    ULONG ulDataLen= 0;
    OS_EVENT * pstQueue =NULL;
    UCHAR SendIsBoth = FALSE;            //true(1)要发往南北双向    
    LONG lResult = 0;

    //如果在调试模式下收到来自非调试串口的未调数据则丢弃
    if((pstRecv->ulPrmvType == OHCH_UNDE_RECV_IND)&&(g_stYkppParamSet.stYkppCtrlParam.ucDebugMode == TRUE)
    &&(pstRecv->ucSrcIf !=COMM_IF_DEBUG))
    {
        return FAILED;
    }

    UCHAR *pucCmccData = NULL;
    ULONG ulCmccDataLen= 0;
    UCHAR *pucYkppData = NULL;
    ULONG ulYkppDataLen= 0;  

    //判断是否含有移动协议
    ExtractCMCCProtoData(pstRecv->pucData, pstRecv->ulDataLen, &pucCmccData, &ulCmccDataLen);

    //判断是否含有厂家协议
    ExtractYkppData(pstRecv->pucData, pstRecv->ulDataLen, pucYkppData, ulYkppDataLen);
    
    if((ulCmccDataLen > 0) && (ulYkppDataLen > 0)) //如果同时发现两种协议数据
    {
        if(pucCmccData < pucYkppData) //如果移动协议数据在前，则认为移动协议数据有效
        {
            ulYkppDataLen = 0;
            
        }
        else //如果移动协议数据在后，则认为邮科厂家协议数据有效
        {
            ulCmccDataLen = 0;
        }
    }
    
    if(ulCmccDataLen > 0) //处理移动协议数据
    {
        pucData = pucCmccData;
        ulDataLen = ulCmccDataLen;        
        lResult = HandleCMCCProtoData(pucCmccData, &ulDataLen, pstRecv->ulDataLenMax, pstRecv->ucCommMode, &ucDir);
    }
    
    else if(ulYkppDataLen > 0)//处理厂家协议数据
    {
        pucData = pucYkppData;
        ulDataLen = ulYkppDataLen;
        lResult = HandleYkppData(pucYkppData, ulDataLen, pstRecv->ucCommMode, ucDir);        
    }
    
    else
    {
        return FAILED;
    }

    //pstSend->ulPrmvType = OHCH_SEND_DATA_REQ;   //根据lResult决定不同值
    pstSend->ulMagicNum = 0;
    pstSend->ucResendTime = RESEND_TIME_DATATYPE_RSP;
    //pstSend->ucDstIf    = ucDir;                //根据lResult决定不同值
    //pstSend->ucCommMode; //不变
    //pstSend->ucDCS;      //不变
    //pstSend->acDstTelNum;//不变，原来SrcTelNum的位置
    pstSend->ucDataType = DATA_TYPE_RSP;
    pstSend->ulDataLen  = ulDataLen;
    pstSend->pucData    = pucData;

//    ucSendBoth = HandleDebugSendParam(lResult, ucSrcIf,pstSend,&g_pstQue);
    if(lResult == FORWARD_UNDESTATE)              //UNDESTATE表示未调
    {
        pstSend->ulPrmvType = OHCH_UNDE_SEND_REQ; 
        pstSend->ucDstIf = COMM_IF_SOUTH;//如果是未调数据，发南向是共性
        pstQueue = g_pstCHSthQue;
        
        if(g_stYkppParamSet.stYkppCtrlParam.ucDebugMode != TRUE)
        {
            if(ucSrcIf == COMM_IF_SOUTH)  
            {
                SendIsBoth = TRUE;
            }
        }
        else//调试模式
        {
            SendIsBoth = TRUE;
        }
    }
    else if(lResult == SUCCEEDED_DEBUSTATE) //表示数据已调
    {
        //发往原通信口
        pstSend->ulPrmvType = OHCH_DEBU_SEND_REQ; 
        pstSend->ucDstIf    = ucSrcIf;  
        if(ucSrcIf == COMM_IF_SOUTH)
        {
            pstQueue = g_pstCHSthQue;
        }
        else if(ucSrcIf == COMM_IF_NORTH)
        {
            pstQueue = g_pstCHNthQue;
        }
        else
        {
            pstQueue = g_pstCHDebQue;
        }
    }

    //因为以上已经确定了原语类型及发往南向或者原接口
    //所以此后可以直接进行写队列操作
    //如果判断到发南北向的标志才申请新的内存并执行发往北向的操作
    
    if((lResult == FORWARD_UNDESTATE)&&(SendIsBoth == TRUE))//确保未调状态才发往南北双向
    {   
        //申请新内存
        pstSendScnd = (OHCH_SEND_DATA_REQ_ST *)MyMalloc(g_pstMemPool256);
        if(pstSendScnd != NULL)
        {
            //复制原语及数据包至申请的内存
            memcpy(pstSendScnd,pstSend,sizeof(*pstSend)+pstSend->ulDataLen);
            //发往北向接口
            pstSendScnd->ucDstIf = COMM_IF_NORTH;
            if(OSQPost(g_pstCHNthQue, pstSendScnd) != OS_NO_ERR)
            {
                MyFree((void*)pstSendScnd); //释放新申请的内存
            }
        }
    }
    //直接发送
    if(OSQPost(pstQueue, pstSend) != OS_NO_ERR)
    {
        MyFree((void*)pstSend); //写南向队列不成功直接释放内存
        return FAILED;
    }
    return SUCCEEDED;  
}

/*************************************************
  Function:       
  Description:    本函数处理从CH接收到的数据
  Calls:          
  Called By:      
  Input:          pstRecv: 接收数据的原语
  Output:         
  Return:         SUCCEEDED表示成功，FAILED表示失败，NEED_MORE_OP表示需要后续处理
  Others:         本函数供外部调用
*************************************************/
LONG HndlOHCHRecvDataInd(OHCH_RECV_DATA_IND_ST * &pstRecv)
{
    UCHAR ucDir = 0;
    UCHAR *pucData = NULL;
    ULONG ulDataLen= 0;
    LONG lResult = 0;

    //为了防止在查询监控参量列表时CH申请的内存不够，因此统一在此重新申请大的内存块
    if((pstRecv->ucCommMode != COMM_MODE_SMS) && (pstRecv->ulDataLenMax < MEM_BLK_SIZE_256))
    {
        //申请内存
        OHCH_RECV_DATA_IND_ST *pstBuf = (OHCH_RECV_DATA_IND_ST *)MyMalloc(g_pstMemPool512);
        if(pstBuf != NULL)
        {
            memcpy(pstBuf, pstRecv, sizeof(OHCH_RECV_DATA_IND_ST));
            pstBuf->ulDataLenMax = MEM_BLK_SIZE_512 - 4 - sizeof(OHCH_RECV_DATA_IND_ST);
            pstBuf->pucData = (UCHAR *)pstBuf + sizeof(OHCH_RECV_DATA_IND_ST);
            memcpy(pstBuf->pucData, pstRecv->pucData, pstRecv->ulDataLen);

            MyFree(pstRecv);
            pstRecv = pstBuf;
        } //如果内存申请失败则继续往下执行
    }

    OHCH_SEND_DATA_REQ_ST *pstSend = (OHCH_SEND_DATA_REQ_ST *)pstRecv;

    UCHAR *pucCmccData = NULL;
    ULONG ulCmccDataLen= 0;
    UCHAR *pucYkppData = NULL;
    ULONG ulYkppDataLen= 0;

    //判断是否含有移动协议
    ExtractCMCCProtoData(pstRecv->pucData, pstRecv->ulDataLen, &pucCmccData, &ulCmccDataLen);

    //判断是否含有厂家协议
    ExtractYkppData(pstRecv->pucData, pstRecv->ulDataLen, pucYkppData, ulYkppDataLen);

    if((ulCmccDataLen > 0) && (ulYkppDataLen > 0)) //如果同时发现两种协议数据
    {
        if(pucCmccData < pucYkppData) //如果移动协议数据在前，则认为移动协议数据有效
        {
            ulYkppDataLen = 0;
        }
        else //如果移动协议数据在后，则认为邮科厂家协议数据有效
        {
            ulCmccDataLen = 0;
        }
    }
    
    if(ulCmccDataLen > 0) //处理移动协议数据
    {
        pucData = pucCmccData;
        ulDataLen = ulCmccDataLen;

        //如果是短消息则判断电话号码，CSD的号码检查放在CH处理更合适，即在来电时判断
        if(pstRecv->ucCommMode == COMM_MODE_SMS)
        {
            if(g_stYkppParamSet.stYkppCtrlParam.ucTelNumAuthSw != 0) //判断是否进行电话号码鉴权
            {
                if((ValidateTelNum(pstRecv->acSrcTelNum, g_stNmParamSet.acQnsTelNum1) != SUCCEEDED)
                && (ValidateTelNum(pstRecv->acSrcTelNum, g_stNmParamSet.acQnsTelNum2) != SUCCEEDED)
                && (ValidateTelNum(pstRecv->acSrcTelNum, g_stNmParamSet.acQnsTelNum3) != SUCCEEDED)
                && (ValidateTelNum(pstRecv->acSrcTelNum, g_stNmParamSet.acQnsTelNum4) != SUCCEEDED)
                && (ValidateTelNum(pstRecv->acSrcTelNum, g_stNmParamSet.acQnsTelNum5) != SUCCEEDED))
                {
                    return FAILED;
                }
            }
        }    

        //以下两个赋值是否必要
        //pstRecv->pucData   = pucCmccData;   //调整原语中的数据指针，使其指向起始标志
        //pstRecv->ulDataLen = ulDataLen;     //调整原语中的数据长度为包含起始结束标志的未进行AP层编码的数据长度
    
        //处理移动协议数据
        lResult = HandleCMCCProtoData(pucCmccData, &ulDataLen, pstRecv->ulDataLenMax, pstRecv->ucCommMode, &ucDir);

        //在登录成功后，只要收到数据就重启心跳周期定时器，同时将心跳超时次数清零
        if(g_stCenterConnStatus.ucLoginStatus == COMM_STATUS_LOGIN)
        {
            RestartHeartbeatPeriodTimer();

            g_stCenterConnStatus.ucNoRspCnt = 0;
        }
    }
    else if(ulYkppDataLen > 0) //处理厂家协议数据
    {
        pucData = pucYkppData;
        ulDataLen = ulYkppDataLen;
        lResult = HandleYkppData(pucYkppData, ulDataLen, pstRecv->ucCommMode, ucDir);
    }
    else
    {
        return FAILED;
    }
    
    if(lResult == NEED_MORE_OP)
    {
        pstRecv->pucData   = pucData;
        pstRecv->ulDataLen = ulDataLen; //调整数据长度
        return lResult;
    }
    else if(lResult == FAILED)
    {
        return lResult;
    }

    pstSend->ulPrmvType = OHCH_SEND_DATA_REQ;
    pstSend->ulMagicNum = 0;
    pstSend->ucResendTime = RESEND_TIME_DATATYPE_RSP;
    pstSend->ucDstIf    = ucDir;
    //pstSend->ucCommMode; //不变
    //pstSend->ucDCS;      //不变
    //pstSend->acDstTelNum;//不变，原来SrcTelNum的位置
    pstSend->ucDataType = DATA_TYPE_RSP;
    pstSend->ulDataLen  = ulDataLen;
    pstSend->pucData    = pucData;

    //将原语发送给CH
    if(ucDir == COMM_IF_NORTH)
    {
        if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
        {
            MyFree((void*)pstSend);             
            return FAILED;
        }
    }
    else
    {
        if(OSQPost(g_pstCHSthQue, pstSend) != OS_NO_ERR)
        {
            MyFree((void*)pstSend);         
            return FAILED;
        }
    }
    
    return SUCCEEDED;
}


/*************************************************
  Function:
  Description:    本函数对上报进行编码，但是只编码到“上报类型”监控对象，如果是
                  告警或升级上报，需要继续对其它监控参量进行编码
  Calls:
  Called By:
  Input:          pucBuf:   存放编码的内存，长度由调用方保证
                  event:    上报事件
                  commMode: 通信方式，用来决定使用何种AP协议
                  PID:      通信包标识
  Output:         pucBuf:   编码后的结果
  Return:         编码后的AP层的数据长度
  Others:
*************************************************/
ULONG EncodeReportIncludeReportTypeOnly(UCHAR *pucBuf, UCHAR ucReportType, 
                                        UCHAR ucCommMode, USHORT usPid)
{
    VPA_LAYER_ST* pstVpData  = NULL;
    MCP_LAYER_ST* pstMcpData = NULL;

    //AP层编码
    if(ucCommMode == COMM_MODE_SMS)
    {
        ((AP_LAYER_ST*)pucBuf)->ucApType = AP_B;
    }
    else if(ucCommMode == COMM_MODE_CSD)
    {
        ((AP_LAYER_ST*)pucBuf)->ucApType = AP_A;
    }
    else //gprs
    {
        ((AP_LAYER_ST*)pucBuf)->ucApType = AP_C;
    }

    ((AP_LAYER_ST*)pucBuf)->ucPduType = VP_A;

    //VP层编码
    pstVpData = (VPA_LAYER_ST*)(((AP_LAYER_ST *)pucBuf)->aucPdu);

    //1.站点编号和设备编号
    pstVpData->ulStaNum = g_stNmParamSet.ulStaNum;
    pstVpData->ucDevNum = g_stNmParamSet.ucDevNum;

    //2.通信包标号
    pstVpData->usPacketID = ADJUST_WORD(usPid);

    //3.VP层交互标志
    pstVpData->ucVpFlag = VP_FLAG_COMMAND;

    //4.应用协议标识
    pstVpData->ucApId = MCP_A;

    //MAP层编码
    pstMcpData = (MCP_LAYER_ST*)pstVpData->aucPdu;
    pstMcpData->ucCmdId       = MCPA_CMD_ID_REPORT;
    pstMcpData->ucRspFlag     = RSP_FLAG_CMD;
    pstMcpData->aucContent[0] = 4;                      //对象长度，固定为4
    pstMcpData->aucContent[1] = MOBJ_ID_NM_REPORT_TYPE; //对象标识，固定
    pstMcpData->aucContent[2] = MOBJ_ID_NM_PARAM_SET;
    pstMcpData->aucContent[3] = ucReportType;           //对象内容，上报类型

    return 17; //上报的数据长度为17（除去起始结束标志和告警项，CRC校验单元）
}

/*************************************************
  Function:
  Description:    本函数在上报命令的基础上增加升级结果和
                  文件版本号两个参量
  Calls:
  Called By:
  Input:          pucData:  开始编码的地方
                  ucResult: 升级的结果
                  g_stDevInfoSet.acSfwrVer: 当前软件版本号
  Output:         pucData:  编码的结果
  Return:         编码的长度
                  
  Others:
*************************************************/
ULONG EncodeUpgradeResult(UCHAR *pucData, UCHAR ucResult)
{
    ULONG ulLen = 0;
    MCPA_MOBJ_ST *pstMObj = (MCPA_MOBJ_ST *)pucData;
    
    pstMObj->ucMObjLen = MCPA_MOBJ_MIN_LEN + sizeof(g_stDevInfoSet.acSfwrVer); //LT共占三个字节
    pstMObj->aucMObjId[LO_BYTE] = MOBJ_ID_DI_SFWR_VER;
    pstMObj->aucMObjId[HI_BYTE] = MOBJ_ID_DEV_INFO_SET;
    memcpy(pstMObj->aucMObjContent, g_stDevInfoSet.acSfwrVer, sizeof(g_stDevInfoSet.acSfwrVer)); 
    
    ulLen += pstMObj->ucMObjLen;
    pstMObj = (MCPA_MOBJ_ST *)(pucData + ulLen);

    pstMObj->ucMObjLen = MCPA_MOBJ_MIN_LEN + 1; //LT共占三个字节
    pstMObj->aucMObjId[LO_BYTE] = MOBJ_ID_DI_UPGRADE_RESULT;
    pstMObj->aucMObjId[HI_BYTE] = MOBJ_ID_DEV_INFO_SET;
    pstMObj->aucMObjContent[0]  = ucResult;
    ulLen += pstMObj->ucMObjLen;
    
    return ulLen;
}

/*************************************************
  Function:
  Description:    本函数在上报之前调用，检查当前是否符合上报
                  的一般条件
  Calls:
  Called By:
  Input:          ucEvent:                        上报的事件
                  g_ucCurrCommMode:               当前通信方式
                  g_stDevInfoSet.ucRunningMode:   运行模式
                  g_stNmParamSet.ucCommMode:      通信方式
  Output:
  Return:         TRUE表示可以上报，FALSE表示不能上报         
  Others:         本函数供外部调用
*************************************************/
LONG IsReportAllowed(UCHAR ucEvent)
{
    //处在升级模式下则不处理
    if(g_stDevInfoSet.ucRunningMode != RUNNING_MODE_MONITOR)
    {
        return FALSE;
    }

    if(g_ucCurrCommMode == COMM_NO_CONN)
    {
        return TRUE; //如果串口直连方式则允许所有上报
    }

    if(g_ucCurrCommMode != g_stNmParamSet.ucCommMode)
    {
        return FALSE; //如果上报通信方式和目前的所处的通信状态不一致，则不上报
    }
    
    //GPRS方式下的上报除了登录和开站上报以外都要在登录完成后进行
    switch(g_stNmParamSet.ucCommMode)
    {
    case COMM_MODE_SMS:
        if((ucEvent == REPORT_TYPE_LOGIN) || (ucEvent == REPORT_TYPE_HEARTBEAT))
        {
            return FALSE; //登录和心跳上报必须在GPRS方式下进行
        }
        return TRUE;
    case COMM_MODE_CSD:
        if((ucEvent == REPORT_TYPE_LOGIN) || (ucEvent == REPORT_TYPE_HEARTBEAT))
        {
            return FALSE; //登录和心跳上报必须在GPRS方式下进行
        }

        if(ucEvent == REPORT_TYPE_ALARM)
        {
            return FALSE; //CSD方式下不能发送告警
        }
        return TRUE;
    case COMM_MODE_GPRS:
        if(g_stCenterConnStatus.ucLinkStatus != COMM_STATUS_CONNECTED)
        {
            return FALSE; //如果到中心的连接尚未建立则不能发送
        }

        if((ucEvent != REPORT_TYPE_STA_INIT) && (ucEvent != REPORT_TYPE_LOGIN))//开站上报和登录上报可以在未登录的情况下发送
        {
            if(g_stCenterConnStatus.ucLoginStatus != COMM_STATUS_LOGIN)
            {
                return FALSE;
            }
        }
        //通信从机不能发送登录包和心跳包
        if((g_stDevTypeTable.ucCommRoleType != COMM_ROLE_MASTER)&&((ucEvent == REPORT_TYPE_LOGIN)||(ucEvent == REPORT_TYPE_HEARTBEAT)))
        {
            return FALSE;
        }        
        return TRUE;
    default:
        return FALSE; //不应该进入这个分支
    }
}

/*************************************************
  Function:
  Description:    本函数提供其它上报服务，当上报事件被触发则调用本函数; 由底层负
                  责实际的通信方式
  Calls:
  Called By:
  Input:          ucEvent: 上报事件
                  ucResult:升级的结果，如果是上报升级结果的话
  Output:
  Return:         SUCCEED表示成功，否则表示失败
  Others:         本函数供外部调用
*************************************************/
LONG ReportOtherEvent(UCHAR ucEvent, UCHAR ucUpgradeResult)
{
    ULONG ulLen = 0;
    UCHAR *pucBuf = NULL;
    UCHAR *pucRawApData = NULL;
    OHCH_SEND_DATA_REQ_ST *pstSend = NULL;

    //判断是否可以上报
    if(IsReportAllowed(ucEvent) == FALSE)
    {
        return FAILED;
    }
    
    //申请包序号
    USHORT usPacketID = GEN_PACKET_ID();

    //申请内存
    pucBuf = (UCHAR *)MyMalloc(g_pstMemPool256);
    if(pucBuf == 0)
    {
        return FAILED;
    }

    //先构造原语
    pstSend = (OHCH_SEND_DATA_REQ_ST *)pucBuf;
    pstSend->ulPrmvType = OHCH_SEND_DATA_REQ;
    pstSend->ulMagicNum = usPacketID; //使用packet ID作为magic number
    pstSend->ucResendTime = RESEND_TIME_DATATYPE_REPORT;
    pstSend->ucDstIf    = COMM_IF_NORTH;
    pstSend->ucCommMode = g_stNmParamSet.ucCommMode;
    if(pstSend->ucCommMode == COMM_MODE_SMS)
    {
        pstSend->ucDCS      = 0; //暂时默认为GSM 7 bit编码，如果是短消息方式的话
        memcpy(pstSend->acDstTelNum, g_stNmParamSet.acReportTelNum, sizeof(pstSend->acDstTelNum));
    }
    pstSend->ucDataType = ucEvent; //DATA_TYPE_REPORT;
    
    pstSend->pucData    = pucBuf + DATA_START_POS; //为了提高编码效率，在内存块前留出足够原语编码的空间

    pucRawApData = pstSend->pucData + 1; //留出放起始标志的地方 
    
    ulLen = EncodeReportIncludeReportTypeOnly(pucRawApData, 
                                              ucEvent, g_stNmParamSet.ucCommMode,
                                              usPacketID);

    if(ucEvent == REPORT_TYPE_UPGRADE)
    {
        ulLen += EncodeUpgradeResult(pucRawApData + ulLen, ucUpgradeResult);
    }
    
    //发送上报的AP层处理
    ulLen = EncodeApData(pucRawApData, ulLen, 
                         ((AP_LAYER_ST *)pucRawApData)->ucApType, MEM_BLK_SIZE_256 - 4 - sizeof(OHCH_SEND_DATA_REQ_ST));
    if(ulLen == 0)
    {
        return FAILED;
    }
    pstSend->ulDataLen  = ulLen;
    
    //更新上下文
    g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].usPacketID = usPacketID;
    if(g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].ucCommState == OH_WAIT_FOR_RSP)
    {
        //停止定时器
        if(g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].pTimer) //如果原来有定时器需要先停止
        {
            OHRemoveTimer(g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].pTimer, 
                          g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].ulTimerMagicNum);
            g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].pTimer = 0;
            g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].ulTimerMagicNum = 0;
        }
    }
    g_astOtherReportCtx[ucEvent - OTHER_REPORT_BASE].ucCommState = OH_SEND_DATA_PENDING;    

    //将原语发送给CH
    if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
    {
        MyFree((void * )pstSend);
        return FAILED;
    }
    if((ucEvent == REPORT_TYPE_YKPP_STA_INIT)||(ucEvent == REPORT_TYPE_YKPP_INSPECTION)||
      (ucEvent == REPORT_TYPE_YKPP_REPAIR_CONFIRMED)||(ucEvent == REPORT_TYPE_YKPP_CONFIG_CHANGED))  
    {
        SetReportLampStatus(1);//调用亮灯程序指示上报动作已经开始
    }
    return SUCCEEDED;
}

/*************************************************
  Function:
  Description:    本函数在系统启动后检查之前是否进行过升级，如果有
                  则上报升级的结果，因此只有文件下载校验成功并重启
                  后才会使用本函数进行判断，且此时如果失败只可能是
                  因为bootloader引导不成功
  Calls:
  Called By:
  Input:          g_astFilePartTbl: 文件分区表
  Output:
  Return:         升级的结果
  Others:         本函数供外部调用
*************************************************/
UCHAR CheckUpgradeAfterStartup()
{
    UCHAR ucUpgradeResult = UPGRADE_NOT_OCCURED;
    
    if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucStatus == PROGRAM_STATUS_UPGRADE)
    {
        g_astFilePartTbl[PROGRAM_PART_1_IDX].ucStatus = PROGRAM_STATUS_RUNNABLE;

        //更新软件版本号
        memcpy(g_stDevInfoSet.acSfwrVer, 
               g_astFilePartTbl[PROGRAM_PART_1_IDX].acVersion, 
               sizeof(g_stDevInfoSet.acSfwrVer));
        
        //ReportOtherEvent(REPORT_TYPE_UPGRADE, UPGRADE_DONE);
        ucUpgradeResult = UPGRADE_DONE;
    }
    else if(g_astFilePartTbl[PROGRAM_PART_2_IDX].ucStatus == PROGRAM_STATUS_UPGRADE)
    {
        g_astFilePartTbl[PROGRAM_PART_2_IDX].ucStatus = PROGRAM_STATUS_RUNNABLE;

        //更新软件版本号
        memcpy(g_stDevInfoSet.acSfwrVer, 
               g_astFilePartTbl[PROGRAM_PART_2_IDX].acVersion, 
               sizeof(g_stDevInfoSet.acSfwrVer));

        //ReportOtherEvent(REPORT_TYPE_UPGRADE, UPGRADE_DONE);
        ucUpgradeResult = UPGRADE_DONE;
    }

    if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucStatus == PROGRAM_STATUS_UNRUNNABLE)
    {
        g_astFilePartTbl[PROGRAM_PART_1_IDX].ucStatus = PROGRAM_STATUS_INVALID;

        //ReportOtherEvent(REPORT_TYPE_UPGRADE, UPGRADE_ABORTED);
        ucUpgradeResult = UPGRADE_ABORTED;
    }
    else if(g_astFilePartTbl[PROGRAM_PART_2_IDX].ucStatus == PROGRAM_STATUS_UNRUNNABLE)
    {
        g_astFilePartTbl[PROGRAM_PART_2_IDX].ucStatus = PROGRAM_STATUS_INVALID;

        //ReportOtherEvent(REPORT_TYPE_UPGRADE, UPGRADE_ABORTED);
        ucUpgradeResult = UPGRADE_ABORTED;
    }
    
    //保存文件信息至FLASH
    SaveUpgradeInfo();

    return ucUpgradeResult;
}

/*************************************************
  Function:
  Description:    本函数在最外层处理完应答后调用，检查当前是否
                  处在升级模式，如果是则判断升级是否结束，并作相关处理
  Calls:
  Called By:
  Input:          g_stDevInfoSet.ucRunningMode: 运行模式
                  g_stUpgradeCtx:               升级过程的上下文
  Output:         
  Return:         0表示无需重启，其它表示需要重启
                  
  Others:         本函数供外部调用
*************************************************/
LONG WaitForUpgradeCompletion()
{
    if(g_stDevInfoSet.ucRunningMode != RUNNING_MODE_UPGRADE)
    {
        return 0;
    }

    if((g_stUpgradeCtx.ulUpgradeProgress == TRANS_CTRL_ABORT)
     ||(g_stUpgradeCtx.ulUpgradeProgress == TRANS_CTRL_CANCEL)
     ||((g_stUpgradeCtx.ulUpgradeProgress == TRANS_CTRL_UPGRADE_COMPLETE) 
     && (g_stUpgradeCtx.lUpgradeResult != UPGRADE_DONE)))
    {
        //为了防止有定时器遗漏
        StopUpgradeTimer();

        //切换到监控状态
        g_stDevInfoSet.ucRunningMode = RUNNING_MODE_MONITOR;
        //MCM-50_20070105_zhonghw_begin
        g_stUpgradeCtx.ulUnSaveBlocks = 0;
        //MCM-50_20070105_zhonghw_end

        //上报升级的结果
        ReportOtherEvent(REPORT_TYPE_UPGRADE, (UCHAR)g_stUpgradeCtx.lUpgradeResult);

        //应该不需要将上下文复位，因为没有保存到NV Memory中
        return 0;
    }
    else if(g_stUpgradeCtx.ulUpgradeProgress == TRANS_CTRL_UPGRADE_COMPLETE) 
    {
        //为了防止有定时器遗漏
        StopUpgradeTimer();

        //切换到监控状态
        g_stDevInfoSet.ucRunningMode = RUNNING_MODE_MONITOR;

        //将数据保存到NV Memory中
        g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ucStatus  = PROGRAM_STATUS_VALID;
        g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileSize= g_stUpgradeCtx.ulSavedFileSize;
        g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ucNew     = FILE_NEW; //更新文件的新旧状态
        if(PROGRAM_PART_1_IDX == g_stUpgradeCtx.ulUpgradePartIdx)
        {
            g_astFilePartTbl[PROGRAM_PART_2_IDX].ucNew = FILE_OLD;
        }
        else
        {
            g_astFilePartTbl[PROGRAM_PART_1_IDX].ucNew = FILE_OLD;
        }
#ifndef M3
        //将文件头信息读出，保存到文件分区表中
        memcpy(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].acFileName, 
               (void *)g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr,
               FPD_FILE_NAME_LEN);
        memcpy(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].aucTimeStamp,
               (void *)(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr + FPD_FILE_NAME_LEN),
               TIME_STAMP_LEN);
        memcpy(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].acVersion,
               (void *)(g_astFilePartTbl[g_stUpgradeCtx.ulUpgradePartIdx].ulFileAddr + FPD_FILE_NAME_LEN + TIME_STAMP_LEN),
               FILE_VERSION_LEN);
#endif        
        g_stUpgradeCtx.ulNextBlockSn = 0; //初始化Next SN
        //MCM-50_20070105_zhonghw_begin
        g_stUpgradeCtx.ulUnSaveBlocks = 0;
        //MCM-50_20070105_zhonghw_end

        SaveUpgradeInfo();
        
        //重新启动
        return 1;
    }

    return 0;
}

/*************************************************
  Function:
  Description:    本函数将传入的告警发送缓冲中的告警项
                  编码成告警监控对象，编码在传入的内存
                  块中进行，由调用方保证内存长度足够
  Calls:
  Called By:
  Input:          pucDst: 告警监控量开始编码的地址
                  pusSrc: 发送缓冲区中的告警项地址
                  ucCnt:  告警项个数
  Output:
  Return:         编码后的长度
  Others:         
*************************************************/
ULONG EncodeAlarmItem(UCHAR *pucAlarmMsg, ALARM_ITEM_IN_BUF_ST *pstAlarmBuf, UCHAR ucCnt)
{
    MCPA_MOBJ_ST* pstMObj = NULL;
    UCHAR i = 0;
    ULONG ulLen = 0;

    for(i = 0; i < ucCnt; i++)
    {
        pstMObj = (MCPA_MOBJ_ST *)(pucAlarmMsg + ulLen);
        pstMObj->ucMObjLen = MOBJ_ALARM_LEN;
        pstMObj->aucMObjId[HI_BYTE]= MOBJ_ID_ALARM_STATUS_SET;
        pstMObj->aucMObjId[LO_BYTE]= (pstAlarmBuf + i)->ucIDLoByte;
        pstMObj->aucMObjContent[0] = (pstAlarmBuf + i)->ucStatus;
        ulLen += MOBJ_ALARM_LEN;
    }
    return ulLen;
}


/*************************************************
  Function:
  Description:    本函数构造发送告警的原语
  Calls:
  Called By:
  Input:          ucCtxIdx:            告警上下文的索引
                  g_astAlarmReportCtx: 其它上报过程上下文
                  g_stAlarmItemBuf:    告警发送缓冲
  Output:         
  Return:         0表示构造失败，否则表示原语指针
                  
  Others:         
*************************************************/
UCHAR* BuildAlarmReport(UCHAR ucCtxIdx)
{
    UCHAR *pucBuf = NULL;
    OHCH_SEND_DATA_REQ_ST *pstSend = NULL;
    ULONG ulLen = 0;
    UCHAR *pucRawApData = NULL;
    
    //申请内存
    pucBuf = (UCHAR *)MyMalloc(g_pstMemPool512);
    if(pucBuf == 0)
    {
        return 0;
    }

    //先构造原语
    pstSend = (OHCH_SEND_DATA_REQ_ST *)pucBuf;
    pstSend->ulPrmvType = OHCH_SEND_DATA_REQ;
    pstSend->ulMagicNum = g_astAlarmReportCtx[ucCtxIdx].usPacketID; //使用packet ID作为magic number
    pstSend->ucResendTime = RESEND_TIME_DATATYPE_ALARM;
    pstSend->ucDstIf    = COMM_IF_NORTH;
    pstSend->ucCommMode = g_stNmParamSet.ucCommMode;
    if(pstSend->ucCommMode == COMM_MODE_SMS)
    {
        pstSend->ucDCS      = 0; //暂时默认为GSM 7 bit编码，如果是短消息方式的话
        memcpy(pstSend->acDstTelNum, g_stNmParamSet.acReportTelNum, sizeof(pstSend->acDstTelNum));
    }
    pstSend->ucDataType = REPORT_TYPE_ALARM; //DATA_TYPE_REPORT;
    
    pstSend->pucData    = pucBuf + DATA_START_POS; //为了提高编码效率，在内存块前留出足够原语编码的空间

    pucRawApData = pstSend->pucData + 1; //留出放起始标志的地方 
    
    ulLen = EncodeReportIncludeReportTypeOnly(pucRawApData, REPORT_TYPE_ALARM,
                                              g_stNmParamSet.ucCommMode,
                                              g_astAlarmReportCtx[ucCtxIdx].usPacketID);

    ulLen += EncodeAlarmItem(pucRawApData + ulLen,
                             &g_stAlarmItemBuf.astAlarmItems[g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemBegin],
                             g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemCount);

    //发送上报的AP层处理
    ulLen = EncodeApData(pucRawApData, ulLen, 
                         ((AP_LAYER_ST *)pucRawApData)->ucApType, MEM_BLK_SIZE_512 - 4 - sizeof(OHCH_SEND_DATA_REQ_ST));
    if(ulLen == 0)
    {
        return NULL;
    }
    pstSend->ulDataLen  = ulLen;

    return pucBuf;
}

//本函数请求连接到监控中心，再CH被分为两个步骤，先建立GPRS连接，再建立TCP连接
void ConnToCenter()
{
    UCHAR *pucBuf = (UCHAR *)MyMalloc(g_pstMemPool16);
    if(pucBuf == 0)
    {
        return; //在程序运行之初不应该进入这个分支，否则无法解决这个问题，只能让程序退出
    }

    OHCH_CONN_CENTER_REQ_ST *pstSend = (OHCH_CONN_CENTER_REQ_ST *)pucBuf;
    pstSend->ulPrmvType = OHCH_CONN_CENTER_REQ;
    memcpy(pstSend->aucIpAddr, g_stNmParamSet.aucEmsIpAddr, sizeof(pstSend->aucIpAddr));
    pstSend->usPortNum = g_stNmParamSet.usEmsPortNum;
    
    if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
    {
        MyFree((void * )pstSend);
        return;
    }
    g_stCenterConnStatus.ucConnAttempCnt++;
}

//本函数请求断开连接
void DisconnFromCenter()
{
    //断开GPRS连接的同时清除登录和心跳定时器    
    if(g_astOtherReportCtx[REPORT_TYPE_LOGIN - OTHER_REPORT_BASE].pTimer)
    {
        OHRemoveTimer(g_astOtherReportCtx[REPORT_TYPE_LOGIN - OTHER_REPORT_BASE].pTimer, 
                      g_astOtherReportCtx[REPORT_TYPE_LOGIN - OTHER_REPORT_BASE].ulTimerMagicNum);
        g_astOtherReportCtx[REPORT_TYPE_LOGIN - OTHER_REPORT_BASE].pTimer = 0;
        g_astOtherReportCtx[REPORT_TYPE_LOGIN - OTHER_REPORT_BASE].ulTimerMagicNum = 0;
    }
    
    if(g_astOtherReportCtx[REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE].pTimer)
    {
        OHRemoveTimer(g_astOtherReportCtx[REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE].pTimer, 
                      g_astOtherReportCtx[REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE].ulTimerMagicNum);
        g_astOtherReportCtx[REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE].pTimer = 0;
        g_astOtherReportCtx[REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE].ulTimerMagicNum = 0;
    }
    
    UCHAR *pucBuf = (UCHAR *)MyMalloc(g_pstMemPool16);
    if(pucBuf == 0)
    {
        return;
    }

    OHCH_GPRS_DISCONN_REQ_ST *pstSend = (OHCH_GPRS_DISCONN_REQ_ST *)pucBuf;
    pstSend->ulPrmvType = OHCH_GPRS_DISCONN_REQ;
    if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
    {
        MyFree((void * )pstSend);
        return;
    }

    g_stCenterConnStatus.ucLinkStatus = COMM_STATUS_DISCONNECTED;
    g_stCenterConnStatus.ucLoginStatus= COMM_STATUS_LOGOUT;
}

//本函数重新建立到中心的连接，首先断开连接，然后重新连接
void ReconnToCenter()
{
    if(g_stCenterConnStatus.ucLinkStatus == COMM_STATUS_CONNECTED)
    {
        DisconnFromCenter();
    }

    g_stCenterConnStatus.ucLinkStatus           = COMM_STATUS_DISCONNECTED;
    g_stCenterConnStatus.ucLoginStatus          = COMM_STATUS_LOGOUT;
    g_stCenterConnStatus.ucConnAttempCnt        = 0;
    g_stCenterConnStatus.ucNoRspCnt             = 0;
    g_stCenterConnStatus.ucLoginTimeoutCnt      = 0;
    
    ConnToCenter();
}

/*************************************************
  Function:
  Description:    本函在修改设备类型后修改监控参量列表，
                  即 将对应的全局监控参量写入FLASH,并重启机器
  Calls:
  Called By:
  Input:          
  Output:         
  Return:         
  Others:         本函数供外部调用
*************************************************/
void UpdateDevConfig()
{
    USHORT* pusMObjTbl;
    ULONG ulTblSize = 0;

    //因为机型配置都变化了，所以要初始化全部参数，因为只是因为机型变化引起的初始化,所以并不会像
    //下载bootloader后一样搽除所有的flash数据，所以要对所有的变量进行初始化，达到出厂初始化的要求。

    //网管参数(此部分变量与移动网管中心相关，工程调试中不轻易对其进行复位)
    g_stNmParamSet.ulStaNum          = 0;     //站点编号
    g_stNmParamSet.ucDevNum          = 0;     //设备编号
    g_stNmParamSet.usHeartbeatPeriod = 60;    //心跳间隔
    g_stNmParamSet.ucCommMode        = COMM_MODE_SMS; //通信方式

    
    InitCmccDefaultParam();
    InitYkppDefaultParam();

    
    //根据设备类型配置参量列表及对应机型的特殊参数
    switch(g_stDevInfoSet.ucDevType)
    {
    case WIDE_BAND:               //宽带直放站
        pusMObjTbl = g_aus3gWideBandTbl;
        ulTblSize = sizeof(g_aus3gWideBandTbl);
        break;
    case WIRELESS_FRESELT:        //选频直放站
        pusMObjTbl = g_aus3gWirLessFreSeltTbl;
        ulTblSize = sizeof(g_aus3gWirLessFreSeltTbl);
        break;
    case OPTICAL_DIRECOUPL_LOCAL: //光纤直放站直接耦合近端机
        pusMObjTbl = g_aus3gOpticalDireCouplLocalTbl;
        ulTblSize = sizeof(g_aus3gOpticalDireCouplLocalTbl);
        break;
    case OPTICAL_WIDEBAND_FAR:    //光纤直放站宽带远端机
        pusMObjTbl= g_aus3gOpticalWideBandFar;
        ulTblSize = sizeof(g_aus3gOpticalWideBandFar);    
        break;
    case TRUNK_AMPLIFIER:           //干线放大器
        pusMObjTbl= g_aus3gDryAmplifier;
        ulTblSize = sizeof(g_aus3gDryAmplifier);     
        break;
    case FRESHIFT_FRESELT_FAR:              //移频直放站选频远端机
        pusMObjTbl = g_aus3gFreShiftFreSeltFarTbl;
        ulTblSize = sizeof(g_aus3gFreShiftFreSeltFarTbl);
        break;
    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL: //移频直放站直接耦合宽带近端机
        pusMObjTbl = g_aus3gFreShiftDireCouplWideBandLocalTbl;
        ulTblSize = sizeof(g_aus3gFreShiftDireCouplWideBandLocalTbl);
        break;   
    case FRESHIFT_WIRELESS_WIDEBAND_LOCAL: //移频直放站无线耦合宽带近端机
        pusMObjTbl = g_aus3gFreShiftWireLessCouplWideBandLocalTbl;
        ulTblSize = sizeof(g_aus3gFreShiftWireLessCouplWideBandLocalTbl);
        break;
    case OPTICAL_FRESELT_FAR:              //光纤直放站选频远端机
        pusMObjTbl = g_aus3gOpticalFreseltFarTbl;
        ulTblSize = sizeof(g_aus3gOpticalFreseltFarTbl);
        break;
    case OPTICAL_WIRELESSCOUPL_LOCAL:      //光纤直放站无线耦合近端机
        pusMObjTbl = g_aus3gOpticalWireLessCouplLocalTbl;
        ulTblSize = sizeof(g_aus3gOpticalWireLessCouplLocalTbl);
        break;
    case FRESHIFT_WIDEBAND_FAR:            //移频直放站宽带远端机
        pusMObjTbl = g_aus3gFreShiftWideBandFarTbl;
        ulTblSize = sizeof(g_aus3gFreShiftWideBandFarTbl);
        break; 
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL: //移频直放站直接耦合选频近端机
        pusMObjTbl = g_aus3gFreShiftDireCouplFreSeltLocalTbl;
        ulTblSize = sizeof(g_aus3gFreShiftDireCouplFreSeltLocalTbl);
        break;
    case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL: //移频直放站无线耦合选频近端机
        pusMObjTbl = g_aus3gFreShiftWireLessCouplFreSeltLocalTbl;
        ulTblSize = sizeof(g_aus3gFreShiftWireLessCouplFreSeltLocalTbl);
        break;
    case BS_AMPLIFIER:                    //基站放大器
        break;        
    case (WIDE_BAND + DEV_WITH_SEC):
        pusMObjTbl= g_ausWideBandWithSecTbl;
        ulTblSize = sizeof(g_ausWideBandWithSecTbl);
        break;
    case POI_STAKEOUT:
        pusMObjTbl= g_ausPoiDevConfig;
        ulTblSize = sizeof(g_ausPoiDevConfig);
        break;
//---------------------2g机型------------------------//
    case WIDE_BAND_2G:               //宽带直放站
        pusMObjTbl = g_aus2gWideBandTbl;
        ulTblSize = sizeof(g_aus2gWideBandTbl);
        break;
    case WIRELESS_FRESELT_2G:        //选频直放站
        pusMObjTbl = g_aus2gWirLessFreSeltTbl;
        ulTblSize = sizeof(g_aus2gWirLessFreSeltTbl);
        break;
    case OPTICAL_DIRECOUPL_LOCAL_2G: //光纤直放站直接耦合近端机
        pusMObjTbl = g_aus2gOpticalDireCouplLocalTbl;
        ulTblSize = sizeof(g_aus2gOpticalDireCouplLocalTbl);
        break;
    case OPTICAL_WIDEBAND_FAR_2G:    //光纤直放站宽带远端机
        pusMObjTbl= g_aus2gOpticalWideBandFar;
        ulTblSize = sizeof(g_aus2gOpticalWideBandFar);    
        break;
    case TRUNK_AMPLIFIER_2G:           //干线放大器
        pusMObjTbl= g_aus2gDryAmplifier;
        ulTblSize = sizeof(g_aus2gDryAmplifier);     
        break;
    case FRESHIFT_FRESELT_FAR_2G:              //移频直放站选频远端机
        pusMObjTbl = g_aus2gFreShiftFreSeltFarTbl;
        ulTblSize = sizeof(g_aus2gFreShiftFreSeltFarTbl);
        break;
    case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL_2G: //移频直放站直接耦合宽带近端机
        pusMObjTbl = g_aus2gFreShiftDireCouplWideBandLocalTbl;
        ulTblSize = sizeof(g_aus2gFreShiftDireCouplWideBandLocalTbl);
        break;   

    //MCM-63_20070315_Zhonghw_begin        
    case BI_FRE_OPTICAL_FAR_2G:              //双频光纤直放站远端机
        pusMObjTbl = g_aus2gBiFreOpticalFarTbl;
        ulTblSize = sizeof(g_aus2gBiFreOpticalFarTbl);
        break;
    case BI_FRE_OPTICAL_DIRECOUPL_LOCAL_2G:              //双频光纤直放站直接耦合近端机
        pusMObjTbl = g_aus2gBiFreOpticalDireCouplLocalTbl;
        ulTblSize = sizeof(g_aus2gBiFreOpticalDireCouplLocalTbl);
        break;
    case BI_DIRECT_WIDE_BAND_TOWER_AMP_2G:              //双向宽带塔放
        pusMObjTbl = g_aus2gBiDirectionalWideBandTowerAmpTbl;
        ulTblSize = sizeof(g_aus2gBiDirectionalWideBandTowerAmpTbl);
        break;
    case BI_DIRECT_FRE_SELECT_TOWER_AMP_2G:              //双向选频塔放
        pusMObjTbl = g_aus2gBiDirectionalFreSelectTowerAmpTbl;
        ulTblSize = sizeof(g_aus2gBiDirectionalFreSelectTowerAmpTbl);
        break;        
    //MCM-63_20070315_Zhonghw_begin            
            
    case FRESHIFT_WIRELESS_WIDEBAND_LOCAL_2G: //移频直放站无线耦合宽带近端机
        pusMObjTbl = g_aus2gFreShiftWireLessCouplWideBandLocalTbl;
        ulTblSize = sizeof(g_aus2gFreShiftWireLessCouplWideBandLocalTbl);
        break;
    case OPTICAL_FRESELT_FAR_2G:              //光纤直放站选频远端机
        pusMObjTbl = g_aus2gOpticalFreseltFarTbl;
        ulTblSize = sizeof(g_aus2gOpticalFreseltFarTbl);
        break;
    //MCM-63_20070315_Zhonghw_end
    case OPTICAL_CARRIER_WIDEBAND_LOCAL_2G:              //光纤载波池宽带基站端机
        pusMObjTbl = g_aus2gOpticalCarryWideBandLocalTbl;
        ulTblSize = sizeof(g_aus2gOpticalCarryWideBandLocalTbl);
        break;
    case OPTICAL_CARRIER_FRESELT_LOCAL_2G:              //光纤载波池选频基站端机
        pusMObjTbl = g_aus2gOpticalCarryFreSelectLocalTbl;
        ulTblSize = sizeof(g_aus2gOpticalCarryFreSelectLocalTbl);
        break;
    case OPTICAL_CARRIER_WIDEBAND_FAR_2G:              //光纤载波池宽带远端机
        pusMObjTbl = g_aus2gOpticalCarryWideBandFarTbl;
        ulTblSize = sizeof(g_aus2gOpticalCarryWideBandFarTbl);
        break;
    case OPTICAL_CARRIER_FRESELT_FAR_2G:              //光纤载波池选频远端机
        pusMObjTbl = g_aus2gOpticalCarryFreSelectFarTbl;
        ulTblSize = sizeof(g_aus2gOpticalCarryFreSelectFarTbl);
        break;        
    //MCM-63_20070315_Zhonghw_end
    
    case OPTICAL_WIRELESSCOUPL_LOCAL_2G:      //光纤直放站无线耦合近端机
        pusMObjTbl = g_aus2gOpticalWireLessCouplLocalTbl;
        ulTblSize = sizeof(g_aus2gOpticalWireLessCouplLocalTbl);
        break;
    case FRESHIFT_WIDEBAND_FAR_2G:            //移频直放站宽带远端机
        pusMObjTbl = g_aus2gFreShiftWideBandFarTbl;
        ulTblSize = sizeof(g_aus2gFreShiftWideBandFarTbl);
        break; 
    case FRESHIFT_DIRECOUPL_FRESELT_LOCAL_2G: //移频直放站直接耦合选频近端机
        pusMObjTbl = g_aus2gFreShiftDireCouplFreSeltLocalTbl;
        ulTblSize = sizeof(g_aus2gFreShiftDireCouplFreSeltLocalTbl);
        break;
    case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL_2G: //移频直放站无线耦合选频近端机
        pusMObjTbl = g_aus2gFreShiftWireLessCouplFreSeltLocalTbl;
        ulTblSize = sizeof(g_aus2gFreShiftWireLessCouplFreSeltLocalTbl);
        break;
    case (WIRELESS_FRESELT_2G + DEV_WITH_SEC):        //选频直放站+太阳能
        pusMObjTbl = g_aus2gWirLessFreSeltWithSecTbl;
        ulTblSize = sizeof(g_aus2gWirLessFreSeltWithSecTbl);
        break;
    case BS_AMPLIFIER_2G:                    //基站放大器
        //MCM-63_20070315_Zhonghw_begin
        pusMObjTbl = g_ausBSAmpTbl;
        ulTblSize = sizeof(g_ausBSAmpTbl);    
        break;
        //MCM-63_20070315_Zhonghw_end

//---------------------2g机型------------------------//
//---------------------测试机型------------------------//
    case MACHINE_FOR_TEST:
        pusMObjTbl = g_ausTestMachine;
        ulTblSize = sizeof(g_ausTestMachine);
        break;
//---------------------测试机型------------------------//
    default:
        pusMObjTbl = g_ausTestMachine;
        ulTblSize = sizeof(g_ausTestMachine);
        return;
    }
    
    //MCM-24_20061108_zhangjie_begin
    if(ulTblSize > sizeof(g_stMObjIdTbl))
    {
        ulTblSize = sizeof(g_stMObjIdTbl);
    }
    //MCM-24_20061108_zhangjie_end
    
    memcpy(&g_stMObjIdTbl, pusMObjTbl, ulTblSize);

    //保存所有信息
    SaveReadOnlyParam();
    SaveAlarmCenterStatus();

    SaveUpgradeInfo();
    SaveWritableParam();
    SaveYkppParam();
    
    MyDelay(1000);
    Restart();
}
/*************************************************
  Function:
  Description:    本函数根据实际的信道总数自动配置信道号和移频号
                  ID，使得信道数和移频总数与信道总数相对应。
  Calls:
  Called By:
  Input:          
  Output:         
  Return:         
  Others:         本函数供外部调用
*************************************************/
void HandleModifyChNumAccordWithChCnt( )
{
    USHORT usChCount = g_stDevInfoSet.ucChCount;
    USHORT usShiftChCount = g_stDevInfoSet.ucChCount;    
    USHORT usParamCount = g_stMObjIdTbl.usParamCount;
    USHORT usCpyCnt = 0;
    UCHAR i = 0;
    USHORT j = 0;
    
    //*****************************配置工作信道号********************************
    if((g_stDevTypeTable.ucChannelType == CH_SHIFT_NOTHING)||(g_stDevTypeTable.ucChannelType == ONLY_SHIFT))
    {
        usChCount = 0;
    }    
    for(i = 0; i < usChCount; i++)    //确定包含应该包含的参量
    {    
        for(j=0;j<usParamCount;j++)
        {
            if(g_stMObjIdTbl.ausParamID[j] == g_ausWorkChNumMObjId[i])
            {
               break;
            }
        }
        if(j >= usParamCount)    //未找到应该包含的参量，故从信号号数组中拷贝该ID
        {
            memcpy((CHAR *)&g_stMObjIdTbl.ausParamID + usParamCount * sizeof(USHORT),
                   (CHAR *)&g_ausWorkChNumMObjId[i],
                   sizeof(USHORT));
        
            g_stMObjIdTbl.usParamCount++;//更新监控参量总数
            usParamCount++;
        }
    }
    
    for(i=usChCount;i<16;i++)      //确定不包含不应该包含的参量，16表示信号号的总数
    {
        for(j=0;j<usParamCount;j++)
        {
            if(g_stMObjIdTbl.ausParamID[j] == g_ausWorkChNumMObjId[i])//找到不应该包含的参量，应该删除此ID            
            {
                usCpyCnt = usParamCount - j - 1;
                memcpy((CHAR *)&g_stMObjIdTbl.ausParamID[j],(CHAR *)&g_stMObjIdTbl.ausParamID[j+1],sizeof(USHORT) * usCpyCnt);
                g_stMObjIdTbl.usParamCount --;//更新监控参量总数
                usParamCount --;
                break;
            }
        }
    }
       
    //*****************************配置移频信道号********************************
    if((g_stDevTypeTable.ucChannelType == CH_SHIFT_NOTHING)||(g_stDevTypeTable.ucChannelType == ONLY_CH))
    {
        usShiftChCount = 0;
    }
    
    for(i = 0; i < usShiftChCount; i++)    //确定包含应该包含的参量
    {
        for(j=0;j<usParamCount;j++)
        {
            if(g_stMObjIdTbl.ausParamID[j] == g_ausShiftChNumMObjId[i])
            {
               break;
            }
        }
        if(j >= usParamCount)    //未找到应该包含的参量，故从信号号数组中拷贝该ID
        {
            memcpy((CHAR *)&g_stMObjIdTbl.ausParamID + usParamCount * sizeof(USHORT),
                   (CHAR *)&g_ausShiftChNumMObjId[i],
                   sizeof(USHORT));
        
            g_stMObjIdTbl.usParamCount++;//更新监控参量总数
            usParamCount++;
        }
    }
    
    for(i=usShiftChCount;i<16;i++)      //确定不包含不应该包含的参量，16表示信号号的总数
    {
        for(j=0;j<usParamCount;j++)
        {
            if(g_stMObjIdTbl.ausParamID[j] == g_ausShiftChNumMObjId[i])//找到不应该包含的参量，应该删除此ID            
            {
                usCpyCnt = usParamCount - j - 1;
                memcpy((CHAR *)&g_stMObjIdTbl.ausParamID[j],(CHAR *)&g_stMObjIdTbl.ausParamID[j+1],sizeof(USHORT) * usCpyCnt);
                g_stMObjIdTbl.usParamCount --;//更新监控参量总数
                usParamCount --;
                break;
            }
        }
    }
}

/*************************************************
  Function:
  Description:    本函数将设置参数设到硬件设备中，使之生效，将在
                  处理完接收的数据后于最外层调用
  Calls:
  Called By:
  Input:          g_stSettingParamSet:    设置参数集
                  g_usSettingParamBitmap: 设置参数位图
  Output:         
  Return:         
  Others:         本函数供外部调用
*************************************************/
void TakeParamEffect()
{
    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_RF_SW))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_RF_SW);
        SetRfSw();
    }
    
    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_ATT))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_ATT);
        SetAtt();
    }

    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_CH_NUM))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_CH_NUM);
        SetFreq();
    }

    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_SMC_ADDR))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_SMC_ADDR);

        //申请内存
        UCHAR *pucBuf = (UCHAR *)MyMalloc(g_pstMemPool256);
        if(pucBuf == 0)
        {
            return; //不改变设置标志，准备下次再执行
        }

        OHCH_RW_SMC_ADDR_REQ_ST *pstSend = (OHCH_RW_SMC_ADDR_REQ_ST *)pucBuf;
        pstSend->ulPrmvType = OHCH_W_SMC_ADDR_REQ;
        memcpy(pstSend->acTelNum, g_stNmParamSet.acSmcAddr, sizeof(g_stNmParamSet.acSmcAddr));

        if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
        {
            MyFree((void *) pstSend);
            return;
        }
    }

    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_MODIFY_DEV_TYPE))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_DEV_TYPE);
        UpdateDevConfig();
    }
    
    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_MODIFY_CH_COUNT))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_CH_COUNT);
        MyDelay(1000);
        Restart(); 
    }    

    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_MODIFY_MOB_LIST))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_MOB_LIST);
        MyDelay(1000);
        Restart();
    }
    //MCM-32_20061114_zhonghw_begin
    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_SWITCH_FILE_VER ))
    {
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_SWITCH_FILE_VER);
        MyDelay(1000);
        Restart();
    }    
    //MCM-32_20061114_zhonghw_end
    UCHAR ucNeedToConnCenter = 0;
    
    //判断通信方式是否被改变，如果改变则需要进行切换
    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_MODIFY_COMM_MODE))
    {
        if((g_stNmParamSet.ucCommMode == COMM_MODE_SMS) //从GPRS切换到SMS
         &&(g_stCenterConnStatus.ucLinkStatus == COMM_STATUS_CONNECTED))
        {
            DisconnFromCenter();
        }

        else if((g_stNmParamSet.ucCommMode == COMM_MODE_CSD) //从GPRS切换到CSD
         &&(g_stCenterConnStatus.ucLinkStatus == COMM_STATUS_CONNECTED))
        {
            DisconnFromCenter();
        }
        
        else if(g_stNmParamSet.ucCommMode == COMM_MODE_GPRS)
        {
            ucNeedToConnCenter = 1;
        }
        
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_COMM_MODE);
    }

    //如果在GPRS方式下修改了IP地址或端口号，则需要重新连接
    if(IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_MODIFY_IP_ADDR)
    || IS_BITMAP_SET(g_usSettingParamBitmap, BITMAP_MODIFY_PORT_NUM))
    {
        if((g_stNmParamSet.ucCommMode == COMM_MODE_GPRS)
          &&(g_stCenterConnStatus.ucLinkStatus == COMM_STATUS_CONNECTED))
        {
            ucNeedToConnCenter = 1;
        }

        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_IP_ADDR);
        CLEAR_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_PORT_NUM);
    }

    if(ucNeedToConnCenter)
    {
        ReconnToCenter();
    }
    
}

/*************************************************
  Function:
  Description:    本函数在接收原语上对其中的协议数据进行
                  编码并发送，如果处理失败，直接在内部将
                  原语释放掉
  Calls:
  Called By:
  Input:          pstPrmv: 接收数据的原语
  Output:         
  Return:         SUCCEEDED表示成功，FAILED表示失败
  Others:         
*************************************************/
LONG EncodeAndResponse(OHCH_RECV_DATA_IND_ST *pstPrmv)
{
    AP_LAYER_ST *pstApData  = (AP_LAYER_ST *)(pstPrmv->pucData + 1); //pstPrmv->pucData指向起始标志的未完成的协议应答数据，还未进行AP层编码
    VPA_LAYER_ST *pstVpData = (VPA_LAYER_ST *)pstApData->aucPdu;

    //VP层数据编码，主要针对设置站点编号和设备编号的情况，需要更新相应字段
    pstVpData->ulStaNum = g_stNmParamSet.ulStaNum;
    pstVpData->ucDevNum = g_stNmParamSet.ucDevNum;

    //AP层数据编码
    ULONG ulRawApDataLen = pstPrmv->ulDataLen - 2 - CRC_LEN; //要扣除起始结束标志和CRC长度

    //pucData指向起始标志
    ULONG ulDataLen = EncodeApData(pstPrmv->pucData + 1, ulRawApDataLen,
                                   ((AP_LAYER_ST *)(pstPrmv->pucData + 1))->ucApType, pstPrmv->ulDataLenMax);

    if(ulDataLen == 0)
    {
        MyFree(pstPrmv);
        return FAILED;
    }

    //向北向接口发送协议数据
    OHCH_SEND_DATA_REQ_ST *pstSend = (OHCH_SEND_DATA_REQ_ST *)pstPrmv;
    pstSend->ulPrmvType   = OHCH_SEND_DATA_REQ;
    pstSend->ulMagicNum   = 0;
    pstSend->ucResendTime = RESEND_TIME_DATATYPE_RSP;
    pstSend->ucDstIf      = COMM_IF_NORTH;
    //pstSend->ucCommMode; //不变
    //pstSend->ucDCS;      //不变
    //pstSend->acDstTelNum;//不变，原来SrcTelNum的位置
    pstSend->ucDataType = DATA_TYPE_RSP;
    pstSend->ulDataLen  = ulDataLen;

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
  Description:    本函数处理上报过程定时器超时的情况
  Calls:
  Called By:
  Input:          pstPrmv:                      定时器超时原语
                  g_stDevInfoSet.ucRunningMode: 运行模式
                  g_astOtherReportCtx:          其它上报过程上下文
                  g_stUpgradeCtx:               升级过程的上下文
  Output:         
  Return:         
                  
  Others:         本函数供外部调用，输入原语在外层释放
*************************************************/
void TimerExpired(TIMEOUT_EVENT_ST *pstPrmv)
{
    OH_TIMER_CTX_UN uCtx;
    uCtx.ulParam = (ULONG)pstPrmv->pvPtr;
    UCHAR *pucBuf = NULL;
    UCHAR ucCtxIdx = 0;
    
    switch(uCtx.stParam.ucTimerID)
    {
    case TIMER_ALARM_REPORT_ID:
        ucCtxIdx = uCtx.stParam.ucParam1;
        switch(g_astAlarmReportCtx[ucCtxIdx].ucResendState)
        {
        case FIRST_NO_TIMEOUT:              //从未超时
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = FIRST_1_SHORT_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;
            break;
        case FIRST_1_SHORT_TIMEOUT:         //第一轮第1次短超时
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = FIRST_2_SHORT_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;
            break;
        case FIRST_2_SHORT_TIMEOUT:         //第一轮第2次短超时
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = FIRST_WAIT_FOR_LONG_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;

            //启动一个长超时定时器
            g_astAlarmReportCtx[ucCtxIdx].pTimer
                = OHCreateTimer((void *)uCtx.ulParam, TIMER_ALARM_LONG_INTV, 
                               &(g_astAlarmReportCtx[ucCtxIdx].ulTimerMagicNum));
            return;
        case FIRST_WAIT_FOR_LONG_TIMEOUT:   //等待第一轮长超时
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = SECOND_NO_TIMEOUT;
            break;
        case SECOND_NO_TIMEOUT:             //第一轮长超时
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = SECOND_1_SHORT_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;
            break;
        case SECOND_1_SHORT_TIMEOUT:        //第二轮第1次短超时
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = SECOND_2_SHORT_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;
            break;
        case SECOND_2_SHORT_TIMEOUT:        //第二轮第2次短超时
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = SECOND_WAIT_FOR_LONG_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;

            //启动一个长超时定时器
            g_astAlarmReportCtx[ucCtxIdx].pTimer
                = OHCreateTimer((void *)uCtx.ulParam, TIMER_ALARM_LONG_INTV, 
                               &(g_astAlarmReportCtx[ucCtxIdx].ulTimerMagicNum));
            return;
        case SECOND_WAIT_FOR_LONG_TIMEOUT:  //等待第二轮长超时
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = THIRD_NO_TIMEOUT;
            break;
        case THIRD_NO_TIMEOUT:              //第二轮长超时
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = THIRD_1_SHORT_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;
            break;
        case THIRD_1_SHORT_TIMEOUT:         //第三轮第1次短超时
            g_astAlarmReportCtx[ucCtxIdx].ucResendState = THIRD_2_SHORT_TIMEOUT;
            g_stCenterConnStatus.ucNoRspCnt++;
            break;
        case THIRD_2_SHORT_TIMEOUT:         //第三轮第2次短超时
            g_stCenterConnStatus.ucNoRspCnt++;
            //如果告警状态不变化，则不会再发送告警，因此这里do nothing
            return;
        default: //不应该进入这个分支
            return;
        }

        if(g_stCenterConnStatus.ucLoginStatus == COMM_STATUS_LOGIN)
        {
            if(g_stCenterConnStatus.ucNoRspCnt > MAX_HEARTBEAT_TO_CNT)
            {
                return; //如果告警超时导致无应答次数超过最大值，则退出在外层重新连接
            }
        }
        
        //告警重发的定时器启动应该在收到OHCH_SEND_DATA_CNF后启动
        //重新启动定时器uCtx.stParam.ucTimerID和uCtx.stParam.ucParam1不变
        //g_astAlarmReportCtx[ucCtxIdx].pTimer
        //        = OHCreateTimer((void *)uCtx.ulParam, TIMER_ALARM_SHORT_INTV, 
        //                       &(g_astAlarmReportCtx[ucCtxIdx].ulTimerMagicNum));
        g_astAlarmReportCtx[ucCtxIdx].ucCommState = OH_SEND_DATA_PENDING;

        //判断是否可以上报
        if(IsReportAllowed(REPORT_TYPE_ALARM) == FALSE)
        {
            return;
        }

        //重新组包发送
        pucBuf = BuildAlarmReport(ucCtxIdx);
        if(pucBuf == 0)
        {
            return; //这是由内存分配不到引起的，因此等待下次告警重组时再尝试
        }        
        
        //将原语发送给CH
        if(OSQPost(g_pstCHNthQue, pucBuf) != OS_NO_ERR)
        {
            MyFree((void * )pucBuf);
            return;
        }        
        break;
    case TIMER_OTHER_REPORT_ID:
        switch(uCtx.stParam.ucParam1) //参数中存放的是上报类型
        {
        case REPORT_TYPE_STA_INIT:
        case REPORT_TYPE_INSPECTION:
        case REPORT_TYPE_REPAIR_CONFIRMED:
        case REPORT_TYPE_CONFIG_CHANGED:
        case REPORT_TYPE_UPGRADE:
            g_astOtherReportCtx[uCtx.stParam.ucParam1 - OTHER_REPORT_BASE].pTimer = 0;
            g_astOtherReportCtx[uCtx.stParam.ucParam1 - OTHER_REPORT_BASE].ulTimerMagicNum = 0;
            g_astOtherReportCtx[uCtx.stParam.ucParam1 - OTHER_REPORT_BASE].ucCommState = OH_IDLE;
            
            //报告上报超时
            NotifyReportResult(uCtx.stParam.ucParam1, FAILED);
            break;
        default:
            break;
        }
        break;
    case TIMER_HEARTBEAT_PERIOD_ID: //发送心跳包
        ReportOtherEvent(REPORT_TYPE_HEARTBEAT, 0);
        break;
    case TIMER_LOGIN_PERIOD_ID: //发送登录
        ReportOtherEvent(REPORT_TYPE_LOGIN, 0);
        break;
    case TIMER_HEARTBEAT_TO_ID:
        //心跳超时必须改变通信口状态
        g_astOtherReportCtx[REPORT_TYPE_HEARTBEAT - OTHER_REPORT_BASE].ucCommState = OH_IDLE;
        
        g_stCenterConnStatus.ucNoRspCnt++;
        if(g_stCenterConnStatus.ucNoRspCnt > MAX_HEARTBEAT_TO_CNT)
        {
            //ReconnToCenter(); //准备断开连接，重新登录，放在外层统一做
        }
        else
        {
            RestartHeartbeatPeriodTimer(); //重启心跳周期定时器
        }
        break;
    case TIMER_LOGIN_TO_ID:
        g_stCenterConnStatus.ucLoginTimeoutCnt++;
        if(g_stCenterConnStatus.ucLoginTimeoutCnt > MAX_LOGIN_TO_CNT)
        {
            ReconnToCenter(); //准备断开连接，重新登录
        }
        else
        {
            ReportOtherEvent(REPORT_TYPE_LOGIN, 0);
        }
        break;
    
    case TIMER_TRANS_CTRL_ID:
    case TIMER_TRANS_BLK_ID:
        g_stUpgradeCtx.ulUpgradeProgress = TRANS_CTRL_ABORT;
        g_stUpgradeCtx.lUpgradeResult    = UPGRADE_COMM_TIMEOUT;
        g_stUpgradeCtx.pTimer            = 0;
        
        //MCM-50_20070105_zhonghw_begin
        //回退数据包
        g_stUpgradeCtx.ulNextBlockSn -= g_stUpgradeCtx.ulUnSaveBlocks;
        g_stUpgradeCtx.ulUnSaveBlocks = 0;                        
        //MCM-50_20070105_zhonghw_end        

        break;

    //异步操作超时
    case TIMER_ASYN_OP_TO_ID:
        ucCtxIdx = uCtx.stParam.ucParam1;

        if(g_astAsynOpCtx[ucCtxIdx].pfTo)
        {
            g_astAsynOpCtx[ucCtxIdx].pfTo(&g_astAsynOpCtx[ucCtxIdx]);
        }
        break;
    default:
        return;
    }
}

/*************************************************
  Function:
  Description:    本函数处理CH返回的发送数据结果确认，如果是
                  应答数据，成功则启动通信定时器，失败则直接
                  启动重发定时器(告警)或者提示失败(其它上报)，
  Calls:
  Called By:
  Input:          pstPrmv: 返回的发送结果确认
  Output:
  Return:         IGNORE表示数据可以释放，SUCCEEDED表示数据不能释放
  Others:         本函数供外部调用，输入原语在外层释放
*************************************************/
LONG HndlOHCHSendDataCnf(OHCH_SEND_DATA_CNF_ST *pstPrmv)
{
    ULONG i = 0;
    OH_TIMER_CTX_UN uCtx;
    ULONG ulIntv = 0;

    //判断是否是上报数据
    if(pstPrmv->ucDataType == DATA_TYPE_RSP)
    {
        if(pstPrmv->cResult != SUCCEEDED)
        {
            if(pstPrmv->ucResendTime == 0) //如果重发计数器为0则丢弃不再发送
            {
                return IGNORE;
            }

            pstPrmv->ucResendTime --;
            
            pstPrmv->ulPrmvType = OHCH_SEND_DATA_REQ;
            if(OSQPost(g_pstCHNthQue, pstPrmv) != OS_NO_ERR)
            {
                MyFree((void * )pstPrmv);
                return IGNORE;
            }
            return SUCCEEDED;
        }
        else
        {
            return IGNORE;
        }
    }
    else //上报
    {
        if(pstPrmv->cResult != SUCCEEDED)
        {
            if(pstPrmv->ucResendTime > 0) //如果重发计数器大于0则继续发送
            {
                pstPrmv->ucResendTime --;
                pstPrmv->ulPrmvType = OHCH_SEND_DATA_REQ;
                if(OSQPost(g_pstCHNthQue, pstPrmv) != OS_NO_ERR)
                {
                    MyFree((void * )pstPrmv);
                    return IGNORE;
                }
                return SUCCEEDED;
            }
        }
    }

    //告警上下文中查找
    for(i = 0; i < MAX_ALARM_REPORTS; i++)
    {
        if(g_astAlarmReportCtx[i].usPacketID == (USHORT)pstPrmv->ulMagicNum)
        {
            if(pstPrmv->cResult == SUCCEEDED)
            {
                //发送成功则进行状态迁移，否则直接启动定时器，等待下次超时重发
                g_astAlarmReportCtx[i].ucCommState = OH_WAIT_FOR_RSP;
            
                //GPRS下成功登录后的所有成功上报需要复位心跳定时器
                if(g_stCenterConnStatus.ucLoginStatus == COMM_STATUS_LOGIN)
                {   
                    //如果OH刚向CH成功发送但是没有得到发送确认
                    //或者CH向OMC成功发送但是没有收到应答
                    //一个心跳包的发送结果处于裁决阶段
                    RestartHeartbeatPeriodTimer();
                }            
            }
            
            //启动定时器
            uCtx.stParam.ucTimerID = TIMER_ALARM_REPORT_ID;    
            uCtx.stParam.ucParam1  = (UCHAR)i; //将上报上下文的下标作为参数，在超时传回时判断

            g_astAlarmReportCtx[i].pTimer = OHCreateTimer((void *)uCtx.ulParam, TIMER_ALARM_SHORT_INTV, 
                                                         &(g_astAlarmReportCtx[i].ulTimerMagicNum));
            return IGNORE;
        }
    }

    //如果不是告警，则在其它上报的上下文中查找
    for(i = 0; i < OTHER_REPORT_COUNT; i++)
    {
        if(g_astOtherReportCtx[i].usPacketID == pstPrmv->ulMagicNum)
        {
            if(pstPrmv->cResult == SUCCEEDED) //发送成功
            {
                //启动定时器
                if(i + OTHER_REPORT_BASE == REPORT_TYPE_LOGIN)
                {
                    uCtx.stParam.ucTimerID = TIMER_LOGIN_TO_ID;
                    ulIntv = TIMER_LOGIN_TO_INTV;
                }
                else if(i + OTHER_REPORT_BASE == REPORT_TYPE_HEARTBEAT)
                {
                    uCtx.stParam.ucTimerID = TIMER_HEARTBEAT_TO_ID;
                    ulIntv = TIMER_HEARTBEAT_TO_INTV;
                }
                else
                {
                    uCtx.stParam.ucTimerID = TIMER_OTHER_REPORT_ID;
                    ulIntv = TIMER_ALARM_SHORT_INTV;

                    //GPRS下成功登录后的所有成功上报需要复位心跳定时器
                    if(g_stCenterConnStatus.ucLoginStatus == COMM_STATUS_LOGIN)
                    {   
                        //如果OH刚向CH成功发送但是没有得到发送确认
                        //或者CH向OMC成功发送但是没有收到应答
                        //即一个心跳包的交互结果处于裁决阶段，不复位心跳定时器
                        RestartHeartbeatPeriodTimer();
                    }            
                }
                uCtx.stParam.ucParam1  = (UCHAR)i + OTHER_REPORT_BASE; //将上报类型作为参数，在超时传回时判断

                g_astOtherReportCtx[i].pTimer = OHCreateTimer((void *)uCtx.ulParam, ulIntv, 
                                                             &(g_astOtherReportCtx[i].ulTimerMagicNum));

                //状态迁移
                g_astOtherReportCtx[i].ucCommState = OH_WAIT_FOR_RSP;
            }
            else //发送失败
            {
                if(g_astOtherReportCtx[i].pTimer) //如果原来有定时器需要先停止
                {
                    OHRemoveTimer(g_astOtherReportCtx[i].pTimer, 
                                  g_astOtherReportCtx[i].ulTimerMagicNum);
                    g_astOtherReportCtx[i].pTimer = 0;
                    g_astOtherReportCtx[i].ulTimerMagicNum = 0;              
                }
                
                g_astOtherReportCtx[i].ucCommState = OH_IDLE;

                if(i + OTHER_REPORT_BASE == REPORT_TYPE_LOGIN) //仍然启动定时器，等待超时后的处理
                {
                    uCtx.stParam.ucTimerID = TIMER_LOGIN_TO_ID;
                    uCtx.stParam.ucParam1  = (UCHAR)i + OTHER_REPORT_BASE;
                    g_astOtherReportCtx[i].pTimer = OHCreateTimer((void *)uCtx.ulParam, TIMER_LOGIN_TO_INTV, 
                                                                 &(g_astOtherReportCtx[i].ulTimerMagicNum));
                }                
                else if(i + OTHER_REPORT_BASE == REPORT_TYPE_HEARTBEAT) //仍然启动定时器，等待超时后的处理
                {
                    uCtx.stParam.ucTimerID = TIMER_HEARTBEAT_TO_ID;
                    uCtx.stParam.ucParam1  = (UCHAR)i + OTHER_REPORT_BASE;
                    g_astOtherReportCtx[i].pTimer = OHCreateTimer((void *)uCtx.ulParam, TIMER_HEARTBEAT_TO_INTV, 
                                                                 &(g_astOtherReportCtx[i].ulTimerMagicNum));
                }
                else
                {
                    //通知上报失败
                    NotifyReportResult((UCHAR)i + OTHER_REPORT_BASE, FAILED);
                }
            }
        }
    }
    return IGNORE;
}

/*************************************************
  Function:
  Description:    本函数提供上报告警的服务，进行上下文
                  操作和协议编码；由底层负责实际的通信方式
  Calls:
  Called By:
  Input:          g_astAlarmReportCtx: 告警上下文
                  g_uSavedAlarmItems:  告警项信息
                  g_stAlarmItemBuf:    发送告警的缓冲区

  Output:
  Return:
  Others:         本函数供外部调用
*************************************************/
void ReportAlarm()
{
    ULONG i = 0;
    UCHAR ucLocal  = 0;
    //UCHAR ucCenter = 0;
    UCHAR ucMaxAlarmItemCnt = 0;
    UCHAR ucCtxIdx = 0;
    UCHAR *pucBuf = NULL;

    //判断是否可以上报
    if(IsReportAllowed(REPORT_TYPE_ALARM) == FALSE)
    {
        return;
    }

    //产生新的上下文，原来的都清除
    for(i = 0; i < MAX_ALARM_REPORTS; i++)
    {
        if(g_astAlarmReportCtx[i].pTimer) //如果原来有定时器需要先停止
        {
            OHRemoveTimer(g_astAlarmReportCtx[i].pTimer, 
                          g_astAlarmReportCtx[i].ulTimerMagicNum);
            g_astAlarmReportCtx[i].pTimer = 0;
            g_astAlarmReportCtx[i].ulTimerMagicNum = 0;

            //请求CH清除原来的发送操作???
        }
    }
    memset(g_astAlarmReportCtx, 0, sizeof(g_astAlarmReportCtx));

    //清空告警发送缓冲区，因为告警重发时判断告警缓存非0为有效数据
    memset(&g_stAlarmItemBuf, 0, sizeof(g_stAlarmItemBuf));

    //生成缓存告警项
    for(i = 0; i < MAX_ALARM_ITEMS; i++)
    {
        if(g_uAlarmItems.astAlarmItemArray[i].ucSupportedBit == 0)
        {
            continue; //不支持该告警项
        }
        
        ucLocal  = g_uAlarmItems.astAlarmItemArray[i].ucLocalStatus; //本地告警状态
        
        if(ucLocal != g_uCenterStatus.aucAlarmStatusArray[i]) //本地和中心状态不一致
        {
            if(g_uAlarmItems.astAlarmItemArray[i].ucEffectiveBit) //如果告警被屏蔽或者告警使能被关闭则不需要发送
            {
                g_stAlarmItemBuf.astAlarmItems[g_stAlarmItemBuf.ulAlarmItemCount].ucUsed = 1;
                g_stAlarmItemBuf.astAlarmItems[g_stAlarmItemBuf.ulAlarmItemCount].ucIDLoByte 
                            = g_uAlarmItems.astAlarmItemArray[i].ucIDLoByte;
                g_stAlarmItemBuf.astAlarmItems[g_stAlarmItemBuf.ulAlarmItemCount].ucStatus = ucLocal;
                g_stAlarmItemBuf.ulAlarmItemCount++;
            }
        }
    }

    if(g_stNmParamSet.ucCommMode == COMM_MODE_SMS)
    {
        ucMaxAlarmItemCnt = MAX_ALARM_ITEM_COUNT_BY_APB; //短消息只能容纳12个告警项，因此目前最多只支持发送48个告警
    }
    else //目前只考虑使用AP:A，不考虑使用AP:C???
    {
        ucMaxAlarmItemCnt = MAX_ALARM_ITEM_COUNT_BY_APA; //256字节数据可以容纳59个告警项
    }

    i = 0;
    while(i < g_stAlarmItemBuf.ulAlarmItemCount)
    {
        //更新上下文
        g_astAlarmReportCtx[ucCtxIdx].usPacketID = GEN_PACKET_ID();
        g_astAlarmReportCtx[ucCtxIdx].ucCommState = OH_SEND_DATA_PENDING;
        g_astAlarmReportCtx[ucCtxIdx].ucResendState = FIRST_NO_TIMEOUT;

        if(g_stAlarmItemBuf.ulAlarmItemCount - i >= ucMaxAlarmItemCnt)
        {
            g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemCount 
                    = (UCHAR)(ucMaxAlarmItemCnt);
        }
        else
        {
            g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemCount 
                    = (UCHAR)(g_stAlarmItemBuf.ulAlarmItemCount - i);
        }

        g_astAlarmReportCtx[ucCtxIdx].ucAlarmItemBegin = (UCHAR)i;

        pucBuf = BuildAlarmReport(ucCtxIdx);
        if(pucBuf == 0)
        {
            return; //这是由内存分配不到引起的，因此等待下次告警重组时再尝试
        }        
        
        //将原语发送给CH
        if(OSQPost(g_pstCHNthQue, pucBuf) != OS_NO_ERR)
        {
           MyFree((void * )pucBuf);
           return;
        }        
        i += ucMaxAlarmItemCnt;
        ucCtxIdx++;
        
        if(ucCtxIdx > MAX_ALARM_REPORTS) //大于则忽略后续的告警项
        {
            return;
        }
    }
}    

/*************************************************
  Function:
  Description:    本函数在告警采集判断任务中调用;
                  用于判断是否需要上报告警，如果需要则上报。首先比较本地和
                  中心告警状态，判断告警状态是否发生变化（如果告警项无效则忽略），
                  如果变化则准备将其加入到上下文中；如果变化项在上下文中存在，则
                  不处理；如果不存在则说明需要重新发送告警；对于本地和中心告警状
                  态一致，但是告警项仍存在于上下文中的情况，需要重新发送告警（原
                  来的做法是不做处理－－这说明本地状态在跳变，等到应答回来后更新
                  中心告警状态后再做处理，但是由于告警使能和屏蔽的测试要求不允许
                  上报被屏蔽或使能被关闭的告警，因此需要重新生成新的告警项）
  Calls:
  Called By:
  Input:          g_uAlarmItems:    告警项信息
                  g_stAlarmItemBuf: 告警发送缓冲
  Output:         
  Return:         
  Others:         本函数供外部调用，不对全局变量进行赋值
*************************************************/
void CheckAlarmStatus()
{
    UCHAR i = 0;
    UCHAR ucLocal  = 0;
    //UCHAR ucCenter = 0;
    UCHAR ucBufIdx = 0;       //告警发送缓冲中告警项索引
    ULONG ulAlarmItemCnt = 0; //记录比较过程中上下文告警项的累加数

    for(i = 0; i < MAX_ALARM_ITEMS; i++)
    {
        if(g_uAlarmItems.astAlarmItemArray[i].ucSupportedBit == 0)
        {
            continue; //不支持该告警项
        }
    
        ucLocal  = g_uAlarmItems.astAlarmItemArray[i].ucLocalStatus; //本地告警状态

        if(ucLocal != g_uCenterStatus.aucAlarmStatusArray[i]) //本地和中心状态不一致
        {
            if(ulAlarmItemCnt >= g_stAlarmItemBuf.ulAlarmItemCount) //说明上下文中没有该告警，需要重新上报
            {
                if(g_uAlarmItems.astAlarmItemArray[i].ucEffectiveBit)
                {
                    break; //没有被屏蔽且使能打开
                }
                else
                {
                    continue;
                }
            }

            while(g_stAlarmItemBuf.astAlarmItems[ucBufIdx].ucUsed == 0) 
            {
                ucBufIdx++; //跳过中间应确认的部分，如果告警分条就会出现这种情况
            }

            if((g_stAlarmItemBuf.astAlarmItems[ucBufIdx].ucIDLoByte != g_uAlarmItems.astAlarmItemArray[i].ucIDLoByte)
            || ((g_stAlarmItemBuf.astAlarmItems[ucBufIdx].ucIDLoByte == g_uAlarmItems.astAlarmItemArray[i].ucIDLoByte)
              &&(g_stAlarmItemBuf.astAlarmItems[ucBufIdx].ucStatus != ucLocal))) //不一致说明告警发生变化，需要重新发送
            {
                if(g_uAlarmItems.astAlarmItemArray[i].ucEffectiveBit)
                {
                    break; //没有被屏蔽且使能打开
                }
            }
            else //说明告警变化已经被发送，但是还未应答，因此不需要重新发送
            {
                ulAlarmItemCnt++;
                ucBufIdx++; //准备比较下一个告警项
            }
        }
        else //本地和中心状态一致
        {
            if(ulAlarmItemCnt >= g_stAlarmItemBuf.ulAlarmItemCount) //上下文中没有该告警项
            {
                continue;
            }

            while(g_stAlarmItemBuf.astAlarmItems[ucBufIdx].ucUsed == 0) 
            {
                ucBufIdx++; //跳过中间应确认的部分，如果告警分条就会出现这种情况
            }
            
            if(g_stAlarmItemBuf.astAlarmItems[ucBufIdx].ucIDLoByte 
            == g_uAlarmItems.astAlarmItemArray[i].ucIDLoByte)
            {
                break; //告警编号一致，则该告警项需要从缓冲中清除，需要重新上报告警
            }
        }
    }

    if(i == MAX_ALARM_ITEMS) //不需要发送告警
    {
        return;
    }

    //发送告警变化通知给OH
    ALARM_STATUS_CHANGED_EVENT_ST *pstEvent = (ALARM_STATUS_CHANGED_EVENT_ST *)MyMalloc(g_pstMemPool16);
    if(pstEvent == 0)
    {
        return; //没有办法
    }

    pstEvent->ulPrmvType = ALARM_STATUS_CHANGED_EVENT;
#ifndef M3
    if(OSQPost(g_pstOHQue, pstEvent) != OS_NO_ERR)
    {
        MyFree((void * )pstEvent);
        return;
    }
#else
    ULONG aulMsgBuf[4];
    aulMsgBuf[3] = (ULONG)pstEvent;
    g_pOHQue->WriteQue((MSG_ST *)aulMsgBuf);
#endif
}

/*************************************************
  Function:
  Description:    本函数用于验证MCP:A参数的有效性，判断
                  是否支持该参数，以及长度是否正确
  Calls:
  Called By:
  Input:          pstMcpA:      MCP:A参数
                  ucDataType:   数据类型，实际是表示长度
                  ucIsSupported:是否被支持
  Output:         
  Return:         
  Others:         
*************************************************/
LONG ValidateMcpAParam(MCPA_MOBJ_ST *pstMcpA, UCHAR ucDataType, UCHAR ucIsSupported)
{
    LONG lRspFlag = MOBJ_CORRECT;
    ULONG ulMObjLen = 0;                      //用于存放mcpData中的L单元的值
    ULONG ulExpectedLen = 0;                  //用于存放预期的LTV总长
    
    //判断是否支持
    if(ucIsSupported != PARAM_SUPPORTED)
    {
        //将ucMObjId[HI_BYTE]设置监控对象无法识别的错误类型，返回rspFlag命令部分被处理
        SetErrCode(pstMcpA->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, lRspFlag);
        return  lRspFlag;   
    }

    //获得L的值和定义结构中的L单元所占的字节数
    ulMObjLen = pstMcpA->ucMObjLen;

    //计算有效长度
    ulExpectedLen = 3 + ucDataType; //3是T和L占的字节数

    //以下为长度鉴权
    if(ulExpectedLen != ulMObjLen)
    {
        SetErrCode(pstMcpA->aucMObjId[HI_BYTE], MOBJ_LEN_NOT_MATCH, lRspFlag);
        return lRspFlag;
    }

    return lRspFlag;
}
/*********************************************************************************
  Function:
  Description:    本函数处理MCP:A中的监控参量列表设置
  Calls:
  Called By:
  Input:          pstMObj:    MCP层数据指针  
                  ucApType:   AP数据类型
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpAMObjListWrite(MCPA_MOBJ_ST *pstMObj)
{
    //第0第1字节为参量列表总个数(低字节在前)
    //第2字节为参量列表偏移的个数
    //第3字节为当前设置的个数
    //第4字节开始为监控参量列表
    
    LONG lRspFlag = MOBJ_CORRECT;
    USHORT usDonedParamCnt = 0;
    USHORT usParamTotalCnt = 0;
    //记录总个数
    memcpy((UCHAR *)&usParamTotalCnt,&pstMObj->aucMObjContent[0],sizeof(usParamTotalCnt));
    //对有效性进行鉴权
    USHORT ucDataType = pstMObj->aucMObjContent[3] * 2 + 4; //为了调用校验函数，校正ucDataType
    lRspFlag = ValidateMcpAParam(pstMObj,ucDataType,PARAM_SUPPORTED);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }
    memcpy((UCHAR *)&g_stMObjIdTbl.ausParamID + pstMObj->aucMObjContent[2] * 2,
           &pstMObj->aucMObjContent[4],pstMObj->aucMObjContent[3] * 2);

    //计算已经下载的个数
    usDonedParamCnt = pstMObj->aucMObjContent[2] + pstMObj->aucMObjContent[3];
    //比较监控参量列表是否下载完毕
    if(usParamTotalCnt == usDonedParamCnt)//监控列表下载完毕
    {
        g_stMObjIdTbl.usParamCount = usDonedParamCnt;
        SET_BITMAP(g_usSettingParamBitmap,BITMAP_MODIFY_MOB_LIST);        
    }
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    本函数对MCP:A参数进行查询与设置，这些参数支持查询设置，存放在
                  NV Memory中，查询设置过程中只访问数据，没有其它操作
                  使用该函数的参数包括网管参数，告警使能项
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址 
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型   
  Output:         
  Return:         监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpANormalRW(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                        UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //以下为判断请求命令类型并进行处理，同时对于2字节或者4字节整型进行必要的字节序调整，如果
    //不需要关心实际值的大小，则是否调整字节序无所谓，例如IP地址
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //查询,存储器读操作
        if((ucDataType == UINT_2) || (ucDataType == SINT_2))
        {
            //*(USHORT *)pstMObj->aucMObjContent = ADJUST_WORD(*(USHORT *)ulParamAddr);
            SET_WORD(pstMObj->aucMObjContent, GET_WORD(ulParamAddr));
        }
        else if(ucDataType == UINT_4)
        {
            //*(ULONG *)pstMObj->aucMObjContent = ADJUST_DWORD(*(ULONG *)ulParamAddr);
            SET_DWORD(pstMObj->aucMObjContent, GET_DWORD(ulParamAddr));
        }
        else
        {
            memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        }
        break;
    case MCPA_CMD_ID_SET: //设置,存储器写操作
        if(ucDataType == UINT_2)
        {
            //*(USHORT *)ulParamAddr = ADJUST_WORD(*(USHORT *)pstMObj->aucMObjContent);
            SET_WORD(ulParamAddr, GET_WORD(pstMObj->aucMObjContent));
        }
        else if(ucDataType == UINT_4)
        {
            //*(ULONG *)ulParamAddr = ADJUST_DWORD(*(ULONG *)pstMObj->aucMObjContent);
            SET_DWORD(ulParamAddr, GET_DWORD(pstMObj->aucMObjContent));
        }
        else
        {
            memcpy((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        }    
        break;
        //只支持“查询设置”的参量，传入的操作类型是协议定义范围
    default: 
        //不可能进入这个分支，因为调用之前已经对命令进行了判断
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    本函数对MCP:A只读参数"设备的实际信道总数"进行读写操作，该参数的写
                  操作较特殊，需要根据设置的实际信道总数去配置监控参量列表中的信道号
                  ID，并置重新启动的标志，在最外层要重新启动设备使新配置的监控参量
                  列表生效。
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址 
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型   
  Output:         
  Return:         监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpAChCount(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                       UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }
    //以下为判断请求命令类型并进行处理
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //查询,存储器读操作
        pstMObj->aucMObjContent[0] = *(UCHAR *)ulParamAddr;
        break;
    case MCPA_CMD_ID_SET: //设置,存储器写操作
        if(pstMObj->aucMObjContent[0] > 16)//16表示设备支持的最大集
        {
            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_VALUE_OUT_OF_BOUND, lRspFlag);
        }
        else if(*((UCHAR *)ulParamAddr) != (pstMObj->aucMObjContent[0]))
        {
            *((UCHAR *)ulParamAddr) = pstMObj->aucMObjContent[0];
            SET_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_CH_COUNT);
            HandleModifyChNumAccordWithChCnt();
        }
        break;
    default: 
        //不可能进入这个分支，因为调用之前已经对命令进行了判断
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    本函数对MCP:A参数进行查询（不设置），这些参数只支持查询，存放在
                  NV Memory或者SRAM中，查询设置过程中只访问数据，没有其它操作
                  使用该函数操的参数包括设备信息和实时采样数据(不带范围标志的)
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址 
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型  
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpANormalR(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType,
                       UCHAR ucIsSupported, UCHAR)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }
    
    //以下处理较为特殊，为了应对“写”命令中出现“只读”参数，
    //对只读的参数不再判断其操作类，直接对其作读取处理
    memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);

    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    本函数处理带范围标志的实时采样参数，在返回实时采样参数的同时也
                  返回是否超出范围的标志
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址 
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型  
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpARcParamWithThr(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType,
                              UCHAR ucIsSupported, UCHAR)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //读取范围标志，范围标志和实际参数偏移为1
    UCHAR ucErr = *(UCHAR *)(ulParamAddr + ucDataType);

    if(ucErr == MOBJ_CORRECT)
    {
        memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);        
    }
    else
    {
        SetErrCode(pstMObj->aucMObjId[HI_BYTE], ucErr, lRspFlag);
    }

    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    本函数处理MCP:A中的告警项查询，告警项查询后需要更新中心状态
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulIdx:        告警项在数组中的索引
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型 
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpAAlarmStatus(MCPA_MOBJ_ST *pstMObj, ULONG ulIdx, UCHAR ucDataType,
                           UCHAR ucIsSupported, UCHAR)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }
    
    //只支持告警状态查询
    pstMObj->aucMObjContent[0] = g_uAlarmItems.astAlarmItemArray[ulIdx].ucLocalStatus;

    //更新中心状态
    g_uCenterStatus.aucAlarmStatusArray[ulIdx]
            = g_uAlarmItems.astAlarmItemArray[ulIdx].ucLocalStatus;
   
    return lRspFlag;
}
/*********************************************************************************
  Function:
  Description:    本函数处理MCP:A中的设备类型，当设置设备类型时需要设置标志，
                  如果设备类型有设置变化，则在外层整个命令处理完后需要对FLASH
                  中的参量列表进行更新
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型 
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpARWDevType(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                      UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //以下为判断请求命令类型并进行处理   
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY:           //查询，一般的存储器读操作
        pstMObj->aucMObjContent[0] = *(UCHAR *)ulParamAddr;
        break;
    case MCPA_CMD_ID_SET:             //设置，存储器写操作，同时
                                      //要置标志在最外层将监控参量列表写入FLASH
        if(*(UCHAR *)ulParamAddr != pstMObj->aucMObjContent[0])       //如果设备类型并未改变则不动作
        {
            switch( pstMObj->aucMObjContent[0])
            {
            //--------3g机型--------//
            case WIDE_BAND:
            case WIRELESS_FRESELT:
            case OPTICAL_DIRECOUPL_LOCAL:
            case OPTICAL_WIDEBAND_FAR:
            case TRUNK_AMPLIFIER:
            case FRESHIFT_FRESELT_FAR:
            case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL:
            case FRESHIFT_WIRELESS_WIDEBAND_LOCAL:
            case OPTICAL_FRESELT_FAR:
            case OPTICAL_CARRIER_WIDEBAND_LOCAL:
            case OPTICAL_CARRIER_FRESELT_LOCAL: 
            case OPTICAL_CARRIER_WIDEBAND_FAR: 
            case OPTICAL_CARRIER_FRESELT_FAR:                 
            case OPTICAL_WIRELESSCOUPL_LOCAL:
            case FRESHIFT_WIDEBAND_FAR:
            case FRESHIFT_DIRECOUPL_FRESELT_LOCAL:
            case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL:
            case BS_AMPLIFIER:
            case POI_STAKEOUT:
            case (WIDE_BAND + DEV_WITH_SEC):
            //--------3g机型--------//                
            //--------2g机型--------//
            case WIDE_BAND_2G:                              
            case WIRELESS_FRESELT_2G:                       
            case OPTICAL_DIRECOUPL_LOCAL_2G:                
            case OPTICAL_WIDEBAND_FAR_2G:                   
            case TRUNK_AMPLIFIER_2G:                          
            case FRESHIFT_FRESELT_FAR_2G:                   
            case FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL_2G:      
            //MCM-63_20070315_Zhonghw_begin
            case BI_FRE_OPTICAL_FAR_2G:
            case BI_FRE_OPTICAL_DIRECOUPL_LOCAL_2G:
            case BI_DIRECT_WIDE_BAND_TOWER_AMP_2G:
            case BI_DIRECT_FRE_SELECT_TOWER_AMP_2G:
            //MCM-63_20070315_Zhonghw_end                
            case FRESHIFT_WIRELESS_WIDEBAND_LOCAL_2G:       
            case OPTICAL_FRESELT_FAR_2G:                    
            case OPTICAL_CARRIER_WIDEBAND_LOCAL_2G:         
            case OPTICAL_CARRIER_FRESELT_LOCAL_2G:          
            case OPTICAL_CARRIER_WIDEBAND_FAR_2G:           
            case OPTICAL_CARRIER_FRESELT_FAR_2G:            
            case OPTICAL_WIRELESSCOUPL_LOCAL_2G:            
            case FRESHIFT_WIDEBAND_FAR_2G:                  
            case FRESHIFT_DIRECOUPL_FRESELT_LOCAL_2G:       
            case FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL_2G:   
            case BS_AMPLIFIER_2G:
            case MACHINE_FOR_TEST:
            case (WIRELESS_FRESELT_2G + DEV_WITH_SEC):
            //--------2g机型--------//
                *(UCHAR *)ulParamAddr = pstMObj->aucMObjContent[0];
                SET_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_DEV_TYPE);
                break;
            default :          //机型设置错误
                SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_VALUE_OUT_OF_BOUND, lRspFlag);
                return  lRspFlag; 
            }
        }

        break;
    default:
        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, lRspFlag);
        return  lRspFlag; 
    }
    return lRspFlag;
}


/*********************************************************************************
  Function:
  Description:    本函数处理MCP:A中的射频开关，当设置射频开关时需要设置标志，
                  在外层整个命令处理完后进行硬件设置
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型 
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpARfSw(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                      UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //以下为判断请求命令类型并进行处理
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //查询,存储器读操作
        pstMObj->aucMObjContent[0] = *(UCHAR *)ulParamAddr;
        break;
    case MCPA_CMD_ID_SET: //设置,存储器写操作
        *(UCHAR *)ulParamAddr = pstMObj->aucMObjContent[0];
        SET_BITMAP(g_usSettingParamBitmap, BITMAP_RF_SW);
        break;
    default: 
        //不可能进入这个分支，因为调用之前已经对命令进行了判断
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    本函数处理MCP:A中的信道参数，当设置信道时需要设置标志，
                  在外层整个命令处理完后进行硬件设置
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型 
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpAChNum(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                     UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //以下为判断请求命令类型并进行处理
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //查询,存储器读操作
        //*(USHORT *)pstMObj->aucMObjContent = ADJUST_WORD(*(USHORT *)ulParamAddr);
        SET_WORD(pstMObj->aucMObjContent, GET_WORD(ulParamAddr));
        break;
    case MCPA_CMD_ID_SET: //设置,存储器写操作
        //*(USHORT *)ulParamAddr = ADJUST_WORD(*(USHORT *)pstMObj->aucMObjContent);
        SET_WORD(ulParamAddr, GET_WORD(pstMObj->aucMObjContent));
        SET_BITMAP(g_usSettingParamBitmap, BITMAP_CH_NUM);
        break;
    default: 
        //不可能进入这个分支，因为调用之前已经对命令进行了判断
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    本函数处理MCP:A中的衰耗，当设置衰耗时需要设置标志，
                  在外层整个命令处理完后进行硬件设置
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型 
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpAAtt(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                   UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //以下为判断请求命令类型并进行处理
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //查询,存储器读操作
        pstMObj->aucMObjContent[0] = *(UCHAR *)ulParamAddr;
        break;
    case MCPA_CMD_ID_SET: //设置,存储器写操作
        *(UCHAR *)ulParamAddr = pstMObj->aucMObjContent[0];
        SET_BITMAP(g_usSettingParamBitmap, BITMAP_ATT);
        break;
    default: 
        //不可能进入这个分支，因为调用之前已经对命令进行了判断
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    本函数处理MCP:A中的短消息中心号码，当设置短小心中心号码时
                  需要设置标志，在外层整个命令处理完后请求CH层设置到MODEM中
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型 
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpASmcAddr(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                       UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //以下为判断请求命令类型并进行处理
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //查询,存储器读操作
        memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    case MCPA_CMD_ID_SET: //设置,存储器写操作
        memcpy((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        SET_BITMAP(g_usSettingParamBitmap, BITMAP_SMC_ADDR);
        break;
    default: 
        //不可能进入这个分支，因为调用之前已经对命令进行了判断
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    本函数处理MCP:A中的IP地址，当设置IP地址且改变时
                  需要设置标志，在外层整个命令处理完后如果是处在GPRS方式则会
                  重新登录
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型 
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpAIpAddr(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                      UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //以下为判断请求命令类型并进行处理
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //查询,存储器读操作
        memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    case MCPA_CMD_ID_SET: //设置,存储器写操作
        if(memcmp((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN))
        {
            SET_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_IP_ADDR); //如果发生变化则置标志
        }
        memcpy((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    default: 
        //不可能进入这个分支，因为调用之前已经对命令进行了判断
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    本函数处理MCP:A中的端口号，当设置端口号且改变时
                  需要设置标志，在外层整个命令处理完后如果是处在GPRS方式则会
                  重新登录
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型 
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpAPortNum(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                       UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //以下为判断请求命令类型并进行处理
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //查询,存储器读操作
        memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    case MCPA_CMD_ID_SET: //设置,存储器写操作
        if(memcmp((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN))
        {
            SET_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_PORT_NUM); //如果发生变化则置标志
        }
        memcpy((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    default: 
        //不可能进入这个分支，因为调用之前已经对命令进行了判断
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    本函数处理MCP:A中的上报通信方式，当设置上报通信方式且改变时
                  需要设置标志，在外层整个命令处理完后会对通信方式切换进行处理
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型 
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpACommMode(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                        UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //以下为判断请求命令类型并进行处理
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //查询,存储器读操作
        memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    case MCPA_CMD_ID_SET: //设置,存储器写操作
        if(memcmp((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN))
        {
            //如果是对上报通信的设置则需要映射到通信方式上来
            if(pstMObj->aucMObjId[LO_BYTE] == MOBJ_ID_NM_REPORT_COMM_MODE)
            {
                switch(pstMObj->aucMObjContent[0])
                {
                case COMM_REPORT_MODE_GPRS:
                    g_stNmParamSet.ucCommMode = COMM_MODE_GPRS;
                    break;
                case COMM_REPORT_MODE_SMS:
                    g_stNmParamSet.ucCommMode = COMM_MODE_SMS;
                    break;
                case COMM_REPORT_MODE_CSD:
                    g_stNmParamSet.ucCommMode = COMM_MODE_CSD;
                    break;
                default:
                    break;
                }
            }
            
            SET_BITMAP(g_usSettingParamBitmap, BITMAP_MODIFY_COMM_MODE); //如果发生变化则置标志
        }
        memcpy((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    default: 
        //不可能进入这个分支，因为调用之前已经对命令进行了判断
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    本函数处理MCP:A中的日期和时间，查询直接从RTC中读取，设置直接
                  设置到RTC中
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型 
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpADateTime(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType, 
                        UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    //以下为判断请求命令类型并进行处理
    switch(ucOperation)
    {
    case MCPA_CMD_ID_QUERY: //查询RTC
        RtcGetDateTime((UCHAR *)ulParamAddr);
        memcpy(pstMObj->aucMObjContent, (void *)ulParamAddr, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        break;
    case MCPA_CMD_ID_SET: //设置RTC
        memcpy((void *)ulParamAddr, pstMObj->aucMObjContent, pstMObj->ucMObjLen - MCPA_MOBJ_MIN_LEN);
        if(RtcSetDateTime((UCHAR *)ulParamAddr) == FAILED)
        {
            lRspFlag = MOBJ_OTHER_ERR;
        }
        break;
    default: 
        //不可能进入这个分支，因为调用之前已经对命令进行了判断
        break;
    }    
    return lRspFlag;
}

/*********************************************************************************
  Function:
  Description:    本函数处理将要发送到TD同步模块的参数，将其复制
                  到全局缓冲区中
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址，此处无效
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型 
  Output:         g_stTDParam:  用于保存正在被处理的TD同步模块参数
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleAsynOpTDParam(MCPA_MOBJ_ST *pstMObj, ULONG, UCHAR ucDataType, 
                         UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    g_stTDParam.ucOpType = ucOperation;

    //复制TD同步模块的监控参量
    memcpy(g_stTDParam.aucParam + g_stTDParam.usParamLen, pstMObj, pstMObj->ucMObjLen);
    g_stTDParam.usParamLen += pstMObj->ucMObjLen;
    
    //将要等待应答的参数暂时置为错误，在获得器件的应答后再清楚错误标志
    SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_OTHER_ERR, lRspFlag); //此处的lRspFlag不起作用

    return MOBJ_CORRECT;
}

/*********************************************************************************
  Function:
  Description:    本函数处理将要发送到太阳能控制器的参数，将其复制
                  到全局缓冲区中
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址，此处无效
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型 
  Output:         g_stTDParam:  用于保存正在被处理的TD同步模块参数
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleAsynOpSECParam(MCPA_MOBJ_ST *pstMObj, ULONG, UCHAR ucDataType, 
                          UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag = ValidateMcpAParam(pstMObj, ucDataType, ucIsSupported);
    if(lRspFlag != MOBJ_CORRECT)
    {
        return lRspFlag;
    }

    g_stSECParam.ucOpType = ucOperation;

    //复制太阳能控制器的监控参量
    memcpy(g_stSECParam.aucParam + g_stSECParam.usParamLen, pstMObj, pstMObj->ucMObjLen);
    g_stSECParam.usParamLen += pstMObj->ucMObjLen;
    
    //将要等待应答的参数暂时置为错误，在获得器件的应答后再清楚错误标志
    SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_OTHER_ERR, lRspFlag); //此处的lRspFlag不起作用

    return MOBJ_CORRECT;
}

//zhangjie 20071120 太阳能蓄电池电压
/*********************************************************************************
  Function:
  Description:    本函数对太阳能蓄电池电压查询做处理，根据机型不同确定所调用的函数，
                  当为太阳能控制器机型时使用异步操作，其他则默认该参数均支持且通过本
                  地得到数据
  Calls:
  Called By:
  Input:          pstMObj:      MCP层数据指针  
                  ulParamAddr:  监控对象存储地址 
                  ucDataType:   监控对象数据类型
                  ucIsSupported:监控对象是否被支持
                  ucOperation:  命令的操作类型  
  Output:         
  Return:         rspFlag：监控对象的处理是否产生过错误
  Others:  
*********************************************************************************/
LONG HandleMcpASecVolR(MCPA_MOBJ_ST *pstMObj, ULONG ulParamAddr, UCHAR ucDataType,
                       UCHAR ucIsSupported, UCHAR ucOperation)
{
    LONG lRspFlag;
    if(g_stDevInfoSet.ucDevType == WIRELESS_FRESELT_2G + DEV_WITH_SEC)//太阳能控制器机型
    {
        lRspFlag = HandleAsynOpSECParam(pstMObj, ulParamAddr, ucDataType, 
                          ucIsSupported, ucOperation);
    }
	else//其他机型均默认支持该参数
	{
	    lRspFlag = HandleMcpANormalR(pstMObj, ulParamAddr, ucDataType,
                       PARAM_SUPPORTED, ucOperation);
	}

    return lRspFlag;
}
//zhangjie 20071120 太阳能蓄电池电压

/*********************************************************************************
  Function:
  Description:    本函数将CH读到的短信中心号码保存到全局变量中，读取短信中心号码的
                  操作在每次系统启动后进行
  Calls:
  Called By:
  Input:          pstPrmv: 返回短信中心结果的原语
  Output:         
  Return:         读取的结果，SUCCEEDED表示成功，FAILED表示失败
  Others:  
*********************************************************************************/
LONG RetrieveSmcAddr(OHCH_RW_SMC_ADDR_CNF_ST *pstPrmv)
{
    if(pstPrmv->lResult == SUCCEEDED)
    {
        memcpy(g_stNmParamSet.acSmcAddr, pstPrmv->acTelNum, sizeof(g_stNmParamSet.acSmcAddr));
    }
    return pstPrmv->lResult;
}


/*************************************************
  Function:
  Description:    本函数上报通信链路故障，在3次尝试建立到
                  监控中心的连接失败后上报，不需要判断是否
                  可以收到上报的应答，因此不用分配上下文
  Calls:
  Called By:
  Input:          
  Output:
  Return:         SUCCEED表示成功，否则表示失败
  Others:         本函数供外部调用
*************************************************/
LONG ReportCommLinkFault()
{
    ULONG ulLen = 0;
    UCHAR *pucBuf = NULL;
    UCHAR *pucRawApData = NULL;
    OHCH_SEND_DATA_REQ_ST *pstSend = NULL;

    //该上报使用短信方式，不判断是否允许上报
    
    //申请包序号
    USHORT usPacketID = GEN_PACKET_ID();

    //申请内存
    pucBuf = (UCHAR *)MyMalloc(g_pstMemPool256);
    if(pucBuf == 0)
    {
        return FAILED;
    }

    //先构造原语
    pstSend = (OHCH_SEND_DATA_REQ_ST *)pucBuf;
    pstSend->ulPrmvType = OHCH_SEND_DATA_REQ;
    pstSend->ulMagicNum = usPacketID; //使用packet ID作为magic number
    pstSend->ucResendTime = RESEND_TIME_GPRS_LINKFAULT;//GPRS连接故障上报如果上报失败后不重复发送
    pstSend->ucDstIf    = COMM_IF_NORTH;
    pstSend->ucCommMode = COMM_MODE_SMS; //该上报固定使用短信方式
    if(pstSend->ucCommMode == COMM_MODE_SMS)
    {
        pstSend->ucDCS      = 0; //暂时默认为GSM 7 bit编码，如果是短消息方式的话
        memcpy(pstSend->acDstTelNum, g_stNmParamSet.acReportTelNum, sizeof(pstSend->acDstTelNum));
    }
    pstSend->ucDataType = REPORT_TYPE_COMM_LINK_FAULT; //DATA_TYPE_REPORT;
    
    pstSend->pucData    = pucBuf + DATA_START_POS; //为了提高编码效率，在内存块前留出足够原语编码的空间

    pucRawApData = pstSend->pucData + 1; //留出放起始标志的地方 
    
    ulLen = EncodeReportIncludeReportTypeOnly(pucRawApData, 
                                              REPORT_TYPE_COMM_LINK_FAULT, 
                                              COMM_MODE_SMS,
                                              usPacketID);

    //发送上报的AP层处理
    ulLen = EncodeApData(pucRawApData, ulLen, 
                         ((AP_LAYER_ST *)pucRawApData)->ucApType, MEM_BLK_SIZE_256 - 4 - sizeof(OHCH_SEND_DATA_REQ_ST));

    pstSend->ulDataLen  = ulLen;
    
    //将原语发送给CH
    if(OSQPost(g_pstCHNthQue, pstSend) != OS_NO_ERR)
    {
        MyFree((void * )pstSend);
        return FAILED;
    }
    return SUCCEEDED;
}

/*************************************************
  Function:
  Description:    本函数处理GPRS连接的确认，如果连接失败
                  则需要重连，每失败3次就上报一次通信链
                  路故障。在这里请求GPRS连接不仅意味着建
                  立GPRS连接，还意味着建立TCP连接，因为
                  这些处理都由MODEM完成
  Calls:
  Called By:
  Input:          pstPrmv: GPRS连接确认
  Output:
  Return:         
  Others:         本函数供外部调用
*************************************************/
void HndlOHCHConnCenterCnf(OHCH_CONN_CENTER_CNF_ST *pstPrmv)
{
    if(pstPrmv->lResult == SUCCEEDED)
    {
        g_stCenterConnStatus.ucLinkStatus = COMM_STATUS_CONNECTED;
        g_stCenterConnStatus.ucConnAttempCnt = 0;
        
        //恰巧中心改变通信方式
        if(g_stNmParamSet.ucCommMode != COMM_MODE_GPRS)
        {
            DisconnFromCenter();
        }
        else
        {
            //进行登录
            ReportOtherEvent(REPORT_TYPE_LOGIN, 0);
        }
    }
    else if(pstPrmv->lResult == RESULT_STOP_REQ) //当B端通信方式被设置程GPRS，要登录时会收到这个cause
    {
        //do nothing
    }
    else //需要继续尝试，且每3次失败上报一次通信链路故障
    {
        if(g_stNmParamSet.ucCommMode == COMM_MODE_GPRS)
        {
            g_stCenterConnStatus.ucLinkStatus = COMM_STATUS_DISCONNECTED;
            if(g_stCenterConnStatus.ucConnAttempCnt >= MAX_CONN_ATTEMP_CNT)
            {
                DisconnFromCenter(); //防止GPRS连接是正常的但是TCP连接建立不起来的情况
                ReportCommLinkFault();
                g_stCenterConnStatus.ucConnAttempCnt = 0;
            }
            ConnToCenter(); //CH需要考虑短信未发送成功但收到请求建立GPRS连接的处理
        }
    }
}

/*************************************************
  Function:
  Description:    本函数处理CSD来电指示，并对其中的来电
                  号码进行鉴权。如果没有来电显示，则CH
                  不使用该原语通知OH
  Calls:
  Called By:
  Input:          pstPrmv: CSD来电指示
  Output:
  Return:         
  Others:         本函数供外部调用
*************************************************/
void HndlOHCHCsdConnInd(OHCH_CSD_CONN_IND_ST *pstPrmv)
{
    if(g_stYkppParamSet.stYkppCtrlParam.ucTelNumAuthSw != 0) //判断是否进行电话号码鉴权
    {
        if((ValidateTelNum(pstPrmv->acTelNum, g_stNmParamSet.acQnsTelNum1) != 0)
        && (ValidateTelNum(pstPrmv->acTelNum, g_stNmParamSet.acQnsTelNum2) != 0)
        && (ValidateTelNum(pstPrmv->acTelNum, g_stNmParamSet.acQnsTelNum3) != 0)
        && (ValidateTelNum(pstPrmv->acTelNum, g_stNmParamSet.acQnsTelNum4) != 0)
        && (ValidateTelNum(pstPrmv->acTelNum, g_stNmParamSet.acQnsTelNum5) != 0))
        {
            pstPrmv->lResult = FAILED;
        }
        else
        {
            pstPrmv->lResult = SUCCEEDED;
        }
    }
    else
    {
        pstPrmv->lResult = SUCCEEDED;
    }
    
    pstPrmv->ulPrmvType = OHCH_CSD_CONN_RSP;

    if(OSQPost(g_pstCHNthQue, pstPrmv) != OS_NO_ERR)
    {
        MyFree((void * )pstPrmv);
        return;
    }
}

/*************************************************
  Function:
  Description:    本函数处理由从机发来的数据，并转发到北向接口，
                  主机不改变需要转发的数据，同时将其当作自己的
                  应答数据处理
  Calls:
  Called By:
  Input:          pstPrmv: 从机发来的数据
  Output:
  Return:         
  Others:         本函数供外部调用
*************************************************/
void HndlOHCHSendDataReq(OHCH_SEND_DATA_REQ_ST *pstPrmv)
{
    //如果是上报需要先判断是否可以发送
    if(DATA_TYPE_RSP != pstPrmv->ucDataType)
    {
        pstPrmv->ucResendTime = RESEND_TIME_DATATYPE_ALARM;
        if(IsReportAllowed(pstPrmv->ucDataType) == FALSE)
        {
            MyFree(pstPrmv);
            return;
        }
    }
    else 
    {
        pstPrmv->ucResendTime = RESEND_TIME_DATATYPE_RSP;
    }
    
    //由于OHCH_SLAVE_SENDDATA_IND_ST和OHCH_SEND_DATA_REQ_ST结构相同，因此直接在上面编码
    pstPrmv->ulPrmvType   = OHCH_SEND_DATA_REQ;
    pstPrmv->ulMagicNum   = 0xFFFFFFFF; //此处MagicNum可能会和本地上报产生处突，但是冲突的可能性非常小
    pstPrmv->ucDstIf      = COMM_IF_NORTH;

    //将原语发送给CH
    if(OSQPost(g_pstCHNthQue, pstPrmv) != OS_NO_ERR)
    {
        MyFree((void * )pstPrmv);
        return;
    }    
}

/*************************************************
  Function:
  Description:    本函数处理由器件发送来的应答，如果应答和请求
                  一致，则向北向接口发送应答数据，否则等待剩余
                  的应答
  Calls:
  Called By:
  Input:          pstMsg: 器件发来的应答消息，消息内存的释放在外层进行
  Output:
  Return:         
  Others:         本函数供外部调用
*************************************************/
void HndlMessQueryOrSetRsp(MESSAGE_ST *pstMsg)
{
    UCHAR ucIdx = 0;
    
    //利用消息SN在异步操作上下文中查找
    for(; ucIdx < ASYN_OP_CTX_CNT; ucIdx++)
    {
        if(pstMsg->ucMsgSn == g_astAsynOpCtx[ucIdx].ucSn)
        {
            break;
        }
    }

    if(ucIdx >= ASYN_OP_CTX_CNT)
    {
        return;
    }

    //将器件应答数据更新到保存的应答中
    OHCH_RECV_DATA_IND_ST *pstPrmv = (OHCH_RECV_DATA_IND_ST *)g_astAsynOpCtx[ucIdx].pvData;
    AP_LAYER_ST *pstApData  = (AP_LAYER_ST *)(pstPrmv->pucData + 1); //pstPrmv->pucData指向起始标志的未完成的协议应答数据，还未进行AP层编码
    VPA_LAYER_ST *pstVpData = (VPA_LAYER_ST *)pstApData->aucPdu;
    MCP_LAYER_ST *pstMcp    = (MCP_LAYER_ST *)pstVpData->aucPdu;

    LONG lMObjTotalLen = 0;       //应答中的监控参数
    MCPA_MOBJ_ST *pstMObj = NULL; //应答中的监控参数的长度

    MCPA_MOBJ_ST *pstParamInMsg = (MCPA_MOBJ_ST *)pstMsg->aucData; //消息中的监控参数
    LONG lParamLen = pstMsg->usDataLen;                            //消息中的监控参数的长度
    UCHAR ucParamInMsgMatched = false;                             //消息中的监控参数和被缓存的相匹配

    while(lParamLen >= pstParamInMsg->ucMObjLen) //遍历器件返回消息中所有的参数
    {
        if(pstParamInMsg->ucMObjLen < MCPA_MOBJ_MIN_LEN)
        {
            pstMcp->ucRspFlag = RSP_FLAG_PARTIALLY_DONE;
            break;
        }
        
        lMObjTotalLen = (LONG)(pstPrmv->ulDataLen - 2 - AP_OVERHEAD - VP_OVERHEAD - MCP_MIN_LEN); //2表示起始结束标志长度
        pstMObj = (MCPA_MOBJ_ST *)(pstMcp->aucContent);

        while(lMObjTotalLen >= pstMObj->ucMObjLen) //遍历缓存的应答中的参数
        {
            if(((pstMObj->aucMObjId[HI_BYTE] & 0xF) == (pstParamInMsg->aucMObjId[HI_BYTE] & 0xF))
             &&((pstMObj->aucMObjId[LO_BYTE]) == (pstParamInMsg->aucMObjId[LO_BYTE])))
            {
                memcpy(pstMObj, pstParamInMsg, pstMObj->ucMObjLen);

                if((pstParamInMsg->aucMObjId[HI_BYTE] & 0xF0) != 0) //判断该参数是否存在错误，是则更新应答标志
                {
                     pstMcp->ucRspFlag = RSP_FLAG_PARTIALLY_DONE;
                }
                ucParamInMsgMatched = true;
                break;
            }
            else
            {
                lMObjTotalLen -= pstMObj->ucMObjLen; //匹配下一个参数
                pstMObj = (MCPA_MOBJ_ST *)(((UCHAR *)pstMObj) + pstMObj->ucMObjLen);
            }
        }

        //如果是告警数据需要同步实时告警状态和中心告警状态（本地状态将在AC中被实时
        //告警状态赋值，这样可以判断中状态是否进行翻转），但是这里要区分这一告警
        //是否是查询请求而产生的，如果不是说明器件可能处理有误，此时可以更新本地告
        //警状态，但是不能更新中心告警状态
        if((pstParamInMsg->aucMObjId[HI_BYTE] & 0xF) == MOBJ_ID_ALARM_STATUS_SET)
        {
            for(ULONG j = 0; j < MAX_ALARM_ITEMS; j++)
            {
                if(g_uAlarmItems.astAlarmItemArray[j].ucIDLoByte
                == pstParamInMsg->aucMObjId[LO_BYTE])
                {
                    if(pstParamInMsg->aucMObjContent[0])
                    {
                    #ifndef M3
                        g_uAlarmItems.astAlarmItemArray[j].ucRcStatus = 1;
                    #else
                        g_uAlarmItems.astAlarmItemArray[j].ucLocalStatus = 1;
                    #endif
                        if(ucParamInMsgMatched)
                        {
                            g_uCenterStatus.aucAlarmStatusArray[j] = 1;
                        }
                    }
                    else
                    {
                    #ifndef M3
                        g_uAlarmItems.astAlarmItemArray[j].ucRcStatus = 0;
                    #else
                        g_uAlarmItems.astAlarmItemArray[j].ucLocalStatus = 0;
                    #endif
                        if(ucParamInMsgMatched)
                        {
                            g_uCenterStatus.aucAlarmStatusArray[j] = 0;
                        }    
                    }

                    break;
                }
            }
        }

        lParamLen -= pstParamInMsg->ucMObjLen; //匹配下一个参数
        pstParamInMsg = (MCPA_MOBJ_ST *)(((UCHAR *)pstParamInMsg) + pstParamInMsg->ucMObjLen);
        ucParamInMsgMatched = false;
    }

    //如果更新中心状态，则需要保存到NV Memory中
    SaveAlarmCenterStatus();

    //有可能会出现器件回复的应答的参数少于给器件的请求的参数，暂时没有办法处理，应该由器件保证

    //根据操作对象更新位图
    g_astAsynOpCtx[ucIdx].ucObjBitmap &= ~pstMsg->ucObj;

    if(g_astAsynOpCtx[ucIdx].ucObjBitmap)
    {
        return; //还有器件未返回应答，继续等待
    }

    //停止定时器
    if(g_astAsynOpCtx[ucIdx].pTimer)
    {
        OHRemoveTimer(g_astAsynOpCtx[ucIdx].pTimer, g_astAsynOpCtx[ucIdx].ulTimerMagicNum);
        g_astAsynOpCtx[ucIdx].pTimer = NULL;
    }
    g_astAsynOpCtx[ucIdx].ucState = OH_IDLE;

    EncodeAndResponse(pstPrmv);
    g_astAsynOpCtx[ucIdx].pvData = NULL; //为了防止接收方还未收到数据，内存就被ClearAsynOpCtx()释放掉；
                                         //如果返回失败则已经在内部释放，不需要被ClearAsynOpCtx()释放

    //初始化上下文
    ClearAsynOpCtx(ucIdx);
}

/*************************************************
  Function:
  Description:    本函数处理由器件发送来的告警，并将告警更新
                  到本地告警参数中
  Calls:
  Called By:
  Input:          pstMsg: 器件发来的应答消息，消息内存的释放在外层进行
  Output:
  Return:         
  Others:         本函数供外部调用
*************************************************/
void HndlMessageAlarmRep(MESSAGE_ST *pstMsg)
{
    LONG i, j;

    //现将所有器件告警的实时状态都置为正常
    //此处属于告警采集，不属于告警判断产生的结果，故只更新实时状态
    for(i = MOBJ_ID_A_SEC_ST_BAT_BLOWOUT; i <= MOBJ_ID_A_SEC_ALEAK; i++)
    {
        for(j = 0; j < MAX_ALARM_ITEMS; j++)
        {
            if(g_uAlarmItems.astAlarmItemArray[j].ucIDLoByte == i)
            {
            #ifndef M3
                g_uAlarmItems.astAlarmItemArray[j].ucRcStatus = 0;
            #else
                g_uAlarmItems.astAlarmItemArray[j].ucLocalStatus = 0;
            #endif
            }
        }
    }

    //将器件消息中的告警项更新到实时告警状态中
    MCPA_MOBJ_ST *pstAlarmItem = (MCPA_MOBJ_ST *)pstMsg->aucData;
    SHORT sDataLen = (SHORT)pstMsg->usDataLen;
    while(sDataLen >= MOBJ_ALARM_LEN)
    {
        for(i = 0; i < MAX_ALARM_ITEMS; i++) //起始可以从厂家定义的告警项开始查找，减少查找次数
        {
            if((pstAlarmItem->aucMObjId[HI_BYTE] == MOBJ_ID_ALARM_STATUS_SET)
            && (pstAlarmItem->aucMObjId[LO_BYTE] == g_uAlarmItems.astAlarmItemArray[i].ucIDLoByte))
            {
                if(pstAlarmItem->aucMObjContent[0])
                {
                #ifndef M3
                    g_uAlarmItems.astAlarmItemArray[i].ucRcStatus = 1;
                #else
                    g_uAlarmItems.astAlarmItemArray[i].ucLocalStatus = 1;
                #endif
                }
                break;
            }
        }
        pstAlarmItem = (MCPA_MOBJ_ST *)((UCHAR *)pstAlarmItem + MOBJ_ALARM_LEN);
        sDataLen -= MOBJ_ALARM_LEN;
    }
}

/*************************************************
  Function:
  Description:    本函数处理由移动协议触发的异步请
                  求超时的情况，超时后将保存的已处
                  理的应答回送，同时将应答标志置为
                  部分错误，监控参量的错误代码在之
                  前已经填写
  Calls:
  Called By:
  Input:          pvCtx: 原来保存的上下文指针，实际指向对应的异步操作上下文
  Output:         
  Return:         
  Others:
*************************************************/
void AsynOpToCommon(void *pvCtx)
{
    ASYN_OP_CTX_ST *pstCtx = (ASYN_OP_CTX_ST *)pvCtx;

    //将缓存的应答数据取出发送
    OHCH_RECV_DATA_IND_ST *pstPrmv = (OHCH_RECV_DATA_IND_ST *)pstCtx->pvData;    
    AP_LAYER_ST *pstApData   = (AP_LAYER_ST *)(pstPrmv->pucData + 1); //pstPrmv->pucData指向起始标志的未完成的协议应答数据，还未进行AP层编码
    VPA_LAYER_ST *pstVpData  = (VPA_LAYER_ST *)pstApData->aucPdu;
    MCP_LAYER_ST *pstMcpData = (MCP_LAYER_ST *)pstVpData->aucPdu;

    //更新MCP应答标志
    pstMcpData->ucRspFlag = RSP_FLAG_PARTIALLY_DONE;
    EncodeAndResponse(pstPrmv);
    pstCtx->pvData = NULL; //为了防止接收方还未收到数据，内存就被ClearAsynOpCtx()释放掉；
                           //如果返回失败则已经在内部释放，不需要被ClearAsynOpCtx()释放

    //初始化上下文，此处使用两个结构地址相减获得索引
    ClearAsynOpCtx(pstCtx - (ASYN_OP_CTX_ST *)&g_astAsynOpCtx);
}


/*********************************************************************************
  Function:
  Description:    本函数处理非初始化阶段CH读写短信中心号码的结果，如果操作正确，则将此
                  保存到全局变量中,并用读取的短信中心号码更新缓存单元。并将厂家命令处理
                  结果修改为正确，如果读取失败，则将厂家命令处理结果修改为错误。最后打
                  包发送数据，并根据实际情况决定是否关闭定时器。
  Calls:
  Called By:
  Input:          pstPrmv: 返回短信中心结果的原语
  Output:         
  Return:         
  Others:  
*********************************************************************************/
void HandleRWSmcAddrRsp(OHCH_RW_SMC_ADDR_CNF_ST *pstPrmv)
{
    //找到短信中心号码操作的的相关上下文
    UCHAR ucIdx = 0;
    for(; ucIdx < ASYN_OP_CTX_CNT; ucIdx++)
    {
        if(pstPrmv->ucMsgSn == g_astAsynOpCtx[ucIdx].ucSn)
        {
            break;
        }            
    }
    
    if(ucIdx < ASYN_OP_CTX_CNT)        //将缓存的应答数据取出发送
    {
        OHCH_RECV_DATA_IND_ST *pstBufPrmv = (OHCH_RECV_DATA_IND_ST *)g_astAsynOpCtx[ucIdx].pvData;   
        YKPP_PDU_ST * pstYkPduData = (YKPP_PDU_ST *)(pstBufPrmv->pucData + 1); //跳过起始标志

        if(pstPrmv->lResult == SUCCEEDED)            
        {
            //更新应答标志为正确
            pstYkPduData->ucResult = SUCCEEDED;
            
            //将读取的短信中心号码填入相应内存单元
            //memcpy(g_stNmParamSet.acSmcAddr, pstPrmv->acTelNum, sizeof(g_stNmParamSet.acSmcAddr));
            
            //用回应数据更新缓存
            memcpy(pstYkPduData->aucParamTbl + 2, pstPrmv->acTelNum,
                   sizeof(g_stNmParamSet.acSmcAddr));
        }
        else
        {
            pstYkPduData->ucResult = YKPP_ERR_OPER_FAILED;
        }

        //将缓存按照协议格式打包
        EncodeAndResponseYkppBuf(pstBufPrmv);
        g_astAsynOpCtx[ucIdx].pvData = NULL;
        
        g_astAsynOpCtx[ucIdx].ucObjBitmap &= ~ASYN_OP_OBJ_SMC;

        if(g_astAsynOpCtx[ucIdx].ucObjBitmap)
        {
            return; //还有器件未返回应答，继续等待
        }
        //停止定时器
        if(g_astAsynOpCtx[ucIdx].pTimer)
        {
            OHRemoveTimer(g_astAsynOpCtx[ucIdx].pTimer, g_astAsynOpCtx[ucIdx].ulTimerMagicNum);
            g_astAsynOpCtx[ucIdx].pTimer = NULL;
        }
        g_astAsynOpCtx[ucIdx].ucState = OH_IDLE;

        //初始化上下文
        ClearAsynOpCtx(ucIdx);            
    }
    return;
}

//MCM-32_20061114_zhonghw_begin
/*********************************************************************************
  Function:
  Description:   本函数处理处理软件版本切换，具体更新程序分区中的程序新旧状态
  Calls:
  Called By:
  Input:          
  Output:         
  Return:         
  Others:  
*********************************************************************************/
LONG SwitchFileVer()
{
    UCHAR i = 0;
    
    //确定是否有两套可运行程序
    for(i = 0;i<2;i++)
    {
        if(g_astFilePartTbl[i].ucStatus != PROGRAM_STATUS_RUNNABLE)
        {
            return FAILED;
        } 
    }

    //此判断可以规避ucIsRunning都等于1的情况    
    if(g_astFilePartTbl[PROGRAM_PART_1_IDX].ucIsRunning == 1)
    {
        g_astFilePartTbl[PROGRAM_PART_1_IDX].ucNew = FILE_OLD;
        g_astFilePartTbl[PROGRAM_PART_2_IDX].ucNew = FILE_NEW;            
    }
    else
    {
        g_astFilePartTbl[PROGRAM_PART_2_IDX].ucNew = FILE_OLD;
        g_astFilePartTbl[PROGRAM_PART_1_IDX].ucNew = FILE_NEW;                        
    }
    
    return SUCCEEDED;
}
/*********************************************************************************
  Function:
  Description:   该函数处理数据单元中厂家代码及设备类别鉴权是否正确，
                 如果正确则调用版本切换函数,并置bitmap相应标志位
  Calls:
  Called By:
  Input:          
  Output:         
  Return:        SUCCEEDED:表示命令正确被处理，或者因为长度错误导致的处理失败
                 FAILED   :表示因为其他错误引起的处理失败

  Others:   注释:
                 主调函数判断函数返回为失败时，统一将MCP应答标志置为"其他错误"
                 相当于切换监控软件版本的处理中，最终MCP应答标志有以下几种结果:
                 1.正确处理   2.长度错误   3.其他错误
  
*********************************************************************************/
LONG HandleSwitchSofVer(MCP_LAYER_ST * pstMcp,LONG lMcpDataLen)
{
    MCPA_MOBJ_ST *pstMObj        = NULL;
    UCHAR ucSwitchVerMnftAuth    = 0;        //鉴权通过标志，0表示未通过，1表示通过
    UCHAR ucSwitchVerDevTypeAuth = 0;        //鉴权通过标志，0表示未通过，1表示通过

    //长度判断
    if(lMcpDataLen != MCPA_DATA_SWITCH_VER_LEN)//确保该命令只包含两个ID
    {
        pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR;
        return SUCCEEDED;
    }
    
    lMcpDataLen -= 2;   //扣除命令单元长度 
    pstMObj = (MCPA_MOBJ_ST *)pstMcp->aucContent;
        
    while(lMcpDataLen >= MCPA_MOBJ_SWITCH_VER_MIN_LEN)  
    {
        if(pstMObj->ucMObjLen != MCPA_MOBJ_SWITCH_VER_MIN_LEN)
        {
            pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR; 
            return SUCCEEDED;
        }

        if(pstMObj->ucMObjLen > lMcpDataLen)
        {
            pstMcp->ucRspFlag = RSP_FLAG_LEN_ERR; 
            return SUCCEEDED;
        }
        
        if(pstMObj->aucMObjId[HI_BYTE] == MOBJ_ID_DEV_INFO_SET)
        {
            if(g_astDevInfoHndlrTbl[pstMObj->aucMObjId[LO_BYTE]].ucIsSupported)                
            {    
                switch(pstMObj->aucMObjId[LO_BYTE])
                {
                case MOBJ_ID_DI_MNFT_ID:
                    if(pstMObj->aucMObjContent[0] == g_stDevInfoSet.ucMnftId)
                    {
                        ucSwitchVerMnftAuth = 1;    //厂家代码鉴权通过
                    }
                    else
                    {
                        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_OTHER_ERR, pstMcp->ucRspFlag);
                    }
                    break;
                case MOBJ_ID_DI_DEV_TYPE:
                    if(pstMObj->aucMObjContent[0] == g_stDevInfoSet.ucDevType)
                    {
                        ucSwitchVerDevTypeAuth = 1;//设备类别鉴权通过
                    }
                    else
                    {
                        SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_OTHER_ERR, pstMcp->ucRspFlag);
                    }
                    break;
                default:
                    //调用此函数后，其实 pstMcp->ucRspFlag并不是最终的值
                    //外层调用函数发现本函数为FAILED后统一将其置为"其他错误"                    
                    SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
                    break;
                }
            }
            else
            {
                SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);
            }
        }
        else
        {
            SetErrCode(pstMObj->aucMObjId[HI_BYTE], MOBJ_ID_UNRECOGNIZED, pstMcp->ucRspFlag);        
        }
        
        lMcpDataLen -= pstMObj->ucMObjLen;
        pstMObj = (MCPA_MOBJ_ST *)(((UCHAR *)pstMObj) + pstMObj->ucMObjLen);
    }

    if((ucSwitchVerMnftAuth == 1)&&(ucSwitchVerDevTypeAuth == 1))
    {
        if(SwitchFileVer() != SUCCEEDED)
        {
            return FAILED;
        }
        
        SaveUpgradeInfo();
        SET_BITMAP(g_usSettingParamBitmap, BITMAP_SWITCH_FILE_VER);        
    }
    else
    {
        return FAILED;
    }
    
    return SUCCEEDED;
}
//MCM-32_20061114_zhonghw_end

