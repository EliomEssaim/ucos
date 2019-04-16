//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : lib_rtc.h
//* Object              : Real Time Clock Library Function Prototyping File.
//*
//* 1.0 17/07/00  PF    : Creation
//*----------------------------------------------------------------------------

#ifndef RtcH
#define RtcH

#include "../../include/UserType.h"
#include "../../include/at91m55800.h"
#include "../../include/MyBoard.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------*/
/* RTC Descriptor Structure Definition */
/*-------------------------------------*/
//test
/*
typedef struct
{
    RTC_REG_ST          *pstRtcBase ;
    UCHAR               ucPeriphId ;
} RTC_DEV_ST;
*/

/* Function Prototyping */
extern void RtcClose ();
extern void RtcOpen ();

/* Status register */
extern UINT32 RtcGetEvent ();
extern void RtcSetEvent (UINT32 uiMode );
extern UINT32 RtcGetStatus ();
extern void RtcClearStatus ();

/* 24H AQM/ PM  mamagement */
extern void RtcSet24 ();
extern void RtcClear24 ();
extern UINT32 RtcGet24 ();

/* Time and calendar */
extern LONG RtcSetTime (UCHAR ucSec, UCHAR ucMin, UCHAR ucHour, UCHAR ucAmpm );
extern UINT32 RtcGetTime ();
extern LONG RtcSetCalendar (UCHAR ucCent, UCHAR ucYear, UCHAR ucMonth, UCHAR ucDay, UCHAR ucDate );
extern UINT32 RtcGetCalendar ();

extern void RtcGetDateTime (UCHAR *pucBCD);
extern LONG RtcSetDateTime (UCHAR *pucBCD);

extern UCHAR CompareDateTime (UCHAR *pucBCD1,UCHAR *pcuBCD2);
extern UINT32 IsDateTime (UCHAR *pucBCD);

/* Alarm mamagement */
extern void RtcSetTimeAlarm (UCHAR ucSec, UCHAR ucMin, UCHAR ucHour, UCHAR ucAmpm,UINT32 uiMode );
extern UINT32 RtcGetTimeAlarm ();
extern void RtcSetCalAlarm (UCHAR ucMonth, UCHAR ucDate, UINT32 uiMode );
extern UINT32 RtcGetCalAlarm ();

#ifdef __cplusplus
}
#endif

#endif  /* lib_rtc_h */
