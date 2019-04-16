#include "At91M55800.h"
/*
�ú���ʹ��WATCHDOG�ڲ������ķ�ʽ��������ϵͳ
Ŀǰ������������ʱ�䣬4ms(32MHz������)
*/
void Restart(void)
{
    WD_OMR = 0x2340;
    //HPCV = 0;WDCLKS = MCK/32,�������ʱ��
    WD_CMR = 0x3700;
    WD_CR = 0xC071;
    //�����ڲ�������ʽ
    WD_OMR = 0x2343;
    while(1);
}
/*
�ú���ʹ�ô�WATCHDOG
Ŀǰ����������wdʱ�䣬8s(32MHz������)
*/
void WdOpen(void)
{
    WD_OMR = 0x2340;
    //HPCV = 1111;WDCLKS = MCK/4096,��������ʱ��
    WD_CMR = 0x373F;
    WD_CR = 0xC071;
    //�����ڲ�������ʽ
    WD_OMR = 0x2343;
}
/*
�ú���ʹ�ùر�WATCHDOG
*/
void WdClose(void)
{
    WD_OMR = 0x2340;
}
/*
�ú�������ι��
*/
void WdRestart(void)
{
    WD_CR = 0xC071;
}