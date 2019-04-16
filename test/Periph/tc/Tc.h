//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : lib_tc.h
//* Object              : Timer Counter Library Function Prototyping File.
//*
//* 1.0 01/04/00 JCZ    : Creation
//*----------------------------------------------------------------------------

#ifndef TcH
#define TcH

#include "../../include/UserType.h"
#include "../../include/at91m55800.h"
#include "../../include/MyBoard.h"
#include "../pio/Pio.h"
#include "../aic/Aic.h"

#ifdef __cplusplus
extern "C" {
#endif

//* Define Register coding for <reg_id>
#define RA  0
#define RB  1
#define RC  2
#define CV  3

//* at91_tc_trig_cmd <cmd> parameter
#define TC_TRIG_CHANNEL         0x1
#define TC_TRIG_BLOCK           0x2

typedef void (*TypeTCHandler) (void) ;

/*------------------------------------*/
/* Timer Counter Descriptor Structure */
/*------------------------------------*/

typedef struct
{
    TC_REG_ST     *pstTcBase ;
    const PIO_DEV_ST  *pstPioDev ;
    UCHAR                  ucPeriphId ;
    UCHAR                  ucPinTioa ;
    UCHAR                  ucPinTiob ;
    UCHAR                  ucPinTclk ;
} TC_DEV_ST ;

/*------------------------------------------*/
/* Timer Counter Block Descriptor Structure */
/*------------------------------------------*/
typedef struct
{
    const TC_DEV_ST    *pstTc0Dev ;
    const TC_DEV_ST    *pstTc1Dev ;
    const TC_DEV_ST    *pstTc2Dev ;
} TC_BLOCK_DEV_ST ;

extern const TC_DEV_ST g_stTc0;
extern const TC_DEV_ST g_stTc1;
extern const TC_DEV_ST g_stTc2;
extern const TC_DEV_ST g_stTc3;
extern const TC_DEV_ST g_stTc4;
extern const TC_DEV_ST g_stTc5;

extern void TcOpen ( const TC_DEV_ST* pstTcDev, UINT32 uiMode, UINT32 uiEnaTioa, UINT32 uiEnaTiob );
extern void TcClose ( const TC_DEV_ST *pstTcDev );
extern UINT32 TcGetStatus ( const TC_DEV_ST *pstTcDev );
extern void TcTrigCmd ( const TC_DEV_ST *pstTcDev, UINT32 uiCmd );
extern void TcSetMode ( const TC_DEV_ST *pstTcDev, UINT32 uiMask, UINT32 uiData );
extern void TcRead ( const TC_DEV_ST *pstTcDev, UINT32 auiReg[] );
extern void TcWrite ( const TC_DEV_ST *pstTcDev, UINT32 auiReg[] );

#ifdef __cplusplus
}
#endif


#endif /* TcH */
