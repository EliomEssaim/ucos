//*--------------------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*--------------------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*--------------------------------------------------------------------------------------
//* File Name           : lib_apmc55800.c
//* Object              : Advanced Power Management Controller Library.
//*
//* 1.0 22/05/00  PF    : Creation
//*--------------------------------------------------------------------------------------
#include    "./Apmc.h"

//*--------------------------------------------------------------------------------------
//* Function Name       : at91_clock_set_uiMode
//* Object              : Set System Clock uiMode.
//* Input Parameters    : <uiMode> =
//* Output Parameters   : none
//* Functions called    : at91_error
//*--------------------------------------------------------------------------------------
void ApmcClockSetMode ( UINT32 uiMode )
//* Begin
{
    //* Depending on the required uiMode
    switch (uiMode)
    {
        //* Idle uiMode required
        case APMC_MODE_IDLE:
            //* Write the System Clock Disable Register
            APMC_SCDR = APMC_ARM7DIS ;
            break ;

        //* Active all peripheral clocks
        case APMC_ALL_PERIPH_ACTIVE:
            //* Enable all the peripheral clocks
            APMC_PCER = 0xFFFFFFFF ;
            break ;

        //* Desactive all peripheral clocks
        case APMC_ALL_PERIPH_INACTIVE:
            //* Disable all the peripheral clocks
            APMC_PCDR = 0xFFFFFFFF ;
            break ;

        //* Unknown
        default:
        //* Run AT91 Library error function
            break;    
    }
    //* EndSwitch
}
//* End

//*--------------------------------------------------------------------------------------
//* Function Name       : at91_clock_open
//* Object              : Enable the peripheral clock
//* Input Parameters    : <uiPeriphId> = peripheral identifier
//* Output Parameters   : none
//* Functions called    : none
//*--------------------------------------------------------------------------------------
void ApmcClockOpen ( UINT32 uiPeriphId )
//* Begin
{
    //* Write the Peripheral Clock Enable Register
    APMC_PCER = (1 << uiPeriphId) ;
}
//* End

//*--------------------------------------------------------------------------------------
//* Function Name       : at91_clock_close
//* Object              : Disable the clock of a Peripheral
//* Input Parameters    : <uiPeriphId> = peripheral identifier
//* Output Parameters   : none
//* Functions called    : none
//*--------------------------------------------------------------------------------------
void ApmcClockClose ( UINT32 uiPeriphId )
//* Begin
{
    //* Write the Peripheral Clock Disable Register
    APMC_PCDR = (1 << uiPeriphId) ;
}
//* End

//*--------------------------------------------------------------------------------------
//* Function Name       : at91_clock_get_status
//* Object              : Return the Peripheral clock status
//* Input Parameters    : <uiPeriphId> = peripheral identifier
//* Output Parameters   : none
//* Functions called    : none
//*--------------------------------------------------------------------------------------
UINT32 ApmcClockGetStatus ( UINT32 uiPeriphId )
//* Begin
{
    //* Return the Peripheral Clock Status Register
    return (APMC_PCSR & (1<<uiPeriphId) ) ;
}
//* End


//*--------------------------------------------------------------------------------------
//* Function Name       : at91_clock_get_pll_status
//* Object              : Return the APMC Status Register
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions called    : none
//*--------------------------------------------------------------------------------------
UINT32 ApmcClockGetPllStatus ( void )
//* Begin
{
    //* Return the PMC Status Register
    return ( APMC_SR ) ;
}
//* End

//*--------------------------------------------------------------------------------------
//* Function Name       : at91_clock_generator_uiMode
//* Object              : Set Master Clock selection, Main OSC & PLL
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions called    : none
//*--------------------------------------------------------------------------------------
void ApmcClockGeneratorMode (UINT32 uiMode ) 
//* Begin
{
    APMC_CGMR = uiMode ;
}
//* End
//*--------------------------------------------------------------------------------------
//* Function Name       : at91_clock_generator_state NEW
//* Object              : Return Master Clock selection, Main OSC & PLL
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions called    : none
//*--------------------------------------------------------------------------------------
UINT32 ApmcClockGeneratorState (void)
//* Begin
{
    //* Return the CGMR state
    return ( APMC_CGMR ) ;
}
//* End

//*--------------------------------------------------------------------------------------
//* Function Name       : at91_power_uiMode
//* Object              : Set Power uiMode register
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions called    : none
//*--------------------------------------------------------------------------------------
void ApmcPowerMode ( UINT32 uiMode )
//* Begin
{
    APMC_PMR = uiMode ;
}
//* End

//*--------------------------------------------------------------------------------------
//* Function Name       : at91_trig_cmd
//* Object              : Trig Power Control register
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions called    : none
//*--------------------------------------------------------------------------------------
void ApmcTrigCmd ( UINT32 uiCmd )
//* Begin
{
    APMC_PCR = uiCmd ;
}
//* End

//*--------------------------------------------------------------------------------------
//* Function Name       : at91_interrupt_enable NEW
//* Object              : Enable Main Oscillator or PLL source interrupt
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions called    : none
//*--------------------------------------------------------------------------------------
void ApmcInterruptEnable (UINT32 uiSourceEnable)
{
    APMC_IER = uiSourceEnable ;
}

//*--------------------------------------------------------------------------------------
//* Function Name       : at91_interrupt_disable NEW
//* Object              : Disable Main Oscillator or PLL source interrupt
//* Input Parameters    : none
//* Output Parameters   : none
//* Functions called    : none
//*--------------------------------------------------------------------------------------
void ApmcInterruptDisable (UINT32 uiSourceDisable)
{
    APMC_IDR = uiSourceDisable ;
}