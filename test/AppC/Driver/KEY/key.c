/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   Key.c
    作者:     林玮
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  
       作者:  林玮
       描述:  代码编写完成
---------------------------------------------------------------------------*/
#include "../../include/UserType.h"
#include "../../include/at91m55800.h"
#include "../../include/MyBoard.h"
#include "../../periph/pio/pio.h"
#include "../../periph/usart/Usart.h"
#include "key.h"


//按键扫描控制
UCHAR ucScanBeginFlag;  //是否需要开始判断按键状态的标志，由key.c置高
USHORT usFirstKeyValue;  //当有按键被按下时，按键最初的状态

//********************************************
//               管脚连接情况               **
//********************************************

//外部中断IRQ1的键值扫描硬件定义
EXT_IRQ_DEV_ST g_stKeyScan = 
{   
    &g_stPioA,
    IRQ1_ID,
    10
};

//********************************************
//                 函数申明                 **
//********************************************
extern void KeyScanHandler(void);

//********************************************
//                 程序实现                 **
//********************************************
/*************************************************
  Function:      CountKeyValue
  Description:   根据按键扫描结果，计算出当前键值
  Called By:     
  Input:         aucKeyScan[4] - 扫描到的按键状态
  Return:        当前键值
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
//功能：    键盘扫描的初始化
//最后修改时间：2006年6月29日
//======================================================================
void KeyScanInit(void)
{
    ExtIrqOpen(&g_stKeyScan, 7, AIC_SRCTYPE_EXT_NEGATIVE_EDGE, (AIC_TYPE_HANDLE *)KeyScanHandler);
}

//======================================================================
//功能：    键盘扫描的中断函数
//最后修改时间：2006年6月29日
//备注：  
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
        
        ucScanBeginFlag = 2;  //激活按键扫描任务
    }
    //}
}
