#ifndef NandFlashH
#define NandFlashH

#include "../../include/UserType.h"
#include "../../include/at91m55800.h"
#include "../../AppC/Driver/adc/comdef.h"

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Constants for hardware specific CLE/ALE/NCE function
*/
/* Select the chip by setting nCE to low */
#define NF_CTL_SETNCE 1
/* Deselect the chip by setting nCE to high */
#define NF_CTL_CLRNCE 2
/* Select the command latch by setting CLE to high */
#define NF_CTL_SETCLE 3
/* Deselect the command latch by setting CLE to low */
#define NF_CTL_CLRCLE 4
/* Select the address latch by setting ALE to high */
#define NF_CTL_SETALE 5
/* Deselect the address latch by setting ALE to low */
#define NF_CTL_CLRALE 6
/* Set write protection by setting WP to high. Not used! */
#define NF_CTL_SETWP  7
/* Clear write protection by setting WP to low. Not used! */
#define NF_CTL_CLRWP  8

/*
 * Standard NAND flash commands
 */
#define NF_CMD_READ0      0
#define NF_CMD_READ1      1
#define NF_CMD_PAGEPROG   0x10
#define NF_CMD_READOOB    0x50
#define NF_CMD_ERASE1     0x60
#define NF_CMD_STATUS     0x70
#define NF_CMD_SEQIN      0x80
#define NF_CMD_READID     0x90
#define NF_CMD_ERASE2     0xd0
#define NF_CMD_RESET      0xff

/* size in page */
#define NF_A_AREA_SIZE     256
#define NF_B_AREA_SIZE     256
#define NF_C_AREA_SIZE     16
#define NF_SECTOR_SIZE     512
#define NF_PAGE_SIZE       528

/* there is 1024 blocks in K9F2808 */
#define NF_PAGE_NUM     0x8000      // 32k Total number of pages in the device.
#define NF_BLOCK_NUM    0x400       // 1k Total number of blocks in the device.
#define NF_BLOCK_SIZE   0x4000      // 16k block size
#define NF_TOTAL_SIZE   0x1000000   // 16M Device size
#define NF_DEV_ID       0xec73      // Device ID.
#define NF_BB_IND_OFF   0x6         // 6th of spare space

#define NF_PAGENUM_PERBLOCK (NF_BLOCK_SIZE/NF_SECTOR_SIZE)//一个BLOCK含有PAGE的数目

/*nand flash pio*/
#define NANDFLASH_PIO_CTRL g_stPioB
/* ce pin GPC5 */
#define PIN_CE  PB16
/* rb pin GPC4 */
#define PIN_RB  PB17

#define SETNCE  MA_OUTWM( PIOB_CODR, PIN_CE, PIN_CE )
#define CLRNCE  MA_OUTWM( PIOB_SODR, PIN_CE, PIN_CE )



#define NF_FLASH_BASE 0x40000000
#define RD_ADDR         (*(volatile UCHAR *)(NF_FLASH_BASE) )
#define WT_ADDR         RD_ADDR
#define ADDR_SET_CLE    (*(volatile UCHAR *)( NF_FLASH_BASE + 0x04 ))
#define ADDR_SET_ALE    (*(volatile UCHAR *)( NF_FLASH_BASE + 0x08 ))

//#define WAIT_FLASH_READY()  while (!MA_INHM( PIOB_PDSR, PIN_RB )) 

#define NF_WRITE_CMD(x)   \
{\
ADDR_SET_CLE=x;\
}

#define NF_WRITE_ADDR(x) \
{\
ADDR_SET_ALE=x;\
}


#define PAGE2BLOCK(page)    (page >> 5)
#define BLOCK2PAGE(block)   (block << 5)
#define ADDR2PAGE(addr)     (addr >> 9)

#define NF_START_BLOCK_FLAG   0xf5f5
#define NF_BAD_BLOCK_FLAG     0xf0f0
#define NF_UNUSED_BLOCK_FLAG  0xffff

#define FORMAT_INFO "MMI NAND FLASH 16M"


typedef enum {
    NF_PAGE_AREA_A,
    NF_PAGE_AREA_B,
    NF_PAGE_AREA_C
}PAGE_AREA_T;

extern UCHAR g_ucFlashIsOpen;

extern void NFDelay(UINT32 uiDelayTime);
extern LONG NFInit(void);
extern USHORT NFReadId(void);
extern void NFReadPage(UINT32 uiColumn, UINT32 uiPage, UCHAR * pucBuf);
extern void NFReadOob(UINT32 uiColumn, UINT32 uiPage, UCHAR * pucBuffer);
extern UCHAR NFWritePage(UINT32 uiColumn, UINT32 uiPage, UCHAR * pucBuf);
extern UCHAR NFErase(INT32 uiStartPage);
extern void NFReset(void);

extern LONG NFFormat(UINT32 uiPageNum);
extern INT32 NFOpen(void);
extern void NFRepBadBlock(USHORT usBadblockId);
extern void NFCheckBadBlock(USHORT usBlockNum, UCHAR * pucBuf);

extern void NFReadLogDateTime(UINT32 uiPage, UCHAR *pucBuf);
extern UINT32 LogicPage2PhyPage (UINT32 uiPage);

extern INT32 NFTest();

enum
{
    NF_OP_OK,
    NF_OP_CARD_NEED_INIT,
    /* the return value after sening a SD card command */
    NF_CMD_OK,
    NF_CMD_CRC_ERR,
    NF_CMD_TIME_ERR,

    NF_CMD_TIME_OUT,
    /* Data */
    NF_DATA_OK,
    NF_DATA_CRC_ERR,
    NF_DATA_TIMEOUT,
    NF_DATA_BITS_ERR,
    NF_DATA_FIFO_UNDER_ERR,
    NF_DATA_FIFO_OVER_ERR,

    /* the return value after init SD card  */
    NF_INIT_FAIL,

    /* the return value after read a block */
    NF_READ_BLOCK_FAIL,

    /* the return value after write a block */
    NF_WRITE_BLOCK_FAIL,
    NF_WRITE_CARD_PROTECD,
    NF_WRITE_BLOCK_PROTECD,
    NF_WRITE_LOCK,

    NF_ERASE_BLOCK_FAIL,
    NF_DEV_MEM_FULL,
    NF_DEV_FAIL
};

/*
//当有多个NF时用于扩展
// NF Device info.
typedef struct 
{
    UINT32 uiNumPages;          // Total number of pages in the device.
    UINT32 uiNumBlocks;             // Total number of blocks in the device.
    USHORT usPageSize;         //page size
    UINT32 uiBlockSize;          // block size
    UINT32 uiTotalSize;        //Device size
    USHORT usId;         // Device ID. 
} NF_DEV_INFO_ST;
*/
LONG NFInit(void);
UCHAR NFRead(UCHAR * pucBuf, UINT32 uiOffset, UINT32 uiCount);
UCHAR NFWrite(UCHAR * pucBuf, UINT32 uiOffset, UINT32 uiCount);
UCHAR NFBlockErase(UINT32 uiBlockIndex);
UCHAR NFChipErase(void);
USHORT NFReadPageId(void);

#ifdef __cplusplus
}
#endif

#endif /* NandFlashH */
