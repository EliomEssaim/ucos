/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   ad.c
    作者:     林玮
    文件说明: 此文件实现A/D模拟量采集、并将采集值根据不同计算公式，得到相应结果
    其它:
    函数列表:

    历史记录:
    1. 日期:  2006/09/28
       作者:  林玮
       描述:  编码完成
    2. 日期:  2006/11/29
       作者:  章杰
       描述:  修改问题MCM-41，
              移出高于低于检测范围的宏定义
---------------------------------------------------------------------------*/
#include "../../include/Interface.h"
#include "../../OH/CMCC_1_Ext.h"
#include "../../OH/YKPP.h"
#include "../../AC/Actask.h"
#include "ad.h"

#define UpLoad   1 //上行，CountPowOut的参数
#define DownLoad 0 //下行，CountPowOut的参数
#define LOWER_VALUE  0x80 //低于检测范围时的返回值
#define HIGHER_VALUE 0x7F //高于检测范围时的返回值
#define ADC_AVAILABLE_BIT 0x03ff //A/D精度为10bit，所以采集结果中取最低10个位

extern SETTING_PARAM_SET_ST g_stSettingParamSet;

//所有告警信息
extern ALARM_ITEM_UN g_uAlarmItems;
//中心告警状态
extern ALARM_STATUS_UN g_uCenterStatus;
//实时采样参数，不需要掉电保存
extern RC_PARAM_SET_ST g_stRcParamSet;
extern DEV_INFO_SET_ST  g_stDevInfoSet;

//厂家扩展参数
extern YKPP_PARAM_SET_ST g_stYkppParamSet;

extern UCHAR MasterSlaveJudge(void); //本函数判断本机型属于射频主机还是射频从机

char cDetectDlInPwr = 0;   //

void AdcInit(void);
void AdcExe(void);
/*
UINT32 CountPowOut(UINT32 uiOldValue, UINT32 uiUpOrDown);
UINT32 CountVSWR(UINT32 uiOldVSWR, UINT32 uiDownOutput);
INT32 CountTem(UINT32 uiOldTem);
INT32 CountBigPwr(UINT32 uiOldPwr);
INT32 CountSmallPwr(UINT32 uiOldPwr);
INT32 CountBigPwr3G(UINT32 uiOldPwr);
INT32 CountSmallPwr3G(UINT32 uiOldPwr);
void PutIntoRam(UINT32* uiValueTemp);
*/
//======================================================================
//功能：	完成对两个A/D采集器的初始化
//最后编写时间：2006年3月14日
//======================================================================
void AdcInit(void)
{
    //ARM内部的A/D相关初始化
    APMC_PCER |= 0x00018000;  //打开ADC_0和ADC_1的外部时钟
    MA_OUTW(ADC_IDR_0, 0x01);  //关闭ADC_0的中断使能
    MA_OUTW(ADC_IDR_1, 0x01);  //关闭ADC_1的中断使能
    MA_OUTWM(ADC_MR_0, ADC_PRESCAL, ADC_PRESCAL); //设置采样精度为10bit，取消硬件触发启动，
                                                //采样时钟为MCK/((63+1)*2)
    MA_OUTWM(ADC_MR_1, ADC_PRESCAL, ADC_PRESCAL); //设置采样精度为10bit，取消硬件触发启动，
                                                //采样时钟为MCK/((63+1)*2)
    PioOpen( &g_stPioB, ADC_SE|ADC_EN1|ADC_EN2, PIO_OUTPUT); //同时进行了受PIO控制和设置为输出状态

}

//======================================================================
//功能：	 清除ADC_CHSR的低四位
//最后修改： 2006年7月5日
//描述：     使两个ADC模块的所有通道除于disable状态，因为同时有多个通道
//           enable的话，如果采集过程被中断打断，采集就会停止
//======================================================================
void ClearAdcCHSR(void)
{
    MA_OUTW(ADC_CHDR_0, 0x0F);
    MA_OUTW(ADC_CHDR_1, 0x0F);
}

#ifdef YK_POI
#include "./poi_conversion.c"
#else
#include "./gen_conversion.c"
#endif


//======================================================================
//功能：	  完成A/D采集、模拟量的告警判断
//最后修改： 2006年7月20日
//描述：     主文件中只需定时执行本函数，即可完成A/D采集和结果计算工作
//======================================================================
void AdcExe(void)
{
    UINT32 i, uiWAdcValue[24]; //用来暂时存放采集到的结果

 //   AdcInit(); //完成对两个A/D采集器的初始化
  //*********************************
  // 此处加入外部模拟开关的切换控制
    SELECT_A;
    DISSELECT_U30;
    SELECT_U6U7;
  //*********************************
    MA_OUTW(ADC_CHER_0, 0x01);       //先选第0路来采集
    for(i=0;i<5000;i++);  //由于硬件电路中采用模拟电子开关来切换选择需要采集的模拟量，开
                          //关切换时可能引起电源抖动，故延时等待信号稳定
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D采集器0开始工作
    while(MA_INWM(ADC_SR_0, 0x01) == 0); //等待采集完毕的标志
    uiWAdcValue[0]=MA_INWM(ADC_CDR0_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x01);       //先选第0路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    while(MA_INWM(ADC_SR_1, 0x01) == 0); //等待采集完毕的标志
    uiWAdcValue[1]=MA_INWM(ADC_CDR0_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_0, 0x02);       //选第1路来采集
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D采集器0开始工作
    while(MA_INWM(ADC_SR_0, 0x02) == 0); //等待采集完毕的标志
    uiWAdcValue[2]=MA_INWM(ADC_CDR1_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x02);       //选第1路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    while(MA_INWM(ADC_SR_1, 0x02) == 0); //等待采集完毕的标志
    uiWAdcValue[3]=MA_INWM(ADC_CDR1_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_0, 0x04);       //选第2路来采集
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D采集器0开始工作
    while(MA_INWM(ADC_SR_0, 0x04) == 0); //等待采集完毕的标志
    uiWAdcValue[4]=MA_INWM(ADC_CDR2_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x04);       //选第2路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    while(MA_INWM(ADC_SR_1, 0x04) == 0); //等待采集完毕的标志
    uiWAdcValue[5]=MA_INWM(ADC_CDR2_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_0, 0x08);       //选第3路来采集
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D采集器0开始工作
    while(MA_INWM(ADC_SR_0, 0x08) == 0); //等待采集完毕的标志
    uiWAdcValue[6]=MA_INWM(ADC_CDR3_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x08);       //选第3路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    while(MA_INWM(ADC_SR_1, 0x08) == 0); //等待采集完毕的标志
    uiWAdcValue[7]=MA_INWM(ADC_CDR3_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
  //*********************************
  // 此处加入外部模拟开关的切换控制
    SELECT_B;
    for(i=0;i<5000;i++);  //由于硬件电路中采用模拟电子开关来切换选择需要采集的模拟量，开
                          //关切换时可能引起电源抖动，故延时等待信号稳定
  //*********************************
    MA_OUTW(ADC_CHER_0, 0x01);       //先选第0路来采集
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D采集器0开始工作
    while(MA_INWM(ADC_SR_0, 0x01) == 0); //等待采集完毕的标志
    uiWAdcValue[8]=MA_INWM(ADC_CDR0_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x01);       //先选第0路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    while(MA_INWM(ADC_SR_1, 0x01) == 0); //等待采集完毕的标志
    uiWAdcValue[9]=MA_INWM(ADC_CDR0_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_0, 0x02);       //选第1路来采集
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D采集器0开始工作
    while(MA_INWM(ADC_SR_0, 0x02) == 0); //等待采集完毕的标志
    uiWAdcValue[10]=MA_INWM(ADC_CDR1_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x02);       //选第1路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    while(MA_INWM(ADC_SR_1, 0x02) == 0); //等待采集完毕的标志
    uiWAdcValue[11]=MA_INWM(ADC_CDR1_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_0, 0x04);       //选第2路来采集
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D采集器0开始工作
    while(MA_INWM(ADC_SR_0, 0x04) == 0); //等待采集完毕的标志
    uiWAdcValue[12]=MA_INWM(ADC_CDR2_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x04);       //选第2路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    while(MA_INWM(ADC_SR_1, 0x04) == 0); //等待采集完毕的标志
    uiWAdcValue[13]=MA_INWM(ADC_CDR2_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_0, 0x08);       //选第3路来采集
    MA_OUTW(ADC_CR_0, ADC_START);    //A/D采集器0开始工作
    while(MA_INWM(ADC_SR_0, 0x08) == 0); //等待采集完毕的标志
    uiWAdcValue[14]=MA_INWM(ADC_CDR3_0, ADC_AVAILABLE_BIT);

    MA_OUTW(ADC_CHER_1, 0x08);       //选第3路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    while(MA_INWM(ADC_SR_1, 0x08) == 0); //等待采集完毕的标志
    uiWAdcValue[15]=MA_INWM(ADC_CDR3_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------

#ifndef YK_POI
  //*********************************
  // 此处加入外部模拟开关的切换控制
    SELECT_A;
    DISSELECT_U6U7;
    SELECT_U30;
    for(i=0;i<5000;i++);  //由于硬件电路中采用模拟电子开关来切换选择需要采集的模拟量，开
                          //关切换时可能引起电源抖动，故延时等待信号稳定
  //*********************************
    MA_OUTW(ADC_CHER_1, 0x01);       //先选第0路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    
    while(MA_INWM(ADC_SR_1, 0x01) == 0); //等待采集完毕的标志
    uiWAdcValue[16]=MA_INWM(ADC_CDR0_1, ADC_AVAILABLE_BIT);
        
    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_1, 0x02);       //选第1路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    
    while(MA_INWM(ADC_SR_1, 0x02) == 0); //等待采集完毕的标志
    uiWAdcValue[17]=MA_INWM(ADC_CDR1_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_1, 0x04);       //选第2路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    
    while(MA_INWM(ADC_SR_1, 0x04) == 0); //等待采集完毕的标志
    uiWAdcValue[18]=MA_INWM(ADC_CDR2_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_1, 0x08);       //选第3路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作

    while(MA_INWM(ADC_SR_1, 0x08) == 0); //等待采集完毕的标志
    uiWAdcValue[19]=MA_INWM(ADC_CDR3_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();
        //--------------------
  //*********************************
  // 此处加入外部模拟开关的切换控制
    SELECT_B;
    for(i=0;i<5000;i++);  //由于硬件电路中采用模拟电子开关来切换选择需要采集的模拟量，开
                          //关切换时可能引起电源抖动，故延时等待信号稳定
  //*********************************
    MA_OUTW(ADC_CHER_1, 0x01);       //先选第0路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    
    while(MA_INWM(ADC_SR_1, 0x01) == 0); //等待采集完毕的标志
    uiWAdcValue[20]=MA_INWM(ADC_CDR0_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_1, 0x02);       //选第1路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    
    while(MA_INWM(ADC_SR_1, 0x02) == 0); //等待采集完毕的标志
    uiWAdcValue[21]=MA_INWM(ADC_CDR1_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_1, 0x04);       //选第2路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作
    
    while(MA_INWM(ADC_SR_1, 0x04) == 0); //等待采集完毕的标志
    uiWAdcValue[22]=MA_INWM(ADC_CDR2_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();

        //--------------------
    MA_OUTW(ADC_CHER_1, 0x08);       //选第3路来采集
    MA_OUTW(ADC_CR_1, ADC_START);    //A/D采集器1开始工作

    while(MA_INWM(ADC_SR_1, 0x08) == 0); //等待采集完毕的标志
    uiWAdcValue[23]=MA_INWM(ADC_CDR3_1, ADC_AVAILABLE_BIT);

    ClearAdcCHSR();
        //--------------------
#endif

/*********** TEST **********
    uiWAdcValue[1] = 512;
    uiWAdcValue[2] = 640;
    uiWAdcValue[3] = 512;
    uiWAdcValue[4] = 512;
    uiWAdcValue[5] = 512;
    uiWAdcValue[6] = 512;
    uiWAdcValue[7] = 512;
    uiWAdcValue[8] = 512;
    uiWAdcValue[9] = 512;
    uiWAdcValue[10] = 512;
    uiWAdcValue[11] = 512;
    uiWAdcValue[12] = 512;
    uiWAdcValue[13] = 512;
    uiWAdcValue[14] = 512;
    uiWAdcValue[15] = 512;
    uiWAdcValue[16] = 512;
    uiWAdcValue[17] = 512;
    uiWAdcValue[18] = 512;
    uiWAdcValue[19] = 512;
    uiWAdcValue[20] = 512;
    uiWAdcValue[21] = 512;
    uiWAdcValue[22] = 512;
    uiWAdcValue[23] = 512;
    
/***************************/
    PutIntoRam(uiWAdcValue); //数据采集完毕后的各模拟量计算,以及告警状态判断
}


