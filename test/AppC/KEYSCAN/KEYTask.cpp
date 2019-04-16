/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   KeyTask.cpp
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

extern UCHAR ucScanBeginFlag;  //�Ƿ���Ҫ��ʼ�жϰ���״̬�ı�־����key.c�ø�
extern USHORT usFirstKeyValue;  //���а���������ʱ�����������״̬

/*************************************************
  Function:      CheckKeyNumbers
  Description:   ���ݰ���ɨ�������������ǰ��ֵ
  Called By:     
  Input:         usKeyValue - ��ǰ��ֵ
  Return:        �����°����ĸ���
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
    //UCHAR ucKeyStatus = 0;     //�����µİ�����״̬
    USHORT usKeyNewValue = 0;  //����������״̬
    USHORT usKeyValueToUI;      //����UI���������ֵ
    UCHAR ucKeyTypeToUI;       //����UI����İ������ͣ��ֳ�������ϻ򵥶�
    UCHAR ucKeyValueIsGot = 0;  //�Ƿ��Ѽ�⵽��ֵ
    
    while(1)
    {
        MyDelay(10);  //��ʱ10ms
        if(ucScanBeginFlag != 0)
        {
            if(ucScanBeginFlag == 2)  //��==2��˵���а����ձ�����
            {
                //�˴��ж��Ƿ��ж��������ɨ�赽,�������ͬʱ���µĻ���ֱ�ӷ�����ϼ�ֵ
                ucTempNum = CheckKeyNumbers(usFirstKeyValue);
                if(ucTempNum > 1)
                {
                    ucKeyValueIsGot = 1;
                    usKeyValueToUI = usFirstKeyValue; //���ص�ǰ��ֵusFirstKeyValue
                    ucKeyTypeToUI = KEY_STATUS_COMPOUNDING; //���ص�ǰ����״̬Ϊ����ϡ�
                    MyDelay(100);
                    ucScanBeginFlag = 0;  //����ɨ�迪ʼ��־����
                }
                else if(ucTempNum == 0)
                {
                    ucScanBeginFlag = 0;  //����ɨ�迪ʼ��־����
                }
                else
                {
                    ucScanBeginFlag = 1;  //����ɨ�迪ʼ��־��1
                    ucScanTimes = 1;  //ɨ�������1
                }
            }

            MyDelay(10);  //��ʱ10ms��������

            aucKeyValue[0] = *(KEY_BASE + 0xfd);  //����ɨ�谴��״̬
            aucKeyValue[1] = *(KEY_BASE + 0xfb);
            aucKeyValue[2] = *(KEY_BASE + 0xf7);
            aucKeyValue[3] = *(KEY_BASE + 0xef);

            usKeyNewValue = CountKeyValue(aucKeyValue);  //����ɨ���������㵱ǰ��ֵ

            ucScanTimes++;  //ɨ�������1

            if(usKeyNewValue != usFirstKeyValue)
            {
                if(usKeyNewValue == 0)
                {
                    if(ucScanTimes >= 3)  //��Ϊ������������������ºͷſ�
                    {
                        ucKeyValueIsGot = 1;
                        usKeyValueToUI = usFirstKeyValue; //���ص�ǰ��ֵusFirstKeyValue
                        ucKeyTypeToUI = KEY_STATUS_SINGLE; //���ص�ǰ����״̬Ϊ��������
                        MyDelay(100);
                        ucScanBeginFlag = 0;  //����ɨ�迪ʼ��־����
                    }
                    else  //�����ǰ���ε�ɨ���з���û�а��������£�˵��Ϊ��������������ɨ��
                    {
                        ucScanBeginFlag = 0;  //����ɨ�迪ʼ��־����
                    }
                }
                else if((usKeyNewValue | usFirstKeyValue) == usKeyNewValue)
                {  //�ж�Ϊ��ϼ�
                    ucKeyValueIsGot = 1;
                    usKeyValueToUI = usKeyNewValue; //���ص�ǰ��ֵusKeyNewValue
                    ucKeyTypeToUI = KEY_STATUS_COMPOUNDING; //���ص�ǰ����״̬Ϊ����ϡ�
                    MyDelay(100);
                    ucScanBeginFlag = 0;  //����ɨ�迪ʼ��־����
                }
                else
                {
                    usFirstKeyValue = usKeyNewValue;
                    ucScanBeginFlag = 2;  //���¼����ɨ������
                }
            }
            else
            {
                if(ucScanTimes >= 150)
                {
                    ucKeyValueIsGot = 1;
                    usKeyValueToUI = usFirstKeyValue; //���ص�ǰ��ֵusFirstKeyValue
                    ucKeyTypeToUI = KEY_STATUS_LONGPRESS; //���ص�ǰ����״̬Ϊ��������
                    MyDelay(100);
                    ucScanBeginFlag = 0;  //����ɨ�迪ʼ��־����
                }
            }

            //����ѵõ���ֵ������Ϣ��UI����
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
                    continue; //û�а취
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
