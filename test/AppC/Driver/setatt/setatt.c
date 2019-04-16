/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   setatt.c
    作者:     林玮
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  
       作者:  林玮
       描述:  代码编写完成
    2. 日期:  2006/11/06
       作者:  林雨
       描述:  修改问题MCM-17，将所有带通道号的通道1的参数都映射对应的
              到不带通道号的参数上，即当设备是多通道时，查询通道1的参
              数实际访问的是对应的不带通道号的参数的地址。
---------------------------------------------------------------------------*/
/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
CPLD电路中关于衰耗器操作的步骤：
      1、用ATT_CS1、ATT_CS2来片选某一个功放或低噪放
      2、将衰耗值从ATT_1～ATT_16送出给CPLD
      3、将ATT_OUTEN拉低，将CPLD的锁存的数据发送出去
      4、适当延时后（如果需要延时的话），将ATT_OUTEN拉高

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "../../include/UserType.h"
#include "../../include/at91m55800.h"
#include "../../include/MyBoard.h"
#include "../../periph/pio/pio.h"
#include "../../OH/CMCC_1_Ext.h"
#include "../../OH/YKPP.h"
#include "../../periph/usart/Usart.h"
//#include "setatt.h"

//********************************************
//               管脚连接情况               **
//********************************************

#define PIO_STATE &g_stPioB  //定义在发送数据时往PIOB口送

#define ATT_CS1  PB6  //衰耗器1的片选  PIOB口
#define ATT_CS2  PB7  //衰耗器2的片选  PIOB口
#define ATT_OUTEN PB5  //衰耗器数据发送使能  PIOB口
//将衰耗器数据发送使能拉高，使ATT处于就绪状态
#define SET_CS PioWrite( PIO_STATE, ATT_OUTEN, PIO_SET_OUT)
  //#define SET_CS MA_OUTWM( PIO_SODR_B, ATT_OUTEN, ATT_OUTEN);
//将衰耗器数据发送使能拉低，将ATT数据发送出去
#define CLR_CS PioWrite( PIO_STATE, ATT_OUTEN, PIO_CLEAR_OUT)
  //#define CLR_CS MA_OUTWM( PIO_CODR_B, ATT_OUTEN, ATT_OUTEN);


#define ATT_1  PB0  //衰耗数值的最低位  PIOB口
#define ATT_2  PB1  //衰耗数值2         PIOB口
#define ATT_4  PB2  //衰耗数值4         PIOB口
#define ATT_8  PB3  //衰耗数值8         PIOB口
#define ATT_16 PB4  //衰耗器值的最高位  PIOB口
#define ATT_INDEX 0x1F //表示ATT数据脚在最低5位

#define ATT_ALL_DATA  ATT_1|ATT_2|ATT_4|ATT_8|ATT_16
#define ATT_ALL_CTRL  ATT_CS1|ATT_CS2|ATT_OUTEN|ATT_ALL_DATA


extern SETTING_PARAM_SET_ST g_stSettingParamSet;

//厂家扩展参数
extern YKPP_PARAM_SET_ST g_stYkppParamSet;

void SetAttInit(void);
void SetAtt(void);
void SetRfSw(void);
void SetRfSwUD(void);

//********************************************
//                 程序实现                 **
//********************************************

//======================================================================
//功能：	衰耗器设置的初始化
//最后编写时间：2006年3月14日
//======================================================================
void SetAttInit(void)
{
    PioOpen( PIO_STATE, ATT_ALL_CTRL, PIO_OUTPUT); //设置所有ATT管脚为输出状态，并受PIO控制
    
//    //使所有用于衰耗器控制的8个管脚都受PIO控制
//    MA_OUTWM(PIO_PER_B, ATT_ALL_CTRL, ATT_ALL_CTRL);
//    //设置所有用于衰耗器控制的8个管脚为输出状态
//    MA_OUTWM(PIO_ODR_B, ATT_ALL_CTRL, ATT_ALL_CTRL);
    
    //将衰耗器数据发送使能拉高，使ATT处于就绪状态
    SET_CS;
}

//======================================================================
//功能：	衰耗器控制，之前需调用SetAttInit()进行管脚初始化
//最后编写时间：2006年3月17日
//备注：        共有4组输出，前三组为ATT输出，第4组为上、下行功放开关和????
//======================================================================
void SetAtt(void)
{
    UINT32 uiAttSwitch, uiAttOutput;

    //------------ 通道1的上行衰减值1的设置 ------------
    DataToPio(PIO_STATE, ATT_CS1, ATT_CS1|ATT_CS2); //选择第2组，即进行通道1的上行衰耗设置
    
    //上行衰减值（通道1） → uiAttOutput
    //MCM-17_20061106_linyu_begin
    uiAttOutput = g_stSettingParamSet.ucUlAtt;  //上行衰减值（通道1）
    //MCM-17_20061106_linyu_end
    uiAttOutput = ~uiAttOutput;
    uiAttOutput &= 0x1F;
    
    DataToPio(PIO_STATE, uiAttOutput, ATT_INDEX);  //  P0 = uiAttOutput;
  
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //延时若干微秒
    CLR_CS;    //ATT1_CS = 1;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //延时若干微秒
    SET_CS;    //ATT1_CS = 0;
    
    //------------ 通道1的下行衰减值的设置 ------------
    DataToPio(PIO_STATE, 0, ATT_CS1|ATT_CS2); //选择第1组，即进行通道1的下行衰耗设置
    
    //下行衰减值（通道1） → uiAttOutput
    //MCM-17_20061106_linyu_begin
    uiAttOutput = g_stSettingParamSet.ucDlAtt;  //下行衰减值（通道1）
    //MCM-17_20061106_linyu_end
    uiAttOutput = ~uiAttOutput;
    uiAttOutput &= 0x1F;
    
    DataToPio(PIO_STATE, uiAttOutput, ATT_INDEX);  //  P0 = uiAttOutput;
    
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //延时若干微秒
    CLR_CS;    //ATT2_CS = 1;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //延时若干微秒
    SET_CS;    //ATT2_CS = 0;
    
    //------------ 通道2的下行衰减值的设置 ------------
    DataToPio(PIO_STATE, ATT_CS2, ATT_CS1|ATT_CS2); //选择第3组，即进行通道2的下行衰耗设置
    
    //下行衰减值（通道2） → uiAttOutput
    uiAttOutput = g_stSettingParamSet.ucDlAtt2;  //下行衰减值（通道2）
    uiAttOutput = ~uiAttOutput;
    uiAttOutput &= 0x1F;
    
    DataToPio(PIO_STATE, uiAttOutput, ATT_INDEX);  //  P0 = uiAttOutput;
    
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //延时若干微秒
    CLR_CS;    //ATT3_CS = 1;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //延时若干微秒
    SET_CS;    //ATT3_CS = 0;
}

//======================================================================
//功能：	射频开关控制，之前需调用SetAttInit()进行管脚初始化
//最后编写时间：2006年4月29日
//备注：        从SETATT中分离出来。
//======================================================================
void SetRfSw(void)
{
    UINT32 uiAttSwitch, uiAttOutput;

    DataToPio(PIO_STATE, ATT_CS1|ATT_CS2, ATT_CS1|ATT_CS2); //选择第4组

    uiAttOutput = 0;
    //射频信号开关状态 → uiAttSwitch
    uiAttSwitch = g_stSettingParamSet.ucRfSw;  //射频信号开关状态
    if((uiAttSwitch & 0x01) == 0)
    {
        uiAttOutput |= 0x07; //B‘00000111’
    }
    else uiAttOutput = 0;

    DataToPio(PIO_STATE, uiAttOutput, ATT_INDEX);  //P0 = uiAttOutput;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //延时若干微秒
    CLR_CS;    //ATT4_CS = 1;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //延时若干微秒	
    SET_CS;    //ATT4_CS = 0;
}

//======================================================================
//功能：	      射频开关分上、下行，测试时专用（未完成）
//最后编写时间：2006年5月11日
//备注：       
//======================================================================
void SetRfSwUD(void)
{
    UINT32 uiAttSwitch, uiAttOutput;

    DataToPio(PIO_STATE, ATT_CS1|ATT_CS2, ATT_CS1|ATT_CS2); //选择第4组

    uiAttOutput = 0;
    
    //射频信号开关状态 → uiAttSwitch
    uiAttOutput = 0;
    uiAttOutput |= g_stYkppParamSet.stYkppRfParam.ucDlRfSw;  //下行射频信号开关
    uiAttOutput |= g_stYkppParamSet.stYkppRfParam.ucUlRfSw << 1;  //上行射频信号开关
    uiAttOutput |= g_stSettingParamSet.ucRfSw << 2;  //射频信号开关状态

    DataToPio(PIO_STATE, uiAttOutput, ATT_INDEX);  //P0 = uiAttOutput;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //延时若干微秒
    CLR_CS;    //ATT4_CS = 1;
    uiAttSwitch = 16;
    while(uiAttSwitch--);  //延时若干微秒	
    SET_CS;    //ATT4_CS = 0;
}
