//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : lib_pio.c
//* Object              : PIO functions library.
//*
//* 1.0 01/04/00 JCZ    : Creation
//* 1.1 21/02/02 PFi    : Ligne 66 -> PIO_MDSR changed to PIO_MDDR
//*----------------------------------------------------------------------------

#include    "./Pio.h"



/* PIO Controller A Descriptor */
const PIO_DEV_ST g_stPioA =
{
    PIOA_BASE,
    PIOA_ID,
    NB_PIOA
} ;

/* PIO Controller B Descriptor */
const PIO_DEV_ST g_stPioB =
{
    PIOB_BASE,
    PIOB_ID,
    NB_PIOB
} ;

//*----------------------------------------------------------------------------
//* Function Name       : at91_pio_open
//* Object              : Setup pins to be Parallel IOs, as managed by the PIO
//* Input Parameters    : <pstPio> = PIO Controller Descriptor
//*                     : <uiMask>   = bit uiMask identifying the PIOs
//*                     : <config> = uiMask identifying the PIOs configuration
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------
void  PioOpen  ( const PIO_DEV_ST *pstPio, UINT32 uiMask, UINT32 uiConfig ) 
//* Begin
{
    UINT32 uiTemp ;
    //* If PIOs required to be output
    if ((uiConfig & PIO_SENSE_BIT) != 0 )
    {
        //* Defines the PIOs as output
        pstPio->pstPioBase->PIO_OER = uiMask ;
    }
    //* Else
    else
    {
        //* Defines the PIOs as input
        pstPio->pstPioBase->PIO_ODR = uiMask ;
    }

    //* If PIOs required to be filtered
    if ((uiConfig & PIO_FILTER_BIT) != 0 )
    {
        //* Enable the filter on PIOs
        pstPio->pstPioBase->PIO_IFER = uiMask ;
    }
    else
    {
        //* Disable the filter on PIOs
        pstPio->pstPioBase->PIO_IFDR = uiMask ;
    }

    //* If PIOs required to be open-drain
    if ((uiConfig & PIO_OPENDRAIN_BIT) != 0 )
    {
        //* Enable the open drain mode on PIOs
        pstPio->pstPioBase->PIO_MDER = uiMask ;
    }
    else
    {
        //* Disable the open drain mode on PIOs
        pstPio->pstPioBase->PIO_MDDR = uiMask ;
    }

    //* If PIOs required for an input change interrupt
    if ((uiConfig & PIO_INPUT_IRQ_BIT) != 0 )
    {
        //* Remove any interrupt */
        uiTemp = pstPio->pstPioBase->PIO_ISR ;
        //* Enable the Input Change Interrupt on PIOs
        pstPio->pstPioBase->PIO_IER = uiMask ;
    }
    else
    {
        //* Disable the Input Change Interrupt on PIOs
        pstPio->pstPioBase->PIO_IDR = uiMask ;
    }

    //* Defines the pins to be controlled by PIO Controller
    pstPio->pstPioBase->PIO_PER = uiMask ;

//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_pio_close
//* Object              : Cancel PIO Controller handling from pins managed by
//*                       a peripheral
//* Input Parameters    : <pstPio> = PIO Descriptor pointer
//*                     : <uiMask>   = defines the pins to managed by peripheral
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------
void  PioClose ( const PIO_DEV_ST *pstPio, UINT32 uiMask ) 
//* Begin
{
    //* Define PIOs to be controlled by peripherals
    pstPio->pstPioBase->PIO_PDR = uiMask ;

//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_pio_write
//* Object              : Write a data on required PIOs
//* Input Parameters    : <pstPio> = PIO Controller Descriptor Address
//*                     : <uiMask>   = defines work pins
//*                     : <state>  = defines set ( =0) or clear ( #0)
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------
void  PioWrite ( const PIO_DEV_ST *pstPio, UINT32 uiMask, UINT32 uiState )
//* Begin
{
    if (uiState == PIO_CLEAR_OUT )
    {
        //* Clear PIOs with data at 0 in CODR (Clear Output Data Register)
        pstPio->pstPioBase->PIO_CODR = uiMask ;
    }
    else
    {
        //* Set PIOs with data at 1 in SODR (Set Output Data Register)
        pstPio->pstPioBase->PIO_SODR = uiMask ;
    }

//* End
}
//*----------------------------------------------------------------------------
//* Function Name       : at91_pio_read
//* Object              : Read the state of the PIO pins
//* Input Parameters    : <pstPio> = PIO Controller Descriptor Address
//* Output Parameters   : defines the pins value
//* Functions called    : at91_clock_get_status, at91_clock_open,
//*                       at91_clock_close
//*----------------------------------------------------------------------------
UINT32 PioRead(const PIO_DEV_ST * pstPio)

//* Begin
{

    UINT32   uiReturnVal ;
    UINT32   uiSaveClock;

    //* Get clock Status
    uiSaveClock = at91_clock_get_status ( pstPio->ucPeriphId) ;

    //* Enable the PIO Clock
    at91_clock_open ( pstPio->ucPeriphId ) ;

    //* Read the Data in input of the PIO Controller
    uiReturnVal = pstPio->pstPioBase->PIO_PDSR ;

    //* If PIO controller clock was disabled
    if (( uiSaveClock & (1 << pstPio->ucPeriphId)) == 0 )
    {
        //* Disable the PIO Clock
        at91_clock_close ( pstPio->ucPeriphId ) ;
    }

    return (uiReturnVal);

//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_pio_set_mode
//* Object              : Modify the mode of PIOs
//* Input Parameters    : <pstPio> = PIO Controller Descriptor
//*                     : <uiMask>   = bit uiMask identifying the PIOs
//*                     : <mode> = the required PIOs configuration
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------
void  PioSetMode  ( const PIO_DEV_ST *pstPio, UINT32 uiMask, UINT32 uiMode ) 
//* Begin
{
    //* If PIOs required to be filtered
    if ((uiMode & PIO_FILTER_BIT) != 0 )
        //* Enable the filter on PIOs
        pstPio->pstPioBase->PIO_IFER = uiMask ;
    //* Else
    else
        //* Disable the filter on PIOs
        pstPio->pstPioBase->PIO_IFDR = uiMask ;

    //* If PIOs required to be open-drain
    if ((uiMode & PIO_OPENDRAIN_BIT) != 0 )
        //* Enable the filter on PIOs
        pstPio->pstPioBase->PIO_MDER = uiMask ;
    //* Else
    else
        //* Disable the filter on PIOs
        pstPio->pstPioBase->PIO_MDSR = uiMask ;
//* End
}
//*----------------------------------------------------------------------------
//* Function Name       : DataToPio
//* Object              : Send Data To PioA Or PioB
//* Input Parameters    : <pstPio> = PIO Controller Descriptor
//*                     : <DATA>   = The Data To Be Sent
//*                     : <INDEX>  = Available Bit In DATA
//* Output Parameters   : none
//* Functions called    : PioWrite
//*----------------------------------------------------------------------------
void DataToPio ( const PIO_DEV_ST *pstPio, UINT32 DATA, UINT32 INDEX) 
{
    UINT32 temp1;
    temp1 = (DATA & INDEX);
    PioWrite( pstPio, temp1, PIO_SET_OUT);
    temp1 = ((~(DATA)) & (INDEX));
    PioWrite( pstPio, temp1, PIO_CLEAR_OUT);
}
