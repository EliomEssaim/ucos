/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   Poi_conversion.cpp
    作者:     林玮
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  
       作者:  
       描述:  代码编写完成
    2. 日期:  2006/11/06
       作者:  林雨
       描述:  修改问题MCM-17，将所有带通道号的通道1的参数都映射对应的
              到不带通道号的参数上，即当设备是多通道时，查询通道1的参
              数实际访问的是对应的不带通道号的参数的地址。
    3. 日期:  2006/11/06
       作者:  林雨
       描述:  修改GENNERAL笔误
---------------------------------------------------------------------------*/
/*************************************************
  Copyright (C), 2006-2007, Lin Wei
  文件名：    poi_conversion.c
  作者:       林玮
  版本：      1.0.0
  日期：      2006/07/19 
  功能描述：  此文件实现将A/D模拟量采集值根据不同计算公式，得到相应结果
  备注：      此文件仅针对邮科POI项目
  函数列表：
        void PutIntoRam(UINT32* uiValueTemp);
*************************************************/

#define GENERAL_COUPLER  30  //通用的耦合器参数数值
#define GENERAL_MAXVALUE 20  //通用的功率电平检测器能检测到的最大功率电平值
#define REFLECT_MAXVALUE 10  //反射信号检测时能检测到的最大功率电平值

//**********************************************************
//功能：将采集值通过计算得出功率电平值(dBm),并进行告警状态判断
//输入：lPickValue -- 采集结果
//      lMaxValue -- 功率电平最大值
//      sAdjustValue -- 校准值
//      ucCoupler -- 耦合器参数
//返回：功率电平值
//**********************************************************
CHAR CountPow(LONG lPickValue, LONG lMaxValue, SHORT sAdjustValue, UCHAR ucCoupler)
{
    LONG lTempPowOut;
    //N=MAX-(45-x*25/512)+Adjust
    lTempPowOut = lMaxValue - (45 - (lPickValue * 25 + 256) / 512) + (LONG)sAdjustValue;

    if(lTempPowOut > lMaxValue)
    {
        return (CHAR)(lMaxValue + ucCoupler);  //高于检测范围时的返回值
    }
    else if(lTempPowOut < (lMaxValue - 32))
    {
        return 0;  //低于检测范围时的返回值
    }
    else
    {
        return (CHAR)(lTempPowOut + ucCoupler);
    }
}

//**********************************************************
//功能： 将驻波比采样电平值，根据公式计算出对应的驻波比值
//输入： lPickValue -- 反射功率采集结果
//       lMaxValue -- 功率电平最大值
//       sAdjustValue -- 反射功率电平校准值
//       ucCoupler -- 耦合器参数
//       cDownOut -- 对应的下行输出功率值
//返回： 驻波比（10倍）
//**********************************************************
UCHAR CountVSWR(LONG lPickValue, LONG lMaxValue, SHORT sAdjustValue, UCHAR ucCoupler, CHAR cDownOut)
{
    CHAR cReflectPower;

    cReflectPower = CountPow(lPickValue, lMaxValue, sAdjustValue, ucCoupler);
    cReflectPower = cDownOut - cReflectPower;   //将输出功率减去反射功率。
    
    if(cReflectPower >= 17)
    {
        return 12;   //大于17时，驻波比值为1.2
    }
    else if(cReflectPower >= 16)
    {
        return 13;   //大于16时，驻波比值为1.3
    }
    else if(cReflectPower >= 15)
    {
        return 14;   //等于15或16时，驻波比值为1.4
    }
    else if(cReflectPower >= 13)
    {
        return 15;   //等于13或14时，驻波比值为1.5
    }
    else if(cReflectPower >= 12)
    {
        return 16;   //等于12时，驻波比值为1.6
    }
    else if(cReflectPower >= 11)
    {
        return 17;   //等于11时，驻波比值为1.7
    }
    else if(cReflectPower >= 10)
    {
        return 18;   //等于10时，驻波比值为1.8
    }
    else if (cReflectPower >= 8)
    {
        return 20;   //等于8或9时，驻波比值为2.0
    }
    else if (cReflectPower >= 7)
    {
        return 23;   // 7时，驻波比值为2.3
    }
    else if (cReflectPower >= 6)
    {
        return 25;   // 6时，驻波比值为2.5
    }
    else if (cReflectPower >= 4)
    {
        return 30;   // 4~5时，驻波比值为3.0
    }
    else return 40;     //小于4时，驻波比值为显示为4.0
}

//**********************************************************
//功能： 
//输入： uiValueTemp  -- 
//返回： 
//**********************************************************
void PutIntoRam(ULONG* ulValueTemp)
{
    CHAR cTempAdjust;   //校准值临时存放
    UCHAR ucTempMaxValue; //最大功率电平临时存放
    UCHAR ucTempCoupler;  //耦合器参数临时存放
    CHAR cTempData;

    //#########################################################################
    //########################## CDMA800输入功率电平 ##########################
    
    MYALARM.stCdma800InOverPwr.ucRcStatus = NOT_ALARM;  //CDMA800输入过功率告警
    MYALARM.stCdma800InUnderPwr.ucRcStatus = NOT_ALARM; //CDMA800输入欠功率告警

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cCdma800InPwrAdjust;  // CDMA800输入功率电平校准值
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[0], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cCdma800InPwr = cTempData; //CDMA800输入功率电平值

    //--------- 以下为告警判断 ---------
    if(g_stRcParamSet.cCdma800InPwr < g_stSettingParamSet.cCdma800InUnderPwrThr) //CDMA800输入功率欠功率门限
    {
        MYALARM.stCdma800InUnderPwr.ucRcStatus = ALARM;  //CDMA800输入欠功率告警
    }
    else if(g_stRcParamSet.cCdma800InPwr > g_stSettingParamSet.cCdma800InOverPwrThr) //CDMA800输入功率过功率门限
    {
        MYALARM.stCdma800InOverPwr.ucRcStatus = ALARM;  //CDMA800输入过功率告警
    }

    //#########################################################################
    //######################### 移动GSM输入功率电平值 #########################
    
    MYALARM.stMbGsmInOverPwr.ucRcStatus = NOT_ALARM;  //移动GSM输入过功率告警
    MYALARM.stMbGsmInUnderPwr.ucRcStatus = NOT_ALARM; //移动GSM输入欠功率告警

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cMbGsmInPwrAdjust;  //移动GSM输入功率电平校准值
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[1], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cMbGsmInPwr = cTempData; //移动GSM输入功率电平值

    //--------- 以下为告警判断 ---------
    if(g_stRcParamSet.cMbGsmInPwr < g_stSettingParamSet.cMbGsmInUnderPwrThr) //移动GSM输入功率欠功率门限
    {
        MYALARM.stMbGsmInUnderPwr.ucRcStatus = ALARM;  //移动GSM输入欠功率告警
    }
    else if(g_stRcParamSet.cMbGsmInPwr > g_stSettingParamSet.cMbGsmInOverPwrThr) //移动GSM输入功率过功率门限
    {
        MYALARM.stMbGsmInOverPwr.ucRcStatus = ALARM;  //移动GSM输入过功率告警
    }

    //#########################################################################
    //######################### 联通GSM输入功率电平值 #########################
    
    MYALARM.stUcGsmInOverPwr.ucRcStatus = NOT_ALARM;  //联通GSM输入过功率告警
    MYALARM.stUcGsmInUnderPwr.ucRcStatus = NOT_ALARM; //联通GSM输入欠功率告警

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cUcGsmInPwrAdjust;  //联通GSM输入功率电平校准值
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[2], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cUcGsmInPwr = cTempData; //移动GSM输入功率电平值

    //--------- 以下为告警判断 ---------
    if(g_stRcParamSet.cUcGsmInPwr < g_stSettingParamSet.cUcGsmInUnderPwrThr) //联通GSM输入功率欠功率门限
    {
        MYALARM.stUcGsmInUnderPwr.ucRcStatus = ALARM;  //联通GSM输入欠功率告警
    }
    else if(g_stRcParamSet.cUcGsmInPwr > g_stSettingParamSet.cUcGsmInOverPwrThr) //联通GSM输入功率过功率门限
    {
        MYALARM.stUcGsmInOverPwr.ucRcStatus = ALARM;  //联通GSM输入过功率告警
    }

    //#########################################################################
    //######################### 移动DCS输入功率电平值 #########################
    
    MYALARM.stMbDcsInOverPwr.ucRcStatus = NOT_ALARM;  //移动DCS输入过功率告警
    MYALARM.stMbDcsInUnderPwr.ucRcStatus = NOT_ALARM; //移动DCS输入欠功率告警

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cMbDcsInPwrAdjust;  //移动DCS输入功率电平校准值
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[3], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cMbDcsInPwr = cTempData; //移动GSM输入功率电平值

    //--------- 以下为告警判断 ---------
    if(g_stRcParamSet.cMbDcsInPwr < g_stSettingParamSet.cMbDcsInUnderPwrThr) //移动DCS输入功率欠功率门限
    {
        MYALARM.stMbDcsInUnderPwr.ucRcStatus = ALARM;  //移动DCS输入欠功率告警
    }
    else if(g_stRcParamSet.cMbDcsInPwr > g_stSettingParamSet.cMbDcsInOverPwrThr) //移动DCS输入功率过功率门限
    {
        MYALARM.stMbDcsInOverPwr.ucRcStatus = ALARM;  //移动DCS输入过功率告警
    }

    //#########################################################################
    //######################### 联通DCS输入功率电平值 #########################
    
    MYALARM.stUcDcsInOverPwr.ucRcStatus = NOT_ALARM;  //联通DCS输入过功率告警
    MYALARM.stUcDcsInUnderPwr.ucRcStatus = NOT_ALARM; //联通DCS输入欠功率告警

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cUcDcsInPwrAdjust;  //联通DCS输入功率电平校准值
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[4], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cUcDcsInPwr = cTempData; //联通DCS输入功率电平值

    //--------- 以下为告警判断 ---------
    if(g_stRcParamSet.cUcDcsInPwr < g_stSettingParamSet.cUcDcsInUnderPwrThr) //联通DCS输入功率欠功率门限
    {
        MYALARM.stUcDcsInUnderPwr.ucRcStatus = ALARM;  //联通DCS输入欠功率告警
    }
    else if(g_stRcParamSet.cUcDcsInPwr > g_stSettingParamSet.cUcDcsInOverPwrThr) //联通DCS输入功率过功率门限
    {
        MYALARM.stUcDcsInOverPwr.ucRcStatus = ALARM;  //联通DCS输入过功率告警
    }

    //#########################################################################
    //######################### 3G1-FDD输入功率电平值 #########################
    
    MYALARM.st3G1FDDInOverPwr.ucRcStatus = NOT_ALARM;  //3G1-FDD输入过功率告警
    MYALARM.st3G1FDDInUnderPwr.ucRcStatus = NOT_ALARM; //3G1-FDD输入欠功率告警

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.c3G1FDDInPwrAdjust;  //3G1-FDD输入功率电平校准值
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[5], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.c3G1FDDInPwr = cTempData; //3G1-FDD输入功率电平值

    //--------- 以下为告警判断 ---------
    if(g_stRcParamSet.c3G1FDDInPwr < g_stSettingParamSet.c3G1FDDInUnderPwrThr) // 3G1-FDD输入功率欠功率门限
    {
        MYALARM.st3G1FDDInUnderPwr.ucRcStatus = ALARM;  //3G1-FDD输入欠功率告警
    }
    else if(g_stRcParamSet.c3G1FDDInPwr > g_stSettingParamSet.c3G1FDDInOverPwrThr) // 3G1-FDD输入功率过功率门限
    {
        MYALARM.st3G1FDDInOverPwr.ucRcStatus = ALARM;  //3G1-FDD输入过功率告警
    }

    //#########################################################################
    //######################### 3G2-FDD输入功率电平值 #########################
    
    MYALARM.st3G2FDDInOverPwr.ucRcStatus = NOT_ALARM;  //3G2-FDD输入过功率告警
    MYALARM.st3G2FDDInUnderPwr.ucRcStatus = NOT_ALARM; //3G2-FDD输入欠功率告警

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.c3G2FDDInPwrAdjust;  //3G2-FDD输入功率电平校准值
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[6], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.c3G2FDDInPwr = cTempData; //3G2-FDD输入功率电平值

    //--------- 以下为告警判断 ---------
    if(g_stRcParamSet.c3G2FDDInPwr < g_stSettingParamSet.c3G2FDDInUnderPwrThr) // 3G2-FDD输入功率欠功率门限
    {
        MYALARM.st3G2FDDInUnderPwr.ucRcStatus = ALARM;  //3G2-FDD输入欠功率告警
    }
    else if(g_stRcParamSet.c3G2FDDInPwr > g_stSettingParamSet.c3G2FDDInOverPwrThr) // 3G2-FDD输入功率过功率门限
    {
        MYALARM.st3G2FDDInOverPwr.ucRcStatus = ALARM;  //3G2-FDD输入过功率告警
    }

    //#########################################################################
    //######################### 3G3-TDD输入功率电平值 #########################
    
    MYALARM.st3G3TDDInOverPwr.ucRcStatus = NOT_ALARM;  //3G3-TDD输入过功率告警
    MYALARM.st3G3TDDInUnderPwr.ucRcStatus = NOT_ALARM; //3G3-TDD输入欠功率告警

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.c3G3TDDInPwrAdjust;  //3G3-TDD输入功率电平校准值
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[7], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.c3G3TDDInPwr = cTempData; //3G3-TDD输入功率电平值

    //--------- 以下为告警判断 ---------
    if(g_stRcParamSet.c3G3TDDInPwr < g_stSettingParamSet.c3G3TDDInUnderPwrThr) // 3G3-TDD输入功率欠功率门限
    {
        MYALARM.st3G3TDDInUnderPwr.ucRcStatus = ALARM;  //3G3-TDD输入欠功率告警
    }
    else if(g_stRcParamSet.c3G3TDDInPwr > g_stSettingParamSet.c3G3TDDInOverPwrThr) // 3G3-TDD输入功率过功率门限
    {
        MYALARM.st3G3TDDInOverPwr.ucRcStatus = ALARM;  //3G3-TDD输入过功率告警
    }

    //#########################################################################
    //######################### 集群系统输入功率电平值 #########################
    
    MYALARM.stTrunkInOverPwr.ucRcStatus = NOT_ALARM;  //集群系统输入过功率告警
    MYALARM.stTrunkInUnderPwr.ucRcStatus = NOT_ALARM; //集群系统输入欠功率告警

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cTrunkInPwrAdjust;  //集群系统输入功率电平校准值
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[8], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cTrunkInPwr = cTempData; //集群系统输入功率电平值

    //--------- 以下为告警判断 ---------
    if(g_stRcParamSet.cTrunkInPwr < g_stSettingParamSet.cTrunkInUnderPwrThr) //集群系统输入功率欠功率门限
    {
        MYALARM.stTrunkInUnderPwr.ucRcStatus = ALARM;  //集群系统输入欠功率告警
    }
    else if(g_stRcParamSet.cTrunkInPwr > g_stSettingParamSet.cTrunkInOverPwrThr) //集群系统输入功率过功率门限
    {
        MYALARM.stTrunkInOverPwr.ucRcStatus = ALARM;  //集群系统输入过功率告警
    }







    //########################################################################
    //########################## 第一路输出功率电平 ##########################
    
    MYALARM.stDlOutOverPwr1.ucRcStatus = NOT_ALARM;  //下行输出过功率告警（通道1） 
    MYALARM.stDlOutUnderPwr1.ucRcStatus = NOT_ALARM; //下行输出欠功率告警（通道1）

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cOutPwrAdjust1;  //第一路输出功率电平校准值
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    //MCM-17_20061106_linyu_begin    
    cTempData = CountPow(ulValueTemp[13], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cDlOutPwr = cTempData; //下行输出功率电平（通道1）

    //--------- 以下为告警判断 ---------
    if(g_stRcParamSet.cDlOutPwr < g_stSettingParamSet.cDlOutUnderPwrThr) //下行输出功率欠功率门限（通道1）
    {
        MYALARM.stDlOutUnderPwr1.ucRcStatus = ALARM;  //下行输出欠功率告警（通道1）
    }
    else if(g_stRcParamSet.cDlOutPwr > g_stSettingParamSet.cDlOutOverPwrThr) //下行输出功率过功率门限（通道1）
    {
        MYALARM.stDlOutOverPwr1.ucRcStatus = ALARM;  //下行输出过功率告警（通道1）
    }
    //MCM-17_20061106_linyu_end    
    //########################################################################
    //########################## 第二路输出功率电平 ##########################
    
    MYALARM.stDlOutOverPwr2.ucRcStatus = NOT_ALARM;  //下行输出过功率告警（通道2） 
    MYALARM.stDlOutUnderPwr2.ucRcStatus = NOT_ALARM; //下行输出欠功率告警（通道2）

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cOutPwrAdjust2;  //第二路输出功率电平校准值
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[10], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cDlOutPwr2 = cTempData; //下行输出功率电平（通道2）

    //--------- 以下为告警判断 ---------
    if(g_stRcParamSet.cDlOutPwr2 < g_stSettingParamSet.cDlOutUnderPwrThr2) //下行输出功率欠功率门限（通道2）
    {
        MYALARM.stDlOutUnderPwr2.ucRcStatus = ALARM;  //下行输出欠功率告警（通道2）
    }
    else if(g_stRcParamSet.cDlOutPwr2 > g_stSettingParamSet.cDlOutOverPwrThr2) //下行输出功率过功率门限（通道2）
    {
        MYALARM.stDlOutOverPwr2.ucRcStatus = ALARM;  //下行输出过功率告警（通道2）
    }

    //########################################################################
    //############################ 下行驻波比值1 #############################
    
    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cReflectPwrAdjust1;   //第一路反射功率电平校准值
    ucTempMaxValue = REFLECT_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    //MCM-17_20061106_linyu_begin    
    g_stRcParamSet.ucDlSwr = CountVSWR(ulValueTemp[11], ucTempMaxValue, cTempAdjust, 
                                         ucTempCoupler, g_stRcParamSet.cDlOutPwr);

    MYALARM.stDlSwr1.ucRcStatus = NOT_ALARM;  //先取消下行驻波比1告警
    
    //--------- 以下为告警判断 ---------
    if(g_stRcParamSet.ucDlSwr > g_stSettingParamSet.ucDlSwrThr) //下行驻波比门限（通道1）
    {
        MYALARM.stDlSwr1.ucRcStatus = ALARM;  //下行驻波比1告警
    }
    //MCM-17_20061106_linyu_end
    //########################################################################
    //############################ 下行驻波比值2 #############################
    
    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cReflectPwrAdjust2;   //第二路反射功率电平校准值
    ucTempMaxValue = REFLECT_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    g_stRcParamSet.ucDlSwr2 = CountVSWR(ulValueTemp[12], ucTempMaxValue, cTempAdjust, 
                                         ucTempCoupler, g_stRcParamSet.cDlOutPwr2);

    MYALARM.stDlSwr2.ucRcStatus = NOT_ALARM;  //先取消下行驻波比2告警
    
    //--------- 以下为告警判断 ---------
    if(g_stRcParamSet.ucDlSwr2 > g_stSettingParamSet.ucDlSwrThr2) //下行驻波比门限（通道2）
    {
        MYALARM.stDlSwr2.ucRcStatus = ALARM;  //下行驻波比2告警
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //%%%%%%%%%%%%%%%%%%%%%%% 电源故障告警判断 %%%%%%%%%%%%%%%%%%%%%%%
    // uiValueTemp[9]  12V输出  低于9V为故障
    // uiValueTemp[7]  27V输出  低于20V为故障
    if(ulValueTemp[9] < 153)// || (ulValueTemp[14] < 159))
    {
        MYALARM.stPwrFault.ucRcStatus = ALARM; //电源故障告警
    }
    else
    {
        MYALARM.stPwrFault.ucRcStatus = NOT_ALARM; //电源故障告警
    }

}
