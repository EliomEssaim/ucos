//****************************����ͷ�ļ�**************************//
#include "../include/Interface.h"
#include "../AppMain.h"
#include "Timer.h"
#include "../Util/Util.h"


//*****************************�궨��*****************************//

//**************************ȫ�ֱ�������**************************//

//**************************ȫ�ֱ�������**************************//

//****************************��������****************************//


void TCTask(void *pv)
{
    //����ѭ��
    while(1)
    {
        MyDelay(TIMER_UNIT_MS);
        g_TimerMngr.CountAndCheck();
    }    
}



