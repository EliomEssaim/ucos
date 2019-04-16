//======================================================================
//功能：	      完成开关量采集结果赋值
//描述：        本函数针对移动3G项目
//最后编写时间：2006年7月19日
//======================================================================
void DisposeResult(UCHAR* aucWAlarmValue)
{
    //------------ 以上数据采集完毕，下面开始填写数据  -------------
    //-------------------------------------------
    if((aucWAlarmValue[1] & 0x08) != 0)
    {
        if(MYALARM.stPwrLost.ucRcStatus == ALARM) //电源掉电恢复时需要重新设置频点和衰耗值
        {
            ucSetFreq = TRUE; //需要设置频率标志
            ucSetAtt = TRUE;  //需要设置衰耗标志
        }
        MYALARM.stPwrLost.ucRcStatus = NOT_ALARM; //电源掉电告警，低告警
    }else
    {
        MYALARM.stPwrLost.ucRcStatus = ALARM; //电源掉电告警，低告警
    }
    /************ 改为模拟量来判断 **************
    if((aucWAlarmValue[1] & 0x01) == 0)
    {
        MYALARM.stPwrFault.ucRcStatus = ALARM; //电源故障告警，低告警
    }else
    {
        MYALARM.stPwrFault.ucRcStatus = NOT_ALARM; //电源故障告警，低告警
    }
    //*******************************************/
    if((aucWAlarmValue[1] & 0x10) != 0)
    {
        MYALARM.stDoor.ucRcStatus = ALARM; //门禁告警，高告警
    }else
    {
        MYALARM.stDoor.ucRcStatus = NOT_ALARM; //门禁告警，高告警
    }
    /************ 改为模拟量来判断 **************
    if((aucWAlarmValue[1] & 0x08) == 0)
    {
        MYALARM.stBatFalut.ucRcStatus = ALARM; //监控模块电池故障告警，低告警
    }else
    {
        MYALARM.stBatFalut.ucRcStatus = NOT_ALARM; //监控模块电池故障告警，低告警
    }
    //*******************************************/
    if((aucWAlarmValue[1] & 0x04) != 0)
    {
        MYALARM.stPosition.ucRcStatus = ALARM; //位置告警，高告警
    }else
    {
        MYALARM.stPosition.ucRcStatus = NOT_ALARM; //位置告警，高告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[1] & 0x60) != 0)
    {
        MYALARM.stOptModule.ucRcStatus = ALARM; //光收发模块故障告警，高告警
    }else
    {
        MYALARM.stOptModule.ucRcStatus = NOT_ALARM; //光收发模块故障告警，高告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[0] & 0x20) == 0)
    {
        MYALARM.stUlLna.ucRcStatus = ALARM;	//上行低噪放故障告警，低告警
    }else
    {
        MYALARM.stUlLna.ucRcStatus = NOT_ALARM; //上行低噪放故障告警，低告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[0] & 0x40) == 0)
    {
        MYALARM.stDlLna.ucRcStatus = ALARM;	//下行低噪放故障告警，低告警
    }else
    {
        MYALARM.stDlLna.ucRcStatus = NOT_ALARM; //下行低噪放故障告警，低告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[0] & 0x01) == 0)
    {
        MYALARM.stUlPa.ucRcStatus = ALARM;	//上行功放告警，低告警
    }else
    {
        MYALARM.stUlPa.ucRcStatus = NOT_ALARM; //上行功放告警，低告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[0] & 0x02) != 0)
    {
        MYALARM.stUlSelfOsc.ucRcStatus = ALARM; //上行自激告警，高告警
        MYALARM.stSelfOsc.ucRcStatus = ALARM; //自激告警，高告警
    }else
    {
        MYALARM.stUlSelfOsc.ucRcStatus = NOT_ALARM; //上行自激告警，高告警
        MYALARM.stSelfOsc.ucRcStatus = NOT_ALARM; //自激告警，高告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[0] & 0x04) == 0)
    {
        MYALARM.stDlPa.ucRcStatus = ALARM;	//下行功放告警，低告警
    }else
    {
        MYALARM.stDlPa.ucRcStatus = NOT_ALARM; //下行功放告警，低告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[0] & 0x08) != 0)
    {
        MYALARM.stDlSelfOsc.ucRcStatus = ALARM; //下行自激告警，高告警
    }else
    {
        MYALARM.stDlSelfOsc.ucRcStatus = NOT_ALARM; //下行自激告警，高告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x08) != 0)
    {
        MYALARM.stExt4.ucRcStatus = ALARM; //外部告警4，高告警
    }else
    {
        MYALARM.stExt4.ucRcStatus = NOT_ALARM; //外部告警4，高告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x04) != 0)
    {
        MYALARM.stExt3.ucRcStatus = ALARM; //外部告警3
    }else
    {
        MYALARM.stExt3.ucRcStatus = NOT_ALARM; //外部告警3，高告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x02) != 0)
    {
        MYALARM.stExt2.ucRcStatus = ALARM; //外部告警2，高告警
    }else
    {
        MYALARM.stExt2.ucRcStatus = NOT_ALARM; //外部告警2，高告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x01) != 0)
    {
        MYALARM.stExt1.ucRcStatus = ALARM; //外部告警1，高告警
    }else
    {
        MYALARM.stExt1.ucRcStatus = NOT_ALARM; //外部告警1，高告警
    }
        //-------------------------------------------
    if((aucWAlarmValue[4] & 0x80) != 0)
    {
        MYALARM.stExt8.ucRcStatus = ALARM; //外部告警8，高告警
    }else
    {
        MYALARM.stExt8.ucRcStatus = NOT_ALARM; //外部告警8，高告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x40) != 0)
    {
        MYALARM.stExt7.ucRcStatus = ALARM; //外部告警7
    }else
    {
        MYALARM.stExt7.ucRcStatus = NOT_ALARM; //外部告警7，高告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x20) != 0)
    {
        MYALARM.stExt6.ucRcStatus = ALARM; //外部告警6，高告警
    }else
    {
        MYALARM.stExt6.ucRcStatus = NOT_ALARM; //外部告警6，高告警
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x10) != 0)
    {
        MYALARM.stExt5.ucRcStatus = ALARM; //外部告警5，高告警
    }else
    {
        MYALARM.stExt5.ucRcStatus = NOT_ALARM; //外部告警5，高告警
    }
    //-------------------------------------------
    aucWAlarmValue[2] &= aucWAlarmValue[3];
    aucWAlarmValue[2] &= 0xff;
    if (aucWAlarmValue[2] != 0xff)
    {
        MYALARM.stOscUnlocked.ucRcStatus = ALARM; //本振失锁告警，低告警
        ucSetFreq = TRUE; //需要设置频率标志
    }
    else
    {
        MYALARM.stOscUnlocked.ucRcStatus = NOT_ALARM; //本振失锁告警，低告警
    }
    //-------------------------------------------
    //USHORT usCommAlarm;//主从通信告警标识，16位表示16个从机，1:告警，0:正常
    if(g_stYkppParamSet.stYkppCommParam.usCommAlarm != 0)
    {
        MYALARM.stMsLink.ucRcStatus = ALARM; //主从监控链路告警
    }
    else
    {
        MYALARM.stMsLink.ucRcStatus = NOT_ALARM; //主从监控链路告警取消
    }

    ////////////// 以上完成各开关量告警判断，以下判断是否需要重设衰耗和频点 //////////////
    if(ucSetFreq == TRUE) //需要设置频率标志
    {
        SetFreq(); //重设频点
        ucSetFreq = FALSE;
    }
    if(ucSetAtt == TRUE) //需要设置衰耗标志
    {
        SetAtt(); //重设衰耗
        ucSetAtt = FALSE;
    }

}


