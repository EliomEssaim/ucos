//======================================================================
//功能：	      完成开关量采集结果赋值
//描述：        本函数仅针对POI项目
//最后编写时间：2006年7月19日
//======================================================================
void DisposeResult(UCHAR* aucWAlarmValue)
{
    //-------------------------------------------
    if((aucWAlarmValue[1] & 0x08) != 0)
    {
        MYALARM.stPwrLost.ucRcStatus = NOT_ALARM; //电源掉电告警，低告警
    }else
    {
        MYALARM.stPwrLost.ucRcStatus = ALARM; //电源掉电告警，低告警
    }
    //*******************************************/
    if((aucWAlarmValue[1] & 0x10) != 0)
    {
        MYALARM.stDoor.ucRcStatus = ALARM; //门禁告警，高告警
    }else
    {
        MYALARM.stDoor.ucRcStatus = NOT_ALARM; //门禁告警，高告警
    }
}

