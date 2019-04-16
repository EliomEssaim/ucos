/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   Key.c
    ����:     ����
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  
       ����:  ����
       ����:  �����д���
---------------------------------------------------------------------------*/
#include "../../include/UserType.h"
#include "../../include/at91m55800.h"
#include "../../include/MyBoard.h"
#include "../../periph/pio/pio.h"
#include "../../periph/usart/Usart.h"
#include "key.h"


//����ɨ�����
UCHAR ucScanBeginFlag;  //�Ƿ���Ҫ��ʼ�жϰ���״̬�ı�־����key.c�ø�
USHORT usFirstKeyValue;  //���а���������ʱ�����������״̬

//********************************************
//               �ܽ��������               **
//********************************************

//�ⲿ�ж�IRQ1�ļ�ֵɨ��Ӳ������
EXT_IRQ_DEV_ST g_stKeyScan = 
{   
    &g_stPioA,
    IRQ1_ID,
    10
};

//********************************************
//                 ��������                 **
//********************************************
extern void KeyScanHandler(void);

//********************************************
//                 ����ʵ��                 **
//********************************************
/*************************************************
  Function:      CountKeyValue
  Description:   ���ݰ���ɨ�������������ǰ��ֵ
  Called By:     
  Input:         aucKeyScan[4] - ɨ�赽�İ���״̬
  Return:        ��ǰ��ֵ
*************************************************/
USHORT CountKeyValue(UCHAR* aucKeyScan)
{
    USHORT usValue = 0;

    aucKeyScan[0] &= KEY_MASK;
    aucKeyScan[1] &= KEY_MASK;
    aucKeyScan[2] &= KEY_MASK;
    aucKeyScan[3] &= KEY_MASK;

    if( (aucKeyScan[0] & 0x01) == 0 )
        usValue |= 0x01;
    if( (aucKeyScan[0] & 0x02) == 0 )
        usValue |= 0x02;
    if( (aucKeyScan[0] & 0x04) == 0 )
        usValue |= 0x04;
    if( (aucKeyScan[0] & 0x08) == 0 )
        usValue |= 0x08;

    if( (aucKeyScan[1] & 0x01) == 0 )
        usValue |= 0x10;
    if( (aucKeyScan[1] & 0x02) == 0 )
        usValue |= 0x20;
    if( (aucKeyScan[1] & 0x04) == 0 )
        usValue |= 0x40;
    if( (aucKeyScan[1] & 0x08) == 0 )
        usValue |= 0x80;

    if( (aucKeyScan[2] & 0x01) == 0 )
        usValue |= 0x0100;
    if( (aucKeyScan[2] & 0x02) == 0 )
        usValue |= 0x0200;
    if( (aucKeyScan[2] & 0x04) == 0 )
        usValue |= 0x0400;
    if( (aucKeyScan[2] & 0x08) == 0 )
        usValue |= 0x0800;

    if( (aucKeyScan[3] & 0x01) == 0 )
        usValue |= 0x1000;
    if( (aucKeyScan[3] & 0x02) == 0 )
        usValue |= 0x2000;
    if( (aucKeyScan[3] & 0x04) == 0 )
        usValue |= 0x4000;
    if( (aucKeyScan[3] & 0x08) == 0 )
        usValue |= 0x8000;

    return usValue;
}

//======================================================================
//���ܣ�    ����ɨ��ĳ�ʼ��
//����޸�ʱ�䣺2006��6��29��
//======================================================================
void KeyScanInit(void)
{
    ExtIrqOpen(&g_stKeyScan, 7, AIC_SRCTYPE_EXT_NEGATIVE_EDGE, (AIC_TYPE_HANDLE *)KeyScanHandler);
}

//======================================================================
//���ܣ�    ����ɨ����жϺ���
//����޸�ʱ�䣺2006��6��29��
//��ע��  
//======================================================================
void KeyScanInterrupt(void)
{
    UCHAR ucTempKeyValue[5];
    
    AIC_ICCR = 0x10000000;
    if(ucScanBeginFlag == 0)
    {
        ucTempKeyValue[0] = *(KEY_BASE + 0xfd);
        ucTempKeyValue[1] = *(KEY_BASE + 0xfb);
        ucTempKeyValue[2] = *(KEY_BASE + 0xf7);
        ucTempKeyValue[3] = *(KEY_BASE + 0xef);

        usFirstKeyValue = CountKeyValue(ucTempKeyValue);
        
        ucScanBeginFlag = 2;  //�����ɨ������
    }
    //}
}
