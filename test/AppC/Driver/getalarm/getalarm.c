/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

get_alarm->stAlarmItemSet->c

CPLD电路中关于开关量采集操作步骤：
    1、GET_DA_CE用于在开关量采集功能与选频器操作之间切换，先切换到开关量采集；
    2、然后设置GET_DA_S0～GET_DA_S3来选择采集某一组8路开关量；
    3、接着就可以从GET_DA0～GET_DA7读入数值。

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "../../OH/CMCC_1_Ext.h"
#include "../../OH/YKPP.h"
#include "../../AC/Actask.h"
#include "getalarm.h"

extern ALARM_ITEM_UN g_uAlarmItems;

//存放与主从协议相关的参数
//通过判断从机编号可清楚是否注册
extern YKPP_PARAM_SET_ST g_stYkppParamSet;

//********************************************
//                管脚连接情况               **
//********************************************
#define GET_DA0 PB8  //开关量值输入第0脚  PIOB口
#define GET_DA1 PB9  //开关量值输入第1脚  PIOB口
#define GET_DA2 PB10  //开关量值输入第2脚  PIOB口
#define GET_DA3 PB11  //开关量值输入第3脚  PIOB口
#define GET_DA4 PB12  //开关量值输入第4脚  PIOB口
#define GET_DA5 PB13  //开关量值输入第5脚  PIOB口
#define GET_DA6 PB14  //开关量值输入第6脚  PIOB口
#define GET_DA7 PB15  //开关量值输入第7脚  PIOB口
#define GET_DA_DATA  GET_DA7|GET_DA6|GET_DA5|GET_DA4|GET_DA3|GET_DA2|GET_DA1|GET_DA0
#define MOVE_TIME 8

#define GET_DA_CE PA2  //开关量采集的使能脚  PIOA口

//将GET_DA_CE脚清零，选择开关量采集
#define SELECT_ALARM  PioWrite( &g_stPioA, GET_DA_CE, PIO_CLEAR_OUT)
//#define SELECT_ALARM  MA_OUTWM( PIO_CODR_A, GET_DA_CE, GET_DA_CE)

#define GET_DA_S0 PA3  //开关量采集的片选第0脚  PIOA口
#define GET_DA_S1 PA4  //开关量采集的片选第1脚  PIOA口
#define GET_DA_S2 PA5  //开关量采集的片选第2脚  PIOA口
#define GET_DA_S3 PA6  //开关量采集的片选第3脚  PIOA口
#define GET_DA_SALL GET_DA_S0|GET_DA_S1|GET_DA_S2|GET_DA_S3
#define GET_DA_CONTROL  GET_DA_CE|GET_DA_SALL

extern void SetAtt(void);
extern void SetFreq(void);

void GetAlarmInit(void);
void GetAlarmData(void);

UCHAR ucSetFreq; //需要设置频率标志
UCHAR ucSetAtt;   //需要设置衰耗标志

//======================================================================
//功能：	开关量采集的初始化
//最后编写时间：2006年3月14日
//======================================================================
void GetAlarmInit(void)
{
    PioOpen( &g_stPioB, GET_DA_DATA, PIO_INPUT); //将开关量采集数据脚受PIO控制，并设置为输入状态
    PioOpen( &g_stPioA, GET_DA_CONTROL, PIO_OUTPUT); //将开关量采集控制脚受PIO控制，并设置为输出状态
}

#ifdef YK_POI
#include "./poi_switch.c"
#else
#include "./gen_switch.c"
#endif

//======================================================================
//功能：	开关量采集，完成开关量采集结果赋值
//描述：        之前必须先调用GetAlarmInit()进行初始化
//最后编写时间：2006年3月14日
//返回：        无
//======================================================================
void GetAlarmData(void)
{
    UCHAR aucWAlarmValue[5]; //用来暂时存放采集到的结果
    
    SELECT_ALARM; //从设置频率那边切换回开关量采集，由外部CPLD电路决定
    
    PioWrite( &g_stPioA, GET_DA_SALL, PIO_CLEAR_OUT); //片选，选择第1组8个开关量
    aucWAlarmValue[0] = ( ( PioRead( &g_stPioB)) >> MOVE_TIME);
    PioWrite( &g_stPioA, GET_DA_S0, PIO_SET_OUT); //片选，选择第2组8个开关量
    aucWAlarmValue[1] = ( ( PioRead( &g_stPioB)) >> MOVE_TIME);
    PioWrite( &g_stPioA, GET_DA_S1, PIO_SET_OUT); //片选，选择第4组8个开关量
    aucWAlarmValue[3] = ( ( PioRead( &g_stPioB)) >> MOVE_TIME);
    PioWrite( &g_stPioA, GET_DA_SALL, PIO_CLEAR_OUT); //片选，先恢复全0输出
    PioWrite( &g_stPioA, GET_DA_S1, PIO_SET_OUT); //片选，选择第3组8个开关量
    aucWAlarmValue[2] = ( ( PioRead( &g_stPioB)) >> MOVE_TIME);
    PioWrite( &g_stPioA, GET_DA_SALL, PIO_CLEAR_OUT); //片选，先恢复全0输出
    PioWrite( &g_stPioA, GET_DA_S2, PIO_SET_OUT); //片选，选择第5组3个开关量
    aucWAlarmValue[4] = ( ( PioRead( &g_stPioB)) >> MOVE_TIME);

    //------------ 以上数据采集完毕，下面开始填写数据  -------------
    DisposeResult(aucWAlarmValue);

}



