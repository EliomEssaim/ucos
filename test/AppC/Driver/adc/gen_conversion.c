/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   Gen_conversion.cpp
    ����:     ����
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  
       ����:  
       ����:  �����д���
    2. ����:  2006/11/10
       ����:  �½�
       ����:  �޸�����MCM-29����BCCH��ƽ�ж������Ӷ�0���ж�,Ϊ������
              �����������������Ƿͬʱ�澯���ڲ�������һ���澯ʱ��ʹ��һ�����澯��
    3. ����:  2006/11/01
       ����:  �½�
       ����:  �޸�����MCM-11����ԭ�������漰�����жϵĵط�ȫ����Ϊ��
              �������ñ���ĳ����߶���ֵ���жϡ�
    4. ����:  2006/11/06
       ����:  ����
       ����:  �޸�����MCM-17�������д�ͨ���ŵ�ͨ��1�Ĳ�����ӳ���Ӧ��
              ������ͨ���ŵĲ����ϣ������豸�Ƕ�ͨ��ʱ����ѯͨ��1�Ĳ�
              ��ʵ�ʷ��ʵ��Ƕ�Ӧ�Ĳ���ͨ���ŵĲ����ĵ�ַ��
    5. ����:  2006/11/20
       ����:  ����
       ����:  �޸�����MCM-39���޸�3G�豸������ʵ������ֵ���㹫ʽ��
    6. ����:  2006/11/29
       ����:  ����
       ����:  �޸�����MCM-42��
              ����ͨ���������������פ������
    7. ����:  2006/12/18
       ����:  ����
       ����:  �޸�����MCM-48��
              ����פ������ʱ���жϣ�����޷���������
    8. ����:  2007/11/20
       ����:  �½�
       ����:  �޸�����MCM-80��              
              ����̫�������ص�ѹ��⡣
---------------------------------------------------------------------------*/
#include "../../AC/Actask.h"

//ȫ�ֱ�������
extern DEV_TYPE_TABLE_ST g_stDevTypeTable;

/*************************************************
  Copyright (C), 2006-2007, Lin Wei
  �ļ�����    current_conversion.c
  ����:       ����
  �汾��      1.0.0
  ���ڣ�      2006/07/19 
  ����������  ���ļ�ʵ�ֽ�A/Dģ�����ɼ�ֵ���ݲ�ͬ���㹫ʽ���õ���Ӧ���
  ��ע��      ���ļ�������ƶ�3G��Ŀ
  �����б�  // ��Ҫ�����б�ÿ����¼Ӧ���������������ܼ�Ҫ˵��
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
//���ܣ�����2G�����������ֵ(dBm)
//���룺uiOldValue -- �ɼ����
//���أ��������(��λ��dBm)����ڼ�ⷶΧ���ֻ���ڼ�ⷶΧ
//**********************************************************
UINT32 CountDlPow2G(UINT32 uiOldValue)  //���ɼ������������ֵ�����ݹ�ʽ�������Ӧ��DBֵ
{
    INT32 iMaxPow;
    LONG lTempPowOut;
    INT32 iAdjustValue;

    //��������������ֵ
    //�����������У׼ֵ����ֵĬ��Ϊ0��ֻ�е�����ģ�鲻��������Ҫ��ʱ����Ҫ����
    iMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax;  //��������������
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlOutAdjust;  //�����������У׼ֵ

    lTempPowOut = iAdjustValue + 35 - (((LONG)(uiOldValue) * 50 + 512) / 1024);

    //����������ʼ�ⷶΧ��ƹ���+2����ƹ���-18
    if(lTempPowOut < -2)
    {
        return HIGHER_VALUE;  //���ڼ�ⷶΧʱ�ķ���ֵ
    }
    else if(lTempPowOut > 18)
    {
        return LOWER_VALUE;  //���ڼ�ⷶΧʱ�ķ���ֵ
    }
    else
    {
        return (UINT32)(iMaxPow - lTempPowOut);
    }
}
//**********************************************************
//���ܣ�����2G�����������ֵ(dBm)(ͨ����)
//���룺uiOldValue -- �ɼ����
//���أ��������(��λ��dBm)����ڼ�ⷶΧ���ֻ���ڼ�ⷶΧ
//**********************************************************
UINT32 CountDlPow2GChunnel2(UINT32 uiOldValue)  //���ɼ������������ֵ�����ݹ�ʽ�������Ӧ��DBֵ
{
    INT32 iMaxPow;
    LONG lTempPowOut;
    INT32 iAdjustValue;

    //��������������ֵ
    //�����������У׼ֵ����ֵĬ��Ϊ0��ֻ�е�����ģ�鲻��������Ҫ��ʱ����Ҫ����
    iMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax2;  //��������������(ͨ����)
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlOutAdjust2;  //�����������У׼ֵ(ͨ����)

    lTempPowOut = iAdjustValue + 35 - (((LONG)(uiOldValue) * 50 + 512) / 1024);

    //����������ʼ�ⷶΧ��ƹ���+2����ƹ���-18
    if(lTempPowOut < -2)
    {
        return HIGHER_VALUE;  //���ڼ�ⷶΧʱ�ķ���ֵ
    }
    else if(lTempPowOut > 18)
    {
        return LOWER_VALUE;  //���ڼ�ⷶΧʱ�ķ���ֵ
    }
    else
    {
        return (UINT32)(iMaxPow - lTempPowOut);
    }
}

//**********************************************************
//���ܣ�����2G�����������ֵ(dBm)
//���룺uiOldValue -- �ɼ����
//���أ��������(��λ��dBm)����ڼ�ⷶΧ���ֻ���ڼ�ⷶΧ
//**********************************************************
UINT32 CountUlPow2G(UINT32 uiOldValue)  //���ɼ������������ֵ�����ݹ�ʽ�������Ӧ��DBֵ
{
    INT32 iMaxPow;
    LONG lTempPowOut;
    INT32 iAdjustValue;

    //��������������ֵ
    //�����������У׼ֵ����ֵĬ��Ϊ0��ֻ�е�����ģ�鲻��������Ҫ��ʱ����Ҫ����
    iMaxPow = g_stYkppParamSet.stYkppRfParam.cUlOutMax;  //��������������
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sUlOutAdjust;  //�����������У׼ֵ

    lTempPowOut = iAdjustValue + 35 - (((LONG)(uiOldValue) * 50 + 512) / 1024);
    
    //����������ʼ�ⷶΧ��ƹ���+2����ƹ���-13
    if(lTempPowOut < -2)
    {
        return HIGHER_VALUE;  //���ڼ�ⷶΧʱ�ķ���ֵ
    }
    else if(lTempPowOut > 13)
    {
        return LOWER_VALUE;  //���ڼ�ⷶΧʱ�ķ���ֵ
    }
    else
    {
        return (UINT32)(iMaxPow - lTempPowOut);
    }
}

//**********************************************************
//���ܣ� ��פ���Ȳ�����ƽֵ�����ݹ�ʽ�������Ӧ��פ����ֵ
//���룺  uiOldVSWR  -- ���书�ʲɼ����
//       uiDownOutput -- �����������ֵ
//���أ� �������(��λ��dBm)����ڼ�ⷶΧ���ֻ���ڼ�ⷶΧ
//��ע�� ���ñ�����ǰ��������CountPowOut3G���������������ֵ
//**********************************************************
UINT32 CountVSWR2G(UINT32 uiOldVSWR, UINT32 uiDownOutput)
{
    UINT32 uiMaxPow, TempVSWR;
    INT32 iAdjustValue;

    uiMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax;  //��������������
    //���з��书��У׼ֵ��Ĭ��Ϊ0��ֻ�е�����ģ�鲻��������Ҫ��ʱ����Ҫ����
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlReflAdjust;  //���з��书��У׼ֵ

    TempVSWR = (uiOldVSWR * 25 + 256) / 512; //��256��Ϊ����������

    uiMaxPow = uiMaxPow + iAdjustValue + TempVSWR - 45;  //�õ����书��ֵ//3.5V = ��ƹ���-10������Ϊ45
    
    //MCM-48_20061212_linwei_begin
    if(uiDownOutput > uiMaxPow)
    {
        uiMaxPow = uiDownOutput - uiMaxPow;   //������������ʼ�ȥ���з��书�ʡ�
    }
    else
    {
        uiMaxPow = 0;
    }
    //MCM-48_20061212_linwei_end

    if(uiMaxPow >= 17)
    {
        return 12;   //����17ʱ��פ����ֵΪ1.2
    }
    else if(uiMaxPow >= 16)
    {
        return 13;   //����16ʱ��פ����ֵΪ1.3
    }
    else if(uiMaxPow >= 15)
    {
        return 14;   //����15��16ʱ��פ����ֵΪ1.4
    }
    else if(uiMaxPow >= 13)
    {
        return 15;   //����13��14ʱ��פ����ֵΪ1.5
    }
    else if(uiMaxPow >= 12)
    {
        return 16;   //����12ʱ��פ����ֵΪ1.6
    }
    else if(uiMaxPow >= 11)
    {
        return 17;   //����11ʱ��פ����ֵΪ1.7
    }
    else if(uiMaxPow >= 10)
    {
        return 18;   //����10ʱ��פ����ֵΪ1.8
    }
    else if (uiMaxPow >= 8)
    {
        return 20;   //����8��9ʱ��פ����ֵΪ2.0
    }
    else if (uiMaxPow >= 7)
    {
        return 23;   // 7ʱ��פ����ֵΪ2.3
    }
    else if (uiMaxPow >= 6)
    {
        return 25;   // 6ʱ��פ����ֵΪ2.5
    }
    else if (uiMaxPow >= 4)
    {
        return 30;   // 4~5ʱ��פ����ֵΪ3.0
    }
    else return 40;     //С��4ʱ��פ����ֵΪ��ʾΪ4.0
}

//**********************************************************
//���ܣ� ��פ���Ȳ�����ƽֵ�����ݹ�ʽ�������Ӧ��פ����ֵ(ͨ����)
//���룺  uiOldVSWR  -- ���书�ʲɼ����
//       uiDownOutput -- �����������ֵ
//���أ� �������(��λ��dBm)����ڼ�ⷶΧ���ֻ���ڼ�ⷶΧ
//��ע�� ���ñ�����ǰ��������CountPowOut3G���������������ֵ
//**********************************************************
UINT32 CountVSWR2GChunnel2(UINT32 uiOldVSWR, UINT32 uiDownOutput)
{
    UINT32 uiMaxPow, TempVSWR;
    INT32 iAdjustValue;

    uiMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax2;  //��������������
    //���з��书��У׼ֵ��Ĭ��Ϊ0��ֻ�е�����ģ�鲻��������Ҫ��ʱ����Ҫ����
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlReflAdjust2;  //���з��书��У׼ֵ

    TempVSWR = (uiOldVSWR * 25 + 256) / 512; //��256��Ϊ����������

    uiMaxPow = uiMaxPow + iAdjustValue + TempVSWR - 45;  //�õ����书��ֵ//3.5V = ��ƹ���-10������Ϊ45
    
    //MCM-48_20061212_linwei_begin
    if(uiDownOutput > uiMaxPow)
    {
        uiMaxPow = uiDownOutput - uiMaxPow;   //������������ʼ�ȥ���з��书�ʡ�
    }
    else
    {
        uiMaxPow = 0;
    }
    //MCM-48_20061212_linwei_end

    if(uiMaxPow >= 17)
    {
        return 12;   //����17ʱ��פ����ֵΪ1.2
    }
    else if(uiMaxPow >= 16)
    {
        return 13;   //����16ʱ��פ����ֵΪ1.3
    }
    else if(uiMaxPow >= 15)
    {
        return 14;   //����15��16ʱ��פ����ֵΪ1.4
    }
    else if(uiMaxPow >= 13)
    {
        return 15;   //����13��14ʱ��פ����ֵΪ1.5
    }
    else if(uiMaxPow >= 12)
    {
        return 16;   //����12ʱ��פ����ֵΪ1.6
    }
    else if(uiMaxPow >= 11)
    {
        return 17;   //����11ʱ��פ����ֵΪ1.7
    }
    else if(uiMaxPow >= 10)
    {
        return 18;   //����10ʱ��פ����ֵΪ1.8
    }
    else if (uiMaxPow >= 8)
    {
        return 20;   //����8��9ʱ��פ����ֵΪ2.0
    }
    else if (uiMaxPow >= 7)
    {
        return 23;   // 7ʱ��פ����ֵΪ2.3
    }
    else if (uiMaxPow >= 6)
    {
        return 25;   // 6ʱ��פ����ֵΪ2.5
    }
    else if (uiMaxPow >= 4)
    {
        return 30;   // 4~5ʱ��פ����ֵΪ3.0
    }
    else return 40;     //С��4ʱ��פ����ֵΪ��ʾΪ4.0
}

//**********************************************************
//���ܣ����ɼ�ֵͨ������ó��������(dBm),�����и澯״̬�ж�
//���룺uiOldValue -- �ɼ����
//      uiUpOrDown=1 -- ����   uiUpOrDown=0 -- ����
//���أ��������(��λ��dBm)����ڼ�ⷶΧ���ֻ���ڼ�ⷶΧ
//**********************************************************
UINT32 CountPowOut3G(UINT32 uiOldValue, UINT32 uiUpOrDown)  //���ɼ������������ֵ�����ݹ�ʽ�������Ӧ��DBֵ
{
    INT32 iMaxPow;
    ULONG ulTempPowOut;
    INT32 iAdjustValue;

    //�ϻ���������������ֵ
    //�ϻ������������У׼ֵ����ֵĬ��Ϊ0��ֻ�е�����ģ�鲻��������Ҫ��ʱ����Ҫ����
    if(uiUpOrDown == DownLoad)
    {
        iMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax;  //��������������
        iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlOutAdjust;  //�����������У׼ֵ
    }
    else
    {
        iMaxPow = g_stYkppParamSet.stYkppRfParam.cUlOutMax;  //��������������
        iAdjustValue = g_stYkppParamSet.stYkppRfParam.sUlOutAdjust;  //�����������У׼ֵ
    }
//*----- test -----
   // uiMaxPow = 37;
   // iAdjustValue = 0;
//---------------*/

    //ulTempPowOut = ((ULONG)(uiOldValue) * 25 * 10 + 256) / 512; //����Ҫ��С�����һλ�����Գ���10
    ulTempPowOut = (2250 / 4) - (((ULONG)(uiOldValue) * 125 * 10) / (512 * 4));
    //����������ʼ�ⷶΧ��ƹ���+2����ƹ���-45
    if((LONG)(ulTempPowOut) < (LONG)(iAdjustValue * 10 - 20))
    {
        return HIGHER_VALUE;  //���ڼ�ⷶΧʱ�ķ���ֵ
    }
    else if(ulTempPowOut > (iAdjustValue * 10 + 450))
    {
        return LOWER_VALUE;  //���ڼ�ⷶΧʱ�ķ���ֵ
    }
    else
    {
        return (UINT32)((iMaxPow * 10) + iAdjustValue * 10 - ulTempPowOut);
    }
}

//**********************************************************
//���ܣ����ɼ�ֵͨ������ó��������(dBm),�����и澯״̬�ж�
//���룺uiOldValue -- �ɼ����
//      uiUpOrDown=1 -- ����   uiUpOrDown=0 -- ����
//���أ��������(��λ��dBm)����ڼ�ⷶΧ���ֻ���ڼ�ⷶΧ
//**********************************************************
UINT32 CountPowOut3GChunnel2(UINT32 uiOldValue, UINT32 uiUpOrDown)  //���ɼ������������ֵ�����ݹ�ʽ�������Ӧ��DBֵ
{
    INT32 iMaxPow;
    ULONG ulTempPowOut;
    INT32 iAdjustValue;

    //�ϻ���������������ֵ
    //�ϻ������������У׼ֵ����ֵĬ��Ϊ0��ֻ�е�����ģ�鲻��������Ҫ��ʱ����Ҫ����
    if(uiUpOrDown == DownLoad)
    {
        iMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax2;  //��������������
        iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlOutAdjust2;  //�����������У׼ֵ
    }
    else
    {
        iMaxPow = g_stYkppParamSet.stYkppRfParam.cUlOutMax;  //��������������
        iAdjustValue = g_stYkppParamSet.stYkppRfParam.sUlOutAdjust;  //�����������У׼ֵ
    }
//*----- test -----
   // uiMaxPow = 37;
   // iAdjustValue = 0;
//---------------*/

    //ulTempPowOut = ((ULONG)(uiOldValue) * 25 * 10 + 256) / 512; //����Ҫ��С�����һλ�����Գ���10
    ulTempPowOut = (2250 / 4) - (((ULONG)(uiOldValue) * 125 * 10) / (512 * 4));
    //����������ʼ�ⷶΧ��ƹ���+2����ƹ���-45
    if((LONG)(ulTempPowOut) < (LONG)(iAdjustValue * 10 - 20))
    {
        return HIGHER_VALUE;  //���ڼ�ⷶΧʱ�ķ���ֵ
    }
    else if(ulTempPowOut > (iAdjustValue * 10 + 450))
    {
        return LOWER_VALUE;  //���ڼ�ⷶΧʱ�ķ���ֵ
    }
    else
    {
        return (UINT32)((iMaxPow * 10) + iAdjustValue * 10 - ulTempPowOut);
    }
}

//**********************************************************
//���ܣ� ��פ���Ȳ�����ƽֵ�����ݹ�ʽ�������Ӧ��פ����ֵ
//���룺  uiOldVSWR  -- ���书�ʲɼ����
//       uiDownOutput -- �����������ֵ
//���أ� �������(��λ��dBm)����ڼ�ⷶΧ���ֻ���ڼ�ⷶΧ
//��ע�� ���ñ�����ǰ��������CountPowOut3G���������������ֵ
//**********************************************************
UINT32 CountVSWR3G(UINT32 uiOldVSWR, UINT32 uiDownOutput)
{
    UINT32 uiMaxPow, TempVSWR;
    INT32 iAdjustValue;

    uiMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax;  //��������������
    //���з��书��У׼ֵ��Ĭ��Ϊ0��ֻ�е�����ģ�鲻��������Ҫ��ʱ����Ҫ����
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlReflAdjust;  //���з��书��У׼ֵ
//******** test ********
  //  uiMaxPow = 37;
  //  iAdjustValue = -1;
/***********************/

    TempVSWR = (uiOldVSWR * 25 + 256) / 512; //��256��Ϊ����������

    uiMaxPow = uiMaxPow + iAdjustValue + TempVSWR - 55;  //�õ����书��ֵ//4.5V = ��ƹ���-10������Ϊ45

    //MCM-48_20061212_linwei_begin
    uiDownOutput = uiDownOutput / 10;
    if(uiDownOutput > uiMaxPow)
    {
        uiMaxPow = uiDownOutput - uiMaxPow;   //������������ʼ�ȥ���з��书�ʡ�
    }
    else
    {
        uiMaxPow = 0;
    }
    //MCM-48_20061212_linwei_end

    if(uiMaxPow >= 17)
    {
        return 12;   //����17ʱ��פ����ֵΪ1.2
    }
    else if(uiMaxPow >= 16)
    {
        return 13;   //����16ʱ��פ����ֵΪ1.3
    }
    else if(uiMaxPow >= 15)
    {
        return 14;   //����15��16ʱ��פ����ֵΪ1.4
    }
    else if(uiMaxPow >= 13)
    {
        return 15;   //����13��14ʱ��פ����ֵΪ1.5
    }
    else if(uiMaxPow >= 12)
    {
        return 16;   //����12ʱ��פ����ֵΪ1.6
    }
    else if(uiMaxPow >= 11)
    {
        return 17;   //����11ʱ��פ����ֵΪ1.7
    }
    else if(uiMaxPow >= 10)
    {
        return 18;   //����10ʱ��פ����ֵΪ1.8
    }
    else if (uiMaxPow >= 8)
    {
        return 20;   //����8��9ʱ��פ����ֵΪ2.0
    }
    else if (uiMaxPow >= 7)
    {
        return 23;   // 7ʱ��פ����ֵΪ2.3
    }
    else if (uiMaxPow >= 6)
    {
        return 25;   // 6ʱ��פ����ֵΪ2.5
    }
    else if (uiMaxPow >= 4)
    {
        return 30;   // 4~5ʱ��פ����ֵΪ3.0
    }
    else return 40;     //С��4ʱ��פ����ֵΪ��ʾΪ4.0
}

//**********************************************************
//���ܣ� ��פ���Ȳ�����ƽֵ�����ݹ�ʽ�������Ӧ��פ����ֵ
//���룺  uiOldVSWR  -- ���书�ʲɼ����
//       uiDownOutput -- �����������ֵ
//���أ� �������(��λ��dBm)����ڼ�ⷶΧ���ֻ���ڼ�ⷶΧ
//��ע�� ���ñ�����ǰ��������CountPowOut3G���������������ֵ
//**********************************************************
UINT32 CountVSWR3GChunnel2(UINT32 uiOldVSWR, UINT32 uiDownOutput)
{
    UINT32 uiMaxPow, TempVSWR;
    INT32 iAdjustValue;

    uiMaxPow = g_stYkppParamSet.stYkppRfParam.ucDlOutMax2;  //��������������
    //���з��书��У׼ֵ��Ĭ��Ϊ0��ֻ�е�����ģ�鲻��������Ҫ��ʱ����Ҫ����
    iAdjustValue = g_stYkppParamSet.stYkppRfParam.sDlReflAdjust2;  //���з��书��У׼ֵ
//******** test ********
  //  uiMaxPow = 37;
  //  iAdjustValue = -1;
/***********************/

    TempVSWR = (uiOldVSWR * 25 + 256) / 512; //��256��Ϊ����������

    uiMaxPow = uiMaxPow + iAdjustValue + TempVSWR - 55;  //�õ����书��ֵ//4.5V = ��ƹ���-10������Ϊ45

    //MCM-48_20061212_linwei_begin
    uiDownOutput = uiDownOutput / 10;
    if(uiDownOutput > uiMaxPow)
    {
        uiMaxPow = uiDownOutput - uiMaxPow;   //������������ʼ�ȥ���з��书�ʡ�
    }
    else
    {
        uiMaxPow = 0;
    }
    //MCM-48_20061212_linwei_end

    if(uiMaxPow >= 17)
    {
        return 12;   //����17ʱ��פ����ֵΪ1.2
    }
    else if(uiMaxPow >= 16)
    {
        return 13;   //����16ʱ��פ����ֵΪ1.3
    }
    else if(uiMaxPow >= 15)
    {
        return 14;   //����15��16ʱ��פ����ֵΪ1.4
    }
    else if(uiMaxPow >= 13)
    {
        return 15;   //����13��14ʱ��פ����ֵΪ1.5
    }
    else if(uiMaxPow >= 12)
    {
        return 16;   //����12ʱ��פ����ֵΪ1.6
    }
    else if(uiMaxPow >= 11)
    {
        return 17;   //����11ʱ��פ����ֵΪ1.7
    }
    else if(uiMaxPow >= 10)
    {
        return 18;   //����10ʱ��פ����ֵΪ1.8
    }
    else if (uiMaxPow >= 8)
    {
        return 20;   //����8��9ʱ��פ����ֵΪ2.0
    }
    else if (uiMaxPow >= 7)
    {
        return 23;   // 7ʱ��פ����ֵΪ2.3
    }
    else if (uiMaxPow >= 6)
    {
        return 25;   // 6ʱ��פ����ֵΪ2.5
    }
    else if (uiMaxPow >= 4)
    {
        return 30;   // 4~5ʱ��פ����ֵΪ3.0
    }
    else return 40;     //С��4ʱ��פ����ֵΪ��ʾΪ4.0
}

//**********************************************************
//���ܣ� ���¶Ȳ���ֵ�����ݹ�ʽ�������Ӧ���¶�ֵ���棩
//���룺 uiOldTem   -- �¶Ȳ���ֵ
//���أ� �¶�ֵ����λ �棬�з�����INT32��
//**********************************************************
INT32 CountTem3G(UINT32 uiOldTem)
{
    INT32 TemValue;
    TemValue = (INT32)((125 * uiOldTem + 256) / 512) - 50;  //ת����ʽ��N=N*125/512 - 50
    return TemValue;
}

//**********************************************************
//���ܣ� ���¶Ȳ���ֵ�����ݹ�ʽ�������Ӧ���¶�ֵ���棩
//���룺 uiOldTem   -- �¶Ȳ���ֵ
//���أ� �¶�ֵ����λ �棬�з�����INT32��
//**********************************************************
INT32 CountTem2G(UINT32 uiOldTem)
{
    INT32 TemValue;
    //TemValue = (INT32)((125 * uiOldTem + 384) / 768) - 50;  //ת����ʽ��N=N*125/768 - 50
    TemValue = ((125 * (ULONG)uiOldTem + 256) / 512) - 50 -1; //��-1��ΪУ׼����Ϊ������ѹֻ��2.49������2.5
    return TemValue;
}

//**********************************************************
//���ܣ� �ô��ʼ����ʱ���������빦��dBmֵ����
//���룺 uiOldPwr  -- �������빦�ʲ���ֵ
//���أ� �������빦��dBmֵ
//**********************************************************
INT32 CountBigPwr(UINT32 uiOldPwr)
{
    INT32 PwrValue;
    //ת����ʽ��N=MAX-(4.5/0.08-x*500/1024/8)
    PwrValue = 0 - ((225 / 4) - (((INT32)(uiOldPwr) * 125) / (512 * 4)));
    //?????�˴������������������Ŀ���
    PwrValue += g_stYkppParamSet.stYkppRfParam.ucDlInCoupler;  //�����������������
    return PwrValue;
}

//**********************************************************
//���ܣ� ��С���ʼ����ʱ���������빦��dBmֵ����
//���룺 uiOldPwr  -- �������빦�ʲ���ֵ
//���أ� �������빦��dBmֵ
//**********************************************************
INT32 CountSmallPwr(UINT32 uiOldPwr)
{
    INT32 PwrValue;
    //ת����ʽ��N=MAX-(4.5/0.08-x*500/1024/8)
    PwrValue = 0 - 40 - ((225 / 4) - (((INT32)(uiOldPwr) * 125) / (512 * 4)));
    //?????�˴������������������Ŀ���
    PwrValue += g_stYkppParamSet.stYkppRfParam.ucDlInCoupler;  //�����������������
    return PwrValue;
}

//**********************************************************
//���ܣ� �ô��ʼ����ʱ���������빦��dBmֵ����
//���룺 uiOldPwr  -- �������빦�ʲ���ֵ
//���أ� �������빦��dBmֵ
//**********************************************************
INT32 CountBigPwr3G(UINT32 uiOldPwr)
{
    INT32 PwrValue;
    //ת����ʽ��N=MAX-10*(4.5/0.08-x*500/1024/8)
    PwrValue = 100 - 10 * ((225 / 4) - (((INT32)(uiOldPwr) * 125) / (512 * 4)));
    //?????�˴������������������Ŀ���
    //PwrValue += ��������� * 10��
    PwrValue += (g_stYkppParamSet.stYkppRfParam.ucDlInCoupler * 10);  //�����������������
    return PwrValue;
}

//**********************************************************
//���ܣ� ��С���ʼ����ʱ���������빦��dBmֵ����
//���룺 uiOldPwr  -- �������빦�ʲ���ֵ
//���أ� �������빦��dBmֵ
//**********************************************************
INT32 CountSmallPwr3G(UINT32 uiOldPwr)
{
    INT32 PwrValue;
    //ת����ʽ��N=MAX-10*(4.5/0.08-x*500/1024/8)
    PwrValue = 0 - 300 - 10 * ((225 / 4) - (((INT32)(uiOldPwr) * 125) / (512 * 4)));
    //?????�˴������������������Ŀ���
    //PwrValue += ��������� * 10��
    PwrValue += (g_stYkppParamSet.stYkppRfParam.ucDlInCoupler * 10);  //�����������������
    return PwrValue;
}

//**********************************************************
//���ܣ� 
//���룺 uiValueTemp  -- 
//���أ� 
//**********************************************************
void PutIntoRam(UINT32* uiValueTemp)
{
    UCHAR ucUpPwrSpecial;  //ֱ����ϻ��ͱ�־
    INT32 TempData;
    
//************* test **************
  //  ucTempType = 2;
/*********************************/
    //%%%%%%%%%%%%%%%%%%%%%%%%%% �������빦�ʵ�ƽ %%%%%%%%%%%%%%%%%%%%%%%%%%
    //========= �������Ƶ�ӻ�����������ֵ���������ݹ��� ============
    if(g_stDevTypeTable.ucRFRoleType != RF_MASTER)
    {
        //�˴���������Ƶ�ӻ�������������gen_actask.cpp�д���
    }
    else
    {
        //�ȴ���BCCH���յ�ƽ����ѯ����ʱ��Ҫ��ʾ����
        
        //MCM-29_20061110_zhangjie_begin
        if(g_stRcParamSet.cSsBcchRxLev >= 0) //����0˵�����²�ѯ������Դ��Ϣ����Ҫ����
        {
            g_stRcParamSet.cSsBcchRxLev = g_stRcParamSet.cSsBcchRxLev - 110;
            g_stRcParamSet.cSsBcchRxLev += g_stYkppParamSet.stYkppRfParam.ucDlInCoupler;
        }
        //MCM-29_20061110_zhangjie_end

        g_stRcParamSet.ucDlInPwrFlag = NORMAL_FLAG; //�������빦�ʵ�ƽ������Χ�ı�־
        g_stRcParamSet.uc3GDlInPwrFlag = NORMAL_FLAG; //�������빦�ʵ�ƽ������Χ�ı�־
        ucUpPwrSpecial = 0;  //ֱ����ϻ��ͱ�־
        cDetectDlInPwr = 0;
    
    //======== �������Э����Ҫ��ʹ��MODEN�������������Ļ� ========
    if(g_stYkppParamSet.stYkppCtrlParam.ucDlInAcqMode == DLIN_MODEN_MEASURE)
    {
        if(g_stRcParamSet.cSsBcchRxLev == -1)
        {
            g_stRcParamSet.ucDlInPwrFlag = LOWER_FLAG; //�������빦�ʵ�ƽ���ڼ�ⷶΧ
            g_stRcParamSet.uc3GDlInPwrFlag = LOWER_FLAG;
            MYALARM.stDlInUnderPwr.ucRcStatus = ALARM; //��������Ƿ���ʸ澯
            cDetectDlInPwr = 1;
        }
        else
        {
            g_stRcParamSet.cDlInPwr = g_stRcParamSet.cSsBcchRxLev;
            g_stRcParamSet.sDlInPwr = g_stRcParamSet.cDlInPwr * 10;
        }
    }
    //============ �ù��ʼ��ģ��������������빦�ʵĻ� ============
    else
    {
        if(g_stDevTypeTable.ucIsCoupling)
        {
            ucUpPwrSpecial = 1;  //ֱ����ϻ��ͱ�־
        }

        if(ucUpPwrSpecial == 1)  //ֱ����ϻ��ͱ�־
        {
            TempData = CountBigPwr3G(uiValueTemp[11]);
        }
        else
        {
            TempData = CountSmallPwr3G(uiValueTemp[11]);
        }
        //3G:�������빦�ʵ�ƽ��sint2�ͣ���λΪdBm������Ϊ10
        g_stRcParamSet.sDlInPwr = TempData;//(SHORT)(TempData) * 10;
        //�������빦�ʵ�ƽ
        g_stRcParamSet.cDlInPwr = (SHORT)(TempData) / 10;//TempData;
    }
    //============ �������ַ�ʽ����⣬��⵽���ݺ�ĸ澯�ж� ============
    if(cDetectDlInPwr == 0)
    {
        TempData = g_stRcParamSet.cDlInPwr;
        if(TempData > (-30 + ucUpPwrSpecial * 40)) //���������ź��Ǿ���������ϵ��豸��Ҫ���ⷶΧ�ڣ�30dBm����80dBm
        {
            g_stRcParamSet.ucDlInPwrFlag = HIGHER_FLAG; //�������빦�ʵ�ƽ���ڼ�ⷶΧ
            g_stRcParamSet.uc3GDlInPwrFlag = HIGHER_FLAG; //�������빦�ʵ�ƽ������Χ�ı�־
            MYALARM.stDlInOverPwr.ucRcStatus = ALARM; //������������ʸ澯
            //MCM-29_20061110_zhangjie_begin
            MYALARM.stDlInUnderPwr.ucRcStatus = NOT_ALARM; //��������Ƿ���ʸ澯
            //MCM-29_20061110_zhangjie_end
        }
        else if(TempData < (-80 + ucUpPwrSpecial * 50)) //���������ź��Ǿ���������ϵ��豸��Ҫ���ⷶΧ�ڣ�30dBm����80dBm
        {
            g_stRcParamSet.ucDlInPwrFlag = LOWER_FLAG; //�������빦�ʵ�ƽ���ڼ�ⷶΧ
            g_stRcParamSet.uc3GDlInPwrFlag = LOWER_FLAG;
            MYALARM.stDlInUnderPwr.ucRcStatus = ALARM; //��������Ƿ���ʸ澯
            //MCM-29_20061110_zhangjie_begin
            MYALARM.stDlInOverPwr.ucRcStatus = NOT_ALARM; //������������ʸ澯
            //MCM-29_20061110_zhangjie_end
        }
        else
        {        //--------- ֵ��������Χ��ʱ�ĸ澯�ж� ---------
            if(g_stDevTypeTable.ucGenType == GEN_TYPE_3G)  //3G���͵��豸���͵�ֵ������49
            {   //3G����ʱ����������澯�ж�
                if(g_stRcParamSet.sDlInPwr < g_stSettingParamSet.sDlInUnderPwrThr) //3G�������빦��Ƿ��������
                {
                    MYALARM.stDlInUnderPwr.ucRcStatus = ALARM; //��������Ƿ���ʸ澯
                    MYALARM.stDlInOverPwr.ucRcStatus = NOT_ALARM; //������������ʲ��澯
                }
                else if(g_stRcParamSet.sDlInPwr > g_stSettingParamSet.sDlInOverPwrThr) //3G�������빦�ʹ���������
                {
                    MYALARM.stDlInOverPwr.ucRcStatus = ALARM; //������������ʸ澯
                    MYALARM.stDlInUnderPwr.ucRcStatus = NOT_ALARM; //��������Ƿ���ʲ��澯
                }
                else
                {
                    MYALARM.stDlInOverPwr.ucRcStatus = NOT_ALARM; //������������ʲ��澯
                    MYALARM.stDlInUnderPwr.ucRcStatus = NOT_ALARM; //��������Ƿ���ʲ��澯
                }
            }
            else
            {   //2G����ʱ����������澯�ж�
                if(g_stRcParamSet.cDlInPwr < g_stSettingParamSet.cDlInUnderPwrThr) //�������빦��Ƿ��������
                {
                    MYALARM.stDlInUnderPwr.ucRcStatus = ALARM; //��������Ƿ���ʸ澯
                    MYALARM.stDlInOverPwr.ucRcStatus = NOT_ALARM; //������������ʲ��澯
                }
                else if(g_stRcParamSet.cDlInPwr > g_stSettingParamSet.cDlInOverPwrThr) //�������빦�ʹ���������
                {
                    MYALARM.stDlInOverPwr.ucRcStatus = ALARM; //������������ʸ澯
                    MYALARM.stDlInUnderPwr.ucRcStatus = NOT_ALARM; //��������Ƿ���ʲ��澯
                }
                else
                {
                    MYALARM.stDlInOverPwr.ucRcStatus = NOT_ALARM; //������������ʲ��澯
                    MYALARM.stDlInUnderPwr.ucRcStatus = NOT_ALARM; //��������Ƿ���ʲ��澯
                    }
                }
            }
        }
    }
    //%%%%%%%%%%%%%%%%%%%%%%%%%% ����������ʵ�ƽ %%%%%%%%%%%%%%%%%%%%%%%%%%
    g_stRcParamSet.sUlOutPwr = 0; //3G:����������ʵ�ƽ��sint2�ͣ���λΪdBm������Ϊ10
    g_stRcParamSet.cUlOutPwr = 0; //����������ʵ�ƽ

    if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)  //�����2G�豸���ͣ������2G�ļ��㺯��
    {
        TempData = CountUlPow2G(uiValueTemp[2]);
    }
    else               //�������3G�ļ��㺯��
    {
        TempData = CountPowOut3G(uiValueTemp[2], UpLoad);
    }
    
    MYALARM.stUlOutOverPwr.ucRcStatus = NOT_ALARM;
    MYALARM.stUlOutUnderPwr.ucRcStatus = NOT_ALARM;
    
    if(TempData == HIGHER_VALUE)
    {
        g_stRcParamSet.ucUlOutPwrFlag = HIGHER_FLAG; //����������ʵ�ƽ������Χ�ı�־
        g_stRcParamSet.uc3GUlOutPwrFlag = HIGHER_FLAG;
        MYALARM.stUlOutOverPwr.ucRcStatus = ALARM;  //������������ʸ澯
    }
    else if(TempData == LOWER_VALUE)
    {
        g_stRcParamSet.ucUlOutPwrFlag = LOWER_FLAG; //����������ʵ�ƽ������Χ�ı�־
        g_stRcParamSet.uc3GUlOutPwrFlag = LOWER_FLAG;
        MYALARM.stUlOutUnderPwr.ucRcStatus = ALARM;  //�������Ƿ���ʸ澯
    }
    else
    {
        g_stRcParamSet.sUlOutPwr = TempData; //3G:����������ʵ�ƽ��sint2�ͣ���λΪdBm������Ϊ10
        g_stRcParamSet.cUlOutPwr = TempData; //2G:����������ʵ�ƽ
        g_stRcParamSet.ucUlOutPwrFlag = NORMAL_FLAG; //����������ʵ�ƽ������Χ�ı�־
        g_stRcParamSet.uc3GUlOutPwrFlag = NORMAL_FLAG;
        //--------- ����Ϊ�澯�ж� ---------
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_3G)  //3G����ʱ����������澯�ж�
        {   //3G����ʱ����������澯�ж�
            if(g_stRcParamSet.sUlOutPwr > g_stSettingParamSet.sUlOutOverPwrThr) //3G:����������ʹ���������
            {
                MYALARM.stUlOutOverPwr.ucRcStatus = ALARM;  //������������ʸ澯
            }
            else if(g_stRcParamSet.sUlOutPwr < g_stSettingParamSet.sUlOutUnderPwrThr) //3G:�����������Ƿ��������
            {
                MYALARM.stUlOutUnderPwr.ucRcStatus = ALARM;  //�������Ƿ���ʸ澯
            }
        }
        else if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)   //2G����ʱ����������澯�ж�
        {
            if(g_stRcParamSet.cUlOutPwr > g_stSettingParamSet.cUlOutOverPwrThr) //����������ʹ���������
            {
                MYALARM.stUlOutOverPwr.ucRcStatus = ALARM;  //������������ʸ澯
            }
        }
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%% ����������ʵ�ƽ %%%%%%%%%%%%%%%%%%%%%%%%%%
    g_stRcParamSet.cDlOutPwr = 0; //����������ʵ�ƽ  sint1�ͣ���λΪdBm
    g_stRcParamSet.sDlOutPwr = 0; //3G:����������ʵ�ƽ��sint2�ͣ���λΪdBm������Ϊ10
    
    if(g_stSettingParamSet.ucRfSw == 0)   //��Ƶ���ر��ر�
    {
        g_stRcParamSet.ucDlOutPwrFlag = LOWER_FLAG; //����������ʵ�ƽ������Χ�ı�־
        g_stRcParamSet.uc3GDlOutPwrFlag = LOWER_FLAG;
    }
    else
    {
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)  //�����2G�豸���ͣ������2G�ļ��㺯��
        {
            TempData = CountDlPow2G(uiValueTemp[14]);
        }
        else               //�������3G�ļ��㺯��
        {
            TempData = CountPowOut3G(uiValueTemp[14], DownLoad);
        }

        MYALARM.stDlOutOverPwr.ucRcStatus = NOT_ALARM;  //ȡ��������������ʸ澯
        MYALARM.stDlOutUnderPwr.ucRcStatus = NOT_ALARM;  //ȡ���������Ƿ���ʸ澯
        if(TempData == HIGHER_VALUE)
        {
            g_stRcParamSet.ucDlOutPwrFlag = HIGHER_FLAG; //����������ʵ�ƽ������Χ�ı�־
            g_stRcParamSet.uc3GDlOutPwrFlag = HIGHER_FLAG;
            MYALARM.stDlOutOverPwr.ucRcStatus = ALARM;  //������������ʸ澯
        }
        else if(TempData == LOWER_VALUE)
        {
            g_stRcParamSet.ucDlOutPwrFlag = LOWER_FLAG; //����������ʵ�ƽ������Χ�ı�־
            g_stRcParamSet.uc3GDlOutPwrFlag = LOWER_FLAG;
            MYALARM.stDlOutUnderPwr.ucRcStatus = ALARM;  //�������Ƿ���ʸ澯
        }
        else
        {
            g_stRcParamSet.cDlOutPwr = TempData; //����������ʵ�ƽ  sint1�ͣ���λΪdBm
            g_stRcParamSet.sDlOutPwr = TempData; //3G:����������ʵ�ƽ��sint2�ͣ���λΪdBm������Ϊ10
            g_stRcParamSet.ucDlOutPwrFlag = NORMAL_FLAG; //����������ʵ�ƽ������Χ�ı�־
            g_stRcParamSet.uc3GDlOutPwrFlag = NORMAL_FLAG;

            //--------- ����Ϊ�澯�ж� ---------
            if(g_stDevTypeTable.ucGenType == GEN_TYPE_3G)  //3G����ʱ����������澯�ж�
            {   //3G����ʱ����������澯�ж�
                if(g_stRcParamSet.sDlOutPwr < g_stSettingParamSet.sDlOutUnderPwrThr) //�����������Ƿ��������
                {
                    MYALARM.stDlOutUnderPwr.ucRcStatus = ALARM;  //�������Ƿ���ʸ澯
                }
                else if(g_stRcParamSet.sDlOutPwr > g_stSettingParamSet.sDlOutOverPwrThr) //����������ʹ���������
                {
                    MYALARM.stDlOutOverPwr.ucRcStatus = ALARM;  //������������ʸ澯
                }
            }
            else if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)   //2G����ʱ����������澯�ж�
            {
                if(g_stRcParamSet.cDlOutPwr < g_stSettingParamSet.cDlOutUnderPwrThr) //�����������Ƿ��������
                {
                    MYALARM.stDlOutUnderPwr.ucRcStatus = ALARM;  //�������Ƿ���ʸ澯
                }
                else if(g_stRcParamSet.cDlOutPwr > g_stSettingParamSet.cDlOutOverPwrThr) //����������ʹ���������
                {
                    MYALARM.stDlOutOverPwr.ucRcStatus = ALARM;  //������������ʸ澯
                }
            }
        }
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%% ����������ʵ�ƽ(ͨ����) %%%%%%%%%%%%%%%%%%%%%%%%%%
    g_stRcParamSet.cDlOutPwr2 = 0; //����������ʵ�ƽ(ͨ����)  sint1�ͣ���λΪdBm
    g_stRcParamSet.sDlOutPwr2 = 0; //3G:����������ʵ�ƽ(ͨ����)��sint2�ͣ���λΪdBm������Ϊ10
    
    if(g_stSettingParamSet.ucRfSw == 0)   //��Ƶ���ر��ر�
    {
        g_stRcParamSet.ucDlOutPwrFlag2 = LOWER_FLAG; //����������ʵ�ƽ(ͨ����)������Χ�ı�־
        g_stRcParamSet.uc3GDlOutPwrFlag2 = LOWER_FLAG;
    }
    else
    {
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)  //�����2G�豸���ͣ������2G�ļ��㺯��
        {
            TempData = CountDlPow2GChunnel2(uiValueTemp[17]);
        }
        else               //�������3G�ļ��㺯��
        {
            TempData = CountPowOut3GChunnel2(uiValueTemp[17], DownLoad);
        }

        MYALARM.stDlOutOverPwr2.ucRcStatus = NOT_ALARM;  //ȡ���������������(ͨ����)�澯
        MYALARM.stDlOutUnderPwr2.ucRcStatus = NOT_ALARM;  //ȡ���������Ƿ����(ͨ����)�澯
        if(TempData == HIGHER_VALUE)
        {
            g_stRcParamSet.ucDlOutPwrFlag2 = HIGHER_FLAG; //����������ʵ�ƽ(ͨ����)������Χ�ı�־
            g_stRcParamSet.uc3GDlOutPwrFlag2 = HIGHER_FLAG;
            MYALARM.stDlOutOverPwr2.ucRcStatus = ALARM;  //������������ʸ澯(ͨ����)
        }
        else if(TempData == LOWER_VALUE)
        {
            g_stRcParamSet.ucDlOutPwrFlag2 = LOWER_FLAG; //����������ʵ�ƽ(ͨ����)������Χ�ı�־
            g_stRcParamSet.uc3GDlOutPwrFlag2 = LOWER_FLAG;
            MYALARM.stDlOutUnderPwr2.ucRcStatus = ALARM;  //�������Ƿ���ʸ澯(ͨ����)
        }
        else
        {
            g_stRcParamSet.cDlOutPwr2 = TempData; //����������ʵ�ƽ(ͨ����)  sint1�ͣ���λΪdBm
            g_stRcParamSet.sDlOutPwr2 = TempData; //3G:����������ʵ�ƽ(ͨ����)��sint2�ͣ���λΪdBm������Ϊ10
            g_stRcParamSet.ucDlOutPwrFlag2 = NORMAL_FLAG; //����������ʵ�ƽ(ͨ����)������Χ�ı�־
            g_stRcParamSet.uc3GDlOutPwrFlag2 = NORMAL_FLAG;

            //--------- ����Ϊ�澯�ж� ---------
            if(g_stDevTypeTable.ucGenType == GEN_TYPE_3G)  //3G����ʱ����������澯�ж�
            {   //3G����ʱ����������澯�ж�
                if(g_stRcParamSet.sDlOutPwr2 < g_stSettingParamSet.sDlOutUnderPwrThr2) //�����������Ƿ��������
                {
                    MYALARM.stDlOutUnderPwr2.ucRcStatus = ALARM;  //�������Ƿ���ʸ澯
                }
                else if(g_stRcParamSet.sDlOutPwr2 > g_stSettingParamSet.sDlOutOverPwrThr2) //����������ʹ���������
                {
                    MYALARM.stDlOutOverPwr2.ucRcStatus = ALARM;  //������������ʸ澯
                }
            }
            else if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)   //2G����ʱ����������澯�ж�
            {
                if(g_stRcParamSet.cDlOutPwr2 < g_stSettingParamSet.cDlOutUnderPwrThr2) //�����������Ƿ��������
                {
                    MYALARM.stDlOutUnderPwr2.ucRcStatus = ALARM;  //�������Ƿ���ʸ澯
                }
                else if(g_stRcParamSet.cDlOutPwr2 > g_stSettingParamSet.cDlOutOverPwrThr2) //����������ʹ���������
                {
                    MYALARM.stDlOutOverPwr2.ucRcStatus = ALARM;  //������������ʸ澯
                }
            }
        }
    }
 
    //%%%%%%%%%%%%%%%%%%%%%%%%%% ����פ����ֵ %%%%%%%%%%%%%%%%%%%%%%%%%%
    TempData = g_stYkppParamSet.stYkppRfParam.ucDlOutMax;  //��������������

    MYALARM.stDlSwr.ucRcStatus = NOT_ALARM;  //��ȡ������פ���ȸ澯
    
    if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)  //========= 2G�豸������פ������ ==========
    {
        if(g_stRcParamSet.ucDlOutPwrFlag == LOWER_FLAG) //����������ʵ��ڼ�ⷶΧʱ��פ��Ҳ���ص��ڼ�ⷶΧ
        {
            g_stRcParamSet.ucDlSwrFlag = LOWER_FLAG; //����פ����ֵ������Χ�ı�־
        }
        else if(g_stRcParamSet.cDlOutPwr > TempData)  //����������ʸ��ڶ�������
        {
            g_stRcParamSet.ucDlSwrFlag = HIGHER_FLAG; //����פ����ֵ������Χ�ı�־
        }
        else if(g_stRcParamSet.cDlOutPwr < (TempData - 35)) //����������ʵ��ڶ�������-35dB
        {
            g_stRcParamSet.ucDlSwrFlag = LOWER_FLAG; //����פ����ֵ������Χ�ı�־
        }
        else
        {
            //����פ����ֵ��UCHAR�ͣ�����Ϊ10
            g_stRcParamSet.ucDlSwr
                   = CountVSWR2G(uiValueTemp[6], (UINT32)(g_stRcParamSet.cDlOutPwr));
            g_stRcParamSet.ucDlSwrFlag = NORMAL_FLAG; //����פ����ֵ������Χ�ı�־
            //--------- ����Ϊ�澯�ж� ---------
            if(g_stRcParamSet.ucDlSwr > g_stSettingParamSet.ucDlSwrThr) //����פ��������
            {
                MYALARM.stDlSwr.ucRcStatus = ALARM;  //����פ���ȸ澯
            }
        }
    }
    else    //========= 3G�豸������פ������ ==========
    {
        if(g_stRcParamSet.ucDlOutPwrFlag == LOWER_FLAG) //����������ʵ��ڼ�ⷶΧʱ��פ��Ҳ���ص��ڼ�ⷶΧ
        {
            g_stRcParamSet.ucDlSwrFlag = LOWER_FLAG; //����פ����ֵ������Χ�ı�־
        }
        else if(g_stRcParamSet.sDlOutPwr > (TempData*10))  //����������ʸ��ڶ�������
        {
            g_stRcParamSet.ucDlSwrFlag = HIGHER_FLAG; //����פ����ֵ������Χ�ı�־
        }
        else if(g_stRcParamSet.sDlOutPwr < (TempData*10 - 350)) //����������ʵ��ڶ�������-35dB
        {
            g_stRcParamSet.ucDlSwrFlag = LOWER_FLAG; //����פ����ֵ������Χ�ı�־
        }
        else
        {
            //����פ����ֵ��UCHAR�ͣ�����Ϊ10
            g_stRcParamSet.ucDlSwr
                   = CountVSWR3G(uiValueTemp[6], (UINT32)(g_stRcParamSet.sDlOutPwr));
            g_stRcParamSet.ucDlSwrFlag = NORMAL_FLAG; //����פ����ֵ������Χ�ı�־
            //--------- ����Ϊ�澯�ж� ---------
            if(g_stRcParamSet.ucDlSwr > g_stSettingParamSet.ucDlSwrThr) //����פ��������
            {
                MYALARM.stDlSwr.ucRcStatus = ALARM;  //����פ���ȸ澯
            }
        }
    }
 
    //%%%%%%%%%%%%%%%%%%%%%%%%%% ����פ����ֵ(ͨ����) %%%%%%%%%%%%%%%%%%%%%%%%%%
    TempData = g_stYkppParamSet.stYkppRfParam.ucDlOutMax2;  //��������������

    MYALARM.stDlSwr2.ucRcStatus = NOT_ALARM;  //��ȡ������פ���ȸ澯
    
    if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)  //========= 2G�豸������פ������ ==========
    {
        if(g_stRcParamSet.ucDlOutPwrFlag2 == LOWER_FLAG) //����������ʵ��ڼ�ⷶΧʱ��פ��Ҳ���ص��ڼ�ⷶΧ
        {
            g_stRcParamSet.ucDlSwrFlag2 = LOWER_FLAG; //����פ����ֵ������Χ�ı�־
        }
        else if(g_stRcParamSet.cDlOutPwr2 > TempData)  //����������ʸ��ڶ�������
        {
            g_stRcParamSet.ucDlSwrFlag2 = HIGHER_FLAG; //����פ����ֵ������Χ�ı�־
        }
        else if(g_stRcParamSet.cDlOutPwr2 < (TempData - 35)) //����������ʵ��ڶ�������-35dB
        {
            g_stRcParamSet.ucDlSwrFlag2 = LOWER_FLAG; //����פ����ֵ������Χ�ı�־
        }
        else
        {
            //����פ����ֵ��UCHAR�ͣ�����Ϊ10
            g_stRcParamSet.ucDlSwr2
                   = CountVSWR2GChunnel2(uiValueTemp[21], (UINT32)(g_stRcParamSet.cDlOutPwr2));
            g_stRcParamSet.ucDlSwrFlag2 = NORMAL_FLAG; //����פ����ֵ������Χ�ı�־
            //--------- ����Ϊ�澯�ж� ---------
            if(g_stRcParamSet.ucDlSwr2 > g_stSettingParamSet.ucDlSwrThr2) //����פ��������
            {
                MYALARM.stDlSwr2.ucRcStatus = ALARM;  //����פ���ȸ澯
            }
        }
    }
    else    //========= 3G�豸������פ������ ==========
    {
        if(g_stRcParamSet.ucDlOutPwrFlag2 == LOWER_FLAG) //����������ʵ��ڼ�ⷶΧʱ��פ��Ҳ���ص��ڼ�ⷶΧ
        {
            g_stRcParamSet.ucDlSwrFlag2 = LOWER_FLAG; //����פ����ֵ������Χ�ı�־
        }
        else if(g_stRcParamSet.sDlOutPwr2 > (TempData*10))  //����������ʸ��ڶ�������
        {
            g_stRcParamSet.ucDlSwrFlag2 = HIGHER_FLAG; //����פ����ֵ������Χ�ı�־
        }
        else if(g_stRcParamSet.sDlOutPwr2 < (TempData*10 - 350)) //����������ʵ��ڶ�������-35dB
        {
            g_stRcParamSet.ucDlSwrFlag2 = LOWER_FLAG; //����פ����ֵ������Χ�ı�־
        }
        else
        {
            //����פ����ֵ��UCHAR�ͣ�����Ϊ10
            g_stRcParamSet.ucDlSwr2
                   = CountVSWR3GChunnel2(uiValueTemp[21], (UINT32)(g_stRcParamSet.sDlOutPwr2));
            g_stRcParamSet.ucDlSwrFlag2 = NORMAL_FLAG; //����פ����ֵ������Χ�ı�־
            //--------- ����Ϊ�澯�ж� ---------
            if(g_stRcParamSet.ucDlSwr2 > g_stSettingParamSet.ucDlSwrThr2) //����פ��������
            {
                MYALARM.stDlSwr2.ucRcStatus = ALARM;  //����פ���ȸ澯
            }
        }
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%% �����¶� %%%%%%%%%%%%%%%%%%%%%%%%%%
    uiValueTemp[4] = uiValueTemp[4] > uiValueTemp[12] ? uiValueTemp[4] : uiValueTemp[12];
    uiValueTemp[4] = uiValueTemp[4] > uiValueTemp[8] ? uiValueTemp[4] : uiValueTemp[8];
    uiValueTemp[4] = uiValueTemp[4] > uiValueTemp[10] ? uiValueTemp[4] : uiValueTemp[10];
    uiValueTemp[4] = uiValueTemp[4] > uiValueTemp[20] ? uiValueTemp[4] : uiValueTemp[20];
    uiValueTemp[4] = uiValueTemp[4] > uiValueTemp[22] ? uiValueTemp[4] : uiValueTemp[22];
    if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G)  //�����2G�豸���ͣ������2G�ļ��㺯��
    {
        g_stRcParamSet.cPaTemp = CountTem2G(uiValueTemp[4]); //�����¶�ֵ��sint1�ͣ���λΪ�棬��鹦��ȡ���ֵ
    }
    else
    {
        g_stRcParamSet.cPaTemp = CountTem3G(uiValueTemp[4]); //�����¶�ֵ��sint1�ͣ���λΪ�棬��鹦��ȡ���ֵ
    }
    if(g_stRcParamSet.cPaTemp > g_stSettingParamSet.cPaOverheatThr)  //���Ź��¶ȸ澯����
    {
        MYALARM.stPaOverheat.ucRcStatus = ALARM;  //���Ź��¸澯
    }
    else 
    {
        MYALARM.stPaOverheat.ucRcStatus = NOT_ALARM;  //ȡ�����Ź��¸澯
    }
    //%%%%%%%%%%%%%%%%%%%%%%%%%% ������������ %%%%%%%%%%%%%%%%%%%%%%%%%%
    //������������  sint1�ͣ���λΪdB (�����������-����˥��ֵ)
    //MCM-17_20061106_linyu_begin
    g_stRcParamSet.cUlTheGain  = g_stYkppParamSet.stYkppRfParam.ucUlGainMax - g_stSettingParamSet.ucUlAtt;  
    //MCM-17_20061106_linyu_end
    g_stRcParamSet.cUlTheGain2 = g_stYkppParamSet.stYkppRfParam.ucUlGainMax - g_stSettingParamSet.ucUlAtt2;  
    //       ������˥�Ļ����������������ʱ��Ҫ�޸������������档ƽʱ����

    //%%%%%%%%%%%%%%%%%%%%%%%%%% ����ʵ������ %%%%%%%%%%%%%%%%%%%%%%%%%%
    if((g_stRcParamSet.ucDlInPwrFlag == LOWER_FLAG)
            || (g_stRcParamSet.ucDlOutPwrFlag == LOWER_FLAG) )
    {//������������������ʵ��ڼ�ⷶΧ�Ļ�
        g_stRcParamSet.ucDlActGainFlag = LOWER_FLAG; //����ʵ�����泬����Χ�ı�־
    }
    else if((g_stRcParamSet.ucDlInPwrFlag == HIGHER_FLAG)
            || (g_stRcParamSet.ucDlOutPwrFlag == HIGHER_FLAG))
    {//������������������ʸ��ڼ�ⷶΧ�Ļ�
        g_stRcParamSet.ucDlActGainFlag = HIGHER_FLAG; //����ʵ�����泬����Χ�ı�־
    }
    else
    {
        //�������롢������ʶ�������������ʵ������=�������-�������롣
        //MCM-39_20061120_linwei_begin
        g_stRcParamSet.ucDlActGainFlag = NORMAL_FLAG; //����ʵ�����泬����Χ�ı�־
        if(g_stDevTypeTable.ucGenType == GEN_TYPE_2G) //2G�豸������ʵ���������
        {
            g_stRcParamSet.cDlActGain = (g_stRcParamSet.cDlOutPwr)
                        - (g_stRcParamSet.cDlInPwr);
        }
        else //3G�豸������ʵ���������
        {
            g_stRcParamSet.cDlActGain = (g_stRcParamSet.sDlOutPwr + 5) / 10
                        - g_stRcParamSet.cDlInPwr;        
        }
        //MCM-39_20061120_linwei_end
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%% ��Դ�仯�澯 %%%%%%%%%%%%%%%%%%%%%%%%%%
    //USHORT usSsCi;  //��Դ��Ϣ��С��ʶ����ʵʱֵ
    //USHORT usSrcCellId;  //��ԴС��ʶ�������ֵ
    if(g_stRcParamSet.usSsCi == g_stSettingParamSet.usSrcCellId) //��ԴС��ʶ�������ֵ
    {
        MYALARM.stSrcSigChanged.ucRcStatus = NOT_ALARM; //ȡ����Դ�仯�澯
    }
    else
    {
        MYALARM.stSrcSigChanged.ucRcStatus = ALARM; //��Դ�仯�澯
    }

    //%%%%%%%%%%%%%%%%%%%%%%% ���ģ���ع��ϸ澯�ж� %%%%%%%%%%%%%%%%%%%%%%%
    //UCHAR ucTempType;  //�豸����
    //(uiValueTemp[11] / 1024)*2.5*6*100  �õ���ص�ѹ��100����ֵ
    uiValueTemp[3] = (uiValueTemp[3] * 5 * 3 * 25 + 128) / 256; //"+128"Ϊ����������
    g_stRcParamSet.usModuleBatValue = uiValueTemp[3] + 10;  //"+10"��Ϊ��У׼һ��

    //%%%%%%%%%%%%%%%%%%%%%%% ��Դ���ϸ澯�ж� %%%%%%%%%%%%%%%%%%%%%%%
    // uiValueTemp[9]  12V���  ����9VΪ����
    // uiValueTemp[7]  27V���  ����20VΪ����
    
    MYALARM.stPwrFault.ucRcStatus = NOT_ALARM; //��Դ���ϸ澯
    if(uiValueTemp[9] < 153)
    {
        MYALARM.stPwrFault.ucRcStatus = ALARM; //��Դ���ϸ澯
    }
    //zhangjie 20071120 ̫�������ص�ѹ
    //�����ѹֵ ��ѹֵ���32.5v����Ӧ��g_stRcParamSet.usSecBatVol��ֵΪ3250����Ӧ��AD��ֵ��uiValueTemp[1]= 1028
    //5v��ӦuiValueTemp[1]= 158
    g_stRcParamSet.usSecBatVol = uiValueTemp[1]*500/158;
    //zhangjie 20071120 ̫�������ص�ѹ
}
