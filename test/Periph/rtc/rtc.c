//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : lib_rtc.c
//* Object              : Real Time Clock Library File
//*
//* 1.0 06/09/00  JPP    : Creation
//*----------------------------------------------------------------------------

#include "./Rtc.h"

//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_close
//* Object              : close by stop Real Time Clock count
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : none
//*----------------------------------------------------------------------------
void RtcClose ()
//* Begin
{
    //* Stop the Real Time Clock Mode Register
    RTC_CR |= (RTC_UPDTIM |RTC_UPDCAL) ;
//* End
}
//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_open
//* Object              : open Real Time Clock count
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : none
//*----------------------------------------------------------------------------
void RtcOpen ()
//* Begin
{
    //* Set the Real Time Clock Mode Register
    RTC_CR  &= ~(RTC_UPDTIM |RTC_UPDCAL) ;

    //* clear status register
    RTC_SCR = RTC_ACKUPD | RTC_ALARM | RTC_SEC | RTC_TIMEV | RTC_CALEV;

    //* clear all interrup
    RTC_IDR = RTC_ACKUPD | RTC_ALARM | RTC_SEC | RTC_TIMEV | RTC_CALEV;
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_get_event
//* Object              : Read the event register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : none
//*----------------------------------------------------------------------------
UINT32 RtcGetEvent ()
//* Begin
{
    //* Read the event register
    return (RTC_CR ) ;
//* End
}
//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_set_event
//* Object              : set the event in Mode Register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//*                       <mode> new event value
//* Output Parameters   : none

//*----------------------------------------------------------------------------
void RtcSetEvent (UINT32 uiMode )
//* Begin
{
    UINT32 tmp;
    //*  set the event in Mode Register
    // Get the update value
    tmp = RTC_CR & RTC_UPD_MASQ ;
    // Update othr field
    tmp |= uiMode;
    RTC_CR = tmp ;
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_set_24
//* Object              : Set Real Time Clock Mode Register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//*                     : <mode> = set/clear bits in the mode register
//* Output Parameters   : none
//*----------------------------------------------------------------------------
void RtcSet24 ()
//* Begin
{
    //* Set the Real Time Clock Mode Register
    RTC_MR = RTC_24_HRMOD ;
//* End
}
//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_clear_24
//* Object              : Set Real Time Clock Mode Register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//*                     : <mode> = set/clear bits in the mode register
//* Output Parameters   : none
//*----------------------------------------------------------------------------
void RtcClear24 ()
//* Begin
{
    //* Set the Real Time Clock Mode Register
    RTC_MR = RTC_12_HRMOD ;
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_get_24
//* Object              : Read  Real Time Clock Mode Register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : <mode>  1 => 24 h 0 => AM / PM
//*----------------------------------------------------------------------------
UINT32 RtcGet24 ()
//* Begin
{
    //* Return the Real Time Clock Mode Register
    return (RTC_MR );
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_set_time
//* Object              : Set Real Time Clock Time Register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : <sec> = second in BCD
//*                       <min> = minute in BCD
//*                       <hour> = hour in BCD
//*                       <ampm> = 0 for AM or 1 for PM
//*                       如果是24小时模式，则<ampm> = 0
//*----------------------------------------------------------------------------
LONG RtcSetTime (UCHAR ucSec, UCHAR ucMin, UCHAR ucHour, UCHAR ucAmpm )
//* Begin
{
    //* clear status
    RTC_SCR = RTC_ACKUPD;

    //* Stop the RTC
    RTC_CR |= RTC_UPDTIM ;

    //* bit RTC_ACKUPD
    ULONG i = 0;
    while(((volatile INT32)RTC_SR & RTC_ACKUPD) != RTC_ACKUPD) 
    {
        //防止RTC运行不正常
        i++;
        if(i > 1000000)
        {
            RtcOpen();
            return FAILED;
        }
    }

    //* Set the Real Time Clock Time Register
    RTC_TIMR = ucSec | ( ucMin << RTC_MIN) | (ucHour <<RTC_HOUR) | ( ucAmpm << RTC_AMPM);

    //* Restart the RTC
    RTC_CR &= ~(RTC_UPDTIM) ;

    return SUCCEEDED;

//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_get_time
//* Object              : Read Real Time Clock Time Register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : <mode> = tim value
//*----------------------------------------------------------------------------
UINT32 RtcGetTime ()
//* Begin
{
    //* Return the Real Time Clock Time Register
    return ( RTC_TIMR );
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_set_calendar
//* Object              : Set Real Time Clock Calendar Register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : <cent> = century in BCD (19-20)
//*                       <year> = year in BCD (00-99)
//*                       <month> = month in BCD (01-12)
//*                       <day> = day in BCD (01-07)
//*                       <date> = date in BCD (01-31))
//*----------------------------------------------------------------------------
LONG RtcSetCalendar (UCHAR ucCent, UCHAR ucYear, UCHAR ucMonth, UCHAR ucDay, UCHAR ucDate )
//* Begin
{
    //* clear status
    RTC_SCR = RTC_ACKUPD;

    //* Stop the RTC
    RTC_CR |= RTC_UPDCAL ;

    //* bit RTC_ACKUPD
    ULONG i = 0;
    while(((volatile INT32)RTC_SR & RTC_ACKUPD) != RTC_ACKUPD) 
    {
        //防止RTC运行不正常
        i++;
        if(i > 1000000)
        {
            RtcOpen();
            return FAILED;
        }
    }

    //* Set the Real Time Clock Calendar Register
    RTC_CALR = ucCent | ( ucYear << RTC_YEAR) | (ucMonth <<RTC_MONTH) | ( ucDay << RTC_DAY)| ( ucDate << RTC_DATE);

    //* Restart the RTC
    RTC_CR &= ~(RTC_UPDCAL) ;

    return SUCCEEDED;
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_get_calendar
//* Object              : Read Real Time Clock Calendar Register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : <mode> = Calendar Register
//*----------------------------------------------------------------------------
UINT32 RtcGetCalendar ()
//* Begin
{
    //* Return the Real Time Clock Calendar Register
    return ( RTC_CALR );
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_set_time_alarm
//* Object              : Set Real Time Clock Time Alarm Register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : <sec> = second in BCD
//*                       <min> = minute in BCD
//*                       <hour> = hour in BCD
//*                       <ampm> = 0 for AM or 1 for PM
//*                       <mode> = Alarm type sec,  mimute, hours
//*----------------------------------------------------------------------------
void RtcSetTimeAlarm (UCHAR ucSec, UCHAR ucMin, UCHAR ucHour, UCHAR ucAmpm,UINT32 uiMode )
//* Begin
{
    //* Set the Real Time Clock Time Alarm Register
    RTC_TAR = ucSec | ( ucMin << RTC_MIN) | (ucHour <<RTC_HOUR) | ( ucAmpm << RTC_AMPM ) | uiMode;
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_get_time_alarm
//* Object              : Read Real Time Clock Time Alarm Register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : <mode> = set/clear bits in the mode register
//*----------------------------------------------------------------------------
UINT32 RtcGetTimeAlarm ()
//* Begin
{
    //* Return the Real Time Clock Time Alarm Register
    return ( RTC_TAR );
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_set_cal_alarm
//* Object              : Set Real Time Clock Calendar Alarm Register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : <month> = month in BCD (01-12)
//*                       <date> = date in BCD (01-31))
//*----------------------------------------------------------------------------
void RtcSetCalAlarm (UCHAR ucMonth, UCHAR ucDate, UINT32 uiMode )
//* Begin
{
    //* Set the Real Time Clock Calendar Alarm Register
    RTC_CAR =  (ucMonth <<RTC_MONTH) |( ucDate << RTC_DATE) | uiMode ;
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_get_cal_alarm
//* Object              : Read Real Time Clock Calendar Alarm Register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : <mode> = set/clear bits in the mode register
//*----------------------------------------------------------------------------
UINT32 RtcGetCalAlarm ()
//* Begin
{
    //* Return the Real Time Clock Calendar Alarm Register
    return ( RTC_CAR );
//* End
}

//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_get_status
//* Object              : Read Real Time Clock  Status Register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : <mode> = set/clear bits in the mode register
//*----------------------------------------------------------------------------
UINT32 RtcGetStatus ()
//* Begin
{
    //* Return the Real Time Clock Status Register
    return ( RTC_SR );
//* End
}
//*----------------------------------------------------------------------------
//* Function Name       : at91_rtc_clear_status
//* Object              : Clear status register
//* Input Parameters    : <rtc_desc> = RTC Descriptor pointer
//* Output Parameters   : none
//*----------------------------------------------------------------------------
void RtcClearStatus ()
//* Begin
{
    //* clear status register
    RTC_SCR = RTC_ACKUPD | RTC_ALARM | RTC_SEC | RTC_TIMEV | RTC_CALEV;
//* End
}

/*************************************************
  Function:     RtcGetDateTime
  Description:  本函数得到日期时间
  Calls:
  Called By:
  Input:        pucBCD:   BCD数字串，
数字串，由7个字节组成，用BCD码表示（年用4位，时间用24小时格式）
如：2005年1月18日，15点38分44秒，则用0x20，0x05，0x01，0x18，0x15，0x38，0x44表示。

  Output:       
  Return:         
  Others:       调用方保证指针所指空间有效
*************************************************/
void RtcGetDateTime (UCHAR *pucBCD)
//* Begin
{
    //* Return the Real Time Clock Calendar Register
    pucBCD[0] = (UCHAR)((RTC_CALR & RTC_MASQ_CENT) >> RTC_CENT);
    pucBCD[1] = (UCHAR)((RTC_CALR & RTC_MASQ_YEAR) >> RTC_YEAR);
    pucBCD[2] = (UCHAR)((RTC_CALR & RTC_MASQ_MONTH) >> RTC_MONTH);
    pucBCD[3] = (UCHAR)((RTC_CALR & RTC_MASQ_DATE) >> RTC_DATE);    
    pucBCD[4] = (UCHAR)((RTC_TIMR & RTC_MASQ_HOUR) >> RTC_HOUR);
    pucBCD[5] = (UCHAR)((RTC_TIMR & RTC_MASQ_MIN) >> RTC_MIN);
    pucBCD[6] = (UCHAR)(RTC_TIMR & RTC_MASQ_SEC);

//* End
}

/*************************************************
  Function:     RtcSetDateTime
  Description:  本函数得到日期时间
  Calls:
  Called By:
  Input:        pucBCD:   BCD数字串，
数字串，由7个字节组成，用BCD码表示（年用4位，时间用24小时格式）
如：2005年1月18日，15点38分44秒，则用0x20，0x05，0x01，0x18，0x15，0x38，0x44表示。

  Output:       
  Return:         
  Others:       调用方保证指针所指空间有效
*************************************************/
LONG RtcSetDateTime (UCHAR *pucBCD)
//* Begin
{
    //星期几无效，默认星期一
    if(RtcSetCalendar(pucBCD[0], pucBCD[1], pucBCD[2], 1, pucBCD[3]) == FAILED)
    {
        return FAILED;
    }
    //AMPM无效，默认24小时格式
    if(RtcSetTime(pucBCD[6], pucBCD[5], pucBCD[4], 0) == FAILED)
    {
        return FAILED;
    }

    return SUCCEEDED;
//* End
}

/*************************************************
  Function:     CompareDateTime
  Description:  本函数比较日期时间
  Calls:
  Called By:
  Input:        pucBCD:   BCD数字串，
数字串，由7个字节组成，用BCD码表示（年用4位，时间用24小时格式）
如：2005年1月18日，15点38分44秒，则用0x20，0x05，0x01，0x18，0x15，0x38，0x44表示。

  Output:       
  Return:       0:时间相等 1:pcuBCD1时间较早，2:pcuBCD2时间较早
  Others:       调用方保证指针所指空间有效
*************************************************/
UCHAR CompareDateTime (UCHAR *pucBCD1, UCHAR *pucBCD2)
//* Begin
{
    UINT32 i;
    for(i=0;i<7;i++)
    {
        if(pucBCD1[i] < pucBCD2[i])
        {
            return 1;
        }
        else if(pucBCD1[i] > pucBCD2[i])
        {
            return 2;
        }
    }
    return 0;
//* End
}

/*************************************************
  Function:     IsDateTime
  Description:  本函数判断是否是日期时间
  Calls:
  Called By:
  Input:        pucBCD:   BCD数字串，
数字串，由7个字节组成，用BCD码表示（年用4位，时间用24小时格式）
如：2005年1月18日，15点38分44秒，则用0x20，0x05，0x01，0x18，0x15，0x38，0x44表示。

  Output:       
  Return:       0:时间相等 1:pcuBCD1时间较早，2:pcuBCD2时间较早
  Others:       调用方保证指针所指空间有效
*************************************************/
UINT32 IsDateTime (UCHAR *pucBCD)
//* Begin
{
    if( (pucBCD[0] != 0x19) && (pucBCD[0] != 0x20) )
    {
        return FALSE;
    }
    if(pucBCD[1] > 0x99)
    {
        return FALSE;
    }
    if(pucBCD[2] > 0x12)
    {
        return FALSE;
    }
    if(pucBCD[3] > 0x31)
    {
        return FALSE;
    }
    if(pucBCD[4] > 0x23)
    {
        return FALSE;
    }
    if(pucBCD[5] > 0x59)
    {
        return FALSE;
    }
    if(pucBCD[6] > 0x59)
    {
        return FALSE;
    }
    return TRUE;
//* End
}
//* End of file
