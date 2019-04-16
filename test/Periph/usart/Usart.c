/*======================================================================*/
//���ܣ�	�ڲ������жϷ������
//��дʱ�䣺2004��4��13�գ���2004��4��14��
//��д�ˣ�	han
//�汾�ţ�	1.1.0
//�����ˣ�	han
//���Խ����OK
/*======================================================================*/

/*======================================================================*/
//���ܣ�	�ڲ�����2��GSMRʹ��
//��дʱ�䣺2004��4��16��
//��д�ˣ�	han
//�汾�ţ�	1.2.0
//�����ˣ�	han
//���Խ����OK
/*======================================================================*/

/*======================================================================*/
//���ܣ�	�ڲ�����0��1��MMIͨѶʹ��
//��дʱ�䣺2004��4��18��
//��д�ˣ�	han
//�汾�ţ�	1.3.0
//�����ˣ�	han
//���Խ����
/*======================================================================*/

#include "./Usart.h"

extern void Usart0AsmIrqhandler(void);
extern void Usart1AsmIrqhandler(void);
extern void Usart2AsmIrqhandler(void);

//���ڽ��շ��ͻ���������
static TERMINAL_DATA_ST stUsart0Data;
static TERMINAL_DATA_ST stUsart1Data;
static TERMINAL_DATA_ST stUsart2Data;

/* Usart 0 Hardware Descriptor */
const USART_HARDWARE_ST stUsart0Hardware =
{
    USART0_BASE,
    &g_stPioA,
    PIORXD0,
    PIOTXD0,
    PIOSCK0,
    US0_ID ,
} ;

/* Usart 1 Hardware Descriptor */
const USART_HARDWARE_ST stUsart1Hardware =
{
    USART1_BASE ,
    &g_stPioA,
    PIORXD1,
    PIOTXD1,
    PIOSCK1,
    US1_ID ,
} ;

/* Usart 2 Hardware Descriptor */
const USART_HARDWARE_ST stUsart2Hardware =
{
    USART2_BASE ,
    &g_stPioA,
    PIORXD2,
    PIOTXD2,
    PIOSCK2,
    US2_ID ,
} ;

//����0  �豸����
USART_DEV_ST g_stUsart0 = 
{
    &stUsart0Hardware,
    &stUsart0Data,
    9600,
    US_ASYNC_MODE,
    Usart0AsmIrqhandler,
    NULL//��485ʹ��
};

//����1  �豸����
USART_DEV_ST g_stUsart1 = 
{
    &stUsart0Hardware,
    &stUsart1Data,
    9600,
    US_ASYNC_MODE,
    Usart1AsmIrqhandler,
    PA17
};

//����2  �豸����
USART_DEV_ST g_stUsart2 =
{
    &stUsart2Hardware,
    &stUsart2Data,
    9600,
    US_ASYNC_MODE,
    Usart2AsmIrqhandler,
    PA14
};

/*======================================================================*/
//���ܣ��ڲ�����0�жϷ����ӳ���
//������pstUsartDev�ڲ����ڱ�ţ�������Int_Uasr0��2
//���أ�void
//��дʱ�䣺2004��4��14��
//���Խ����
/*======================================================================*/
void Usart0CHandler  ( void )  
{
    UINT32   uiStatus ;
    UINT32   uiTemp ;
    USART_DEV_ST *pstUsartDev;
    
    pstUsartDev = &g_stUsart0;

    USART_REG_ST         *pstUsartReg = pstUsartDev->pUsartHardware->pstUsartBase;
    TERMINAL_DATA_ST    *pstData = pstUsartDev->pstData;

    while (( uiStatus = ( pstUsartReg->US_CSR &  pstUsartReg->US_IMR )) != 0 )
    {
        //* If a reception error occured        
        if (( uiStatus & US_MASK_IRQ_ERROR ) != 0 )
        {
            uiStatus &= ~US_RXRDY ;
            pstUsartReg->US_CR = US_RSTSTA ;
            uiTemp = pstUsartReg->US_RHR ;
            //���������½���
            pstData->pucRxIn= pstData->aucRxBuf ;
            pstData->pucRxOut = pstData->aucRxBuf;
            pstData->uiRxCnt = 0;
            //return;
        }
        

        //* RXRDY interrupt
        if (( uiStatus & US_RXRDY ) != 0 )
        {
            if ((UINT32)pstData->uiRxCnt >= USART_SIZE_BUFFER)
            {
                pstData->uiError |= TERMINAL_ERROR_RX_OVF ;

                uiStatus &= ~US_RXRDY ;
                pstUsartReg->US_CR = US_RSTSTA ;
                uiTemp = pstUsartReg->US_RHR ;
                //���������½���
                pstData->pucRxIn= pstData->aucRxBuf ;
                pstData->pucRxOut = pstData->aucRxBuf;
                pstData->uiRxCnt = 0;
                //return;
            }
            else
            {
                pstData->uiRxCnt++;

                //* reception
                *(pstData->pucRxIn) = pstUsartReg->US_RHR ;
                pstData->pucRxIn++;
                if ((UINT32)pstData->pucRxIn>= (UINT32)pstData->aucRxBuf+ USART_SIZE_BUFFER)
                {
                    pstData->pucRxIn= pstData->aucRxBuf ;
                }
            }
            //test
            /*
            if(((UINT32)pstData->pucRxOut) > ((UINT32)pstData->pucRxIn))
            {
                pstData->uiRxCnt = ((UINT32)pstData->pucRxIn) + USART_SIZE_BUFFER 
                        - ((UINT32)pstData->pucRxOut);            
            }
            else
            {
                pstData->uiRxCnt = ((UINT32)pstData->pucRxIn) - ((UINT32)pstData->pucRxOut);
            }
            */
        }

        //* TXRDY interrupt
        if (( uiStatus & US_TXRDY ) != 0 )
        {
            if ((UINT32)pstData->uiTxCnt> 0)
            {
                pstData->uiTxCnt--;

                //* transmission
                pstUsartReg->US_THR = *(pstData->pucTxOut)++ ;
                if ((UINT32)pstData->pucTxOut >= (UINT32)pstData->aucTxBuf+ USART_SIZE_BUFFER)
                {
                    pstData->pucTxOut= pstData->aucTxBuf;
                }
            }
            else
            {
                //* reset tx pointer
                pstData->pucTxIn= pstData->aucTxBuf;
                pstData->pucTxOut= pstData->aucTxBuf;
                pstData->uiTxCnt= 0;

                //* Disable the TXRDY interrupt
                pstUsartReg->US_IDR = US_TXRDY;
            }
        }
    }
}


/*======================================================================*/
//���ܣ��ڲ�����1�жϷ����ӳ���
//������pstUsartDev�ڲ����ڱ�ţ�������Int_Uasr0��2
//���أ�void
//��дʱ�䣺2004��4��14��
//���Խ����
/*======================================================================*/
void Usart1CHandler  ( void )  
{
    UINT32   uiStatus ;
    UINT32   uiTemp ;
    USART_DEV_ST *pstUsartDev;
    
    pstUsartDev = &g_stUsart1;

    USART_REG_ST         *pstUsartReg = pstUsartDev->pUsartHardware->pstUsartBase;
    TERMINAL_DATA_ST    *pstData = pstUsartDev->pstData;

    while (( uiStatus = ( pstUsartReg->US_CSR &  pstUsartReg->US_IMR )) != 0 )
    {
        //* If a reception error occured
        if (( uiStatus & US_MASK_IRQ_ERROR ) != 0 )
        {
            uiStatus &= ~US_RXRDY ;
            pstUsartReg->US_CR = US_RSTSTA ;
            uiTemp = pstUsartReg->US_RHR ;
            //���������½���
            pstData->pucRxIn= pstData->aucRxBuf ;
            pstData->pucRxOut = pstData->aucRxBuf;
            pstData->uiRxCnt = 0;
            //return;
        }

        //* RXRDY interrupt
        if (( uiStatus & US_RXRDY ) != 0 )
        {
            if ((UINT32)pstData->uiRxCnt >= USART_SIZE_BUFFER)
            {
                pstData->uiError |= TERMINAL_ERROR_RX_OVF ;

                uiStatus &= ~US_RXRDY ;
                pstUsartReg->US_CR = US_RSTSTA ;
                uiTemp = pstUsartReg->US_RHR ;
                //���������½���
                pstData->pucRxIn= pstData->aucRxBuf ;
                pstData->pucRxOut = pstData->aucRxBuf;
                pstData->uiRxCnt = 0;
                //return;
            }
            else
            {
                pstData->uiRxCnt++;

                //* reception
                *(pstData->pucRxIn) = pstUsartReg->US_RHR ;
                pstData->pucRxIn++;
                if ((UINT32)pstData->pucRxIn>= (UINT32)pstData->aucRxBuf+ USART_SIZE_BUFFER)
                {
                    pstData->pucRxIn= pstData->aucRxBuf ;
                }
            }
        }

        //* TXRDY interrupt
        if (( uiStatus & US_TXRDY ) != 0 )
        {
            if ((UINT32)pstData->uiTxCnt> 0)
            {
                pstData->uiTxCnt--;

                //* transmission
                pstUsartReg->US_THR = *(pstData->pucTxOut)++ ;
                if ((UINT32)pstData->pucTxOut >= (UINT32)pstData->aucTxBuf+ USART_SIZE_BUFFER)
                {
                    pstData->pucTxOut= pstData->aucTxBuf;
                }
            }
            else
            {
                //* reset tx pointer
                pstData->pucTxIn= pstData->aucTxBuf;
                pstData->pucTxOut= pstData->aucTxBuf;
                pstData->uiTxCnt= 0;

                //* Disable the TXRDY interrupt
                pstUsartReg->US_IDR = US_TXRDY;
            }
        }
    }
}

/*======================================================================*/
//���ܣ�     �жϷ����ӳ���
//������	pstUsartDev	�ڲ����ڱ�ţ�������Int_Uasr0��2
//���أ�	void
//��дʱ�䣺2004��4��14��
//���Խ����
/*======================================================================*/
void Usart2CHandler  ( void )  
{
    UINT32   uiStatus ;
    UINT32   uiTemp ;
    USART_DEV_ST *pstUsartDev;
    
    pstUsartDev = &g_stUsart2;

    USART_REG_ST         *pstUsartReg = pstUsartDev->pUsartHardware->pstUsartBase;
    TERMINAL_DATA_ST    *pstData = pstUsartDev->pstData;

    while (( uiStatus = ( pstUsartReg->US_CSR &  pstUsartReg->US_IMR )) != 0 )
    {
        //* If a reception error occured
        if (( uiStatus & US_MASK_IRQ_ERROR ) != 0 )
        {
            uiStatus &= ~US_RXRDY ;
            pstUsartReg->US_CR = US_RSTSTA ;
            uiTemp = pstUsartReg->US_RHR ;
            //���������½���
            pstData->pucRxIn= pstData->aucRxBuf ;
            pstData->pucRxOut = pstData->aucRxBuf;
            pstData->uiRxCnt = 0;
            //return;
        }

        //* RXRDY interrupt
        if (( uiStatus & US_RXRDY ) != 0 )
        {
            if ((UINT32)pstData->uiRxCnt >= USART_SIZE_BUFFER)
            {
                pstData->uiError |= TERMINAL_ERROR_RX_OVF ;

                uiStatus &= ~US_RXRDY ;
                pstUsartReg->US_CR = US_RSTSTA ;
                uiTemp = pstUsartReg->US_RHR ;
                //���������½���
                pstData->pucRxIn= pstData->aucRxBuf ;
                pstData->pucRxOut = pstData->aucRxBuf;
                pstData->uiRxCnt = 0;
                //return;
            }
            else
            {
                pstData->uiRxCnt++;

                //* reception
                *(pstData->pucRxIn) = pstUsartReg->US_RHR ;
                pstData->pucRxIn++;
                if ((UINT32)pstData->pucRxIn>= (UINT32)pstData->aucRxBuf+ USART_SIZE_BUFFER)
                {
                    pstData->pucRxIn= pstData->aucRxBuf ;
                }
            }
        }

        //* TXRDY interrupt
        if (( uiStatus & US_TXRDY ) != 0 )
        {
            if ((UINT32)pstData->uiTxCnt> 0)
            {
                pstData->uiTxCnt--;

                //* transmission
                pstUsartReg->US_THR = *(pstData->pucTxOut)++ ;
                if ((UINT32)pstData->pucTxOut >= (UINT32)pstData->aucTxBuf+ USART_SIZE_BUFFER)
                {
                    pstData->pucTxOut= pstData->aucTxBuf ;
                }
            }
            else
            {
                //* reset tx pointer
                pstData->pucTxIn= pstData->aucTxBuf;
                pstData->pucTxOut= pstData->aucTxBuf;
                pstData->uiTxCnt= 0;

                //* Disable the TXRDY interrupt
                pstUsartReg->US_IDR = US_TXRDY;
            }
        }
    }
}


/*======================================================================*/
//���ܣ����ڲ�����
//������term_desc�ڲ����ڱ�ţ�������Int_Uasr0��2
//���أ�void
//��дʱ�䣺2004��4��14��
//���Խ����
/*======================================================================*/
void UsartOpen(USART_DEV_ST * pstUsartDev) 
{

    //* Write the Peripheral Clock Enable Register
    APMC_PCER = (1 << (pstUsartDev->pUsartHardware->ucPeriphId) ) ;

    //* If External clock used
    if ((pstUsartDev->uiFormat & SCK_USED ) != 0 )
    {
        //* Define RXD, TXD and SCK as peripheral
        PioClose ( pstUsartDev->pUsartHardware->pstPioDev,
                         (1 << pstUsartDev->pUsartHardware->ucPinTxd) |
                         (1 << pstUsartDev->pUsartHardware->ucPinRxd) |
                         (1 << pstUsartDev->pUsartHardware->ucPinSck) ) ;
    }
    //* Else
    else
    {
        //* Define RXD and TXD as peripheral
        PioClose ( pstUsartDev->pUsartHardware->pstPioDev,
                         (1 << pstUsartDev->pUsartHardware->ucPinTxd) |
                         (1 << pstUsartDev->pUsartHardware->ucPinRxd) ) ;
    //* EndIf
    }



    //* Reset receiver and transmitter
    pstUsartDev->pUsartHardware->pstUsartBase->US_CR = US_RSTRX | US_RSTTX | US_RXDIS | US_TXDIS ;

    //* Clear Transmit and Receive Counters
    pstUsartDev->pUsartHardware->pstUsartBase->US_RCR = 0 ;
    pstUsartDev->pUsartHardware->pstUsartBase->US_TCR = 0 ;

    //* Define the baud rate divisor register
    UINT32 uiSpeed;
    uiSpeed = (UINT32)( MCK/16/(pstUsartDev->uiBaudRate) );
    pstUsartDev->pUsartHardware->pstUsartBase->US_BRGR = uiSpeed;

    //* Define the USART mode
    pstUsartDev->pUsartHardware->pstUsartBase->US_MR = pstUsartDev->uiFormat;

    //* Write the Timeguard Register
    pstUsartDev->pUsartHardware->pstUsartBase->US_TTGR = 0 ;

    //* Enable receiver and transmitter
    pstUsartDev->pUsartHardware->pstUsartBase->US_CR = US_RXEN | US_TXEN ;

    pstUsartDev->pUsartHardware->pstUsartBase->US_IER = US_RXRDY;
    pstUsartDev->pUsartHardware->pstUsartBase->US_IMR = US_RXRDY| US_TXRDY;

    pstUsartDev->pstData->pucTxIn= pstUsartDev->pstData->aucTxBuf;
    pstUsartDev->pstData->pucTxOut= pstUsartDev->pstData->aucTxBuf ;
    pstUsartDev->pstData->pucRxIn= pstUsartDev->pstData->aucRxBuf;
    pstUsartDev->pstData->pucRxOut= pstUsartDev->pstData->aucRxBuf;
    pstUsartDev->pstData->uiRxCnt= 0 ;
    pstUsartDev->pstData->uiTxCnt= 0 ;
    pstUsartDev->pstData->uiError= 0 ;


    /*�����жϷ������ʹ��*/
     IrqOpen(pstUsartDev->pUsartHardware->ucPeriphId,3,AIC_SRCTYPE_EXT_HIGH_LEVEL,pstUsartDev->pUsartAsmHandler);

}




/*======================================================================*/
//���ܣ��ر��ڲ�����
//������term_desc�ڲ����ڱ�ţ�������Int_Uasr0��2
//���أ�void
//��дʱ�䣺2004��4��14��
//���Խ����
/*======================================================================*/
void UsartClose(USART_DEV_ST * pstUsartDev)
{
    //* Write the Peripheral Clock Disable Register
    APMC_PCDR = (1 << pstUsartDev->pUsartHardware->ucPeriphId) ;

    //* Define all USARTs pins as pio
    PioOpen ( pstUsartDev->pUsartHardware->pstPioDev,
                    (1 << pstUsartDev->pUsartHardware->ucPinTxd) |
                    (1 << pstUsartDev->pUsartHardware->ucPinRxd) |
                    (1 << pstUsartDev->pUsartHardware->ucPinSck),
                    PIO_RESET_CONF ) ;

    //* Disable all interrupts
    pstUsartDev->pUsartHardware->pstUsartBase->US_IDR = 0xFFFFFFFF ;

    //* Abort the Peripheral Data Transfers
    pstUsartDev->pUsartHardware->pstUsartBase->US_RCR = 0 ;
    pstUsartDev->pUsartHardware->pstUsartBase->US_TCR = 0 ;

    //* Disable receiver and transmitter and stop any activity immediately
    pstUsartDev->pUsartHardware->pstUsartBase->US_CR = US_TXDIS | US_RXDIS | US_RSTTX | US_RSTRX ;		

    IrqClose(pstUsartDev->pUsartHardware->ucPeriphId);
}



/*======================================================================*/
//���ܣ��ڲ����ڷ��ͺ���
//������term_desc�ڲ����ڱ�ţ�������Int_Uasr0��2
//character�������ݻ�������ַ
//charsize���ݴ�С
//���أ�void
//��дʱ�䣺2004��4��14��
//���Խ����
/*======================================================================*/
void UsartWrite(USART_DEV_ST * pstUsartDev, UCHAR * pucData, UINT32 uiDataSize) 
{
    UINT32 i;
    TERMINAL_DATA_ST   *pstData=pstUsartDev->pstData;
    for(i = 0; i < uiDataSize; i++)
    {
        if (pstData->uiTxCnt< USART_SIZE_BUFFER)
        {
            pstData->uiTxCnt++ ;
            if((UINT32)pstData->pucTxIn>= (UINT32)pstData->aucTxBuf+ USART_SIZE_BUFFER)
            {
                pstData->pucTxIn= pstData->aucTxBuf;
            }

            *(pstData->pucTxIn)++ = *(pucData + i) ;
        }
        else
        {
            pstData->uiError |= TERMINAL_ERROR_TX_OVF ;
            pstData->pucTxIn = pstData->aucTxBuf;
            pstData->pucTxOut = pstData->aucTxBuf;
            pstData->uiTxCnt= 0;
        }
    }
    if(uiDataSize)
    {
        pstUsartDev->pUsartHardware->pstUsartBase->US_IER = US_TXRDY;
    }
}




/*======================================================================*/
//���ܣ����ڲ����ڶ����ݺ���
//������term_desc�ڲ����ڱ�ţ�������Int_Uasr0��2
//character�������ݻ�������ַ
//���أ�u_int�յ������ݴ�С
//��дʱ�䣺2004��4��14��
//���Խ����
/*======================================================================*/
UINT32 UsartRead(USART_DEV_ST * pstUsartDev, UCHAR * pucData)
{

    TERMINAL_DATA_ST   *pstData=pstUsartDev->pstData ;
    UINT32 i,j;

    i = pstData->uiRxCnt;
    j = i;
    for(; i> 0; i--)
    {
        pstData->uiRxCnt--;
        *pucData++ = *(pstData->pucRxOut)++;
        if ((UINT32)pstData->pucRxOut >= (UINT32)pstData->aucRxBuf + USART_SIZE_BUFFER)
        pstData->pucRxOut= pstData->aucRxBuf;
    }

    return(j);
}


/*======================================================================*/
//���ܣ��ж��ڲ������Ƿ��յ�����
//������term_desc�ڲ����ڱ�ţ�������Int_Uasr0��2
//���أ�u_int		0��ʾû�У�1��ʾ��
//��дʱ�䣺2004��4��14��
//���Խ����
/*======================================================================*/
UINT32 UsartReadReady ( USART_DEV_ST *pstUsartDev )
{

    TERMINAL_DATA_ST    *pstData = pstUsartDev->pstData ;
    if(pstData->uiRxCnt> 0)
        return TRUE;
    else
        return FALSE;
}

/*======================================================================*/
//���ܣ��ж��ڲ����ڷ��ͻ����ѿ�
//������
//���أ�
//��дʱ�䣺2004��4��14��
//���Խ����
/*======================================================================*/
UINT32 UsartSendReady ( USART_DEV_ST *pstUsartDev )
{

    TERMINAL_DATA_ST    *pstData = pstUsartDev->pstData ;
    if(pstData->uiTxCnt== 0)
        return TRUE;
    else
        return FALSE;
}



