/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   AppMain.cpp
    ����:     ����
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  
       ����:  ����
       ����:  �������
    2. ����:  2006/11/06
       ����:  �½�
       ����:  �޸�����MCM-13��
              �����ڴ�ʹ�ã����ٸ���������ʹ�õĶ�ջ�ռ䡣
    3. ����:  2006/11/07
       ����:  ����
       ����:  �޸�����MCM-20��
              ���Ӱ��������ϱ�����
    4. ����:  2007/01/09
       ����:  ����
       ����:  �޸�����MCM-52��ʹ��#error�����ʾ��Ϣ����ֹ
              �궨��ı����벻��������������
    5. ����:  2007/01/26
       ����:  ����
       ����:  �޸�����MCM-56��ʹ�ú꿪��YK_DEBUG���ƴ򿪿��Ź���
              ��Debug�����в��򿪣���Release�����д�
---------------------------------------------------------------------------*/
//****************************����ͷ�ļ�**************************//
#include <string.h>
#include "includes.h" //uC/OS��ͷ�ļ�

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



//**************************ȫ�ֱ�������**************************//
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


//�����ջ
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

//����Ķ��У����ڴ����Ϣָ��
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

//��ʱ��������
TimerManager g_TimerMngr;

//uC/OS��API��ʹ�õı���
UCHAR g_ucErr;

UCHAR g_aucMem16[MEM_BLK_SIZE_16_COUNT][MEM_BLK_SIZE_16]; //���ڳ��Ƚ϶̵�ԭ��
UCHAR g_aucMem256[MEM_BLK_SIZE_256_COUNT][MEM_BLK_SIZE_256]; //�ڴ���СΪ256�ֽڣ�����ͨ�Ŵ����ҵ����֮�䴫�ݶ���Ϣ
UCHAR g_aucMem512[MEM_BLK_SIZE_512_COUNT][MEM_BLK_SIZE_512]; //�ڴ���СΪ512�ֽڣ�����ͨ�Ŵ����ҵ����֮�䴫�ݳ��ȳ���256�ֽڵ�����

OS_MEM *g_pstMemPool16;
OS_MEM *g_pstMemPool256;
OS_MEM *g_pstMemPool512;


//���ڼ�¼�������ջʹ������ı���
#if MY_CHECK_STACK_EN > 0
typedef struct
{
    ULONG ulStackSize;      //���ֽ�Ϊ��λ
    ULONG ulUsedSize;       //���ֽ�Ϊ��λ
    ULONG ulFreeSize;       //���ֽ�Ϊ��λ
}MY_STACK_INFO_ST;
#endif

//���ڼ�¼�ڴ�ʹ������ı���
#if MY_CHECK_MEM_EN > 0
typedef struct
{
    void *pvMemPart;        //���ڴ�ŷ����ĵ�ַ
    ULONG ulBlkSize;        //ÿ���ڴ��ĳ��ȣ����ֽ�Ϊ��λ��ֻ��ʼ��һ��
    ULONG ulNBlks;          //�ܵĿ�����ֻ��ʼ��һ��
    ULONG ulNUsedMax;       //����ʹ����
    ULONG ulNUsedCurrent;   //��ǰ��ʹ����
}MY_MEM_INFO_ST;
#endif

//���ڼ�¼����ʹ������ı���
#if MY_CHECK_QUE_EN > 0
typedef struct
{
    void *pvQue;            //���ڴ�Ŷ��еĵ�ַ
    ULONG ulQueSize;        //���еĳ��ȣ�ֻ��ʼ��һ��
    ULONG ulEntriesMax;     //����ʹ����
    ULONG ulEntriesCurrent; //��ǰ��ʹ����
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



//**************************ȫ�ֱ�������**************************//

//**************************��������**************************//
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
    //�򿪿��Ź�
    //MCM-56_20070126_linyu_begin
    #ifndef YK_DEBUG
    WdOpen();
    #endif
    //MCM-56_20070126_linyu_end

    //�������ڵĳ�ʼ��
    UsartOpen(&g_stUsart0);//��ʼ��USART0    
    UsartOpen(&g_stUsart1);//��ʼ��USART1  
    UsartOpen(&g_stUsart2);//��ʼ��USART2  
        
    KeyScanInit();  //�����жϳ�ʼ��//zj090320
    
    //485 �رշ���ʹ��
    PioWrite(&U485_PIO_CTRL,U485_MASK,U485_ENABLE_OFF);
    //485 �÷���ʹ��Ϊ���
    PioOpen(&U485_PIO_CTRL,U485_MASK,PIO_OUTPUT);
    
    //LED �ر�    
    PioWrite(&LED_PIO_CTRL,LED_MASK,LED_OFF);//zj090320
    //������LED�������ó����
    PioOpen(&LED_PIO_CTRL, LED_MASK,PIO_OUTPUT); 

    //Ӳ����ʱ����ʼ��
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
    

    //��Χ������ʼ��//zj090320
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
    //��ʼ��Ӳ����ʱ������Ӳ����ʱ���ж�
    g_stTc0.pstTcBase->TC_IER = TC_CPCS;            

    //�����ڴ����
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

    //��ʼ��������Դ
    //notice OHҪ�ȳ�ʼ������Ϊ�����ж�flash����
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
    
    //��ʼ����ʱ����������Ϊ�˱���������ڹ��캯��֮����ʾ��ʼ��
    g_TimerMngr.Init();

    //Ϊ�˷�ֹ����֮����������ϵ�������ʼ������ͳһ�ڴ����???

    //��������//zj090320
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
    //д������־,notice����ϵͳ������ſ�д��־//zj090320
    switch(SF_RSR)
    {
    case SF_EXT_RESET:
        MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "EXT RESET"));
        break;
    //���Ź�������Ҫ����modem
    case SF_WD_RESET:
        ResetModem();
        MY_LOG_MSG_STR((LS_TRACE, LL_INFO, "WATCHDOG RESET"));
        break;
    default:
        break;
    }

//ͳ���������cpuʹ������ҪӦ�ó����ʼ��
#if OS_TASK_STAT_EN > 0
    OSStatInit();
#endif

    //ɾ���������񣬿�ʼ�������
    OSTaskDel(OS_PRIO_SELF);
}

extern "C" void Main(void)//int argc, char **argv
{
    ULONG uiTest=0;
    
    TargetBoardInit();
    
    //Lcd_Init();

    //����ϵͳ��ʼ��
    OSInit();
    
    OSTimeSet(0);




    //��ӡLogo������ʾ��Ϣ


    
    //������������
    OSTaskCreate(TaskStart,(void *)&uiTest, &StackMain[STACKSIZE - 1], 0);

    //���в���ϵͳ
    OSStart();    
}

//���ڼ�¼�����ջ��ʹ�����
#if MY_CHECK_STACK_EN > 0
extern "C" void MyCheckStack(ULONG ulTaskId, ULONG ulUsedSize, ULONG ulFreeSize)
{
    if(ulTaskId < MAX_TASK_ID) //���������ͳ�������IDΪ65535��65534
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

//���ڼ�¼���е�ʹ�����
#if MY_CHECK_MEM_EN > 0
void MyCheckMem(OS_MEM *pmem)
{
    OS_MEM_DATA stMemData;
    for(int i = 0; i < OS_MAX_MEM_PART; i++)
    {
        if((void *)pmem == g_stMyCheckInfo.astMemInfo[i].pvMemPart) //���Ҷ�Ӧ���ڴ����
        {
            if(OS_NO_ERR == OSMemQuery(pmem, &stMemData))
            {
                g_stMyCheckInfo.astMemInfo[i].ulNUsedCurrent = stMemData.OSNUsed;
                if(g_stMyCheckInfo.astMemInfo[i].ulNUsedCurrent > g_stMyCheckInfo.astMemInfo[i].ulNUsedMax)
                {
                    g_stMyCheckInfo.astMemInfo[i].ulNUsedMax = g_stMyCheckInfo.astMemInfo[i].ulNUsedCurrent; //��¼���ֵ
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
        if((void *)pevent == g_stMyCheckInfo.astQueInfo[i].pvQue) //���Ҷ�Ӧ�Ķ���
        {
            if(OS_NO_ERR == OSQQuery(pevent, &stQuedata))
            {
                g_stMyCheckInfo.astQueInfo[i].ulEntriesCurrent = stQuedata.OSNMsgs;
                if(g_stMyCheckInfo.astQueInfo[i].ulEntriesCurrent > g_stMyCheckInfo.astQueInfo[i].ulEntriesMax)
                {
                    g_stMyCheckInfo.astQueInfo[i].ulEntriesMax = g_stMyCheckInfo.astQueInfo[i].ulEntriesCurrent; //��¼���ֵ
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
