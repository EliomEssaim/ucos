\#include "./NandFlash.h"
#include "../pio/Pio.h"

//逻辑BLOCK和物理BLOCK的对应表
USHORT g_usBlockLookupTbl[NF_BLOCK_NUM] = {0};
UCHAR g_ucSpareBuf[16];
USHORT g_usLastPage;
UCHAR g_ucFlashIsOpen = 0;

/*************************************************
  Function:       LogicPage2PhyPage
  Description:    本函数将逻辑的PAGE地址转换为物理PAGE地址

  Calls:
  Called By:
  Input:          
                  uiPage:逻辑的PAGE地址
  Output:     
  Return:         
  Others:
*************************************************/
UINT32 LogicPage2PhyPage (UINT32 uiPage)
{
    UINT32 uiPhyPage;
    UINT32 uiPhyBlock;
    UINT32 uiPageOffset;

    uiPageOffset = uiPage & (NF_PAGENUM_PERBLOCK - 1);
    uiPhyBlock = g_usBlockLookupTbl[PAGE2BLOCK(uiPage)];
    uiPhyPage = BLOCK2PAGE(uiPhyBlock) + uiPageOffset;

    return uiPhyPage;
}


UINT32 NFReady(void)
{
    UINT32 i;
    for(i=0;i<0xfff;i++)
    {
//        NF_WRITE_CMD(NF_CMD_STATUS);

//        if ( (RD_ADDR & 0x40) != 0 )
//        {
//            return TRUE;
//        }
//        else
//        {
            if((PioRead(&NANDFLASH_PIO_CTRL) & PIN_RB)!=0)
            {
                return TRUE;
            }
//        }
    }
    return FALSE;
}


UCHAR NFRead (UCHAR *pucBuf, UINT32 uiOffset, UINT32 uiCount)
{
    UINT32 uiPage;
    USHORT usPhyBlock;
    USHORT usPageOffset;

    uiPage = ADDR2PAGE(uiOffset);
    usPageOffset = uiPage & (32 - 1);
    usPhyBlock = g_usBlockLookupTbl[PAGE2BLOCK(uiPage)];
    uiPage = BLOCK2PAGE(usPhyBlock) + usPageOffset;
    uiCount = ADDR2PAGE(uiCount);
    if ( uiCount + usPageOffset > NF_PAGENUM_PERBLOCK )
    {
        uiCount = NF_PAGENUM_PERBLOCK - usPageOffset;
    }
    while ( uiCount-- )
    {
        NFReadPage(0, uiPage++, pucBuf);
        pucBuf += NF_SECTOR_SIZE;
    }

    return 0;
}


UCHAR NFWrite (UCHAR *pucBuf, UINT32 uiOffset, UINT32 uiCount)
{
    UINT32 uiPage;
    USHORT usPhyBlock;
    USHORT usPageOffset;
    UCHAR ucStatus;

    uiPage = ADDR2PAGE(uiOffset);
    usPageOffset = uiPage & (NF_PAGENUM_PERBLOCK - 1);
    usPhyBlock = g_usBlockLookupTbl[PAGE2BLOCK(uiPage)];
    uiPage = BLOCK2PAGE(usPhyBlock) + usPageOffset;
    uiCount = ADDR2PAGE(uiCount);
    if ( uiCount + usPageOffset > NF_PAGENUM_PERBLOCK )
    {
        uiCount = NF_PAGENUM_PERBLOCK - usPageOffset;
    }
    while ( uiCount-- )
    {
        ucStatus = NFWritePage(0, uiPage++, pucBuf);
        if(ucStatus != NF_OP_OK)
        {
            NFWritePage(0, uiPage++, pucBuf);
        }
        pucBuf += NF_SECTOR_SIZE;
    }
    return ucStatus;
}

/*************************************************
  Function:       NFBlockErase
  Description:    本函数擦除NANDFLASH的一个逻辑BLOCK
                  
  Calls:
  Called By:
  Input:          uiBlockIndex:逻辑BLOCK序号
  
  Output:     
  Return:         
  Others:
*************************************************/
UCHAR NFBlockErase (UINT32 uiBlockIndex)
{
    return NFErase(BLOCK2PAGE(g_usBlockLookupTbl[uiBlockIndex]));
}


UCHAR NFChipErase (void)
{
    UINT32 i;
    UCHAR ucStatus;
    ucStatus = NF_OP_OK;
 
    for(i=0;i<NF_BLOCK_NUM;i++)
    {
        if(NFBlockErase(i) != NF_OP_OK)
        {
            ucStatus = NF_ERASE_BLOCK_FAIL;
        }
    }
    return ucStatus;
}


UINT32 NFBufCompare(UCHAR *pucBuf1, UCHAR *pucBuf2, UINT32 uiSize)
{ 
    UINT32 i;

    for ( i = 0; i < uiSize; i++ )
    {
        if ( pucBuf1[i] != pucBuf2[i] )
        {
            return (UINT32)&pucBuf2[i];
        }
    }
    return 0;
}

/*page number must 32 align*/
LONG NFFormat(UINT32 uiPageNum)
{
    USHORT usBlockNum;
    UINT32 i,j;
    UCHAR *pucBuf;
    UCHAR *pucVeriBuffer;
    UCHAR ucProgramOk = 1;
    UCHAR ucStatus;
    UCHAR aucBuffer[NF_SECTOR_SIZE];
    UCHAR aucVeriBuffer[NF_SECTOR_SIZE];

    NFDelay(100);
    if ( NFReadPageId() != NF_DEV_ID)
        return FAILED;//bad id
    usBlockNum = PAGE2BLOCK(uiPageNum);

    pucBuf = aucBuffer;

    pucVeriBuffer = aucVeriBuffer;

    /* erase all */
    for (i = 0; i < NF_BLOCK_NUM; i++ )
    {
        if(NFErase(BLOCK2PAGE(i)) != NF_OP_OK)
        {
            return FAILED;
        }
    }
    NFDelay(100);
    NFCheckBadBlock(usBlockNum, pucBuf);
    /*write the block look up table to block 0*/

    j = 0;
    while (1)
    {
        //只格式化3次
        j++;
        if(j>3)
        {
            return FAILED;
        }

        ucProgramOk = 1;
        NFErase(BLOCK2PAGE(0)); //erase the block
        /*write format information */
        memset(pucBuf, 0, NF_SECTOR_SIZE);
        memcpy(pucBuf, (CHAR *)FORMAT_INFO, sizeof(FORMAT_INFO));
        NFWritePage(0,0, pucBuf);//write the first page
        NFReadPage(0, 0, pucVeriBuffer);
        //check if program OK
        if ( NFBufCompare(pucBuf, pucVeriBuffer,NF_SECTOR_SIZE) != 0 )
        {
            continue;
        }

        /* write g_usBlockLookupTbl */
        i = 0;
        while ( i < 4 )
        {
            ucStatus = NFWritePage(0, i+1, (UCHAR *)&g_usBlockLookupTbl[i<<8]);
            if ( ucStatus != NF_OP_OK )
            {
                ucProgramOk = 0;
                break;
            }
            NFReadPage(0, i+1, (UCHAR *)pucVeriBuffer);
            //check if program OK
            if ( NFBufCompare((UCHAR *)&g_usBlockLookupTbl[i<<8], pucVeriBuffer,NF_SECTOR_SIZE) != 0 )
            {
                ucProgramOk = 0;
                break;
            }
            i++;
        }
        if ( ucProgramOk == 1 )
            break;
    }
    return SUCCEEDED;
}


INT32 NFOpen(void)
{
    UINT32 i;
    USHORT usBlockNum = 0;
    USHORT *pusTempTbl;
    UCHAR *pucBuffer;
    UCHAR aucBuffer1[NF_SECTOR_SIZE];
    USHORT ausTempTbl[NF_BLOCK_NUM];

    if ( NFReadPageId() != NF_DEV_ID)
        return FAILED;

    pucBuffer = aucBuffer1;
    memset(pucBuffer,0,NF_SECTOR_SIZE);

    while (1)
    {
        NFReadPage(0,0,pucBuffer);
        if ( pucBuffer[0] != 'M' )//have not been formated
        {
            if(NFFormat(BLOCK2PAGE(NF_BLOCK_NUM))==FAILED)
            {
                return FAILED;
            }
        }
        else
        {
            break;
        }
    }

    /* read g_usBlockLookupTbl */
    pusTempTbl = ausTempTbl;

    i = 0;
    while ( i < 4 )
    {
        NFReadPage(0, i+1, (UCHAR *)&pusTempTbl[i<<8]);
        i++;
    }

    memset(g_usBlockLookupTbl, 0xff, sizeof(g_usBlockLookupTbl));

    for (i = 1; i < NF_BLOCK_NUM; i++ )
    {
        if ( pusTempTbl[i] != 0xf0f0 && pusTempTbl[i] != 0xffff )
        {
            g_usBlockLookupTbl[ pusTempTbl[i] ] = i;
            usBlockNum++;
        }
    }

    /*g_usBlockLookupTbl now is a map table from logic sectors to physical sectors */
    g_usLastPage = BLOCK2PAGE(usBlockNum) - 1;
    return SUCCEEDED;
}

void NFRepBadBlock(USHORT usBadblockId)
{
    UCHAR *pucBuffer;
    USHORT usNewblockId;
    USHORT usPhyNewblock,usPhyBadblock;
    UINT32 i;
    UCHAR aucBuffer[NF_SECTOR_SIZE];

    if(NFOpen() == FAILED)
        return;

    pucBuffer = aucBuffer;

    if ( usBadblockId != 0 )
    {
        usNewblockId = usBadblockId;
        usPhyBadblock = g_usBlockLookupTbl[usBadblockId];
    }
    else
    {
        usNewblockId = PAGE2BLOCK(g_usLastPage + 1);
    }

    NFReadPage(0,0,pucBuffer);

    while ( i < 4 )
    {
        NFReadPage(0, i+1, (UCHAR *)&g_usBlockLookupTbl[i<<8]);
        i++;
    }

    for (i = 1; i < NF_BLOCK_NUM; i++ )
    {
        if ( g_usBlockLookupTbl[i] == NF_UNUSED_BLOCK_FLAG)
        {
            break;
        }
    }

    if ( i == NF_BLOCK_NUM )
    {
        return; //no unused good block yet
    }

    usPhyNewblock = i;
    g_usBlockLookupTbl[usPhyNewblock] = usNewblockId; //mark new block
    g_usBlockLookupTbl[usPhyBadblock] = NF_BAD_BLOCK_FLAG; //mark bad block

    while (1)
    {
        NFErase(0);

        NFWritePage(0,0, pucBuffer);

        /* write g_usBlockLookupTbl */
        while ( i < 4 )
        {
            NFWritePage(0, i+1, (UCHAR *)&g_usBlockLookupTbl[i<<8]);
            i++;
        }
        break;
    }
}


void NFCheckBadBlock(USHORT usBlockNum, UCHAR *pucBuf)
{
    USHORT usBlock;
    USHORT usBlockId = 0;
    UCHAR ucFlag1, ucFlag2;

    g_usBlockLookupTbl[0] = NF_START_BLOCK_FLAG;

    for (usBlock = 1; usBlock < NF_BLOCK_NUM; usBlock++)
    {
        NFReadPage(0, BLOCK2PAGE(usBlock), pucBuf);
        ucFlag1 = g_ucSpareBuf[NF_BB_IND_OFF];
        NFReadPage(0, BLOCK2PAGE(usBlock)+1, pucBuf);
        ucFlag2 = g_ucSpareBuf[NF_BB_IND_OFF];
        if ((ucFlag1 != 0xff) || (ucFlag2 != 0xff))//bad block
        {
            g_usBlockLookupTbl[usBlock] = NF_BAD_BLOCK_FLAG;
        }
        else//good block
        {
            if ( usBlockId < usBlockNum)
            {
                g_usBlockLookupTbl[usBlock] = usBlockId++;
            }
            else//good and unused
            {
                g_usBlockLookupTbl[usBlock] = NF_UNUSED_BLOCK_FLAG;
            }
        }
    }
}


void NFDelay(UINT32 uiDelayTime)
{
    UINT32 i;

    while (uiDelayTime-- > 0)
    {
        for (i=0;i<10; i++);
        {
        }
    }

}

void NFReset(void)
{
    UINT32 i;
    NF_WRITE_CMD(NF_CMD_RESET);
    for (i=0;i<0xfff;i++)
    {
    }
}

void NFInitPio(void)
{
    /*R/B, input*/
    /*CE, output*/
    PioOpen(&NANDFLASH_PIO_CTRL, PIN_CE,PIO_OUTPUT);
    PioOpen(&NANDFLASH_PIO_CTRL, PIN_RB,PIO_INPUT);
    /*CE LOW*/
    PioWrite(&NANDFLASH_PIO_CTRL, PIN_CE, PIO_CLEAR_OUT);
}

LONG NFInit(void)
{
    NFInitPio();
    NFReset();
    if(NFOpen()==FAILED)
    {
        return FAILED;
    }
    else
    {
        return SUCCEEDED;
    }
}


USHORT NFReadPageId(void)
{
    USHORT usId;
    USHORT usIdH;
    USHORT usIdL;

    PioWrite(&NANDFLASH_PIO_CTRL, PIN_CE, PIO_CLEAR_OUT);

    NF_WRITE_CMD(NF_CMD_READID);
    NF_WRITE_ADDR(0x00);
    NFDelay(100);
    usIdH = RD_ADDR;
    NFDelay(100);
    usIdL= RD_ADDR;
    usId =(usIdH<<8) | usIdL;

    return usId;
}
/*************************************************
  Function:       NFReadPage
  Description:    本函数读NANDFLASH的一个PAGE

  Calls:
  Called By:
  Input:          uiColumn:
                  uiPage:物理PAGE地址   
                  pucBuf:   
  Output:     
  Return:         
  Others:
*************************************************/
void NFReadPage(UINT32 uiColumn, UINT32 uiPage, UCHAR *pucBuf)
{
    UINT32 i;

    NF_WRITE_CMD(NF_CMD_READ0);
    NF_WRITE_ADDR(uiColumn);
    NF_WRITE_ADDR(uiPage & 0xff);
    NF_WRITE_ADDR((uiPage >> 8) & 0xff);

    if(NFReady()==FALSE)
    {
        return;
    }
    /* read to buffer */
    for ( i = 0; i < NF_SECTOR_SIZE; i++)
    {
        pucBuf[i] = RD_ADDR;
    }

    /* write SPARE SPACE, write 0 when not use FTL*/
    for ( i = 0; i < NF_C_AREA_SIZE; i++)
    {
        g_ucSpareBuf[i] = RD_ADDR;
    }
}
#if 0
void NF_read(PAGE_AREA_T area, int column, int page, UCHAR *buf, USHORT len)
{
	int i;
	UCHAR cmd;
	USHORT len_limit;
	switch (area)
	{
		case NF_PAGE_AREA_A:
			cmd = NF_CMD_READ0;
			len_limit = NF_PAGE_SIZE;
			break;
		case NF_PAGE_AREA_B:
			cmd = NF_CMD_READ1;
			len_limit = NF_PAGE_AREA_B + NF_PAGE_AREA_C; 
			break;
		case NF_PAGE_AREA_C:
			cmd = NF_CMD_READOOB;
			len_limit = NF_PAGE_AREA_C;
			break;
		default:
			break;
	}
	NF_WRITE_CMD(cmd);
	NF_WRITE_ADDR(column);
	NF_WRITE_ADDR(page & 0xff);
	NF_WRITE_ADDR((page >> 8) & 0xff);
	
    if(NFReady()==FALSE)
    {
        return;
    }
		;
	/* read to buffer */
	len = ( len > len_limit ) ? len_limit : len;
	for ( i = 0; i < len; i++)
	{
		buf[i] = RD_ADDR;
	}
}
#endif
void NFReadPageOob(UINT32 uiColumn, UINT32 uiPage, UCHAR *pucBuffer)
{
    UINT32 i;

    NF_WRITE_CMD(NF_CMD_READOOB);
    NF_WRITE_ADDR(uiColumn);
    NF_WRITE_ADDR(uiPage & 0xff);
    NF_WRITE_ADDR((uiPage >> 8) & 0xff);
    if(NFReady()==FALSE)
    {
        return;
    }
    /* read to buffer */

    for ( i = 0; i < NF_C_AREA_SIZE; i++)
    {
        pucBuffer[i] = RD_ADDR;
    }

}
/*************************************************
  Function:       NFWritePage
  Description:    本函数写NANDFLASH的一个PAGE

  Calls:
  Called By:
  Input:          uiColumn:
                  uiPage:物理PAGE地址   
                  pucBuf:   
  Output:     
  Return:         
  Others:
*************************************************/
UCHAR NFWritePage(UINT32 uiColumn, UINT32 uiPage, UCHAR *pucBuf)
{
    INT32 i;

    NF_WRITE_CMD(NF_CMD_SEQIN);
    NF_WRITE_ADDR(uiColumn);
    NF_WRITE_ADDR(uiPage & 0xff);
    NF_WRITE_ADDR((uiPage >> 8) & 0xff);

    /* write from buffer */
    for ( i = 0; i < NF_SECTOR_SIZE; i++)
    {
        WT_ADDR = pucBuf[i];
    }
    /* write SPARE SPACE, write 0 when not use FTL*/
    for ( i = 0; i < NF_C_AREA_SIZE; i++)
    {
        WT_ADDR = 0x00;
    }

    NF_WRITE_CMD(NF_CMD_PAGEPROG);

    /* read I/O bit 6 */
    if(NFReady()==FALSE)
    {
        return;
    }
    //NFDelay(100);
    NF_WRITE_CMD(NF_CMD_STATUS);

    if ( (RD_ADDR & 0x01) == 0 )
    {
        return NF_OP_OK;
    }
    else
    {
        return NF_WRITE_BLOCK_FAIL;
    }

}
/*************************************************
  Function:       NFErase
  Description:    本函数擦除NANDFLASH的一个BLOCK
                  输入地址为PAGE地址(A9-A23),但A9-A13无效
  Calls:
  Called By:
  Input:          uiStartPage:物理BLOCK序号转化后的物理PAGE地址
  
  Output:     
  Return:         
  Others:
*************************************************/
UCHAR NFErase(INT32 uiStartPage)
{
    NF_WRITE_CMD(NF_CMD_ERASE1);
    NF_WRITE_ADDR(uiStartPage & 0xff);
    NF_WRITE_ADDR((uiStartPage >> 8) & 0xff);
    NF_WRITE_CMD(NF_CMD_ERASE2);

    /* read I/O bit 6 */
    if(NFReady()==FALSE)
    {
        return NF_ERASE_BLOCK_FAIL;
    }
    NF_WRITE_CMD(NF_CMD_STATUS);

    if ( (RD_ADDR & 0x01) == 0 )
    {
        return NF_OP_OK;
    }
    else
    {
        return NF_ERASE_BLOCK_FAIL;
    }
}

INT32  NFTest()
{
    UINT32 i,j;
    UCHAR aucTemp1[NF_SECTOR_SIZE],aucTemp2[NF_SECTOR_SIZE];
    USHORT usPhyBlock;

    NFInitPio();
    NFReset();

    if(NFOpen() == FAILED)
    {
        return FAILED;
    }

    for ( i = 0; i < NF_SECTOR_SIZE; i++ )
    {
        aucTemp1[i] = (UCHAR)(i+1);
    }

    for( i = 0; i<NF_BLOCK_NUM; i++)
    {
        if( (g_usBlockLookupTbl[i] != NF_START_BLOCK_FLAG) &&
            (g_usBlockLookupTbl[i] != NF_BAD_BLOCK_FLAG) &&
                (g_usBlockLookupTbl[i] != NF_UNUSED_BLOCK_FLAG) )
        {
            usPhyBlock = g_usBlockLookupTbl[i];
            NFErase(BLOCK2PAGE(usPhyBlock));
            for( j = 0; j < NF_PAGENUM_PERBLOCK; j++)
            {
                NFWritePage(0, BLOCK2PAGE(usPhyBlock) + j, &aucTemp1);
                memset(&aucTemp2,0,NF_SECTOR_SIZE);
                NFReadPage(0, BLOCK2PAGE(usPhyBlock) + j, &aucTemp2);
                if(NFBufCompare(&aucTemp1,&aucTemp2,NF_SECTOR_SIZE) != 0)
                {
                    j = BLOCK2PAGE(usPhyBlock) + j;
                }
            }
        }
    }
    return SUCCEEDED;

}

/*************************************************
  Function:       NFReadLogDateTime
  Description:    本函数读NANDFLASH一个PAGE中的前7个字节
                  前7个字节为日志的日期时间
  Calls:
  Called By:
  Input:          
                  uiPage: 物理PAGE地址 
                  pucBuf:   
  Output:     
  Return:         
  Others:
*************************************************/
void NFReadLogDateTime(UINT32 uiPage, UCHAR *pucBuf)
{
    UINT32 i;

    NF_WRITE_CMD(NF_CMD_READ0);
    NF_WRITE_ADDR(0);
    NF_WRITE_ADDR(uiPage & 0xff);
    NF_WRITE_ADDR((uiPage >> 8) & 0xff);

    if(NFReady()==FALSE)
    {
        return;
    }
    /* read to buffer */
    for ( i = 0; i < 7; i++)
    {
        pucBuf[i] = RD_ADDR;
    }

}
