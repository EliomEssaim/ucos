//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : lib_aic.h
//* Object              : Advanced Interrupt Controller Library Function.
//*                       Prototyping File.
//*
//* 1.0 01/04/00 JCZ    : Creation
//*----------------------------------------------------------------------------

#ifndef AicH
#define AicH

#include "UserType.h"
#include "At91M55800.h"
#include "MyBoard.h"
#include "../pio/Pio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (AIC_TYPE_HANDLE) (void) ;

/* External Interrupt Sources Descriptor */
typedef struct
{
    const PIO_DEV_ST   *pstPio ;     /* PIO Stucture definition */
    UINT32              uiSourceId ;     /* AIC interrupt source Identifier */
    UINT32              uiPinIrq ;       /* internal PIO pin number */
} EXT_IRQ_DEV_ST ;

/* Spurious Vector Reference (written in assembly) */
extern void SpuriousHandler ( void ) ;

/* Function Prototyping */
extern void IrqDefaultHandler ( void );
extern void FiqDefaultHandler ( void );
extern void IrqOpen ( UINT32 uiIrqId,UINT32 uiPriority,
                            UINT32 uiSrcType, AIC_TYPE_HANDLE pHandler ) ;

extern void IrqClose ( UINT32 uiIrqId ) ;
extern void IrqTrigCmd ( UINT32 uiIrqId, UINT32 uiMask  ) ;
extern UINT32 IrqGetStatus ( void ) ;

extern void ExtIrqOpen  ( const EXT_IRQ_DEV_ST *pstIrq,
                              UINT32 uiPriority,
                              UINT32 uiSrcType,
                              AIC_TYPE_HANDLE pHandler ) ;
extern void ExtIrqClose ( EXT_IRQ_DEV_ST *pstIrq ) ;

#ifdef __cplusplus
}
#endif

#endif /* AicH */
