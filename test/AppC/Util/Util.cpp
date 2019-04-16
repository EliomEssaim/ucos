#include "UserType.h"
#include "includes.h" //uC/OS的头文件
#include "../AppMain.h"
#include "Util.h"
/*************************************************
  Function:
  Description:    本函数根据转入的转义参数对数据进行转义编码，为了不使用
                  额外的内存，先统计被转义的字节个数，然后从尾部开始编码，
                  编码的结果正好放在原来数据的起始地址
  Calls:
  Called By:
  Input:          pucData:         待转义的数据
                  ulInputLen:      待转义的数据的长度
                  ulBufSize:       存放转义结果的内存长度
                  ucEscapeChar:    转义字符
                  pucAfterEscape:  转义后的字符集
                  pucBeforeEscape: 转义前的字符集，与转义后的字符集一一对应
                  ulEscapeNum:     转义字符集大小
  Output:         pucData:         转义后的数据
  Return:         0表示转义失败，其它表示转义后的数据长度
  Others:
*************************************************/
ULONG EncodeEscape(UCHAR* pucData, ULONG ulInputLen, ULONG ulBufSize,
                   UCHAR ucEscapeChar, const UCHAR* pucAfterEscape, 
                   const UCHAR* pucBeforeEscape, UINT32 ulEscapeNum)
{
    ULONG ulEncodedLen = 0; //已经处理的数据长度
    ULONG i = 0;
    LONG lNextEncodedPos = 0;
    ULONG ulEscapeCnt = 0;

    //统计被转义的字符的个数
    while(ulEncodedLen != ulInputLen)
    {
        for(i = 0; i < ulEscapeNum; i++)
        {
            if(pucData[ulInputLen - ulEncodedLen - 1] == pucBeforeEscape[i])
            {
                ulEscapeCnt++;
                break;
            }           
        }
        
        ulEncodedLen++;
    }

    if(ulInputLen + ulEscapeCnt > ulBufSize)
    {
        return 0; //说明内存的长度不足以放下编码后的结果
    }

    //开始转义编码，从最后一个字节开始
    ulEncodedLen = 0;
    lNextEncodedPos = ulInputLen + ulEscapeCnt - 1; //从尾部开始存放编码的结果
    while(ulEncodedLen != ulInputLen)
    {
        for(i = 0; i < ulEscapeNum; i++)
        {
            if(pucData[ulInputLen - ulEncodedLen - 1] == pucBeforeEscape[i])
            {
                pucData[lNextEncodedPos] = pucAfterEscape[i];
                lNextEncodedPos--;
                pucData[lNextEncodedPos] = ucEscapeChar;
                lNextEncodedPos--;
                break;
            }           
        }

        if(i == ulEscapeNum)
        {
            pucData[lNextEncodedPos] = pucData[ulInputLen - ulEncodedLen - 1];
            lNextEncodedPos--;
        }
        
        ulEncodedLen++;
    }

    return ulInputLen + ulEscapeCnt;
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
                    const UCHAR* pucEscaped, const UCHAR* pucUnEscaped, UINT32 uiEscapeNum)
{
    UINT32 i, j;
    UINT32 uiRetLen = 0;

    for(i = 0; i < uiLen; i++)
    {
        if(pucData[i] == ucEscapeChar)
        {
            i++;
            for(j = 0; j < uiEscapeNum; j++)
            {
                if(pucData[i] == pucEscaped[j])
                {
                    pucData[uiRetLen] = pucUnEscaped[j];
                    break;
                }
            }

            if(j >= uiEscapeNum)
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

/*************************************************
  Function:
  Description:    本函数计算输入字节数据的CRC校验码
  Calls:
  Called By:
  Input:          pucData: 待计算的数据
                  ulDataLen: 待计算的数据长度
                  usCrcInitValue: CRC初始值，用于分段计算CRC时使用，如果不分段则使用默认值0
  Output:         
  Return:         二字节的CRC校验码
  Others:         
*************************************************/
USHORT CalcuCRC(const UCHAR* pucData, ULONG ulDataLen, USHORT usCrcInitValue)
{
    ULONG i = 0;
    USHORT usCrc = usCrcInitValue;
    while(ulDataLen-- != 0)
    {
        for(i = 0x80; i != 0; i/=2)
        {
            if((usCrc & 0x8000) != 0)
            {
                usCrc *= 2;
                usCrc ^= 0x1021; //余式CRC乘以2再求CRC
            }
            else
            {
                usCrc *= 2;
            }
            if((*pucData & i) != 0)
            {
                usCrc ^= 0x1021; //再加上本位的CRC
            }
        }
        pucData++;
    }
    return usCrc;
} 


/*************************************************
  Function:
  Description:    本函数用于延时，单位ms，只能在
                       有操作系统时使用，且延时时间为
                       MS_PER_TICK的整数倍，如果输入的ms数不是
                       MS_PER_TICK的整数倍，则会有一定误差
  Calls:
  Called By:
  Input:         
  Output:         
  Return:        
  Others:
*************************************************/
void MyDelay(USHORT ulTime)
{
    INT16U time;
    time = ulTime/MS_PER_TICK;//MS_PER_TICK:TICKTIME的毫秒数
    if(ulTime % MS_PER_TICK!=0)
    {
        time = time + 1;
    }
    OSTimeDly(time);
}


//***************查找单个字符***************
//输入：
//要查找的字符串pucStr，
//要查找第uiNmum个匹配字符ucTarget的位置
//查找的最大长度uiMaxlength
//输出：要查找字符的位置：从0开始计数，如果没有找到则返回-1.
UINT32 FindChar(UCHAR* pucStr, UCHAR ucTarget, UINT32 uiNum, UINT32 uiMaxlength)
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

/*************************************************
  Function:
  Description:    本函数将相邻的两字节合并为一个字节，表示的字符的实际值，每两个
                  相邻字节的前一个字节作为高四位，后一个字节作为低四位，最后合并
                  成一个字节，生成的数据长度为原来的一半，如果传入的数据长度为奇
                  数，则只处理其长度减1的数据; 合并是在传入的数据块上进行的，调
                  用方需要保证传入数据块的有效性
  Calls:
  Called By:
  Input:          pucData:   待合并的数据
                  ulDataLen: 待合并的数据长度
  Output:
  Return:         0:   失败
                  非0: 合并后的实际长度
  Others:
*************************************************/
ULONG Merge2Bytes(UCHAR* pucData, ULONG ulDataLen)
{
    ULONG i = 0;
    ULONG j = 0;
    UCHAR ucMergedByte = 0;

    ulDataLen = (ulDataLen >> 1) << 1; //将奇数截取为偶数
    while(i < ulDataLen)
    {
        for(j = 0; j < 2; j++)
        {
            if((*(pucData + i + j) >= '0')
             &&(*(pucData + i + j) <= '9'))     //0-9
            {
                ucMergedByte |= (*(pucData + i + j) - '0') << (4 * (1 - j));
            }
            else if((*(pucData + i + j) >= 'A')
                  &&(*(pucData + i + j) <= 'F')) //A-F
            {
                ucMergedByte |= (*(pucData + i + j) - 'A' + 0xA) << (4 * (1 - j));
            }
            else if((*(pucData + i + j) >= 'a')
                  &&(*(pucData + i + j) <= 'f')) //a-f
            {
                ucMergedByte |= (*(pucData + i + j) - 'a' + 0xA) << (4 * (1 - j));
            }
            else //非法字符
            {
                return 0;
            }
        }
        pucData[i >> 1] = ucMergedByte;
        ucMergedByte = 0;
        i += 2;
    }
    return i >> 1;
}

/*************************************************
  Function:
  Description:    本函数将字节流中的单字节数据分拆成两个字节，高字节用可见字符表
                  示高4位，低字节用可见字符表示低4位，输出长度是输入长度的两倍;
                  拆分是在传入的数据块上进行的，调用方需要保证传入数据块足够存放
                  拆分后的数据
  Calls:
  Called By:
  Input:          pucData:   待拆分的数据
                  ulDataLen: 待拆分的数据长度
  Output:
  Return:         0:   失败
                  非0: 合并后的实际长度
  Others:
*************************************************/
ULONG SplitInto2Bytes(UCHAR *pucData, ULONG ulDataLen)
{
    LONG i = 0;
    UCHAR ucByte = 0;

    //由于拆分是在传入的数据上进行的，为了不破坏未处理的数据，拆分必须从尾部开始，倒序进行
    for(i = ulDataLen - 1; i >= 0; i--)
    {
        ucByte = (UCHAR)(pucData[i] & 0x0F);       //低四位
        if(ucByte <= 9)
        {
            pucData[(i << 1) + 1] = ucByte + 0x30;
        }
        else
        {
            pucData[(i << 1) + 1] = ucByte + 0x37;
        }

        ucByte = (UCHAR)((pucData[i] & 0xF0) >> 4); //高四位
        if(ucByte <= 9) //0－9
        {
            pucData[i << 1] = ucByte + 0x30;
        }
        else //A－F
        {
            pucData[i << 1] = ucByte + 0x37;
        }
    }

    return (ulDataLen << 1);
}

