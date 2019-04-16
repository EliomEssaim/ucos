/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   Poi_conversion.cpp
    ����:     ����
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  
       ����:  
       ����:  �����д���
    2. ����:  2006/11/06
       ����:  ����
       ����:  �޸�����MCM-17�������д�ͨ���ŵ�ͨ��1�Ĳ�����ӳ���Ӧ��
              ������ͨ���ŵĲ����ϣ������豸�Ƕ�ͨ��ʱ����ѯͨ��1�Ĳ�
              ��ʵ�ʷ��ʵ��Ƕ�Ӧ�Ĳ���ͨ���ŵĲ����ĵ�ַ��
    3. ����:  2006/11/06
       ����:  ����
       ����:  �޸�GENNERAL����
---------------------------------------------------------------------------*/
/*************************************************
  Copyright (C), 2006-2007, Lin Wei
  �ļ�����    poi_conversion.c
  ����:       ����
  �汾��      1.0.0
  ���ڣ�      2006/07/19 
  ����������  ���ļ�ʵ�ֽ�A/Dģ�����ɼ�ֵ���ݲ�ͬ���㹫ʽ���õ���Ӧ���
  ��ע��      ���ļ�������ʿ�POI��Ŀ
  �����б�
        void PutIntoRam(UINT32* uiValueTemp);
*************************************************/

#define GENERAL_COUPLER  30  //ͨ�õ������������ֵ
#define GENERAL_MAXVALUE 20  //ͨ�õĹ��ʵ�ƽ������ܼ�⵽������ʵ�ƽֵ
#define REFLECT_MAXVALUE 10  //�����źż��ʱ�ܼ�⵽������ʵ�ƽֵ

//**********************************************************
//���ܣ����ɼ�ֵͨ������ó����ʵ�ƽֵ(dBm),�����и澯״̬�ж�
//���룺lPickValue -- �ɼ����
//      lMaxValue -- ���ʵ�ƽ���ֵ
//      sAdjustValue -- У׼ֵ
//      ucCoupler -- ���������
//���أ����ʵ�ƽֵ
//**********************************************************
CHAR CountPow(LONG lPickValue, LONG lMaxValue, SHORT sAdjustValue, UCHAR ucCoupler)
{
    LONG lTempPowOut;
    //N=MAX-(45-x*25/512)+Adjust
    lTempPowOut = lMaxValue - (45 - (lPickValue * 25 + 256) / 512) + (LONG)sAdjustValue;

    if(lTempPowOut > lMaxValue)
    {
        return (CHAR)(lMaxValue + ucCoupler);  //���ڼ�ⷶΧʱ�ķ���ֵ
    }
    else if(lTempPowOut < (lMaxValue - 32))
    {
        return 0;  //���ڼ�ⷶΧʱ�ķ���ֵ
    }
    else
    {
        return (CHAR)(lTempPowOut + ucCoupler);
    }
}

//**********************************************************
//���ܣ� ��פ���Ȳ�����ƽֵ�����ݹ�ʽ�������Ӧ��פ����ֵ
//���룺 lPickValue -- ���书�ʲɼ����
//       lMaxValue -- ���ʵ�ƽ���ֵ
//       sAdjustValue -- ���书�ʵ�ƽУ׼ֵ
//       ucCoupler -- ���������
//       cDownOut -- ��Ӧ�������������ֵ
//���أ� פ���ȣ�10����
//**********************************************************
UCHAR CountVSWR(LONG lPickValue, LONG lMaxValue, SHORT sAdjustValue, UCHAR ucCoupler, CHAR cDownOut)
{
    CHAR cReflectPower;

    cReflectPower = CountPow(lPickValue, lMaxValue, sAdjustValue, ucCoupler);
    cReflectPower = cDownOut - cReflectPower;   //��������ʼ�ȥ���书�ʡ�
    
    if(cReflectPower >= 17)
    {
        return 12;   //����17ʱ��פ����ֵΪ1.2
    }
    else if(cReflectPower >= 16)
    {
        return 13;   //����16ʱ��פ����ֵΪ1.3
    }
    else if(cReflectPower >= 15)
    {
        return 14;   //����15��16ʱ��פ����ֵΪ1.4
    }
    else if(cReflectPower >= 13)
    {
        return 15;   //����13��14ʱ��פ����ֵΪ1.5
    }
    else if(cReflectPower >= 12)
    {
        return 16;   //����12ʱ��פ����ֵΪ1.6
    }
    else if(cReflectPower >= 11)
    {
        return 17;   //����11ʱ��פ����ֵΪ1.7
    }
    else if(cReflectPower >= 10)
    {
        return 18;   //����10ʱ��פ����ֵΪ1.8
    }
    else if (cReflectPower >= 8)
    {
        return 20;   //����8��9ʱ��פ����ֵΪ2.0
    }
    else if (cReflectPower >= 7)
    {
        return 23;   // 7ʱ��פ����ֵΪ2.3
    }
    else if (cReflectPower >= 6)
    {
        return 25;   // 6ʱ��פ����ֵΪ2.5
    }
    else if (cReflectPower >= 4)
    {
        return 30;   // 4~5ʱ��פ����ֵΪ3.0
    }
    else return 40;     //С��4ʱ��פ����ֵΪ��ʾΪ4.0
}

//**********************************************************
//���ܣ� 
//���룺 uiValueTemp  -- 
//���أ� 
//**********************************************************
void PutIntoRam(ULONG* ulValueTemp)
{
    CHAR cTempAdjust;   //У׼ֵ��ʱ���
    UCHAR ucTempMaxValue; //����ʵ�ƽ��ʱ���
    UCHAR ucTempCoupler;  //�����������ʱ���
    CHAR cTempData;

    //#########################################################################
    //########################## CDMA800���빦�ʵ�ƽ ##########################
    
    MYALARM.stCdma800InOverPwr.ucRcStatus = NOT_ALARM;  //CDMA800��������ʸ澯
    MYALARM.stCdma800InUnderPwr.ucRcStatus = NOT_ALARM; //CDMA800����Ƿ���ʸ澯

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cCdma800InPwrAdjust;  // CDMA800���빦�ʵ�ƽУ׼ֵ
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[0], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cCdma800InPwr = cTempData; //CDMA800���빦�ʵ�ƽֵ

    //--------- ����Ϊ�澯�ж� ---------
    if(g_stRcParamSet.cCdma800InPwr < g_stSettingParamSet.cCdma800InUnderPwrThr) //CDMA800���빦��Ƿ��������
    {
        MYALARM.stCdma800InUnderPwr.ucRcStatus = ALARM;  //CDMA800����Ƿ���ʸ澯
    }
    else if(g_stRcParamSet.cCdma800InPwr > g_stSettingParamSet.cCdma800InOverPwrThr) //CDMA800���빦�ʹ���������
    {
        MYALARM.stCdma800InOverPwr.ucRcStatus = ALARM;  //CDMA800��������ʸ澯
    }

    //#########################################################################
    //######################### �ƶ�GSM���빦�ʵ�ƽֵ #########################
    
    MYALARM.stMbGsmInOverPwr.ucRcStatus = NOT_ALARM;  //�ƶ�GSM��������ʸ澯
    MYALARM.stMbGsmInUnderPwr.ucRcStatus = NOT_ALARM; //�ƶ�GSM����Ƿ���ʸ澯

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cMbGsmInPwrAdjust;  //�ƶ�GSM���빦�ʵ�ƽУ׼ֵ
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[1], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cMbGsmInPwr = cTempData; //�ƶ�GSM���빦�ʵ�ƽֵ

    //--------- ����Ϊ�澯�ж� ---------
    if(g_stRcParamSet.cMbGsmInPwr < g_stSettingParamSet.cMbGsmInUnderPwrThr) //�ƶ�GSM���빦��Ƿ��������
    {
        MYALARM.stMbGsmInUnderPwr.ucRcStatus = ALARM;  //�ƶ�GSM����Ƿ���ʸ澯
    }
    else if(g_stRcParamSet.cMbGsmInPwr > g_stSettingParamSet.cMbGsmInOverPwrThr) //�ƶ�GSM���빦�ʹ���������
    {
        MYALARM.stMbGsmInOverPwr.ucRcStatus = ALARM;  //�ƶ�GSM��������ʸ澯
    }

    //#########################################################################
    //######################### ��ͨGSM���빦�ʵ�ƽֵ #########################
    
    MYALARM.stUcGsmInOverPwr.ucRcStatus = NOT_ALARM;  //��ͨGSM��������ʸ澯
    MYALARM.stUcGsmInUnderPwr.ucRcStatus = NOT_ALARM; //��ͨGSM����Ƿ���ʸ澯

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cUcGsmInPwrAdjust;  //��ͨGSM���빦�ʵ�ƽУ׼ֵ
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[2], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cUcGsmInPwr = cTempData; //�ƶ�GSM���빦�ʵ�ƽֵ

    //--------- ����Ϊ�澯�ж� ---------
    if(g_stRcParamSet.cUcGsmInPwr < g_stSettingParamSet.cUcGsmInUnderPwrThr) //��ͨGSM���빦��Ƿ��������
    {
        MYALARM.stUcGsmInUnderPwr.ucRcStatus = ALARM;  //��ͨGSM����Ƿ���ʸ澯
    }
    else if(g_stRcParamSet.cUcGsmInPwr > g_stSettingParamSet.cUcGsmInOverPwrThr) //��ͨGSM���빦�ʹ���������
    {
        MYALARM.stUcGsmInOverPwr.ucRcStatus = ALARM;  //��ͨGSM��������ʸ澯
    }

    //#########################################################################
    //######################### �ƶ�DCS���빦�ʵ�ƽֵ #########################
    
    MYALARM.stMbDcsInOverPwr.ucRcStatus = NOT_ALARM;  //�ƶ�DCS��������ʸ澯
    MYALARM.stMbDcsInUnderPwr.ucRcStatus = NOT_ALARM; //�ƶ�DCS����Ƿ���ʸ澯

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cMbDcsInPwrAdjust;  //�ƶ�DCS���빦�ʵ�ƽУ׼ֵ
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[3], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cMbDcsInPwr = cTempData; //�ƶ�GSM���빦�ʵ�ƽֵ

    //--------- ����Ϊ�澯�ж� ---------
    if(g_stRcParamSet.cMbDcsInPwr < g_stSettingParamSet.cMbDcsInUnderPwrThr) //�ƶ�DCS���빦��Ƿ��������
    {
        MYALARM.stMbDcsInUnderPwr.ucRcStatus = ALARM;  //�ƶ�DCS����Ƿ���ʸ澯
    }
    else if(g_stRcParamSet.cMbDcsInPwr > g_stSettingParamSet.cMbDcsInOverPwrThr) //�ƶ�DCS���빦�ʹ���������
    {
        MYALARM.stMbDcsInOverPwr.ucRcStatus = ALARM;  //�ƶ�DCS��������ʸ澯
    }

    //#########################################################################
    //######################### ��ͨDCS���빦�ʵ�ƽֵ #########################
    
    MYALARM.stUcDcsInOverPwr.ucRcStatus = NOT_ALARM;  //��ͨDCS��������ʸ澯
    MYALARM.stUcDcsInUnderPwr.ucRcStatus = NOT_ALARM; //��ͨDCS����Ƿ���ʸ澯

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cUcDcsInPwrAdjust;  //��ͨDCS���빦�ʵ�ƽУ׼ֵ
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[4], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cUcDcsInPwr = cTempData; //��ͨDCS���빦�ʵ�ƽֵ

    //--------- ����Ϊ�澯�ж� ---------
    if(g_stRcParamSet.cUcDcsInPwr < g_stSettingParamSet.cUcDcsInUnderPwrThr) //��ͨDCS���빦��Ƿ��������
    {
        MYALARM.stUcDcsInUnderPwr.ucRcStatus = ALARM;  //��ͨDCS����Ƿ���ʸ澯
    }
    else if(g_stRcParamSet.cUcDcsInPwr > g_stSettingParamSet.cUcDcsInOverPwrThr) //��ͨDCS���빦�ʹ���������
    {
        MYALARM.stUcDcsInOverPwr.ucRcStatus = ALARM;  //��ͨDCS��������ʸ澯
    }

    //#########################################################################
    //######################### 3G1-FDD���빦�ʵ�ƽֵ #########################
    
    MYALARM.st3G1FDDInOverPwr.ucRcStatus = NOT_ALARM;  //3G1-FDD��������ʸ澯
    MYALARM.st3G1FDDInUnderPwr.ucRcStatus = NOT_ALARM; //3G1-FDD����Ƿ���ʸ澯

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.c3G1FDDInPwrAdjust;  //3G1-FDD���빦�ʵ�ƽУ׼ֵ
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[5], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.c3G1FDDInPwr = cTempData; //3G1-FDD���빦�ʵ�ƽֵ

    //--------- ����Ϊ�澯�ж� ---------
    if(g_stRcParamSet.c3G1FDDInPwr < g_stSettingParamSet.c3G1FDDInUnderPwrThr) // 3G1-FDD���빦��Ƿ��������
    {
        MYALARM.st3G1FDDInUnderPwr.ucRcStatus = ALARM;  //3G1-FDD����Ƿ���ʸ澯
    }
    else if(g_stRcParamSet.c3G1FDDInPwr > g_stSettingParamSet.c3G1FDDInOverPwrThr) // 3G1-FDD���빦�ʹ���������
    {
        MYALARM.st3G1FDDInOverPwr.ucRcStatus = ALARM;  //3G1-FDD��������ʸ澯
    }

    //#########################################################################
    //######################### 3G2-FDD���빦�ʵ�ƽֵ #########################
    
    MYALARM.st3G2FDDInOverPwr.ucRcStatus = NOT_ALARM;  //3G2-FDD��������ʸ澯
    MYALARM.st3G2FDDInUnderPwr.ucRcStatus = NOT_ALARM; //3G2-FDD����Ƿ���ʸ澯

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.c3G2FDDInPwrAdjust;  //3G2-FDD���빦�ʵ�ƽУ׼ֵ
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[6], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.c3G2FDDInPwr = cTempData; //3G2-FDD���빦�ʵ�ƽֵ

    //--------- ����Ϊ�澯�ж� ---------
    if(g_stRcParamSet.c3G2FDDInPwr < g_stSettingParamSet.c3G2FDDInUnderPwrThr) // 3G2-FDD���빦��Ƿ��������
    {
        MYALARM.st3G2FDDInUnderPwr.ucRcStatus = ALARM;  //3G2-FDD����Ƿ���ʸ澯
    }
    else if(g_stRcParamSet.c3G2FDDInPwr > g_stSettingParamSet.c3G2FDDInOverPwrThr) // 3G2-FDD���빦�ʹ���������
    {
        MYALARM.st3G2FDDInOverPwr.ucRcStatus = ALARM;  //3G2-FDD��������ʸ澯
    }

    //#########################################################################
    //######################### 3G3-TDD���빦�ʵ�ƽֵ #########################
    
    MYALARM.st3G3TDDInOverPwr.ucRcStatus = NOT_ALARM;  //3G3-TDD��������ʸ澯
    MYALARM.st3G3TDDInUnderPwr.ucRcStatus = NOT_ALARM; //3G3-TDD����Ƿ���ʸ澯

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.c3G3TDDInPwrAdjust;  //3G3-TDD���빦�ʵ�ƽУ׼ֵ
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[7], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.c3G3TDDInPwr = cTempData; //3G3-TDD���빦�ʵ�ƽֵ

    //--------- ����Ϊ�澯�ж� ---------
    if(g_stRcParamSet.c3G3TDDInPwr < g_stSettingParamSet.c3G3TDDInUnderPwrThr) // 3G3-TDD���빦��Ƿ��������
    {
        MYALARM.st3G3TDDInUnderPwr.ucRcStatus = ALARM;  //3G3-TDD����Ƿ���ʸ澯
    }
    else if(g_stRcParamSet.c3G3TDDInPwr > g_stSettingParamSet.c3G3TDDInOverPwrThr) // 3G3-TDD���빦�ʹ���������
    {
        MYALARM.st3G3TDDInOverPwr.ucRcStatus = ALARM;  //3G3-TDD��������ʸ澯
    }

    //#########################################################################
    //######################### ��Ⱥϵͳ���빦�ʵ�ƽֵ #########################
    
    MYALARM.stTrunkInOverPwr.ucRcStatus = NOT_ALARM;  //��Ⱥϵͳ��������ʸ澯
    MYALARM.stTrunkInUnderPwr.ucRcStatus = NOT_ALARM; //��Ⱥϵͳ����Ƿ���ʸ澯

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cTrunkInPwrAdjust;  //��Ⱥϵͳ���빦�ʵ�ƽУ׼ֵ
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[8], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cTrunkInPwr = cTempData; //��Ⱥϵͳ���빦�ʵ�ƽֵ

    //--------- ����Ϊ�澯�ж� ---------
    if(g_stRcParamSet.cTrunkInPwr < g_stSettingParamSet.cTrunkInUnderPwrThr) //��Ⱥϵͳ���빦��Ƿ��������
    {
        MYALARM.stTrunkInUnderPwr.ucRcStatus = ALARM;  //��Ⱥϵͳ����Ƿ���ʸ澯
    }
    else if(g_stRcParamSet.cTrunkInPwr > g_stSettingParamSet.cTrunkInOverPwrThr) //��Ⱥϵͳ���빦�ʹ���������
    {
        MYALARM.stTrunkInOverPwr.ucRcStatus = ALARM;  //��Ⱥϵͳ��������ʸ澯
    }







    //########################################################################
    //########################## ��һ·������ʵ�ƽ ##########################
    
    MYALARM.stDlOutOverPwr1.ucRcStatus = NOT_ALARM;  //������������ʸ澯��ͨ��1�� 
    MYALARM.stDlOutUnderPwr1.ucRcStatus = NOT_ALARM; //�������Ƿ���ʸ澯��ͨ��1��

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cOutPwrAdjust1;  //��һ·������ʵ�ƽУ׼ֵ
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    //MCM-17_20061106_linyu_begin    
    cTempData = CountPow(ulValueTemp[13], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cDlOutPwr = cTempData; //����������ʵ�ƽ��ͨ��1��

    //--------- ����Ϊ�澯�ж� ---------
    if(g_stRcParamSet.cDlOutPwr < g_stSettingParamSet.cDlOutUnderPwrThr) //�����������Ƿ�������ޣ�ͨ��1��
    {
        MYALARM.stDlOutUnderPwr1.ucRcStatus = ALARM;  //�������Ƿ���ʸ澯��ͨ��1��
    }
    else if(g_stRcParamSet.cDlOutPwr > g_stSettingParamSet.cDlOutOverPwrThr) //����������ʹ��������ޣ�ͨ��1��
    {
        MYALARM.stDlOutOverPwr1.ucRcStatus = ALARM;  //������������ʸ澯��ͨ��1��
    }
    //MCM-17_20061106_linyu_end    
    //########################################################################
    //########################## �ڶ�·������ʵ�ƽ ##########################
    
    MYALARM.stDlOutOverPwr2.ucRcStatus = NOT_ALARM;  //������������ʸ澯��ͨ��2�� 
    MYALARM.stDlOutUnderPwr2.ucRcStatus = NOT_ALARM; //�������Ƿ���ʸ澯��ͨ��2��

    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cOutPwrAdjust2;  //�ڶ�·������ʵ�ƽУ׼ֵ
    ucTempMaxValue = GENERAL_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    cTempData = CountPow(ulValueTemp[10], ucTempMaxValue, cTempAdjust, ucTempCoupler);
    g_stRcParamSet.cDlOutPwr2 = cTempData; //����������ʵ�ƽ��ͨ��2��

    //--------- ����Ϊ�澯�ж� ---------
    if(g_stRcParamSet.cDlOutPwr2 < g_stSettingParamSet.cDlOutUnderPwrThr2) //�����������Ƿ�������ޣ�ͨ��2��
    {
        MYALARM.stDlOutUnderPwr2.ucRcStatus = ALARM;  //�������Ƿ���ʸ澯��ͨ��2��
    }
    else if(g_stRcParamSet.cDlOutPwr2 > g_stSettingParamSet.cDlOutOverPwrThr2) //����������ʹ��������ޣ�ͨ��2��
    {
        MYALARM.stDlOutOverPwr2.ucRcStatus = ALARM;  //������������ʸ澯��ͨ��2��
    }

    //########################################################################
    //############################ ����פ����ֵ1 #############################
    
    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cReflectPwrAdjust1;   //��һ·���书�ʵ�ƽУ׼ֵ
    ucTempMaxValue = REFLECT_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    //MCM-17_20061106_linyu_begin    
    g_stRcParamSet.ucDlSwr = CountVSWR(ulValueTemp[11], ucTempMaxValue, cTempAdjust, 
                                         ucTempCoupler, g_stRcParamSet.cDlOutPwr);

    MYALARM.stDlSwr1.ucRcStatus = NOT_ALARM;  //��ȡ������פ����1�澯
    
    //--------- ����Ϊ�澯�ж� ---------
    if(g_stRcParamSet.ucDlSwr > g_stSettingParamSet.ucDlSwrThr) //����פ�������ޣ�ͨ��1��
    {
        MYALARM.stDlSwr1.ucRcStatus = ALARM;  //����פ����1�澯
    }
    //MCM-17_20061106_linyu_end
    //########################################################################
    //############################ ����פ����ֵ2 #############################
    
    cTempAdjust = g_stYkppParamSet.stYkppRfParam.cReflectPwrAdjust2;   //�ڶ�·���书�ʵ�ƽУ׼ֵ
    ucTempMaxValue = REFLECT_MAXVALUE;
    ucTempCoupler = GENERAL_COUPLER;
    
    g_stRcParamSet.ucDlSwr2 = CountVSWR(ulValueTemp[12], ucTempMaxValue, cTempAdjust, 
                                         ucTempCoupler, g_stRcParamSet.cDlOutPwr2);

    MYALARM.stDlSwr2.ucRcStatus = NOT_ALARM;  //��ȡ������פ����2�澯
    
    //--------- ����Ϊ�澯�ж� ---------
    if(g_stRcParamSet.ucDlSwr2 > g_stSettingParamSet.ucDlSwrThr2) //����פ�������ޣ�ͨ��2��
    {
        MYALARM.stDlSwr2.ucRcStatus = ALARM;  //����פ����2�澯
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //%%%%%%%%%%%%%%%%%%%%%%% ��Դ���ϸ澯�ж� %%%%%%%%%%%%%%%%%%%%%%%
    // uiValueTemp[9]  12V���  ����9VΪ����
    // uiValueTemp[7]  27V���  ����20VΪ����
    if(ulValueTemp[9] < 153)// || (ulValueTemp[14] < 159))
    {
        MYALARM.stPwrFault.ucRcStatus = ALARM; //��Դ���ϸ澯
    }
    else
    {
        MYALARM.stPwrFault.ucRcStatus = NOT_ALARM; //��Դ���ϸ澯
    }

}
