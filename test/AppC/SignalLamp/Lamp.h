#ifndef LampH
#define LampH

#define FLICKER 1       //支持闪烁
#define NOT_FLICKER 0   //不支持闪烁

/*****************************************/
// 告警信号灯采用原SETATT的18个输出脚
// 所以将结构定义如下
/*****************************************/
typedef struct
{
    UCHAR ucPioNum;           //硬件管脚
    UCHAR ucGroupNum;         //处于SETATT输出的第几组
    UCHAR ucLampStatus;       //灯的亮灭状态 0灭 1亮 2闪烁
    UCHAR ucIsItFlicker;      //是否支持闪烁，1支持，0不支持
    UCHAR ucNowStatus;        //灯的当前状态，当灯的状态为闪烁时有效
}POI_SIGNAL_LAMP_ST;

typedef struct
{
    ULONG ulPioNum;           //硬件管脚
    UCHAR ucLampStatus;       //灯的亮灭状态
    UCHAR ucIsItFlicker;      //是否支持闪烁，1支持，0不支持
    UCHAR ucNowStatus;        //灯的当前状态，当灯的状态为闪烁时有效
}SIGNAL_LAMP_ST;

/*****************************************/
void LampTask(void);

extern void SetReportLampStatus(UCHAR ucLampSta);  //修改上报状态指示灯的当前状态，由其他任务执行

#endif
