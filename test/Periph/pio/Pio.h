//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : lib_pio.h
//* Object              : Parallel I/O Function Prototyping File.
//*
//* 1.0 01/04/00 JCZ    : Creation
//*----------------------------------------------------------------------------

#ifndef PioH
#define PioH

#include "UserType.h"
#include "At91M55800.h"
#include "MyBoard.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Configuration bits Definition : argument <config> of at91_pio_open */
#define PIO_SENSE_BIT           0x1
#define PIO_OUTPUT              0x1
#define PIO_INPUT               0x0
#define PIO_FILTER_BIT          0x2
#define PIO_FILTER_ON           0x2
#define PIO_FILTER_OFF          0x0
#define PIO_OPENDRAIN_BIT       0x4
#define PIO_OPENDRAIN_ON        0x4
#define PIO_OPENDRAIN_OFF       0x0
#define PIO_INPUT_IRQ_BIT       0x8
#define PIO_INPUT_IRQ_ON        0x8
#define PIO_INPUT_IRQ_OFF       0x0
#define PIO_RESET_CONF          (PIO_INPUT|PIO_FILTER_OFF|PIO_OPENDRAIN_OFF|PIO_INPUT_IRQ_OFF)

/* Configuration bits Definition : argument <state> of at91_pio_write */
#define PIO_SET_OUT         0x1
#define PIO_CLEAR_OUT       0x0

/*-------------------------------------*/
/* PIO Controller Descriptor Structure */
/*-------------------------------------*/
typedef struct
{
    PIO_REG_ST  *pstPioBase ;         /* Base Address */
    UCHAR          ucPeriphId ;         /* Peripheral Identifier */
    UCHAR          ucPioNumber ;        /* Total Pin Number */
} PIO_DEV_ST ;

extern const PIO_DEV_ST g_stPioA;
extern const PIO_DEV_ST g_stPioB;

/* Functions Prototyping */
extern void  PioOpen  ( const PIO_DEV_ST *pstPio, UINT32 uiMask, UINT32 uiConfig ) ;
extern void  PioClose ( const PIO_DEV_ST *pstPio, UINT32 uiMask ) ;
extern void  PioWrite ( const PIO_DEV_ST *pstPio, UINT32 uiMask, UINT32 uiState ) ;
extern UINT32 PioRead  ( const PIO_DEV_ST *pstPio ) ;
extern void  PioSetMode  ( const PIO_DEV_ST *pstPio, UINT32 uiMask, UINT32 uiMode ) ;
extern void DataToPio ( const PIO_DEV_ST *pstPio, UINT32 DATA, UINT32 INDEX ) ;
#ifdef __cplusplus
}
#endif

#endif /* PioH */
