#include "UserType.h"
#include "includes.h" //uC/OS��ͷ�ļ�
#include "../AppMain.h"
#include "Util.h"
/*************************************************
  Function:
  Description:    ����������ת���ת����������ݽ���ת����룬Ϊ�˲�ʹ��
                  ������ڴ棬��ͳ�Ʊ�ת����ֽڸ�����Ȼ���β����ʼ���룬
                  ����Ľ�����÷���ԭ�����ݵ���ʼ��ַ
  Calls:
  Called By:
  Input:          pucData:         ��ת�������
                  ulInputLen:      ��ת������ݵĳ���
                  ulBufSize:       ���ת�������ڴ泤��
                  ucEscapeChar:    ת���ַ�
                  pucAfterEscape:  ת�����ַ���
                  pucBeforeEscape: ת��ǰ���ַ�������ת�����ַ���һһ��Ӧ
                  ulEscapeNum:     ת���ַ�����С
  Output:         pucData:         ת��������
  Return:         0��ʾת��ʧ�ܣ�������ʾת�������ݳ���
  Others:
*************************************************/
ULONG EncodeEscape(UCHAR* pucData, ULONG ulInputLen, ULONG ulBufSize,
                   UCHAR ucEscapeChar, const UCHAR* pucAfterEscape, 
                   const UCHAR* pucBeforeEscape, UINT32 ulEscapeNum)
{
    ULONG ulEncodedLen = 0; //�Ѿ���������ݳ���
    ULONG i = 0;
    LONG lNextEncodedPos = 0;
    ULONG ulEscapeCnt = 0;

    //ͳ�Ʊ�ת����ַ��ĸ���
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
        return 0; //˵���ڴ�ĳ��Ȳ����Է��±����Ľ��
    }

    //��ʼת����룬�����һ���ֽڿ�ʼ
    ulEncodedLen = 0;
    lNextEncodedPos = ulInputLen + ulEscapeCnt - 1; //��β����ʼ��ű���Ľ��
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
  Description:    ���������������ֽ����ݵ�CRCУ����
  Calls:
  Called By:
  Input:          pucData: �����������
                  ulDataLen: ����������ݳ���
                  usCrcInitValue: CRC��ʼֵ�����ڷֶμ���CRCʱʹ�ã�������ֶ���ʹ��Ĭ��ֵ0
  Output:         
  Return:         ���ֽڵ�CRCУ����
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
                usCrc ^= 0x1021; //��ʽCRC����2����CRC
            }
            else
            {
                usCrc *= 2;
            }
            if((*pucData & i) != 0)
            {
                usCrc ^= 0x1021; //�ټ��ϱ�λ��CRC
            }
        }
        pucData++;
    }
    return usCrc;
} 


/*************************************************
  Function:
  Description:    ������������ʱ����λms��ֻ����
                       �в���ϵͳʱʹ�ã�����ʱʱ��Ϊ
                       MS_PER_TICK������������������ms������
                       MS_PER_TICK���������������һ�����
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
    time = ulTime/MS_PER_TICK;//MS_PER_TICK:TICKTIME�ĺ�����
    if(ulTime % MS_PER_TICK!=0)
    {
        time = time + 1;
    }
    OSTimeDly(time);
}


//***************���ҵ����ַ�***************
//���룺
//Ҫ���ҵ��ַ���pucStr��
//Ҫ���ҵ�uiNmum��ƥ���ַ�ucTarget��λ��
//���ҵ���󳤶�uiMaxlength
//�����Ҫ�����ַ���λ�ã���0��ʼ���������û���ҵ��򷵻�-1.
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
  Description:    �����������ڵ����ֽںϲ�Ϊһ���ֽڣ���ʾ���ַ���ʵ��ֵ��ÿ����
                  �����ֽڵ�ǰһ���ֽ���Ϊ����λ����һ���ֽ���Ϊ����λ�����ϲ�
                  ��һ���ֽڣ����ɵ����ݳ���Ϊԭ����һ�룬�����������ݳ���Ϊ��
                  ������ֻ�����䳤�ȼ�1������; �ϲ����ڴ�������ݿ��Ͻ��еģ���
                  �÷���Ҫ��֤�������ݿ����Ч��
  Calls:
  Called By:
  Input:          pucData:   ���ϲ�������
                  ulDataLen: ���ϲ������ݳ���
  Output:
  Return:         0:   ʧ��
                  ��0: �ϲ����ʵ�ʳ���
  Others:
*************************************************/
ULONG Merge2Bytes(UCHAR* pucData, ULONG ulDataLen)
{
    ULONG i = 0;
    ULONG j = 0;
    UCHAR ucMergedByte = 0;

    ulDataLen = (ulDataLen >> 1) << 1; //��������ȡΪż��
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
            else //�Ƿ��ַ�
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
  Description:    ���������ֽ����еĵ��ֽ����ݷֲ�������ֽڣ����ֽ��ÿɼ��ַ���
                  ʾ��4λ�����ֽ��ÿɼ��ַ���ʾ��4λ��������������볤�ȵ�����;
                  ������ڴ�������ݿ��Ͻ��еģ����÷���Ҫ��֤�������ݿ��㹻���
                  ��ֺ������
  Calls:
  Called By:
  Input:          pucData:   ����ֵ�����
                  ulDataLen: ����ֵ����ݳ���
  Output:
  Return:         0:   ʧ��
                  ��0: �ϲ����ʵ�ʳ���
  Others:
*************************************************/
ULONG SplitInto2Bytes(UCHAR *pucData, ULONG ulDataLen)
{
    LONG i = 0;
    UCHAR ucByte = 0;

    //���ڲ�����ڴ���������Ͻ��еģ�Ϊ�˲��ƻ�δ��������ݣ���ֱ����β����ʼ���������
    for(i = ulDataLen - 1; i >= 0; i--)
    {
        ucByte = (UCHAR)(pucData[i] & 0x0F);       //����λ
        if(ucByte <= 9)
        {
            pucData[(i << 1) + 1] = ucByte + 0x30;
        }
        else
        {
            pucData[(i << 1) + 1] = ucByte + 0x37;
        }

        ucByte = (UCHAR)((pucData[i] & 0xF0) >> 4); //����λ
        if(ucByte <= 9) //0��9
        {
            pucData[i << 1] = ucByte + 0x30;
        }
        else //A��F
        {
            pucData[i << 1] = ucByte + 0x37;
        }
    }

    return (ulDataLen << 1);
}

