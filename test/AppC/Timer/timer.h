#ifndef TimerH
#define TimerH

#include "UserType.h"

#define MAX_TIMER_COUNT 10

enum TIMER_STATUS_EN
{
    IDLE = 0, //初始状态
    RUNNING,  //正在运行
    STOPPED,  //停止
    PAUSED    //暂停
};

enum TIMER_TYPE_EN
{
    NONPERIODIC = 0, //定时器可以结束
    PERIODIC //重新定时，适用于周期性定时器
};

typedef void (*TX_TIMER_FUNC)(void*); //定时器回调函数指针类型
class Timer
{
#ifdef UNIT_TEST
public:
#else
private:
#endif
    Timer *m_pPrev;
    Timer *m_pNext;

    //ULONG m_ulXprTime;        //超时绝对时间
    ULONG m_ulInterval;         //定时器时长
    ULONG m_ulRelativeInterval; //相对于前一个定时器的时长，将被计时递减
    TIMER_STATUS_EN m_enStatus; //定时器状态
    TIMER_TYPE_EN m_enType;     //定时器的类型

    void* m_pvContext;          //上下文，存放回调函数需要的参数，注意，如果指向的是动态申请的内存则要由调用方去释放(在停止定时器和超时时)
    TX_TIMER_FUNC m_pfFunction; //定时器超时执行的回调函数指针

    Timer (const Timer& Obj);
    Timer& operator= (const Timer& Obj);
public:
    ULONG ulMagicNum;           //在创建定时器时分配，并在删除定时器时用于分辨是否是对应的定时器

    Timer(){}
    //~Timer();
    
    void Init(TX_TIMER_FUNC pfFunction, void* pvContext, ULONG ulInterval, TIMER_TYPE_EN enTimerType);
    void Init();
    void Expire(); //定时器的超时操作
    void *Context()
    {
        return m_pvContext;
    }
    
    friend class TimerManager;
};

typedef struct
{
    UCHAR ucTimerCnt;               //定时器总数
    UCHAR ucIdleTimerCnt;           //空闲的定时器个数
    UCHAR ucPeriodicTimerCnt;       //周期性定时器个数
    UCHAR ucNonPeriodicTimerCnt;    //非周期性定时器个数
    UCHAR ucPausedTimerCnt;         //暂停的定时器个数
}TIMER_INFO_ST;

class TimerManager
{
#ifdef UNIT_TEST
public:
#else
private:
#endif
    //列表的基本内容和操作，与其中业务内容无关
    Timer m_aList[MAX_TIMER_COUNT];
    Timer* m_pFirstIdleTimer;
    Timer* m_pFirstRunningTimer;
    Timer* m_pFirstPausedTimer;    

    ULONG m_ulMagicNum; //用于给定时器分配magic number
    
    void InsertRunningTimer(Timer *pTimer); //在CreateTimer()内调用
    //Timer* Next();
    
public:
    TimerManager();
    void Init();
    Timer* CreateTimer(TX_TIMER_FUNC pfFunction, void* pvContext,
                       ULONG ulInterval, TIMER_TYPE_EN enTimerType,
                       ULONG *pulMagicNum);
    void RemoveTimer(Timer *pRemoved, ULONG ulMagicNum);
    void CountAndCheck(); //定时器计数并检查是否超时

    void Query(TIMER_INFO_ST &);
};

#endif