#ifndef LampH
#define LampH

#define FLICKER 1       //֧����˸
#define NOT_FLICKER 0   //��֧����˸

/*****************************************/
// �澯�źŵƲ���ԭSETATT��18�������
// ���Խ��ṹ��������
/*****************************************/
typedef struct
{
    UCHAR ucPioNum;           //Ӳ���ܽ�
    UCHAR ucGroupNum;         //����SETATT����ĵڼ���
    UCHAR ucLampStatus;       //�Ƶ�����״̬ 0�� 1�� 2��˸
    UCHAR ucIsItFlicker;      //�Ƿ�֧����˸��1֧�֣�0��֧��
    UCHAR ucNowStatus;        //�Ƶĵ�ǰ״̬�����Ƶ�״̬Ϊ��˸ʱ��Ч
}POI_SIGNAL_LAMP_ST;

typedef struct
{
    ULONG ulPioNum;           //Ӳ���ܽ�
    UCHAR ucLampStatus;       //�Ƶ�����״̬
    UCHAR ucIsItFlicker;      //�Ƿ�֧����˸��1֧�֣�0��֧��
    UCHAR ucNowStatus;        //�Ƶĵ�ǰ״̬�����Ƶ�״̬Ϊ��˸ʱ��Ч
}SIGNAL_LAMP_ST;

/*****************************************/
void LampTask(void);

extern void SetReportLampStatus(UCHAR ucLampSta);  //�޸��ϱ�״ָ̬ʾ�Ƶĵ�ǰ״̬������������ִ��

#endif
