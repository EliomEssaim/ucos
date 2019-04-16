//****************************����ͷ�ļ�**************************//
//#include "../AppMain.h"
#include "../Util/Util.h"
#include "../OH/CMCC_1_Ext.h"
//#include "../include/Interface.h"
#include "../../include/UserType.h"
#include "../../include/at91m55800.h"
//#include "../../include/MyBoard.h"
#include "../../periph/pio/pio.h"
#include "../AC/Actask.h"
#include "Lamp.h"


#ifdef YK_POI
#include "./poi_lamptask.cpp"
#else
#include "./gen_lamptask.cpp"
#endif


/*************************************************
  Function:      LampTask
  Description:   
  Calls:
  Called By:
  Input:          
  Output:     
  Return:         
  Others:
*************************************************/
void LampTask(void *pv)
{
    while(1)
    {
        MyDelay(666);  //ʹÿһ��ѭ��ʱ��ԼΪ0.25��

        UpdateAllLamp();    //���ݸ��澯�������źŵƵ�״̬

        ControlAllLamp();   //���ݸ��źŵ�״̬�����ƵƵ��������˸

    }//while(1)
}
