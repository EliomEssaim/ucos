//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : lib_aic.c
//* Object              : Advanced Interrupt Controller Library.
//*
//* 1.0 01/04/00 JCZ    : Creation
//*----------------------------------------------------------------------------

#include    "./Aic.h"

//*----------------------------------------------------------------------------
//* Function Name       : at91_default_irq_handler
//* Object              : Default Interrupt Handler
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions called    : at91_error
//*----------------------------------------------------------------------------
void IrqDefaultHandler ( void )
//* Begin
{
    //* Run the Error function
    while(1);

//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_default_fiq_handler
//* Object              : Default FIQ Handler
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions called    : at91_error
//*----------------------------------------------------------------------------
void FiqDefaultHandler ( void )
//* Begin
{
    //* Run the Error function
    while(1) ;

//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_irq_open
//* Object              : Interrupt Handler Initialization
//* Input Parameters    : <uiIrqId>      = interrupt number to initialize
//*                     : <priority>    = priority to give to the interrupt
//*                     : <src_type>    = activation and sense of activation
//*                     : <handler_pt>  = address of the interrupt handler
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------
void IrqOpen ( UINT32 uiIrqId,UINT32 uiPriority,
                            UINT32 uiSrcType, AIC_TYPE_HANDLE pHandler )
//* Begin
{
    UINT32       uiMask ;
    uiMask = 0x1 << uiIrqId ;
    //* Disable the interrupt on the interrupt controller
    AIC_IDCR = uiMask ;
    //* Save the interrupt handler routine pointer and the interrupt priority
    AIC_BASE->AIC_SVR[uiIrqId] = (UINT32) pHandler ;
    //* Store the Source Mode Register
    AIC_BASE->AIC_SMR[uiIrqId] = uiSrcType | uiPriority  ;
    //* Clear the interrupt on the interrupt controller
    AIC_ICCR = uiMask ;
    //* Enable the interrupt on the interrupt controller
    AIC_IECR = uiMask ;
}
//* End

//*----------------------------------------------------------------------------
//* Function Name       : at91_irq_close
//* Object              : Disable Interrupt Source
//* Input Parameters    : <uiIrqId> = Interrupt Source Identifier
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------
void IrqClose ( UINT32 uiIrqId )
//* Begin
{
    UINT32       uiMask ;
    uiMask = 0x1 << uiIrqId ;
    //* Disable the interrupt on the interrupt controller
    AIC_IDCR = uiMask ;
    //* Clear the interrupt on the Interrupt Controller ( if one is pending )
    AIC_ICCR = uiMask ;
    //* If removing the FIQ
    if ( uiIrqId == 0 )
        //* Default FIQ vector to <no_handler_fiq>
        AIC_BASE->AIC_SVR[0] = (UINT32) IrqDefaultHandler ;
    //* Else
    else
        //* Default IRQ vector to <no_handler_irq>
        AIC_BASE->AIC_SVR[uiIrqId] = (UINT32) FiqDefaultHandler ;
    //* EndIf
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_irq_trig_cmd
//* Object              : Interrupt Controller Command Trigger
//* Input Parameters    : <uiIrqId> = Interrupt Identifier
//*                     : <uiMask>   = Interrupt Source uiMask Identifier
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------
void IrqTrigCmd ( UINT32 uiIrqId, UINT32 uiMask  )
//* Begin
{
    //* If uiMask is null
    if ( uiMask == 0 )
        //* Set the interrupt on the interrupt controller
        AIC_ISCR = (1 << uiIrqId) ;
    //* Else
    else
        //* Set the interrupt on the interrupt controller
        AIC_ISCR = uiMask ;
    //* EndIf
}
//* End

//*----------------------------------------------------------------------------
//* Function Name       : at91_irq_get_status
//* Object              : Get current interrupt source identifier
//* Input Parameters    : None
//* Output Parameters   : Source identifier of the current interrupt
//* Functions called    : none
//*----------------------------------------------------------------------------
UINT32 IrqGetStatus ( void )
//* Begin
{
    //* Return AIC_ISR
    return ( AIC_ISR ) ;
//* End
}


//*----------------------------------------------------------------------------
//* Function Name       : at91_extirq_open
//* Object              : External Interrupt Set up
//* Input Parameters    : <irq_pt>      = External Interrupt Descriptor pointer
//*                     : <priority>    = Interrupt source priority
//*                     : <src_type>    = activation and sense of activation
//*                     : <handler_pt>  = address of the interrupt handler
//* Output Parameters   : none
//* Functions called    : at91_irq_close, at91_pio_close, at91_irq_open
//*----------------------------------------------------------------------------
void ExtIrqOpen  ( const EXT_IRQ_DEV_ST *pstIrq,
                              UINT32 uiPriority,
                              UINT32 uiSrcType,
                              AIC_TYPE_HANDLE pHandler ) 
//* Begin
{
    //* description pin
    UINT32 uiMask = 0x1<<(pstIrq->uiPinIrq) ;

    //* Make sure the interrupt source is inactive
    IrqClose ( pstIrq->uiSourceId) ;

    //* Define the corresponding IRQ line controlled by peripheral
    PioClose ( pstIrq->pstPio, uiMask ) ;

    //* Initialize the Interrupt Source on the AIC
    IrqOpen ( pstIrq->uiSourceId,
                     uiPriority,
                     uiSrcType,
                     pHandler ) ;
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_extirq_close
//* Object              : Remove an external interrupt
//* Input Parameters    : <irq_pt> = External Interrupt Descriptor pointer
//* Output Parameters   : none
//* Functions called    : at91_irq_close, at91_pio_open
//*----------------------------------------------------------------------------
void ExtIrqClose ( EXT_IRQ_DEV_ST *pstIrq ) 
//* Begin
{
     UINT32 uiMask = 0x1<<(pstIrq->uiPinIrq) ;
    //* Remove the interrupt source from the AIC
    IrqClose ( pstIrq->uiSourceId) ;

    //* Define the corresponding IRQ line controlled by peripheral
    PioOpen ( pstIrq->pstPio, uiMask, PIO_RESET_CONF ) ;

//* End
}

