/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   UITask.cpp
    ����:     ����
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  
       ����:  ����
       ����:  �����д���
---------------------------------------------------------------------------*/
//****************************����ͷ�ļ�**************************//
#include "../include/Interface.h"
#include "../AppMain.h"
#include "../Driver/key/key.h"
#include "../MM/MemMgmt.h"

//###########################################################
//                    ����Ϊ UITask
//###########################################################
/*************************************************
  Function:      UITaskInit
  Description:   ������Ӧ��������δ����ǰ����
  Others:        UI����ĳ�ʼ��
*************************************************/
void UITaskInit()
{
    //��������
    g_pstUIQue = OSQCreate(g_apvUIQue, UI_QUE_SIZE);

}

/****************************************************
 Function:    UITask
 Description: UI�������
 Others:      ����KeyScan������Ϣ�����ݲ�ͬ��ֵ�Ͱ���
              ״̬��ִ�в�ͬ����
****************************************************/
void UITask(void *pv)
{
    UCHAR ucGotKeyType;   //KeyScan����ɨ�赽�ļ�ֵ
    USHORT usGotKeyValue; //KeyScan����ɨ�赽�İ���״̬
    USHORT usIdleTimes;   //����������ʱ�䣬�������60�룬���ж�Ϊ����

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
                    continue; //û�а취
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
