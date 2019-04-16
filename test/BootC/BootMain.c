/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   BootMain.c
    ����:     �½�
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  2006/09/28
       ����:  �½�
       ����:  �������
    2. ����:  2006/11/07
       ����:  �½�
       ����:  �޸�����MCM-22�����ٲ���CPLDÿ������ĵȴ�ʱ��
    3. ����:  2006/11/30
       ����:  �½�
       ����:  �޸�����MCM-44�����Ӷ��ļ�����ʱ��Ĵ�ӡ
    4. ����:  2007/01/31
       ����:  �ӻ���
       ����:  �޸�����MCM-57���޸Ķ��ļ�����ʧ��дFLASHʧ�ܵ���ʾ
    5. ����:  2007/02/06
       ����:  �ӻ���
       ����:  �޸�����MCM-59��
              �޸ĵ��ö�ȡ�������ݺ�����ʹ�õĻ�������С��ʹ����
              ���ڽ��ջ�������Сһ�£��Ӷ���ֹ��ȡ����û���жϴ�
              ��Ļ����С���������ڴ����Խ��
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

//����Э��Ľ��ջ��峤��
#define PRIVATE_DATA_PACK_SIZE   (6*1024)
//����Э�������С��������β�ַ�
#define PRIVATE_DATA_MINSIZE  11


//����Э��ת���ַ��б���
#define PRIVATE_ESCAPE_NUM  3
//����Э��ת���ַ�
#define PRIVATE_ESCAPE_CHAR  '^'

//�������ӳ�ʱ��(0x10000 = about 1 sec)
#define NO_INPUT_TIMEOUT 0x60000

#define FIND_NO_CHAR 0xFFFFFFF0

#define FILE_REC_TEMP_ADDR ((UCHAR*)0x2000000)

//��ѡ��Ĵ���
#define SELECT_USART  g_stUsart0

//����CPLD�ĺ궨��
//˥����(���)
#define ATT1    PB0
#define ATT2    PB1
#define ATT4    PB2
#define ATT8    PB3
#define ATT16   PB4
#define ATT_OUT_MASK (ATT1|ATT2|ATT4|ATT8|ATT16)
#define ATT_PIO_CTRL g_stPioB

//(�������)
#define OUT_ENA PB5
#define S1              PB6
#define S2              PB7
#define ATT_CTRL_MASK (OUT_ENA|S1|S2)

//�������ɼ�ģ��(����)
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

//ѡƵ��(�������)
#define FR_CE   PA2
#define FR_S3   PA3
#define FR_S2   PA4
#define FR_S1   PA5
#define FR_S0   PA6
#define FR_CK   PA7
#define FR_DA   PA8
#define FR_CTRL_MASK (FR_CE|FR_S3|FR_S2|FR_S1|FR_S0|FR_CK|FR_DA)
#define FR_PIO_CTRL g_stPioA

//CPLD״̬Ǩ��ʱ��
#define CPLD_STA_LONG_TIME 600
//MCM-22_20061107_zhangjie_begin
#define CPLD_STA_MID_TIME 300
#define CPLD_STA_SHORT_TIME 60
//MCM-22_20061107_zhangjie_end

//������ΪaucDataPack[0]=< ... <aucDataPack[uiInPos]
typedef struct
{
    UINT32 uiState;
    UINT32 uiInPos;
    UCHAR aucDataPack[PRIVATE_DATA_PACK_SIZE];
}DATA_PACK_ST;

DATA_PACK_ST g_stPrivateDataPack;

//����Э����ת���ַ��б�
UCHAR g_aucPEscaped[] =
{
   0x01,
   0x02,
   0x03,
};


//����Э��δת���ַ�(����Ҫת��)�б�
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

//��������
extern void WdOpen(void);
extern void WdClose(void);

//���·�װFLASH�Ĳ���������ʹ���ø�����
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
  Description:    ���������ڱȽ�FLASH�е����ݿ��RAM�е�
                  ���ݿ��Ƿ�һ�£���Ҫ������дFLASH֮ǰ��
                  ���жϣ��������һ��������дFLASH
  Calls:
  Called By:
  Input:          ulFlashAddr: FLASH����ʼ��ַ
                  pvData:      RAM�е���ʼ��ַ
                  ulSize:      ���ݿ鳤��
  Output:         
  Return:         SUCCEEDED:   ����һ��
                  FAILED:      ���߲�һ��
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
    return; //���ݲ�����ֱ�ӷ���
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

//����˸
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

//***************���ҵ����ַ�***************
//���룺
//Ҫ���ҵ��ַ���pucStr��
//Ҫ���ҵ�uiNmum��ƥ���ַ�ucTarget��λ��
//���ҵ���󳤶�uiMaxlength
//�����Ҫ�����ַ���λ�ã���0��ʼ���������û���ҵ��򷵻�-1.
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
    static UINT32 uiFileLen = 0;//���ڴ����ļ�ʱ����   

    if(uiCount <= PRIVATE_DATA_MINSIZE)
    {
        return FAILED;
    }
    //ת��crc
    //��β�ַ�����ת��
    pucStart++;
    uiCount = uiCount-2;
    //ת��
    uiCount = DecodeEscape(pucStart, uiCount, PRIVATE_ESCAPE_CHAR, 
            &g_aucPEscaped, &g_aucPUnEscape, PRIVATE_ESCAPE_NUM);

    //��ת��ʧ�ܵ��ж�  
    if(uiCount == 0)
    {
        return FAILED;
    }

    USHORT usPackCount = *((USHORT*)pucStart);
    USHORT usPackNum = *((USHORT*)(pucStart+2));

    //����ָ��ͳ���
    pucStart = pucStart + 7;
    uiCount = uiCount -9;

    //д�ļ���RAM
    for(i=0; i<uiCount; i++)
    {
        *(FILE_REC_TEMP_ADDR + uiFileLen + i) = pucStart[i]; 
    }
    uiFileLen = uiFileLen + uiCount;

    //test
    if(usPackCount == usPackNum)
    {
        //�ļ�����дFLASH
        UINT32 uiSectorSize = g_stNorFlash0.uiSectorSize;        
        UINT32 uiSectorCount = uiFileLen/uiSectorSize + 1;
        FLASHWORD* pfwBaseAddr = g_stNorFlash0.pfwBaseAddr;
        //UCHAR *pucFlashAddr = (UCHAR*)(pstFileDesc->ulFileAddr);
        UCHAR *pucFlashAddr;
        (UINT32)pucFlashAddr = (*pstFileDesc).ulFileAddr;
        //д�ļ�������Ϣ
        (*pstFileDesc).ulFileSize = uiFileLen;
        memcpy((UCHAR *)pstFileDesc,(UCHAR *)FILE_REC_TEMP_ADDR,FPD_FILE_NAME_LEN+TIME_STAMP_LEN+FILE_VERSION_LEN);

        //MCM-57_20070131_Zhonghw_begin
        //д�ļ���FLASH
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

        //д������Ϣ��FLASH
        SaveFileParam();

        //�ļ����������رտ��Ź�
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
    //notice Ϊ���ڶ�ȡ���ٵ��ڴ��СӦ���ȴ������ݽ��ջ�����С
    UCHAR ucRec[USART_SIZE_BUFFER];        //USART_SIZE_BUFFER
    //MCM-59_20070206_Zhonghw_end
    
    if(UsartReadReady ( &SELECT_USART ))
    {
        //��ֻ֤��һ����������
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
  Description:    ��������ת��
  Calls:
  Called By:
  Input:          pucData:   ��ת�������
                  uiLen: ��ת�����ݵĳ���
                  ucEscapeChar: ת���ַ�
                  pucEscaped: ��ת�����ַ��б�
                  pucUnEscaped: ��ת�����ַ��б���pucEscaped��Ӧ
                  uiEscapeNum: ת���ַ��б�ĳ���                 
  Output:
  Return:         0:   ʧ��
                  ��0: ��ת����ʵ�ʳ���
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

    //�򿪿��Ź�����ֹ���κ���������
    WdOpen();

    //���϶������Ƿ�������
    for(;;)
    {
        if(UsartReadReady ( &SELECT_USART ))
        {
            //�����ݾ�ι��
            WdRestart();
            ENTER_CRITICAL();
            uiCount = UsartRead ( &SELECT_USART, aucRec);
            EXIT_CRITICAL();
            //���յ������ݰ����д���ֱ���������
            for(;;)
            {
                uiStartPos = FindChar(aucRec, FILEBLOCK_START_CHAR, 1, uiCount);
                uiEndPos = FindChar(aucRec, FILEBLOCK_END_CHAR, 1, uiCount);
                if(g_stPrivateDataPack.uiState == DATAPACK_STATE_NO)
                {
                    g_stPrivateDataPack.uiInPos = 0;
                    //����NO S NO E
                    if((uiStartPos == FIND_NO_CHAR) && (uiEndPos == FIND_NO_CHAR))
                    {                    
                        //��������������
                        break;
                    }
                    //����NO S ONLY E
                    else if((uiStartPos == FIND_NO_CHAR) && (uiEndPos != FIND_NO_CHAR))
                    {
                        //��������������               
                        break;
                    }
                    //����ONLY S NO E
                    else if((uiStartPos != FIND_NO_CHAR) && (uiEndPos == FIND_NO_CHAR))
                    {
                        CopyStr(g_stPrivateDataPack.aucDataPack, aucRec, uiCount);
                        g_stPrivateDataPack.uiInPos = g_stPrivateDataPack.uiInPos + uiCount;
                        g_stPrivateDataPack.uiState = DATAPACK_STATE_START;
                        break;
                    }
                    //uiStartPos != FIND_NO_CHAR && uiEndPos != FIND_NO_CHAR
                    //����S E                       
                    else if(uiStartPos < uiEndPos)
                    {
                        CopyStr(g_stPrivateDataPack.aucDataPack, 
                                aucRec+uiStartPos, uiEndPos - uiStartPos +1);
                        g_stPrivateDataPack.uiInPos = uiEndPos - uiStartPos +1;
                        
                        //����������
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
                        //����ʣ�������                    
                        uiCount = uiCount - uiEndPos - 1;
                        //����ʣ�������                    
                        CopyStr(aucRec, aucRec+uiEndPos+1, uiCount);
                        //��������ʣ�����ݰ�
                        continue;
                    }
                    //uiStartPos����İ�Ϊ��Ч��
                    //����E  S
                    else if(uiStartPos > uiEndPos)
                    {
                        //ǰ���յ��İ���uiStartPos��ǰ�İ���Ч
                        //g_stPrivateDataPack.uiInPos = 0;
                        //g_stPrivateDataPack.uiState = DATAPACK_STATE_NO;

                        //����ʣ�������                    
                        uiCount = uiCount - uiStartPos;
                        //����ʣ�������                    
                        CopyStr(aucRec, aucRec+uiStartPos, uiCount);
                        //��������ʣ�����ݰ�
                        continue;
                    }
                    //��Ч�İ�
                    //����S E�����ݰ�����̫С
                    else
                    {
                        //ǰ���յ��İ���Ч
                        g_stPrivateDataPack.uiInPos = 0;
                        g_stPrivateDataPack.uiState = DATAPACK_STATE_NO;
                        //��������������               
                        break;
                    }
                }//g_stPrivateDataPack.uiState == DATAPACK_STATE_NO     
                if(g_stPrivateDataPack.uiState == DATAPACK_STATE_START)
                {
                    //����ONLY S NO E
                    if((uiStartPos == FIND_NO_CHAR) && (uiEndPos == FIND_NO_CHAR))
                    {
                        CopyStr(g_stPrivateDataPack.aucDataPack+g_stPrivateDataPack.uiInPos, 
                                aucRec, uiCount);
                        g_stPrivateDataPack.uiInPos = g_stPrivateDataPack.uiInPos + uiCount;
                        //��������������
                        break;
                    }
                    //����S E
                    else if((uiStartPos == FIND_NO_CHAR) && (uiEndPos != FIND_NO_CHAR))
                    {
                        CopyStr(g_stPrivateDataPack.aucDataPack + g_stPrivateDataPack.uiInPos,
                                aucRec, uiEndPos + 1);
                        g_stPrivateDataPack.uiInPos = g_stPrivateDataPack.uiInPos + uiEndPos + 1;
                        
                        //����������
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
                        //����ʣ�������                    
                        uiCount = uiCount - uiEndPos - 1;
                        //����ʣ�������                    
                        CopyStr(aucRec, aucRec+uiEndPos+1, uiCount);
                        //��������ʣ�����ݰ�
                        continue;
                    }
                    //����S S
                    else if((uiStartPos != FIND_NO_CHAR) && (uiEndPos == FIND_NO_CHAR))
                    {
                        //ǰ���յ��İ�S S��Ч
                        g_stPrivateDataPack.uiInPos = 0;
                        g_stPrivateDataPack.uiState = DATAPACK_STATE_NO;
                        //����ʣ�������                    
                        uiCount = uiCount - uiStartPos;
                        //����ʣ�������                    
                        CopyStr(aucRec, aucRec+uiStartPos, uiCount);
                        //��������ʣ�����ݰ�
                        continue;
                    }
                    //uiStartPos != FIND_NO_CHAR && uiEndPos != FIND_NO_CHAR
                    //����S S E                    
                    else if(uiStartPos < uiEndPos)
                    {
                        //ǰ���յ��İ�S S��Ч
                        g_stPrivateDataPack.uiInPos = 0;
                        g_stPrivateDataPack.uiState = DATAPACK_STATE_NO;

                        //S E��Ч
                        CopyStr(g_stPrivateDataPack.aucDataPack, 
                                aucRec+uiStartPos, uiEndPos - uiStartPos +1);
                        g_stPrivateDataPack.uiInPos = g_stPrivateDataPack.uiInPos 
                                + uiEndPos - uiStartPos +1;
                        
                        //����������
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
                        //����ʣ�������                    
                        uiCount = uiCount - uiEndPos - 1;
                        //����ʣ�������                    
                        CopyStr(aucRec, aucRec+uiEndPos+1, uiCount);
                        //��������ʣ�����ݰ�
                        continue;
                    }
                    //����S E S
                    //uiEndPosǰ��İ�S EΪ��Ч��
                    else if(uiStartPos > uiEndPos)
                    {
                        CopyStr(g_stPrivateDataPack.aucDataPack+g_stPrivateDataPack.uiInPos, 
                                aucRec, uiEndPos +1);
                        //E Sֱ��Ϊ��Ч���ݰ�
                        //����ʣ�������                    
                        uiCount = uiCount - uiStartPos;
                        //����ʣ�������                    
                        CopyStr(aucRec, aucRec+uiStartPos, uiCount);
                        //��������ʣ�����ݰ�
                        continue;
                    }
                    //����S S E�����ݰ�����̫С
                    else
                    {
                        g_stPrivateDataPack.uiInPos = 0;
                        g_stPrivateDataPack.uiState = DATAPACK_STATE_NO; 
                        //��������������               
                        break;
                    }
                }//g_stPrivateDataPack.uiState == DATAPACK_STATE_START                  
            }//for(;;)���յ������ݰ����д���ֱ���������
        }//UsartReadReady
    }//for(;;) ���϶������Ƿ�������
}

void FilePartInit(void)
{
    //�����ļ�������Ϣ
    FlashRead(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
            (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));

    //�ѳ�ʼ�������ٳ�ʼ��
    //if((g_astFilePartTbl[0].ulFileAddr == PROGRAM_PART_1_ADDR) &&
        //    (g_astFilePartTbl[1].ulFileAddr == PROGRAM_PART_2_ADDR))
    //{
        //return;
    //}

    //��ʼ���ļ�������Ϣ
    g_astFilePartTbl[0].ulFileAddr = PROGRAM_PART_1_ADDR;
    g_astFilePartTbl[1].ulFileAddr = PROGRAM_PART_2_ADDR;

    //��ʼ���ļ�״̬��ĿǰΪ����ģʽ
    //��ʼ����ֻ������״̬
    //INVALID, RUNNABLE, VALID, UNRUNNABLE 
    //ֻ����VALID��ѡ������ʱ���ų���UPGRADING
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

    //��ʼ���ļ�״̬��ĿǰΪ����ģʽ
    //��ʼ����ֻ������״̬
    //INVALID, RUNNABLE, VALID, UNRUNNABLE 
    //ֻ����VALID��ѡ������ʱ���ų���UPGRADING
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

    //д������Ϣ��FLASH         
    //test
    //FlashEraseSector(g_stNorFlash0.pfwBaseAddr, 
    //       (UCHAR*)FILE_PART_TBL_ADDR);
    //FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
    //       (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));       

}

//������(���̻��ߴ���)��ʱ�����
//��ʼ����ֻ������״̬
//INVALID, RUNNABLE, VALID, UNRUNNABLE 
void SelectProgramRun(void)
{
    UCHAR ucP1Status = g_astFilePartTbl[0].ucStatus;
    UCHAR ucP2Status = g_astFilePartTbl[1].ucStatus;
    UCHAR ucSelectP = 0;//ѡ�����1��2��0��ʾ�쳣
    ULONG cpu_sr;

    //״̬��ͬ��ֱ��break��Ϊ�쳣
    if(ucP1Status == ucP2Status)
    {
        //ѡ���µĳ�����������µ���ѡ��1
        //������Ǿɵ���ѡ��2
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
        //ѡ���µĳ�����������µ���ѡ��1
        //������Ǿɵ���ѡ��2        
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
    
    //״̬��ͬ
    //VALID���ȼ����
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

    //RUNNABLE���ȼ��θ�
    else if(ucP1Status == PROGRAM_STATUS_RUNNABLE)
    {
        ucSelectP = 1;
    }
    else if(ucP2Status == PROGRAM_STATUS_RUNNABLE)
    {
        ucSelectP = 2;
    }
    //���������м�û��VALID��Ҳû��RUNNABLE���쳣
    else
    {
        ucSelectP = 0;
    }

    //д������Ϣ��FLASH         
    //FlashEraseSector(g_stNorFlash0.pfwBaseAddr, 
        //    (UCHAR*)FILE_PART_TBL_ADDR);
    //FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
        //    (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));

    //ѡ���������
    if(ucSelectP == 1)
    {
        OutputMenu(&SELECT_USART, (UCHAR*)aucStr7, sizeof(aucStr7));

    //д������Ϣ��FLASH
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

        //��������RAM������,ƫ��128�ֽڣ�ǰ128�Լ�Ϊ�ļ���Ϣ
        FlashRead(g_stNorFlash0.pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[0].ulFileAddr+128),
                (UCHAR*)(P_RUN_ADDR),g_astFilePartTbl[0].ulFileSize-128);

        
        ENTER_CRITICAL();

        JMPTORAM();        
    }
    else if(ucSelectP == 2)
    {
        OutputMenu(&SELECT_USART, (UCHAR*)aucStr8, sizeof(aucStr8));

    //д������Ϣ��FLASH
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

        //��������RAM������,ƫ��128�ֽڣ�ǰ128�Լ�Ϊ�ļ���Ϣ
        FlashRead(g_stNorFlash0.pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[1].ulFileAddr+128),
                (UCHAR*)(P_RUN_ADDR),g_astFilePartTbl[1].ulFileSize-128);

        ENTER_CRITICAL();
        JMPTORAM();
    }
    //�����쳣����
    else
    {
        OutputMenu(&SELECT_USART, (UCHAR*)aucStr9, sizeof(aucStr9));    
    }
}

//����ģʽ�¶Ա�ѡ�����״̬�Ĵ���
void PStaHndlInDebug(UCHAR ucSelectP)
{
}


//MCM-44_20061130_zhangjie_begin
void OutputFileInfo(void)
{
#define BUF_SIZE 100
    UINT32 uiCount;
    CHAR aucBuff[BUF_SIZE];

    //��Ϣͷ
    memset(aucBuff, 0, BUF_SIZE);
    uiCount = sprintf(aucBuff,"\n\rFile Information(P1=Program 1;  P2=Program 2):");
    OutputMenu(&SELECT_USART,aucBuff,uiCount);

    //�ļ���
    g_astFilePartTbl[0].acFileName[FPD_FILE_NAME_LEN-1] = 0;
    g_astFilePartTbl[1].acFileName[FPD_FILE_NAME_LEN-1] = 0;
    memset(aucBuff, 0, BUF_SIZE);
    uiCount = sprintf(aucBuff,"\n\rP1Name:%s  P2Name:%s  ",
            (g_astFilePartTbl[0].acFileName),(g_astFilePartTbl[1].acFileName));
    OutputMenu(&SELECT_USART,aucBuff,uiCount);

    //�ļ��汾
    g_astFilePartTbl[0].acVersion[FPD_FILE_NAME_LEN-1] = 0;
    g_astFilePartTbl[1].acVersion[FPD_FILE_NAME_LEN-1] = 0;
    memset(aucBuff, 0, BUF_SIZE);
    uiCount = sprintf(aucBuff,"\n\rP1Version:%s  P2Version:%s  ",
            (g_astFilePartTbl[0].acVersion),(g_astFilePartTbl[1].acVersion));
    OutputMenu(&SELECT_USART,aucBuff,uiCount);

    //��С
    memset(aucBuff, 0, BUF_SIZE);
    uiCount = sprintf(aucBuff,"\n\rP1Size=%dBytes  P2Size=%dBytes  ",
            (g_astFilePartTbl[0].ulFileSize),(g_astFilePartTbl[1].ulFileSize));
    OutputMenu(&SELECT_USART,aucBuff,uiCount);

    //ʱ��
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

    //����1״̬
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

    //����2״̬
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

    //����1�¾�
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

    //����2�¾�
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

//����λ������˥�ĵĵ���λ���������λ������˥�ĵĵ���λ���
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

    PioWrite(&LED_PIO_CTRL, //������ΧPIO��������
            LED_MASK,       //����MyBoard.h��Ӳ�������LED����
            LED_OFF);       //LED �ر�
    //������LED�������ó����
    PioOpen(&LED_PIO_CTRL,
            LED_MASK,
            PIO_OUTPUT);    //���״̬

    SELECT_USART.uiBaudRate = 9600;
    UsartOpen(&SELECT_USART);   //��ʼ��USART0

    //���bootloader�汾��Ϣ
    uiCount = sprintf(aucBuff,"\n\r\n\rYouKe MC2006 BootLoader");
    OutputMenu(&SELECT_USART,aucBuff,uiCount);
    uiCount = sprintf(aucBuff,"\n\rVersion 1.2.0 Built on Feb-06-2007");    
    OutputMenu(&SELECT_USART,aucBuff,uiCount);
    uiCount = sprintf(aucBuff,"\n\r\n\rStart Testing NandFlash... \n\r");
    OutputMenu(&SELECT_USART,aucBuff,uiCount);                              
    
    //����NANDFLASH
    NFInitPio();
    NFReset();
    if(NFOpen() == FAILED)
    {
        //��ʧ�ܿ�����ID����
        USHORT usId = NFReadPageId();

        //��Ϣͷ
        memset(aucBuff, 0, 60);
        if(usId!=NF_DEV_ID)
        {
            uiCount = sprintf(aucBuff,"\n\rNandFlash Error:id is 0xEC73,but read id is 0x%x\n\r",usId);
        }
        //id��ͬ���ʽ������
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
    //�������أ�����ѡ�����У������Զ����ж���д������Ϣ
    //FlashEraseSector(g_stNorFlash0.pfwBaseAddr, 
        //   (UCHAR*)FILE_PART_TBL_ADDR);
    //FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
        //   (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));               

    //test
    //SelectProgramRun();



    //������ʱ��
    UINT32 uiTime = 0;

    while(1)
    {
        uiTime++;
        if(uiTime>=NO_INPUT_TIMEOUT)
        {
            //��ʱ��ѡ���������
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
                //�����س����ΪVALID,�¾ɽ���
                g_astFilePartTbl[0].ucStatus = PROGRAM_STATUS_VALID;
                g_astFilePartTbl[0].ucNew = FILE_NEW;
                g_astFilePartTbl[1].ucNew = FILE_OLD;

                //д������Ϣ��FLASH
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
                //�����س����ΪVALID,�¾ɽ���
                g_astFilePartTbl[1].ucStatus = PROGRAM_STATUS_VALID;
                g_astFilePartTbl[1].ucNew = FILE_NEW;
                g_astFilePartTbl[0].ucNew = FILE_OLD;

                //д������Ϣ��FLASH
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
            //Ҫ���г���ʱ�ٰ�VALID��ΪUPGRADING
            //���г����UPGRADING��ΪRUNNABLE�򲻴���
            //������������������ʱ��UPGRADING��ΪUNRUNNABLE
            if(g_astFilePartTbl[0].ucStatus == PROGRAM_STATUS_VALID)
            {
                g_astFilePartTbl[0].ucStatus = PROGRAM_STATUS_UPGRADE;
            }

            //д������Ϣ��FLASH
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

            //notice ������֮ǰ�������Ź�
            WdOpen();

            //��������RAM������,ƫ��128�ֽڣ�ǰ128�Լ�Ϊ�ļ���Ϣ
            FlashRead(g_stNorFlash0.pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[0].ulFileAddr+128),
                    (UCHAR*)(P_RUN_ADDR),g_astFilePartTbl[0].ulFileSize-128);

            ENTER_CRITICAL();
            JMPTORAM();            
            
        }
        else if(uiSelect == SELECT_RUN_P_2) 
        {
            //Ҫ���г���ʱ�ٰ�VALID��ΪUPGRADING
            //���г����UPGRADING��ΪRUNNABLE�򲻴���
            //������������������ʱ��UPGRADING��ΪUNRUNNABLE
            if(g_astFilePartTbl[1].ucStatus == PROGRAM_STATUS_VALID)
            {
                g_astFilePartTbl[1].ucStatus = PROGRAM_STATUS_UPGRADE;
            }

            g_astFilePartTbl[0].ucIsRunning = 0;
            g_astFilePartTbl[1].ucIsRunning = 1;

            //д������Ϣ��FLASH
/*
            FlashEraseSector(g_stNorFlash0.pfwBaseAddr, 
                    (UCHAR*)FILE_PART_TBL_ADDR);
            FlashWrite(g_stNorFlash0.pfwBaseAddr, (UCHAR*)FILE_PART_TBL_ADDR, 
                    (UCHAR*)g_astFilePartTbl, sizeof(g_astFilePartTbl));
*/
            SaveFileParam();
            OutputMenu(&SELECT_USART, (UCHAR*)aucStr8, sizeof(aucStr8));

            WdOpen();

            //��������RAM������,ƫ��128�ֽڣ�ǰ128�Լ�Ϊ�ļ���Ϣ
            FlashRead(g_stNorFlash0.pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[1].ulFileAddr+128),
                    (UCHAR*)(P_RUN_ADDR),g_astFilePartTbl[1].ulFileSize-128);
            ENTER_CRITICAL();
            JMPTORAM();
        }
        else if(uiSelect == SELECT_RUN_TEST_CPLD) 
        {
            ULONG i,j,k;
            OutputMenu(&SELECT_USART, (UCHAR*)aucStr11, sizeof(aucStr11));
            
            //��ʼ��IO
            PioWrite(&ATT_PIO_CTRL,ATT_CTRL_MASK|ATT_OUT_MASK,0);
            PioOpen(&ATT_PIO_CTRL,ATT_CTRL_MASK|ATT_OUT_MASK,PIO_OUTPUT);
            PioWrite(&FR_PIO_CTRL,FR_CTRL_MASK,0);
            PioOpen(&FR_PIO_CTRL,FR_CTRL_MASK,PIO_OUTPUT);
            PioWrite(&DA_PIO_CTRL,DA_IN_MASK,0);
            PioOpen(&DA_PIO_CTRL,DA_IN_MASK,PIO_INPUT);

            while(1)
            {
                //MCM-22_20061107_zhangjie_begin
                //ֱͨ���01��10��11��00
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

                //ѡ������˥��
                CPLDStaStart();
                DataToPio(&ATT_PIO_CTRL,0x00,ATT_CTRL_MASK);
                AttOutput(32);
                CPLDStaEnd();
                
                //ѡ������˥��
                CPLDStaStart();
                DataToPio(&ATT_PIO_CTRL,0x40,ATT_CTRL_MASK);
                AttOutput(32);
                CPLDStaEnd();
                
                //ѡ���3·˥��
                CPLDStaStart();
                DataToPio(&ATT_PIO_CTRL,0x80,ATT_CTRL_MASK);
                AttOutput(32);
                CPLDStaEnd();
                
                //ѡ�񹦷ſ���
                CPLDStaStart();
                DataToPio(&ATT_PIO_CTRL,0xC0,ATT_CTRL_MASK);
                AttOutput(8);
                CPLDStaEnd();

                //ѡƵ�����
                
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
                
                //���ֲɼ�
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
                    if(j > 0x2580000)//��Լ10����
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

       
        //���ļ���RAM�Ӵ��ڷ���
        //����ƬSECTOR
            for(i=0; i<uiPackCount; i++)
            {
                FlashRead(pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[0].ulFileAddr+i*uiPackSize),
                        pucTemp,uiPackSize); 
                ENTER_CRITICAL();
                UsartWrite(&SELECT_USART,pucTemp,uiPackSize);
                EXIT_CRITICAL();
                while(SELECT_USART.pstData->pucTxIn!=SELECT_USART.pstData->pucTxOut);
            }
                //�����һƬSECTOR
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

       
        //���ļ���RAM�Ӵ��ڷ���
        //����ƬSECTOR
            for(i=0; i<uiPackCount; i++)
            {
                FlashRead(pfwBaseAddr,(UCHAR*)(g_astFilePartTbl[1].ulFileAddr+i*uiPackSize),
                        pucTemp,uiPackSize); 
                ENTER_CRITICAL();
                UsartWrite(&SELECT_USART,pucTemp,uiPackSize);
                EXIT_CRITICAL();
                while(SELECT_USART.pstData->pucTxIn!=SELECT_USART.pstData->pucTxOut);
            }
                //�����һƬSECTOR
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


