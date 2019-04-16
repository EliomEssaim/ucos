//****************************包含头文件**************************//
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
        MyDelay(666);  //使每一轮循环时间约为0.25秒

        UpdateAllLamp();    //根据各告警量更新信号灯的状态

        ControlAllLamp();   //根据各信号灯状态，控制灯的亮灭或闪烁

    }//while(1)
}
