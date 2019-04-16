#include "At91M55800.h"
/*
该函数使用WATCHDOG内部重启的方式重新启动系统
目前采用最快的重启时间，4ms(32MHz条件下)
*/
void Restart(void)
{
    WD_OMR = 0x2340;
    //HPCV = 0;WDCLKS = MCK/32,最快重启时间
    WD_CMR = 0x3700;
    WD_CR = 0xC071;
    //采用内部重启方式
    WD_OMR = 0x2343;
    while(1);
}
/*
该函数使用打开WATCHDOG
目前采用最慢的wd时间，8s(32MHz条件下)
*/
void WdOpen(void)
{
    WD_OMR = 0x2340;
    //HPCV = 1111;WDCLKS = MCK/4096,最慢重启时间
    WD_CMR = 0x373F;
    WD_CR = 0xC071;
    //采用内部重启方式
    WD_OMR = 0x2343;
}
/*
该函数使用关闭WATCHDOG
*/
void WdClose(void)
{
    WD_OMR = 0x2340;
}
/*
该函数用于喂狗
*/
void WdRestart(void)
{
    WD_CR = 0xC071;
}