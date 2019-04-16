//*--------------------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*--------------------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*--------------------------------------------------------------------------------------
//* File Name           : lib_power_save.h
//* Object              : Power Saving Function Prototyping File.
//*
//* 1.0 01/04/00  JCZ   : Creation
//* 1.1 22/05/00  PFi   : Clean Up
//* 1.2 29/09/00  JPP   : 55800 and 42800
//* 1.3 06/08/01  PFi   : Add r40008
//*--------------------------------------------------------------------------------------
#ifndef ApmcH
#define ApmcH

#include "UserType.h"
#include "At91M55800.h"
#include "MyBoard.h"

#define     APMC_MODE_IDLE            0x1
#define     APMC_ALL_PERIPH_ACTIVE    0x2
#define     APMC_ALL_PERIPH_INACTIVE  0x3

/* -----------------------*/
/* Function Prototyping   */
/* -----------------------*/
/* For AT91M40400, AT91M40800, AT91R40807, AT91R40008, AT91M42800, AT91M55800, AT91M63200 */
extern void ApmcClockSetMode ( UINT32 uiMode ) ;
extern UINT32 ApmcClockGetStatus ( UINT32 uiPeriphId ) ;

/* For AT91M40800, AT91R40807, AT91R40008, AT91M42800, AT91M55800, AT91M63200 */
extern void ApmcClockOpen ( UINT32 uiPeriphId ) ;
extern void ApmcClockClose ( UINT32 uiPeriphId ) ;

/* For AT91M42800, AT91M55800 */
extern UINT32 ApmcClockGetPllStatus ( void ) ;
extern void ApmcClockGeneratorMode (UINT32 uiMode ) ;
extern UINT32 ApmcClockGeneratorState (void) ;

/* For AT91M55800 */
extern void ApmcInterruptEnable (UINT32 uiSourceEnable) ;
extern void ApmcInterruptDisable (UINT32 uiSourceDisable) ;
extern void ApmcPowerMode ( UINT32 uiMode ) ;
extern void ApmcTrigCmd ( UINT32 uiCmd ) ;

#endif /* ApmcH */
