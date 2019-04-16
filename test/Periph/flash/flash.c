//*-----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*-----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*-----------------------------------------------------------------------------
//* File Name               : lib_flash_at49.c
//* Object                  : FLASH programmer for :
//*                             - AT49BV1604/AT49BV1604T
//*                             - AT49BV1614/AT49BV1604T
//*                             - AT49BV8011/AT49BV8011T
//*                             - AT49BV8011/AT49BV8011T
//*
//* 1.1 06/11/00 JPP        : Creation
//*-----------------------------------------------------------------------------

//* --------------------------- include file ----------------------------------

#include    "./Flash.h"
//test
#include "../usart/usart.h"
#define  ENTER_CRITICAL()  (cpu_sr = CPUSaveSR())    /* Disable interrupts   */                     
#define  EXIT_CRITICAL()   (CPURestoreSR(cpu_sr))    /* Restore  interrupts */

const FLASH_DEV_ST g_stNorFlash0 =
{
    2*1024*1024,                  /* 2 M Bytes */
    FLASH_MANUFACTURED,
    FLASH_SST39VF1601,
    0x000FFFFF,
    512,
    1024*4,
    1,
    (FLASHWORD *)0x1000000,
    "SST39VF1601",
};
//* --------------------------- Static function -------------------------------

//*----------------------------------------------------------------------------
//* Function Name       : Pause
//* Object              : wait flash responce
//* Input Parameters    : none
//* Output Parameters   : none
//*----------------------------------------------------------------------------
static void Pause(UINT32 uiCount)
{
    UINT32 i;
    for (i=0;i < uiCount ;i++)
    {
        /* Do nothing - just wait */
    }
}

//* --------------------------- Export function -------------------------------

//*----------------------------------------------------------------------------
//* Function Name       : flash_at49_identify
//* Object              : Get the device id
//* Input Parameters    : <sart_addr> Flash base address
//* Output Parameters   : device_code or 0xFFFF if bad manuf code
//*----------------------------------------------------------------------------
FLASHWORD FlashIdentify ( const FLASH_DEV_ST *pstFlash )
//* Begin
{
    FLASHWORD    *pfwBaseAddr = pstFlash->pfwBaseAddr;
    FLASHWORD      fwManufCode ;
    FLASHWORD      fwDeviceCode ;
    Pause(FLASH_TIME_OUT/10);
    //* Enter Software Product Identification Mode
    *(pfwBaseAddr + FLASH_SEQ_ADD1) = FLASH_CODE1;
    *(pfwBaseAddr + FLASH_SEQ_ADD2) = FLASH_CODE2;
    *(pfwBaseAddr + FLASH_SEQ_ADD1) = FLASH_ID_IN_CODE;

    //* Read Manufacturer and device code from the device
    fwManufCode  = *pfwBaseAddr ;
    fwDeviceCode = *(pfwBaseAddr + 1) ;

    //* Exit Software Product Identification Mode
    *(pfwBaseAddr + FLASH_SEQ_ADD1) = FLASH_CODE1;
    *(pfwBaseAddr + FLASH_SEQ_ADD2) = FLASH_CODE2;
    *(pfwBaseAddr + FLASH_SEQ_ADD1) = FLASH_ID_OUT_CODE;

    if ( fwManufCode != FLASH_MANUFACTURED )
    {
        return (FLASH_UNKNOW);
    }

    //* Return pointer to Flash found
    return ( fwDeviceCode ) ;
}

//*----------------------------------------------------------------------------
//* Function Name       : flash_wait_flash_ready
//* Object              : wait the end of write
//* Input Parameters    : <address> Adress to write
//*                       <data> data write at the  <address>
//* Output Parameters   : if data write TRUE or FALSE if time out
//*----------------------------------------------------------------------------
INT32 FlashWaitReady ( FLASHWORD *pfwAddress, FLASHWORD fwData )
{
//* Begin
    UINT32 uiCount = 0 ;    

    //* While two consecutive read don't give same value or timeout
    while (( *pfwAddress != fwData ) && ( uiCount++ < FLASH_TIME_OUT )) ;

    //* If timeout
    if ( uiCount < FLASH_TIME_OUT )
    {  
        return ( TRUE ) ;
    }
    //* Else
    else
    {  
        return ( FALSE ) ;
    }
    //* Endif
}
//* End


//*----------------------------------------------------------------------------
//* Function Name       : flash_at49_check_flash_erased
//* Object              : check if all sector are erased
//* Input Parameters    : <flash> Flash descriptor
//*                       <sector_addr> base sector address
//* Output Parameters   : if data sector erase TRUE or FALSE
//*----------------------------------------------------------------------------
INT32 FlashCheckFlashErased ( const FLASH_DEV_ST *pstFlash )
//* Begin
{
    INT32   i ;
    FLASHWORD  fwReadData ;
    INT32   uiSize = pstFlash->uiFlashSize;

    //* For each word of the flash
    for ( i = 0 ; i < (uiSize/2) ; i ++ )
    {
        //* Check erased value reading, if not
        if (( fwReadData = *(pstFlash->pfwBaseAddr + i )) != (FLASHWORD)0xFFFF )
        {
            //*  return False
            return ( FALSE ) ;
        }
        //* Endif
    }
    //* Endfor

    //* Return True
    return ( TRUE ) ;
}
//* End




//*----------------------------------------------------------------------------
//* Function Name       : flash_at49_erase_sector
//* Object              : erase flash sector
//* Input Parameters    : <pfwBaseAddr> Flash base address
//*                       <sector_addr> Flash sector address
//* Output Parameters   : none
//*----------------------------------------------------------------------------
INT32 FlashEraseSector (FLASHWORD *pfwBaseAddr, UCHAR *pucSectorAddr)
//* Begin
{
    FLASHWORD* pucSectorAddrTemp;
    (UINT32)pucSectorAddrTemp = (UINT32)pucSectorAddr & 0xFFFFFFFE;     
    
    //* Enter Sector Erase Sequence codes
    *(pfwBaseAddr + FLASH_SEQ_ADD1) = FLASH_CODE1;
    *(pfwBaseAddr + FLASH_SEQ_ADD2) = FLASH_CODE2;
    *(pfwBaseAddr + FLASH_SEQ_ADD1) = FLASH_ERASE_SECTOR_CODE1; 
    *(pfwBaseAddr + FLASH_SEQ_ADD1) = FLASH_CODE1;
    *(pfwBaseAddr + FLASH_SEQ_ADD2) = FLASH_CODE2;

//test
    UINT32 i;
        for(i=0;i<500;i++);      
    *(pucSectorAddrTemp) = FLASH_ERASE_SECTOR_CODE2 ;
    
    if ( FlashWaitReady( pucSectorAddrTemp, (FLASHWORD)0xFFFF )== FALSE ) 
    {
          return(FAILED);
    }   

    return(SUCCEEDED);
}
//* End

//*----------------------------------------------------------------------------
//* Function Name       : flash_at49_write_flash
//* Object              : Write short in Flash
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
INT32 FlashWriteOne ( FLASHWORD *pfwBaseAddr,FLASHWORD *pfwLoadAddr,FLASHWORD fwData )
{
    FLASHWORD  fwReadData ;
 
    //* Enter Programming code sequence
    *(pfwBaseAddr + FLASH_SEQ_ADD1) = FLASH_CODE1 ;
    *(pfwBaseAddr + FLASH_SEQ_ADD2) = FLASH_CODE2 ;
    *(pfwBaseAddr + FLASH_SEQ_ADD1) = FLASH_WRITE_CODE ;
    *(pfwLoadAddr) = fwData ; 


    //* Wait for Flash ready after erasing, if timeout
    if ( FlashWaitReady (pfwLoadAddr, fwData ) != TRUE )
    {
        return ( FAILED ) ;
    }
    //* Endif

    //* If Data written does not equal data
    if (( fwReadData = *pfwLoadAddr ) != fwData )
    {
        //* Return False
        return ( FAILED );
    }
    //* Endif
    fwReadData = *(pfwLoadAddr);
    //* Return False
    return ( SUCCEEDED ) ;
}


INT32 FlashWrite ( FLASHWORD *pfwBaseAddr,UCHAR *pucFlashAddr,UCHAR* pucData ,UINT32 uiSize)
{
    FLASHWORD fwReadData;
    FLASHWORD *pfwAddr;
    UCHAR *pucDataTmp;
    UINT32 i,uiCount;
    uiCount = uiSize;
    pfwAddr = (FLASHWORD*)pucFlashAddr;
    pucDataTmp = pucData;
    
    if(uiCount == 0)
    {
        return SUCCEEDED;
    }
    
    if((UINT32)pfwAddr%2 != 0)
    {
        (UINT32)pfwAddr = (UINT32)pfwAddr - 1;
        fwReadData = *pfwAddr;  
        fwReadData = (fwReadData & 0x00FF) + (((FLASHWORD)*pucDataTmp)<<8);
        if(FlashWriteOne ( pfwBaseAddr,pfwAddr,fwReadData ) == FAILED)
        {
            return(FAILED);
        }
        pfwAddr = pfwAddr + 1;
     uiCount = uiCount - 1;
     pucDataTmp = pucDataTmp + 1;
    }

    if(uiCount/2 > 0)
    {
        for(i=0;i<uiCount/2;i++)
        {
            fwReadData = (FLASHWORD)(*(pucDataTmp+2*i))+((FLASHWORD)(*(pucDataTmp+2*i+1))<<8);
         if(FlashWriteOne(pfwBaseAddr,pfwAddr+i,fwReadData) == FAILED)
            {
             return(FAILED);
            }
        }
    }
    
    if(uiCount%2 != 0)
    {
        pucDataTmp = pucDataTmp + uiCount -1;
        (UINT32)pfwAddr = (UINT32)pfwAddr + uiCount - 1;
        
        fwReadData = *(pfwAddr);
        fwReadData = (fwReadData & 0xFF00) + (FLASHWORD)(*pucDataTmp);
        if(FlashWriteOne ( pfwBaseAddr,pfwAddr,fwReadData ) == FAILED)
        {
            return(FAILED);
        }
    }

    return(SUCCEEDED);
    
}
//* End
//*----------------------------------------------------------------------------
//* Function Name       : flash_at49_erase_flash
//* Object              : ERASSE FLASH
//* Input Parameters    : <flash> Flash descriptor
//*                       <pfwBaseAddress> base sector address
//* Output Parameters   : if data sector erase TRUE or FALSE
//*----------------------------------------------------------------------------
INT32 FlashEraseFlash ( const FLASH_DEV_ST *pstFlash )
//* Begin
{
    INT32     iTrial = 0;
    INT32     iCmpt;
    INT32     iStatus= SUCCEEDED;
    FLASHWORD    *pfwBaseAddr = pstFlash->pfwBaseAddr;
    //* While flash is not erased or too much erasing performed
    while (( FlashCheckFlashErased ( pstFlash ) == FALSE ) &&
              ( iTrial++ < FLASH_NB_TRIAL_ERASE ))
    {

        //* Enter Sector Erase Sequence codes
        *(pfwBaseAddr + FLASH_SEQ_ADD1) = FLASH_CODE1;
        *(pfwBaseAddr + FLASH_SEQ_ADD2) = FLASH_CODE2;
        *(pfwBaseAddr + FLASH_SEQ_ADD1) = FLASH_ERASE_SECTOR_CODE1;
        *(pfwBaseAddr + FLASH_SEQ_ADD1) = FLASH_CODE1;
        *(pfwBaseAddr + FLASH_SEQ_ADD2) = FLASH_CODE2;
        *(pfwBaseAddr + FLASH_SEQ_ADD1) = FLASH_CHIP_ERASE_CODE;

        //* wait
        for ( iCmpt = 0 ; iCmpt < FLASH_DELAY_ERASE ; iCmpt ++ ) ;

        //* Wait for Flash Ready after Erase, if timeout
        if ( FlashWaitReady ( pfwBaseAddr,0xFFFF ) == FALSE )
        {
            //* return False
            if ( iTrial==FLASH_NB_TRIAL_ERASE) return (FAILED) ;

        }
        //* Endif
    }
    //* EndWhile

    //* Return True
    return ( iStatus ) ;
}


void FlashRead(FLASHWORD *pfwBaseAddr,UCHAR *pucFlashAddr, UCHAR *pucData, UINT32 uiSize)
{
    FLASHWORD fwReadData;
    FLASHWORD *pfwAddr;
    UCHAR *pucDataTmp;
    UINT32 i,uiCount;
    uiCount = uiSize;
    pfwAddr = (FLASHWORD*)pucFlashAddr;
    pucDataTmp = pucData;
    
    if(uiCount == 0)
    {
        return;
    }
    
    if((UINT32)pfwAddr%2 != 0)
    {
        (UINT32)pfwAddr = (UINT32)pfwAddr - 1;
        fwReadData = *pfwAddr;
        fwReadData = fwReadData & 0xFF00;
        fwReadData = fwReadData >> 8;
        *pucDataTmp = (UCHAR)(fwReadData);
        pfwAddr = pfwAddr + 1;
        uiCount = uiCount - 1;
        pucDataTmp = pucDataTmp + 1;
    }

    if(uiCount/2 > 0)
    {
        for(i=0;i<uiCount/2;i++)
        {
            fwReadData = *(pfwAddr+i);
           *( pucDataTmp+2*i) = (UCHAR)fwReadData;
           fwReadData = fwReadData >>8;
           *( pucDataTmp+2*i+1) = (UCHAR)fwReadData;           
        }
    }

    if(uiCount%2 != 0)
    {
        pucDataTmp = pucDataTmp + uiCount -1;
        (UINT32)pfwAddr = (UINT32)pfwAddr + uiCount - 1;
        
        fwReadData = *(pfwAddr);
        fwReadData = fwReadData & 0x00FF;
        *pucDataTmp = (UCHAR)(fwReadData);
    }

}
//* End
