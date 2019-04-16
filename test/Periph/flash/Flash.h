//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : lib_flash_at49.h
//* Object              : Flash AT49BV Prototyping File
//*                             AT49BV1604/AT49BV1604T
//*                             AT49BV1614/AT49BV1604T
//*                             AT49BV8011/AT49BV8011T
//*                             AT49BV8011/AT49BV8011T
//*
//* 1.0 02/11/00 JPP    : Creation
//*----------------------------------------------------------------------------

#ifndef FlashH
#define FlashH

//* --------------------------- include file ----------------------------------

#include "UserType.h"
#include "At91M55800.h"
#include "MyBoard.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------  Constants definition -------------------------*/


/*Flash type*/
#define FLASH_MANUFACTURED          0x00BF
#define FLASH_UNKNOW                      0xFFFF
#define FLASH_SST39VF1601            0x234B


/* Timeout loop count */
#define FLASH_TIME_OUT                    1000000
/* Loop count for delay after sequence */
#define FLASH_DELAY_ERASE                 10000000

/* Trials Number to erase a sector */
#define FLASH_NB_TRIAL_ERASE              10

/* Define Flash Codes */
#define FLASH_SEQ_ADD1              (0x5555)
#define FLASH_SEQ_ADD2              (0x2AAA)

#define FLASH_CODE1                 ((FLASHWORD)(0xAA))
#define FLASH_CODE2                 ((FLASHWORD)(0x55))
#define FLASH_ID_IN_CODE                  ((FLASHWORD)(0x90))
#define FLASH_ID_OUT_CODE                 ((FLASHWORD)(0xF0))
#define FLASH_WRITE_CODE                  ((FLASHWORD)(0xA0))
#define FLASH_ERASE_SECTOR_CODE1          ((FLASHWORD)(0x80))
#define FLASH_ERASE_SECTOR_CODE2          ((FLASHWORD)(0x30))
#define FLASH_CHIP_ERASE_CODE             ((FLASHWORD)(0x10))

/*---------------------------  Structure definition -------------------------*/

//* Defines Flash device definition structure
typedef struct 
{
    UINT32    uiFlashSize;    
    UINT32    uiFlashManufId;
    UINT32    uiFlashId;
    UINT32    uiFlashMask;
    UINT32    uiSectorNumber ;
    UINT32    uiSectorSize ;
    UINT32    uiFlashBlockNb;
    FLASHWORD *pfwBaseAddr;
    CHAR      *pcFlashName;

} FLASH_DEV_ST;

extern const FLASH_DEV_ST g_stNorFlash0;

/*---------------------------  Function Prototyping -------------------------*/

extern FLASHWORD FlashIdentify ( const FLASH_DEV_ST *pstFlash );
extern INT32 FlashEraseSector ( FLASHWORD *pfwBaseAddr, UCHAR *pucSectorAddr);

extern INT32 FlashWrite ( FLASHWORD *pfwBaseAddr,UCHAR *pucFlashAddr,UCHAR* pucData ,UINT32 uiSize);
extern INT32 FlashCheckFlashErased (const FLASH_DEV_ST *pstFlash );
extern INT32 FlashEraseFlash (const FLASH_DEV_ST *pstFlash );
extern void FlashRead(FLASHWORD *pfwBaseAddr,UCHAR *pucFlashAddr, UCHAR *pucData, UINT32 uiSize);


#ifdef __cplusplus
}
#endif

#endif  /* FlashH*/
