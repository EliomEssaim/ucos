#ifndef UtilH
#define UtilH

#include "UserType.h"
//ʹ��FIND_NO_CHAR�ڼӼ�1��2ʱ�����Ϊһ����Ч�Ĳ���λ��
//�����ڲ���0D0A����3��AAʱ�Ƚ������ж�
#define FIND_NO_CHAR 0xFFFFFFF0

#define SET_BITMAP(Byte, Bitmap)     ((Byte) |= (Bitmap))
#define CLEAR_BITMAP(Byte, Bitmap)   ((Byte) &= (~Bitmap))
#define IS_BITMAP_SET(Byte, Bitmap)  ((Byte) & (Bitmap))

ULONG EncodeEscape(UCHAR* pucData, ULONG ulInputLen, ULONG ulBufSize,
                   UCHAR ucEscapeChar, const UCHAR* pucAfterEscape, 
                   const UCHAR* pucBeforeEscape, UINT32 ulEscapeNum);
UINT32 DecodeEscape(UCHAR* pucData, UINT32 uiLen, UCHAR ucEscapeChar,
                    const UCHAR* pucEscaped, const UCHAR* pucUnEscaped, UINT32 uiEscapeNum);
USHORT CalcuCRC(const UCHAR* pucData, ULONG ulDataLen, USHORT usCrcInitValue = 0);
void MyDelay(USHORT ulTime);
UINT32 FindChar(UCHAR* pucStr, UCHAR ucTarget, UINT32 uiNum, UINT32 uiMaxlength);
ULONG Merge2Bytes(UCHAR* pucData, ULONG ulDataLen);
ULONG SplitInto2Bytes(UCHAR *pucData, ULONG ulDataLen);

#endif
