/*======================================================================*/
//功能：	内部串口中断服务程序头文件
//编写时间：2004年4月13日－－2004年4月14日
//编写人：	han
//版本号：	1.1.0
//测试人：	han
//测试结果：OK
/*======================================================================*/

#ifndef USARTH
#define USARTH
#include "UserType.h"
#include "At91M55800.h"
#include "MyBoard.h"
#include "../pio/Pio.h"
#include "../aic/Aic.h"


#ifdef __cplusplus
extern "C" {
#endif

#define TERMINAL_ERROR_RX_OVF   0x1
#define TERMINAL_ERROR_TX_OVF   0x2

#define USART_SIZE_BUFFER     0x1300

/*---------------------------------------*/
/* 内部串口硬件描述 */
/*---------------------------------------*/
typedef struct
{
    USART_REG_ST     *pstUsartBase ;   /* Peripheral base */
    const PIO_DEV_ST *pstPioDev ;     /* IO controller descriptor */
    UCHAR                  ucPinRxd ;       /* RXD pin number in the PIO */
    UCHAR                  ucPinTxd ;       /* TXD pin number in the PIO */
    UCHAR                  ucPinSck ;       /* SCK pin number in the PIO */
    UCHAR                  ucPeriphId ;     /* USART Peripheral Identifier */
} USART_HARDWARE_ST ;

/*------------------------------------------*/
/* 内部串口数据缓冲区结构描述 */
/*------------------------------------------*/
typedef struct
{
    UCHAR          aucRxBuf[USART_SIZE_BUFFER] ;//接收缓冲
    UCHAR          aucTxBuf[USART_SIZE_BUFFER] ;//发送缓冲
    UCHAR          *pucRxIn ;//接收缓冲写入指针
    UCHAR          *pucRxOut ;//接收缓冲读出指针
    UINT32         uiRxCnt;//接收缓冲存放数据大小
    UCHAR          *pucTxIn ;//发送缓冲写入指针
    UCHAR          *pucTxOut ;//发送缓冲读出指针
    UINT32         uiTxCnt ;//发送缓冲存放数据大小
    UINT32         uiError ;//缓冲错误标识
} TERMINAL_DATA_ST ;
/*------------------------------------------*/
/* 内部串口结构描述 */
/*------------------------------------------*/
typedef struct
{
    const USART_HARDWARE_ST         *pUsartHardware ;
    TERMINAL_DATA_ST  *pstData ;
    UINT32                   uiBaudRate ;
    UINT32                   uiFormat ;
    AIC_TYPE_HANDLE          *pUsartAsmHandler ;//中断处理程序指针
    UINT32 uiU485Enable;    //485使能脚
} USART_DEV_ST ;


extern USART_DEV_ST g_stUsart0;
extern USART_DEV_ST g_stUsart1;
extern USART_DEV_ST g_stUsart2;

/*	外部调用的函数	*/
void Usart0CHandler  ( void ) ;
void Usart1CHandler  ( void ) ;
void Usart2CHandler  ( void ) ;
void UsartOpen  ( USART_DEV_ST *pstUsartDev ) ;
void UsartClose ( USART_DEV_ST *pstUsartDev ) ;
void UsartWrite ( USART_DEV_ST *pstUsartDev, UCHAR *pucData, UINT32 uiDataSize ) ;
UINT32 UsartRead  ( USART_DEV_ST *pstUsartDev, UCHAR *pucData ) ;
UINT32 UsartReadReady ( USART_DEV_ST *pstUsartDev );
UINT32 UsartSendReady ( USART_DEV_ST *pstUsartDev );

#ifdef __cplusplus
}
#endif

#endif
