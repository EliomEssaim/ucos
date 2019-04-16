/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   AppMain.cpp
    作者:     林雨
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  
       作者:  林雨
       描述:  编码完成
    2. 日期:  2006/11/06
       作者:  章杰
       描述:  修改问题MCM-13，
              减少内存使用：减少各个任务所使用的堆栈空间。
    3. 日期:  2006/11/07
       作者:  林玮
       描述:  修改问题MCM-20，
              增加按键触发上报功能
    4. 日期:  2007/01/09
       作者:  林雨
       描述:  修改问题MCM-52，使用#error输出提示消息，防止
              宏定义改变后代码不完整而出现问题
    5. 日期:  2007/01/26
       作者:  林雨
       描述:  修改问题MCM-56，使用宏开关YK_DEBUG控制打开看门狗，
              在Debug工程中不打开，在Release工程中打开
---------------------------------------------------------------------------*/
//****************************包含头文件**************************//
#include <string.h>
#include "includes.h" //uC/OS的头文件

#include "include/Interface.h"
#include "include/MyCfg.h"
#include "Timer/Timer.h"

#include "./CH/CH.h"

#include "../periph/pio/Pio.h"
#include "../periph/usart/Usart.h"
#include "../periph/tc/Tc.h"
#include "../periph/rtc/Rtc.h"
#include "../periph/nandflash/NandFlash.h"
#include "./Log/Log.h"
#include "AppMain.h"



//**************************全局变量定义**************************//
//MCM-13_20061106_zhangjie_begin
#define OH_TASK_STACK_SIZE 512
#define CH_TASK_STACK_SIZE (1024*18)
#define AC_TASK_STACK_SIZE 256
#define TC_TASK_STACK_SIZE 256
#define LOG_TASK_STACK_SIZE 256
#define LAMP_TASK_STACK_SIZE 64
//MCM-13_20061106_zhangjie_end
#define KEY_TASK_STACK_SIZE 64
#define UI_TASK_STACK_SIZE 1024


//任务堆栈
OS_STK g_OHTaskStack[OH_TASK_STACK_SIZE];
OS_STK g_CHTaskStack[CH_TASK_STACK_SIZE];
OS_STK g_ACTaskStack[AC_TASK_STACK_SIZE];
OS_STK g_TCTaskStack[TC_TASK_STACK_SIZE];
OS_STK g_LAMPTaskStack[LAMP_TASK_STACK_SIZE];
OS_STK g_KEYTaskStack[KEY_TASK_STACK_SIZE];
OS_STK g_UITaskStack[UI_TASK_STACK_SIZE];

#if MY_LOG_EN > 0
OS_STK g_LogTaskStack[LOG_TASK_STACK_SIZE];
#endif

//任务的队列，用于存放消息指针
OS_EVENT *g_pstOHQue;
OS_EVENT *g_pstACQue;
OS_EVENT *g_pstCHNthQue;
OS_EVENT *g_pstCHSthQue;
OS_EVENT *g_pstCHDebQue;
OS_EVENT *g_pstUIQue;
OS_EVENT *g_pstUICHQue;

#if MY_LOG_EN > 0
OS_EVENT *g_pstLogQue;
#endif

void *g_apvOHQue[OH_QUE_SIZE];
void *g_apvACQue[AC_QUE_SIZE];
void *g_apvCHNthQue[CH_NTH_QUE_SIZE];
void *g_apvCHSthQue[CH_STH_QUE_SIZE];
void *g_apvCHDebQue[CH_DEB_QUE_SIZE];
void *g_apvUIQue[UI_QUE_SIZE];

#if MY_LOG_EN > 0
void *g_apvLogQue[LOG_QUE_SIZE];
#endif

//定时器管理器
TimerManager g_TimerMngr;

//uC/OS的API中使用的变量
UCHAR g_ucErr;

UCHAR g_aucMem16[MEM_BLK_SIZE_16_COUNT][MEM_BLK_SIZE_16]; //用于长度较短的原语
UCHAR g_aucMem256[MEM_BLK_SIZE_256_COUNT][MEM_BLK_SIZE_256]; //内存块大小为256字节，用于通信处理和业务处理之间传递短消息
UCHAR g_aucMem512[MEM_BLK_SIZE_512_COUNT][MEM_BLK_SIZE_512]; //内存块大小为512字节，用于通信处理和业务处理之间传递长度超过256字节的数据

OS_MEM *g_pstMemPool16;
OS_MEM *g_pstMemPool256;
OS_MEM *g_pstMemPool512;


//用于记录各任务堆栈使用情况的变量
#if MY_CHECK_STACK_EN > 0
typedef struct
{
    ULONG ulStackSize;      //以字节为单位
    ULONG ulUsedSize;       //以字节为单位
    ULONG ulFreeSize;       //以字节为单位
}MY_STACK_INFO_ST;
#endif

//用于记录内存使用情况的变量
#if MY_CHECK_MEM_EN > 0
typedef struct
{
    void *pvMemPart;        //用于存放分区的地址
    ULONG ulBlkSize;        //每个内存块的长度，以字节为单位，只初始化一次
    ULONG ulNBlks;          //总的块数，只初始化一次
    ULONG ulNUsedMax;       //最大的使用数
    ULONG ulNUsedCurrent;   //当前的使用数
}MY_MEM_INFO_ST;
#endif

//用于记录队列使用情况的变量
#if MY_CHECK_QUE_EN > 0
typedef struct
{
    void *pvQue;            //用于存放队列的地址
    ULONG ulQueSize;        //队列的长度，只初始化一次
    ULONG ulEntriesMax;     //最大的使用数
    ULONG ulEntriesCurrent; //当前的使用数
}MY_QUE_INFO_ST;
#endif

#if (MY_CHECK_STACK_EN > 0) || (MY_CHECK_MEM_EN > 0) || (MY_CHECK_QUE_EN > 0)
typedef struct
{
    #if MY_CHECK_STACK_EN > 0
    MY_STACK_INFO_ST    astStackInfo[MAX_TASK_ID];
    #endif
    
    #if MY_CHECK_MEM_EN > 0
    MY_MEM_INFO_ST      astMemInfo[OS_MAX_MEM_PART];
    #endif
    
    #if MY_CHECK_QUE_EN > 0
    MY_QUE_INFO_ST      astQueInfo[OS_MAX_QS];
    #endif

}MY_CHECK_INFO_ST;

MY_CHECK_INFO_ST g_stMyCheckInfo;
#endif



//**************************全局变量申明**************************//

//**************************函数声明**************************//
extern void OHTask(void *pv);
extern void OHTaskInit();
extern void ACTaskInit();
extern void ACTask(void *);
extern void TCTask(void *);
extern void LampTaskInit();
extern void LampTask(void *);
extern void KEYTask(void * pv);
extern void UITask(void * pv);
extern void UITaskInit();
extern "C" void KeyScanInit();
#if MY_LOG_EN > 0
void LogTaskInit();
void LogTask(void *);
#endif

extern "C" void SetAttInit();
extern "C" void SetFreqInit();
extern "C" void AdcInit();
extern "C" void DacInit();
extern "C" void GetAlarmInit();
extern "C" void I2cInit();
extern "C" void SetFreq();
extern "C" void SetAtt();
extern "C" void WdOpen(void);
extern void ResetModem(void);

//*----------------------------------------------------------------------------
//* Function Name       : SystemInitialization
//* Object              : Setup timer interrupt every 10ms 
//* Input Parameters    : none
//* Output Parameters   : none
//*----------------------------------------------------------------------------
void TargetBoardInit (void)
{
    //打开看门狗
    //MCM-56_20070126_linyu_begin
    #ifndef YK_DEBUG
    WdOpen();
    #endif
    //MCM-56_20070126_linyu_end

    //三个串口的初始化
    UsartOpen(&g_stUsart0);//初始化USART0    
    UsartOpen(&g_stUsart1);//初始化USART1  
    UsartOpen(&g_stUsart2);//初始化USART2  
        
    KeyScanInit();  //按键中断初始化//zj090320
    
    //485 关闭发送使能
    PioWrite(&U485_PIO_CTRL,U485_MASK,U485_ENABLE_OFF);
    //485 置发送使能为输出
    PioOpen(&U485_PIO_CTRL,U485_MASK,PIO_OUTPUT);
    
    //LED 关闭    
    PioWrite(&LED_PIO_CTRL,LED_MASK,LED_OFF);//zj090320
    //将所有LED引脚设置成输出
    PioOpen(&LED_PIO_CTRL, LED_MASK,PIO_OUTPUT); 

    //硬件定时器初始化
    UINT32 uiTimerValue[3] = {0,0,TC_TICK_INIT_VALUE}; 
    
    TcOpen(&g_stTc0, TC_WAVE|TC_CPCTRG|TC_CLKS_MCK8, 0, 0);

    TcWrite(&g_stTc0, uiTimerValue);

    TcTrigCmd(&g_stTc0, TC_TRIG_CHANNEL);

    IrqOpen(g_stTc0.ucPeriphId, 7, AIC_SRCTYPE_INT_EDGE_TRIGGERED, OSTickISR);   

    //RTC
    RtcOpen();
    RtcSet24();

    //NandFlash//zj090320
    if(NFInit() == FAILED)
    {
        g_ucFlashIsOpen = 0;
    }
    else
    {
        g_ucFlashIsOpen = 1;
    }
    

    //外围器件初始化//zj090320
    SetAttInit();
    SetFreqInit();
    AdcInit();
    DacInit();
    GetAlarmInit();
    I2cInit();
  //  SetAtt();
  //  SetFreq();
}


//test 
#define STACKSIZE 50
OS_STK StackMain[STACKSIZE];

void TaskStart(void *pv)
{
    UCHAR ucChId = 0;
    //初始化硬件定时器，打开硬件定时器中断
    g_stTc0.pstTcBase->TC_IER = TC_CPCS;            

    //创建内存分区
    g_pstMemPool16  = OSMemCreate((void *)g_aucMem16,  MEM_BLK_SIZE_16_COUNT,  MEM_BLK_SIZE_16,  &g_ucErr);
    g_pstMemPool256 = OSMemCreate((void *)g_aucMem256, MEM_BLK_SIZE_256_COUNT, MEM_BLK_SIZE_256, &g_ucErr);
    g_pstMemPool512 = OSMemCreate((void *)g_aucMem512, MEM_BLK_SIZE_512_COUNT, MEM_BLK_SIZE_512, &g_ucErr);
#if MY_CHECK_MEM_EN > 0
    memset(&g_stMyCheckInfo.astMemInfo, 0, sizeof(g_stMyCheckInfo.astMemInfo));
    g_stMyCheckInfo.astMemInfo[0].pvMemPart = g_pstMemPool16;
    g_stMyCheckInfo.astMemInfo[0].ulBlkSize = 16;
    g_stMyCheckInfo.astMemInfo[0].ulNBlks   = MEM_BLK_SIZE_16_COUNT;
    g_stMyCheckInfo.astMemInfo[1].pvMemPart = g_pstMemPool256;
    g_stMyCheckInfo.astMemInfo[1].ulBlkSize = 256;
    g_stMyCheckInfo.astMemInfo[1].ulNBlks   = MEM_BLK_SIZE_256_COUNT;
    g_stMyCheckInfo.astMemInfo[2].pvMemPart = g_pstMemPool512;
    g_stMyCheckInfo.astMemInfo[2].ulBlkSize = 512;
    g_stMyCheckInfo.astMemInfo[2].ulNBlks   = MEM_BLK_SIZE_512_COUNT;
#endif

    //初始化任务资源
    //notice OH要先初始化，因为其中有读flash配置
    OHTaskInit();
    CHTaskInit();    
    ACTaskInit();
    LampTaskInit();
    UITaskInit();

#if MY_CHECK_QUE_EN > 0
    memset(g_stMyCheckInfo.astQueInfo, 0, sizeof(g_stMyCheckInfo.astQueInfo));
    g_stMyCheckInfo.astQueInfo[0].pvQue     = (void *)g_pstOHQue;
    g_stMyCheckInfo.astQueInfo[0].ulQueSize = OH_QUE_SIZE;
    g_stMyCheckInfo.astQueInfo[1].pvQue     = (void *)g_pstACQue;
    g_stMyCheckInfo.astQueInfo[1].ulQueSize = AC_QUE_SIZE;
    g_stMyCheckInfo.astQueInfo[2].pvQue     = (void *)g_pstCHNthQue;
    g_stMyCheckInfo.astQueInfo[2].ulQueSize = CH_NTH_QUE_SIZE;
    g_stMyCheckInfo.astQueInfo[3].pvQue     = (void *)g_pstCHSthQue;
    g_stMyCheckInfo.astQueInfo[3].ulQueSize = CH_STH_QUE_SIZE;
    g_stMyCheckInfo.astQueInfo[4].pvQue     = (void *)g_pstCHDebQue;
    g_stMyCheckInfo.astQueInfo[4].ulQueSize = CH_DEB_QUE_SIZE;
    #if MY_LOG_EN > 0    
    g_stMyCheckInfo.astQueInfo[5].pvQue     = (void *)g_pstLogQue;
    #endif
    g_stMyCheckInfo.astQueInfo[5].ulQueSize = LOG_QUE_SIZE;
#endif

    #if MY_LOG_EN > 0
    LogTaskInit();//zj090320
    #endif
    
    //初始化定时器管理器，为了保险起见，在构造函数之外显示初始化
    g_TimerMngr.Init();

    //为了防止任务之间有依赖关系，任务初始化工作统一在此完成???

    //创建任务//zj090320
    #if OS_TASK_CREATE_EXT_EN > 0
        #if OS_STK_GROWTH == 1
            OSTaskCreateExt(OHTask, (void *)&ucChId, 
                            &g_OHTaskStack[OH_TASK_STACK_SIZE - 1], OH_PRIO,
                            OH_TASK_ID,
                            &g_OHTaskStack[0],
                            OH_TASK_STACK_SIZE,
                            (void *)0,
                            OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
            OSTaskCreateExt(CHTask, (void *)&ucChId, 
                            &g_CHTaskStack[CH_TASK_STACK_SIZE - 1], CH_PRIO,
                            CH_TASK_ID,
                            &g_CHTaskStack[0],
                            CH_TASK_STACK_SIZE,
                            (void *)0,
                           OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
            OSTaskCreateExt(ACTask, (void *)&ucChId, 
                            &g_ACTaskStack[AC_TASK_STACK_SIZE - 1], AC_PRIO,
                            AC_TASK_ID,
                            &g_ACTaskStack[0],
                            AC_TASK_STACK_SIZE,
                            (void *)0,
                            OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
            OSTaskCreateExt(TCTask, (void *)&ucChId, 
                            &g_TCTaskStack[TC_TASK_STACK_SIZE - 1], TC_PRIO,
                            TC_TASK_ID,
                            &g_TCTaskStack[0],
                            TC_TASK_STACK_SIZE,
                            (void *)0,
                            OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);            
            OSTaskCreateExt(LampTask, (void *)&ucChId, 
                            &g_LAMPTaskStack[LAMP_TASK_STACK_SIZE - 1], LAMP_PRIO,
                            LAMP_TASK_ID,
                            &g_LAMPTaskStack[0],
                            LAMP_TASK_STACK_SIZE,
                            (void *)0,
                            OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
            
            OSTaskCreateExt(KEYTask, (void *)&ucChId, 
                            &g_KEYTaskStack[KEY_TASK_STACK_SIZE - 1], KEY_PRIO,
                            KEY_TASK_ID,
                            &g_KEYTaskStack[0],
                            KEY_TASK_STACK_SIZE,
                            (void *)0,
                            OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
            
            
            OSTaskCreateExt(UITask, (void *)&ucChId, 
                            &g_UITaskStack[UI_TASK_STACK_SIZE - 1], UI_PRIO,
                            UI_TASK_ID,
                            &g_UITaskStack[0],
                            UI_TASK_STACK_SIZE,
                            (void *)0,
                            OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
                            
            #if MY_LOG_EN > 0
            OSTaskCreateExt(LogTask, (void *)&ucChId, 
                            &g_LogTaskStack[LOG_TASK_STACK_SIZE - 1], LOG_PRIO,
                            LOG_TASK_ID,
                            &g_LogTaskStack[0],
                            LOG_TASK_STACK_SIZE,
                            (void *)0,
                            OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);            
            #endif
        #else //#if OS_STK_GROWTH == 1
//MCM-52_20070109_linyu_begin
            #error ********** This section must be filled when OS_STK_GROWTH != 1. **********
        #endif
    #else //#if OS_TASK_CREATE_EXT_EN > 0
        #error ********** This section must be filled when OS_TASK_CREATE_EXT_EN <= 0. **********
//MCM-52_20070109_linyu_end
    #endif

#if MY_CHECK_STACK_EN > 0
    g_stMyCheckInfo.astStackInfo[OH_TASK_ID].ulStackSize    = OH_TASK_STACK_SIZE * sizeof(OS_STK);
    g_stMyCheckInfo.astStackInfo[CH_TASK_ID].ulStackSize    = CH_TASK_STACK_SIZE * sizeof(OS_STK);
    g_stMyCheckInfo.astStackInfo[TC_TASK_ID].ulStackSize    = TC_TASK_STACK_SIZE * sizeof(OS_STK);
    g_stMyCheckInfo.astStackInfo[AC_TASK_ID].ulStackSize    = AC_TASK_STACK_SIZE * sizeof(OS_STK);
    g_stMyCheckInfo.astStackInfo[LOG_TASK_ID].ulStackSize   = LOG_TASK_STACK_SIZE* sizeof(OS_STK);
#endif

    //ARMTargetStart();
    //test log
    /*
    UCHAR aucBufTemp[512];
    UINT32 i,j=g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulStartRecNum;
    for(i=0;i<g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulRecCnt;i++)
    {
        MY_LOG_READ((j,aucBufTemp));
        j++;
        if(j>=g_stYkppParamSet.stYkppCtrlParam.stLogMgmtParam.ulRecCntMax)
        {
            j=0;
        }
        if((aucBufTemp[0] == 0x20) && (aucBufTemp[1] == 0x06) && (aucBufTemp[2] == 0x10) && (aucBufTemp[3] == 0x20))
        {
            aucBufTemp[511] = 0;
        }
    }
    */
    //写重启日志,notice操作系统启动后才可写日志//zj090320
    switch(SF_RSR)
    {
    case SF_EXT_RESET:
        MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "EXT RESET"));
        break;
    //看门狗重启则要重启modem
    case SF_WD_RESET:
        ResetModem();
        MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "WATCHDOG RESET"));
        break;
    default:
        break;
    }

//统计任务计算cpu使用率需要应用程序初始化
#if OS_TASK_STAT_EN > 0
    OSStatInit();
#endif

    //删除启动任务，开始任务调度
    OSTaskDel(OS_PRIO_SELF);
}

extern "C" void Main(void)//int argc, char **argv
{
    ULONG uiTest=0;
    
    TargetBoardInit();
    
    //Lcd_Init();

    //操作系统初始化
    OSInit();
    
    OSTimeSet(0);




    //打印Logo或者提示信息


    
    //创建启动任务
    OSTaskCreate(TaskStart,(void *)&uiTest, &StackMain[STACKSIZE - 1], 0);

    //运行操作系统
    OSStart();    
}

//用于记录任务堆栈的使用情况
#if MY_CHECK_STACK_EN > 0
extern "C" void MyCheckStack(ULONG ulTaskId, ULONG ulUsedSize, ULONG ulFreeSize)
{
    if(ulTaskId < MAX_TASK_ID) //空闲任务和统计任务的ID为65535和65534
    {
        g_stMyCheckInfo.astStackInfo[ulTaskId].ulUsedSize = ulUsedSize;
        g_stMyCheckInfo.astStackInfo[ulTaskId].ulFreeSize = ulFreeSize;
    }
}

USHORT GetStackInfo(UCHAR *pucInfo, USHORT usMaxSize)
{
    UCHAR ucIndex;
    USHORT usLen = 0;
    
    for(ucIndex = 0; ucIndex < MAX_TASK_ID; ucIndex++)
    {
        if(g_stMyCheckInfo.astStackInfo[ucIndex].ulStackSize == 0)
        {
            if(ucIndex * sizeof(MY_STACK_INFO_ST) > usMaxSize)
            {
                usLen = usMaxSize;
            }
            else
            {
                usLen = ucIndex * sizeof(MY_STACK_INFO_ST);
            }

            memcpy(pucInfo, &g_stMyCheckInfo.astStackInfo, usLen);
            break;
        }
    }
}
#endif

//用于记录队列的使用情况
#if MY_CHECK_MEM_EN > 0
void MyCheckMem(OS_MEM *pmem)
{
    OS_MEM_DATA stMemData;
    for(int i = 0; i < OS_MAX_MEM_PART; i++)
    {
        if((void *)pmem == g_stMyCheckInfo.astMemInfo[i].pvMemPart) //查找对应的内存分区
        {
            if(OS_NO_ERR == OSMemQuery(pmem, &stMemData))
            {
                g_stMyCheckInfo.astMemInfo[i].ulNUsedCurrent = stMemData.OSNUsed;
                if(g_stMyCheckInfo.astMemInfo[i].ulNUsedCurrent > g_stMyCheckInfo.astMemInfo[i].ulNUsedMax)
                {
                    g_stMyCheckInfo.astMemInfo[i].ulNUsedMax = g_stMyCheckInfo.astMemInfo[i].ulNUsedCurrent; //记录最大值
                }
            }
            break;
        }
    }
}

USHORT GetMemInfo(UCHAR *pucInfo, USHORT usMaxSize)
{
    UCHAR ucIndex;
    USHORT usLen = 0;
    
    for(ucIndex = 0; ucIndex < OS_MAX_MEM_PART; ucIndex++)
    {
        if(g_stMyCheckInfo.astMemInfo[ucIndex].pvMemPart == 0)
        {
            if(ucIndex * sizeof(MY_MEM_INFO_ST) > usMaxSize)
            {
                usLen = usMaxSize;
            }
            else
            {
                usLen = ucIndex * sizeof(MY_MEM_INFO_ST);
            }

            memcpy(pucInfo, &g_stMyCheckInfo.astMemInfo, usLen);
            break;
        }
    }
}
#endif

#if MY_CHECK_QUE_EN > 0
extern "C" void MyCheckQue(OS_EVENT *pevent)
{
    OS_Q_DATA stQuedata;
    for(int i = 0; i < OS_MAX_QS; i++)
    {
        if((void *)pevent == g_stMyCheckInfo.astQueInfo[i].pvQue) //查找对应的队列
        {
            if(OS_NO_ERR == OSQQuery(pevent, &stQuedata))
            {
                g_stMyCheckInfo.astQueInfo[i].ulEntriesCurrent = stQuedata.OSNMsgs;
                if(g_stMyCheckInfo.astQueInfo[i].ulEntriesCurrent > g_stMyCheckInfo.astQueInfo[i].ulEntriesMax)
                {
                    g_stMyCheckInfo.astQueInfo[i].ulEntriesMax = g_stMyCheckInfo.astQueInfo[i].ulEntriesCurrent; //记录最大值
                }
            }
            break;
        }
    }
}

USHORT GetQueInfo(UCHAR *pucInfo, USHORT usMaxSize)
{
    UCHAR ucIndex;
    USHORT usLen = 0;
    
    for(ucIndex = 0; ucIndex < OS_MAX_QS; ucIndex++)
    {
        if(g_stMyCheckInfo.astQueInfo[ucIndex].pvQue == 0)
        {
            if(ucIndex * sizeof(MY_QUE_INFO_ST) > usMaxSize)
            {
                usLen = usMaxSize;
            }
            else
            {
                usLen = ucIndex * sizeof(MY_QUE_INFO_ST);
            }

            memcpy(pucInfo, &g_stMyCheckInfo.astQueInfo, usLen);            
            break;
        }
    }
}
#endif
