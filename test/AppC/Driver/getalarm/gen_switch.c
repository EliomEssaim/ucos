//======================================================================
//���ܣ�	      ��ɿ������ɼ������ֵ
//������        ����������ƶ�3G��Ŀ
//����дʱ�䣺2006��7��19��
//======================================================================
void DisposeResult(UCHAR* aucWAlarmValue)
{
    //------------ �������ݲɼ���ϣ����濪ʼ��д����  -------------
    //-------------------------------------------
    if((aucWAlarmValue[1] & 0x08) != 0)
    {
        if(MYALARM.stPwrLost.ucRcStatus == ALARM) //��Դ����ָ�ʱ��Ҫ��������Ƶ���˥��ֵ
        {
            ucSetFreq = TRUE; //��Ҫ����Ƶ�ʱ�־
            ucSetAtt = TRUE;  //��Ҫ����˥�ı�־
        }
        MYALARM.stPwrLost.ucRcStatus = NOT_ALARM; //��Դ����澯���͸澯
    }else
    {
        MYALARM.stPwrLost.ucRcStatus = ALARM; //��Դ����澯���͸澯
    }
    /************ ��Ϊģ�������ж� **************
    if((aucWAlarmValue[1] & 0x01) == 0)
    {
        MYALARM.stPwrFault.ucRcStatus = ALARM; //��Դ���ϸ澯���͸澯
    }else
    {
        MYALARM.stPwrFault.ucRcStatus = NOT_ALARM; //��Դ���ϸ澯���͸澯
    }
    //*******************************************/
    if((aucWAlarmValue[1] & 0x10) != 0)
    {
        MYALARM.stDoor.ucRcStatus = ALARM; //�Ž��澯���߸澯
    }else
    {
        MYALARM.stDoor.ucRcStatus = NOT_ALARM; //�Ž��澯���߸澯
    }
    /************ ��Ϊģ�������ж� **************
    if((aucWAlarmValue[1] & 0x08) == 0)
    {
        MYALARM.stBatFalut.ucRcStatus = ALARM; //���ģ���ع��ϸ澯���͸澯
    }else
    {
        MYALARM.stBatFalut.ucRcStatus = NOT_ALARM; //���ģ���ع��ϸ澯���͸澯
    }
    //*******************************************/
    if((aucWAlarmValue[1] & 0x04) != 0)
    {
        MYALARM.stPosition.ucRcStatus = ALARM; //λ�ø澯���߸澯
    }else
    {
        MYALARM.stPosition.ucRcStatus = NOT_ALARM; //λ�ø澯���߸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[1] & 0x60) != 0)
    {
        MYALARM.stOptModule.ucRcStatus = ALARM; //���շ�ģ����ϸ澯���߸澯
    }else
    {
        MYALARM.stOptModule.ucRcStatus = NOT_ALARM; //���շ�ģ����ϸ澯���߸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[0] & 0x20) == 0)
    {
        MYALARM.stUlLna.ucRcStatus = ALARM;	//���е���Ź��ϸ澯���͸澯
    }else
    {
        MYALARM.stUlLna.ucRcStatus = NOT_ALARM; //���е���Ź��ϸ澯���͸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[0] & 0x40) == 0)
    {
        MYALARM.stDlLna.ucRcStatus = ALARM;	//���е���Ź��ϸ澯���͸澯
    }else
    {
        MYALARM.stDlLna.ucRcStatus = NOT_ALARM; //���е���Ź��ϸ澯���͸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[0] & 0x01) == 0)
    {
        MYALARM.stUlPa.ucRcStatus = ALARM;	//���й��Ÿ澯���͸澯
    }else
    {
        MYALARM.stUlPa.ucRcStatus = NOT_ALARM; //���й��Ÿ澯���͸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[0] & 0x02) != 0)
    {
        MYALARM.stUlSelfOsc.ucRcStatus = ALARM; //�����Լ��澯���߸澯
        MYALARM.stSelfOsc.ucRcStatus = ALARM; //�Լ��澯���߸澯
    }else
    {
        MYALARM.stUlSelfOsc.ucRcStatus = NOT_ALARM; //�����Լ��澯���߸澯
        MYALARM.stSelfOsc.ucRcStatus = NOT_ALARM; //�Լ��澯���߸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[0] & 0x04) == 0)
    {
        MYALARM.stDlPa.ucRcStatus = ALARM;	//���й��Ÿ澯���͸澯
    }else
    {
        MYALARM.stDlPa.ucRcStatus = NOT_ALARM; //���й��Ÿ澯���͸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[0] & 0x08) != 0)
    {
        MYALARM.stDlSelfOsc.ucRcStatus = ALARM; //�����Լ��澯���߸澯
    }else
    {
        MYALARM.stDlSelfOsc.ucRcStatus = NOT_ALARM; //�����Լ��澯���߸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x08) != 0)
    {
        MYALARM.stExt4.ucRcStatus = ALARM; //�ⲿ�澯4���߸澯
    }else
    {
        MYALARM.stExt4.ucRcStatus = NOT_ALARM; //�ⲿ�澯4���߸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x04) != 0)
    {
        MYALARM.stExt3.ucRcStatus = ALARM; //�ⲿ�澯3
    }else
    {
        MYALARM.stExt3.ucRcStatus = NOT_ALARM; //�ⲿ�澯3���߸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x02) != 0)
    {
        MYALARM.stExt2.ucRcStatus = ALARM; //�ⲿ�澯2���߸澯
    }else
    {
        MYALARM.stExt2.ucRcStatus = NOT_ALARM; //�ⲿ�澯2���߸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x01) != 0)
    {
        MYALARM.stExt1.ucRcStatus = ALARM; //�ⲿ�澯1���߸澯
    }else
    {
        MYALARM.stExt1.ucRcStatus = NOT_ALARM; //�ⲿ�澯1���߸澯
    }
        //-------------------------------------------
    if((aucWAlarmValue[4] & 0x80) != 0)
    {
        MYALARM.stExt8.ucRcStatus = ALARM; //�ⲿ�澯8���߸澯
    }else
    {
        MYALARM.stExt8.ucRcStatus = NOT_ALARM; //�ⲿ�澯8���߸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x40) != 0)
    {
        MYALARM.stExt7.ucRcStatus = ALARM; //�ⲿ�澯7
    }else
    {
        MYALARM.stExt7.ucRcStatus = NOT_ALARM; //�ⲿ�澯7���߸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x20) != 0)
    {
        MYALARM.stExt6.ucRcStatus = ALARM; //�ⲿ�澯6���߸澯
    }else
    {
        MYALARM.stExt6.ucRcStatus = NOT_ALARM; //�ⲿ�澯6���߸澯
    }
    //-------------------------------------------
    if((aucWAlarmValue[4] & 0x10) != 0)
    {
        MYALARM.stExt5.ucRcStatus = ALARM; //�ⲿ�澯5���߸澯
    }else
    {
        MYALARM.stExt5.ucRcStatus = NOT_ALARM; //�ⲿ�澯5���߸澯
    }
    //-------------------------------------------
    aucWAlarmValue[2] &= aucWAlarmValue[3];
    aucWAlarmValue[2] &= 0xff;
    if (aucWAlarmValue[2] != 0xff)
    {
        MYALARM.stOscUnlocked.ucRcStatus = ALARM; //����ʧ���澯���͸澯
        ucSetFreq = TRUE; //��Ҫ����Ƶ�ʱ�־
    }
    else
    {
        MYALARM.stOscUnlocked.ucRcStatus = NOT_ALARM; //����ʧ���澯���͸澯
    }
    //-------------------------------------------
    //USHORT usCommAlarm;//����ͨ�Ÿ澯��ʶ��16λ��ʾ16���ӻ���1:�澯��0:����
    if(g_stYkppParamSet.stYkppCommParam.usCommAlarm != 0)
    {
        MYALARM.stMsLink.ucRcStatus = ALARM; //���Ӽ����·�澯
    }
    else
    {
        MYALARM.stMsLink.ucRcStatus = NOT_ALARM; //���Ӽ����·�澯ȡ��
    }

    ////////////// ������ɸ��������澯�жϣ������ж��Ƿ���Ҫ����˥�ĺ�Ƶ�� //////////////
    if(ucSetFreq == TRUE) //��Ҫ����Ƶ�ʱ�־
    {
        SetFreq(); //����Ƶ��
        ucSetFreq = FALSE;
    }
    if(ucSetAtt == TRUE) //��Ҫ����˥�ı�־
    {
        SetAtt(); //����˥��
        ucSetAtt = FALSE;
    }

}


