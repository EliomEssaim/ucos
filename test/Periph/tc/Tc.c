//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : lib_tc.c
//* Object              : Timer Counter Library.
//*
//* 1.0 01/04/00 JCZ    : Creation
//* 1.1 12/10/00 JPP    : cutting the lib_tc in two files
//*----------------------------------------------------------------------------

#include "./Tc.h"

/*------------------------*/
/* Timer Counter Channels */
/*------------------------*/

/* Timer Counter Channel 0 Descriptor */
const TC_DEV_ST g_stTc0 =
{
    &(TCB0_BASE->TC[0]),
    &g_stPioB,
    TC0_ID,
    PIOTIOA0,
    PIOTIOB0,
    PIOTCLK0
} ;

/* Timer Counter Channel 1 Descriptor */
const TC_DEV_ST g_stTc1 =
{
    &TCB0_BASE->TC[1],
    &g_stPioB,
    TC1_ID,
    PIOTIOA1,
    PIOTIOB1,
    PIOTCLK1
} ;

/* Timer Counter Channel 2 Descriptor */
const TC_DEV_ST g_stTc2 =
{
    &TCB0_BASE->TC[2],
    &g_stPioB,
    TC2_ID,
    PIOTIOA2,
    PIOTIOB2,
    PIOTCLK2
} ;

/* Timer Counter Channel 3 Descriptor */
const TC_DEV_ST g_stTc3 =
{
    &TCB1_BASE->TC[0],
    &g_stPioA,
    TC3_ID,
    PIOTIOA3,
    PIOTIOB3,
    PIOTCLK3
} ;

/* Timer Counter Channel 4 Descriptor */
const TC_DEV_ST g_stTc4 =
{
    &TCB1_BASE->TC[1],
    &g_stPioA,
    TC4_ID,
    PIOTIOA4,
    PIOTIOB4,
    PIOTCLK4
} ;

/* Timer Counter Channel 5 Descriptor */
const TC_DEV_ST g_stTc5 =
{
    &TCB1_BASE->TC[2],
    &g_stPioA,
    TC5_ID,
    PIOTIOA5,
    PIOTIOB5,
    PIOTCLK5
} ;

/* Timer Counter Block 0 Descriptor */
const TC_BLOCK_DEV_ST TCB0_DESC =
{
    &g_stTc0,
    &g_stTc1,
    &g_stTc2,
} ;

/* Timer Counter Block 1 Descriptor */
const TC_BLOCK_DEV_ST TCB1_DESC =
{
    &g_stTc3,
    &g_stTc4,
    &g_stTc5,
} ;

//*----------------------------------------------------------------------------
//* Function Name       : at91_tc_open
//* Object              : Initialize Timer Counter Channel and enable is clock
//* Input Parameters    : <pstTcDev> = TC Channel Descriptor Pointer
//*                       <mode> = Timer Counter Mode
//*                     : <tioa> = TIOA enabled as peripheral if non null
//*                     : <tiob> = TIOB enabled as peripheral if non null
//* Output Parameters   : None
//* Functions called    : at91_clock_open, at91_pio_close
//*----------------------------------------------------------------------------
void TcOpen(const TC_DEV_ST * pstTcDev, UINT32 uiMode, UINT32 uiEnaTioa, UINT32 uiEnaTiob)
//* Begin
{
    UINT32 uiPio = 0 ;
    //* Start the Clock of the Timer
    ApmcClockOpen( pstTcDev->ucPeriphId) ;

    uiPio = 1<<pstTcDev->ucPinTclk;
    //* Enable TIOA pin if requested
    if ( uiEnaTioa == TRUE )
    {
        uiPio |= 1<<pstTcDev->ucPinTioa;
    }
    //* Enable TIOB pin if requested
    if ( uiEnaTiob == TRUE )
    {
        uiPio |= 1<<pstTcDev->ucPinTiob;
    }
    PioClose(pstTcDev->pstPioDev, uiPio);

    //* Disable the clock and the interrupts
    pstTcDev->pstTcBase->TC_CCR = TC_CLKDIS ;
    pstTcDev->pstTcBase->TC_IDR = 0xFFFFFFFF ;
    uiPio = pstTcDev->pstTcBase->TC_SR ;

    //* Set the Mode of the Timer Counter
    pstTcDev->pstTcBase->TC_CMR = uiMode ;

    //* Enable the clock
    pstTcDev->pstTcBase->TC_CCR = TC_CLKEN ;
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_tc_close
//* Object              : Stop a Timer Counter Channel and disable is clock
//* Input Parameters    : <pstTcDev> = the channel number
//* Output Parameters   : None
//* Functions called    : at91_clock_close
//*----------------------------------------------------------------------------
void TcClose ( const TC_DEV_ST *pstTcDev )
//* Begin
{
    //* Disable the clock and interrupts
    pstTcDev->pstTcBase->TC_CCR = TC_CLKDIS ;
    pstTcDev->pstTcBase->TC_IDR = 0xFFFFFFFF ;

    //* Stop the Clock of the Timer
    ApmcClockClose( pstTcDev->ucPeriphId) ;
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_tc_get_status
//* Object              : Read the Status of a Timer Counter Channel
//* Input Parameters    : <pstTcDev> = the channel number
//* Output Parameters   : the status value
//* Functions called    : None
//*----------------------------------------------------------------------------
UINT32 TcGetStatus(const TC_DEV_ST * pstTcDev) 
//* Begin
{
    //* Return the value of the Status Register
    return ( pstTcDev->pstTcBase->TC_SR ) ;

//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_tc_trig_cmd
//* Object              : Generate a software trigger on a TC channel
//* Input Parameters    : <pstTcDev> = the channel number to trig
//* Output Parameters   : None
//* Functions called    : at91_error
//*----------------------------------------------------------------------------
void TcTrigCmd(const TC_DEV_ST * pstTcDev, UINT32 uiCmd) 
//* Begin
{
    //* Depending on the command
    switch (uiCmd)
    {
        //* Case Channel Trigger
        case TC_TRIG_CHANNEL:
            //* Perform a Software trigger on the corresponding channel
            pstTcDev->pstTcBase->TC_CCR = TC_SWTRG ;
            break ;

        //* Case Synchronization Trigger
        case TC_TRIG_BLOCK:
            //* Perform a synchronization trigger
            ((TC_BLOCK_ST*) ((UINT32)pstTcDev->pstTcBase & 0xF0))->TC_BCR = TC_SYNC ;
            break ;
    }
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_tc_set_mode
//* Object              : Update Timer Counter Mode Register with mask
//* Input Parameters    : <pstTcDev> = the channel number
//*                     : <mask> = bit to modify in the mode register
//*                     : <data> = set/clear bits in the mode register
//* Output Parameters   : none
//* Functions called    : none
//*----------------------------------------------------------------------------
void TcSetMode(const TC_DEV_ST * pstTcDev, UINT32 uiMask, UINT32 uiData) 
//* Begin
{
    //* If data is not null
    if (uiData != 0)
        //* Set bits in the Mode Register corresponding to the mask
        pstTcDev->pstTcBase->TC_CMR |= uiMask ;
    //* Else
    else
        //* Clear bits in the Mode Register corresponding to the mask
        pstTcDev->pstTcBase->TC_CMR &= ~uiMask ;
    //* EndIf
}
//* End

//*----------------------------------------------------------------------------
//* Function Name       : at91_tc_read
//* Object              : Read all Timer Counter Register
//* Input Parameters    : <pstTcDev> = Channel Descriptor Pointer
//*                     : <reg> = Destination Register Value Table Pointer
//* Output Parameters   : None
//* Functions called    : None
//*----------------------------------------------------------------------------
void TcRead(const TC_DEV_ST * pstTcDev, UINT32 auiReg [ ]) 
//* Begin
{
    auiReg[RA] = pstTcDev->pstTcBase->TC_RA ;
    auiReg[RB] = pstTcDev->pstTcBase->TC_RB ;
    auiReg[RC] = pstTcDev->pstTcBase->TC_RC ;
    auiReg[CV] = pstTcDev->pstTcBase->TC_CV ;
}
//* End

//*----------------------------------------------------------------------------
//* Function Name       : at91_tc_write
//* Object              : Write Timer Counter Register
//* Input Parameters    : <pstTcDev> = Timer Counter Channel Descriptor Pointer
//*                     : <reg> = Source Register Value Table Pointer
//* Output Parameters   : None
//* Functions called    : None
//*----------------------------------------------------------------------------
void TcWrite(const TC_DEV_ST * pstTcDev, UINT32 auiReg [ ]) 
//* Begin
{
    //* First Value -> Register A
    pstTcDev->pstTcBase->TC_RA = auiReg[RA] ;
    //* Second Value -> Register B
    pstTcDev->pstTcBase->TC_RB = auiReg[RB] ;
    //* Third Value -> Register C
    pstTcDev->pstTcBase->TC_RC = auiReg[RC] ;
}
//* End
