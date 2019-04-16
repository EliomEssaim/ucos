/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   Timer.cpp
    作者:     林雨
    文件说明: 本文件包含实现定时器机制的代码
    其它:
    函数列表:

    历史记录:
    1. 日期:  2006/02/16
       作者:  林雨
       描述:  编码完成
    2. 日期:  2006/11/02
       作者:  林雨
       描述:  修改问题MCM-14，使用宏开关M3来切换OS_ENTER_CRITICAL()
              和OS_EXIT_CRITICAL()的定义，保证在发布代码中开关中断
              是有效的
              修改问题MCM-15，在进入TimerManager::CountAndCheck()后
              就关中断，离开前才开中断。但这就更加要求超时函数一定
              要简短。
---------------------------------------------------------------------------*/

//MCM-14_20061102_linyu_begin
#ifndef M3
#include "Includes.h" //使用到了临界区宏定义
#else
#define OS_ENTER_CRITICAL()  GetMutex()
#define OS_EXIT_CRITICAL()   PutMutex()

void GetMutex();
void PutMutex();
#endif

#include "Timer.h"
//MCM-14_20061102_linyu_end

/*************************************************
  Function:       
  Description:    本函数在使用定时器前将其初始化
  Input:          pfFunc: 回调函数指针
                  pvCtx:  回调函数使用的参数指针
                  ulInter:定时器时长
                  enTimerType: 定时器类型，分为周期性和非周期性
  Output:         无
  Return:         无
  Others:         无
*************************************************/
void Timer::Init(TX_TIMER_FUNC pfFunc, void* pvCtx, ULONG ulInter, TIMER_TYPE_EN enTimerType)
{
    m_pfFunction = pfFunc;
    m_pvContext  = pvCtx;
    m_ulInterval = ulInter;
    m_ulRelativeInterval = ulInter;
    m_enStatus   = IDLE;
    m_enType     = enTimerType;
}

/*************************************************
  Function:       
  Description:    本函数用于将定时器内容清空
  Input:          无
  Output:         无
  Return:         无
  Others:         无
*************************************************/
void Timer::Init()
{
    m_pfFunction = NULL;
    m_pvContext  = NULL;
    m_ulInterval = 0;
    m_ulRelativeInterval = 0;
    m_enStatus   = IDLE;
    m_pPrev = NULL;
    m_pNext = NULL;
}

/*************************************************
  Function:       
  Description:    本函数在定时器超时时被调用，执行定时器的回调函数
  Input:          无
  Output:         无
  Return:         无
  Others:         无
*************************************************/
void Timer::Expire()
{   
    if(m_enStatus == RUNNING) //如果正在运行
    {
        m_enStatus = STOPPED; //先暂停，可能是怕函数重入???同时在回调函数中删除定时器也不会有问题

        m_pfFunction(m_pvContext); //执行超时操作
            
        m_enStatus = RUNNING;
    }
}

/*************************************************
  Function:       
  Description:    本函数初始化定时器管理器，将数组中的定时器串成空闲列表
  Input:          无
  Output:         无
  Return:         无
  Others:         无
*************************************************/
TimerManager::TimerManager()
{
    Init();
}

//由于不知道为何G++编译下载到板上，默认构造函数不会被调用，因此使用一个显示初始化函数
void TimerManager::Init()
{
    m_pFirstRunningTimer = NULL;
    m_pFirstPausedTimer  = NULL;
    
    //将定时器加入空闲列表中，空闲列表是一个单项列表
    for(int i = 0; i < MAX_TIMER_COUNT - 1; i++)
    {
        m_aList[i].m_pNext = &m_aList[i + 1];
        m_aList[i].m_pPrev = NULL;
    }
    
    //最后一个节点
    m_aList[MAX_TIMER_COUNT - 1].m_pPrev = NULL;
    m_aList[MAX_TIMER_COUNT - 1].m_pNext = NULL;

    m_pFirstIdleTimer = m_aList;

    m_ulMagicNum = 0;
}

/*************************************************
  Function:       
  Description:    本函数将定时器插入运行列表中，与列表中的定时器超时时长比较后确定
                  插入的位置，同时计算与前一个定时器的相对时长，如果两个定时器同时
                  超时，则后插入的定时器在前
  Input:          pTimer：定时器
  Output:         无
  Return:         无
  Others:         无
*************************************************/
void TimerManager::InsertRunningTimer(Timer *pTimer)
{
    UINT32 cpu_sr;
    Timer *pTmp = NULL;
    Timer *pPrev= NULL;

    pTimer->m_enStatus = RUNNING;
    
    //列表为空，则直接插入最前端
    if(m_pFirstRunningTimer == NULL)
    {
        pTimer->m_pPrev = NULL;
        pTimer->m_pNext = NULL;

        OS_ENTER_CRITICAL();
        m_pFirstRunningTimer = pTimer;        
        OS_EXIT_CRITICAL();
        return;
    }
    
    //超时时长最小，则直接插入列表最前端
    if(pTimer->m_ulInterval <= m_pFirstRunningTimer->m_ulRelativeInterval)
    {        
        pTimer->m_pNext = m_pFirstRunningTimer;
        pTimer->m_pPrev = NULL;

        OS_ENTER_CRITICAL();
        m_pFirstRunningTimer->m_ulRelativeInterval
            = m_pFirstRunningTimer->m_ulRelativeInterval - pTimer->m_ulInterval;
        m_pFirstRunningTimer->m_pPrev = pTimer;
        m_pFirstRunningTimer = pTimer;
        OS_EXIT_CRITICAL();
        return;
    }
    
    pPrev= m_pFirstRunningTimer;
    pTmp = m_pFirstRunningTimer->m_pNext;
    ULONG ulTotalInter = m_pFirstRunningTimer->m_ulRelativeInterval;
    
    while(pTmp != NULL)
    {
        if(pTimer->m_ulInterval <= ulTotalInter + pTmp->m_ulRelativeInterval) //插入列表
        {
            //更新被插入的定时器参数
            pTimer->m_ulRelativeInterval = pTimer->m_ulInterval - ulTotalInter;
            pTimer->m_pNext = pTmp;
            pTimer->m_pPrev = pPrev;

            OS_ENTER_CRITICAL();
            //调整后继节点的相对时长
            pTmp->m_ulRelativeInterval = pTmp->m_ulRelativeInterval
                                       - pTimer->m_ulRelativeInterval;
            
            //建立与后一个节点的联系
            pTmp->m_pPrev = pTimer;
            
            //建立与前一个节点的联系
            pPrev->m_pNext = pTimer;
            
            OS_EXIT_CRITICAL();
            return;
        }

        ulTotalInter += pTmp->m_ulRelativeInterval;
        pPrev= pTmp;
        pTmp = pTmp->m_pNext;        
    }
    
    //将定时器插入列表的最末端 
    pTimer->m_ulRelativeInterval = pTimer->m_ulInterval - ulTotalInter;
    pTimer->m_pNext = NULL;
    pTimer->m_pPrev = pPrev;

    OS_ENTER_CRITICAL();    
    pPrev->m_pNext  = pTimer;    
    OS_EXIT_CRITICAL();
}

/*************************************************
  Function:       
  Description:    本函数从空闲列表中获取定时器并初始化，再将其插入运行列表
  Input:          pfFunction: 回调函数指针
                  pvContext:  回调函数使用的参数指针
                  ulInterval: 定时器时长，这里的时长的单位是定时器的精度
                  enTimerType:定时器类型，分为周期性和非周期性
  Output:         pulMagicNum:定时器创建时分配的magic number
  Return:         返回创建的结果，NULL表示创建失败，否则返回创建的定时器
  Others:         无
*************************************************/
Timer* TimerManager::CreateTimer(TX_TIMER_FUNC pfFunction, void* pvContext,
                                 ULONG ulInterval, TIMER_TYPE_EN enTimerType,
                                 ULONG *pulMagicNum)
{
    UINT32 cpu_sr;
    Timer *pTmp = NULL;
    
    //从空闲列表中获得定时器
    if(m_pFirstIdleTimer == NULL)
    {
        return NULL; //没有多余的定时器
    }
    
    OS_ENTER_CRITICAL();
    pTmp = m_pFirstIdleTimer;
    m_pFirstIdleTimer = m_pFirstIdleTimer->m_pNext; //调整空闲定时器列表    
    OS_EXIT_CRITICAL();
    
    pTmp->Init(pfFunction, pvContext, ulInterval, enTimerType); //初始化定时器

    pTmp->ulMagicNum = m_ulMagicNum++;
    *pulMagicNum = pTmp->ulMagicNum;
    
    InsertRunningTimer(pTmp); //将定时器插入运行队列
    
    return pTmp;
}

/*************************************************
  Function:       
  Description:    本函数将定时器从运行列表或者暂停列表中删除，放回到空闲列表中
  Input:          pRemoved:  待删除的定时器
                  ulMagicNum:创建定时器时分配的magic number
  Output:         无
  Return:         无
  Others:         无
*************************************************/
void TimerManager::RemoveTimer(Timer *pRemoved, ULONG ulMagicNum)
{
    UINT32 cpu_sr;
    
    //防止传入空指针
    if(pRemoved == 0)
    {
        return;
    }

    if(pRemoved->ulMagicNum != ulMagicNum)
    {
        return;
    }
    
    //定时器是否已经被删除或者超时
    if((pRemoved->m_enStatus == IDLE) || (pRemoved->m_enStatus == STOPPED))
    {
        return;
    }

    //从列表中删除，将前后节点连接起来
    OS_ENTER_CRITICAL();
    if((pRemoved->m_pPrev == NULL) && (pRemoved->m_pNext == NULL)) //说明列表中只有一个定时器
    {
        if(pRemoved->m_enStatus == RUNNING)
        {
            m_pFirstRunningTimer = NULL;
        }
        else if(pRemoved->m_enStatus == PAUSED)
        {
            m_pFirstPausedTimer = NULL;
        }
    }    
    else if(pRemoved->m_pPrev == NULL) //说明在列表的最前端
    {
        if(pRemoved->m_enStatus == RUNNING)
        {
            m_pFirstRunningTimer = pRemoved->m_pNext;
        }
        else if(pRemoved->m_enStatus == PAUSED)
        {
            m_pFirstPausedTimer = pRemoved->m_pNext;
        }

        //更新被删除定时器后面的定时器的相对超时时长
        pRemoved->m_pNext->m_ulRelativeInterval += pRemoved->m_ulRelativeInterval;
            
        pRemoved->m_pNext->m_pPrev = NULL;
    }
    else if(pRemoved->m_pNext == NULL) //在列表的最后端
    {
        pRemoved->m_pPrev->m_pNext = NULL;
    }
    else //在列表的中间
    {
        pRemoved->m_pPrev->m_pNext = pRemoved->m_pNext;
        pRemoved->m_pNext->m_pPrev = pRemoved->m_pPrev;

        //更新被删除定时器后面的定时器的相对超时时长，可以加上被删除定时器相对时长，也可以使用被删除前后定时器绝对时长的差再减去前一个定时器的相对时长
        pRemoved->m_pNext->m_ulRelativeInterval += pRemoved->m_ulRelativeInterval;
    }
    OS_EXIT_CRITICAL();
  
    //初始化空闲定时器
    pRemoved->Init();
  
    //放入空闲列表    
    pRemoved->m_pNext = m_pFirstIdleTimer;
    OS_ENTER_CRITICAL();
    m_pFirstIdleTimer = pRemoved;
    OS_EXIT_CRITICAL();
}

/*************************************************
  Function:       
  Description:    本函数在定时周期到来时被调用，对定时器计数值进行递减，同时判断是否有超时；
                  定时器的长度 = 定时周期 ×定时器的Interval
  Input:          无
  Output:         无
  Return:         无
  Others:         无
*************************************************/
void TimerManager::CountAndCheck()
{
    UINT32 cpu_sr;
    Timer *pTmp = NULL;

    //MCM-15_20061102_linyu_begin
    OS_ENTER_CRITICAL();  
    if(m_pFirstRunningTimer == NULL)
    {
        OS_EXIT_CRITICAL();
        return;
    }
    
    if(m_pFirstRunningTimer->m_ulRelativeInterval != 0) //防止初始化时就为0的情况
    {
        //OS_ENTER_CRITICAL();
        m_pFirstRunningTimer->m_ulRelativeInterval--;
        //OS_EXIT_CRITICAL();
    }
    //MCM-15_20061102_linyu_end

    while((m_pFirstRunningTimer != NULL)
       && (m_pFirstRunningTimer->m_ulRelativeInterval == 0)) //这里应该不存在0减1后变成0xFFFFFFFF的情况
    {
        pTmp = m_pFirstRunningTimer;
        pTmp->Expire();        
        
        //将定时器从运行列表中删除
        if(pTmp->m_enType == PERIODIC)
        { 
            //MCM-15_20061102_linyu_begin
            //OS_ENTER_CRITICAL();
            m_pFirstRunningTimer = pTmp->m_pNext;
            if(m_pFirstRunningTimer != NULL) //如果只有一个定时器则为空，需要在此判断
            {
                m_pFirstRunningTimer->m_pPrev = NULL;
            }
            //OS_EXIT_CRITICAL();
            //MCM-15_20061102_linyu_end
          
            pTmp->m_ulRelativeInterval = pTmp->m_ulInterval; //周期性            
          
            InsertRunningTimer(pTmp);
        }
        else
        {            
            RemoveTimer(pTmp, pTmp->ulMagicNum);
        }
    }

    //MCM-15_20061102_linyu_begin
    OS_EXIT_CRITICAL();
    //MCM-15_20061102_linyu_end
}

/*************************************************
  Function:       
  Description:    本函数在用于查询定时器的统计信息
  Input:          无
  Output:         定时器的统计信息
  Return:         无
  Others:         无
*************************************************/
void TimerManager::Query(TIMER_INFO_ST &stTimerMngrInfo)
{
    stTimerMngrInfo.ucTimerCnt              = MAX_TIMER_COUNT;
    stTimerMngrInfo.ucIdleTimerCnt          = 0;
    stTimerMngrInfo.ucPausedTimerCnt        = 0;
    stTimerMngrInfo.ucNonPeriodicTimerCnt   = 0;
    stTimerMngrInfo.ucPausedTimerCnt        = 0;

    Timer *pTmp = m_pFirstRunningTimer;

    while(pTmp != NULL)
    {
        if(pTmp->m_enType == PERIODIC)
        { 
            stTimerMngrInfo.ucPeriodicTimerCnt++;
        }
        else
        {            
            stTimerMngrInfo.ucNonPeriodicTimerCnt++;
        }

        pTmp = pTmp->m_pNext;
    }

    pTmp = m_pFirstIdleTimer;
    while(pTmp != NULL)
    {
        stTimerMngrInfo.ucIdleTimerCnt++;
        pTmp = pTmp->m_pNext;
    }

    pTmp = m_pFirstPausedTimer;
    while(pTmp != NULL)
    {
        stTimerMngrInfo.ucPausedTimerCnt++;
        pTmp = pTmp->m_pNext;
    }
}