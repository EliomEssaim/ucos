/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   ACTask.h
    作者:     林玮
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  2006/09/28
       作者:  林玮
       描述:  编码完成
    2. 日期:  2006/11/29
       作者:  章杰
       描述:  修改问题MCM-41，
              把高于低于检测范围的宏定义移到此位置
---------------------------------------------------------------------------*/
#ifndef AcH
#define AcH

#define MYALARM   g_uAlarmItems.stAlarmItemStruct
#define MYENABLE  g_uAlarmEnable.stAlarmEnableStruct
#define ALARM  1
#define NOT_ALARM  0

#define SUPPORTED 1     //参量是否支持，支持为1，不支持为0
#define NOT_SUPPORTED 0

//MCM-41_20061129_zhangjie_begin
#define HIGHER_FLAG 6 //高于检测范围时的返回值
#define LOWER_FLAG  5 //低于检测范围时的返回值
#define NORMAL_FLAG 0 //在检测范围内时的返回值
//MCM-41_20061129_zhangjie_end

#endif
