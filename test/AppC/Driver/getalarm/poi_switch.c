//======================================================================
//���ܣ�	      ��ɿ������ɼ������ֵ
//������        �����������POI��Ŀ
//����дʱ�䣺2006��7��19��
//======================================================================
void DisposeResult(UCHAR* aucWAlarmValue)
{
    //-------------------------------------------
    if((aucWAlarmValue[1] & 0x08) != 0)
    {
        MYALARM.stPwrLost.ucRcStatus = NOT_ALARM; //��Դ����澯���͸澯
    }else
    {
        MYALARM.stPwrLost.ucRcStatus = ALARM; //��Դ����澯���͸澯
    }
    //*******************************************/
    if((aucWAlarmValue[1] & 0x10) != 0)
    {
        MYALARM.stDoor.ucRcStatus = ALARM; //�Ž��澯���߸澯
    }else
    {
        MYALARM.stDoor.ucRcStatus = NOT_ALARM; //�Ž��澯���߸澯
    }
}

