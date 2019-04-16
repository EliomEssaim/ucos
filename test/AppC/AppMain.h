/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   AppMain.cpp
    作者:     林雨
    文件说明: 本文件包含实现定时器机制的代码
    其它:
    函数列表:

    历史记录:
    1. 日期:  
       作者:  林雨
       描述:  编码完成
    2. 日期:  2006/02/16
       作者:  章杰
       描述:  修改问题MCM-13，
              减少内存使用：减少内存块的分配空间。
    3. 日期:  2006/11/07
       作者:  林玮
       描述:  修改问题MCM-20，
              增加按键触发上报功能
---------------------------------------------------------------------------*/
#ifndef AppMainH
#define AppMainH

#include "includes.h"
#include "MyBoard.h"
#include "Timer/Timer.h"
#include "../periph/usart/Usart.h"

/*OS每TICK的毫秒数*/
#define MS_PER_TICK   10

/*用于OSTICK的硬件定时器初始值*/
/*定时器被8分频，和定时器初始化有关*/
#define TC_TICK_INIT_VALUE   ((MCKKHz/8)*MS_PER_TICK)

//*****************************宏定义*****************************//

//模块ID
#define MID_OH          0
#define MID_CH          1
#define MID_TC          2
#define MID_AC          3
#define MID_LOG         4
#define MID_LAMP        5
#define MID_KEY         6
#define MID_UI          7

//任务ID定义
#define OH_TASK_ID      MID_OH
#define CH_TASK_ID      MID_CH
#define TC_TASK_ID      MID_TC
#define AC_TASK_ID      MID_AC
#define LOG_TASK_ID     MID_LOG
#define LAMP_TASK_ID    MID_LAMP
#define KEY_TASK_ID     MID_KEY
#define UI_TASK_ID      MID_UI
#define MAX_TASK_ID     (UI_TASK_ID + 1) //最大的任务ID加1

//队列长度
#define OH_QUE_SIZE     30//test
#define AC_QUE_SIZE     10
#define CH_NTH_QUE_SIZE 30//test
#define CH_STH_QUE_SIZE 10
#define CH_DEB_QUE_SIZE 10
#define LOG_QUE_SIZE    10
#define UI_QUE_SIZE     10

//任务优先级
#define TC_PRIO   4
#define CH_PRIO   6
#define KEY_PRIO  7
#define OH_PRIO   8
#define LAMP_PRIO 9
#define UI_PRIO   10
#define AC_PRIO   11
#define RC_PRIO   12
#define LOG_PRIO  14

//内存分区，用于存放实际的消息内容，额外的4字节用于内存管理的开销，详见MemMgmt.cpp
#define MEM_BLK_SIZE_16        20 
#define MEM_BLK_SIZE_16_COUNT  10
#define MEM_BLK_SIZE_256       260
//MCM-13_20061106_zhangjie_begin
#define MEM_BLK_SIZE_256_COUNT 40//100
#define MEM_BLK_SIZE_512       USART_SIZE_BUFFER//notice
#define MEM_BLK_SIZE_512_COUNT 6
//MCM-13_20061106_zhangjie_end

//**************************全局变量申明**************************//
//任务的队列
extern OS_EVENT *g_pstOHQue;
extern OS_EVENT *g_pstACQue;
extern OS_EVENT *g_pstCHNthQue;
extern OS_EVENT *g_pstCHSthQue;
extern OS_EVENT *g_pstCHDebQue;
extern OS_EVENT *g_pstLogQue;
extern OS_EVENT *g_pstUIQue;
extern OS_EVENT *g_pstUICHQue;

extern void *g_apvOHQue[];
extern void *g_apvACQue[];
extern void *g_apvCHNthQue[];//北向接口消息队列
extern void *g_apvCHSthQue[];//南向接口消息队列
extern void *g_apvCHDebQue[];//调试接口消息队列
extern void *g_apvLogQue[];  //日志消息队列
extern void *g_apvUIQue[];
extern void *g_apvUICHQue[];

//定时器管理器
#define TIMER_UNIT_MS 100//定时器单位时间，单位:MS
extern TimerManager g_TimerMngr;

//uC/OS的API中使用的变量
extern UCHAR g_ucErr;

//内存分区
extern OS_MEM *g_pstMemPool16;
extern OS_MEM *g_pstMemPool256;
extern OS_MEM *g_pstMemPool512;


#endif
