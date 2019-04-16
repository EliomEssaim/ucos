#ifndef BootH
#define BootH

#include "UserType.h"

#define FPD_FILE_NAME_LEN  20
#define FPD_COUNT          3
#define TIME_STAMP_LEN     7
#define FILE_VERSION_LEN   20

#define FILE_OLD           0//��ʾ�ļ�Ϊ�ɣ���ucNew��ֵ
#define FILE_NEW           1//��ʾ�ļ�Ϊ�����ض�ucNew��ֵ

//�ļ����������ṹ��file partition descriptor
typedef struct
{
    CHAR  acFileName[FPD_FILE_NAME_LEN];     //�ļ���
    UCHAR aucTimeStamp[TIME_STAMP_LEN];      //�ļ�������ʱ��
    CHAR  acVersion[FILE_VERSION_LEN];       //�ļ��汾
    UCHAR ucFileType;                        //�ļ����ͣ������ļ��������ļ�
    UCHAR ucStatus;                          //�ļ�״̬
    UCHAR ucNew;                             //�ļ�״̬��ͬʱ1��ʾ�£�0��ʾ��
    ULONG ulFileSize;                        //�ļ���С
    ULONG ulFileAddr;                        //�ļ���ŵ�λ��
    UCHAR aucFileId[20];                     //CMCC�����ļ���ID
    UCHAR ucIsRunning;                       //�Ƿ����б�־��1:�������У�0:δ����
}FILE_PART_DESC_ST;

#define PROGRAM_STATUS_INVALID      0 //��ʾ������Ч
#define PROGRAM_STATUS_RUNNABLE     1 //��ʾ������Գɹ�����
#define PROGRAM_STATUS_VALID        2 //��ʾ������Ч��׼������
#define PROGRAM_STATUS_UPGRADE      3 //��ʾ���������������ǻ�û�ɹ�
#define PROGRAM_STATUS_UNRUNNABLE   4 //��ʾ��������ʧ��

#define PROGRAM_PART_1_IDX          0 //�����±�
#define PROGRAM_PART_2_IDX          1
#define DATA_PART_1_IDX             2

#define PROGRAM_PART_1_ADDR  (0x1010000) //��FLASH�еĵ�ַ
#define PROGRAM_PART_2_ADDR  (0x1090000)
#define FILE_PART_TBL_ADDR   (0x1183800)//�ļ�������FLASH�ĵ�ַ����CMCC_1_Ext.h�ж���

extern FILE_PART_DESC_ST g_astFilePartTbl[FPD_COUNT];
//extern ULONG g_ulP1Idx; //����ָʾ�����������
//extern ULONG g_ulP2Idx;
extern ULONG g_ulUpgradePartIdx; //�����ļ���ŵķ����±�



#endif