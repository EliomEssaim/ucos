#ifndef UserTypeH
#define UserTypeH
                                                   /*ǰ׺*/
typedef unsigned long   ULONG; /*ul*/
typedef long            LONG;  /*l*/
typedef unsigned short  USHORT;/*us*/
typedef short           SHORT; /*s*/
typedef unsigned char   UCHAR; /*uc*/
typedef signed char     CHAR;  /*c*/ //gccĬ��charΪunsigned char
typedef float           FLOAT; /*f*/
typedef unsigned long   DWORD; /*dw*/
//typedef unsigned short  WORD;/*w*/
//typedef unsigned char   BYTE;/*by*/
#ifndef INT32_USED //��ֹ��BCB�ж����ͻ
typedef long            INT32; /*i*/
#endif
#ifndef UINT32_USED //��ֹ��BCB�ж����ͻ
typedef unsigned long   UINT32;/*ui*/
#endif
typedef int             BOOL;  /*b*/
typedef volatile unsigned long REG;/*r*/
typedef volatile unsigned short FLASHWORD ;/*fw*/

#ifdef SUCCEEDED
#undef SUCCEEDED
#endif
#define SUCCEEDED     0

#ifdef FAILED
#undef FAILED
#endif
#define FAILED        -1

#ifdef NULL
#undef NULL
#endif
#define NULL        0

#define TRUE     1
#define FALSE   0

#endif
 