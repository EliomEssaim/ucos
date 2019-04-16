/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   UITask.cpp
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
#include "../include/Interface.h"
#include "../AppMain.h"
#include "../Driver/key/key.h"
#include "../MM/MemMgmt.h"

//###########################################################
//                    以下为 UITask
//###########################################################
/*************************************************
  Function:      UITaskInit
  Description:   本函数应该在任务未运行前调用
  Others:        UI任务的初始化
*************************************************/
void UITaskInit()
{
    //创建队列
    g_pstUIQue = OSQCreate(g_apvUIQue, UI_QUE_SIZE);

}

/****************************************************
 Function:    UITask
 Description: UI任务入口
 Others:      捕获KeyScan任务消息，根据不同键值和按键
              状态，执行不同操作
****************************************************/
void UITask(void *pv)
{
    UCHAR ucGotKeyType;   //KeyScan任务扫描到的键值
    USHORT usGotKeyValue; //KeyScan任务扫描到的按键状态
    USHORT usIdleTimes;   //按键空闲总时间，如果大于60秒，则判定为空闲

    while(1)
    {
        void *pvPrmv = OSQPend(g_pstUIQue, 20/MS_PER_TICK, &g_ucErr); //DELAY 20MS
        if(pvPrmv == 0)
        {
            usIdleTimes++;
            continue;
        }
        if(*((ULONG*)pvPrmv) == KEY_DOWN_EVENT)
        {
            usGotKeyValue = ((KEY_EVENT_ST *)pvPrmv)->usKey;
            ucGotKeyType = ((KEY_EVENT_ST *)pvPrmv)->ucKeyType;
            usIdleTimes = 0;
            if((usGotKeyValue == KEY_VALUE_STA_INIT) ||
               (usGotKeyValue == KEY_VALUE_INSPECTION) ||
               (usGotKeyValue == KEY_VALUE_REPAIR_CONFIRMED) ||
               (usGotKeyValue == KEY_VALUE_CONFIG_CHANGED))
            {
                KEY_EVENT_ST *pstMessage = (KEY_EVENT_ST *)MyMalloc(g_pstMemPool16);
                if(pstMessage == 0)
                {
                    MyFree(pvPrmv);
                    continue; //没有办法
                }

                pstMessage->ulPrmvType = KEY_DOWN_EVENT;//
                pstMessage->usKey = usGotKeyValue;
                pstMessage->ucKeyType = ucGotKeyType;

                if(OSQPost(g_pstOHQue, pstMessage) != OS_NO_ERR)
                {
                    MyFree((void * )pstMessage);
                }                 
            }
            else
            {
                //g_stScreen.Dispatch(usGotKeyValue, ucGotKeyType);
            }
            MyFree(pvPrmv);
        }
        else
        {
            MyFree(pvPrmv);
        }
    }//while(1)
}
