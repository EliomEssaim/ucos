#ifndef TimerH
#define TimerH

#include "UserType.h"

#define MAX_TIMER_COUNT 10

enum TIMER_STATUS_EN
{
    IDLE = 0, //��ʼ״̬
    RUNNING,  //��������
    STOPPED,  //ֹͣ
    PAUSED    //��ͣ
};

enum TIMER_TYPE_EN
{
    NONPERIODIC = 0, //��ʱ�����Խ���
    PERIODIC //���¶�ʱ�������������Զ�ʱ��
};

typedef void (*TX_TIMER_FUNC)(void*); //��ʱ���ص�����ָ������
class Timer
{
#ifdef UNIT_TEST
public:
#else
private:
#endif
    Timer *m_pPrev;
    Timer *m_pNext;

    //ULONG m_ulXprTime;        //��ʱ����ʱ��
    ULONG m_ulInterval;         //��ʱ��ʱ��
    ULONG m_ulRelativeInterval; //�����ǰһ����ʱ����ʱ����������ʱ�ݼ�
    TIMER_STATUS_EN m_enStatus; //��ʱ��״̬
    TIMER_TYPE_EN m_enType;     //��ʱ��������

    void* m_pvContext;          //�����ģ���Żص�������Ҫ�Ĳ�����ע�⣬���ָ����Ƕ�̬������ڴ���Ҫ�ɵ��÷�ȥ�ͷ�(��ֹͣ��ʱ���ͳ�ʱʱ)
    TX_TIMER_FUNC m_pfFunction; //��ʱ����ʱִ�еĻص�����ָ��

    Timer (const Timer& Obj);
    Timer& operator= (const Timer& Obj);
public:
    ULONG ulMagicNum;           //�ڴ�����ʱ��ʱ���䣬����ɾ����ʱ��ʱ���ڷֱ��Ƿ��Ƕ�Ӧ�Ķ�ʱ��

    Timer(){}
    //~Timer();
    
    void Init(TX_TIMER_FUNC pfFunction, void* pvContext, ULONG ulInterval, TIMER_TYPE_EN enTimerType);
    void Init();
    void Expire(); //��ʱ���ĳ�ʱ����
    void *Context()
    {
        return m_pvContext;
    }
    
    friend class TimerManager;
};

typedef struct
{
    UCHAR ucTimerCnt;               //��ʱ������
    UCHAR ucIdleTimerCnt;           //���еĶ�ʱ������
    UCHAR ucPeriodicTimerCnt;       //�����Զ�ʱ������
    UCHAR ucNonPeriodicTimerCnt;    //�������Զ�ʱ������
    UCHAR ucPausedTimerCnt;         //��ͣ�Ķ�ʱ������
}TIMER_INFO_ST;

class TimerManager
{
#ifdef UNIT_TEST
public:
#else
private:
#endif
    //�б�Ļ������ݺͲ�����������ҵ�������޹�
    Timer m_aList[MAX_TIMER_COUNT];
    Timer* m_pFirstIdleTimer;
    Timer* m_pFirstRunningTimer;
    Timer* m_pFirstPausedTimer;    

    ULONG m_ulMagicNum; //���ڸ���ʱ������magic number
    
    void InsertRunningTimer(Timer *pTimer); //��CreateTimer()�ڵ���
    //Timer* Next();
    
public:
    TimerManager();
    void Init();
    Timer* CreateTimer(TX_TIMER_FUNC pfFunction, void* pvContext,
                       ULONG ulInterval, TIMER_TYPE_EN enTimerType,
                       ULONG *pulMagicNum);
    void RemoveTimer(Timer *pRemoved, ULONG ulMagicNum);
    void CountAndCheck(); //��ʱ������������Ƿ�ʱ

    void Query(TIMER_INFO_ST &);
};

#endif