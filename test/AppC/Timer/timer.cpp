/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   Timer.cpp
    ����:     ����
    �ļ�˵��: ���ļ�����ʵ�ֶ�ʱ�����ƵĴ���
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  2006/02/16
       ����:  ����
       ����:  �������
    2. ����:  2006/11/02
       ����:  ����
       ����:  �޸�����MCM-14��ʹ�ú꿪��M3���л�OS_ENTER_CRITICAL()
              ��OS_EXIT_CRITICAL()�Ķ��壬��֤�ڷ��������п����ж�
              ����Ч��
              �޸�����MCM-15���ڽ���TimerManager::CountAndCheck()��
              �͹��жϣ��뿪ǰ�ſ��жϡ�����͸���Ҫ��ʱ����һ��
              Ҫ��̡�
---------------------------------------------------------------------------*/

//MCM-14_20061102_linyu_begin
#ifndef M3
#include "Includes.h" //ʹ�õ����ٽ����궨��
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
  Description:    ��������ʹ�ö�ʱ��ǰ�����ʼ��
  Input:          pfFunc: �ص�����ָ��
                  pvCtx:  �ص�����ʹ�õĲ���ָ��
                  ulInter:��ʱ��ʱ��
                  enTimerType: ��ʱ�����ͣ���Ϊ�����Ժͷ�������
  Output:         ��
  Return:         ��
  Others:         ��
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
  Description:    ���������ڽ���ʱ���������
  Input:          ��
  Output:         ��
  Return:         ��
  Others:         ��
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
  Description:    �������ڶ�ʱ����ʱʱ�����ã�ִ�ж�ʱ���Ļص�����
  Input:          ��
  Output:         ��
  Return:         ��
  Others:         ��
*************************************************/
void Timer::Expire()
{   
    if(m_enStatus == RUNNING) //�����������
    {
        m_enStatus = STOPPED; //����ͣ���������º�������???ͬʱ�ڻص�������ɾ����ʱ��Ҳ����������

        m_pfFunction(m_pvContext); //ִ�г�ʱ����
            
        m_enStatus = RUNNING;
    }
}

/*************************************************
  Function:       
  Description:    ��������ʼ����ʱ�����������������еĶ�ʱ�����ɿ����б�
  Input:          ��
  Output:         ��
  Return:         ��
  Others:         ��
*************************************************/
TimerManager::TimerManager()
{
    Init();
}

//���ڲ�֪��Ϊ��G++�������ص����ϣ�Ĭ�Ϲ��캯�����ᱻ���ã����ʹ��һ����ʾ��ʼ������
void TimerManager::Init()
{
    m_pFirstRunningTimer = NULL;
    m_pFirstPausedTimer  = NULL;
    
    //����ʱ����������б��У������б���һ�������б�
    for(int i = 0; i < MAX_TIMER_COUNT - 1; i++)
    {
        m_aList[i].m_pNext = &m_aList[i + 1];
        m_aList[i].m_pPrev = NULL;
    }
    
    //���һ���ڵ�
    m_aList[MAX_TIMER_COUNT - 1].m_pPrev = NULL;
    m_aList[MAX_TIMER_COUNT - 1].m_pNext = NULL;

    m_pFirstIdleTimer = m_aList;

    m_ulMagicNum = 0;
}

/*************************************************
  Function:       
  Description:    ����������ʱ�����������б��У����б��еĶ�ʱ����ʱʱ���ȽϺ�ȷ��
                  �����λ�ã�ͬʱ������ǰһ����ʱ�������ʱ�������������ʱ��ͬʱ
                  ��ʱ��������Ķ�ʱ����ǰ
  Input:          pTimer����ʱ��
  Output:         ��
  Return:         ��
  Others:         ��
*************************************************/
void TimerManager::InsertRunningTimer(Timer *pTimer)
{
    UINT32 cpu_sr;
    Timer *pTmp = NULL;
    Timer *pPrev= NULL;

    pTimer->m_enStatus = RUNNING;
    
    //�б�Ϊ�գ���ֱ�Ӳ�����ǰ��
    if(m_pFirstRunningTimer == NULL)
    {
        pTimer->m_pPrev = NULL;
        pTimer->m_pNext = NULL;

        OS_ENTER_CRITICAL();
        m_pFirstRunningTimer = pTimer;        
        OS_EXIT_CRITICAL();
        return;
    }
    
    //��ʱʱ����С����ֱ�Ӳ����б���ǰ��
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
        if(pTimer->m_ulInterval <= ulTotalInter + pTmp->m_ulRelativeInterval) //�����б�
        {
            //���±�����Ķ�ʱ������
            pTimer->m_ulRelativeInterval = pTimer->m_ulInterval - ulTotalInter;
            pTimer->m_pNext = pTmp;
            pTimer->m_pPrev = pPrev;

            OS_ENTER_CRITICAL();
            //������̽ڵ�����ʱ��
            pTmp->m_ulRelativeInterval = pTmp->m_ulRelativeInterval
                                       - pTimer->m_ulRelativeInterval;
            
            //�������һ���ڵ����ϵ
            pTmp->m_pPrev = pTimer;
            
            //������ǰһ���ڵ����ϵ
            pPrev->m_pNext = pTimer;
            
            OS_EXIT_CRITICAL();
            return;
        }

        ulTotalInter += pTmp->m_ulRelativeInterval;
        pPrev= pTmp;
        pTmp = pTmp->m_pNext;        
    }
    
    //����ʱ�������б����ĩ�� 
    pTimer->m_ulRelativeInterval = pTimer->m_ulInterval - ulTotalInter;
    pTimer->m_pNext = NULL;
    pTimer->m_pPrev = pPrev;

    OS_ENTER_CRITICAL();    
    pPrev->m_pNext  = pTimer;    
    OS_EXIT_CRITICAL();
}

/*************************************************
  Function:       
  Description:    �������ӿ����б��л�ȡ��ʱ������ʼ�����ٽ�����������б�
  Input:          pfFunction: �ص�����ָ��
                  pvContext:  �ص�����ʹ�õĲ���ָ��
                  ulInterval: ��ʱ��ʱ���������ʱ���ĵ�λ�Ƕ�ʱ���ľ���
                  enTimerType:��ʱ�����ͣ���Ϊ�����Ժͷ�������
  Output:         pulMagicNum:��ʱ������ʱ�����magic number
  Return:         ���ش����Ľ����NULL��ʾ����ʧ�ܣ����򷵻ش����Ķ�ʱ��
  Others:         ��
*************************************************/
Timer* TimerManager::CreateTimer(TX_TIMER_FUNC pfFunction, void* pvContext,
                                 ULONG ulInterval, TIMER_TYPE_EN enTimerType,
                                 ULONG *pulMagicNum)
{
    UINT32 cpu_sr;
    Timer *pTmp = NULL;
    
    //�ӿ����б��л�ö�ʱ��
    if(m_pFirstIdleTimer == NULL)
    {
        return NULL; //û�ж���Ķ�ʱ��
    }
    
    OS_ENTER_CRITICAL();
    pTmp = m_pFirstIdleTimer;
    m_pFirstIdleTimer = m_pFirstIdleTimer->m_pNext; //�������ж�ʱ���б�    
    OS_EXIT_CRITICAL();
    
    pTmp->Init(pfFunction, pvContext, ulInterval, enTimerType); //��ʼ����ʱ��

    pTmp->ulMagicNum = m_ulMagicNum++;
    *pulMagicNum = pTmp->ulMagicNum;
    
    InsertRunningTimer(pTmp); //����ʱ���������ж���
    
    return pTmp;
}

/*************************************************
  Function:       
  Description:    ����������ʱ���������б������ͣ�б���ɾ�����Żص������б���
  Input:          pRemoved:  ��ɾ���Ķ�ʱ��
                  ulMagicNum:������ʱ��ʱ�����magic number
  Output:         ��
  Return:         ��
  Others:         ��
*************************************************/
void TimerManager::RemoveTimer(Timer *pRemoved, ULONG ulMagicNum)
{
    UINT32 cpu_sr;
    
    //��ֹ�����ָ��
    if(pRemoved == 0)
    {
        return;
    }

    if(pRemoved->ulMagicNum != ulMagicNum)
    {
        return;
    }
    
    //��ʱ���Ƿ��Ѿ���ɾ�����߳�ʱ
    if((pRemoved->m_enStatus == IDLE) || (pRemoved->m_enStatus == STOPPED))
    {
        return;
    }

    //���б���ɾ������ǰ��ڵ���������
    OS_ENTER_CRITICAL();
    if((pRemoved->m_pPrev == NULL) && (pRemoved->m_pNext == NULL)) //˵���б���ֻ��һ����ʱ��
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
    else if(pRemoved->m_pPrev == NULL) //˵�����б����ǰ��
    {
        if(pRemoved->m_enStatus == RUNNING)
        {
            m_pFirstRunningTimer = pRemoved->m_pNext;
        }
        else if(pRemoved->m_enStatus == PAUSED)
        {
            m_pFirstPausedTimer = pRemoved->m_pNext;
        }

        //���±�ɾ����ʱ������Ķ�ʱ������Գ�ʱʱ��
        pRemoved->m_pNext->m_ulRelativeInterval += pRemoved->m_ulRelativeInterval;
            
        pRemoved->m_pNext->m_pPrev = NULL;
    }
    else if(pRemoved->m_pNext == NULL) //���б������
    {
        pRemoved->m_pPrev->m_pNext = NULL;
    }
    else //���б���м�
    {
        pRemoved->m_pPrev->m_pNext = pRemoved->m_pNext;
        pRemoved->m_pNext->m_pPrev = pRemoved->m_pPrev;

        //���±�ɾ����ʱ������Ķ�ʱ������Գ�ʱʱ�������Լ��ϱ�ɾ����ʱ�����ʱ����Ҳ����ʹ�ñ�ɾ��ǰ��ʱ������ʱ���Ĳ��ټ�ȥǰһ����ʱ�������ʱ��
        pRemoved->m_pNext->m_ulRelativeInterval += pRemoved->m_ulRelativeInterval;
    }
    OS_EXIT_CRITICAL();
  
    //��ʼ�����ж�ʱ��
    pRemoved->Init();
  
    //��������б�    
    pRemoved->m_pNext = m_pFirstIdleTimer;
    OS_ENTER_CRITICAL();
    m_pFirstIdleTimer = pRemoved;
    OS_EXIT_CRITICAL();
}

/*************************************************
  Function:       
  Description:    �������ڶ�ʱ���ڵ���ʱ�����ã��Զ�ʱ������ֵ���еݼ���ͬʱ�ж��Ƿ��г�ʱ��
                  ��ʱ���ĳ��� = ��ʱ���� ����ʱ����Interval
  Input:          ��
  Output:         ��
  Return:         ��
  Others:         ��
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
    
    if(m_pFirstRunningTimer->m_ulRelativeInterval != 0) //��ֹ��ʼ��ʱ��Ϊ0�����
    {
        //OS_ENTER_CRITICAL();
        m_pFirstRunningTimer->m_ulRelativeInterval--;
        //OS_EXIT_CRITICAL();
    }
    //MCM-15_20061102_linyu_end

    while((m_pFirstRunningTimer != NULL)
       && (m_pFirstRunningTimer->m_ulRelativeInterval == 0)) //����Ӧ�ò�����0��1����0xFFFFFFFF�����
    {
        pTmp = m_pFirstRunningTimer;
        pTmp->Expire();        
        
        //����ʱ���������б���ɾ��
        if(pTmp->m_enType == PERIODIC)
        { 
            //MCM-15_20061102_linyu_begin
            //OS_ENTER_CRITICAL();
            m_pFirstRunningTimer = pTmp->m_pNext;
            if(m_pFirstRunningTimer != NULL) //���ֻ��һ����ʱ����Ϊ�գ���Ҫ�ڴ��ж�
            {
                m_pFirstRunningTimer->m_pPrev = NULL;
            }
            //OS_EXIT_CRITICAL();
            //MCM-15_20061102_linyu_end
          
            pTmp->m_ulRelativeInterval = pTmp->m_ulInterval; //������            
          
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
  Description:    �����������ڲ�ѯ��ʱ����ͳ����Ϣ
  Input:          ��
  Output:         ��ʱ����ͳ����Ϣ
  Return:         ��
  Others:         ��
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