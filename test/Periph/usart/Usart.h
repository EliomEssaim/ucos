/*======================================================================*/
//���ܣ�	�ڲ������жϷ������ͷ�ļ�
//��дʱ�䣺2004��4��13�գ���2004��4��14��
//��д�ˣ�	han
//�汾�ţ�	1.1.0
//�����ˣ�	han
//���Խ����OK
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
/* �ڲ�����Ӳ������ */
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
/* �ڲ��������ݻ������ṹ���� */
/*------------------------------------------*/
typedef struct
{
    UCHAR          aucRxBuf[USART_SIZE_BUFFER] ;//���ջ���
    UCHAR          aucTxBuf[USART_SIZE_BUFFER] ;//���ͻ���
    UCHAR          *pucRxIn ;//���ջ���д��ָ��
    UCHAR          *pucRxOut ;//���ջ������ָ��
    UINT32         uiRxCnt;//���ջ��������ݴ�С
    UCHAR          *pucTxIn ;//���ͻ���д��ָ��
    UCHAR          *pucTxOut ;//���ͻ������ָ��
    UINT32         uiTxCnt ;//���ͻ��������ݴ�С
    UINT32         uiError ;//��������ʶ
} TERMINAL_DATA_ST ;
/*------------------------------------------*/
/* �ڲ����ڽṹ���� */
/*------------------------------------------*/
typedef struct
{
    const USART_HARDWARE_ST         *pUsartHardware ;
    TERMINAL_DATA_ST  *pstData ;
    UINT32                   uiBaudRate ;
    UINT32                   uiFormat ;
    AIC_TYPE_HANDLE          *pUsartAsmHandler ;//�жϴ������ָ��
    UINT32 uiU485Enable;    //485ʹ�ܽ�
} USART_DEV_ST ;


extern USART_DEV_ST g_stUsart0;
extern USART_DEV_ST g_stUsart1;
extern USART_DEV_ST g_stUsart2;

/*	�ⲿ���õĺ���	*/
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
