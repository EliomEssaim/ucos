/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   Key.h
    ����:     ����
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  
       ����:  ����
       ����:  �����д���
---------------------------------------------------------------------------*/
#ifndef KeyH
#define KeyH


/*****************************************/

#define KEY_BASE ((UCHAR *)0x04000000)
#define KEY_MASK 0x0F

//ɨ�赽�İ���״̬
#define KEY_STATUS_SINGLE      0x01
#define KEY_STATUS_COMPOUNDING 0x02
#define KEY_STATUS_LONGPRESS   0x03

//��һ��ֵ����
#define KEY_VALUE_OK     0x0800 //���ܼ���ȷ����
#define KEY_VALUE_CANCEL 0x0008 //���ܼ������ء�
#define KEY_VALUE_UP     0x8000 //���ܼ�������
#define KEY_VALUE_DOWN   0x0080 //���ܼ�������
#define KEY_VALUE_LEFT   0x0001 //���ܼ��� <��
#define KEY_VALUE_RIGHT  0x0004 //���ܼ��� >��

#define KEY_VALUE_0  0x0002 //���֡�0��
#define KEY_VALUE_1  0x1000 //���֡�1��
#define KEY_VALUE_2  0x2000 //���֡�2��
#define KEY_VALUE_3  0x4000 //���֡�3��
#define KEY_VALUE_4  0x0100 //���֡�4��
#define KEY_VALUE_5  0x0200 //���֡�5��
#define KEY_VALUE_6  0x0400 //���֡�6��
#define KEY_VALUE_7  0x0010 //���֡�7��
#define KEY_VALUE_8  0x0020 //���֡�8��
#define KEY_VALUE_9  0x0040 //���֡�9��

//��ϼ�ֵ����
#define KEY_VALUE_A  KEY_VALUE_LEFT+KEY_VALUE_1 //Ӣ�ġ�A��
#define KEY_VALUE_B  KEY_VALUE_LEFT+KEY_VALUE_2 //Ӣ�ġ�B��
#define KEY_VALUE_C  KEY_VALUE_LEFT+KEY_VALUE_3 //Ӣ�ġ�C��
#define KEY_VALUE_D  KEY_VALUE_LEFT+KEY_VALUE_4 //Ӣ�ġ�D��
#define KEY_VALUE_E  KEY_VALUE_LEFT+KEY_VALUE_5 //Ӣ�ġ�E��
#define KEY_VALUE_F  KEY_VALUE_LEFT+KEY_VALUE_6 //Ӣ�ġ�F��

#define KEY_VALUE_STA_INIT         KEY_VALUE_LEFT+KEY_VALUE_7 //��վ�ϱ�
#define KEY_VALUE_INSPECTION       KEY_VALUE_LEFT+KEY_VALUE_8 //Ѳ���ϱ�
#define KEY_VALUE_REPAIR_CONFIRMED KEY_VALUE_LEFT+KEY_VALUE_9 //�޸��ϱ�
#define KEY_VALUE_CONFIG_CHANGED   KEY_VALUE_LEFT+KEY_VALUE_0 //���ñ���ϱ�

#define KEY_VALUE_OLED_OPEN  KEY_VALUE_LEFT+KEY_VALUE_RIGHT //��ʾ������


#endif


