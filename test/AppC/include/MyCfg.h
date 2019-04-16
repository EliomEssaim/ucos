#ifndef MyCfgH
#define MyCfgH

#define MY_LOG_EN           1   //日志功能的宏开关

#define MY_CHECK_EN         1
#if MY_CHECK_EN > 0
#define MY_CHECK_STACK_EN   1   //检查任务堆栈的开关
#define MY_CHECK_MEM_EN     1   //检查内存的开关
#define MY_CHECK_QUE_EN     1   //检查队列的开关
#endif

#endif
