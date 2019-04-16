/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   KeyTask.cpp
    作者:     林玮
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  
       作者:  林玮
       描述:  代码编写完成
---------------------------------------------------------------------------*/
//****************************包含头文件**************************//
#include "../Util/Util.h"
#include "../include/Interface.h"
#include "../../include/UserType.h"
#include "../../include/at91m55800.h"
//#include "../../include/MyBoard.h"
#include "../Driver/key/key.h"
#include "../MM/MemMgmt.h"
#include "../Appmain.h"

/***** test *****
#include "../../periph/usart/Usart.h"
/****************/

extern "C" USHORT CountKeyValue(UCHAR* aucKeyScan);

extern UCHAR ucScanBeginFlag;  //是否需要开始判断按键状态的标志，由key.c置高
extern USHORT usFirstKeyValue;  //当有按键被按下时，按键最初的状态

/*************************************************
  Function:      CheckKeyNumbers
  Description:   根据按键扫描结果，计算出当前键值
  Called By:     
  Input:         usKeyValue - 当前键值
  Return:        被按下按键的个数
*************************************************/
UCHAR CheckKeyNumbers(USHORT usKeyValue)
{
    UCHAR i, ucNums = 0;
    for(i=0; i<16; i++)
    {
        if( (usKeyValue & 0x01) == 1 )
        {
            ucNums ++;
        }
        usKeyValue = usKeyValue >> 1;
    }
    return ucNums;
}

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
void KEYTask(void *pv)
{
    UCHAR ucTempNum, ucScanTimes, aucKeyValue[4];
    //UCHAR ucKeyStatus = 0;     //被按下的按键的状态
    USHORT usKeyNewValue = 0;  //按键的最终状态
    USHORT usKeyValueToUI;      //发给UI任务的最后键值
    UCHAR ucKeyTypeToUI;       //发给UI任务的按键类型，分长按，组合或单独
    UCHAR ucKeyValueIsGot = 0;  //是否已检测到键值
    
    while(1)
    {
        MyDelay(10);  //延时10ms
        if(ucScanBeginFlag != 0)
        {
            if(ucScanBeginFlag == 2)  //“==2”说明有按键刚被按下
            {
                //此处判断是否有多个按键被扫描到,多个键被同时按下的话，直接返回组合键值
                ucTempNum = CheckKeyNumbers(usFirstKeyValue);
                if(ucTempNum > 1)
                {
                    ucKeyValueIsGot = 1;
                    usKeyValueToUI = usFirstKeyValue; //返回当前键值usFirstKeyValue
                    ucKeyTypeToUI = KEY_STATUS_COMPOUNDING; //返回当前按键状态为“组合”
                    MyDelay(100);
                    ucScanBeginFlag = 0;  //按键扫描开始标志清零
                }
                else if(ucTempNum == 0)
                {
                    ucScanBeginFlag = 0;  //按键扫描开始标志清零
                }
                else
                {
                    ucScanBeginFlag = 1;  //按键扫描开始标志置1
                    ucScanTimes = 1;  //扫描次数置1
                }
            }

            MyDelay(10);  //延时10ms，防抖动

            aucKeyValue[0] = *(KEY_BASE + 0xfd);  //重新扫描按键状态
            aucKeyValue[1] = *(KEY_BASE + 0xfb);
            aucKeyValue[2] = *(KEY_BASE + 0xf7);
            aucKeyValue[3] = *(KEY_BASE + 0xef);

            usKeyNewValue = CountKeyValue(aucKeyValue);  //根据扫描结果，计算当前键值

            ucScanTimes++;  //扫描次数加1

            if(usKeyNewValue != usFirstKeyValue)
            {
                if(usKeyNewValue == 0)
                {
                    if(ucScanTimes >= 3)  //此为正常情况，按键被按下和放开
                    {
                        ucKeyValueIsGot = 1;
                        usKeyValueToUI = usFirstKeyValue; //返回当前键值usFirstKeyValue
                        ucKeyTypeToUI = KEY_STATUS_SINGLE; //返回当前按键状态为“正常”
                        MyDelay(100);
                        ucScanBeginFlag = 0;  //按键扫描开始标志清零
                    }
                    else  //如果在前三次的扫描中发现没有按键被按下，说明为抖动，放弃本次扫描
                    {
                        ucScanBeginFlag = 0;  //按键扫描开始标志清零
                    }
                }
                else if((usKeyNewValue | usFirstKeyValue) == usKeyNewValue)
                {  //判断为组合键
                    ucKeyValueIsGot = 1;
                    usKeyValueToUI = usKeyNewValue; //返回当前键值usKeyNewValue
                    ucKeyTypeToUI = KEY_STATUS_COMPOUNDING; //返回当前按键状态为“组合”
                    MyDelay(100);
                    ucScanBeginFlag = 0;  //按键扫描开始标志清零
                }
                else
                {
                    usFirstKeyValue = usKeyNewValue;
                    ucScanBeginFlag = 2;  //重新激活按键扫描任务
                }
            }
            else
            {
                if(ucScanTimes >= 150)
                {
                    ucKeyValueIsGot = 1;
                    usKeyValueToUI = usFirstKeyValue; //返回当前键值usFirstKeyValue
                    ucKeyTypeToUI = KEY_STATUS_LONGPRESS; //返回当前按键状态为“长按”
                    MyDelay(100);
                    ucScanBeginFlag = 0;  //按键扫描开始标志清零
                }
            }

            //如果已得到键值，则发消息给UI任务
            if(ucKeyValueIsGot == 1)
            {
  /******** test ********
    UCHAR ucOutValue1, ucOutValue2;
    ucOutValue1 = (UCHAR)(usKeyValueToUI >> 8);
    ucOutValue2 = (UCHAR)(usKeyValueToUI);
    UsartWrite(&g_stUsart0, &ucOutValue1, 1);
    while(g_stUsart0.pstData->pucTxIn!=g_stUsart0.pstData->pucTxOut);
    UsartWrite(&g_stUsart0, &ucOutValue2, 1);
    while(g_stUsart0.pstData->pucTxIn!=g_stUsart0.pstData->pucTxOut);
    UsartWrite(&g_stUsart0, &ucKeyTypeToUI, 1);
    while(g_stUsart0.pstData->pucTxIn!=g_stUsart0.pstData->pucTxOut);
  /**********************/
                KEY_EVENT_ST *pstMessage = (KEY_EVENT_ST *)MyMalloc(g_pstMemPool16);
                if(pstMessage == 0)
                {
                    continue; //没有办法
                }

                pstMessage->ulPrmvType = KEY_DOWN_EVENT;//
                pstMessage->usKey = usKeyValueToUI;
                pstMessage->ucKeyType = ucKeyTypeToUI;

                if(OSQPost(g_pstUIQue, pstMessage) != OS_NO_ERR)
                {
                    MyFree((void * )pstMessage);
                }

                ucKeyValueIsGot = 0;
            }
        }
    }//while(1)
}
