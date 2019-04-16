/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   BootMain.c
    作者:     章杰
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  2006/09/28
       作者:  章杰
       描述:  编码完成
    2. 日期:  2006/11/07
       作者:  章杰
       描述:  修改问题MCM-22，减少测试CPLD每个步骤的等待时间
    3. 日期:  2006/11/30
       作者:  章杰
       描述:  修改问题MCM-44，增加对文件编译时间的打印
    4. 日期:  2007/01/31
       作者:  钟华文
       描述:  修改问题MCM-57，修改对文件下载失败写FLASH失败的提示
    5. 日期:  2007/02/06
       作者:  钟华文
       描述:  修改问题MCM-59，
              修改调用读取串口数据函数所使用的缓冲区大小，使其与
              串口接收缓冲区大小一致，从而防止读取函数没有判断传
              入的缓冲大小而产生的内存访问越界
---------------------------------------------------------------------------*/
#include "UserType.h"
#include "At91M55800.h"
#include "MyBoard.h"
#include "boot.h"
#include "../periph/pio/Pio.h"
#include "../periph/usart/Usart.h"
#include "../periph/flash/Flash.h"
#include "../periph/nandflash/NandFlash.h"

#define  ENTER_CRITICAL()  (cpu_sr = CPUSaveSR())    /* Disable interrupts   */                     
#define  EXIT_CRITICAL()   (CPURestoreSR(cpu_sr))    /* Restore  interrupts */

#define FILE_BLOCK_BUFF_SIZE (8*1024)
#define FILE_REC_FINISH 2

//MCM-57_20070131_Zhonghw_begin
#define FILE_WRITE_FAILED 3
//MCM-57_20070131_Zhonghw_end

#define SELECT_NO 0
#define SELECT_DOWNLOAD_P_1 '1'
#define SELECT_DOWNLOAD_P_2 '2'
#define SELECT_RUN_P_1 '3'
#define SELECT_RUN_P_2 '4'
#define SELECT_RUN_TEST_CPLD '5'

#define PRIVATE_START_CHAR '#'
#define PRIVATE_END_CHAR '*'

#define FILEBLOCK_START_CHAR '#'
#define FILEBLOCK_END_CHAR '*'


#define DATAPACK_STATE_START   2
#define DATAPACK_STATE_FINISH  1
#define DATAPACK_STATE_NO         0

//厂家协议的接收缓冲长度
#define PRIVATE_DATA_PACK_SIZE   (6*1024)
//厂家协议接收最小包，含首尾字符
#define PRIVATE_DATA_MINSIZE  11


//厂家协议转义字符列表长度
#define PRIVATE_ESCAPE_NUM  3
//厂家协议转义字符
#define PRIVATE_ESCAPE_CHAR  '^'

//无输入延迟时间(0x10000 = about 1 sec)
#define NO_INPUT_TIMEOUT 0x60000

#define FIND_NO_CHAR 0xFFFFFFF0

#define FILE_REC_TEMP_ADDR ((UCHAR*)0x2000000)

//被选择的串口
#define SELECT_USART  g_stUsart0

//测试CPLD的宏定义
//衰耗器(输出)
#define ATT1    PB0
#define ATT2    PB1
#define ATT4    PB2
#define ATT8    PB3
#define ATT16   PB4
#define ATT_OUT_MASK (ATT1|ATT2|ATT4|ATT8|ATT16)
#define ATT_PIO_CTRL g_stPioB

//(输出控制)
#define OUT_ENA PB5
#define S1              PB6
#define S2              PB7
#define ATT_CTRL_MASK (OUT_ENA|S1|S2)

//数字量采集模块(输入)
#define GET_DA0 PB8
#define GET_DA1 PB9
#define GET_DA2 PB10
#define GET_DA3 PB11
#define GET_DA4 PB12
#define GET_DA5 PB13
#define GET_DA6 PB14
#define GET_DA7 PB15
#define DA_IN_MASK (GET_DA0|GET_DA1|GET_DA2|GET_DA3|GET_DA4|GET_DA5|GET_DA6|GET_DA7)
#define DA_PIO_CTRL g_stPioB

//选频器(输出控制)
#define FR_CE   PA2
#define FR_S3   PA3
#define FR_S2   PA4
#define FR_S1   PA5
#define FR_S0   PA6
#define FR_CK   PA7
#define FR_DA   PA8
#define FR_CTRL_MASK (FR_CE|FR_S3|FR_S2|FR_S1|FR_S0|FR_CK|FR_DA)
#define FR_PIO_CTRL g_stPioA

//CPLD状态迁移时间
#define CPLD_STA_LONG_TIME 600
//MCM-22_20061107_zhangjie_begin
#define CPLD_STA_MID_TIME 300
#define CPLD_STA_SHORT_TIME 60
//MCM-22_20061107_zhangjie_end

//数据区为aucDataPack[0]=< ... <aucDataPack[uiInPos]
typedef struct
{
    UINT32 uiState;
    UINT32 uiInPos;
    UCHAR aucDataPack[PRIVATE_DATA_PACK_SIZE];
}DATA_PACK_ST;

DATA_PACK_ST g_stPrivateDataPack;

//厂家协议已转义字符列表
UCHAR g_aucPEscaped[] =
{
   0x01,
   0x02,
   0x03,
};


//厂家协议未转义字符(即需要转义)列表
UCHAR g_aucPUnEscape[] =
{
   '^',
   '#',
   '*',
};

static FILE_PART_DESC_ST g_astFilePartTbl[FPD_COUNT];


UCHAR aucStr0[] = "\n\r\n\rPlease select :\n\r"
                 "1.Download file to program 1.\n\r"
                 "2.Download file to program 2.\n\r"
                 "3.Run program 1.\n\r"
                 "4.Run program 2.\n\r" 
                 "5.Run Test CPLD program.\n\r";
UCHAR aucStr1[] = "\n\rPlease input file(Download to program 1!):\n\rNotice!Disconnect Modem!!!\n\r";
UCHAR aucStr2[] = "\n\rDownload file to program 1 finish!\n\r";
UCHAR aucStr3[] = "\n\rProgram 1 is running...\n\r";
UCHAR aucStr4[] = "\n\rProgram 2 is running...\n\r";
UCHAR aucStr5[] = "\n\rPlease input file(Download to program 2!):\n\rNotice!Disconnect Modem!!!\n\r";
UCHAR aucStr6[] = "\n\rDownload file to program 2 finish!\n\r";
UCHAR aucStr7[] = "\n\rStart running program 1...\n\r";
UCHAR aucStr8[] = "\n\rStart running program 2...\n\r";
UCHAR aucStr9[] = "\n\rNo program can run!\n\r";
UCHAR aucStr10[] = "\n\rNandFlash OK\n\r";
UCHAR aucStr11[] = "\n\rStart running test CPLD program...\n\r";
UCHAR aucStr12[] = "\n\rTest CPLD program finish!Test CPLD again(y/n)?\n\r";

//MCM-57_20070131_Zhonghw_begin
UCHAR aucStr13[] = "\n\rDownload file to program 1 failed!!!\n\r";
UCHAR aucStr14[] = "\n\rDownload file to program 2 failed!!!\n\r";
//MCM-57_20070131_Zhonghw_end

//函数申明
extern void WdOpen(void);
extern void WdClose(void);

//以下封装FLASH的操作函数，使调用更方便
void SimpleFlashRead(ULONG ulFlashAddr, void *pvData, ULONG ulSize)
{
    FlashRead(g_stNorFlash0.pfwBaseAddr, (UCHAR *)ulFlashAddr, (UCHAR *)pvData, ulSize);
}
LONG SimpleFlashWrite(ULONG ulFlashAddr, void *pvData, ULONG ulSize)
{
    return FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR *)ulFlashAddr, (UCHAR *)pvData, ulSize);
}

LONG SimpleFlashEraseSector(ULONG ulFlashAddr)
{
    return FlashEraseSector(g_stNorFlash0.pfwBaseAddr, (UCHAR *)ulFlashAddr);
}

/*************************************************
  Function:
  Description:    本函数用于比较FLASH中的数据块和RAM中的
                  数据块是否一致，主要用于在写FLASH之前进
                  行判断，如果两者一致则无需写FLASH
  Calls:
  Called By:
  Input:          ulFlashAddr: FLASH的起始地址
                  pvData:      RAM中的起始地址
                  ulSize:      数据块长度
  Output:         
  Return:         SUCCEEDED:   两者一致
                  FAILED:      两者不一致
  Others:         
*************************************************/
LONG CmpData(ULONG ulFlashAddr, void *pvData, ULONG ulSize)
{
    ULONG i = 0;
    UCHAR ucBuf = 0;

    for(i = 0; i < ulSize; i++)
    {
        SimpleFlashRead(ulFlashAddr + i, &ucBuf, 1);
        if(ucBuf != *((UCHAR *)pvData + i))
        {
            return FAILED;
        }        
    }

    return SUCCEEDED;
}

void SaveFileParam()
{
    ULONG ulFileParamSectorAddr;
    ulFileParamSectorAddr = (FILE_PART_TBL_ADDR/g_stNorFlash0.uiSectorSize) * (g_stNorFlash0.uiSectorSize);

    if(CmpData(FILE_PART_TBL_ADDR, &g_astFilePartTbl, sizeof(g_astFilePartTbl)) != SUCCEEDED)
    {
        UCHAR aucTemp[g_stNorFlash0.uiSectorSize];
        SimpleFlashRead(ulFileParamSectorAddr, aucTemp, g_stNorFlash0.uiSectorSize);
        if(SimpleFlashEraseSector(ulFileParamSectorAddr) != SUCCEEDED)
        {
            return;
        }
        memcpy((aucTemp + FILE_PART_TBL_ADDR%g_stNorFlash0.uiSectorSize),&g_astFilePartTbl,sizeof(g_astFilePartTbl));
        SimpleFlashWrite(ulFileParamSectorAddr, aucTemp, g_stNorFlash0.uiSectorSize);
    }
    return; //内容不变则直接返回
}

OutputMenu(USART_DEV_ST* pstUsart,UCHAR* pucData, UINT32 uiSize)
{
    UINT32  cpu_sr;
    ENTER_CRITICAL();
    UsartWrite(pstUsart,pucData,uiSize);
    EXIT_CRITICAL();
    while(pstUsart->pstData->pucTxIn!=pstUsart->pstData->pucTxOut);
}


void Delay(UINT32 uiTime) 
{
    UINT32 i;
    for(i=0;i<uiTime*1000;i++);
}

//灯闪烁
UCHAR g_ucLedSta=0;
void LedFlick(void) 
{
        if(g_ucLedSta == 0)
        {
            PioWrite(&LED_PIO_CTRL,LED_MASK,LED_OFF);
                g_ucLedSta = 1;
        }
        else
        {
            PioWrite(&LED_PIO_CTRL,LED_MASK,LED_ON);
                g_ucLedSta = 0;
        }
}

//***************查找单个字符***************
//输入：
//要查找的字符串pucStr，
//要查找第uiNmum个匹配字符ucTarget的位置
//查找的最大长度uiMaxlength
//输出：要查找字符的位置：从0开始计数，如果没有找到则返回-1.
INT32 FindChar(UCHAR* pucStr, UCHAR ucTarget, UINT32 uiNum, UINT32 uiMaxlength)
{
    UINT32 i;
    if((uiMaxlength == 0)||(uiNum== 0))
    {
        return FIND_NO_CHAR;    
    }
    for(i=0; i<uiMaxlength; i++)
    {
        if(pucStr[i] == ucTarget)
        {
            uiNum = uiNum - 1;
            if (uiNum == 0)
            {
                return i;
            }
        }           
    }    
    return FIND_NO_CHAR;
}


void CopyStr(UCHAR* pdes, UCHAR* pSrc, UINT32 uiLength)
{
    UINT32 i;
    if(uiLength == 0)
    {
        return;    
    }   
    for(i=0; i<uiLength; i++)
    {
        pdes[i] = pSrc[i];
    }
}

UINT32 FileBlockHandle(UCHAR *pucStart, UINT32 uiCount,FILE_PART_DESC_ST *pstFileDesc )
{
    UINT32 i;
    static UINT32 uiFileLen = 0;//用于传输文件时计数   

    if(uiCount <= PRIVATE_DATA_MINSIZE)
    {
        return FAILED;
    }
    //转义crc
    //首尾字符不需转义
    pucStart++;
    uiCount = uiCount-2;
    //转义
    uiCount = DecodeEscape(pucStart, uiCount, PRIVATE_ESCAPE_CHAR, 
            &g_aucPEscaped, &g_aucPUnEscape, PRIVATE_ESCAPE_NUM);

    //对转义失败的判断  
    if(uiCount == 0)
    {
        return FAILED;
    }

    USHORT usPackCount = *((USHORT*)pucStart);
    USHORT usPackNum = *((USHORT*)(pucStart+2));

    //调整指针和长度
    pucStart = pucStart + 7;
    uiCount = uiCount -9;

    //写文件至RAM
    for(i=0; i<uiCount; i++)
    {
        *(FILE_REC_TEMP_ADDR + uiFileLen + i) = pucStart[i]; 
    }
    uiFileLen = uiFileLen + uiCount;

    //test
    if(usPackCount == usPackNum)
    {
        //文件结束写FLASH
        UINT32 uiSectorSize = g_stNorFlash0.uiSectorSize;        
        UINT32 uiSectorCount = uiFileLen/uiSectorSize + 1;
        FLASHWORD* pfwBaseAddr = g_stNorFlash0.pfwBaseAddr;
        //UCHAR *pucFlashAddr = (UCHAR*)(pstFileDesc->ulFileAddr);
        UCHAR *pucFlashAddr;
        (UINT32)pucFlashAddr = (*pstFileDesc).ulFileAddr;
        //写文件分区信息
        (*pstFileDesc).ulFileSize = uiFileLen;
        memcpy((UCHAR *)pstFileDesc,(UCHAR *)FILE_REC_TEMP_ADDR,FPD_FILE_NAME_LEN+TIME_STAMP_LEN+FILE_VERSION_LEN);

        //MCM-57_20070131_Zhonghw_begin
        //写文件至FLASH
        if(uiFileLen>0)
        {
            for(i=0; i<uiSectorCount; i++)
            {
                if(FlashEraseSector(pfwBaseAddr,pucFlashAddr+i*uiSectorSize) != SUCCEEDED)
                {
                    uiFileLen=0;
                    WdClose();
                    return FILE_WRITE_FAILED;
                }

                if(FlashWrite(pfwBaseAddr, pucFlashAddr+i*uiSectorSize,
                              FILE_REC_TEMP_ADDR+i*uiSectorSize, uiSectorSize) !=SUCCEEDED)
                {
                    uiFileLen=0;
                    WdClose();
                    return FILE_WRITE_FAILED;
                }
            }
            uiFileLen=0;
        }
        //MCM-57_20070131_Zhonghw_end     

        //写分区信息至FLASH
        SaveFileParam();

        //文件下载完毕则关闭看门狗
        WdClose();
        return(FILE_REC_FINISH);
    }
/*    

    ENTER_CRITICAL();
    UsartWrite(&SELECT_USART,pucStart,uiCount);
    EXIT_CRITICAL();
    while(SELECT_USART.pstData->pucTxIn!=SELECT_USART.pstData->pucTxOut);            
*/
    return(SUCCEEDED);
}


UINT32 ReadSelect(void)
{
    UINT32  cpu_sr;
    UINT32 uiCount;
    //MCM-59_20070206_Zhonghw_begin
    //notice 为串口读取开辟的内存大小应不比串口数据接收缓冲区小
    UCHAR ucRec[USART_SIZE_BUFFER];        //USART_SIZE_BUFFER
    //MCM-59_20070206_Zhonghw_end
    
    if(UsartReadReady ( &SELECT_USART ))
    {
        //保证只有一个串口数据
        Delay(600);
        ENTER_CRITICAL();
        uiCount = UsartRead ( &SELECT_USART, ucRec );
        EXIT_CRITICAL();  
        if(1 == uiCount) 
        {
            return(ucRec[0]);
        }
    }
    return SELECT_NO;
}

/*************************************************
  Function:
  Description:    本函数解转义
  Calls:
  Called By:
  Input:          pucData:   待转义的数据
                  uiLen: 待转义数据的长度
                  ucEscapeChar: 转义字符
                  pucEscaped: 被转义后的字符列表
                  pucUnEscaped: 解转义后的字符列表，与pucEscaped对应
                  uiEscapeNum: 转义字符列表的长度                 
  Output:
  Return:         0:   失败
                  非0: 解转义后的实际长度
  Others:
*************************************************/
UINT32 DecodeEscape(UCHAR* pucData, UINT32 uiLen, UCHAR ucEscapeChar,
        UCHAR* pucEscaped, UCHAR* pucUnEscaped, UINT32 uiEscapeNum)
{
    UINT32 i, j;
    UINT32 uiRetLen = 0;

    for(i=0; i < uiLen; i++)
    {
        if(pucData[i] == ucEscapeChar)
        {
            i++;
            for(j=0; j<uiEscapeNum; j++)
            {
                if(pucData[i] == pucEscaped[j])
                {
                    pucData[uiRetLen] = pucUnEscaped[j];
                    break;
                }
            }
            if(j>=uiEscapeNum)
            {
                return 0;
            }
        }
        else
        {
            pucData[uiRetLen] = pucData[i];
        }
        uiRetLen++;
    }
    return uiRetLen;
}


UINT32 DownloadFile(FILE_PART_DESC_ST *pstFileDesc)
{
    UCHAR aucRec[PRIVATE_DATA_PACK_SIZE];
    UINT32 cpu_sr,uiCount,uiEndPos=0,uiStartPos=0;

    g_stPrivateDataPack.uiInPos = 0;
    g_stPrivateDataPack.uiState= DATAPACK_STATE_NO;

    //打开看门狗，防止无任何数据输入
    WdOpen();

    //不断读串口是否有数据
    for(;;)
    {
        if(UsartReadReady ( &SELECT_USART ))
        {
            //有数据就喂狗
            WdRestart();
            ENTER_CRITICAL();
            uiCount = UsartRead ( &SELECT_USART, aucRec);
            EXIT_CRITICAL();
            //对收到的数据包进行处理直到处理完毕
            for(;;)
            {
                uiStartPos = FindChar(aucRec, FILEBLOCK_START_CHAR, 1, uiCount);
                uiEndPos = FindChar(aucRec, FILEBLOCK_END_CHAR, 1, uiCount);
                if(g_stPrivateDataPack.uiState == DATAPACK_STATE_NO)
                {
                    g_stPrivateDataPack.uiInPos = 0;
                    //属于NO S NO E
                    if((uiStartPos == FIND_NO_CHAR) && (uiEndPos == FIND_NO_CHAR))
                    {                    
                        //继续读串口数据
                        break;
                    }
                    //属于NO S ONLY E
                    else if((uiStartPos == FIND_NO_CHAR) && (uiEndPos != FIND_NO_CHAR))
                    {
                        //继续读串口数据               
                        break;
                    }
                    //属于ONLY S NO E
                    else if((uiStartPos != FIND_NO_CHAR) && (uiEndPos == FIND_NO_CHAR))
                    {
                        CopyStr(g_stPrivateDataPack.aucDataPack, aucRec, uiCount);
                        g_stPrivateDataPack.uiInPos = g_stPrivateDataPack.uiInPos + uiCount;
                        g_stPrivateDataPack.uiState = DATAPACK_STATE_START;
                        break;
                    }
                    //uiStartPos != FIND_NO_CHAR && uiEndPos != FIND_NO_CHAR
                    //属于S E                       
                    else if(uiStartPos < uiEndPos)
                    {
                        CopyStr(g_stPrivateDataPack.aucDataPack, 
                                aucRec+uiStartPos, uiEndPos - uiStartPos +1);
                        g_stPrivateDataPack.uiInPos = uiEndPos - uiStartPos +1;
                        
                        //处理完整包
                        //MCM-57_20070131_Zhonghw_begin
                        ULONG ulResult = 0;
                        ulResult = FileBlockHandle(g_stPrivateDataPack.aucDataPack, 
                                g_stPrivateDataPack.uiInPos,pstFileDesc);

                        if(ulResult == FILE_REC_FINISH)
                        {
                            return SUCCEEDED;                        
                        }
                        else if(ulResult == FILE_WRITE_FAILED)
                        {
                            return FILE_WRITE_FAILED;
                        }
                        //MCM-57_20070131_Zhonghw_end

                        g_stPrivateDataPack.uiState = DATAPACK_STATE_NO;
                        //调整剩余包长度                    
                        uiCount = uiCount - uiEndPos - 1;
                        //读出剩余包数据                    
                        CopyStr(aucRec, aucRec+uiEndPos+1, uiCount);
                        //继续处理剩余数据包
                        continue;
                    }
                    //uiStartPos后面的包为有效包
                    //属于E  S
                    else if(uiStartPos > uiEndPos)
                    {
                        //前面收到的包即uiStartPos以前的包无效
                        //g_stPrivateDataPack.uiInPos = 0;
                        //g_stPrivateDataPack.uiState = DATAPACK_STATE_NO;

                        //调整剩余包长度                    
                        uiCount = uiCount - uiStartPos;
                        //读出剩余包数据                    
                        CopyStr(aucRec, aucRec+uiStartPos, uiCount);
                        //继续处理剩余数据包
                        continue;
                    }
                    //无效的包
                    //属于S E但数据包长度太小
                    else
                    {
                        //前面收到的包无效
                        g_stPrivateDataPack.uiInPos = 0;
                        g_stPrivateDataPack.uiState = DATAPACK_STATE_NO;
                        //继续读串口数据               
                        break;
                    }
                }//g_stPrivateDataPack.uiState == DATAPACK_STATE_NO     
                if(g_stPrivateDataPack.uiState == DATAPACK_STATE_START)
                {
                    //属于ONLY S NO E
                    if((uiStartPos == FIND_NO_CHAR) && (uiEndPos == FIND_NO_CHAR))
                    {
                        CopyStr(g_stPrivateDataPack.aucDataPack+g_stPrivateDataPack.uiInPos, 
                                aucRec, uiCount);
                        g_stPrivateDataPack.uiInPos = g_stPrivateDataPack.uiInPos + uiCount;
                        //继续读串口数据
                        break;
                    }
                    //属于S E
                    else if((uiStartPos == FIND_NO_CHAR) && (uiEndPos != FIND_NO_CHAR))
                    {
                        CopyStr(g_stPrivateDataPack.aucDataPack + g_stPrivateDataPack.uiInPos,
                                aucRec, uiEndPos + 1);
                        g_stPrivateDataPack.uiInPos = g_stPrivateDataPack.uiInPos + uiEndPos + 1;
                        
                        //处理完整包
                        //MCM-57_20070131_Zhonghw_begin                        
                        ULONG ulResult = 0;
                        ulResult = FileBlockHandle(g_stPrivateDataPack.aucDataPack, 
                                g_stPrivateDataPack.uiInPos,pstFileDesc);

                        if(ulResult == FILE_REC_FINISH)
                        {
                            return SUCCEEDED;                        
                        }
                        else if(ulResult == FILE_WRITE_FAILED)
                        {
                            return FILE_WRITE_FAILED;
                        }                                               
                        //MCM-57_20070131_Zhonghw_end

                        g_stPrivateDataPack.uiState = DATAPACK_STATE_NO;
                        //调整剩余包长度                    
                        uiCount = uiCount - uiEndPos - 1;
                        //读出剩余包数据                    
                        CopyStr(aucRec, aucRec+uiEndPos+1, uiCount);
                        //继续处理剩余数据包
                        continue;
                    }
                    //属于S S
                    else if((uiStartPos != FIND_NO_CHAR) && (uiEndPos == FIND_NO_CHAR))
                    {
                        //前面收到的包S S无效
                        g_stPrivateDataPack.uiInPos = 0;
                        g_stPrivateDataPack.uiState = DATAPACK_STATE_NO;
                        //调整剩余包长度                    
                        uiCount = uiCount - uiStartPos;
                        //读出剩余包数据                    
                        CopyStr(aucRec, aucRec+uiStartPos, uiCount);
                        //继续处理剩余数据包
                        continue;
                    }
                    //uiStartPos != FIND_NO_CHAR && uiEndPos != FIND_NO_CHAR
                    //属于S S E                    
                    else if(uiStartPos < uiEndPos)
                    {
                        //前面收到的包S S无效
                        g_stPrivateDataPack.uiInPos = 0;
                        g_stPrivateDataPack.uiState = DATAPACK_STATE_NO;

                        //S E有效
                        CopyStr(g_stPrivateDataPack.aucDataPack, 
                                aucRec+uiStartPos, uiEndPos - uiStartPos +1);
                        g_stPrivateDataPack.uiInPos = g_stPrivateDataPack.uiInPos 
                                + uiEndPos - uiStartPos +1;
                        
                        //处理完整包
                        //MCM-57_20070131_Zhonghw_begin
                        ULONG ulResult = 0;
                        ulResult = FileBlockHandle(g_stPrivateDataPack.aucDataPack, 
                                g_stPrivateDataPack.uiInPos,pstFileDesc);

                        if(ulResult == FILE_REC_FINISH)
                        {
                            return SUCCEEDED;                        
                        }
                        else if(ulResult == FILE_WRITE_FAILED)
                        {
                            return FILE_WRITE_FAILED;
                        }                                               
                        //MCM-57_20070131_Zhonghw_end
                        
                        g_stPrivateDataPack.uiState = DATAPACK_STATE_NO;
                        //调整剩余包长度                    
                        uiCount = uiCount - uiEndPos - 1;
                        //读出剩余包数据                    
                        CopyStr(aucRec, aucRec+uiEndPos+1, uiCount);
                        //继续处理剩余数据包
                        continue;
                    }
                    //属于S E S
                    //uiEndPos前面的包S E为有效包
                    else if(uiStartPos > uiEndPos)
                    {
                        CopyStr(g_stPrivateDataPack.aucDataPack+g_stPrivateDataPack.uiInPos, 
                                aucRec, uiEndPos +1);
                        //E S直接为无效数据包
                        //调整剩余包长度                    
                        uiCount = uiCount - uiStartPos;
                        //读出剩余包数据                    
                        CopyStr(aucRec, aucRec+uiStartPos, uiCount);
                        //继续处理剩余数据包
                        continue;
                    }
                    //属于S S E但数据包长度太小
                    else
                    {
                        g_stPrivateDataPack.uiInPos = 0;
                        g_stPrivateDataPack.uiState = DATAPACK_STATE_NO; 
                        //继续读串口数据               
                        break;
                    }
                }//g_stPrivateDataPack.uiState == DATAPACK_STATE_START                  
            }//for(;;)对收到的数据包进行处理直到处理完毕
        }//UsartReadReady
    }//for(;;) 不断读串口是否有数据
}

void FilePartInit(void)
{
    //读出文件分区信息
    FlashRead(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
            (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));

    //已初始化则不用再初始化
    //if((g_astFilePartTbl[0].ulFileAddr == PROGRAM_PART_1_ADDR) &&
        //    (g_astFilePartTbl[1].ulFileAddr == PROGRAM_PART_2_ADDR))
    //{
        //return;
    //}

    //初始化文件分区信息
    g_astFilePartTbl[0].ulFileAddr = PROGRAM_PART_1_ADDR;
    g_astFilePartTbl[1].ulFileAddr = PROGRAM_PART_2_ADDR;

    //初始化文件状态，目前为调试模式
    //初始化后只有如下状态
    //INVALID, RUNNABLE, VALID, UNRUNNABLE 
    //只有在VALID被选择运行时，才出现UPGRADING
    if(g_astFilePartTbl[0].ucStatus == PROGRAM_STATUS_UPGRADE)
    {
        g_astFilePartTbl[0].ucStatus = PROGRAM_STATUS_UNRUNNABLE;
    }
    else if(g_astFilePartTbl[0].ucStatus > PROGRAM_STATUS_UNRUNNABLE)
    {
        g_astFilePartTbl[0].ucStatus = PROGRAM_STATUS_INVALID;
    }
    /*
    switch(g_astFilePartTbl[0].ucStatus)
    {
    case PROGRAM_STATUS_INVALID:
        break;
    case PROGRAM_STATUS_RUNNABLE:
        break;
    case PROGRAM_STATUS_VALID:
        break;
    case PROGRAM_STATUS_UPGRADE:
        g_astFilePartTbl[0].ucStatus = PROGRAM_STATUS_UNRUNNABLE;
        break;
    case PROGRAM_STATUS_UNRUNNABLE:
        break;
    default:
        g_astFilePartTbl[0].ucStatus = PROGRAM_STATUS_INVALID;
        break;       
    }
    */

    //初始化文件状态，目前为调试模式
    //初始化后只有如下状态
    //INVALID, RUNNABLE, VALID, UNRUNNABLE 
    //只有在VALID被选择运行时，才出现UPGRADING
    if(g_astFilePartTbl[1].ucStatus == PROGRAM_STATUS_UPGRADE)
    {
        g_astFilePartTbl[1].ucStatus = PROGRAM_STATUS_UNRUNNABLE;
    }
    else if(g_astFilePartTbl[1].ucStatus > PROGRAM_STATUS_UNRUNNABLE)
    {
        g_astFilePartTbl[1].ucStatus = PROGRAM_STATUS_INVALID;
    }
    /*
    switch(g_astFilePartTbl[1].ucStatus)
    {
    case PROGRAM_STATUS_INVALID:
        break;
    case PROGRAM_STATUS_RUNNABLE:
        break;
    case PROGRAM_STATUS_VALID:
        break;
    case PROGRAM_STATUS_UPGRADE:
        g_astFilePartTbl[1].ucStatus = PROGRAM_STATUS_UNRUNNABLE;
        break;
    case PROGRAM_STATUS_UNRUNNABLE:
        break;
    default:
        g_astFilePartTbl[1].ucStatus = PROGRAM_STATUS_INVALID;
        break;       
    }
    */

    //写分区信息至FLASH         
    //test
    //FlashEraseSector(g_stNorFlash0.pfwBaseAddr, 
    //       (UCHAR*)FILE_PART_TBL_ADDR);
    //FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
    //       (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));       

}

//无输入(键盘或者串口)超时后调用
//初始化后只有如下状态
//INVALID, RUNNABLE, VALID, UNRUNNABLE 
void SelectProgramRun(void)
{
    UCHAR ucP1Status = g_astFilePartTbl[0].ucStatus;
    UCHAR ucP2Status = g_astFilePartTbl[1].ucStatus;
    UCHAR ucSelectP = 0;//选择程序1，2；0表示异常
    ULONG cpu_sr;

    //状态相同，直接break的为异常
    if(ucP1Status == ucP2Status)
    {
        //选择新的程序，如果都是新的则选择1
        //如果都是旧的则选择2
        if(ucP1Status == PROGRAM_STATUS_RUNNABLE)
        {
            if(g_astFilePartTbl[0].ucNew == FILE_NEW)
            {
                ucSelectP = 1;
                g_astFilePartTbl[1].ucNew = FILE_OLD;
            }
            else
            {
                ucSelectP = 2;
                g_astFilePartTbl[1].ucNew = FILE_NEW;
            }
        }
        //选择新的程序，如果都是新的则选择1
        //如果都是旧的则选择2        
        else if(ucP1Status == PROGRAM_STATUS_VALID)
        {
            if(g_astFilePartTbl[0].ucNew == FILE_NEW)
            {
                ucSelectP = 1;
                g_astFilePartTbl[1].ucNew = FILE_OLD;                
                g_astFilePartTbl[0].ucStatus= PROGRAM_STATUS_UPGRADE;                
            }
            else
            {
                ucSelectP = 2;
                g_astFilePartTbl[1].ucNew = FILE_NEW;
                g_astFilePartTbl[1].ucStatus= PROGRAM_STATUS_UPGRADE;
            }
        }
        else
        {
            //do nothing
        }            
    }//if(ucP1Status == ucP2Status)
    
    //状态不同
    //VALID优先级最高
    else if(ucP1Status == PROGRAM_STATUS_VALID)
    {
        ucSelectP = 1;
        g_astFilePartTbl[0].ucStatus = PROGRAM_STATUS_UPGRADE;
    }
    else if(ucP2Status == PROGRAM_STATUS_VALID)
    {
        ucSelectP = 2;
        g_astFilePartTbl[1].ucStatus = PROGRAM_STATUS_UPGRADE;
    }

    //RUNNABLE优先级次高
    else if(ucP1Status == PROGRAM_STATUS_RUNNABLE)
    {
        ucSelectP = 1;
    }
    else if(ucP2Status == PROGRAM_STATUS_RUNNABLE)
    {
        ucSelectP = 2;
    }
    //两个程序中既没有VALID，也没有RUNNABLE，异常
    else
    {
        ucSelectP = 0;
    }

    //写分区信息至FLASH         
    //FlashEraseSector(g_stNorFlash0.pfwBaseAddr, 
        //    (UCHAR*)FILE_PART_TBL_ADDR);
    //FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
        //    (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));

    //选择程序运行
    if(ucSelectP == 1)
    {
        OutputMenu(&SELECT_USART, (UCHAR*)aucStr7, sizeof(aucStr7));

    //写分区信息至FLASH
/*
    FlashEraseSector(g_stNorFlash0.pfwBaseAddr, 
            (UCHAR*)FILE_PART_TBL_ADDR);
    FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
            (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));
*/

        g_astFilePartTbl[0].ucIsRunning = 1;
        g_astFilePartTbl[1].ucIsRunning = 0;
        
        SaveFileParam();

        WdOpen();

        //读程序至RAM运行区,偏移128字节，前128自己为文件信息
        FlashRead(g_stNorFlash0.pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[0].ulFileAddr+128),
                (UCHAR*)(P_RUN_ADDR),g_astFilePartTbl[0].ulFileSize-128);

        
        ENTER_CRITICAL();

        JMPTORAM();        
    }
    else if(ucSelectP == 2)
    {
        OutputMenu(&SELECT_USART, (UCHAR*)aucStr8, sizeof(aucStr8));

    //写分区信息至FLASH
/*
    FlashEraseSector(g_stNorFlash0.pfwBaseAddr, 
            (UCHAR*)FILE_PART_TBL_ADDR);
    FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
            (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));
*/
        g_astFilePartTbl[0].ucIsRunning = 0;
        g_astFilePartTbl[1].ucIsRunning = 1;
        
        SaveFileParam();

        WdOpen();

        //读程序至RAM运行区,偏移128字节，前128自己为文件信息
        FlashRead(g_stNorFlash0.pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[1].ulFileAddr+128),
                (UCHAR*)(P_RUN_ADDR),g_astFilePartTbl[1].ulFileSize-128);

        ENTER_CRITICAL();
        JMPTORAM();
    }
    //进入异常处理
    else
    {
        OutputMenu(&SELECT_USART, (UCHAR*)aucStr9, sizeof(aucStr9));    
    }
}

//调试模式下对被选择程序状态的处理
void PStaHndlInDebug(UCHAR ucSelectP)
{
}


//MCM-44_20061130_zhangjie_begin
void OutputFileInfo(void)
{
#define BUF_SIZE 100
    UINT32 uiCount;
    CHAR aucBuff[BUF_SIZE];

    //信息头
    memset(aucBuff, 0, BUF_SIZE);
    uiCount = sprintf(aucBuff,"\n\rFile Information(P1=Program 1;  P2=Program 2):");
    OutputMenu(&SELECT_USART,aucBuff,uiCount);

    //文件名
    g_astFilePartTbl[0].acFileName[FPD_FILE_NAME_LEN-1] = 0;
    g_astFilePartTbl[1].acFileName[FPD_FILE_NAME_LEN-1] = 0;
    memset(aucBuff, 0, BUF_SIZE);
    uiCount = sprintf(aucBuff,"\n\rP1Name:%s  P2Name:%s  ",
            (g_astFilePartTbl[0].acFileName),(g_astFilePartTbl[1].acFileName));
    OutputMenu(&SELECT_USART,aucBuff,uiCount);

    //文件版本
    g_astFilePartTbl[0].acVersion[FPD_FILE_NAME_LEN-1] = 0;
    g_astFilePartTbl[1].acVersion[FPD_FILE_NAME_LEN-1] = 0;
    memset(aucBuff, 0, BUF_SIZE);
    uiCount = sprintf(aucBuff,"\n\rP1Version:%s  P2Version:%s  ",
            (g_astFilePartTbl[0].acVersion),(g_astFilePartTbl[1].acVersion));
    OutputMenu(&SELECT_USART,aucBuff,uiCount);

    //大小
    memset(aucBuff, 0, BUF_SIZE);
    uiCount = sprintf(aucBuff,"\n\rP1Size=%dBytes  P2Size=%dBytes  ",
            (g_astFilePartTbl[0].ulFileSize),(g_astFilePartTbl[1].ulFileSize));
    OutputMenu(&SELECT_USART,aucBuff,uiCount);

    //时间
    memset(aucBuff, 0, BUF_SIZE);
    uiCount = sprintf(aucBuff,"\n\rP1 Built on %d%d%d%d.%d%d.%d%d %d%d:%d%d:%d%d P2 Built on %d%d%d%d.%d%d.%d%d %d%d:%d%d:%d%d\n\r",
            g_astFilePartTbl[0].aucTimeStamp[0] / 16,g_astFilePartTbl[0].aucTimeStamp[0] % 16,
            g_astFilePartTbl[0].aucTimeStamp[1] / 16,g_astFilePartTbl[0].aucTimeStamp[1] % 16,
            g_astFilePartTbl[0].aucTimeStamp[2] / 16,g_astFilePartTbl[0].aucTimeStamp[2] % 16,
            g_astFilePartTbl[0].aucTimeStamp[3] / 16,g_astFilePartTbl[0].aucTimeStamp[3] % 16,
            g_astFilePartTbl[0].aucTimeStamp[4] / 16,g_astFilePartTbl[0].aucTimeStamp[4] % 16,
            g_astFilePartTbl[0].aucTimeStamp[5] / 16,g_astFilePartTbl[0].aucTimeStamp[5] % 16,
            g_astFilePartTbl[0].aucTimeStamp[6] / 16,g_astFilePartTbl[0].aucTimeStamp[6] % 16,

            g_astFilePartTbl[1].aucTimeStamp[0] / 16,g_astFilePartTbl[1].aucTimeStamp[0] % 16,
            g_astFilePartTbl[1].aucTimeStamp[1] / 16,g_astFilePartTbl[1].aucTimeStamp[1] % 16,
            g_astFilePartTbl[1].aucTimeStamp[2] / 16,g_astFilePartTbl[1].aucTimeStamp[2] % 16,
            g_astFilePartTbl[1].aucTimeStamp[3] / 16,g_astFilePartTbl[1].aucTimeStamp[3] % 16,
            g_astFilePartTbl[1].aucTimeStamp[4] / 16,g_astFilePartTbl[1].aucTimeStamp[4] % 16,
            g_astFilePartTbl[1].aucTimeStamp[5] / 16,g_astFilePartTbl[1].aucTimeStamp[5] % 16,
            g_astFilePartTbl[1].aucTimeStamp[6] / 16,g_astFilePartTbl[1].aucTimeStamp[6] % 16);
    OutputMenu(&SELECT_USART,aucBuff,uiCount);

    //程序1状态
    memset(aucBuff, 0, BUF_SIZE);
    if(g_astFilePartTbl[0].ucStatus== PROGRAM_STATUS_RUNNABLE)
    {
        uiCount = sprintf(aucBuff,"P1Status=RUNNABLE  ");     
    }
    else if(g_astFilePartTbl[0].ucStatus == PROGRAM_STATUS_UNRUNNABLE)
    {
        uiCount = sprintf(aucBuff,"P1Status=UNRUNNABLE  ");     
    }
    else if(g_astFilePartTbl[0].ucStatus == PROGRAM_STATUS_UPGRADE)
    {
        uiCount = sprintf(aucBuff,"P1Status=UPGRADING  ");     
    }
    else if(g_astFilePartTbl[0].ucStatus == PROGRAM_STATUS_VALID)
    {
        uiCount = sprintf(aucBuff,"P1Status=VALID  ");     
    }
    else
    {
        uiCount = sprintf(aucBuff,"P1Status=INVALID  ");     
    }
    OutputMenu(&SELECT_USART,aucBuff,uiCount); 

    //程序2状态
    memset(aucBuff, 0, BUF_SIZE);
    if(g_astFilePartTbl[1].ucStatus== PROGRAM_STATUS_RUNNABLE)
    {
        uiCount = sprintf(aucBuff,"P2Status=RUNNABLE  ");     
    }
    else if(g_astFilePartTbl[1].ucStatus == PROGRAM_STATUS_UNRUNNABLE)
    {
        uiCount = sprintf(aucBuff,"P2Status=UNRUNNABLE  ");     
    }
    else if(g_astFilePartTbl[1].ucStatus == PROGRAM_STATUS_UPGRADE)
    {
        uiCount = sprintf(aucBuff,"P2Status=UPGRADING  ");     
    }
    else if(g_astFilePartTbl[1].ucStatus == PROGRAM_STATUS_VALID)
    {
        uiCount = sprintf(aucBuff,"P2Status=VALID  ");     
    }
    else
    {
        uiCount = sprintf(aucBuff,"P2Status=INVALID  ");     
    }
    OutputMenu(&SELECT_USART,aucBuff,uiCount);

    //程序1新旧
    memset(aucBuff, 0, BUF_SIZE);
    if(g_astFilePartTbl[0].ucNew== FILE_NEW)
    {
        uiCount = sprintf(aucBuff,"P1:NEW  ");     
    }
    else
    {
        uiCount = sprintf(aucBuff,"P1:OLD  ");     
    }
    OutputMenu(&SELECT_USART,aucBuff,uiCount);

    //程序2新旧
    memset(aucBuff, 0, BUF_SIZE);
    if(g_astFilePartTbl[1].ucNew== FILE_NEW)
    {
        uiCount = sprintf(aucBuff,"P2:NEW  ");     
    }
    else
    {
        uiCount = sprintf(aucBuff,"P2:OLD  ");     
    }
    OutputMenu(&SELECT_USART,aucBuff,uiCount);

}
//MCM-44_20061130_zhangjie_end

void AttOutput(ULONG ulCount)
{
    ULONG i;
    for(i=0;i<ulCount;i++)
    {
        DataToPio(&ATT_PIO_CTRL,i,ATT_OUT_MASK);
        Delay(CPLD_STA_SHORT_TIME);
        LedFlick();
    }
}

//低四位从下行衰耗的低四位输出，高四位从上行衰耗的低四位输出
void OutPutGetDA(ULONG ulCount)
{
    DataToPio(&ATT_PIO_CTRL,0x20,ATT_CTRL_MASK);
    DataToPio(&ATT_PIO_CTRL,ulCount&0x0F,ATT_OUT_MASK);
    DataToPio(&ATT_PIO_CTRL,0x00,ATT_CTRL_MASK);
    DataToPio(&ATT_PIO_CTRL,0x20,ATT_CTRL_MASK);
    DataToPio(&ATT_PIO_CTRL,(ulCount>>4)&0x0F,ATT_OUT_MASK);
    DataToPio(&ATT_PIO_CTRL,0x40,ATT_CTRL_MASK);
}

void CPLDStaStart(void)
{
    DataToPio(&FR_PIO_CTRL,0x80,FR_CK|FR_DA);
    PioWrite(&LED_PIO_CTRL,LED_MASK,LED_ON);
    g_ucLedSta = 0;
    Delay(CPLD_STA_LONG_TIME);
}

void CPLDStaEnd(void)
{
    DataToPio(&FR_PIO_CTRL,0x100,FR_CK|FR_DA);
    PioWrite(&LED_PIO_CTRL,LED_MASK,LED_OFF);
    g_ucLedSta = 1;
    Delay(CPLD_STA_LONG_TIME);
}


void Main( void )
{
    UINT32  cpu_sr,uiSelect;
    UINT32 uiCount;
    CHAR aucBuff[60];

    WdClose();

    PioWrite(&LED_PIO_CTRL, //定义外围PIO的描述符
            LED_MASK,       //根据MyBoard.h的硬件定义的LED屏蔽
            LED_OFF);       //LED 关闭
    //将所有LED引脚设置成输出
    PioOpen(&LED_PIO_CTRL,
            LED_MASK,
            PIO_OUTPUT);    //输出状态

    SELECT_USART.uiBaudRate = 9600;
    UsartOpen(&SELECT_USART);   //初始化USART0

    //输出bootloader版本信息
    uiCount = sprintf(aucBuff,"\n\r\n\rYouKe MC2006 BootLoader");
    OutputMenu(&SELECT_USART,aucBuff,uiCount);
    uiCount = sprintf(aucBuff,"\n\rVersion 1.2.0 Built on Feb-06-2007");    
    OutputMenu(&SELECT_USART,aucBuff,uiCount);
    uiCount = sprintf(aucBuff,"\n\r\n\rStart Testing NandFlash... \n\r");
    OutputMenu(&SELECT_USART,aucBuff,uiCount);                              
    
    //测试NANDFLASH
    NFInitPio();
    NFReset();
    if(NFOpen() == FAILED)
    {
        //打开失败可能是ID错误
        USHORT usId = NFReadPageId();

        //信息头
        memset(aucBuff, 0, 60);
        if(usId!=NF_DEV_ID)
        {
            uiCount = sprintf(aucBuff,"\n\rNandFlash Error:id is 0xEC73,but read id is 0x%x\n\r",usId);
        }
        //id相同则格式化错误
        else
        {
            uiCount = sprintf(aucBuff,"\n\rNandFlash Error:Format Failed!\n\r");
        }
        OutputMenu(&SELECT_USART,aucBuff,uiCount);
    }
    else
    {
        OutputMenu(&SELECT_USART,(UCHAR *)aucStr10,sizeof(aucStr10));
    }

    FilePartInit();

    OutputFileInfo();

    OutputMenu(&SELECT_USART,(UCHAR *)aucStr0,sizeof(aucStr0));

    //test
    //程序下载，程序选择运行，程序自动运行都会写分区信息
    //FlashEraseSector(g_stNorFlash0.pfwBaseAddr, 
        //   (UCHAR*)FILE_PART_TBL_ADDR);
    //FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
        //   (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));               

    //test
    //SelectProgramRun();



    //启动定时器
    UINT32 uiTime = 0;

    while(1)
    {
        uiTime++;
        if(uiTime>=NO_INPUT_TIMEOUT)
        {
            //超时则选择程序运行
            uiTime = 0;
            SelectProgramRun();        
        }
        uiSelect = SELECT_NO;
        uiSelect = ReadSelect();
        if(uiSelect != SELECT_NO)
        {
            uiTime = 0;
        }
        if(uiSelect == SELECT_DOWNLOAD_P_1) 
        {
            PioWrite(&LED_PIO_CTRL,LED1,LED_ON);
            
            OutputMenu(&SELECT_USART,(UCHAR *)aucStr1,sizeof(aucStr1));

            if(DownloadFile(&g_astFilePartTbl[0]) == SUCCEEDED)
            {
                //新下载程序改为VALID,新旧交换
                g_astFilePartTbl[0].ucStatus = PROGRAM_STATUS_VALID;
                g_astFilePartTbl[0].ucNew = FILE_NEW;
                g_astFilePartTbl[1].ucNew = FILE_OLD;

                //写分区信息至FLASH
/*
                FlashEraseSector(g_stNorFlash0.pfwBaseAddr, 
                        (UCHAR*)FILE_PART_TBL_ADDR);
                FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
                        (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));            
*/
                SaveFileParam();
                OutputMenu(&SELECT_USART,(UCHAR *)aucStr2,sizeof(aucStr2));            
            }

            //MCM-57_20070131_Zhonghw_begin
            else//(DownloadFile(&g_astFilePartTbl[0]) == FILE_WRITE_FAILED)
            {
                //SaveFileParam();
                OutputMenu(&SELECT_USART,(UCHAR *)aucStr13,sizeof(aucStr13));            
            }            
            //MCM-57_20070131_Zhonghw_end
            
            PioWrite(&LED_PIO_CTRL,LED1,LED_OFF);

            continue;
        }
        else if(uiSelect == SELECT_DOWNLOAD_P_2) 
        {
            PioWrite(&LED_PIO_CTRL,LED1,LED_ON);
            
            OutputMenu(&SELECT_USART,(UCHAR *)aucStr5,sizeof(aucStr5));

            if(DownloadFile(&g_astFilePartTbl[1]) == SUCCEEDED)
            { 
                //新下载程序改为VALID,新旧交换
                g_astFilePartTbl[1].ucStatus = PROGRAM_STATUS_VALID;
                g_astFilePartTbl[1].ucNew = FILE_NEW;
                g_astFilePartTbl[0].ucNew = FILE_OLD;

                //写分区信息至FLASH
/*
                FlashEraseSector(g_stNorFlash0.pfwBaseAddr, 
                        (UCHAR*)FILE_PART_TBL_ADDR);
                FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
                        (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));            
*/
                SaveFileParam();
                OutputMenu(&SELECT_USART,(UCHAR *)aucStr6,sizeof(aucStr6));            
            }

            //MCM-57_20070131_Zhonghw_begin
            else//(DownloadFile(&g_astFilePartTbl[0]) == FILE_WRITE_FAILED)
            {
                //SaveFileParam();
                OutputMenu(&SELECT_USART,(UCHAR *)aucStr14,sizeof(aucStr14));            
            }
            //MCM-57_20070131_Zhonghw_end

            PioWrite(&LED_PIO_CTRL,LED1,LED_OFF);

            continue;
        }
        else if(uiSelect == SELECT_RUN_P_1) 
        {
            //要运行程序时再把VALID改为UPGRADING
            //运行程序把UPGRADING改为RUNNABLE或不处理
            //若不处理则重新启动时把UPGRADING改为UNRUNNABLE
            if(g_astFilePartTbl[0].ucStatus == PROGRAM_STATUS_VALID)
            {
                g_astFilePartTbl[0].ucStatus = PROGRAM_STATUS_UPGRADE;
            }

            //写分区信息至FLASH
/*
            FlashEraseSector(g_stNorFlash0.pfwBaseAddr, 
                    (UCHAR*)FILE_PART_TBL_ADDR);
            FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
                    (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));
*/
            g_astFilePartTbl[0].ucIsRunning = 1;
            g_astFilePartTbl[1].ucIsRunning = 0;
            
            SaveFileParam();
            OutputMenu(&SELECT_USART, (UCHAR*)aucStr7, sizeof(aucStr7));

            //notice 读程序之前启动看门狗
            WdOpen();

            //读程序至RAM运行区,偏移128字节，前128自己为文件信息
            FlashRead(g_stNorFlash0.pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[0].ulFileAddr+128),
                    (UCHAR*)(P_RUN_ADDR),g_astFilePartTbl[0].ulFileSize-128);

            ENTER_CRITICAL();
            JMPTORAM();            
            
        }
        else if(uiSelect == SELECT_RUN_P_2) 
        {
            //要运行程序时再把VALID改为UPGRADING
            //运行程序把UPGRADING改为RUNNABLE或不处理
            //若不处理则重新启动时把UPGRADING改为UNRUNNABLE
            if(g_astFilePartTbl[1].ucStatus == PROGRAM_STATUS_VALID)
            {
                g_astFilePartTbl[1].ucStatus = PROGRAM_STATUS_UPGRADE;
            }

            g_astFilePartTbl[0].ucIsRunning = 0;
            g_astFilePartTbl[1].ucIsRunning = 1;

            //写分区信息至FLASH
/*
            FlashEraseSector(g_stNorFlash0.pfwBaseAddr, 
                    (UCHAR*)FILE_PART_TBL_ADDR);
            FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
                    (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));
*/
            SaveFileParam();
            OutputMenu(&SELECT_USART, (UCHAR*)aucStr8, sizeof(aucStr8));

            WdOpen();

            //读程序至RAM运行区,偏移128字节，前128自己为文件信息
            FlashRead(g_stNorFlash0.pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[1].ulFileAddr+128),
                    (UCHAR*)(P_RUN_ADDR),g_astFilePartTbl[1].ulFileSize-128);
            ENTER_CRITICAL();
            JMPTORAM();
        }
        else if(uiSelect == SELECT_RUN_TEST_CPLD) 
        {
            ULONG i,j,k;
            OutputMenu(&SELECT_USART, (UCHAR*)aucStr11, sizeof(aucStr11));
            
            //初始化IO
            PioWrite(&ATT_PIO_CTRL,ATT_CTRL_MASK|ATT_OUT_MASK,0);
            PioOpen(&ATT_PIO_CTRL,ATT_CTRL_MASK|ATT_OUT_MASK,PIO_OUTPUT);
            PioWrite(&FR_PIO_CTRL,FR_CTRL_MASK,0);
            PioOpen(&FR_PIO_CTRL,FR_CTRL_MASK,PIO_OUTPUT);
            PioWrite(&DA_PIO_CTRL,DA_IN_MASK,0);
            PioOpen(&DA_PIO_CTRL,DA_IN_MASK,PIO_INPUT);

            while(1)
            {
                //MCM-22_20061107_zhangjie_begin
                //直通输出01、10、11、00
                DataToPio(&FR_PIO_CTRL,0x80,FR_CK|FR_DA);
                Delay(CPLD_STA_MID_TIME);
                LedFlick();
                DataToPio(&FR_PIO_CTRL,0x100,FR_CK|FR_DA);
                Delay(CPLD_STA_MID_TIME);
                LedFlick();
                DataToPio(&FR_PIO_CTRL,0x180,FR_CK|FR_DA);
                Delay(CPLD_STA_MID_TIME);
                LedFlick();
                DataToPio(&FR_PIO_CTRL,0x00,FR_CK|FR_DA);
                Delay(CPLD_STA_MID_TIME);
                LedFlick();
                //MCM-22_20061107_zhangjie_end

                //选择下行衰耗
                CPLDStaStart();
                DataToPio(&ATT_PIO_CTRL,0x00,ATT_CTRL_MASK);
                AttOutput(32);
                CPLDStaEnd();
                
                //选择上行衰耗
                CPLDStaStart();
                DataToPio(&ATT_PIO_CTRL,0x40,ATT_CTRL_MASK);
                AttOutput(32);
                CPLDStaEnd();
                
                //选择第3路衰耗
                CPLDStaStart();
                DataToPio(&ATT_PIO_CTRL,0x80,ATT_CTRL_MASK);
                AttOutput(32);
                CPLDStaEnd();
                
                //选择功放开关
                CPLDStaStart();
                DataToPio(&ATT_PIO_CTRL,0xC0,ATT_CTRL_MASK);
                AttOutput(8);
                CPLDStaEnd();

                //选频器输出
                
                CPLDStaStart();
                PioWrite(&FR_PIO_CTRL,FR_CE,1);

                for(i=0;i<16;i++)
                {
                    DataToPio(&FR_PIO_CTRL,i*8,FR_S3|FR_S2|FR_S1|FR_S0);
                    //MCM-22_20061107_zhangjie_begin
                    Delay(CPLD_STA_SHORT_TIME);
                    //MCM-22_20061107_zhangjie_end
                    LedFlick();
                }
                CPLDStaEnd();
                
                //数字采集
                PioWrite(&FR_PIO_CTRL,FR_CTRL_MASK,0);
                
                for(k=0;k<5;k++)
                {
                    CPLDStaStart();
                    j = 0;
                    //MCM-22_20061107_zhangjie_begin
                    while(j < 20*1000)
                    //MCM-22_20061107_zhangjie_end
                    {
                        j++;
                        DataToPio(&FR_PIO_CTRL,k<<3,FR_CTRL_MASK);
                        i=PioRead(&DA_PIO_CTRL);
                        i = i & DA_IN_MASK;
                        i = i >> 8;
                        OutPutGetDA(i);
                    }
                    CPLDStaEnd();
                }
                
                OutputMenu(&SELECT_USART, (UCHAR*)aucStr12, sizeof(aucStr12));

                j = 0;
                while((i = ReadSelect()) == SELECT_NO)
                {
                    j++;
                    if(j > 0x2580000)//大约10分钟
                    {
                        break;
                    }
                }
                        
                
                if((i == 'y') || (i == 'Y'))
                {
                    OutputMenu(&SELECT_USART, (UCHAR*)aucStr11, sizeof(aucStr11));
                }
                else
                {
                    uiTime = 0;
                    OutputMenu(&SELECT_USART, (UCHAR*)aucStr0, sizeof(aucStr0));
                    break;
                }
            }
        }
        /*
        else if(uiSelect == SELECT_UPLOARD_P_1) 
        {

            UINT32 uiPackSize = USART_SIZE_BUFFER;
            UINT32 uiPackCount = g_astFilePartTbl[0].ulFileSize/uiPackSize;
            UINT32 uiLastCount = g_astFilePartTbl[0].ulFileSize % uiPackSize;
            UCHAR *pucTemp[USART_SIZE_BUFFER];
            UINT32 i;
            FLASHWORD* pfwBaseAddr = g_stNorFlash0.pfwBaseAddr;

       
        //读文件至RAM从串口发送
        //读整片SECTOR
            for(i=0; i<uiPackCount; i++)
            {
                FlashRead(pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[0].ulFileAddr+i*uiPackSize),
                        pucTemp,uiPackSize); 
                ENTER_CRITICAL();
                UsartWrite(&SELECT_USART,pucTemp,uiPackSize);
                EXIT_CRITICAL();
                while(SELECT_USART.pstData->pucTxIn!=SELECT_USART.pstData->pucTxOut);
            }
                //读最后一片SECTOR
                FlashRead(pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[0].ulFileAddr+uiPackCount*uiPackSize),
                        pucTemp,uiLastCount); 
                ENTER_CRITICAL();
                UsartWrite(&SELECT_USART,pucTemp,uiLastCount);
                EXIT_CRITICAL();
                while(SELECT_USART.pstData->pucTxIn!=SELECT_USART.pstData->pucTxOut);
        }
        else if(uiSelect == SELECT_UPLOARD_P_2) 
        {

            
            UINT32 uiPackSize = USART_SIZE_BUFFER;
            UINT32 uiPackCount = g_astFilePartTbl[1].ulFileSize/uiPackSize;
            UINT32 uiLastCount = g_astFilePartTbl[1].ulFileSize % uiPackSize;
            UCHAR *pucTemp[USART_SIZE_BUFFER];
            UINT32 i;
            FLASHWORD* pfwBaseAddr = g_stNorFlash0.pfwBaseAddr;

       
        //读文件至RAM从串口发送
        //读整片SECTOR
            for(i=0; i<uiPackCount; i++)
            {
                FlashRead(pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[1].ulFileAddr+i*uiPackSize),
                        pucTemp,uiPackSize); 
                ENTER_CRITICAL();
                UsartWrite(&SELECT_USART,pucTemp,uiPackSize);
                EXIT_CRITICAL();
                while(SELECT_USART.pstData->pucTxIn!=SELECT_USART.pstData->pucTxOut);
            }
                //读最后一片SECTOR
                FlashRead(pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[1].ulFileAddr+uiPackCount*uiPackSize),
                        pucTemp,uiLastCount); 
                ENTER_CRITICAL();
                UsartWrite(&SELECT_USART,pucTemp,uiLastCount);
                EXIT_CRITICAL();
                while(SELECT_USART.pstData->pucTxIn!=SELECT_USART.pstData->pucTxOut);
        }
        */
        else
        {
            //do nothing
        } 
    } // while(1) 



   
}


