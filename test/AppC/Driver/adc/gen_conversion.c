/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   Gen_conversion.cpp
    作者:     林玮
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  
       作者:  
       描述:  代码编写完成
    2. 日期:  2006/11/10
       作者:  章杰
       描述:  修改问题MCM-29，在BCCH电平判断中增加对0的判断,为避免下
              行输入过和下行输入欠同时告警，在产生其中一个告警时便使另一个不告警。
    3. 日期:  2006/11/01
       作者:  章杰
       描述:  修改问题MCM-11，把原代码中涉及机型判断的地方全部改为对
              机型配置表中某项或者多项值的判断。
    4. 日期:  2006/11/06
       作者:  林雨
       描述:  修改问题MCM-17，将所有带通道号的通道1的参数都映射对应的
              到不带通道号的参数上，即当设备是多通道时，查询通道1的参
              数实际访问的是对应的不带通道号的参数的地址。
    5. 日期:  2006/11/20
       作者:  林玮
       描述:  修改问题MCM-39，修改3G设备的下行实际增益值计算公式。
    6. 日期:  2006/11/29
       作者:  林玮
       描述:  修改问题MCM-42，
              增加通道二的下行输出和驻波处理
    7. 日期:  2006/12/18
       作者:  林玮
       描述:  修改问题MCM-48，
              增加驻波计算时的判断，针对无符号数减法
    8. 日期:  2007/11/20
       作者:  章杰
       描述:  修改问题MCM-80，              
              增加太阳能蓄电池电压检测。
---------------------------------------------------------------------------*/
#include "../../AC/Actask.h"

//全局变量声明
extern DEV_TYPE_TABLE_ST g_stDevTypeTable;

/*************************************************
  Copyright (C), 2006-2007, Lin Wei
  文件名：    current_conversion.c
  作者:       林玮
  版本：      1.0.0
  日期：      2006/07/19 
  功能描述：  此文件实现将A/D模拟量采集值根据不同计算公式，得到相应结果
  备注：      此文件仅针对移动3G项目
  函数列表：  // 主要函数列表，每条记录应包括函数名及功能简要说明
        UINT32 CountPowOut3G(UINT32 uiOldValue, UINT32 uiUpOrDown);
        UINT32 CountVSWR3G(UINT32 uiOldVSWR, UINT32 uiDownOutput);
        INT32 CountTem(UINT32 uiOldTem);
        INT32 CountBigPwr(UINT32 uiOldPwr);
        INT32 CountSmallPwr(UINT32 uiOldPwr);
        INT32 CountBigPwr3G(UINT32 uiOldPwr);
        INT32 CountSmallPwr3G(UINT32 uiOldPwr);
        void PutIntoRam(UINT32* uiValueTemp);
*************************************************/

//**********************************************************
//功能：计算2G下行输出功率值(dBm)
//输入：uiOldValue -- 采集结果
//返回：输出功率(单位：dBm)或高于检测范围，又或低于检测范围
//**********************************************************
UINT32 CountDlPow2G(UINT32 uiOldValue)  //将采集到的输出功率值，根据公式计算出对应的DB值
{
    INT32 iMaxPow;
    LONG lTempPowOut;
    INT32 iAdjustValue;

    //下行输出功率最大值
    //下行输出功率校准值，该值默认为0，只有当功放模块不符合我们要求时才需要设置
    iMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax;  //下行输出最大增益
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlOutAdjust;  //下行输出功率校准值

    lTempPowOut = iAdjustValue + 35 - (((LONG)(uiOldValue) * 50 + 512) / 1024);

    //下行输出功率检测范围标称功率+2～标称功率-18
    if(lTempPowOut < -2)
    {
        return HIGHER_VALUE;  //高于检测范围时的返回值
    }
    else if(lTempPowOut > 18)
    {
        return LOWER_VALUE;  //低于检测范围时的返回值
    }
    else
    {
        return (UINT32)(iMaxPow - lTempPowOut);
    }
}
//**********************************************************
//功能：计算2G下行输出功率值(dBm)(通道二)
//输入：uiOldValue -- 采集结果
//返回：输出功率(单位：dBm)或高于检测范围，又或低于检测范围
//**********************************************************
UINT32 CountDlPow2GChunnel2(UINT32 uiOldValue)  //将采集到的输出功率值，根据公式计算出对应的DB值
{
    INT32 iMaxPow;
    LONG lTempPowOut;
    INT32 iAdjustValue;

    //下行输出功率最大值
    //下行输出功率校准值，该值默认为0，只有当功放模块不符合我们要求时才需要设置
    iMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax2;  //下行输出最大增益(通道二)
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlOutAdjust2;  //下行输出功率校准值(通道二)

    lTempPowOut = iAdjustValue + 35 - (((LONG)(uiOldValue) * 50 + 512) / 1024);

    //下行输出功率检测范围标称功率+2～标称功率-18
    if(lTempPowOut < -2)
    {
        return HIGHER_VALUE;  //高于检测范围时的返回值
    }
    else if(lTempPowOut > 18)
    {
        return LOWER_VALUE;  //低于检测范围时的返回值
    }
    else
    {
        return (UINT32)(iMaxPow - lTempPowOut);
    }
}

//**********************************************************
//功能：计算2G上行输出功率值(dBm)
//输入：uiOldValue -- 采集结果
//返回：输出功率(单位：dBm)或高于检测范围，又或低于检测范围
//**********************************************************
UINT32 CountUlPow2G(UINT32 uiOldValue)  //将采集到的输出功率值，根据公式计算出对应的DB值
{
    INT32 iMaxPow;
    LONG lTempPowOut;
    INT32 iAdjustValue;

    //上行输出功率最大值
    //上行输出功率校准值，该值默认为0，只有当功放模块不符合我们要求时才需要设置
    iMaxPow = g_stYkppParamSet.stYkppRfParam.cUlOutMax;  //上行输出最大增益
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sUlOutAdjust;  //上行输出功率校准值

    lTempPowOut = iAdjustValue + 35 - (((LONG)(uiOldValue) * 50 + 512) / 1024);
    
    //下行输出功率检测范围标称功率+2～标称功率-13
    if(lTempPowOut < -2)
    {
        return HIGHER_VALUE;  //高于检测范围时的返回值
    }
    else if(lTempPowOut > 13)
    {
        return LOWER_VALUE;  //低于检测范围时的返回值
    }
    else
    {
        return (UINT32)(iMaxPow - lTempPowOut);
    }
}

//**********************************************************
//功能： 将驻波比采样电平值，根据公式计算出对应的驻波比值
//输入：  uiOldVSWR  -- 反射功率采集结果
//       uiDownOutput -- 下行输出功率值
//返回： 输出功率(单位：dBm)或高于检测范围，又或低于检测范围
//备注： 调用本函数前必须先用CountPowOut3G计算下行输出功率值
//**********************************************************
UINT32 CountVSWR2G(UINT32 uiOldVSWR, UINT32 uiDownOutput)
{
    UINT32 uiMaxPow, TempVSWR;
    INT32 iAdjustValue;

    uiMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax;  //下行输出最大增益
    //下行反射功率校准值，默认为0，只有当功放模块不符合我们要求时才需要设置
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlReflAdjust;  //下行反射功率校准值

    TempVSWR = (uiOldVSWR * 25 + 256) / 512; //加256是为了四舍五入

    uiMaxPow = uiMaxPow + iAdjustValue + TempVSWR - 45;  //得到反射功率值//3.5V = 标称功率-10，所以为45
    
    //MCM-48_20061212_linwei_begin
    if(uiDownOutput > uiMaxPow)
    {
        uiMaxPow = uiDownOutput - uiMaxPow;   //将下行输出功率减去下行反射功率。
    }
    else
    {
        uiMaxPow = 0;
    }
    //MCM-48_20061212_linwei_end

    if(uiMaxPow >= 17)
    {
        return 12;   //大于17时，驻波比值为1.2
    }
    else if(uiMaxPow >= 16)
    {
        return 13;   //大于16时，驻波比值为1.3
    }
    else if(uiMaxPow >= 15)
    {
        return 14;   //等于15或16时，驻波比值为1.4
    }
    else if(uiMaxPow >= 13)
    {
        return 15;   //等于13或14时，驻波比值为1.5
    }
    else if(uiMaxPow >= 12)
    {
        return 16;   //等于12时，驻波比值为1.6
    }
    else if(uiMaxPow >= 11)
    {
        return 17;   //等于11时，驻波比值为1.7
    }
    else if(uiMaxPow >= 10)
    {
        return 18;   //等于10时，驻波比值为1.8
    }
    else if (uiMaxPow >= 8)
    {
        return 20;   //等于8或9时，驻波比值为2.0
    }
    else if (uiMaxPow >= 7)
    {
        return 23;   // 7时，驻波比值为2.3
    }
    else if (uiMaxPow >= 6)
    {
        return 25;   // 6时，驻波比值为2.5
    }
    else if (uiMaxPow >= 4)
    {
        return 30;   // 4~5时，驻波比值为3.0
    }
    else return 40;     //小于4时，驻波比值为显示为4.0
}

//**********************************************************
//功能： 将驻波比采样电平值，根据公式计算出对应的驻波比值(通道二)
//输入：  uiOldVSWR  -- 反射功率采集结果
//       uiDownOutput -- 下行输出功率值
//返回： 输出功率(单位：dBm)或高于检测范围，又或低于检测范围
//备注： 调用本函数前必须先用CountPowOut3G计算下行输出功率值
//**********************************************************
UINT32 CountVSWR2GChunnel2(UINT32 uiOldVSWR, UINT32 uiDownOutput)
{
    UINT32 uiMaxPow, TempVSWR;
    INT32 iAdjustValue;

    uiMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax2;  //下行输出最大增益
    //下行反射功率校准值，默认为0，只有当功放模块不符合我们要求时才需要设置
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlReflAdjust2;  //下行反射功率校准值

    TempVSWR = (uiOldVSWR * 25 + 256) / 512; //加256是为了四舍五入

    uiMaxPow = uiMaxPow + iAdjustValue + TempVSWR - 45;  //得到反射功率值//3.5V = 标称功率-10，所以为45
    
    //MCM-48_20061212_linwei_begin
    if(uiDownOutput > uiMaxPow)
    {
        uiMaxPow = uiDownOutput - uiMaxPow;   //将下行输出功率减去下行反射功率。
    }
    else
    {
        uiMaxPow = 0;
    }
    //MCM-48_20061212_linwei_end

    if(uiMaxPow >= 17)
    {
        return 12;   //大于17时，驻波比值为1.2
    }
    else if(uiMaxPow >= 16)
    {
        return 13;   //大于16时，驻波比值为1.3
    }
    else if(uiMaxPow >= 15)
    {
        return 14;   //等于15或16时，驻波比值为1.4
    }
    else if(uiMaxPow >= 13)
    {
        return 15;   //等于13或14时，驻波比值为1.5
    }
    else if(uiMaxPow >= 12)
    {
        return 16;   //等于12时，驻波比值为1.6
    }
    else if(uiMaxPow >= 11)
    {
        return 17;   //等于11时，驻波比值为1.7
    }
    else if(uiMaxPow >= 10)
    {
        return 18;   //等于10时，驻波比值为1.8
    }
    else if (uiMaxPow >= 8)
    {
        return 20;   //等于8或9时，驻波比值为2.0
    }
    else if (uiMaxPow >= 7)
    {
        return 23;   // 7时，驻波比值为2.3
    }
    else if (uiMaxPow >= 6)
    {
        return 25;   // 6时，驻波比值为2.5
    }
    else if (uiMaxPow >= 4)
    {
        return 30;   // 4~5时，驻波比值为3.0
    }
    else return 40;     //小于4时，驻波比值为显示为4.0
}

//**********************************************************
//功能：将采集值通过计算得出输出功率(dBm),并进行告警状态判断
//输入：uiOldValue -- 采集结果
//      uiUpOrDown=1 -- 下行   uiUpOrDown=0 -- 上行
//返回：输出功率(单位：dBm)或高于检测范围，又或低于检测范围
//**********************************************************
UINT32 CountPowOut3G(UINT32 uiOldValue, UINT32 uiUpOrDown)  //将采集到的输出功率值，根据公式计算出对应的DB值
{
    INT32 iMaxPow;
    ULONG ulTempPowOut;
    INT32 iAdjustValue;

    //上或下行输出功率最大值
    //上或下行输出功率校准值，该值默认为0，只有当功放模块不符合我们要求时才需要设置
    if(uiUpOrDown == DownLoad)
    {
        iMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax;  //下行输出最大增益
        iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlOutAdjust;  //下行输出功率校准值
    }
    else
    {
        iMaxPow = g_stYkppParamSet.stYkppRfParam.cUlOutMax;  //上行输出最大增益
        iAdjustValue = g_stYkppParamSet.stYkppRfParam.sUlOutAdjust;  //上行输出功率校准值
    }
//*----- test -----
   // uiMaxPow = 37;
   // iAdjustValue = 0;
//---------------*/

    //ulTempPowOut = ((ULONG)(uiOldValue) * 25 * 10 + 256) / 512; //精度要求小数点后一位，所以乘以10
    ulTempPowOut = (2250 / 4) - (((ULONG)(uiOldValue) * 125 * 10) / (512 * 4));
    //下行输出功率检测范围标称功率+2～标称功率-45
    if((LONG)(ulTempPowOut) < (LONG)(iAdjustValue * 10 - 20))
    {
        return HIGHER_VALUE;  //高于检测范围时的返回值
    }
    else if(ulTempPowOut > (iAdjustValue * 10 + 450))
    {
        return LOWER_VALUE;  //低于检测范围时的返回值
    }
    else
    {
        return (UINT32)((iMaxPow * 10) + iAdjustValue * 10 - ulTempPowOut);
    }
}

//**********************************************************
//功能：将采集值通过计算得出输出功率(dBm),并进行告警状态判断
//输入：uiOldValue -- 采集结果
//      uiUpOrDown=1 -- 下行   uiUpOrDown=0 -- 上行
//返回：输出功率(单位：dBm)或高于检测范围，又或低于检测范围
//**********************************************************
UINT32 CountPowOut3GChunnel2(UINT32 uiOldValue, UINT32 uiUpOrDown)  //将采集到的输出功率值，根据公式计算出对应的DB值
{
    INT32 iMaxPow;
    ULONG ulTempPowOut;
    INT32 iAdjustValue;

    //上或下行输出功率最大值
    //上或下行输出功率校准值，该值默认为0，只有当功放模块不符合我们要求时才需要设置
    if(uiUpOrDown == DownLoad)
    {
        iMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax2;  //下行输出最大增益
        iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlOutAdjust2;  //下行输出功率校准值
    }
    else
    {
        iMaxPow = g_stYkppParamSet.stYkppRfParam.cUlOutMax;  //上行输出最大增益
        iAdjustValue = g_stYkppParamSet.stYkppRfParam.sUlOutAdjust;  //上行输出功率校准值
    }
//*----- test -----
   // uiMaxPow = 37;
   // iAdjustValue = 0;
//---------------*/

    //ulTempPowOut = ((ULONG)(uiOldValue) * 25 * 10 + 256) / 512; //精度要求小数点后一位，所以乘以10
    ulTempPowOut = (2250 / 4) - (((ULONG)(uiOldValue) * 125 * 10) / (512 * 4));
    //下行输出功率检测范围标称功率+2～标称功率-45
    if((LONG)(ulTempPowOut) < (LONG)(iAdjustValue * 10 - 20))
    {
        return HIGHER_VALUE;  //高于检测范围时的返回值
    }
    else if(ulTempPowOut > (iAdjustValue * 10 + 450))
    {
        return LOWER_VALUE;  //低于检测范围时的返回值
    }
    else
    {
        return (UINT32)((iMaxPow * 10) + iAdjustValue * 10 - ulTempPowOut);
    }
}

//**********************************************************
//功能： 将驻波比采样电平值，根据公式计算出对应的驻波比值
//输入：  uiOldVSWR  -- 反射功率采集结果
//       uiDownOutput -- 下行输出功率值
//返回： 输出功率(单位：dBm)或高于检测范围，又或低于检测范围
//备注： 调用本函数前必须先用CountPowOut3G计算下行输出功率值
//**********************************************************
UINT32 CountVSWR3G(UINT32 uiOldVSWR, UINT32 uiDownOutput)
{
    UINT32 uiMaxPow, TempVSWR;
    INT32 iAdjustValue;

    uiMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax;  //下行输出最大增益
    //下行反射功率校准值，默认为0，只有当功放模块不符合我们要求时才需要设置
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlReflAdjust;  //下行反射功率校准值
//******** test ********
  //  uiMaxPow = 37;
  //  iAdjustValue = -1;
/***********************/

    TempVSWR = (uiOldVSWR * 25 + 256) / 512; //加256是为了四舍五入

    uiMaxPow = uiMaxPow + iAdjustValue + TempVSWR - 55;  //得到反射功率值//4.5V = 标称功率-10，所以为45

    //MCM-48_20061212_linwei_begin
    uiDownOutput = uiDownOutput / 10;
    if(uiDownOutput > uiMaxPow)
    {
        uiMaxPow = uiDownOutput - uiMaxPow;   //将下行输出功率减去下行反射功率。
    }
    else
    {
        uiMaxPow = 0;
    }
    //MCM-48_20061212_linwei_end

    if(uiMaxPow >= 17)
    {
        return 12;   //大于17时，驻波比值为1.2
    }
    else if(uiMaxPow >= 16)
    {
        return 13;   //大于16时，驻波比值为1.3
    }
    else if(uiMaxPow >= 15)
    {
        return 14;   //等于15或16时，驻波比值为1.4
    }
    else if(uiMaxPow >= 13)
    {
        return 15;   //等于13或14时，驻波比值为1.5
    }
    else if(uiMaxPow >= 12)
    {
        return 16;   //等于12时，驻波比值为1.6
    }
    else if(uiMaxPow >= 11)
    {
        return 17;   //等于11时，驻波比值为1.7
    }
    else if(uiMaxPow >= 10)
    {
        return 18;   //等于10时，驻波比值为1.8
    }
    else if (uiMaxPow >= 8)
    {
        return 20;   //等于8或9时，驻波比值为2.0
    }
    else if (uiMaxPow >= 7)
    {
        return 23;   // 7时，驻波比值为2.3
    }
    else if (uiMaxPow >= 6)
    {
        return 25;   // 6时，驻波比值为2.5
    }
    else if (uiMaxPow >= 4)
    {
        return 30;   // 4~5时，驻波比值为3.0
    }
    else return 40;     //小于4时，驻波比值为显示为4.0
}

//**********************************************************
//功能： 将驻波比采样电平值，根据公式计算出对应的驻波比值
//输入：  uiOldVSWR  -- 反射功率采集结果
//       uiDownOutput -- 下行输出功率值
//返回： 输出功率(单位：dBm)或高于检测范围，又或低于检测范围
//备注： 调用本函数前必须先用CountPowOut3G计算下行输出功率值
//**********************************************************
UINT32 CountVSWR3GChunnel2(UINT32 uiOldVSWR, UINT32 uiDownOutput)
{
    UINT32 uiMaxPow, TempVSWR;
    INT32 iAdjustValue;

    uiMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax2;  //下行输出最大增益
    //下行反射功率校准值，默认为0，只有当功放模块不符合我们要求时才需要设置
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlReflAdjust2;  //下行反射功率校准值
//******** test ********
  //  uiMaxPow = 37;
  //  iAdjustValue = -1;
/***********************/

    TempVSWR = (uiOldVSWR * 25 + 256) / 512; //加256是为了四舍五入

    uiMaxPow = uiMaxPow + iAdjustValue + TempVSWR - 55;  //得到反射功率值//4.5V = 标称功率-10，所以为45

    //MCM-48_20061212_linwei_begin
    uiDownOutput = uiDownOutput / 10;
    if(uiDownOutput > uiMaxPow)
    {
        uiMaxPow = uiDownOutput - uiMaxPow;   //将下行输出功率减去下行反射功率。
    }
    else
    {
        uiMaxPow = 0;
    }
    //MCM-48_20061212_linwei_end

    if(uiMaxPow >= 17)
    {
        return 12;   //大于17时，驻波比值为1.2
    }
    else if(uiMaxPow >= 16)
    {
        return 13;   //大于16时，驻波比值为1.3
    }
    else if(uiMaxPow >= 15)
    {
        return 14;   //等于15或16时，驻波比值为1.4
    }
    else if(uiMaxPow >= 13)
    {
        return 15;   //等于13或14时，驻波比值为1.5
    }
    else if(uiMaxPow >= 12)
    {
        return 16;   //等于12时，驻波比值为1.6
    }
    else if(uiMaxPow >= 11)
    {
        return 17;   //等于11时，驻波比值为1.7
    }
    else if(uiMaxPow >= 10)
    {
        return 18;   //等于10时，驻波比值为1.8
    }
    else if (uiMaxPow >= 8)
    {
        return 20;   //等于8或9时，驻波比值为2.0
    }
    else if (uiMaxPow >= 7)
    {
        return 23;   // 7时，驻波比值为2.3
    }
    else if (uiMaxPow >= 6)
    {
        return 25;   // 6时，驻波比值为2.5
    }
    else if (uiMaxPow >= 4)
    {
        return 30;   // 4~5时，驻波比值为3.0
    }
    else return 40;     //小于4时，驻波比值为显示为4.0
}

//**********************************************************
//功能： 将温度采样值，根据公式计算出对应的温度值（℃）
//输入： uiOldTem   -- 温度采样值
//返回： 温度值，单位 ℃，有符号数INT32型
//**********************************************************
INT32 CountTem3G(UINT32 uiOldTem)
{
    INT32 TemValue;
    TemValue = (INT32)((125 * uiOldTem + 256) / 512) - 50;  //转换公式：N=N*125/512 - 50
    return TemValue;
}

//**********************************************************
//功能： 将温度采样值，根据公式计算出对应的温度值（℃）
//输入： uiOldTem   -- 温度采样值
//返回： 温度值，单位 ℃，有符号数INT32型
//**********************************************************
INT32 CountTem2G(UINT32 uiOldTem)
{
    INT32 TemValue;
    //TemValue = (INT32)((125 * uiOldTem + 384) / 768) - 50;  //转换公式：N=N*125/768 - 50
    TemValue = ((125 * (ULONG)uiOldTem + 256) / 512) - 50 -1; //“-1”为校准，因为采样电压只有2.49，不是2.5
    return TemValue;
}

//**********************************************************
//功能： 用大功率检测器时的下行输入功率dBm值计算
//输入： uiOldPwr  -- 下行输入功率采样值
//返回： 下行输入功率dBm值
//**********************************************************
INT32 CountBigPwr(UINT32 uiOldPwr)
{
    INT32 PwrValue;
    //转换公式：N=MAX-(4.5/0.08-x*500/1024/8)
    PwrValue = 0 - ((225 / 4) - (((INT32)(uiOldPwr) * 125) / (512 * 4)));
    //?????此处还需加入耦合器参数的考虑
    PwrValue += g_stYkppParamSet.stYkppRfParam.ucDlInCoupler;  //下行输入耦合器参数
    return PwrValue;
}

//**********************************************************
//功能： 用小功率检测器时的下行输入功率dBm值计算
//输入： uiOldPwr  -- 下行输入功率采样值
//返回： 下行输入功率dBm值
//**********************************************************
INT32 CountSmallPwr(UINT32 uiOldPwr)
{
    INT32 PwrValue;
    //转换公式：N=MAX-(4.5/0.08-x*500/1024/8)
    PwrValue = 0 - 40 - ((225 / 4) - (((INT32)(uiOldPwr) * 125) / (512 * 4)));
    //?????此处还需加入耦合器参数的考虑
    PwrValue += g_stYkppParamSet.stYkppRfParam.ucDlInCoupler;  //下行输入耦合器参数
    return PwrValue;
}

//**********************************************************
//功能： 用大功率检测器时的下行输入功率dBm值计算
//输入： uiOldPwr  -- 下行输入功率采样值
//返回： 下行输入功率dBm值
//**********************************************************
INT32 CountBigPwr3G(UINT32 uiOldPwr)
{
    INT32 PwrValue;
    //转换公式：N=MAX-10*(4.5/0.08-x*500/1024/8)
    PwrValue = 100 - 10 * ((225 / 4) - (((INT32)(uiOldPwr) * 125) / (512 * 4)));
    //?????此处还需加入耦合器参数的考虑
    //PwrValue += 耦合器参数 * 10；
    PwrValue += (g_stYkppParamSet.stYkppRfParam.ucDlInCoupler * 10);  //下行输入耦合器参数
    return PwrValue;
}

//**********************************************************
//功能： 用小功率检测器时的下行输入功率dBm值计算
//输入： uiOldPwr  -- 下行输入功率采样值
//返回： 下行输入功率dBm值
//**********************************************************
INT32 CountSmallPwr3G(UINT32 uiOldPwr)
{
    INT32 PwrValue;
    //转换公式：N=MAX-10*(4.5/0.08-x*500/1024/8)
    PwrValue = 0 - 300 - 10 * ((225 / 4) - (((INT32)(uiOldPwr) * 125) / (512 * 4)));
    //?????此处还需加入耦合器参数的考虑
    //PwrValue += 耦合器参数 * 10；
    PwrValue += (g_stYkppParamSet.stYkppRfParam.ucDlInCoupler * 10);  //下行输入耦合器参数
    return PwrValue;
}

//**********************************************************
//功能： 
//输入： uiValueTemp  -- 
//返回： 
//**********************************************************
void PutIntoRam(UINT32* uiValueTemp)
{
    UCHAR ucUpPwrSpecial;  //直接耦合机型标志
    INT32 TempData;
    
//************* test **************
  //  ucTempType = 2;
/*********************************/
    //%%%%%%%%%%%%%%%%%%%%%%%%%% 下行输入功率电平 %%%%%%%%%%%%%%%%%%%%%%%%%%
    //========= 如果是射频从机，下行输入值由主机传递过来 ============
    if(g_stDevTypeTable.ucRFRoleType != RF_MASTER)
    {
        //此处不处理，射频从机的下行输入在gen_actask.cpp中处理
    }
    else
    {
        //先处理BCCH接收电平，查询该项时需要显示负数
        
        //MCM-29_20061110_zhangjie_begin
        if(g_stRcParamSet.cSsBcchRxLev >= 0) //大于0说明是新查询到的信源信息，需要处理
        {
            g_stRcParamSet.cSsBcchRxLev = g_stRcParamSet.cSsBcchRxLev - 110;
            g_stRcParamSet.cSsBcchRxLev += g_stYkppParamSet.stYkppRfParam.ucDlInCoupler;
        }
        //MCM-29_20061110_zhangjie_end

        g_stRcParamSet.ucDlInPwrFlag = NORMAL_FLAG; //下行输入功率电平超出范围的标志
        g_stRcParamSet.uc3GDlInPwrFlag = NORMAL_FLAG; //下行输入功率电平超出范围的标志
        ucUpPwrSpecial = 0;  //直接耦合机型标志
        cDetectDlInPwr = 0;
    
    //======== 如果厂家协议中要求使用MODEN来检测下行输入的话 ========
    if(g_stYkppParamSet.stYkppCtrlParam.ucDlInAcqMode == DLIN_MODEN_MEASURE)
    {
        if(g_stRcParamSet.cSsBcchRxLev == -1)
        {
            g_stRcParamSet.ucDlInPwrFlag = LOWER_FLAG; //下行输入功率电平低于检测范围
            g_stRcParamSet.uc3GDlInPwrFlag = LOWER_FLAG;
            MYALARM.stDlInUnderPwr.ucRcStatus = ALARM; //下行输入欠功率告警
            cDetectDlInPwr = 1;
        }
        else
        {
            g_stRcParamSet.cDlInPwr = g_stRcParamSet.cSsBcchRxLev;
            g_stRcParamSet.sDlInPwr = g_stRcParamSet.cDlInPwr * 10;
        }
    }
    //============ 用功率检测模块来检测下行输入功率的话 ============
    else
    {
        if(g_stDevTypeTable.ucIsCoupling)
        {
            ucUpPwrSpecial = 1;  //直接耦合机型标志
        }

        if(ucUpPwrSpecial == 1)  //直接耦合机型标志
        {
            TempData = CountBigPwr3G(uiValueTemp[11]);
        }
        else
        {
            TempData = CountSmallPwr3G(uiValueTemp[11]);
        }
        //3G:下行输入功率电平，sint2型，单位为dBm，比例为10
        g_stRcParamSet.sDlInPwr = TempData;//(SHORT)(TempData) * 10;
        //下行输入功率电平
        g_stRcParamSet.cDlInPwr = (SHORT)(TempData) / 10;//TempData;
    }
    //============ 不论哪种方式来检测，检测到数据后的告警判断 ============
    if(cDetectDlInPwr == 0)
    {
        TempData = g_stRcParamSet.cDlInPwr;
        if(TempData > (-30 + ucUpPwrSpecial * 40)) //对于输入信号是经过无线耦合的设备，要求检测范围在－30dBm至－80dBm
        {
            g_stRcParamSet.ucDlInPwrFlag = HIGHER_FLAG; //下行输入功率电平高于检测范围
            g_stRcParamSet.uc3GDlInPwrFlag = HIGHER_FLAG; //下行输入功率电平超出范围的标志
            MYALARM.stDlInOverPwr.ucRcStatus = ALARM; //下行输入过功率告警
            //MCM-29_20061110_zhangjie_begin
            MYALARM.stDlInUnderPwr.ucRcStatus = NOT_ALARM; //下行输入欠功率告警
            //MCM-29_20061110_zhangjie_end
        }
        else if(TempData < (-80 + ucUpPwrSpecial * 50)) //对于输入信号是经过无线耦合的设备，要求检测范围在－30dBm至－80dBm
        {
            g_stRcParamSet.ucDlInPwrFlag = LOWER_FLAG; //下行输入功率电平低于检测范围
            g_stRcParamSet.uc3GDlInPwrFlag = LOWER_FLAG;
            MYALARM.stDlInUnderPwr.ucRcStatus = ALARM; //下行输入欠功率告警
            //MCM-29_20061110_zhangjie_begin
            MYALARM.stDlInOverPwr.ucRcStatus = NOT_ALARM; //下行输入过功率告警
            //MCM-29_20061110_zhangjie_end
        }
        else
        {        //--------- 值在正常范围内时的告警判断 ---------
            if(g_stDevTypeTable.ucGenType == GEN_TYPE_3G)  //3G机型的设备类型的值都大于49
            {   //3G机型时的下行输入告警判断
                if(g_stRcParamSet.sDlInPwr < g_stSettingParamSet.sDlInUnderPwrThr) //3G下行输入功率欠功率门限
                {
                    MYALARM.stDlInUnderPwr.ucRcStatus = ALARM; //下行输入欠功率告警
                    MYALARM.stDlInOverPwr.ucRcStatus = NOT_ALARM; //下行输入过功率不告警
                }
                else if(g_stRcParamSet.sDlInPwr > g_stSettingParamSet.sDlInOverPwrThr) //3G下行输入功率过功率门限
                {
                    MYALARM.stDlInOverPwr.ucRcStatus = ALARM; //下行输入过功率告警
                    MYALARM.stDlInUnderPwr.ucRcStatus = NOT_ALARM; //下行输入欠功率不告警
                }
                else
                {
                    MYALARM.stDlInOverPwr.ucRcStatus = NOT_ALARM; //下行输入过功率不告警
                    MYALARM.stDlInUnderPwr.ucRcStatus = NOT_ALARM; //下行输入欠功率不告警
                }
            }
            else
            {   //2G机型时的下行输入告警判断
                if(g_stRcParamSet.cDlInPwr < g_stSettingParamSet.cDlInUnderPwrThr) //下行输入功率欠功率门限
                {
                    MYALARM.stDlInUnderPwr.ucRcStatus = ALARM; //下行输入欠功率告警
                    MYALARM.stDlInOverPwr.ucRcStatus = NOT_ALARM; //下行输入过功率不告警
                }
                else if(g_stRcParamSet.cDlInPwr > g_stSettingParamSet.cDlInOverPwrThr) //下行输入功率过功率门限
                {
                    MYALARM.stDlInOverPwr.ucRcStatus = ALARM; //下行输入过功率告警
                    MYALARM.stDlInUnderPwr.ucRcStatus = NOT_ALARM; //下行输入欠功率不告警
                }
                else
                {
                    MYALARM.stDlInOverPwr.ucRcStatus = NOT_ALARM; //下行输入过功率不告警
                    MYALARM.stDlInUnderPwr.ucRcStatus = NOT_ALARM; //下行输入欠功率不告警
                    }
                }
            }
        }
    }
    //%%%%%%%%%%%%%%%%%%%%%%%%%% 上行输出功率电平 %%%%%%%%%%%%%%%%%%%%%%%%%%
    g_stRcParamSet.sUlOutPwr = 0; //3G:上行输出功率电平，sint2型，单位为dBm，比例为10
    g_stRcParamSet.cUlOutPwr = 0; //上行输出功率电平

    if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)  //如果是2G设备类型，则调用2G的计算函数
    {
        TempData = CountUlPow2G(uiValueTemp[2]);
    }
    else               //否则调用3G的计算函数
    {
        TempData = CountPowOut3G(uiValueTemp[2], UpLoad);
    }
    
    MYALARM.stUlOutOverPwr.ucRcStatus = NOT_ALARM;
    MYALARM.stUlOutUnderPwr.ucRcStatus = NOT_ALARM;
    
    if(TempData == HIGHER_VALUE)
    {
        g_stRcParamSet.ucUlOutPwrFlag = HIGHER_FLAG; //上行输出功率电平超出范围的标志
        g_stRcParamSet.uc3GUlOutPwrFlag = HIGHER_FLAG;
        MYALARM.stUlOutOverPwr.ucRcStatus = ALARM;  //上行输出过功率告警
    }
    else if(TempData == LOWER_VALUE)
    {
        g_stRcParamSet.ucUlOutPwrFlag = LOWER_FLAG; //上行输出功率电平超出范围的标志
        g_stRcParamSet.uc3GUlOutPwrFlag = LOWER_FLAG;
        MYALARM.stUlOutUnderPwr.ucRcStatus = ALARM;  //上行输出欠功率告警
    }
    else
    {
        g_stRcParamSet.sUlOutPwr = TempData; //3G:上行输出功率电平，sint2型，单位为dBm，比例为10
        g_stRcParamSet.cUlOutPwr = TempData; //2G:上行输出功率电平
        g_stRcParamSet.ucUlOutPwrFlag = NORMAL_FLAG; //上行输出功率电平超出范围的标志
        g_stRcParamSet.uc3GUlOutPwrFlag = NORMAL_FLAG;
        //--------- 以下为告警判断 ---------
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_3G)  //3G机型时的上行输出告警判断
        {   //3G机型时的上行输出告警判断
            if(g_stRcParamSet.sUlOutPwr > g_stSettingParamSet.sUlOutOverPwrThr) //3G:上行输出功率过功率门限
            {
                MYALARM.stUlOutOverPwr.ucRcStatus = ALARM;  //上行输出过功率告警
            }
            else if(g_stRcParamSet.sUlOutPwr < g_stSettingParamSet.sUlOutUnderPwrThr) //3G:上行输出功率欠功率门限
            {
                MYALARM.stUlOutUnderPwr.ucRcStatus = ALARM;  //上行输出欠功率告警
            }
        }
        else if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)   //2G机型时的上行输出告警判断
        {
            if(g_stRcParamSet.cUlOutPwr > g_stSettingParamSet.cUlOutOverPwrThr) //上行输出功率过功率门限
            {
                MYALARM.stUlOutOverPwr.ucRcStatus = ALARM;  //上行输出过功率告警
            }
        }
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%% 下行输出功率电平 %%%%%%%%%%%%%%%%%%%%%%%%%%
    g_stRcParamSet.cDlOutPwr = 0; //下行输出功率电平  sint1型，单位为dBm
    g_stRcParamSet.sDlOutPwr = 0; //3G:下行输出功率电平，sint2型，单位为dBm，比例为10
    
    if(g_stSettingParamSet.ucRfSw == 0)   //射频开关被关闭
    {
        g_stRcParamSet.ucDlOutPwrFlag = LOWER_FLAG; //下行输出功率电平超出范围的标志
        g_stRcParamSet.uc3GDlOutPwrFlag = LOWER_FLAG;
    }
    else
    {
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)  //如果是2G设备类型，则调用2G的计算函数
        {
            TempData = CountDlPow2G(uiValueTemp[14]);
        }
        else               //否则调用3G的计算函数
        {
            TempData = CountPowOut3G(uiValueTemp[14], DownLoad);
        }

        MYALARM.stDlOutOverPwr.ucRcStatus = NOT_ALARM;  //取消下行输出过功率告警
        MYALARM.stDlOutUnderPwr.ucRcStatus = NOT_ALARM;  //取消下行输出欠功率告警
        if(TempData == HIGHER_VALUE)
        {
            g_stRcParamSet.ucDlOutPwrFlag = HIGHER_FLAG; //下行输出功率电平超出范围的标志
            g_stRcParamSet.uc3GDlOutPwrFlag = HIGHER_FLAG;
            MYALARM.stDlOutOverPwr.ucRcStatus = ALARM;  //下行输出过功率告警
        }
        else if(TempData == LOWER_VALUE)
        {
            g_stRcParamSet.ucDlOutPwrFlag = LOWER_FLAG; //下行输出功率电平超出范围的标志
            g_stRcParamSet.uc3GDlOutPwrFlag = LOWER_FLAG;
            MYALARM.stDlOutUnderPwr.ucRcStatus = ALARM;  //下行输出欠功率告警
        }
        else
        {
            g_stRcParamSet.cDlOutPwr = TempData; //下行输出功率电平  sint1型，单位为dBm
            g_stRcParamSet.sDlOutPwr = TempData; //3G:下行输出功率电平，sint2型，单位为dBm，比例为10
            g_stRcParamSet.ucDlOutPwrFlag = NORMAL_FLAG; //下行输出功率电平超出范围的标志
            g_stRcParamSet.uc3GDlOutPwrFlag = NORMAL_FLAG;

            //--------- 以下为告警判断 ---------
            if(g_stDevTypeTable.ucGenType == GEN_TYPE_3G)  //3G机型时的上行输出告警判断
            {   //3G机型时的下行输出告警判断
                if(g_stRcParamSet.sDlOutPwr < g_stSettingParamSet.sDlOutUnderPwrThr) //下行输出功率欠功率门限
                {
                    MYALARM.stDlOutUnderPwr.ucRcStatus = ALARM;  //下行输出欠功率告警
                }
                else if(g_stRcParamSet.sDlOutPwr > g_stSettingParamSet.sDlOutOverPwrThr) //下行输出功率过功率门限
                {
                    MYALARM.stDlOutOverPwr.ucRcStatus = ALARM;  //下行输出过功率告警
                }
            }
            else if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)   //2G机型时的上行输出告警判断
            {
                if(g_stRcParamSet.cDlOutPwr < g_stSettingParamSet.cDlOutUnderPwrThr) //下行输出功率欠功率门限
                {
                    MYALARM.stDlOutUnderPwr.ucRcStatus = ALARM;  //下行输出欠功率告警
                }
                else if(g_stRcParamSet.cDlOutPwr > g_stSettingParamSet.cDlOutOverPwrThr) //下行输出功率过功率门限
                {
                    MYALARM.stDlOutOverPwr.ucRcStatus = ALARM;  //下行输出过功率告警
                }
            }
        }
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%% 下行输出功率电平(通道二) %%%%%%%%%%%%%%%%%%%%%%%%%%
    g_stRcParamSet.cDlOutPwr2 = 0; //下行输出功率电平(通道二)  sint1型，单位为dBm
    g_stRcParamSet.sDlOutPwr2 = 0; //3G:下行输出功率电平(通道二)，sint2型，单位为dBm，比例为10
    
    if(g_stSettingParamSet.ucRfSw == 0)   //射频开关被关闭
    {
        g_stRcParamSet.ucDlOutPwrFlag2 = LOWER_FLAG; //下行输出功率电平(通道二)超出范围的标志
        g_stRcParamSet.uc3GDlOutPwrFlag2 = LOWER_FLAG;
    }
    else
    {
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)  //如果是2G设备类型，则调用2G的计算函数
        {
            TempData = CountDlPow2GChunnel2(uiValueTemp[17]);
        }
        else               //否则调用3G的计算函数
        {
            TempData = CountPowOut3GChunnel2(uiValueTemp[17], DownLoad);
        }

        MYALARM.stDlOutOverPwr2.ucRcStatus = NOT_ALARM;  //取消下行输出过功率(通道二)告警
        MYALARM.stDlOutUnderPwr2.ucRcStatus = NOT_ALARM;  //取消下行输出欠功率(通道二)告警
        if(TempData == HIGHER_VALUE)
        {
            g_stRcParamSet.ucDlOutPwrFlag2 = HIGHER_FLAG; //下行输出功率电平(通道二)超出范围的标志
            g_stRcParamSet.uc3GDlOutPwrFlag2 = HIGHER_FLAG;
            MYALARM.stDlOutOverPwr2.ucRcStatus = ALARM;  //下行输出过功率告警(通道二)
        }
        else if(TempData == LOWER_VALUE)
        {
            g_stRcParamSet.ucDlOutPwrFlag2 = LOWER_FLAG; //下行输出功率电平(通道二)超出范围的标志
            g_stRcParamSet.uc3GDlOutPwrFlag2 = LOWER_FLAG;
            MYALARM.stDlOutUnderPwr2.ucRcStatus = ALARM;  //下行输出欠功率告警(通道二)
        }
        else
        {
            g_stRcParamSet.cDlOutPwr2 = TempData; //下行输出功率电平(通道二)  sint1型，单位为dBm
            g_stRcParamSet.sDlOutPwr2 = TempData; //3G:下行输出功率电平(通道二)，sint2型，单位为dBm，比例为10
            g_stRcParamSet.ucDlOutPwrFlag2 = NORMAL_FLAG; //下行输出功率电平(通道二)超出范围的标志
            g_stRcParamSet.uc3GDlOutPwrFlag2 = NORMAL_FLAG;

            //--------- 以下为告警判断 ---------
            if(g_stDevTypeTable.ucGenType == GEN_TYPE_3G)  //3G机型时的上行输出告警判断
            {   //3G机型时的下行输出告警判断
                if(g_stRcParamSet.sDlOutPwr2 < g_stSettingParamSet.sDlOutUnderPwrThr2) //下行输出功率欠功率门限
                {
                    MYALARM.stDlOutUnderPwr2.ucRcStatus = ALARM;  //下行输出欠功率告警
                }
                else if(g_stRcParamSet.sDlOutPwr2 > g_stSettingParamSet.sDlOutOverPwrThr2) //下行输出功率过功率门限
                {
                    MYALARM.stDlOutOverPwr2.ucRcStatus = ALARM;  //下行输出过功率告警
                }
            }
            else if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)   //2G机型时的上行输出告警判断
            {
                if(g_stRcParamSet.cDlOutPwr2 < g_stSettingParamSet.cDlOutUnderPwrThr2) //下行输出功率欠功率门限
                {
                    MYALARM.stDlOutUnderPwr2.ucRcStatus = ALARM;  //下行输出欠功率告警
                }
                else if(g_stRcParamSet.cDlOutPwr2 > g_stSettingParamSet.cDlOutOverPwrThr2) //下行输出功率过功率门限
                {
                    MYALARM.stDlOutOverPwr2.ucRcStatus = ALARM;  //下行输出过功率告警
                }
            }
        }
    }
 
    //%%%%%%%%%%%%%%%%%%%%%%%%%% 下行驻波比值 %%%%%%%%%%%%%%%%%%%%%%%%%%
    TempData = g_stYkppParamSet.stYkppRfParam.ucDlOutMax;  //下行输出最大增益

    MYALARM.stDlSwr.ucRcStatus = NOT_ALARM;  //先取消下行驻波比告警
    
    if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)  //========= 2G设备的下行驻波计算 ==========
    {
        if(g_stRcParamSet.ucDlOutPwrFlag == LOWER_FLAG) //下行输出功率低于检测范围时，驻波也返回低于检测范围
        {
            g_stRcParamSet.ucDlSwrFlag = LOWER_FLAG; //下行驻波比值超出范围的标志
        }
        else if(g_stRcParamSet.cDlOutPwr > TempData)  //下行输出功率高于额定输出功率
        {
            g_stRcParamSet.ucDlSwrFlag = HIGHER_FLAG; //下行驻波比值超出范围的标志
        }
        else if(g_stRcParamSet.cDlOutPwr < (TempData - 35)) //下行输出功率低于额定输出功率-35dB
        {
            g_stRcParamSet.ucDlSwrFlag = LOWER_FLAG; //下行驻波比值超出范围的标志
        }
        else
        {
            //下行驻波比值，UCHAR型，比例为10
            g_stRcParamSet.ucDlSwr
                   = CountVSWR2G(uiValueTemp[6], (UINT32)(g_stRcParamSet.cDlOutPwr));
            g_stRcParamSet.ucDlSwrFlag = NORMAL_FLAG; //下行驻波比值超出范围的标志
            //--------- 以下为告警判断 ---------
            if(g_stRcParamSet.ucDlSwr > g_stSettingParamSet.ucDlSwrThr) //下行驻波比门限
            {
                MYALARM.stDlSwr.ucRcStatus = ALARM;  //下行驻波比告警
            }
        }
    }
    else    //========= 3G设备的下行驻波计算 ==========
    {
        if(g_stRcParamSet.ucDlOutPwrFlag == LOWER_FLAG) //下行输出功率低于检测范围时，驻波也返回低于检测范围
        {
            g_stRcParamSet.ucDlSwrFlag = LOWER_FLAG; //下行驻波比值超出范围的标志
        }
        else if(g_stRcParamSet.sDlOutPwr > (TempData*10))  //下行输出功率高于额定输出功率
        {
            g_stRcParamSet.ucDlSwrFlag = HIGHER_FLAG; //下行驻波比值超出范围的标志
        }
        else if(g_stRcParamSet.sDlOutPwr < (TempData*10 - 350)) //下行输出功率低于额定输出功率-35dB
        {
            g_stRcParamSet.ucDlSwrFlag = LOWER_FLAG; //下行驻波比值超出范围的标志
        }
        else
        {
            //下行驻波比值，UCHAR型，比例为10
            g_stRcParamSet.ucDlSwr
                   = CountVSWR3G(uiValueTemp[6], (UINT32)(g_stRcParamSet.sDlOutPwr));
            g_stRcParamSet.ucDlSwrFlag = NORMAL_FLAG; //下行驻波比值超出范围的标志
            //--------- 以下为告警判断 ---------
            if(g_stRcParamSet.ucDlSwr > g_stSettingParamSet.ucDlSwrThr) //下行驻波比门限
            {
                MYALARM.stDlSwr.ucRcStatus = ALARM;  //下行驻波比告警
            }
        }
    }
 
    //%%%%%%%%%%%%%%%%%%%%%%%%%% 下行驻波比值(通道二) %%%%%%%%%%%%%%%%%%%%%%%%%%
    TempData = g_stYkppParamSet.stYkppRfParam.ucDlOutMax2;  //下行输出最大增益

    MYALARM.stDlSwr2.ucRcStatus = NOT_ALARM;  //先取消下行驻波比告警
    
    if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)  //========= 2G设备的下行驻波计算 ==========
    {
        if(g_stRcParamSet.ucDlOutPwrFlag2 == LOWER_FLAG) //下行输出功率低于检测范围时，驻波也返回低于检测范围
        {
            g_stRcParamSet.ucDlSwrFlag2 = LOWER_FLAG; //下行驻波比值超出范围的标志
        }
        else if(g_stRcParamSet.cDlOutPwr2 > TempData)  //下行输出功率高于额定输出功率
        {
            g_stRcParamSet.ucDlSwrFlag2 = HIGHER_FLAG; //下行驻波比值超出范围的标志
        }
        else if(g_stRcParamSet.cDlOutPwr2 < (TempData - 35)) //下行输出功率低于额定输出功率-35dB
        {
            g_stRcParamSet.ucDlSwrFlag2 = LOWER_FLAG; //下行驻波比值超出范围的标志
        }
        else
        {
            //下行驻波比值，UCHAR型，比例为10
            g_stRcParamSet.ucDlSwr2
                   = CountVSWR2GChunnel2(uiValueTemp[21], (UINT32)(g_stRcParamSet.cDlOutPwr2));
            g_stRcParamSet.ucDlSwrFlag2 = NORMAL_FLAG; //下行驻波比值超出范围的标志
            //--------- 以下为告警判断 ---------
            if(g_stRcParamSet.ucDlSwr2 > g_stSettingParamSet.ucDlSwrThr2) //下行驻波比门限
            {
                MYALARM.stDlSwr2.ucRcStatus = ALARM;  //下行驻波比告警
            }
        }
    }
    else    //========= 3G设备的下行驻波计算 ==========
    {
        if(g_stRcParamSet.ucDlOutPwrFlag2 == LOWER_FLAG) //下行输出功率低于检测范围时，驻波也返回低于检测范围
        {
            g_stRcParamSet.ucDlSwrFlag2 = LOWER_FLAG; //下行驻波比值超出范围的标志
        }
        else if(g_stRcParamSet.sDlOutPwr2 > (TempData*10))  //下行输出功率高于额定输出功率
        {
            g_stRcParamSet.ucDlSwrFlag2 = HIGHER_FLAG; //下行驻波比值超出范围的标志
        }
        else if(g_stRcParamSet.sDlOutPwr2 < (TempData*10 - 350)) //下行输出功率低于额定输出功率-35dB
        {
            g_stRcParamSet.ucDlSwrFlag2 = LOWER_FLAG; //下行驻波比值超出范围的标志
        }
        else
        {
            //下行驻波比值，UCHAR型，比例为10
            g_stRcParamSet.ucDlSwr2
                   = CountVSWR3GChunnel2(uiValueTemp[21], (UINT32)(g_stRcParamSet.sDlOutPwr2));
            g_stRcParamSet.ucDlSwrFlag2 = NORMAL_FLAG; //下行驻波比值超出范围的标志
            //--------- 以下为告警判断 ---------
            if(g_stRcParamSet.ucDlSwr2 > g_stSettingParamSet.ucDlSwrThr2) //下行驻波比门限
            {
                MYALARM.stDlSwr2.ucRcStatus = ALARM;  //下行驻波比告警
            }
        }
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%% 功放温度 %%%%%%%%%%%%%%%%%%%%%%%%%%
    uiValueTemp[4] = uiValueTemp[4] > uiValueTemp[12] ? uiValueTemp[4] : uiValueTemp[12];
    uiValueTemp[4] = uiValueTemp[4] > uiValueTemp[8] ? uiValueTemp[4] : uiValueTemp[8];
    uiValueTemp[4] = uiValueTemp[4] > uiValueTemp[10] ? uiValueTemp[4] : uiValueTemp[10];
    uiValueTemp[4] = uiValueTemp[4] > uiValueTemp[20] ? uiValueTemp[4] : uiValueTemp[20];
    uiValueTemp[4] = uiValueTemp[4] > uiValueTemp[22] ? uiValueTemp[4] : uiValueTemp[22];
    if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)  //如果是2G设备类型，则调用2G的计算函数
    {
        g_stRcParamSet.cPaTemp = CountTem2G(uiValueTemp[4]); //功放温度值，sint1型，单位为℃，多块功放取最大值
    }
    else
    {
        g_stRcParamSet.cPaTemp = CountTem3G(uiValueTemp[4]); //功放温度值，sint1型，单位为℃，多块功放取最大值
    }
    if(g_stRcParamSet.cPaTemp > g_stSettingParamSet.cPaOverheatThr)  //功放过温度告警门限
    {
        MYALARM.stPaOverheat.ucRcStatus = ALARM;  //功放过温告警
    }
    else 
    {
        MYALARM.stPaOverheat.ucRcStatus = NOT_ALARM;  //取消功放过温告警
    }
    //%%%%%%%%%%%%%%%%%%%%%%%%%% 上行理论增益 %%%%%%%%%%%%%%%%%%%%%%%%%%
    //上行理论增益  sint1型，单位为dB (上行最大增益-上行衰减值)
    //MCM-17_20061106_linyu_begin
    g_stRcParamSet.cUlTheGain  = g_stYkppParamSet.stYkppRfParam.ucUlGainMax - g_stSettingParamSet.ucUlAtt;  
    //MCM-17_20061106_linyu_end
    g_stRcParamSet.cUlTheGain2 = g_stYkppParamSet.stYkppRfParam.ucUlGainMax - g_stSettingParamSet.ucUlAtt2;  
    //       在设置衰耗或设置上行最大增益时需要修改上行理论增益。平时不变

    //%%%%%%%%%%%%%%%%%%%%%%%%%% 下行实际增益 %%%%%%%%%%%%%%%%%%%%%%%%%%
    if((g_stRcParamSet.ucDlInPwrFlag == LOWER_FLAG)
            || (g_stRcParamSet.ucDlOutPwrFlag == LOWER_FLAG) )
    {//如果下行输入或输出功率低于检测范围的话
        g_stRcParamSet.ucDlActGainFlag = LOWER_FLAG; //下行实际增益超出范围的标志
    }
    else if((g_stRcParamSet.ucDlInPwrFlag == HIGHER_FLAG)
            || (g_stRcParamSet.ucDlOutPwrFlag == HIGHER_FLAG))
    {//如果下行输入或输出功率高于检测范围的话
        g_stRcParamSet.ucDlActGainFlag = HIGHER_FLAG; //下行实际增益超出范围的标志
    }
    else
    {
        //下行输入、输出功率都正常，则下行实际增益=下行输出-下行输入。
        //MCM-39_20061120_linwei_begin
        g_stRcParamSet.ucDlActGainFlag = NORMAL_FLAG; //下行实际增益超出范围的标志
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G) //2G设备的下行实际增益计算
        {
            g_stRcParamSet.cDlActGain = (g_stRcParamSet.cDlOutPwr)
                        - (g_stRcParamSet.cDlInPwr);
        }
        else //3G设备的下行实际增益计算
        {
            g_stRcParamSet.cDlActGain = (g_stRcParamSet.sDlOutPwr + 5) / 10
                        - g_stRcParamSet.cDlInPwr;        
        }
        //MCM-39_20061120_linwei_end
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%% 信源变化告警 %%%%%%%%%%%%%%%%%%%%%%%%%%
    //USHORT usSsCi;  //信源信息：小区识别码实时值
    //USHORT usSrcCellId;  //信源小区识别码参照值
    if(g_stRcParamSet.usSsCi == g_stSettingParamSet.usSrcCellId) //信源小区识别码参照值
    {
        MYALARM.stSrcSigChanged.ucRcStatus = NOT_ALARM; //取消信源变化告警
    }
    else
    {
        MYALARM.stSrcSigChanged.ucRcStatus = ALARM; //信源变化告警
    }

    //%%%%%%%%%%%%%%%%%%%%%%% 监控模块电池故障告警判断 %%%%%%%%%%%%%%%%%%%%%%%
    //UCHAR ucTempType;  //设备类型
    //(uiValueTemp[11] / 1024)*2.5*6*100  得到电池电压的100倍数值
    uiValueTemp[3] = (uiValueTemp[3] * 5 * 3 * 25 + 128) / 256; //"+128"为了四舍五入
    g_stRcParamSet.usModuleBatValue = uiValueTemp[3] + 10;  //"+10"是为了校准一下

    //%%%%%%%%%%%%%%%%%%%%%%% 电源故障告警判断 %%%%%%%%%%%%%%%%%%%%%%%
    // uiValueTemp[9]  12V输出  低于9V为故障
    // uiValueTemp[7]  27V输出  低于20V为故障
    
    MYALARM.stPwrFault.ucRcStatus = NOT_ALARM; //电源故障告警
    if(uiValueTemp[9] < 153)
    {
        MYALARM.stPwrFault.ucRcStatus = ALARM; //电源故障告警
    }
    //zhangjie 20071120 太阳能蓄电池电压
    //计算电压值 电压值最高32.5v，对应的g_stRcParamSet.usSecBatVol的值为3250，对应于AD的值即uiValueTemp[1]= 1028
    //5v对应uiValueTemp[1]= 158
    g_stRcParamSet.usSecBatVol = uiValueTemp[1]*500/158;
    //zhangjie 20071120 太阳能蓄电池电压
}
