/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   setfreq.c
    作者:     林玮
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  
       作者:  林玮
       描述:  编码完成
    2. 日期:  2006/11/01
       作者:  章杰
       描述:  修改问题MCM-11，把原代码中涉及机型判断的地方全部改为对
              机型配置表中某项或者多项值的判断。
    3. 日期:  2006/11/07
       作者:  章杰
       描述:  修改问题MCM-19，修改笔误，把devtype改为ucFreqArea 
---------------------------------------------------------------------------*/
/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
CPLD电路中关于衰耗器操作的步骤：
      1、用ATT_CS1、ATT_CS2来片选某一个功放或低噪放
      2、将衰耗值从ATT_1～ATT_16送出给CPLD
      3、将ATT_OUTEN拉低，将CPLD的锁存的数据发送出去
      4、适当延时后（如果需要延时的话），将ATT_OUTEN拉高

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "string.h"
#include "../../include/UserType.h"
#include "../../include/at91m55800.h"
#include "../../include/MyBoard.h"
#include "../../periph/pio/pio.h"
#include "../../OH/CMCC_1_Ext.h"
#include "../../periph/flash/Flash.h"
#include "../../periph/usart/Usart.h"

//#include "setfreq.h"

//********************************************
//               管脚连接情况                **
//********************************************
#define PIO_STATE &g_stPioA  //定义在发送数据时往PIOA口送
#define FLASH0BASE  g_stNorFlash0.pfwBaseAddr  //仅方便写代码

#define FRE_CK PA7  //选频操作时的时钟脚 PIOA口
#define FRE_DA PA8  //选频操作时的数据线 PIOA口
#define SET_FRE_CK  PioWrite( PIO_STATE, FRE_CK, PIO_SET_OUT)  //FRE_CK置“1”
  //#define SET_FRE_CK MA_OUTWM( PIO_SODR_A, FRE_CK, FRE_CK);
#define CLR_FRE_CK  PioWrite( PIO_STATE, FRE_CK, PIO_CLEAR_OUT)  //FRE_CK清“0”
  //#define CLR_FRE_CK MA_OUTWM( PIO_CODR_A, FRE_CK, FRE_CK);
#define SET_FRE_DA  PioWrite( PIO_STATE, FRE_DA, PIO_SET_OUT)  //FRE_DA置“1”
  //#define SET_FRE_DA MA_OUTWM( PIO_SODR_A, FRE_DA, FRE_DA);
#define CLR_FRE_DA  PioWrite( PIO_STATE, FRE_DA, PIO_CLEAR_OUT)  //FRE_DA清“0”
  //#define CLR_FRE_DA MA_OUTWM( PIO_CODR_A, FRE_DA, FRE_DA);

#define FRE_CE PA2  //选频操作时的使能脚  PIOA口
#define FRE_S0 PA3  //选频操作时的片选第0脚  PIOA口
#define FRE_S1 PA4  //选频操作时的片选第1脚  PIOA口
#define FRE_S2 PA5  //选频操作时的片选第2脚  PIOA口
#define FRE_S3 PA6  //选频操作时的片选第3脚  PIOA口
#define SET_FRE_CE  PioWrite( PIO_STATE, FRE_CE, PIO_SET_OUT)  //FRE_CE置“1”
  //#define SET_FRE_CE MA_OUTWM( PIO_SODR_A, FRE_CE, FRE_CE);
#define CLR_FRE_CE  PioWrite( PIO_STATE, FRE_CE, PIO_CLEAR_OUT)  //FRE_CE清“0”
  //#define CLR_FRE_CE MA_OUTWM( PIO_CODR_A, FRE_CE, FRE_CE);
#define MOVE_NUMBER 3 //因为FRE_S0是PA3，要选中第1路就必须将0x01左移3位
#define FRE_ALL_CTRL  FRE_CK|FRE_DA|FRE_CE|FRE_S0|FRE_S1|FRE_S2|FRE_S3


#define BYTE_BITLEN 8//字节长度
//上行采用低本振，下行采用高本振

#define UPLINK  -1//上行
#define DOWNLINK  1//下行

#define SW_H 1//SW bit为H
#define SW_L 0//SW bit为L
#define UP_LT 1//上限
#define DOWN_LT 0//下限

#define MB15E07SL_CS 1
#define MB15E07SL_LDS 0
#define MB15E07SL_FC 1

#define  PROG_REF_DIV_C 1//可编程参考分频器控制位(high Level)
#define  PROG_DIV_C 0//可编程分频器控制位(Low Level)

typedef enum EN_PLL{MB1502=0, MB1507=1, MB15E07SL=2} T_PLL_EN;//芯片分类
typedef struct ST_DATA
{
    USHORT BITLEN;//数据长度
    USHORT LIMIT[2];//取值门限(0=下限,1=上限)
}T_DATA_ST;
typedef struct ST_PLL//保存不同芯片的特有信息
{
    USHORT PROG_REF_BITLEN;//参考分频器串行数据位长度

    T_DATA_ST PROG_REF_COUNT;//参考计数器(R值)

    USHORT SW_PRES[2];//SW位H/L时的分频比例因子(1502中的P， 1507E1507SL中的M)

    T_DATA_ST SWAL_COUNT;//SWALLOW COUNTER DIVIDE RATIO(A值)

    T_DATA_ST PROG_COUNT;//分频计数器(N值)
}T_PLL_ST;
T_PLL_ST  ARR_PLL[4]={
    {16, {14, {8, 16383}}, {128, 64},  {7,{0, 127}}, {11, {16, 2047}}},
    {16, {14, {8, 16383}}, {256, 128}, {8,{0, 255}}, {11, {16, 2047}}},
    {19, {14, {3, 16383}}, {64, 32},   {7,{0, 127}}, {11, {3, 2047}}},
    {19, {14, {3, 16383}}, {64, 32},   {7,{0, 127}}, {11, {3, 2047}}}
};

typedef enum EN_FREQ_RANGE_TYPE{
    GSM900,
    EGSM900,
    GSM1800,//GSM1800(DCS)
    CDMA800,
    CDMA1900,
    GSM900_S,
    GSM900_256,   //参考分频比为256，适用于移频器900→1800
    WCDMA
}T_FREQ_RANGE_TYPE_EN;//直放站频段分类

typedef struct ST_FREQ_RANGE_TYPE//保存直放站频段分类信息
{
    ULONG INTER_FREQ;//中频频率(单位KHz)
    ULONG Fosc;//参考振荡器输出频率
    USHORT R;//参考计数器分频频率,R=[8，16383]如果遇到一种新的频段，该值不能够进行随便设置，必须通过计算找到一个满足条件的
           //满足的条件以MB1502为例A=[0,127]，A<N, N=[16, 2047]
    T_PLL_EN Chip;//使用的芯片
    UCHAR SW;//SW,该值决定了Prescaler,即P值
    UCHAR REF_LEN;//参考分频比长度
    ULONG REF_DATA;//参考分频比
    UCHAR SET_LEN;//串行设置值长度
}T_FREQ_RANGE_TYPE_ST;

T_FREQ_RANGE_TYPE_ST  ARR_FREQ_RANGE_TYPE[9] = {
    {71000, 12800, 64, MB1502, SW_H, 16, 0x008101, 19},     //GSM900
    {71000, 12800, 64, MB1502, SW_H, 16, 0x008101, 19},     //EGSM900
    {140000, 12800, 256, MB1507, SW_H, 16, 0x008041, 20},   //GSM1800
    {70020, 12800, 1280, MB1502, SW_H, 16, 0x008051, 19},   //CDMA800
    {115000, 12800, 256, MB1507, SW_H, 16, 0x008041, 20},   //CDMA1900
    {90000, 12800, 256, MB15E07SL, SW_H, 19, 0x04020D, 19},  //GSM900_S
    {140000, 12800, 256, MB1502, SW_H, 16, 0x008041, 19},   //GSM900_256 参考分频比为256，适用于移频器900→1800
    {160000, 12800, 64, MB15E07SL, SW_H, 19, 0x04080D, 19}, //WCDMA
    {90000, 12800, 256, MB15E07SL, SW_H, 19, 0x04020D, 19}  //GSM900_S
};


//********************************************
//                全局变量                  **
//********************************************
void FreqDataOutput(ULONG ulFreqRate,ULONG ulFreqData,
        UCHAR RATE_LENGTH,UCHAR DATA_LENGTH,UCHAR NUMBER);
ULONG ShiftULONG(ULONG ulValue, UCHAR uiCount, UCHAR isLeft);
ULONG ReverseInt(ULONG ulSrc, UCHAR uiLen);
UCHAR EncodeDivOneBitData(
    T_FREQ_RANGE_TYPE_EN frtHndlFreqType,
    ULONG ulFreq,
    signed char cUpDown,//上下行
    ULONG* pulSeriData
);
ULONG CountFreq(USHORT usTempFreqNum, UCHAR ucUpOrDown);
UCHAR FreqToArea(USHORT usTempFreqNum);
void SetFreqInit(void);
void SetFreq(void);


extern DEV_INFO_SET_ST  g_stDevInfoSet;
extern SETTING_PARAM_SET_ST g_stSettingParamSet;
extern DEV_TYPE_TABLE_ST g_stDevTypeTable;

//********************************************
//                程序实现                  **
//********************************************
//---------------------------------------------------------------------------
/*********************************************************
 * 函数名：为C51设计的4字节变量移位操作
 * 输入：4字节变量
 * 输出：4字节变量
 * 返回值：
 * 说明：
 * 作者：
*********************************************************/
ULONG ShiftULONG(ULONG ulValue, UCHAR uiCount, UCHAR isLeft)
{
    USHORT usH, usL;//存放高字节和低字节
    UCHAR i=1;
    int iTemp=0;

    USHORT ausValue[2] = {0};//0存放低字节、1存放高字节

    memcpy(ausValue, &ulValue, sizeof(ulValue));

    usL = ausValue[0];
    usH = ausValue[1];

    for (; i<=uiCount; i++)
    {
        if (isLeft)
        {
            usH <<= 1;
            iTemp = (usL&0x8000) ? 1:0;
            usH += iTemp;
            usL <<= 1;
        }
        else
        {
            usL >>= 1;
            iTemp = (usH&0x0001) ? 0x8000:0;
            usL += iTemp;
            usH >>= 1;
        }
    }

    return (ULONG)usH*0x10000 + usL;
}
//---------------------------------------------------------------------------
/*********************************************************
 * 函数名：将指定长度的bit进行颠倒转换
 * 输入：被处理数据，颠倒bit长度
 * 输出：
 * 返回值：返回颠倒结果，如果长度大于被处理结果的位长返回0
 * 说明：
 * 作者：
 * 举例： (0x00FF0000, 32)->0x0000FF00、(0x000008201, 16)->0x00008041
*********************************************************/
ULONG ReverseInt(ULONG ulSrc, UCHAR uiLen)
{
    ULONG ulSrcBak = ulSrc;
    ULONG uiRsl = 0;
    UCHAR i = 0;

    if (uiLen > sizeof(ulSrc)*BYTE_BITLEN) return 0;

    for (i=0; i<uiLen; i++)
    {
        if ((ulSrcBak & 0x00000001)==1)
        {
            uiRsl += ShiftULONG(1, uiLen-i-1, TRUE);
        }
        ulSrcBak = ShiftULONG(ulSrcBak, 1, FALSE);
    }
    return uiRsl;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*********************************************************
 * 函数：计算一个可编程分频器设置值Serial data
 * 说明:
 * 输入：处理的频段，处理频率
 * 输出：可编程分频器串行数据，
 * 返回：结果，0=成功，其他=错误编码
 * 举例:
*********************************************************/
UCHAR EncodeDivOneBitData(
    T_FREQ_RANGE_TYPE_EN frtHndlFreqType,
    ULONG ulFreq,
    signed char cUpDown,//上下行
    ULONG* pulSeriData
)
{
    UCHAR bErr = 0;
    ULONG ulP = 0;
    USHORT usN = 0;  //UINT32 uiN = 0;
    UCHAR ucA = 0;
    ULONG ulFosc = 0;
    ULONG ulFvco = 0;
    USHORT usR = 0;  //UINT32 usR = 0;
    ULONG ulTemp = 0;

    T_FREQ_RANGE_TYPE_ST* pfrtType = &(ARR_FREQ_RANGE_TYPE[frtHndlFreqType]);
    T_PLL_ST* pmbPLL = &(ARR_PLL[pfrtType->Chip]);

    ulP = pmbPLL->SW_PRES[pfrtType->SW];
    usR = pfrtType->R;
    ulFosc = pfrtType->Fosc;
    if(frtHndlFreqType == WCDMA)
    {
        ulFvco = ulFreq - pfrtType->INTER_FREQ;
    }
    else
    {
        ulFvco = ulFreq + cUpDown * pfrtType->INTER_FREQ;
    }

    //检查R参数
    if ((usR > pmbPLL->PROG_REF_COUNT.LIMIT[UP_LT]) || (usR < pmbPLL->PROG_REF_COUNT.LIMIT[DOWN_LT]))
    {
        bErr = 1;
        return bErr;
    }


    if (ulFvco * usR % ulFosc != 0)
    {
        bErr = 2;
        return bErr;
    };
    ulTemp = ulFvco * usR / ulFosc;

    usN = ulTemp / ulP;
    ucA = ulTemp % ulP;

    if (ucA>=usN)
    {
        bErr = 3;
        return bErr;
    }

    //检查A参数是否正确
    if ((ucA > pmbPLL->SWAL_COUNT.LIMIT[UP_LT]) || (ucA < pmbPLL->SWAL_COUNT.LIMIT[DOWN_LT]))
    {
        bErr = 4;
        return bErr;
    }
    //检查N参数是否正确
    if ((usN > pmbPLL->PROG_COUNT.LIMIT[UP_LT]) || (usN < pmbPLL->PROG_COUNT.LIMIT[DOWN_LT]))
    {
        bErr = 5;
        return bErr;
    }

    //计算串行数据
    *pulSeriData = ShiftULONG(usN, pmbPLL->SWAL_COUNT.BITLEN+1, TRUE) +
                   ShiftULONG(ucA, 1, TRUE) +
                   PROG_DIV_C;
    *pulSeriData = ReverseInt(*pulSeriData, pmbPLL->SWAL_COUNT.BITLEN+pmbPLL->PROG_COUNT.BITLEN+1);

    return bErr;
}


//*********************************************
//  由信道号通过计算得到对应的频率值(单位KHz)
//         usTempFreqNum --- 信道号  
//  ucUpOrDown=0 上行    ucUpOrDown=1 下行
//  返回：频率值(单位KHz)
//---------------------------------------------
ULONG CountFreq(USHORT usTempFreqNum, UCHAR ucUpOrDown)
{
    ULONG ulTempReturn;
    
    if(usTempFreqNum <= 124)
    {  //fup(n)=890+0.2n(MHz) 0≤n≤124   fdown(n)=fup(n)+45(MHz)
        ulTempReturn = (ULONG)(usTempFreqNum)*200 + 890000 + 45000*((ULONG)(ucUpOrDown));
    }
    else if((usTempFreqNum >= 975) && (usTempFreqNum <= 1023))
    {  //fup(n)=890+0.2(n-1024)(MHz) 975≤n≤1023  fdown(n)=fup(n)+45(MHz)
        ulTempReturn = 890000 - (1024-(ULONG)(usTempFreqNum))*200 + 45000*((ULONG)(ucUpOrDown));
    }
    else if((usTempFreqNum >= 512) && (usTempFreqNum <= 885))
    {  //fup(n)=1710.2+0.2(n-512)(MHz) 975≤n≤1023  fdown(n)=fup(n)+95(MHz)
        ulTempReturn = ((ULONG)(usTempFreqNum)-512)*200 + 1710200 + 95000*((ULONG)(ucUpOrDown));
    }
    else if((usTempFreqNum >= 9600) && (usTempFreqNum <= 10837))
    {
        //fup(n)=1922.4+0.2(n-10562)(MHz) 10562≤n≤10837  fdown(n)=fup(n)+190(MHz)
        ulTempReturn = 1922400 + ((ULONG)(usTempFreqNum)-10562)*200 + 190000*((ULONG)(ucUpOrDown));
    }
    else ulTempReturn = 0;
    return ulTempReturn;
}

//*********************************************
//        由信道号得到对应的频率范围
//           usTempFreqNum --- 信道号  
//        返回：频率范围
//---------------------------------------------
UCHAR FreqToArea(USHORT usTempFreqNum)
{
    if(usTempFreqNum <= 124)
    {
        return GSM900;
    }
    else if((usTempFreqNum >= 975) && (usTempFreqNum <= 1023))
    {
        return EGSM900;
    }
    else if((usTempFreqNum >= 512) && (usTempFreqNum <= 885))
    {
        return GSM1800;
    }
    else if((usTempFreqNum >= 9600) && (usTempFreqNum <= 10837))
    {
        return WCDMA;
    }
    else return 0;
}

//======================================================================
//功能：	选频器设置的初始化
//最后编写时间：2006年3月14日
//======================================================================
void SetFreqInit(void)
{
    PioOpen( PIO_STATE, FRE_ALL_CTRL, PIO_OUTPUT); //设置所有ATT管脚为输出状态，并受PIO控制
}

//*************************************************************
//函数用途：执行计算好的分频比和频点设置值
//输入参数：ulFreqRate -- 分频比   RATE_LENGTH -- 分频比有效数据位个数
//			ulFreqData -- 频点设置值  DATA_LENGTH -- 频点设置值有效数据位个数
//			NUMBER -- 通道号
//函数返回：无
//-------------------------------------------------------------
void FreqDataOutput(ULONG ulFreqRate,ULONG ulFreqData,UCHAR RATE_LENGTH,UCHAR DATA_LENGTH,UCHAR NUMBER)
{
    UCHAR i,j;
    USHORT TEMP_HL;
    USHORT Value[2] = {0, 0};//0存放低字节、1存放高字节

/********* test *********
        i = 0xff;
        UsartWrite(&g_stUsart0, &i, 1);
        while(g_stUsart0.pstData->pucTxIn!=g_stUsart0.pstData->pucTxOut);
/************************/

    memcpy(Value, &ulFreqRate, sizeof(ulFreqRate));

    TEMP_HL = Value[0];
    
    CLR_FRE_CE;
    CLR_FRE_CK;  //FRE_CK=0;
    j = 250;
    while(j--);  //延时若干时间，让数据稳定
    j = 250;
    while(j--);  //延时若干时间，让数据稳定

    for(i=0;i<16;i++)  //发送分频比
    {
        if((TEMP_HL & 0x00000001) == 0)
        {
            CLR_FRE_DA;  //FRE_DA=0;
        }
        else SET_FRE_DA;  //FRE_DA=1;
        TEMP_HL >>= 1;
        SET_FRE_CK;  //FRE_CK=1;
        j = 65;
        while(j--);  //延时若干时间，让数据稳定
        CLR_FRE_CK;  //FRE_CK=0;
        j = 60;
        while(j--);  //延时若干时间，让数据稳定
    }
    TEMP_HL = Value[1];
    for(i=0;i<(RATE_LENGTH-16);i++)  //发送分频比
    {
        if((TEMP_HL & 0x00000001) == 0)
        {
            CLR_FRE_DA;  //FRE_DA=0;
        }
        else SET_FRE_DA;  //FRE_DA=1;
        TEMP_HL >>= 1;
        SET_FRE_CK;  //FRE_CK=1;
        j = 65;
        while(j--);  //延时若干时间，让数据稳定
        CLR_FRE_CK;  //FRE_CK=0;
        j = 60;
        while(j--);  //延时若干时间，让数据稳定
    }
    TEMP_HL = ((NUMBER - 1) << MOVE_NUMBER);
    DataToPio(PIO_STATE, TEMP_HL, FRE_S0|FRE_S1|FRE_S2|FRE_S3); //片选数据发送
    j = 250;
    while(j--);  //延时若干时间，让数据稳定
    SET_FRE_CE;
    j = 100;
    while(j--);  //延时若干时间，让数据稳定
    CLR_FRE_CE;
    j = 250;
    while(j--);  //延时若干时间，让数据稳定
    j = 250;
    while(j--);  //延时若干时间，让数据稳定
    j = 250;
    while(j--);  //延时若干时间，让数据稳定

 //------------- 以上完成分频比设置，一下执行信道设置 ---------------
 
    memcpy(Value, &ulFreqData, sizeof(ulFreqData));

    TEMP_HL = Value[0];

    for(i=0;i<16;i++)  //发送分频比
    {
        if((TEMP_HL & 0x00000001) == 0)
        {
            CLR_FRE_DA;  //FRE_DA=0;
        }
        else SET_FRE_DA;  //FRE_DA=1;
        TEMP_HL >>= 1;
        SET_FRE_CK;  //FRE_CK=1;
        j = 65;
        while(j--);  //延时若干时间，让数据稳定
        CLR_FRE_CK;  //FRE_CK=0;
        j = 60;
        while(j--);  //延时若干时间，让数据稳定
    }
    TEMP_HL = Value[1];
    for(i=0;i<(DATA_LENGTH - 16);i++)  //发送分频比
    {
        if((TEMP_HL & 0x00000001) == 0)
        {
            CLR_FRE_DA;  //FRE_DA=0;
        }
        else SET_FRE_DA;  //FRE_DA=1;
        TEMP_HL >>= 1;
        SET_FRE_CK;  //FRE_CK=1;
        j = 65;
        while(j--);  //延时若干时间，让数据稳定
        CLR_FRE_CK;  //FRE_CK=0;
        j = 60;
        while(j--);  //延时若干时间，让数据稳定
    }
    TEMP_HL = ((NUMBER - 1) << MOVE_NUMBER);
    DataToPio(PIO_STATE, TEMP_HL, FRE_S0|FRE_S1|FRE_S2|FRE_S3); //片选数据发送
    j = 250;
    while(j--);  //延时若干时间，让数据稳定
    SET_FRE_CE;
    j = 100;
    while(j--);  //延时若干时间，让数据稳定
    CLR_FRE_CE;
    j = 250;
    while(j--);  //延时若干时间，让数据稳定
    j = 250;
    while(j--);  //延时若干时间，让数据稳定
    j = 250;
    while(j--);  //延时若干时间，让数据稳定

}

/*********************************************************
EGSM900系统上行信道：  fu1(n)=890+0.2n (MHz) ，0≤n≤124
                       fu2(n)= (MHz) ，975≤n≤1023
下行信道：             fd(n)=fu(n)+45 (MHz)，0≤n≤124
GSM1800系统上行信道：fu(n)=1710.2+0.2(n-512) (MHz)，512≤n≤885
下行信道：           fd(n)=fu(n)+95 (MHz)，512≤n≤885
*********************************************************/

//*************************************************************
//函数用途：设置分频比和信道号，控制外部频率器件
//输入参数：devtype -- 设备类型
//备    注：该函数被应用层调用，已收录在setfreq.h头文件中
//*************************************************************
void SetFreq(void)  //devtype--设备类型  
{
    UCHAR ucFreqRateLength, ucFreqDataLength, ucFreqArea;
        //ucFreqRateLength--分频比有效位数  ucFreqDataLength--频点设置值有效位数
    USHORT usFreqNum;
    ULONG ulFreqRate, ulFreqData;  //ulFreqRate--分频比 ulFreqData--频点设置值

    switch (g_stDevTypeTable.ucSetFreqType)
    {
        //@@@@@@@@@@@@@@@@@ 宽带型 @@@@@@@@@@@@@@@@@
    case FREQTYPE_WIDEBAND:
        //                                   读取工作频带下边带信道号（通道1-下边带-上行 1）
        usFreqNum = g_stSettingParamSet.usWorkLbChNum1;
        ucFreqArea=FreqToArea(usFreqNum);  //暂时存放频点所属频段信息。
        //******** 2006-07-18增加修改的，兼容2G和3G ********
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)
        {
            ucFreqArea = GSM900_S;
        }
//—（）%……*￥……·#￥#%·#￥%……#￥—（）%……*￥……·#￥#%·#￥—（）%
        ulFreqRate = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_DATA;
        ucFreqRateLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_LEN;
        ucFreqDataLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].SET_LEN;
//—（）%……*￥……·#￥#%·#￥%……#￥—（）%……*￥……·#￥#%·#￥—（）%
        
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)+12000,-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,1);

        //                                       工作频带下边带信道号（通道1-下边带-下行 9）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)+12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,9);
        //----------------------
        //                                       工作频带上边带信道号（通道1-上边带-上行 2）
        usFreqNum = g_stSettingParamSet.usWorkUbChNum1;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)-12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,2);

        //                                       工作频带上边带信道号（通道1-上边带-下行 10）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)-12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,10);
        //------------------------------------------------
        //                                       工作频带下边带信道号（通道2-下边带-上行 3）
        usFreqNum = g_stSettingParamSet.usWorkLbChNum2;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)+12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,3);

        //                                       工作频带下边带信道号（通道2-下边带-下行 11）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)+12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,11);
        //----------------------
        //                                       工作频带上边带信道号（通道2-上边带-上行 4）
        usFreqNum = g_stSettingParamSet.usWorkUbChNum2;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)-12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,4);

        //                                       工作频带上边带信道号（通道2-上边带-下行 12）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)-12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,12);
        //------------------------------------------------
        //                                       工作频带下边带信道号（通道3-下边带-上行 5）
        usFreqNum = g_stSettingParamSet.usWorkLbChNum3;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)+12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,5);

        //                                      工作频带下边带信道号（通道3-下边带-下行 13）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)+12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,13);
        //----------------------
        //                                       工作频带上边带信道号（通道3-上边带-上行 6）
        usFreqNum = g_stSettingParamSet.usWorkUbChNum3;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)-12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,6);

        //                                       工作频带上边带信道号（通道3-上边带-下行 14）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)-12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,14);
        //------------------------------------------------
        //                                       工作频带下边带信道号（通道4-下边带-上行 7）
        usFreqNum = g_stSettingParamSet.usWorkLbChNum4;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)+12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,7);

        //                                       工作频带下边带信道号（通道4-下边带-下行 15）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)+12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,15);
        //----------------------
        //                                       工作频带上边带信道号（通道4-上边带-上行 8）
        usFreqNum = g_stSettingParamSet.usWorkUbChNum4;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0)-12000,-1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,8);

        //                                       工作频带上边带信道号（通道4-上边带-下行 16）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1)-12000,1,&ulFreqData);

        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,16);
        
        break;
        //@@@@@@@@@@@@@@@@@ 宽带型 @@@@@@@@@@@@@@@@@
        
        //@@@@@@@@@@@@@@@@@ 选频型 @@@@@@@@@@@@@@@@@
    case FREQTYPE_FRESELT: 

        //I2cReadMore(&usFreqNum,&mSetPar.ChaNum1,2); //工作信道号1（通道1-上行 1）
        usFreqNum = g_stSettingParamSet.usWorkChNum1;

//—（）%……*￥……·#￥#%·#￥%……#￥—（）%……*￥……·#￥#%·#￥—（）%
        ucFreqArea=FreqToArea(usFreqNum);  //暂时存放频点所属频段信息。
        ulFreqRate = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_DATA;
        ucFreqRateLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_LEN;
        ucFreqDataLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].SET_LEN;
//—（）%……*￥……·#￥#%·#￥%……#￥—（）%……*￥……·#￥#%·#￥—（）%

        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,1);

        //                                            工作信道号1（通道1-下行 9）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,9);
        //--------------------------
        //                                            工作信道号2（通道2-上行 2）
        usFreqNum = g_stSettingParamSet.usWorkChNum2;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,2);

        //                                            工作信道号2（通道2-下行 10）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,10);
        //--------------------------
        //                                            工作信道号3（通道3-上行 3）
        usFreqNum = g_stSettingParamSet.usWorkChNum3;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,3);

        //                                            工作信道号3（通道3-下行 11）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,11);
        //--------------------------
        //                                             工作信道号4（通道4-上行 4）
        usFreqNum = g_stSettingParamSet.usWorkChNum4;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,4);

        //                                            工作信道号4（通道4-下行 12）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,12);
        //--------------------------
        //                                            工作信道号5（通道5-上行 5）
        usFreqNum = g_stSettingParamSet.usWorkChNum5;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,5);

        //                                            工作信道号5（通道5-下行 13）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,13);
        //--------------------------
        //                                             工作信道号6（通道6-上行 6）
        usFreqNum = g_stSettingParamSet.usWorkChNum6;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,6);

        //                                            工作信道号6（通道6-下行 14）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,14);
        //--------------------------
        //                                             工作信道号7（通道7-上行 7）
        usFreqNum = g_stSettingParamSet.usWorkChNum7;
        //MCM-19_20061107_zhangjie_begin
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        //MCM-19_20061107_zhangjie_end
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,7);

        //                                            工作信道号7（通道7-下行 15）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,15);
        //--------------------------
        //                                             工作信道号8（通道8-上行 8）
        usFreqNum = g_stSettingParamSet.usWorkChNum8;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,8);

        //                                             工作信道号8（通道8-下行 16）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,16);
        break;
        //@@@@@@@@@@@@@@@@@ 选频型 @@@@@@@@@@@@@@@@@
        
        //@@@@@@@@@@@@@@@@@ 移频型 @@@@@@@@@@@@@@@@@
    case FREQTYPE_FRESHIFT:    
        //                                            工作信道号1（上行  1）
        usFreqNum = g_stSettingParamSet.usWorkChNum1;

//—（）%……*￥……·#￥#%·#￥%……#￥—（）%……*￥……·#￥#%·#￥—（）%
        ucFreqArea=FreqToArea(usFreqNum);  //暂时存放频点所属频段信息。
        //******** 2006-07-18增加修改的，兼容2G和3G ********
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)
        {
            ucFreqArea = GSM900_256;
        }
        ulFreqRate = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_DATA;
        ucFreqRateLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_LEN;
        ucFreqDataLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].SET_LEN;
//—（）%……*￥……·#￥#%·#￥%……#￥—（）%……*￥……·#￥#%·#￥—（）%

        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,1);

        //                                            工作信道号1（下行  9）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,9);
        //--------------------------
        //                                             工作信道号2（上行  2）
        usFreqNum = g_stSettingParamSet.usWorkChNum2;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,2);

        //                                            工作信道号2（下行  10）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,10);
        //--------------------------
        //                                             工作信道号3（上行  5/3）
        usFreqNum = g_stSettingParamSet.usWorkChNum3;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,5);

        //                                            工作信道号3（下行  13/11）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,13);
        //--------------------------
        //                                             工作信道号4（上行  6/4）
        usFreqNum = g_stSettingParamSet.usWorkChNum4;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,6);

        //                                            工作信道号4（下行  14/12）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,14);
        
        //--------------------------------------------------------
        //                                                移频信道号1（上行  3/5）
        usFreqNum = g_stSettingParamSet.usShfChNum1;

//—（）%……*￥……·#￥#%·#￥%……#￥—（）%……*￥……·#￥#%·#￥—（）%
        ucFreqArea=FreqToArea(usFreqNum);  //暂时存放频点所属频段信息。
        ulFreqRate = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_DATA;
        ucFreqRateLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_LEN;
        ucFreqDataLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].SET_LEN;
//—（）%……*￥……·#￥#%·#￥%……#￥—（）%……*￥……·#￥#%·#￥—（）%

        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,3);

        //                                                移频信道号1（下行  11/13）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,11);
        //--------------------------
        //                                                  移频信道号2（上行  4/6）
        usFreqNum = g_stSettingParamSet.usShfChNum2;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,4);

        //                                                移频信道号2（下行  12/14）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,12);
        //--------------------------
        //                                                  移频信道号3（上行  7）
        usFreqNum = g_stSettingParamSet.usShfChNum3;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,7);

        //                                                 移频信道号3（下行  15）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,15);
        //--------------------------
        //                                                  移频信道号4（上行  8）
        usFreqNum = g_stSettingParamSet.usShfChNum4;
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,0),-1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,8);

        //                                                移频信道号4（下行  16）
        EncodeDivOneBitData(ucFreqArea,CountFreq(usFreqNum,1),1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,16);
        
        break;
        //@@@@@@@@@@@@@@@@@ 移频型 @@@@@@@@@@@@@@@@@

        //@@@@@@@@@@@@@@@ 移频宽带型 @@@@@@@@@@@@@@@
    case FREQTYPE_FRESHIFT_WIDEBAND:
    
        //                                             工作频带的下边带（通道1）（上行  1）
        usFreqNum = g_stSettingParamSet.usWorkLbChNum1;

//—（）%……*￥……·#￥#%·#￥%……#￥—（）%……*￥……·#￥#%·#￥—（）%
        ucFreqArea=FreqToArea(usFreqNum);  //暂时存放频点所属频段信息。
        //******** 2006-07-18增加修改的，兼容2G和3G ********
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)
        {
            ucFreqArea = GSM900_S;
        }
        ulFreqRate = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_DATA;
        ucFreqRateLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].REF_LEN;
        ucFreqDataLength = ARR_FREQ_RANGE_TYPE[ucFreqArea].SET_LEN;
//—（）%……*￥……·#￥#%·#￥%……#￥—（）%……*￥……·#￥#%·#￥—（）%
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)+12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,1);

        //                                              工作频带的下边带（通道1）（下行  9）
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)+12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,9);
        //--------------------------
        //                                              工作频带的上边带（通道1）（上行  2）
        usFreqNum = g_stSettingParamSet.usWorkUbChNum1;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)-12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,2);

        //                                              工作频带的上边带（通道1）（下行  10）
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)-12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,10);
        
        //----------------------------------------------------
        //                                              工作频带的下边带（通道2）（上行  3）
        usFreqNum = g_stSettingParamSet.usWorkLbChNum2;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)-12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,3);

        //                                              工作频带的下边带（通道2）（下行  11）
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)-12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,11);
        //--------------------------
        //                                              工作频带的上边带（通道2）（上行  4）
        usFreqNum = g_stSettingParamSet.usWorkUbChNum2;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)-12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,4);

        //                                              工作频带的上边带（通道2）（下行  12）
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)-12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,12);
        
        //￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥￥
        //                                              移频频带的下边带（通道1）（上行  5）
        usFreqNum = g_stSettingParamSet.usShfLbChNum1;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)+12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,5);

        //                                              移频频带的下边带（通道1）（下行 13）
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)+12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,13);
        //--------------------------
        //                                              移频频带的上边带（通道1）（上行  6）
        usFreqNum = g_stSettingParamSet.usShfUbChNum1;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)-12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,6);

        //                                               移频频带的上边带（通道1）（下行 14）
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)-12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,14);
        //--------------------------------------------------
        //                                               移频频带的下边带（通道2）（上行  7）
        usFreqNum = g_stSettingParamSet.usShfLbChNum2;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)+12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,7);

        //                                               移频频带的下边带（通道2）（下行 15）
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)+12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,15);
        //--------------------------
        //                                               移频频带的上边带（通道2）（上行  8）
        usFreqNum = g_stSettingParamSet.usShfUbChNum2;
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,0)-12000, -1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,8);

        //                                               移频频带的上边带（通道2）（下行 16）
        EncodeDivOneBitData(ucFreqArea, CountFreq(usFreqNum,1)-12000, 1,&ulFreqData);
        FreqDataOutput(ulFreqRate,ulFreqData,ucFreqRateLength,ucFreqDataLength,16);
        
        break;
        //@@@@@@@@@@@@@@@ 移频宽带型 @@@@@@@@@@@@@@@

    default: break;
    }
}


/*********************************************************
EGSM900系统上行信道：
                          fu1(n)=890+0.2n (MHz) ，0≤n≤124
                          fu2(n)=890+0.2(n-1024) (MHz) ，975≤n≤1023
下行信道：
                          fd(n)=fu(n)+45 (MHz)，0≤n≤124
GSM1800系统上行信道：
                        fu(n)=1710.2+0.2(n-512) (MHz)，512≤n≤885
下行信道：
                        fd(n)=fu(n)+95 (MHz)，512≤n≤885
*********************************************************/
