/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   CMCC_1_Ext.h
    ����:     ����
    �ļ�˵��: ���ļ�����ʵ�ֶ�ʱ�����ƵĴ���
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  2006/02/16
       ����:  ����
       ����:  �������
    2. ����:  2006/11/01
       ����:  �½�
       ����:  �޸�����MCM-11��
              ���ӻ������ñ��ڳ����ʼ��ʱ���ݻ��������������ñ�
              �еĸ�����������á�
    3. ����:  2006/11/01
       ����:  �½�
       ����:  �޸�����MCM-5��
              �����ϱ�ͨ�ŷ�ʽ��ֵ�ĺ궨�壬������ͨ�ŷ�ʽ��ֵ�ĺ궨�������������
    4. ����:  2006/11/3
       ����:  �ӻ���
       ����:  �޸�����MCM-6,�޸����ò�����ID"0x045A-0x045E"��������ز������õ����
    5. ����:  2006/11/06
       ����:  ����
       ����:  �޸�����MCM-17��
              �����д�ͨ���ŵ�ͨ��1�Ĳ�����ӳ���Ӧ�ĵ�����ͨ���ŵĲ����ϣ������豸
              �Ƕ�ͨ��ʱ����ѯͨ��1�Ĳ���ʵ�ʷ��ʵ��Ƕ�Ӧ�Ĳ���ͨ���ŵĲ����ĵ�ַ��
    6. ����:  2006/11/7
       ����:  �½�
       ����:  �޸�����MCM-21��
              �޸ı��󣬰Ѻ궨���е�FIRELESSCOUPL ��ΪWIRELESSCOUPL
    7. ����:  2006/11/8
       ����:  �½�
       ����:  �޸�����MCM-23��
              ���Ӳ����б�ȫ�ֱ����п������ɲ����ĸ�������Ϊ���Ի��͵Ĳ������ѳ�����ԭ��������
    8. ����:  2006/11/8
       ����:  �ӻ���
       ����:  �޸�����MCM-25��
              �����澯�����ṹ���������ӳ�Ա��λ�ã�ʹ���и澯��Ĳ���������ID��С�����˳�����С�
    9. ����:  2006/11/29
       ����:  ����
       ����:  �޸�����MCM-42��
              ����ͨ���������������פ������
    10.����:  2007/03/15
       ����:  �ӻ���
       ����:  �޸�����MCM-63,
              ����ʣ��δ����2g���͵ĺ궨��
    11. ����:  2007/11/20
        ����:  �½�
        ����:  �޸�����MCM-80��              
               �����豸����ѡ��̫�������ص�ѹ���ļ�ⷽʽ��
---------------------------------------------------------------------------*/
#ifndef Cmcc_1_ExtH
#define Cmcc_1_ExtH

#include "UserType.h"

#ifdef BIG_ENDIAN //Ĭ��ΪLittle Endian
#define ADJUST_WORD(word)  ((word) >> 8 | ((word) & 0xFF) << 8) //�ߵ��ֽڶԵ�
#define ADJUST_DWORD(dword) ((((dword) >> 24) & 0xFF) | (((dword) << 24) & 0xFF000000) \
                           | (((dword) >> 8) & 0xFF00) | (((dword) << 8) & 0xFF0000)) //1��4�ֽڶԵ���2��3�ֽڶԵ�

#define GET_WORD_1ST_BYTE(word)     (((word) >> 8) & 0xFF)
#define GET_WORD_2ND_BYTE(word)     ((word) & 0xFF)
#define GET_DWORD_1ST_BYTE(dword)   (((dword) >> 24) & 0xFF)
#define GET_DWORD_2ND_BYTE(dword)   (((dword) >> 16) & 0xFF)
#define GET_DWORD_3RD_BYTE(dword)   (((dword) >> 8) & 0xFF)
#define GET_DWORD_4TH_BYTE(dword)   ((dword) & 0xFF)

#define GET_WORD(ptr)   ((*(UCHAR *)(ptr) << 8) + *((UCHAR *)(ptr) + 1))
#define SET_WORD(ptr, word)  (*(UCHAR *)(ptr) = GET_WORD_2ND_BYTE((word)), \
                              *((UCHAR *)(ptr) + 1) = GET_WORD_1ST_BYTE((word)))

#define GET_DWORD(ptr) ((*(UCHAR *)(ptr) << 24) \
                      + (*((UCHAR *)(ptr) + 1) << 16) \
                      + (*((UCHAR *)(ptr) + 2) << 8) \
                      +  *((UCHAR *)(ptr) + 3))

#define SET_DWORD(ptr, dword)   (*(UCHAR *)(ptr) = GET_DWORD_4TH_BYTE((dword)), \
                                 *((UCHAR *)(ptr) + 1) = GET_DWORD_3RD_BYTE((dword)), \
                                 *((UCHAR *)(ptr) + 2) = GET_DWORD_2ND_BYTE((dword)), \
                                 *((UCHAR *)(ptr) + 3) = GET_DWORD_1ST_BYTE((dword)))


#else
#define ADJUST_WORD(word)  (word)
#define ADJUST_DWORD(dword) (dword)

#define GET_WORD_1ST_BYTE(word)     ((word) & 0xFF)
#define GET_WORD_2ND_BYTE(word)     (((word) >> 8) & 0xFF)
#define GET_DWORD_1ST_BYTE(dword)   ((dword) & 0xFF)
#define GET_DWORD_2ND_BYTE(dword)   (((dword) >> 8) & 0xFF)
#define GET_DWORD_3RD_BYTE(dword)   (((dword) >> 16) & 0xFF)
#define GET_DWORD_4TH_BYTE(dword)   (((dword) >> 24) & 0xFF)

#define GET_WORD(ptr)  (*(UCHAR *)(ptr) + (*((UCHAR *)(ptr) + 1) << 8))
#define SET_WORD(ptr, word)  (*(UCHAR *)(ptr) = GET_WORD_1ST_BYTE((word)), \
                              *((UCHAR *)(ptr) + 1) = GET_WORD_2ND_BYTE((word)))
#define GET_DWORD(ptr) (*(UCHAR *)(ptr) \
                     + (*((UCHAR *)(ptr) + 1) << 8) \
                     + (*((UCHAR *)(ptr) + 2) << 16) \
                     + (*((UCHAR *)(ptr) + 3) << 24))

#define SET_DWORD(ptr, dword)   (*(UCHAR *)(ptr) = GET_DWORD_1ST_BYTE((dword)), \
                                 *((UCHAR *)(ptr) + 1) = GET_DWORD_2ND_BYTE((dword)), \
                                 *((UCHAR *)(ptr) + 2) = GET_DWORD_3RD_BYTE((dword)), \
                                 *((UCHAR *)(ptr) + 3) = GET_DWORD_4TH_BYTE((dword)))

#endif


#define IGNORE              FAILED
#define FORWARD_UNDESTATE   1
#define SUCCEEDED_DEBUSTATE 2  
#define NEED_TO_FORWARD     3
#define NEED_MORE_OP        4


//AP����ʼ������־
#define APAC_FLAG       0x7E    //APA,APC��ʼ������־ ~
#define APB_FLAG        0x21    //APB��ʼ������־ !

//MCM-5_20061101_zhangjie_begin
//ͨ�ŷ�ʽ
#define COMM_NO_CONN                1 //��������CSD����GPRS��ʽʱ��Ϊ���ڸ÷�ʽ����ʱ�κ��ϱ����ɷ���
#define COMM_MODE_SMS               1
#define COMM_MODE_GPRS              2
#define COMM_MODE_CSD               0xEE //�Զ���ͨ�ŷ�ʽ

//�ϱ�ͨ�ŷ�ʽ
#define COMM_REPORT_NO_CONN                1 //2G��ʽ��ʹ��
#define COMM_REPORT_MODE_SMS               1
#define COMM_REPORT_MODE_GPRS              3
#define COMM_REPORT_MODE_CSD               2 
//MCM-5_20061101_zhangjie_end

//��������
#define STR_LEN         20
#define LONG_STR_LEN    40
#define DEV_MODEL_LEN   STR_LEN
#define DEV_SN_LEN      STR_LEN
#define LONGITUDE_LEN   STR_LEN
#define LATITUDE_LEN    STR_LEN
//#define MOBJ_TBL_LEN 314*2
#define SFWR_VER_LEN    STR_LEN
#define TEL_NUM_LEN     STR_LEN
#define IP_LEN          4
#define DATE_TIME_LEN   7
#define CRC_LEN         2 //CRCУ���볤��
//#define STA_NUM_LEN     4
//#define DEV_NUM_LEN     1
#define IP_ADDR_LEN     4


//***************��ض����ʶ***************
//�豸��Ϣ
#define MOBJ_ID_DEV_INFO_SET        0       //����ID�ĸ��ֽ�
#define MOBJ_ID_DI_MNFT_ID          0x02    //�豸���̴��� 
#define MOBJ_ID_DI_DEV_TYPE         0x03    //�豸���
#define MOBJ_ID_DI_DEV_MODEL        0x04    //�豸�ͺ�
#define MOBJ_ID_DI_DEV_SN           0x05    //�豸�������к�
#define MOBJ_ID_DI_CH_COUNT         0x06    //�豸��ʵ���ŵ�����
#define MOBJ_ID_DI_LONGITUDE        0x07    //����
#define MOBJ_ID_DI_LATITUDE         0x08    //γ��
#define MOBJ_ID_DI_MOBJ_TABLE       0x09    //�豸�ļ�ز����б�
#define MOBJ_ID_DI_SFWR_VER         0x0A    //��ذ汾��Ϣ
#define MOBJ_ID_DI_DEV_MODEL2       0x0B    //3G:�豸�ͺ�
#define MOBJ_ID_DI_RUNNING_MODE     0x10    //3G:Ƕ��ʽ�������ģʽ
#define MOBJ_ID_DI_MAX_APC_LEN      0x11    //3G:��֧��AP��CЭ�����󳤶�
#define MOBJ_ID_DI_MCPB_MODE        0x12    //3G:MCP��B���õĽ�������
#define MOBJ_ID_DI_NC               0x13    //3G:����ϵ����NC��
#define MOBJ_ID_DI_T1               0x14    //3G:�豸��Ӧ��ʱ��TOT1��
#define MOBJ_ID_DI_TG               0x15    //3G:���ͼ��ʱ�䣨TG������������NC��1ʱ�������壩
#define MOBJ_ID_DI_TP               0x16    //3G:��ͣ����ȴ�ʱ�䣨TP��
#define MOBJ_ID_DI_TIME_TO_SWITCH   0x17    //3G:ת�����������ģʽʱ��OMC��Ҫ�ȴ���ʱ��
#define MOBJ_ID_DI_UPGRADE_RESULT   0x18    //3G:�豸ִ����������Ľ��
#define MOBJ_ID_DI_UPGRADE_MODE     0x20    //3G:�豸ʹ�õ�Զ��������ʽ

//0x0030��0x009F    ϵͳ����
//0x00A0��0x00FF    �����Զ���

//------------------------------����Ϊ̫���ܿ�������������-----------------------------------
//Solar Energy Controller
#define MOBJ_ID_DI_SEC_MNFT_ID          0xC0 //̫�����豸���̴���
#define MOBJ_ID_DI_SEC_DEV_TYPE         0xC1 //̫�����豸���
#define MOBJ_ID_DI_SEC_DEV_MODEL        0xC2 //̫�����豸�ͺ�
#define MOBJ_ID_DI_SEC_SFWR_VER         0xC3 //̫���ܼ�ذ汾��Ϣ
#define MOBJ_ID_DI_SEC_DEV_SN           0xC4 //̫�����豸�������к�
#define MOBJ_ID_DI_SEC_SB_MODEL         0xC5 //����ͺ�
#define MOBJ_ID_DI_SEC_SB_SPEC          0xC6 //�����
#define MOBJ_ID_DI_SEC_SB_CNT           0xC7 //�������
#define MOBJ_ID_DI_SEC_SB_DIR           0xC8 //����淽λ��
#define MOBJ_ID_DI_SEC_SB_PITCH_ANGLE   0xC9 //����温����
#define MOBJ_ID_DI_SEC_ST_BAT_MODEL     0xCA //�����ͺ�
#define MOBJ_ID_DI_SEC_ST_BAT_SPEC      0xCB //���ع��
#define MOBJ_ID_DI_SEC_ST_BAT_CNT       0xCC //��������
#define MOBJ_ID_DI_SEC_STD_SLMT_PER_DAY 0xCD //�վ���׼����ʱ��standar sunlight mean time per day
#define MOBJ_ID_DI_SEC_LONGITUDE        0xCE //̫���ܵ�Դվ�㾭��
#define MOBJ_ID_DI_SEC_LATITUDE         0xCF //̫���ܵ�Դվ��γ��
#define MOBJ_ID_DI_SEC_ALTITUDE         0xD0 //̫���ܵ�Դվ�㺣��
//------------------------------����Ϊ̫���ܿ�������������-----------------------------------


//�豸��Ϣ��չ��������д���������豸��Ϣͬ
#define MOBJ_ID_DEV_INFO_SET_EX     0x0A    //����ID�ĸ��ֽ�


//���ܲ���
#define MOBJ_ID_NM_PARAM_SET        0x01    //����ID�ĸ��ֽ�
#define MOBJ_ID_NM_STA_NUM          0x01    //վ����
#define MOBJ_ID_NM_DEV_NUM          0x02    //�豸���
#define MOBJ_ID_NM_SMC_ADDR         0x10    //���ŷ������ĺ���
#define MOBJ_ID_NM_QNS_TEL_NUM_1    0x11    //��ѯ/���õ绰����1��5
#define MOBJ_ID_NM_QNS_TEL_NUM_2    0x12
#define MOBJ_ID_NM_QNS_TEL_NUM_3    0x13
#define MOBJ_ID_NM_QNS_TEL_NUM_4    0x14
#define MOBJ_ID_NM_QNS_TEL_NUM_5    0x15
#define MOBJ_ID_NM_REPORT_TEL_NUM   0x20    //�ϱ�����
#define MOBJ_ID_NM_EMS_IP_ADDR      0x30    //�������IP��ַ��IP v4��
#define MOBJ_ID_NM_EMS_PORT_NUM     0x31    //�������IP��ַ�˿ں�
#define MOBJ_ID_NM_USE_GPRS         0x32    //3G:�豸�Ƿ�ʹ��GPRS��ʽ
#define MOBJ_ID_NM_GPRS_APN         0x33    //3G:GPRS��������ƣ�����APN��
#define MOBJ_ID_NM_HEARTBEAT_PERIOD 0x34    //3G:�豸�����������ʱ��
#define MOBJ_ID_NM_DEV_TEL_NUM      0x35    //3G:�豸�ĵ绰����
#define MOBJ_ID_NM_GPRS_USER_ID     0x36    //3G:GPRS�������û���ʶ
#define MOBJ_ID_NM_GPRS_PWD         0x37    //3G:GPRS����������
#define MOBJ_ID_NM_REPORT_COMM_MODE 0x40    //�ϱ�ͨ�ŷ�ʽ
#define MOBJ_ID_NM_REPORT_TYPE      0x41    //�ϱ�����
#define MOBJ_ID_NM_COMM_MODE        0x42    //ͨ�ŷ�ʽ                         //�淶20060810
#define MOBJ_ID_NM_DATE_TIME        0x50    //���ڡ�ʱ��
#define MOBJ_ID_NM_FTP_SERVER_IP_ADDR   0x60 //3G:FTP������IP��ַ��IP v4��
#define MOBJ_ID_NM_FTP_SERVER_PORT_NUM  0x61 //3G:FTP������IP��ַ�˿ں�
#define MOBJ_ID_NM_FTP_USER_ID      0x62    //3G:FTP�����������û���
#define MOBJ_ID_NM_FTP_PWD          0x63    //3G:FTP���������Ŀ���
#define MOBJ_ID_NM_FTP_PATH         0x64    //3G:���·��
#define MOBJ_ID_NM_FILE_NAME        0x65    //3G:�ļ���

//0x0170��0x019F    ϵͳ����    
//0x01A0��0x01FF    �����Զ��� 
//------------------------------����Ϊ̫���ܿ�������������-----------------------------------
#define MOBJ_ID_NM_SEC_STA_NUM      0xCO

//------------------------------����Ϊ̫���ܿ�������������-----------------------------------


//�澯ʹ�ܺ͸澯״̬
#define MOBJ_ID_ALARM_ENA_SET       0x02    //����ID�ĸ��ֽ�
#define MOBJ_ID_ALARM_STATUS_SET    0x03    //����ID�ĸ��ֽ�

#define MOBJ_ID_A_PWR_LOST          0x01    //��Դ����澯
#define MOBJ_ID_A_PWR_FAULT         0x02    //��Դ���ϸ澯
#define MOBJ_ID_A_SOLAR_BAT_LOW_PWR 0x03    //̫���ܵ�ص͵�ѹ�澯
#define MOBJ_ID_A_BAT_FAULT         0x04    //���ģ���ع��ϸ澯
#define MOBJ_ID_A_POSITION          0x05    //λ�ø澯
#define MOBJ_ID_A_PA_OVERHEAT       0x06    //���Ź��¸澯
#define MOBJ_ID_A_SRC_SIG_CHANGED   0x07    //��Դ�仯�澯
#define MOBJ_ID_A_OTHER_MODULE      0x08    //����ģ��澯
#define MOBJ_ID_A_OSC_UNLOCKED      0x09    //����ʧ���澯
#define MOBJ_ID_A_UL_LNA            0x0A    //���е���Ź��ϸ澯
#define MOBJ_ID_A_DL_LNA            0x0B    //���е���Ź��ϸ澯
#define MOBJ_ID_A_UL_PA             0x0C    //���й��Ÿ澯
#define MOBJ_ID_A_DL_PA             0x0D    //���й��Ÿ澯
#define MOBJ_ID_A_OPT_MODULE        0x0E    //���շ�ģ����ϸ澯
#define MOBJ_ID_A_MS_LINK           0x0F    //���Ӽ����·�澯
#define MOBJ_ID_A_DL_IN_OVER_PWR    0x10    //������������ʸ澯
#define MOBJ_ID_A_DL_IN_UNDER_PWR   0x11    //��������Ƿ���ʸ澯
#define MOBJ_ID_A_DL_OUT_OVER_PWR   0x12    //������������ʸ澯
#define MOBJ_ID_A_DL_OUT_UNDER_PWR  0x13    //�������Ƿ���ʸ澯
#define MOBJ_ID_A_DL_SWR            0x14    //����פ���ȸ澯
#define MOBJ_ID_A_UL_OUT_OVER_PWR   0x15    //������������ʸ澯
#define MOBJ_ID_A_UL_OUT_UNDER_PWR  0x16    //�������Ƿ���ʸ澯//�淶20060810
#define MOBJ_ID_A_UL_SWR            0x17    //����פ���ȸ澯    //�淶20060810
#define MOBJ_ID_A_EXT_1             0x20    //�ⲿ�澯1��8
#define MOBJ_ID_A_EXT_2             0x21
#define MOBJ_ID_A_EXT_3             0x22
#define MOBJ_ID_A_EXT_4             0x23
#define MOBJ_ID_A_EXT_5             0x24
#define MOBJ_ID_A_EXT_6             0x25
#define MOBJ_ID_A_EXT_7             0x26
#define MOBJ_ID_A_EXT_8             0x27

#define MOBJ_ID_A_DOOR              0x28    //�Ž��澯
#define MOBJ_ID_A_SELF_OSC          0x29    //�Լ��澯
#define MOBJ_ID_A_GPRS_LOGIN_FAILED 0x2A    //3G:GPRS��¼ʧ�ܸ澯

#define MOBJ_ID_A_DL_IN_OVER_PWR_1      0x30    //������������ʸ澯��ͨ��1��  
#define MOBJ_ID_A_DL_IN_UNDER_PWR_1     0x31    //��������Ƿ���ʸ澯��ͨ��1��
#define MOBJ_ID_A_DL_OUT_OVER_PWR_1     0x32    //������������ʸ澯��ͨ��1��
#define MOBJ_ID_A_DL_OUT_UNDER_PWR_1    0x33    //�������Ƿ���ʸ澯��ͨ��1��
#define MOBJ_ID_A_UL_OUT_OVER_PWR_1     0x34    //������������ʸ澯��ͨ��1��
#define MOBJ_ID_A_DL_SWR_1              0x35    //����פ���ȸ澯    ��ͨ��1�� 

#define MOBJ_ID_A_DL_IN_OVER_PWR_2      0x36    //������������ʸ澯��ͨ��2��  
#define MOBJ_ID_A_DL_IN_UNDER_PWR_2     0x37    //��������Ƿ���ʸ澯��ͨ��2��
#define MOBJ_ID_A_DL_OUT_OVER_PWR_2     0x38    //������������ʸ澯��ͨ��2��
#define MOBJ_ID_A_DL_OUT_UNDER_PWR_2    0x39    //�������Ƿ���ʸ澯��ͨ��2��
#define MOBJ_ID_A_UL_OUT_OVER_PWR_2     0x3A    //������������ʸ澯��ͨ��2��
#define MOBJ_ID_A_DL_SWR_2              0x3B    //����פ���ȸ澯    ��ͨ��2�� 

#define MOBJ_ID_A_DL_IN_OVER_PWR_3      0x3C    //������������ʸ澯��ͨ��3��  
#define MOBJ_ID_A_DL_IN_UNDER_PWR_3     0x3D    //��������Ƿ���ʸ澯��ͨ��3��
#define MOBJ_ID_A_DL_OUT_OVER_PWR_3     0x3E    //������������ʸ澯��ͨ��3��
#define MOBJ_ID_A_DL_OUT_UNDER_PWR_3    0x3F    //�������Ƿ���ʸ澯��ͨ��3��
#define MOBJ_ID_A_UL_OUT_OVER_PWR_3     0x40    //������������ʸ澯��ͨ��3��
#define MOBJ_ID_A_DL_SWR_3              0x41    //����פ���ȸ澯    ��ͨ��3�� 

#define MOBJ_ID_A_DL_IN_OVER_PWR_4      0x42    //������������ʸ澯��ͨ��4��  
#define MOBJ_ID_A_DL_IN_UNDER_PWR_4     0x43    //��������Ƿ���ʸ澯��ͨ��4��
#define MOBJ_ID_A_DL_OUT_OVER_PWR_4     0x44    //������������ʸ澯��ͨ��4��
#define MOBJ_ID_A_DL_OUT_UNDER_PWR_4    0x45    //�������Ƿ���ʸ澯��ͨ��4��
#define MOBJ_ID_A_UL_OUT_OVER_PWR_4     0x46    //������������ʸ澯��ͨ��4��
#define MOBJ_ID_A_DL_SWR_4              0x47    //����פ���ȸ澯    ��ͨ��4�� 

//MCM-25_20061108_zhonghw_begin
#define MOBJ_ID_A_UL_OUT_UNDER_PWR_1    0x48    //�������Ƿ���ʸ澯��ͨ��1)//�淶20060810
#define MOBJ_ID_A_UL_OUT_UNDER_PWR_2    0x49    //�������Ƿ���ʸ澯��ͨ��2)//�淶20060810
#define MOBJ_ID_A_UL_OUT_UNDER_PWR_3    0x4A    //�������Ƿ���ʸ澯��ͨ��3)//�淶20060810
#define MOBJ_ID_A_UL_OUT_UNDER_PWR_4    0x4B    //�������Ƿ���ʸ澯��ͨ��4)//�淶20060810
#define MOBJ_ID_A_UL_SWR_1              0x4C    //����פ���ȸ澯    ��ͨ��1)//�淶20060810
#define MOBJ_ID_A_UL_SWR_2              0x4D    //����פ���ȸ澯    ��ͨ��2)//�淶20060810
#define MOBJ_ID_A_UL_SWR_3              0x4E    //����פ���ȸ澯    ��ͨ��3)//�淶20060810
#define MOBJ_ID_A_UL_SWR_4              0x4F    //����פ���ȸ澯    ��ͨ��4)//�淶20060810
//MCM-25_20061108_zhonghw_end

#define MOBJ_ID_A_TTA_1             0x60    //�����Ŵ����澯ʹ��1��8
#define MOBJ_ID_A_TTA_2             0x61
#define MOBJ_ID_A_TTA_3             0x62
#define MOBJ_ID_A_TTA_4             0x63
#define MOBJ_ID_A_TTA_5             0x64
#define MOBJ_ID_A_TTA_6             0x65
#define MOBJ_ID_A_TTA_7             0x66
#define MOBJ_ID_A_TTA_8             0x67

#define MOBJ_ID_A_PA_BYPASS_1       0x68    //������·�澯ʹ��1��8
#define MOBJ_ID_A_PA_BYPASS_2       0x69
#define MOBJ_ID_A_PA_BYPASS_3       0x6A
#define MOBJ_ID_A_PA_BYPASS_4       0x6B
#define MOBJ_ID_A_PA_BYPASS_5       0x6C
#define MOBJ_ID_A_PA_BYPASS_6       0x6D
#define MOBJ_ID_A_PA_BYPASS_7       0x6E
#define MOBJ_ID_A_PA_BYPASS_8       0x6F

#define MOBJ_ID_A_SELF_OSC_SHUTDOWN 0x70    //3G:�Լ��ػ������澯
#define MOBJ_ID_A_SELF_OSC_REDUCE_ATT 0x71  //3G:�Լ������汣���澯

//0x0280��0x029F    ϵͳ����    
//0x02A0��0x02FF    �����Զ���
//0x0380��0x039F    ϵͳ����    
//0x03A0��0x03FF    �����Զ��� 
#define MOBJ_ID_A_RESERVED_BEGIN    0x80
#define MOBJ_ID_A_RESERVED_END      0x9F
#define MOBJ_ID_A_USER_DEFINE_BEGIN 0xA0
#define MOBJ_ID_A_USER_DEFINE_END   0xFF


//------------------------------����ΪPOI��������-----------------------------------

#define MOBJ_ID_A_CDMA800_IN_OVER_PWR   0xA0    //CDMA800��������ʸ澯
#define MOBJ_ID_A_CDMA800_IN_UNDER_PWR  0xA1    //CDMA800����Ƿ���ʸ澯
#define MOBJ_ID_A_MB_GSM_IN_OVER_PWR    0xA2    //�ƶ�GSM��������ʸ澯
#define MOBJ_ID_A_MB_GSM_IN_UNDER_PWR   0xA3    //�ƶ�GSM����Ƿ���ʸ澯
#define MOBJ_ID_A_UC_GSM_IN_OVER_PWR    0xA4    //��ͨGSM��������ʸ澯
#define MOBJ_ID_A_UC_GSM_IN_UNDER_PWR   0xA5    //��ͨGSM����Ƿ���ʸ澯
#define MOBJ_ID_A_MB_DCS_IN_OVER_PWR    0xA6    //�ƶ�DCS��������ʸ澯
#define MOBJ_ID_A_MB_DCS_IN_UNDER_PWR   0xA7    //�ƶ�DCS����Ƿ���ʸ澯
#define MOBJ_ID_A_UC_DCS_IN_OVER_PWR    0xA8    //��ͨDCS��������ʸ澯
#define MOBJ_ID_A_UC_DCS_IN_UNDER_PWR   0xA9    //��ͨDCS����Ƿ���ʸ澯
#define MOBJ_ID_A_3G1_FDD_IN_OVER_PWR   0xAA    //3G1-FDD��������ʸ澯
#define MOBJ_ID_A_3G1_FDD_IN_UNDER_PWR  0xAB    //3G1-FDD����Ƿ���ʸ澯
#define MOBJ_ID_A_3G2_FDD_IN_OVER_PWR   0xAC    //3G2-FDD��������ʸ澯
#define MOBJ_ID_A_3G2_FDD_IN_UNDER_PWR  0xAD    //3G2-FDD����Ƿ���ʸ澯
#define MOBJ_ID_A_3G3_TDD_IN_OVER_PWR   0xAE    //3G3-TDD��������ʸ澯
#define MOBJ_ID_A_3G3_TDD_IN_UNDER_PWR  0xAF    //3G3-TDD����Ƿ���ʸ澯
#define MOBJ_ID_A_TRUNK_IN_OVER_PWR     0xB0    //��Ⱥϵͳ��������ʸ澯
#define MOBJ_ID_A_TRUNK_IN_UNDER_PWR    0xB1    //��Ⱥϵͳ����Ƿ���ʸ澯

//------------------------------����ΪPOI��������-----------------------------------

//------------------------------����Ϊ̫���ܿ�������������-----------------------------------
#define MOBJ_ID_A_SEC_ST_BAT_BLOWOUT    0xC0    //���ر���˿��
#define MOBJ_ID_A_SEC_SELF_RESUME_BLOWOUT 0xC1  //����Իָ�����˿��
#define MOBJ_ID_A_SEC_OUTPUT_OVER_CUR   0xC2    //�������, over current
#define MOBJ_ID_A_SEC_CIRCUIT_FAULT     0xC3    //���Ƶ�·����
#define MOBJ_ID_A_SEC_VOL_24_FAULT      0xC4    //24V���ع���
#define MOBJ_ID_A_SEC_VOL_12_FAULT      0xC5    //12V���ع���
#define MOBJ_ID_A_SEC_VOL_5_FAULT       0xC6    //5V���ع���
#define MOBJ_ID_A_SEC_USE_UNDER_VOL     0xC7    //Ƿѹʹ����ʾ
#define MOBJ_ID_A_SEC_OVER_VOL          0xC8    //���ع�ѹ�澯
#define MOBJ_ID_A_SEC_OVER_DISCHARGING  0xC9    //����Ԥ���Ÿ澯
#define MOBJ_ID_A_SEC_OVER_DISCHARGED   0xCA    //���ع��Ÿ澯
#define MOBJ_ID_A_SEC_ST_BAT_FAULT      0xCB    //���ع���
#define MOBJ_ID_A_SEC_SB_FAULT          0xCC    //������
#define MOBJ_ID_A_SEC_DOOR              0xCD    //�Ž��澯
#define MOBJ_ID_A_SEC_ALEAK             0xCE    //ˮ©�澯
//------------------------------����Ϊ̫���ܿ�������������-----------------------------------

//���ĸ澯��ID�ĵ��ֽڣ������жϸ澯��ز����ĺϷ��ԣ������Ӹ澯��ʱ��Ҫ����
#define MAX_ALARM_ITEM_ID               MOBJ_ID_A_SEC_ALEAK


//���ò���
#define MOBJ_ID_SETTING_PARAM_SET   0x04    //����ID�ĸ��ֽ�

#define MOBJ_ID_SP_RF_SW            0x01    //��Ƶ�źſ���״̬
#define MOBJ_ID_SP_PA_SW_1          0x02    //���ſ���״̬1��8 
#define MOBJ_ID_SP_PA_SW_2          0x03
#define MOBJ_ID_SP_PA_SW_3          0x04
#define MOBJ_ID_SP_PA_SW_4          0x05
#define MOBJ_ID_SP_PA_SW_5          0x06
#define MOBJ_ID_SP_PA_SW_6          0x07
#define MOBJ_ID_SP_PA_SW_7          0x08
#define MOBJ_ID_SP_PA_SW_8          0x09

#define MOBJ_ID_SP_WORK_CH_NUM_1    0x10    //�����ŵ���1��16
#define MOBJ_ID_SP_WORK_CH_NUM_2    0x11
#define MOBJ_ID_SP_WORK_CH_NUM_3    0x12
#define MOBJ_ID_SP_WORK_CH_NUM_4    0x13
#define MOBJ_ID_SP_WORK_CH_NUM_5    0x14
#define MOBJ_ID_SP_WORK_CH_NUM_6    0x15
#define MOBJ_ID_SP_WORK_CH_NUM_7    0x16
#define MOBJ_ID_SP_WORK_CH_NUM_8    0x17
#define MOBJ_ID_SP_WORK_CH_NUM_9    0x18    
#define MOBJ_ID_SP_WORK_CH_NUM_10   0x19
#define MOBJ_ID_SP_WORK_CH_NUM_11   0x1A
#define MOBJ_ID_SP_WORK_CH_NUM_12   0x1B
#define MOBJ_ID_SP_WORK_CH_NUM_13   0x1C
#define MOBJ_ID_SP_WORK_CH_NUM_14   0x1D
#define MOBJ_ID_SP_WORK_CH_NUM_15   0x1E
#define MOBJ_ID_SP_WORK_CH_NUM_16   0x1F

#define MOBJ_ID_SP_SHF_CH_NUM_1     0x20   //��Ƶ�ŵ���1��16 
#define MOBJ_ID_SP_SHF_CH_NUM_2     0x21
#define MOBJ_ID_SP_SHF_CH_NUM_3     0x22
#define MOBJ_ID_SP_SHF_CH_NUM_4     0x23
#define MOBJ_ID_SP_SHF_CH_NUM_5     0x24
#define MOBJ_ID_SP_SHF_CH_NUM_6     0x25
#define MOBJ_ID_SP_SHF_CH_NUM_7     0x26
#define MOBJ_ID_SP_SHF_CH_NUM_8     0x27
#define MOBJ_ID_SP_SHF_CH_NUM_9     0x28
#define MOBJ_ID_SP_SHF_CH_NUM_10    0x29
#define MOBJ_ID_SP_SHF_CH_NUM_11    0x2A
#define MOBJ_ID_SP_SHF_CH_NUM_12    0x2B
#define MOBJ_ID_SP_SHF_CH_NUM_13    0x2C
#define MOBJ_ID_SP_SHF_CH_NUM_14    0x2D
#define MOBJ_ID_SP_SHF_CH_NUM_15    0x2E
#define MOBJ_ID_SP_SHF_CH_NUM_16    0x2F

#define MOBJ_ID_SP_WORK_UB_CH_NUM   0x30    //����Ƶ�����ϱߴ��ŵ���
#define MOBJ_ID_SP_WORK_LB_CH_NUM   0x31    //����Ƶ�����±ߴ��ŵ���
#define MOBJ_ID_SP_SHF_UB_CH_NUM    0x32    //��ƵƵ�����ϱߴ��ŵ���
#define MOBJ_ID_SP_SHF_LB_CH_NUM    0x33    //��ƵƵ�����±ߴ��ŵ���

#define MOBJ_ID_SP_WORK_UB_CH_NUM_1   0x34  //����Ƶ�����ϱߴ��ŵ��ţ�ͨ��1��
#define MOBJ_ID_SP_WORK_LB_CH_NUM_1   0x35  //����Ƶ�����±ߴ��ŵ��ţ�ͨ��1��
#define MOBJ_ID_SP_WORK_UB_CH_NUM_2   0x36  //����Ƶ�����ϱߴ��ŵ��ţ�ͨ��2��
#define MOBJ_ID_SP_WORK_LB_CH_NUM_2   0x37  //����Ƶ�����±ߴ��ŵ��ţ�ͨ��2��
#define MOBJ_ID_SP_WORK_UB_CH_NUM_3   0x38  //����Ƶ�����ϱߴ��ŵ��ţ�ͨ��3��
#define MOBJ_ID_SP_WORK_LB_CH_NUM_3   0x39  //����Ƶ�����±ߴ��ŵ��ţ�ͨ��3��
#define MOBJ_ID_SP_WORK_UB_CH_NUM_4   0x3A  //����Ƶ�����ϱߴ��ŵ��ţ�ͨ��4��
#define MOBJ_ID_SP_WORK_LB_CH_NUM_4   0x3B  //����Ƶ�����±ߴ��ŵ��ţ�ͨ��4��

#define MOBJ_ID_SP_UL_ATT          0x40     //����˥��ֵ
#define MOBJ_ID_SP_DL_ATT          0x41     //����˥��ֵ
#define MOBJ_ID_SP_UL_ATT_1        0x42     //����˥��ֵ��ͨ��1��
#define MOBJ_ID_SP_DL_ATT_1        0x43     //����˥��ֵ��ͨ��1��
#define MOBJ_ID_SP_UL_ATT_2        0x44     //����˥��ֵ��ͨ��2��
#define MOBJ_ID_SP_DL_ATT_2        0x45     //����˥��ֵ��ͨ��2��
#define MOBJ_ID_SP_UL_ATT_3        0x46     //����˥��ֵ��ͨ��3��
#define MOBJ_ID_SP_DL_ATT_3        0x47     //����˥��ֵ��ͨ��3��
#define MOBJ_ID_SP_UL_ATT_4        0x48     //����˥��ֵ��ͨ��4��
#define MOBJ_ID_SP_DL_ATT_4        0x49     //����˥��ֵ��ͨ��4��

#define MOBJ_ID_SP_DL_SWR_THR      0x50     //����פ��������
#define MOBJ_ID_SP_PA_OVERHEAT_THR 0x51     //���Ź��¶ȸ澯����
#define MOBJ_ID_SP_SRC_CELL_ID     0x52     //��ԴС��ʶ�������ֵ

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR  0x53 //�������빦��Ƿ��������
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR   0x54 //�������빦�ʹ���������
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR 0x55 //�����������Ƿ��������
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR  0x56 //����������ʹ���������
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR  0x57 //����������ʹ���������

#define MOBJ_ID_SP_SHF_UB_CH_NUM_1 0x58     //��ƵƵ�����ϱߴ��ŵ��ţ�ͨ��1��
#define MOBJ_ID_SP_SHF_LB_CH_NUM_1 0x59     //��ƵƵ�����±ߴ��ŵ��ţ�ͨ��1��
#define MOBJ_ID_SP_SHF_UB_CH_NUM_2 0x5A     //��ƵƵ�����ϱߴ��ŵ��ţ�ͨ��2��
#define MOBJ_ID_SP_SHF_LB_CH_NUM_2 0x5B     //��ƵƵ�����±ߴ��ŵ��ţ�ͨ��2��
#define MOBJ_ID_SP_SHF_UB_CH_NUM_3 0x5C     //��ƵƵ�����ϱߴ��ŵ��ţ�ͨ��3��
#define MOBJ_ID_SP_SHF_LB_CH_NUM_3 0x5D     //��ƵƵ�����±ߴ��ŵ��ţ�ͨ��3��
#define MOBJ_ID_SP_SHF_UB_CH_NUM_4 0x5E     //��ƵƵ�����ϱߴ��ŵ��ţ�ͨ��4��
#define MOBJ_ID_SP_SHF_LB_CH_NUM_4 0x5F     //��ƵƵ�����±ߴ��ŵ��ţ�ͨ��4��

#define MOBJ_ID_SP_RF_SW_1         0x60     //��Ƶ�л�����״̬1��6 ���ز��أ�
#define MOBJ_ID_SP_RF_SW_2         0x61
#define MOBJ_ID_SP_RF_SW_3         0x62
#define MOBJ_ID_SP_RF_SW_4         0x63
#define MOBJ_ID_SP_RF_SW_5         0x64
#define MOBJ_ID_SP_RF_SW_6         0x65

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_1  0x66 //�������빦��Ƿ�������ޣ�ͨ��1��
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_1   0x67 //�������빦�ʹ��������ޣ�ͨ��1��
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_1 0x68 //�����������Ƿ�������ޣ�ͨ��1��
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_1  0x69 //����������ʹ��������ޣ�ͨ��1��
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_1  0x6A //����������ʹ��������ޣ�ͨ��1��
#define MOBJ_ID_SP_DL_SWR_THR_1           0x6B //����פ��������        ��ͨ��1��
#define MOBJ_ID_SP_UL_SWR_THR_1           0x4A //����פ��������        ��ͨ��1��//�淶20060810

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_2  0x6C //�������빦��Ƿ�������ޣ�ͨ��2��
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_2   0x6D //�������빦�ʹ��������ޣ�ͨ��2��
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_2 0x6E //�����������Ƿ�������ޣ�ͨ��2��
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_2  0x6F //����������ʹ��������ޣ�ͨ��2��
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_2  0x70 //����������ʹ��������ޣ�ͨ��2��
#define MOBJ_ID_SP_DL_SWR_THR_2           0x71 //����פ��������        ��ͨ��2��
#define MOBJ_ID_SP_UL_SWR_THR_2           0x4B //����פ��������        ��ͨ��2��//�淶20060810

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_3  0x72 //�������빦��Ƿ�������ޣ�ͨ��3��
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_3   0x73 //�������빦�ʹ��������ޣ�ͨ��3��
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_3 0x74 //�����������Ƿ�������ޣ�ͨ��3��
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_3  0x75 //����������ʹ��������ޣ�ͨ��3��
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_3  0x76 //����������ʹ��������ޣ�ͨ��3��
#define MOBJ_ID_SP_DL_SWR_THR_3           0x77 //����פ��������        ��ͨ��3��
#define MOBJ_ID_SP_UL_SWR_THR_3           0x4C//����פ��������         ��ͨ��3��//�淶20060810

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_4  0x78 //�������빦��Ƿ�������ޣ�ͨ��4��
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_4   0x79 //�������빦�ʹ��������ޣ�ͨ��4��
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_4 0x7A //�����������Ƿ�������ޣ�ͨ��4��
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_4  0x7B //����������ʹ��������ޣ�ͨ��4��
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_4  0x7C //����������ʹ��������ޣ�ͨ��4��
#define MOBJ_ID_SP_DL_SWR_THR_4           0x7D //����פ��������        ��ͨ��4��
#define MOBJ_ID_SP_UL_SWR_THR_4           0x4D //����פ��������        ��ͨ��4��//�淶20060810

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX    0x80 //3G:�������빦��Ƿ�������ޣ�sint2�ͣ���λΪdBm������Ϊ10������ͬ
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX     0x81 //3G:�������빦�ʹ���������
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX   0x82 //3G:�����������Ƿ��������
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX    0x83 //3G:����������ʹ���������
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX    0x84 //3G:����������ʹ���������
#define MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX   0x85 //3G:�����������Ƿ��������//�淶20060810
#define MOBJ_ID_SP_UL_SWR_THR_EX             0x86 //����פ��������           //�淶20060810

#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_1  0x88 //3G:�������빦��Ƿ�������ޣ�ͨ��1��
#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_2  0x89 //3G:�������빦��Ƿ�������ޣ�ͨ��2��
#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_3  0x8A //3G:�������빦��Ƿ�������ޣ�ͨ��3��
#define MOBJ_ID_SP_DL_IN_UNDER_PWR_THR_EX_4  0x8B //3G:�������빦��Ƿ�������ޣ�ͨ��4��

#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_1   0x8C //3G:�������빦�ʹ��������ޣ�ͨ��1��
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_2   0x8D //3G:�������빦�ʹ��������ޣ�ͨ��2��
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_3   0x8E //3G:�������빦�ʹ��������ޣ�ͨ��3��
#define MOBJ_ID_SP_DL_IN_OVER_PWR_THR_EX_4   0x8F //3G:�������빦�ʹ��������ޣ�ͨ��4��

#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_1 0x90 //3G:�����������Ƿ�������ޣ�ͨ��1��
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_2 0x91 //3G:�����������Ƿ�������ޣ�ͨ��2��
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_3 0x92 //3G:�����������Ƿ�������ޣ�ͨ��3��
#define MOBJ_ID_SP_DL_OUT_UNDER_PWR_THR_EX_4 0x93 //3G:�����������Ƿ�������ޣ�ͨ��4��

#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_1  0x94 //3G:����������ʹ��������ޣ�ͨ��1��
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_2  0x95 //3G:����������ʹ��������ޣ�ͨ��2��
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_3  0x96 //3G:����������ʹ��������ޣ�ͨ��3��
#define MOBJ_ID_SP_DL_OUT_OVER_PWR_THR_EX_4  0x97 //3G:����������ʹ��������ޣ�ͨ��4��

#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_1  0x98 //3G:����������ʹ��������ޣ�ͨ��1��
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_2  0x99 //3G:����������ʹ��������ޣ�ͨ��2��
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_3  0x9A //3G:����������ʹ��������ޣ�ͨ��3��
#define MOBJ_ID_SP_UL_OUT_OVER_PWR_THR_EX_4  0x9B //3G:����������ʹ��������ޣ�ͨ��4��

#define MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_1 0x9C //3G:�����������Ƿ�������� (ͨ��1)//�淶20060810
#define MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_2 0x9D //3G:�����������Ƿ�������� (ͨ��2)//�淶20060810
#define MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_3 0x9E //3G:�����������Ƿ�������� (ͨ��3)//�淶20060810
#define MOBJ_ID_SP_UL_OUT_UNDER_PWR_THR_EX_4 0x9F //3G:�����������Ƿ�������� (ͨ��4)//�淶20060810

//0x049C��0x049F    ϵͳ����    
//0x04A0��0x04FF    �����Զ���
//------------------------------����ΪPOI��������-----------------------------------

#define MOBJ_ID_SP_CDMA800_IN_OVER_PWR_THR  0xA0    //CDMA800�������������
#define MOBJ_ID_SP_CDMA800_IN_UNDER_PWR_THR 0xA1    //CDMA800����Ƿ��������

#define MOBJ_ID_SP_MB_GSM_IN_OVER_PWR_THR   0xA2    //�ƶ�GSM�������������
#define MOBJ_ID_SP_MB_GSM_IN_UNDER_PWR_THR  0xA3    //�ƶ�GSM����Ƿ��������

#define MOBJ_ID_SP_UC_GSM_IN_OVER_PWR_THR   0xA4    //��ͨGSM�������������
#define MOBJ_ID_SP_UC_GSM_IN_UNDER_PWR_THR  0xA5    //��ͨGSM����Ƿ��������

#define MOBJ_ID_SP_MB_DCS_IN_OVER_PWR_THR   0xA6    //�ƶ�DCS�������������
#define MOBJ_ID_SP_MB_DCS_IN_UNDER_PWR_THR  0xA7    //�ƶ�DCS����Ƿ��������

#define MOBJ_ID_SP_UC_DCS_IN_OVER_PWR_THR   0xA8    //��ͨDCS�������������
#define MOBJ_ID_SP_UC_DCS_IN_UNDER_PWR_THR  0xA9    //��ͨDCS����Ƿ��������

#define MOBJ_ID_SP_3G1_FDD_IN_OVER_PWR_THR  0xAA    //3G1-FDD�������������
#define MOBJ_ID_SP_3G1_FDD_IN_UNDER_PWR_THR 0xAB    //3G1-FDD����Ƿ��������

#define MOBJ_ID_SP_3G2_FDD_IN_OVER_PWR_THR  0xAC    //3G2-FDD�������������
#define MOBJ_ID_SP_3G2_FDD_IN_UNDER_PWR_THR 0xAD    //3G2-FDD����Ƿ��������

#define MOBJ_ID_SP_3G3_TDD_IN_OVER_PWR_THR  0xAE    //3G3-TDD�������������
#define MOBJ_ID_SP_3G3_TDD_IN_UNDER_PWR_THR 0xAF    //3G3-TDD����Ƿ��������

#define MOBJ_ID_SP_TRUNK_IN_OVER_PWR_THR    0xB0    //��Ⱥϵͳ�������������
#define MOBJ_ID_SP_TRUNK_IN_UNDER_PWR_THR   0xB1    //��Ⱥϵͳ����Ƿ��������

//------------------------------����ΪPOI��������-----------------------------------

//------------------------------����Ϊ̫���ܿ�������������-----------------------------------
#define MOBJ_ID_SP_SEC_OVER_CHARGED_THR             0xC0    //�����ѹ����
#define MOBJ_ID_SP_SEC_EVEN_CHARGE_THR              0xC1    //�������ѹ����
#define MOBJ_ID_SP_SEC_OVER_CHARGED_RESUME_THR      0xC2    //����ָ���ѹ����
#define MOBJ_ID_SP_SEC_FC_TO_PC_VOL_THR             0xC3    //ǿ��ת�����ѹ����
#define MOBJ_ID_SP_SEC_OVER_DISCHARGING_THR         0xC4    //Ԥ���ŵ�ѹ����
#define MOBJ_ID_SP_SEC_OVER_DISCHARGED_THR          0xC5    //���ŵ�ѹ����
#define MOBJ_ID_SP_SEC_OVER_DISCHARGED_RESUME_THR   0xC6    //���Żָ���ѹ����
#define MOBJ_ID_SP_SEC_PC_TO_FC_CUR_THR             0xC7    //����תǿ���������
#define MOBJ_ID_SP_SEC_OUTPUT_OVER_CUR_THR          0xC8    //�����������
#define MOBJ_ID_SP_SEC_RESET_SW                     0xC9    //��λ����
#define MOBJ_ID_SP_SEC_CHARGE_SW                    0xCA    //��翪��
#define MOBJ_ID_SP_SEC_LOAD_SW                      0xCB    //���ؿ���
#define MOBJ_ID_SP_SEC_STARTUP_UNDER_VOL_SW         0xCC    //����Ƿѹ��������
//------------------------------����Ϊ̫���ܿ�������������-----------------------------------

//ʵʱ��������
#define MOBJ_ID_RT_COLL_PARAM_SET  0x05     //����ID�ĸ��ֽ�
#define MOBJ_ID_RC_PA_TEMP         0x01     //�����¶�ֵ
#define MOBJ_ID_RC_DL_IN_PWR       0x02     //�������빦�ʵ�ƽ
#define MOBJ_ID_RC_DL_OUT_PWR      0x03     //����������ʵ�ƽ
#define MOBJ_ID_RC_UL_THE_GAIN     0x04     //������������
#define MOBJ_ID_RC_DL_ACT_GAIN     0x05     //����ʵ������
#define MOBJ_ID_RC_DL_SWR          0x06     //����פ����ֵ
#define MOBJ_ID_RC_SS_MNC          0x07     //��Դ��Ϣ����Ӫ�̴���
#define MOBJ_ID_RC_SS_LAC          0x08     //��Դ��Ϣ��λ��������
#define MOBJ_ID_RC_SS_BSIC         0x09     //��Դ��Ϣ����վʶ����
#define MOBJ_ID_RC_SS_BCCH         0x0A     //��Դ��Ϣ��BCCH������Ƶ��
#define MOBJ_ID_RC_SS_BCCH_RX_LEV  0x0B     //��Դ��Ϣ��BCCH���յ�ƽ
#define MOBJ_ID_RC_SS_CI           0x0C     //��Դ��Ϣ��С��ʶ����ʵʱֵ
#define MOBJ_ID_RC_UL_OUT_PWR      0x0D     //����������ʵ�ƽ
#define MOBJ_ID_RC_OPT_RX_PWR      0x0E     //���չ���
#define MOBJ_ID_RC_OPT_TX_PWR      0x0F     //�ⷢ����

#define MOBJ_ID_RC_DL_IN_PWR_1     0x10     //�������빦�ʵ�ƽ��ͨ��1��
#define MOBJ_ID_RC_DL_OUT_PWR_1    0x11     //����������ʵ�ƽ��ͨ��1��
#define MOBJ_ID_RC_UL_THE_GAIN_1   0x12     //�����������棨ͨ��1��
#define MOBJ_ID_RC_DL_ACT_GAIN_1   0x13     //����ʵ�����棨ͨ��1��
#define MOBJ_ID_RC_UL_OUT_PWR_1    0x14     //����������ʵ�ƽ��ͨ��1��
#define MOBJ_ID_RC_DL_SWR_1        0x15     //����פ����ֵ��ͨ��1��

#define MOBJ_ID_RC_DL_IN_PWR_2     0x16     //�������빦�ʵ�ƽ��ͨ��2��
#define MOBJ_ID_RC_DL_OUT_PWR_2    0x17     //����������ʵ�ƽ��ͨ��2��
#define MOBJ_ID_RC_UL_THE_GAIN_2   0x18     //�����������棨ͨ��2��
#define MOBJ_ID_RC_DL_ACT_GAIN_2   0x19     //����ʵ�����棨ͨ��2��
#define MOBJ_ID_RC_UL_OUT_PWR_2    0x1A     //����������ʵ�ƽ��ͨ��2��
#define MOBJ_ID_RC_DL_SWR_2        0x1B     //����פ����ֵ��ͨ��2��

#define MOBJ_ID_RC_DL_IN_PWR_3     0x1C     //�������빦�ʵ�ƽ��ͨ��3��
#define MOBJ_ID_RC_DL_OUT_PWR_3    0x1D     //����������ʵ�ƽ��ͨ��3��
#define MOBJ_ID_RC_UL_THE_GAIN_3   0x1E     //�����������棨ͨ��3��
#define MOBJ_ID_RC_DL_ACT_GAIN_3   0x1F     //����ʵ�����棨ͨ��3��
#define MOBJ_ID_RC_UL_OUT_PWR_3    0x20     //����������ʵ�ƽ��ͨ��3��
#define MOBJ_ID_RC_DL_SWR_3        0x21     //����פ����ֵ��ͨ��3��

#define MOBJ_ID_RC_DL_IN_PWR_4     0x22     //�������빦�ʵ�ƽ��ͨ��4��
#define MOBJ_ID_RC_DL_OUT_PWR_4    0x23     //����������ʵ�ƽ��ͨ��4��
#define MOBJ_ID_RC_UL_THE_GAIN_4   0x24     //�����������棨ͨ��4��
#define MOBJ_ID_RC_DL_ACT_GAIN_4   0x25     //����ʵ�����棨ͨ��4��
#define MOBJ_ID_RC_UL_OUT_PWR_4    0x26     //����������ʵ�ƽ��ͨ��4��
#define MOBJ_ID_RC_DL_SWR_4        0x27     //����פ����ֵ��ͨ��4��

#define MOBJ_ID_RC_UL_BYPASS_STATUS  0x28   //�����ź���·״̬
#define MOBJ_ID_RC_DL_BYPASS_STATUS  0x29   //�����ź���·״̬

#define MOBJ_ID_RC_DL_IN_PWR_EX    0x30     //3G:�������빦�ʵ�ƽ��sint2�ͣ���λΪdBm������Ϊ10������ͬ
#define MOBJ_ID_RC_DL_OUT_PWR_EX   0x31     //3G:����������ʵ�ƽ
#define MOBJ_ID_RC_UL_OUT_PWR_EX   0x32     //3G:����������ʵ�ƽ
#define MOBJ_ID_RC_UL_SWR          0x33     //����פ����ֵ           //�淶20060810

#define MOBJ_ID_RC_DL_IN_PWR_EX_1  0x34     //3G:�������빦�ʵ�ƽ��ͨ��1��
#define MOBJ_ID_RC_DL_IN_PWR_EX_2  0x35     //3G:�������빦�ʵ�ƽ��ͨ��2��
#define MOBJ_ID_RC_DL_IN_PWR_EX_3  0x36     //3G:�������빦�ʵ�ƽ��ͨ��3��
#define MOBJ_ID_RC_DL_IN_PWR_EX_4  0x37     //3G:�������빦�ʵ�ƽ��ͨ��4��

#define MOBJ_ID_RC_DL_OUT_PWR_EX_1 0x38     //3G:����������ʵ�ƽ��ͨ��1��
#define MOBJ_ID_RC_DL_OUT_PWR_EX_2 0x39     //3G:����������ʵ�ƽ��ͨ��2��
#define MOBJ_ID_RC_DL_OUT_PWR_EX_3 0x3A     //3G:����������ʵ�ƽ��ͨ��3��
#define MOBJ_ID_RC_DL_OUT_PWR_EX_4 0x3B     //3G:����������ʵ�ƽ��ͨ��4��

#define MOBJ_ID_RC_UL_OUT_PWR_EX_1 0x3C     //3G:����������ʵ�ƽ��ͨ��1��
#define MOBJ_ID_RC_UL_OUT_PWR_EX_2 0x3D     //3G:����������ʵ�ƽ��ͨ��2��
#define MOBJ_ID_RC_UL_OUT_PWR_EX_3 0x3E     //3G:����������ʵ�ƽ��ͨ��3��
#define MOBJ_ID_RC_UL_OUT_PWR_EX_4 0x3F     //3G:����������ʵ�ƽ��ͨ��4��

//0x049C��0x049F    ��Դ��Ϣ��WCDMAԤ����

#define MOBJ_ID_RC_DONOR_CPICH_LEV     0x50 //ʩ���˿�CPICH���ʵ�ƽ
#define MOBJ_ID_RC_SERVICE_CPICH_LEV   0x51 //�û��˿�CPICH���ʵ�ƽ

//0x0560��0x059F    ϵͳ����    
//0x05A0��0x05FF    �����Զ��� 
//------------------------------����ΪPOI��������-----------------------------------

#define MOBJ_ID_RC_CDMA800_IN_PWR   0xA0    //CDMA800���빦�ʵ�ƽֵ
#define MOBJ_ID_RC_MB_GSM_IN_PWR    0xA1    //�ƶ�GSM���빦�ʵ�ƽֵ
#define MOBJ_ID_RC_UC_GSM_IN_PWR    0xA2    //��ͨGSM���빦�ʵ�ƽֵ
#define MOBJ_ID_RC_MB_DCS_IN_PWR    0xA3    //�ƶ�DCS���빦�ʵ�ƽֵ
#define MOBJ_ID_RC_UC_DCS_IN_PWR    0xA4    //��ͨDCS���빦�ʵ�ƽֵ
#define MOBJ_ID_RC_3G1_FDD_IN_PWR   0xA5    //3G1-FDD���빦�ʵ�ƽֵ
#define MOBJ_ID_RC_3G2_FDD_IN_PWR   0xA6    //3G2-FDD���빦�ʵ�ƽֵ
#define MOBJ_ID_RC_3G3_TDD_IN_PWR   0xA7    //3G3-FDD���빦�ʵ�ƽֵ
#define MOBJ_ID_RC_TRUNK_IN_PWR     0xA8    //��Ⱥϵͳ���빦�ʵ�ƽֵ

//------------------------------����ΪPOI��������-----------------------------------

//------------------------------����Ϊ̫���ܿ�������������-----------------------------------
#define MOBJ_ID_RC_SEC_ENV_TEMP         0xC0    //���ػ����¶�
#define MOBJ_ID_RC_SEC_SB_VOL           0xC1    //����ѹ
#define MOBJ_ID_RC_SEC_ST_BAT_VOL       0xC2    //���ص�ѹ
#define MOBJ_ID_RC_SEC_VOL_24           0xC3    //���24V��ѹ
#define MOBJ_ID_RC_SEC_VOL_12           0xC4    //���12V��ѹ
#define MOBJ_ID_RC_SEC_VOL_5            0xC5    //���5V��ѹ
#define MOBJ_ID_RC_SEC_SB_CUR           0xC6    //������
#define MOBJ_ID_RC_SEC_LOAD_CUR         0xC7    //���ص���
#define MOBJ_ID_RC_SEC_CHARGE_CUR       0xC8    //������
#define MOBJ_ID_RC_SEC_WIRE_VOL         0xC9    //��������ѹ��
#define MOBJ_ID_RC_SEC_CAPACITY         0xCA    //���غɵ���
#define MOBJ_ID_RC_SEC_CHARGE_CAPACITY  0xCB    //�������ĵ���
//------------------------------����Ϊ̫���ܿ�������������-----------------------------------

//MCP:B�ļ�ز���
//�豸Զ��������Ϣ
#define MOBJ_ID_UPGRADE_INFO_SET        0x02     //����ID�ĸ��ֽ�
#define MOBJ_ID_UI_RUNNING_MODE         0x01     //Ƕ��ʽ�������ģʽ
#define MOBJ_ID_UI_NEXT_BLOCK_SN        0x02     //��һ���ļ����ݿ����
#define MOBJ_ID_UI_MAX_BLOCK_SIZE       0x03     //֧�ֵ����ݿ鳤��

//Զ�������ļ���Ϣ
#define MOBJ_ID_UPGRADE_FILE_INFO_SET   0x03     //����ID�ĸ��ֽ�
#define MOBJ_ID_UF_FILE_ID              0x01     //�ļ���ʶ��
#define MOBJ_ID_UF_TRANS_CTRL_BYTE      0x02     //�ļ��������
#define MOBJ_ID_UF_TRANS_RSP_FLAG       0x03     //�ļ����ݰ�Ӧ��
#define MOBJ_ID_UF_CURRENT_BLOCK_SN     0x04     //�ļ����ݿ����
#define MOBJ_ID_UF_FILE_BLOCK           0x05     //�ļ����ݿ�

//��ز����б�������NV Memory��
//MCM-23_20061108_zhangjie_begin
#define MAX_PARAM_COUNT    500
//MCM-23_20061108_zhangjie_end
typedef struct
{
    USHORT usParamCount;     //�����ĸ���
    USHORT ausParamID[MAX_PARAM_COUNT];
}MOBJ_ID_TBL_ST;

//�豸��Ϣ
typedef struct
{
    UCHAR  ucMnftId;                    //�豸���̴���                                    
    UCHAR  ucDevType;                   //�豸���                                        
    CHAR   acDevModel[STR_LEN];         //�豸�ͺ�                                        
    CHAR   acDevSn[STR_LEN];            //�豸�������к�                                  
    UCHAR  ucChCount;                   //�豸��ʵ���ŵ�����                              
    CHAR   acLongitude[STR_LEN];        //����                                            
    CHAR   acLatitude[STR_LEN];         //γ��                                            
    CHAR   acSfwrVer[STR_LEN];          //��ذ汾��Ϣ                                       
    CHAR   acDevModel2[LONG_STR_LEN];   //3G:�豸�ͺ�
    UCHAR  ucRunningMode;               //3G:Ƕ��ʽ�������ģʽ��ֻ��Ӧ��ʱ��״̬�����ñ���
    USHORT usMaxApcLen;                 //3G:��֧��AP��CЭ�����󳤶�                       
    UCHAR  ucMcpbMode;                  //3G:MCP��B���õĽ�������                            
    UCHAR  ucNc;                        //3G:����ϵ����NC��                                  
    UCHAR  ucT1;                        //3G:�豸��Ӧ��ʱ��TOT1��                            
    USHORT usTg;                        //3G:���ͼ��ʱ�䣨TG������������NC��1ʱ�������壩   
    UCHAR  ucTp;                        //3G:��ͣ����ȴ�ʱ�䣨TP��                          
    USHORT usTimeToSwitch;              //3G:ת�����������ģʽʱ��OMC��Ҫ�ȴ���ʱ��         
    UCHAR  ucUpgradeResult;             //3G:�豸ִ����������Ľ�������ñ���                          
    UCHAR  ucUpgradeMode;               //3G:�豸ʹ�õ�Զ��������ʽ                          
}DEV_INFO_SET_ST;

//���ܲ���
typedef struct
{
    ULONG  ulStaNum;                    //վ����                          
    UCHAR  ucDevNum;                    //�豸���                    
    CHAR   acSmcAddr[TEL_NUM_LEN];      //���ŷ������ĺ���              
    CHAR   acQnsTelNum1[TEL_NUM_LEN];   //��ѯ/���õ绰����1��5       
    CHAR   acQnsTelNum2[TEL_NUM_LEN];                              
    CHAR   acQnsTelNum3[TEL_NUM_LEN];                              
    CHAR   acQnsTelNum4[TEL_NUM_LEN];                              
    CHAR   acQnsTelNum5[TEL_NUM_LEN];                              
    CHAR   acReportTelNum[TEL_NUM_LEN]; //�ϱ�����                    
    UCHAR  aucEmsIpAddr[IP_ADDR_LEN];   //�������IP��ַ��IP v4��     
    USHORT usEmsPortNum;                //�������IP��ַ�˿ں�        
    UCHAR  ucUseGprs;                   //3G:�豸�Ƿ�ʹ��GPRS��ʽ���Ѿ���ɾ��
    CHAR   acGprsApn[STR_LEN];          //3G:GPRS��������ƣ�����APN��
    USHORT usHeartbeatPeriod;           //3G:�豸�����������ʱ��     
    CHAR   acDevTelNum[TEL_NUM_LEN];    //3G:�豸�ĵ绰����            
    CHAR   acGprsUserId[STR_LEN];       //3G:GPRS�������û���ʶ       
    CHAR   acGprsPwd[STR_LEN];          //3G:GPRS����������           
    UCHAR  ucReportCommMode;            //�ϱ�ͨ�ŷ�ʽ
    UCHAR  ucCommMode;                  //ͨ�ŷ�ʽ                                  //�淶20060810
    UCHAR  ucReportType;                //�ϱ����ͣ����ñ���
    UCHAR  aucDateTime[DATE_TIME_LEN];  //���ڡ�ʱ��                       
    UCHAR  aucFtpServerIpAddr[IP_ADDR_LEN];   //3G:FTP������IP��ַ��IP v4�������ñ���
    USHORT usFtpServerPortNum;          //3G:FTP������IP��ַ�˿ںţ����ñ���
    CHAR   acFtpUserId[STR_LEN];        //3G:FTP�����������û��������ñ���
    CHAR   acFtpPwd[STR_LEN];           //3G:FTP���������Ŀ�����ñ���
    CHAR   acFtpPath[LONG_STR_LEN];     //3G:���·�������ñ���
    CHAR   acFileName[LONG_STR_LEN];    //3G:�ļ��������ñ���
}NM_PARAM_SET_ST;   

//���ò���
typedef struct
{
    UCHAR ucRfSw;           //��Ƶ�źſ���״̬

    UCHAR ucPaSw1;          //���ſ���״̬1��8    bit��
    UCHAR ucPaSw2;          
    UCHAR ucPaSw3;          
    UCHAR ucPaSw4;          
    UCHAR ucPaSw5;          
    UCHAR ucPaSw6;          
    UCHAR ucPaSw7;          
    UCHAR ucPaSw8;          

    USHORT usWorkChNum1;    //�����ŵ���1��16 uint2��    
    USHORT usWorkChNum2;    
    USHORT usWorkChNum3;    
    USHORT usWorkChNum4;    
    USHORT usWorkChNum5;    
    USHORT usWorkChNum6;    
    USHORT usWorkChNum7;    
    USHORT usWorkChNum8;    
    USHORT usWorkChNum9;    
    USHORT usWorkChNum10;   
    USHORT usWorkChNum11;   
    USHORT usWorkChNum12;   
    USHORT usWorkChNum13;   
    USHORT usWorkChNum14;   
    USHORT usWorkChNum15;   
    USHORT usWorkChNum16;   

    USHORT usShfChNum1;     //��Ƶ�ŵ���1��16 uint2��     
    USHORT usShfChNum2;     
    USHORT usShfChNum3;     
    USHORT usShfChNum4;     
    USHORT usShfChNum5;     
    USHORT usShfChNum6;     
    USHORT usShfChNum7;     
    USHORT usShfChNum8;     
    USHORT usShfChNum9;     
    USHORT usShfChNum10;    
    USHORT usShfChNum11;    
    USHORT usShfChNum12;    
    USHORT usShfChNum13;    
    USHORT usShfChNum14;    
    USHORT usShfChNum15;    
    USHORT usShfChNum16;    

    USHORT usWorkUbChNum;   //����Ƶ�����ϱߴ��ŵ���         
    USHORT usWorkLbChNum;   //����Ƶ�����±ߴ��ŵ���         
    USHORT usShfUbChNum;    //��ƵƵ�����ϱߴ��ŵ���         
    USHORT usShfLbChNum;    //��ƵƵ�����±ߴ��ŵ���         
                                 
    USHORT usWorkUbChNum1;  //����Ƶ�����ϱߴ��ŵ��ţ�ͨ��1�� 
    USHORT usWorkLbChNum1;  //����Ƶ�����±ߴ��ŵ��ţ�ͨ��1�� 
    USHORT usWorkUbChNum2;  //����Ƶ�����ϱߴ��ŵ��ţ�ͨ��2�� 
    USHORT usWorkLbChNum2;  //����Ƶ�����±ߴ��ŵ��ţ�ͨ��2�� 
    USHORT usWorkUbChNum3;  //����Ƶ�����ϱߴ��ŵ��ţ�ͨ��3�� 
    USHORT usWorkLbChNum3;  //����Ƶ�����±ߴ��ŵ��ţ�ͨ��3�� 
    USHORT usWorkUbChNum4;  //����Ƶ�����ϱߴ��ŵ��ţ�ͨ��4�� 
    USHORT usWorkLbChNum4;  //����Ƶ�����±ߴ��ŵ��ţ�ͨ��4��
                                 
    UCHAR  ucUlAtt;         //����˥��ֵ
    UCHAR  ucDlAtt;         //����˥��ֵ
    UCHAR  ucUlAtt1;        //����˥��ֵ��ͨ��1����Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����            
    UCHAR  ucDlAtt1;        //����˥��ֵ��ͨ��1����Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ�����            
    UCHAR  ucUlAtt2;        //����˥��ֵ��ͨ��2��            
    UCHAR  ucDlAtt2;        //����˥��ֵ��ͨ��2��            
    UCHAR  ucUlAtt3;        //����˥��ֵ��ͨ��3��            
    UCHAR  ucDlAtt3;        //����˥��ֵ��ͨ��3��            
    UCHAR  ucUlAtt4;        //����˥��ֵ��ͨ��4��            
    UCHAR  ucDlAtt4;        //����˥��ֵ��ͨ��4��            
                                 
    UCHAR  ucDlSwrThr;      //����פ��������                 
    CHAR   cPaOverheatThr;  //���Ź��¶ȸ澯����             
    USHORT usSrcCellId;     //��ԴС��ʶ�������ֵ           

    CHAR   cDlInUnderPwrThr;  //�������빦��Ƿ��������
    CHAR   cDlInOverPwrThr;   //�������빦�ʹ���������
    CHAR   cDlOutUnderPwrThr; //�����������Ƿ��������
    CHAR   cDlOutOverPwrThr;  //����������ʹ���������
    CHAR   cUlOutOverPwrThr;  //����������ʹ���������

    USHORT usShfUbChNum1;   //��ƵƵ�����ϱߴ��ŵ��ţ�ͨ��1��
    USHORT usShfLbChNum1;   //��ƵƵ�����±ߴ��ŵ��ţ�ͨ��1��
    USHORT usShfUbChNum2;   //��ƵƵ�����ϱߴ��ŵ��ţ�ͨ��2��
    USHORT usShfLbChNum2;   //��ƵƵ�����±ߴ��ŵ��ţ�ͨ��2��
    USHORT usShfUbChNum3;   //��ƵƵ�����ϱߴ��ŵ��ţ�ͨ��3��
    USHORT usShfLbChNum3;   //��ƵƵ�����±ߴ��ŵ��ţ�ͨ��3��
    USHORT usShfUbChNum4;   //��ƵƵ�����ϱߴ��ŵ��ţ�ͨ��4��
    USHORT usShfLbChNum4;   //��ƵƵ�����±ߴ��ŵ��ţ�ͨ��4��
                                
    UCHAR  ucRfSw1;         //��Ƶ�л�����״̬1��6 ���ز��أ�
    UCHAR  ucRfSw2;         
    UCHAR  ucRfSw3;         
    UCHAR  ucRfSw4;         
    UCHAR  ucRfSw5;         
    UCHAR  ucRfSw6;         

    CHAR   cDlInUnderPwrThr1;   //�������빦��Ƿ�������ޣ�ͨ��1����Ŀǰ�����㽫ͨ��1�Ĳ���ӳ�䵽����ͨ���ŵĶ�Ӧ�Ĳ����� 
    CHAR   cDlInOverPwrThr1;    //�������빦�ʹ��������ޣ�ͨ��1��
    CHAR   cDlOutUnderPwrThr1;  //�����������Ƿ�������ޣ�ͨ��1��
    CHAR   cDlOutOverPwrThr1;   //����������ʹ��������ޣ�ͨ��1��
    CHAR   cUlOutOverPwrThr1;   //����������ʹ��������ޣ�ͨ��1��
    UCHAR  ucDlSwrThr1;         //����פ�������ޣ�ͨ��1��
    UCHAR  ucUlSwrThr1;         //����פ�������ޣ�ͨ��1��          //�淶20060810
    
    CHAR   cDlInUnderPwrThr2;   //�������빦��Ƿ�������ޣ�ͨ��2��
    CHAR   cDlInOverPwrThr2;    //�������빦�ʹ��������ޣ�ͨ��2��
    CHAR   cDlOutUnderPwrThr2;  //�����������Ƿ�������ޣ�ͨ��2��
    CHAR   cDlOutOverPwrThr2;   //����������ʹ��������ޣ�ͨ��2��
    CHAR   cUlOutOverPwrThr2;   //����������ʹ��������ޣ�ͨ��2��
    UCHAR  ucDlSwrThr2;         //����פ�������ޣ�ͨ��2��      
    UCHAR  ucUlSwrThr2;         //����פ�������ޣ�ͨ��2��          //�淶20060810
                                                                 
    CHAR   cDlInUnderPwrThr3;   //�������빦��Ƿ�������ޣ�ͨ��3��
    CHAR   cDlInOverPwrThr3;    //�������빦�ʹ��������ޣ�ͨ��3��
    CHAR   cDlOutUnderPwrThr3;  //�����������Ƿ�������ޣ�ͨ��3��
    CHAR   cDlOutOverPwrThr3;   //����������ʹ��������ޣ�ͨ��3��
    CHAR   cUlOutOverPwrThr3;   //����������ʹ��������ޣ�ͨ��3��
    UCHAR  ucDlSwrThr3;         //����פ�������ޣ�ͨ��3��   
    UCHAR  ucUlSwrThr3;         //����פ�������ޣ�ͨ��3��          //�淶20060810 
                                                                 
    CHAR   cDlInUnderPwrThr4;   //�������빦��Ƿ�������ޣ�ͨ��4��
    CHAR   cDlInOverPwrThr4;    //�������빦�ʹ��������ޣ�ͨ��4��
    CHAR   cDlOutUnderPwrThr4;  //�����������Ƿ�������ޣ�ͨ��4��
    CHAR   cDlOutOverPwrThr4;   //����������ʹ��������ޣ�ͨ��4��
    CHAR   cUlOutOverPwrThr4;   //����������ʹ��������ޣ�ͨ��4��
    UCHAR  ucDlSwrThr4;         //����פ�������ޣ�ͨ��4��
    UCHAR  ucUlSwrThr4;         //����פ�������ޣ�ͨ��4��          //�淶20060810

    SHORT  sDlInUnderPwrThr;    //3G:�������빦��Ƿ�������ޣ�sint2�ͣ���λΪdBm������Ϊ10������ͬ
    SHORT  sDlInOverPwrThr;     //3G:�������빦�ʹ���������                                      
    SHORT  sDlOutUnderPwrThr;   //3G:�����������Ƿ��������                                        
    SHORT  sDlOutOverPwrThr;    //3G:����������ʹ���������                                      
    SHORT  sUlOutOverPwrThr;    //3G:����������ʹ���������
    SHORT  sUlOutUnderPwrThr;   //3G:�����������Ƿ��������        //�淶20060810
    UCHAR  ucUlSwrThr;          //3G:����פ��������                //�淶20060810
    
                               
    SHORT  sDlInUnderPwrThr1;   //3G:�������빦��Ƿ�������ޣ�ͨ��1��
    SHORT  sDlInUnderPwrThr2;   //3G:�������빦��Ƿ�������ޣ�ͨ��2��
    SHORT  sDlInUnderPwrThr3;   //3G:�������빦��Ƿ�������ޣ�ͨ��3��
    SHORT  sDlInUnderPwrThr4;   //3G:�������빦��Ƿ�������ޣ�ͨ��4��
                               
    SHORT  sDlInOverPwrThr1;    //3G:�������빦�ʹ��������ޣ�ͨ��1��
    SHORT  sDlInOverPwrThr2;    //3G:�������빦�ʹ��������ޣ�ͨ��2��
    SHORT  sDlInOverPwrThr3;    //3G:�������빦�ʹ��������ޣ�ͨ��3��
    SHORT  sDlInOverPwrThr4;    //3G:�������빦�ʹ��������ޣ�ͨ��4��
                               
    SHORT  sDlOutUnderPwrThr1;  //3G:�����������Ƿ�������ޣ�ͨ��1��
    SHORT  sDlOutUnderPwrThr2;  //3G:�����������Ƿ�������ޣ�ͨ��2��
    SHORT  sDlOutUnderPwrThr3;  //3G:�����������Ƿ�������ޣ�ͨ��3��
    SHORT  sDlOutUnderPwrThr4;  //3G:�����������Ƿ�������ޣ�ͨ��4��
                               
    SHORT  sDlOutOverPwrThr1;   //3G:����������ʹ��������ޣ�ͨ��1��
    SHORT  sDlOutOverPwrThr2;   //3G:����������ʹ��������ޣ�ͨ��2��
    SHORT  sDlOutOverPwrThr3;   //3G:����������ʹ��������ޣ�ͨ��3��
    SHORT  sDlOutOverPwrThr4;   //3G:����������ʹ��������ޣ�ͨ��4��
                               
    SHORT  sUlOutOverPwrThr1;   //3G:����������ʹ��������ޣ�ͨ��1��
    SHORT  sUlOutOverPwrThr2;   //3G:����������ʹ��������ޣ�ͨ��2��
    SHORT  sUlOutOverPwrThr3;   //3G:����������ʹ��������ޣ�ͨ��3��
    SHORT  sUlOutOverPwrThr4;   //3G:����������ʹ��������ޣ�ͨ��4��

    SHORT  sUlOutUnderPwrThr1;   //3G:�����������Ƿ�������ޣ�ͨ��1��    //�淶20060810
    SHORT  sUlOutUnderPwrThr2;   //3G:�����������Ƿ�������ޣ�ͨ��2��    //�淶20060810
    SHORT  sUlOutUnderPwrThr3;   //3G:�����������Ƿ�������ޣ�ͨ��3��    //�淶20060810
    SHORT  sUlOutUnderPwrThr4;   //3G:�����������Ƿ�������ޣ�ͨ��4��    //�淶20060810

//------------------------------����ΪPOI��������-----------------------------------

    CHAR  cCdma800InUnderPwrThr;// CDMA800���빦��Ƿ��������
    CHAR  cCdma800InOverPwrThr; // CDMA800���빦�ʹ���������

    CHAR  cMbGsmInUnderPwrThr;  // �ƶ�GSM���빦��Ƿ��������
    CHAR  cMbGsmInOverPwrThr;   // �ƶ�GSM���빦�ʹ���������

    CHAR  cUcGsmInUnderPwrThr;  // ��ͨGSM���빦��Ƿ��������
    CHAR  cUcGsmInOverPwrThr;   // ��ͨGSM���빦�ʹ���������

    CHAR  cMbDcsInUnderPwrThr;  //�ƶ�DCS���빦��Ƿ��������
    CHAR  cMbDcsInOverPwrThr;   // �ƶ�DCS���빦�ʹ���������

    CHAR  cUcDcsInUnderPwrThr;  // ��ͨDCS���빦��Ƿ��������
    CHAR  cUcDcsInOverPwrThr;   // ��ͨDCS���빦�ʹ���������

    CHAR  c3G1FDDInUnderPwrThr; // 3G1-FDD���빦��Ƿ��������
    CHAR  c3G1FDDInOverPwrThr;  // 3G1-FDD���빦�ʹ���������

    CHAR  c3G2FDDInUnderPwrThr; // 3G2-FDD���빦��Ƿ��������
    CHAR  c3G2FDDInOverPwrThr;  // 3G2-FDD���빦�ʹ���������

    CHAR  c3G3TDDInUnderPwrThr; // 3G3-TDD���빦��Ƿ��������
    CHAR  c3G3TDDInOverPwrThr;  // 3G3-TDD���빦�ʹ���������

    CHAR  cTrunkInUnderPwrThr;  // ��Ⱥϵͳ���빦��Ƿ��������
    CHAR  cTrunkInOverPwrThr;   // ��Ⱥϵͳ���빦�ʹ���������

//------------------------------����ΪPOI��������-----------------------------------
    
}SETTING_PARAM_SET_ST;                   
                   
//ʵʱ������������ͨ�����г�����Χ��־�Ĳ�����ʱ�����ǣ������Ҫ�������
typedef struct
{
    CHAR   cPaTemp;             //�����¶�ֵ    sint1�ͣ���λΪ��              
    CHAR   cDlInPwr;            //�������빦�ʵ�ƽ  sint1�ͣ���λΪdBm
    UCHAR  ucDlInPwrFlag;       //�������빦�ʵ�ƽ������Χ�ı�־
    CHAR   cDlOutPwr;           //����������ʵ�ƽ  sint1�ͣ���λΪdBm
    UCHAR  ucDlOutPwrFlag;      //����������ʵ�ƽ������Χ�ı�־
    CHAR   cUlTheGain;          //������������  sint1�ͣ���λΪdB              
    CHAR   cDlActGain;          //����ʵ������  sint1�ͣ���λΪdB              
    UCHAR  ucDlActGainFlag;     //����ʵ�����泬����Χ�ı�־
    UCHAR  ucDlSwr;             //����פ����ֵ  UCHAR�ͣ�����Ϊ10  
    UCHAR  ucDlSwrFlag;         //����פ����ֵ������Χ�ı�־
    UCHAR  ucUlSwr;             //����פ����ֵ  UCHAR�ͣ�����Ϊ10          //�淶20060810
    UCHAR  ucUlSwrFlag;         //����פ����ֵ������Χ�ı�־               //�淶20060810
    UCHAR  ucSsMnc;             //��Դ��Ϣ����Ӫ�̴���  UCHAR��                
    USHORT usSsLac;             //��Դ��Ϣ��λ��������  uint2��                
    UCHAR  ucSsBsic;            //��Դ��Ϣ����վʶ����  UCHAR��                
    USHORT usSsBcch;            //��Դ��Ϣ��BCCH������Ƶ��  uint2��            
    CHAR   cSsBcchRxLev;        //��Դ��Ϣ��BCCH���յ�ƽ    sint1�ͣ���λΪdBm 
    UCHAR  ucSsBcchRxLevFlag;   //BCCH���յ�ƽ������Χ�ı�־
    USHORT usSsCi;              //��Դ��Ϣ��С��ʶ����ʵʱֵ    uint2��        
    CHAR   cUlOutPwr;           //����������ʵ�ƽ  sint1�ͣ���λΪdBm         
    UCHAR  ucUlOutPwrFlag;      //����������ʵ�ƽ������Χ�ı�־
    CHAR   cOptRxPwr;           //���չ���  sint1�ͣ���λΪdBm                 
    CHAR   cOptTxPwr;           //�ⷢ����  sint1�ͣ���λΪdBm

//����Ŀǰû��ʹ�ô�ͨ���Ĳ����������Щ�����ĳ�����Χ��־��û�ж���

    CHAR   cDlInPwr1;           //�������빦�ʵ�ƽ��ͨ��1�� sint1�ͣ���λΪdBm 
    CHAR   cDlOutPwr1;          //����������ʵ�ƽ��ͨ��1�� sint1�ͣ���λΪdBm 
    CHAR   cUlTheGain1;         //�����������棨ͨ��1�� sint1�ͣ���λΪdB      
    CHAR   cDlActGain1;         //����ʵ�����棨ͨ��1�� sint1�ͣ���λΪdB      
    CHAR   cUlOutPwr1;          //����������ʵ�ƽ��ͨ��1�� sint1�ͣ���λΪdBm 
    UCHAR  ucDlSwr1;            //����פ����ֵ��ͨ��1�� UCHAR�ͣ�����Ϊ10      
                                               
    CHAR   cDlInPwr2;           //�������빦�ʵ�ƽ��ͨ��2�� sint1�ͣ���λΪdBm 
    UCHAR  ucDlInPwrFlag2;      //�������빦�ʵ�ƽ��ͨ��2�� ������Χ�ı�־
    CHAR   cDlOutPwr2;          //����������ʵ�ƽ��ͨ��2�� sint1�ͣ���λΪdBm 
    UCHAR  ucDlOutPwrFlag2;     //����������ʵ�ƽ��ͨ��2�� ������Χ�ı�־
    CHAR   cUlTheGain2;         //�����������棨ͨ��2�� sint1�ͣ���λΪdB      
    CHAR   cDlActGain2;         //����ʵ�����棨ͨ��2�� sint1�ͣ���λΪdB      
    UCHAR  ucDlActGainFlag2;    //����ʵ�����棨ͨ��2�� ������Χ�ı�־
    CHAR   cUlOutPwr2;          //����������ʵ�ƽ��ͨ��2�� sint1�ͣ���λΪdBm 
    UCHAR  ucUlOutPwrFlag2;     //����������ʵ�ƽ��ͨ��2�� ������Χ�ı�־
    UCHAR  ucDlSwr2;            //����פ����ֵ��ͨ��2�� UCHAR�ͣ�����Ϊ10      
    UCHAR  ucDlSwrFlag2;        //����פ����ֵ��ͨ��2�� ������Χ�ı�־
                                               
    CHAR   cDlInPwr3;           //�������빦�ʵ�ƽ��ͨ��3�� sint1�ͣ���λΪdBm 
    CHAR   cDlOutPwr3;          //����������ʵ�ƽ��ͨ��3�� sint1�ͣ���λΪdBm 
    CHAR   cUlTheGain3;         //�����������棨ͨ��3�� sint1�ͣ���λΪdB      
    CHAR   cDlActGain3;         //����ʵ�����棨ͨ��3�� sint1�ͣ���λΪdB      
    CHAR   cUlOutPwr3;          //����������ʵ�ƽ��ͨ��3�� sint1�ͣ���λΪdBm 
    UCHAR  ucDlSwr3;            //����פ����ֵ��ͨ��3�� UCHAR�ͣ�����Ϊ10      
                                               
    CHAR   cDlInPwr4;           //�������빦�ʵ�ƽ��ͨ��4�� sint1�ͣ���λΪdBm 
    CHAR   cDlOutPwr4;          //����������ʵ�ƽ��ͨ��4�� sint1�ͣ���λΪdBm 
    CHAR   cUlTheGain4;         //�����������棨ͨ��4�� sint1�ͣ���λΪdB      
    CHAR   cDlActGain4;         //����ʵ�����棨ͨ��4�� sint1�ͣ���λΪdB      
    CHAR   cUlOutPwr4;          //����������ʵ�ƽ��ͨ��4�� sint1�ͣ���λΪdBm 
    UCHAR  ucDlSwr4;            //����פ����ֵ��ͨ��4�� UCHAR�ͣ�����Ϊ10      
                                               
    UCHAR  ucUlBypassStatus;    //�����ź���·״̬  bit�ͣ�0��ʾ������1��ʾ��·
    UCHAR  ucDlBypassStatus;    //�����ź���·״̬  bit�ͣ�0��ʾ������1��ʾ��· 

    SHORT  sDlInPwr;            //3G:�������빦�ʵ�ƽ��sint2�ͣ���λΪdBm������Ϊ10������ͬ
    UCHAR  uc3GDlInPwrFlag;     //�������빦�ʵ�ƽ������Χ�ı�־
    SHORT  sDlOutPwr;           //3G:����������ʵ�ƽ
    UCHAR  uc3GDlOutPwrFlag;    //����������ʵ�ƽ������Χ�ı�־
    SHORT  sUlOutPwr;           //3G:����������ʵ�ƽ
    UCHAR  uc3GUlOutPwrFlag;    //����������ʵ�ƽ������Χ�ı�־

//����Ŀǰû��ʹ�ô�ͨ���Ĳ����������Щ�����ĳ�����Χ��־��û�ж���

    SHORT  sDlInPwr1;           //3G:�������빦�ʵ�ƽ��ͨ��1��ͨ��4��
    SHORT  sDlInPwr2;
    SHORT  sDlInPwr3;
    SHORT  sDlInPwr4;

    SHORT  sDlOutPwr1;          //3G:����������ʵ�ƽ��ͨ��1��ͨ��4��
    SHORT  sDlOutPwr2;
    UCHAR  uc3GDlOutPwrFlag2;   //3G:����������ʵ�ƽ��ͨ��2��������Χ�ı�־
    SHORT  sDlOutPwr3;
    SHORT  sDlOutPwr4;

    SHORT  sUlOutPwr1;          //3G:����������ʵ�ƽ��ͨ��1��ͨ��4��
    SHORT  sUlOutPwr2;
    SHORT  sUlOutPwr3;
    SHORT  sUlOutPwr4;

    SHORT  sDonorCpichLev;      //3G:ʩ���˿�CPICH���ʵ�ƽ
    SHORT  sServiceCpichLev;    //3G:�û��˿�CPICH���ʵ�ƽ
    
    //--------- ����Ϊ������� ---------
    USHORT usModuleBatValue;    //���ģ���ص�ѹ��û�ж�Ӧ�Ĳ���ID
    //--------- ����Ϊ������� --------- 
    //zhangjie 20071120 ̫�������ص�ѹ
    USHORT usSecBatVol;
    //zhangjie 20071120 ̫�������ص�ѹ

//------------------------------����ΪPOI��������-----------------------------------

    CHAR  cCdma800InPwr;        //CDMA800���빦�ʵ�ƽֵ
    UCHAR ucCdma800InPwrFlag;   //������Χ�ı�־
    CHAR  cMbGsmInPwr;          //�ƶ�GSM���빦�ʵ�ƽֵ
    UCHAR uccMbGsmInPwrFlag;    //������Χ�ı�־
    CHAR  cUcGsmInPwr;          //��ͨGSM���빦�ʵ�ƽֵ
    UCHAR uccUcGsmInPwrFlag;    //������Χ�ı�־
    CHAR  cMbDcsInPwr;          //�ƶ�DCS���빦�ʵ�ƽֵ
    UCHAR uccMbDcsInPwrFlag;    //������Χ�ı�־
    CHAR  cUcDcsInPwr;          //��ͨDCS���빦�ʵ�ƽֵ
    UCHAR uccUcDcsInPwrFlag;    //������Χ�ı�־
    CHAR  c3G1FDDInPwr;         //3G1-FDD���빦�ʵ�ƽֵ
    UCHAR ucc3G1FDDInPwrFlag;   //������Χ�ı�־
    CHAR  c3G2FDDInPwr;         //3G2-FDD���빦�ʵ�ƽֵ
    UCHAR ucc3G2FDDInPwrFlag;   //������Χ�ı�־
    CHAR  c3G3TDDInPwr;         //3G3-TDD���빦�ʵ�ƽֵ
    UCHAR ucc3G3TDDInPwrFlag;   //������Χ�ı�־
    CHAR  cTrunkInPwr;          //��Ⱥϵͳ���빦�ʵ�ƽֵ
    UCHAR uccTrunkInPwrFlag;    //������Χ�ı�־

//------------------------------����ΪPOI��������-----------------------------------
    
}RC_PARAM_SET_ST;                   


//�澯ʹ��
typedef struct   
{                               
    UCHAR ucPwrLost;           //��Դ����澯                
    UCHAR ucPwrFault;          //��Դ���ϸ澯                
    UCHAR ucSolarBatLowPwr;    //̫���ܵ�ص͵�ѹ�澯        
    UCHAR ucBatFalut;          //���ģ���ع��ϸ澯        
    UCHAR ucPosition;          //λ�ø澯                    
    UCHAR ucPaOverheat;        //���Ź��¸澯                
    UCHAR ucSrcSigChanged;     //��Դ�仯�澯                
    UCHAR ucOtherModule;       //����ģ��澯                
    UCHAR ucOscUnlocked;       //����ʧ���澯                
    UCHAR ucUlLna;             //���е���Ź��ϸ澯          
    UCHAR ucDlLna;             //���е���Ź��ϸ澯          
    UCHAR ucUlPa;              //���й��Ÿ澯                
    UCHAR ucDlPa;              //���й��Ÿ澯                
    UCHAR ucOptModule;         //���շ�ģ����ϸ澯          
    UCHAR ucMsLink;            //���Ӽ����·�澯            
    UCHAR ucDlInOverPwr;       //������������ʸ澯          
    UCHAR ucDlInUnderPwr;      //��������Ƿ���ʸ澯          
    UCHAR ucDlOutOverPwr;      //������������ʸ澯          
    UCHAR ucDlOutUnderPwr;     //�������Ƿ���ʸ澯          
    UCHAR ucDlSwr;             //����פ���ȸ澯              
    UCHAR ucUlOutOverPwr;      //������������ʸ澯
    UCHAR ucUlOutUnderPwr;     //�������Ƿ���ʸ澯 //�淶20060810
    UCHAR ucUlSwr;             //����פ���ȸ澯    //�淶20060810
    UCHAR ucExt1;              //�ⲿ�澯1��8                
    UCHAR ucExt2;                                            
    UCHAR ucExt3;                                            
    UCHAR ucExt4;                                            
    UCHAR ucExt5;                                            
    UCHAR ucExt6;                                            
    UCHAR ucExt7;                                            
    UCHAR ucExt8;                                            
    UCHAR ucDoor;              //�Ž��澯                    
    UCHAR ucSelfOsc;           //�Լ��澯                    
    UCHAR ucGprsLoginFailed;   //3G:GPRS��¼ʧ�ܸ澯���Ѿ���ɾ��

    UCHAR ucDlInOverPwr1;      //������������ʸ澯��ͨ��1�� 
    UCHAR ucDlInUnderPwr1;     //��������Ƿ���ʸ澯��ͨ��1�� 
    UCHAR ucDlOutOverPwr1;     //������������ʸ澯��ͨ��1�� 
    UCHAR ucDlOutUnderPwr1;    //�������Ƿ���ʸ澯��ͨ��1�� 
    UCHAR ucUlOutOverPwr1;     //������������ʸ澯��ͨ��1�� 
    UCHAR ucDlSwr1;            //����פ���ȸ澯    ��ͨ��1��     

    UCHAR ucDlInOverPwr2;      //������������ʸ澯��ͨ��2�� 
    UCHAR ucDlInUnderPwr2;     //��������Ƿ���ʸ澯��ͨ��2�� 
    UCHAR ucDlOutOverPwr2;     //������������ʸ澯��ͨ��2�� 
    UCHAR ucDlOutUnderPwr2;    //�������Ƿ���ʸ澯��ͨ��2�� 
    UCHAR ucUlOutOverPwr2;     //������������ʸ澯��ͨ��2�� 
    UCHAR ucDlSwr2;            //����פ���ȸ澯    ��ͨ��2��     

    UCHAR ucDlInOverPwr3;      //������������ʸ澯��ͨ��3�� 
    UCHAR ucDlInUnderPwr3;     //��������Ƿ���ʸ澯��ͨ��3�� 
    UCHAR ucDlOutOverPwr3;     //������������ʸ澯��ͨ��3�� 
    UCHAR ucDlOutUnderPwr3;    //�������Ƿ���ʸ澯��ͨ��3�� 
    UCHAR ucUlOutOverPwr3;     //������������ʸ澯��ͨ��3�� 
    UCHAR ucDlSwr3;            //����פ���ȸ澯    ��ͨ��3��     

    UCHAR ucDlInOverPwr4;      //������������ʸ澯��ͨ��4�� 
    UCHAR ucDlInUnderPwr4;     //��������Ƿ���ʸ澯��ͨ��4�� 
    UCHAR ucDlOutOverPwr4;     //������������ʸ澯��ͨ��4�� 
    UCHAR ucDlOutUnderPwr4;    //�������Ƿ���ʸ澯��ͨ��4�� 
    UCHAR ucUlOutOverPwr4;     //������������ʸ澯��ͨ��4�� 
    UCHAR ucDlSwr4;            //����פ���ȸ澯    ��ͨ��4��     

//MCM-25_20061108_zhonghw_begin
    UCHAR ucUlOutUnderPwr1;    //�������Ƿ���ʸ澯��ͨ��1��      //�淶20060810
    UCHAR ucUlOutUnderPwr2;    //�������Ƿ���ʸ澯��ͨ��2��      //�淶20060810
    UCHAR ucUlOutUnderPwr3;    //�������Ƿ���ʸ澯��ͨ��3��      //�淶20060810  
    UCHAR ucUlOutUnderPwr4;    //�������Ƿ���ʸ澯��ͨ��4��     //�淶20060810
    UCHAR ucUlSwr1;            //����פ���ȸ澯    ��ͨ��1��      //�淶20060810
    UCHAR ucUlSwr2;            //����פ���ȸ澯    ��ͨ��2��      //�淶20060810      
    UCHAR ucUlSwr3;            //����פ���ȸ澯    ��ͨ��3��      //�淶20060810          
    UCHAR ucUlSwr4;            //����פ���ȸ澯    ��ͨ��4��     //�淶20060810     
//MCM-25_20061108_zhonghw_end

    UCHAR ucTta1;              //�����Ŵ����澯ʹ��1��8      
    UCHAR ucTta2;                                            
    UCHAR ucTta3;                                            
    UCHAR ucTta4;                                            
    UCHAR ucTta5;                                            
    UCHAR ucTta6;             
    UCHAR ucTta7;                                    
    UCHAR ucTta8;                                    
    UCHAR ucPaBypass1;         //������·�澯ʹ��1��8
    UCHAR ucPaBypass2;                               
    UCHAR ucPaBypass3;                               
    UCHAR ucPaBypass4;                               
    UCHAR ucPaBypass5;                               
    UCHAR ucPaBypass6;                               
    UCHAR ucPaBypass7;                               
    UCHAR ucPaBypass8;                               
    UCHAR ucSelfOscShutdown;   //3G:�Լ��ػ������澯    
    UCHAR ucSelfOscReduceAtt;  //3G:�Լ������汣���澯  

//���Ϲ�74��
//------------------------------����ΪPOI������������15��-----------------------------------

    UCHAR  ucCdma800InOverPwr;  //CDMA800��������ʸ澯
    UCHAR  ucCdma800InUnderPwr; //CDMA800����Ƿ���ʸ澯

    UCHAR  ucMbGsmInOverPwr;    //�ƶ�GSM��������ʸ澯
    UCHAR  ucMbGsmInUnderPwr;   //�ƶ�GSM����Ƿ���ʸ澯

    UCHAR  ucUcGsmInOverPwr;    //��ͨGSM��������ʸ澯
    UCHAR  ucUcGsmInUnderPwr;   //��ͨGSM/����Ƿ���ʸ澯

    UCHAR  ucMbDcsInOverPwr;    //�ƶ�DCS��������ʸ澯
    UCHAR  ucMbDcsInUnderPwr;   //�ƶ�DCS����Ƿ���ʸ澯

    UCHAR  ucUcDcsInOverPwr;    //��ͨDCS��������ʸ澯
    UCHAR  ucUcDcsInUnderPwr;   //��ͨDCS����Ƿ���ʸ澯

    UCHAR  uc3G1FDDInOverPwr;   //3G1-FDD��������ʸ澯
    UCHAR  uc3G1FDDInUnderPwr;  //3G1-FDD����Ƿ���ʸ澯

    UCHAR  uc3G2FDDInOverPwr;   //3G2-FDD��������ʸ澯
    UCHAR  uc3G2FDDInUnderPwr;  //3G2-FDD����Ƿ���ʸ澯

    UCHAR  uc3G3TDDInOverPwr;   //3G3-TDD��������ʸ澯
    UCHAR  uc3G3TDDInUnderPwr;  //3G3-TDD����Ƿ���ʸ澯

    UCHAR  ucTrunkInOverPwr;    //��Ⱥϵͳ��������ʸ澯
    UCHAR  ucTrunkInUnderPwr;   //��Ⱥϵͳ����Ƿ���ʸ澯

//------------------------------����ΪPOI������������15��-----------------------------------

//------------------------------����Ϊ̫���ܿ�����������������15��----------------------------

    UCHAR ucSecStBatBlowout;   //���ر���˿��
    UCHAR ucSecSelfResumeBlowout;//����Իָ�����˿��
    UCHAR ucSecOutputOverCur;  //�������, over current
    UCHAR ucSecCircuitFault;   //���Ƶ�·����
    UCHAR ucSecVol24fault;     //24V���ع���
    UCHAR ucSecVol12Fault;     //12V���ع���
    UCHAR ucSecVol5Fault;      //5V���ع���
    UCHAR ucSecUseUnderVol;    //Ƿѹʹ����ʾ
    UCHAR ucSecOverVol;        //���ع�ѹ�澯
    UCHAR ucSecOverDischarging;//����Ԥ���Ÿ澯
    UCHAR ucSecOverDischarged; //���ع��Ÿ澯
    UCHAR ucSecStBatFault;     //���ع���
    UCHAR ucSecSBFault;        //������
    UCHAR ucSecDoor;           //�Ž��澯
    UCHAR ucSecAleak;          //ˮ©�澯

//------------------------------����Ϊ̫���ܿ�����������������15��----------------------------


//--------- ���������ڲ�ʹ�ã����ڲ����Լ�������澯�͹ػ������澯����˱��������󣬷���ᵼ��ID��λ
    UCHAR ucUlSelfOsc;         //�����Լ��澯��û�ж�Ӧ�Ĳ���ID
    UCHAR ucDlSelfOsc;         //�����Լ��澯��û�ж�Ӧ�Ĳ���ID

}ALARM_ITEM_BYTE_ST; 

typedef union
{
    UCHAR aucAlarmEnableArray[1];
    ALARM_ITEM_BYTE_ST  stAlarmEnableStruct;
}ALARM_ENABLE_UN;

typedef union
{
    UCHAR aucAlarmStatusArray[1];
    ALARM_ITEM_BYTE_ST stAlarmStatusStruct;
}ALARM_STATUS_UN;

//ÿ���澯�����������Ϣ
typedef struct
{
    UCHAR ucIDLoByte;

    UCHAR ucSupportedBit : 1; //��ʾ�Ƿ�֧�ָø澯�0����1���ǣ����豸���;���
  //UCHAR ucEnabledBit   : 1; //��ʾʹ��״̬��0:ʹ�ܹأ�1:ʹ�ܿ�
    UCHAR ucEffectiveBit : 1; //��ʾ�Ƿ���Ч����ʹ�ܿ���δ������ʱ��Ч��������Ч
  //UCHAR ucCenterStatus : 1; //��ʾ���ĸ澯״̬��0:����,1:�澯
    UCHAR ucLocalStatus  : 1; //��ʾ���ظ澯״̬��0:����,1:�澯�����ǲ�ѯʱ��õĸ澯״̬
    UCHAR ucRcStatus     : 1; //�澯ʵʱ�ɼ�״̬��0:����,1:�澯
    UCHAR ucIsInstant    : 1; //��ʾ�����澯������Ҫ���и澯����

    UCHAR ucTotalCount;   //���澯���Ѿ��ɼ��Ĵ���
    UCHAR ucChangedCount; //���澯������ڱ��ظ澯״̬�仯�Ĵ���
}ALARM_ITEM_INFO_ST;


//�澯״̬
typedef struct   
{                               
    ALARM_ITEM_INFO_ST stPwrLost;           //��Դ����澯                
    ALARM_ITEM_INFO_ST stPwrFault;          //��Դ���ϸ澯                
    ALARM_ITEM_INFO_ST stSolarBatLowPwr;    //̫���ܵ�ص͵�ѹ�澯        
    ALARM_ITEM_INFO_ST stBatFalut;          //���ģ���ع��ϸ澯        
    ALARM_ITEM_INFO_ST stPosition;          //λ�ø澯                    
    ALARM_ITEM_INFO_ST stPaOverheat;        //���Ź��¸澯                
    ALARM_ITEM_INFO_ST stSrcSigChanged;     //��Դ�仯�澯                
    ALARM_ITEM_INFO_ST stOtherModule;       //����ģ��澯                
    ALARM_ITEM_INFO_ST stOscUnlocked;       //����ʧ���澯                
    ALARM_ITEM_INFO_ST stUlLna;             //���е���Ź��ϸ澯          
    ALARM_ITEM_INFO_ST stDlLna;             //���е���Ź��ϸ澯          
    ALARM_ITEM_INFO_ST stUlPa;              //���й��Ÿ澯                
    ALARM_ITEM_INFO_ST stDlPa;              //���й��Ÿ澯                
    ALARM_ITEM_INFO_ST stOptModule;         //���շ�ģ����ϸ澯          
    ALARM_ITEM_INFO_ST stMsLink;            //���Ӽ����·�澯            
    ALARM_ITEM_INFO_ST stDlInOverPwr;       //������������ʸ澯          
    ALARM_ITEM_INFO_ST stDlInUnderPwr;      //��������Ƿ���ʸ澯          
    ALARM_ITEM_INFO_ST stDlOutOverPwr;      //������������ʸ澯          
    ALARM_ITEM_INFO_ST stDlOutUnderPwr;     //�������Ƿ���ʸ澯          
    ALARM_ITEM_INFO_ST stDlSwr;             //����פ���ȸ澯              
    ALARM_ITEM_INFO_ST stUlOutOverPwr;      //������������ʸ澯          
    ALARM_ITEM_INFO_ST stUlOutUnderPwr;     //�������Ƿ���ʸ澯                //�淶20060810
    ALARM_ITEM_INFO_ST stUlSwr;             //����פ���ȸ澯                    //�淶20060810
    ALARM_ITEM_INFO_ST stExt1;              //�ⲿ�澯1��8                
    ALARM_ITEM_INFO_ST stExt2;                                            
    ALARM_ITEM_INFO_ST stExt3;                                            
    ALARM_ITEM_INFO_ST stExt4;                                            
    ALARM_ITEM_INFO_ST stExt5;                                            
    ALARM_ITEM_INFO_ST stExt6;                                            
    ALARM_ITEM_INFO_ST stExt7;                                            
    ALARM_ITEM_INFO_ST stExt8;                                            
    ALARM_ITEM_INFO_ST stDoor;              //�Ž��澯                    
    ALARM_ITEM_INFO_ST stSelfOsc;           //�Լ��澯                    
    ALARM_ITEM_INFO_ST stGprsLoginFailed;   //3G:GPRS��¼ʧ�ܸ澯���Ѿ���ɾ��

    ALARM_ITEM_INFO_ST stDlInOverPwr1;      //������������ʸ澯 ��ͨ��1�� 
    ALARM_ITEM_INFO_ST stDlInUnderPwr1;     //��������Ƿ���ʸ澯 ��ͨ��1�� 
    ALARM_ITEM_INFO_ST stDlOutOverPwr1;     //������������ʸ澯 ��ͨ��1�� 
    ALARM_ITEM_INFO_ST stDlOutUnderPwr1;    //�������Ƿ���ʸ澯 ��ͨ��1�� 
    ALARM_ITEM_INFO_ST stUlOutOverPwr1;     //������������ʸ澯 ��ͨ��1�� 
    ALARM_ITEM_INFO_ST stDlSwr1;            //����פ���ȸ澯     ��ͨ��1��     

    ALARM_ITEM_INFO_ST stDlInOverPwr2;      //������������ʸ澯 ��ͨ��2�� 
    ALARM_ITEM_INFO_ST stDlInUnderPwr2;     //��������Ƿ���ʸ澯 ��ͨ��2�� 
    ALARM_ITEM_INFO_ST stDlOutOverPwr2;     //������������ʸ澯 ��ͨ��2�� 
    ALARM_ITEM_INFO_ST stDlOutUnderPwr2;    //�������Ƿ���ʸ澯 ��ͨ��2�� 
    ALARM_ITEM_INFO_ST stUlOutOverPwr2;     //������������ʸ澯 ��ͨ��2�� 
    ALARM_ITEM_INFO_ST stDlSwr2;            //����פ���ȸ澯     ��ͨ��2��     

    ALARM_ITEM_INFO_ST stDlInOverPwr3;      //������������ʸ澯 ��ͨ��3�� 
    ALARM_ITEM_INFO_ST stDlInUnderPwr3;     //��������Ƿ���ʸ澯 ��ͨ��3�� 
    ALARM_ITEM_INFO_ST stDlOutOverPwr3;     //������������ʸ澯 ��ͨ��3�� 
    ALARM_ITEM_INFO_ST stDlOutUnderPwr3;    //�������Ƿ���ʸ澯 ��ͨ��3�� 
    ALARM_ITEM_INFO_ST stUlOutOverPwr3;     //������������ʸ澯 ��ͨ��3�� 
    ALARM_ITEM_INFO_ST stDlSwr3;            //����פ���ȸ澯     ��ͨ��3��     

    ALARM_ITEM_INFO_ST stDlInOverPwr4;      //������������ʸ澯 ��ͨ��4�� 
    ALARM_ITEM_INFO_ST stDlInUnderPwr4;     //��������Ƿ���ʸ澯 ��ͨ��4�� 
    ALARM_ITEM_INFO_ST stDlOutOverPwr4;     //������������ʸ澯 ��ͨ��4�� 
    ALARM_ITEM_INFO_ST stDlOutUnderPwr4;    //�������Ƿ���ʸ澯 ��ͨ��4�� 
    ALARM_ITEM_INFO_ST stUlOutOverPwr4;     //������������ʸ澯 ��ͨ��4�� 
    ALARM_ITEM_INFO_ST stDlSwr4;            //����פ���ȸ澯     ��ͨ��4��     
//MCM-25_20061108_zhonghw_begin
    ALARM_ITEM_INFO_ST stUlOutUnderPwr1;    //�������Ƿ���ʸ澯��ͨ��1��        //�淶20060810
    ALARM_ITEM_INFO_ST stUlOutUnderPwr2;    //�������Ƿ���ʸ澯��ͨ��2��        //�淶20060810
    ALARM_ITEM_INFO_ST stUlOutUnderPwr3;    //�������Ƿ���ʸ澯��ͨ��3��        //�淶20060810    
    ALARM_ITEM_INFO_ST stUlOutUnderPwr4;     //�������Ƿ���ʸ澯��ͨ��4��       //�淶20060810
    ALARM_ITEM_INFO_ST stUlSwr1;            //����פ���ȸ澯    ��ͨ��1��        //�淶20060810    
    ALARM_ITEM_INFO_ST stUlSwr2;            //����פ���ȸ澯    ��ͨ��2��        //�淶20060810
    ALARM_ITEM_INFO_ST stUlSwr3;            //����פ���ȸ澯    ��ͨ��3��        //�淶20060810    
    ALARM_ITEM_INFO_ST stUlSwr4;             //����פ���ȸ澯    ��ͨ��4��       //�淶20060810
//MCM-25_20061108_zhonghw_end

    ALARM_ITEM_INFO_ST stTta1;              //�����Ŵ����澯ʹ��1��8      
    ALARM_ITEM_INFO_ST stTta2;                                            
    ALARM_ITEM_INFO_ST stTta3;                                            
    ALARM_ITEM_INFO_ST stTta4;                                            
    ALARM_ITEM_INFO_ST stTta5;                                            
    ALARM_ITEM_INFO_ST stTta6;             
    ALARM_ITEM_INFO_ST stTta7;                                    
    ALARM_ITEM_INFO_ST stTta8;                                    
    ALARM_ITEM_INFO_ST stPaBypass1;         //������·�澯ʹ��1��8
    ALARM_ITEM_INFO_ST stPaBypass2;                               
    ALARM_ITEM_INFO_ST stPaBypass3;                               
    ALARM_ITEM_INFO_ST stPaBypass4;                               
    ALARM_ITEM_INFO_ST stPaBypass5;                               
    ALARM_ITEM_INFO_ST stPaBypass6;                               
    ALARM_ITEM_INFO_ST stPaBypass7;                               
    ALARM_ITEM_INFO_ST stPaBypass8;                               
    ALARM_ITEM_INFO_ST stSelfOscShutdown;   //3G:�Լ��ػ������澯    
    ALARM_ITEM_INFO_ST stSelfOscReduceAtt;  //3G:�Լ������汣���澯  

//���Ϲ�74��
//------------------------------����ΪPOI������������18��-----------------------------------

    ALARM_ITEM_INFO_ST stCdma800InOverPwr;  // CDMA800��������ʸ澯
    ALARM_ITEM_INFO_ST stCdma800InUnderPwr; // CDMA800����Ƿ���ʸ澯

    ALARM_ITEM_INFO_ST stMbGsmInOverPwr;    // �ƶ�GSM��������ʸ澯
    ALARM_ITEM_INFO_ST stMbGsmInUnderPwr;   // �ƶ�GSM����Ƿ���ʸ澯

    ALARM_ITEM_INFO_ST stUcGsmInOverPwr;    // ��ͨGSM��������ʸ澯
    ALARM_ITEM_INFO_ST stUcGsmInUnderPwr;   // ��ͨGSM����Ƿ���ʸ澯

    ALARM_ITEM_INFO_ST stMbDcsInOverPwr;    // �ƶ�DCS��������ʸ澯
    ALARM_ITEM_INFO_ST stMbDcsInUnderPwr;   // �ƶ�DCS����Ƿ���ʸ澯

    ALARM_ITEM_INFO_ST stUcDcsInOverPwr;    // ��ͨDCS��������ʸ澯
    ALARM_ITEM_INFO_ST stUcDcsInUnderPwr;   // ��ͨDCS����Ƿ���ʸ澯

    ALARM_ITEM_INFO_ST st3G1FDDInOverPwr;   // 3G1-FDD��������ʸ澯
    ALARM_ITEM_INFO_ST st3G1FDDInUnderPwr;  // 3G1-FDD����Ƿ���ʸ澯

    ALARM_ITEM_INFO_ST st3G2FDDInOverPwr;   // 3G2-FDD��������ʸ澯
    ALARM_ITEM_INFO_ST st3G2FDDInUnderPwr;  // 3G2-FDD����Ƿ���ʸ澯

    ALARM_ITEM_INFO_ST st3G3TDDInOverPwr;   // 3G3-TDD��������ʸ澯
    ALARM_ITEM_INFO_ST st3G3TDDInUnderPwr;  // 3G3-TDD����Ƿ���ʸ澯

    ALARM_ITEM_INFO_ST stTrunkInOverPwr;    // ��Ⱥϵͳ��������ʸ澯
    ALARM_ITEM_INFO_ST stTrunkInUnderPwr;   // ��Ⱥϵͳ����Ƿ���ʸ澯

//------------------------------����ΪPOI������������18��-----------------------------------

//------------------------------����Ϊ̫���ܿ�����������������15��----------------------------

    ALARM_ITEM_INFO_ST stSecStBatBlowout;   //���ر���˿��
    ALARM_ITEM_INFO_ST stSecSelfResumeBlowout;//����Իָ�����˿��
    ALARM_ITEM_INFO_ST stSecOutputOverCur;  //�������, over current
    ALARM_ITEM_INFO_ST stSecCircuitFault;   //���Ƶ�·����
    ALARM_ITEM_INFO_ST stSecVol24fault;     //24V���ع���
    ALARM_ITEM_INFO_ST stSecVol12Fault;     //12V���ع���
    ALARM_ITEM_INFO_ST stSecVol5Fault;      //5V���ع���
    ALARM_ITEM_INFO_ST stSecUseUnderVol;    //Ƿѹʹ����ʾ
    ALARM_ITEM_INFO_ST stSecOverVol;        //���ع�ѹ�澯
    ALARM_ITEM_INFO_ST stSecOverDischarging;//����Ԥ���Ÿ澯
    ALARM_ITEM_INFO_ST stSecOverDischarged; //���ع��Ÿ澯
    ALARM_ITEM_INFO_ST stSecStBatFault;     //���ع���
    ALARM_ITEM_INFO_ST stSecSBFault;        //������
    ALARM_ITEM_INFO_ST stSecDoor;           //�Ž��澯
    ALARM_ITEM_INFO_ST stSecAleak;          //ˮ©�澯

//------------------------------����Ϊ̫���ܿ�����������������15��----------------------------

//--------- ���������ڲ�ʹ�ã����ڲ����Լ�������澯�͹ػ������澯����˱��������󣬷���ᵼ��ID��λ
    ALARM_ITEM_INFO_ST stUlSelfOsc;         //�����Լ��澯��û�ж�Ӧ�Ĳ���ID
    ALARM_ITEM_INFO_ST stDlSelfOsc;         //�����Լ��澯��û�ж�Ӧ�Ĳ���ID
    
}ALARM_ITEM_SET_ST;

#define MAX_ALARM_ITEMS                 ((sizeof(ALARM_ITEM_SET_ST)) / sizeof(ALARM_ITEM_INFO_ST))


//����ʼ����ʱ�����ȳ�ʼ�����и澯���ID��״̬�������Ҫ�����澯�
//��ʹ�����鷽ʽ���ʣ������Ҫ���ĳ��澯��������ʹ�ýṹ��ʽ���ʡ�
//����ʱ����ֱ�ӻ�ø澯���ID������ʱ����ͨ�����ַ����Ҷ�ӦID�ĸ澯�
//�����鷽ʽ���ʵ��澯��ʱ������ʹ������Ľṹ���ʸ澯������ݡ�
typedef union
{
    ALARM_ITEM_INFO_ST astAlarmItemArray[1];
    ALARM_ITEM_SET_ST  stAlarmItemStruct;
}ALARM_ITEM_UN;

//��Ҫ���浽NV Memory�еļ����Ϣ
#ifndef M3
#define ADDR_CFG_FILE_BASE       (0x1180000)
#else
#define ADDR_CFG_FILE_BASE       (0)
#endif

//ֻ����������һ��sector��
#ifndef M3
#define ADDR_MOBJ_ID_TBL         (ADDR_CFG_FILE_BASE + 0)
#define ADDR_DEV_INFO_SET        (ADDR_CFG_FILE_BASE + 0x800)
#else
#define ADDR_MOBJ_ID_TBL         ((ULONG)&g_stMObjIdTbl)
#define ADDR_DEV_INFO_SET        ((ULONG)&g_stDevInfoSet)
#endif

//��д��������һ��sector��
#ifndef M3
#define ADDR_NM_PARAM_SET        (ADDR_CFG_FILE_BASE + 0x1000)
#define ADDR_ALARM_ENABLE_SET    (ADDR_CFG_FILE_BASE + 0x1400)
#define ADDR_SETTING_PARAM_SET   (ADDR_CFG_FILE_BASE + 0x1800)
#else
#define ADDR_NM_PARAM_SET        ((ULONG)&g_stNmParamSet)
#define ADDR_ALARM_ENABLE_SET    ((ULONG)&g_uAlarmEnable)
#define ADDR_SETTING_PARAM_SET   ((ULONG)&g_stSettingParamSet)
#endif

//�澯����״̬��������һ��sector��
#ifndef M3
#define ADDR_CENTER_STATUS_SET   (ADDR_CFG_FILE_BASE + 0x2000)
#else
#define ADDR_CENTER_STATUS_SET   ((ULONG)&g_uCenterStatus)
#endif

//��Զ��������صķ���һ��sector��
#ifndef M3
#define ADDR_UPGRADE_INFO_SET    (ADDR_CFG_FILE_BASE + 0x3000)
#define ADDR_FILE_PART_TBL       (ADDR_CFG_FILE_BASE + 0x3800)
#else
#define ADDR_UPGRADE_INFO_SET    ((ULONG)&g_stUpgradeCtx)
#define ADDR_FILE_PART_TBL       ((ULONG)&g_astFilePartTbl)
#endif

//����˽�в�������һ��sector��
#ifndef M3
#define ADDR_YKPP_PARAM_SET   (ADDR_CFG_FILE_BASE + 0x4000)
#else
#define ADDR_YKPP_PARAM_SET   ((ULONG)&g_stYkppParamSet)
#endif

//GPRSͨ�ŷ�ʽʱ���غ����ĵ�����״̬
typedef struct
{
    UCHAR ucLinkStatus;
    UCHAR ucLoginStatus;
    UCHAR ucConnAttempCnt;      //���ӳ��Խ������ӵĴ�����ÿʧ��3�ξ��ϱ�ͨ����·����
    UCHAR ucLoginTimeoutCnt;    //��¼��ʱ�Ĵ�������ʱ2�μ��Ͽ��������µ�¼
    UCHAR ucNoRspCnt;           //������ʱ�Ĵ�������ʱ2�μ��Ͽ��������µ�¼    
}CENTER_CONN_STATUS_ST;

#define COMM_STATUS_DISCONNECTED    0 //δ���ӣ���ʾGPRSδ���ӣ�����TCPδ����
#define COMM_STATUS_CONNECTED       1 //�����ӣ���ʾGPRS���ӳɹ�����TCP���ӳɹ�

#define COMM_STATUS_LOGOUT          2 //δ��¼
#define COMM_STATUS_LOGIN           3 //�ѵ�¼

#define MAX_CONN_ATTEMP_CNT         3
#define MAX_LOGIN_TO_CNT            2
#define MAX_HEARTBEAT_TO_CNT        2

//***************************3G���Ͷ���******************  
#define WIDE_BAND                              51    //���ֱ��վ
#define WIRELESS_FRESELT                       52    //ѡƵֱ��վ
#define OPTICAL_DIRECOUPL_LOCAL                53    //����ֱ��վֱ����Ͻ��˻�
#define OPTICAL_WIDEBAND_FAR                   54    //����ֱ��վ���Զ�˻�
#define TRUNK_AMPLIFIER                        55    //���߷Ŵ��� 
#define FRESHIFT_FRESELT_FAR                   56    //��Ƶֱ��վѡƵԶ�˻�
#define FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL      57    //��Ƶֱ��վֱ����Ͽ�����˻�
#define FRESHIFT_WIRELESS_WIDEBAND_LOCAL       62    //��Ƶֱ��վ������Ͽ�����˻�
#define OPTICAL_FRESELT_FAR                    63    //����ֱ��վѡƵԶ�˻�
#define OPTICAL_CARRIER_WIDEBAND_LOCAL         64    //�����ز��ؿ����վ�˻�
#define OPTICAL_CARRIER_FRESELT_LOCAL          65    //�����ز���ѡƵ��վ�˻�
#define OPTICAL_CARRIER_WIDEBAND_FAR           66    //�����ز��ؿ��Զ�˻�
#define OPTICAL_CARRIER_FRESELT_FAR            67    //�����ز���ѡƵԶ�˻�
#define OPTICAL_WIRELESSCOUPL_LOCAL            68    //����ֱ��վ������Ͻ��˻�
#define FRESHIFT_WIDEBAND_FAR                  69    //��Ƶֱ��վ���Զ�˻�
#define FRESHIFT_DIRECOUPL_FRESELT_LOCAL       70    //��Ƶֱ��վֱ�����ѡƵ���˻�
#define FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL   71    //��Ƶֱ��վ�������ѡƵ���˻�
#define BS_AMPLIFIER                           72    //��վ�Ŵ��� 
#define POI_STAKEOUT                           90    //POIר���豸����

#define DEV_WITH_SEC                           100   //����̫���ܿ�����
//***************************3G���Ͷ���******************
//***************************2G���Ͷ���******************
#define WIDE_BAND_2G                               1    //���ֱ��վ
#define WIRELESS_FRESELT_2G                        2    //ѡƵֱ��վ
#define OPTICAL_DIRECOUPL_LOCAL_2G                 3    //����ֱ��վֱ����Ͻ��˻�
#define OPTICAL_WIDEBAND_FAR_2G                    4    //����ֱ��վ���Զ�˻�
#define TRUNK_AMPLIFIER_2G                         5    //���߷Ŵ���
#define FRESHIFT_FRESELT_FAR_2G                    6    //��Ƶֱ��վѡƵԶ�˻�
#define FRESHIFT_DIRECOUPL_WIDEBAND_LOCAL_2G       7    //��Ƶֱ��վֱ����Ͽ�����˻�

//MCM-63_20070315_Zhonghw_begin
#define BI_FRE_OPTICAL_FAR_2G                      8    //˫Ƶ����ֱ��վԶ�˻�
#define BI_FRE_OPTICAL_DIRECOUPL_LOCAL_2G          9    //˫Ƶ����ֱ��վֱ����Ͻ��˻�
#define BI_DIRECT_WIDE_BAND_TOWER_AMP_2G          10    //˫��������
#define BI_DIRECT_FRE_SELECT_TOWER_AMP_2G         11    //˫��ѡƵ����
//MCM-63_20070315_Zhonghw_end
#define FRESHIFT_WIRELESS_WIDEBAND_LOCAL_2G       12    //��Ƶֱ��վ������Ͽ�����˻�
#define OPTICAL_FRESELT_FAR_2G                    13    //����ֱ��վѡƵԶ�˻�
#define OPTICAL_CARRIER_WIDEBAND_LOCAL_2G         14    //�����ز��ؿ����վ�˻�
#define OPTICAL_CARRIER_FRESELT_LOCAL_2G          15    //�����ز���ѡƵ��վ�˻�
#define OPTICAL_CARRIER_WIDEBAND_FAR_2G           16    //�����ز��ؿ��Զ�˻�
#define OPTICAL_CARRIER_FRESELT_FAR_2G            17    //�����ز���ѡƵԶ�˻�
#define OPTICAL_WIRELESSCOUPL_LOCAL_2G            18    //����ֱ��վ������Ͻ��˻�
#define FRESHIFT_WIDEBAND_FAR_2G                  19    //��Ƶֱ��վ���Զ�˻�
#define FRESHIFT_DIRECOUPL_FRESELT_LOCAL_2G       20    //��Ƶֱ��վֱ�����ѡƵ���˻�
#define FRESHIFT_WIRELESSCOUPL_FRESELT_LOCAL_2G   21    //��Ƶֱ��վ�������ѡƵ���˻�
#define BS_AMPLIFIER_2G                           22    //��վ�Ŵ��� 
//***************************2G���Ͷ���******************
//****************************���Ի���*******************
#define MACHINE_FOR_TEST                          99
//****************************���Ի���*******************

//MCM-11_20061101_zhangjie_begin
//������"ѡƵ����"����
#define FREQTYPE_WIDEBAND           0   //�����
#define FREQTYPE_FRESELT            1   //ѡƵ��
#define FREQTYPE_FRESHIFT           2   //��Ƶ��
#define FREQTYPE_FRESHIFT_WIDEBAND  3   //��Ƶ�����

//������"2G3G"����
#define GEN_TYPE_2G         0           //2G����
#define GEN_TYPE_3G         1           //3G����

//������"ͨ�����ӻ�"����
#define COMM_ROLE_MASTER    0           //ͨ������
#define COMM_ROLE_SLAVE     1           //ͨ�Ŵӻ�

//������"��Ƶ���ӻ�"����
#define RF_MASTER    0
#define RF_SLAVE     1

//������"�ŵ�������"����
#define CH_SHIFT_NOTHING    0           //��������Ƶ�ŵ��Ŷ�û��
#define ONLY_CH             1           //ֻ�й����ŵ���
#define ONLY_SHIFT          2           //ֻ����Ƶ�ŵ���
#define CH_SHIFT            3           //��������Ƶ�ŵ��Ŷ���

//������"���˻�����"����
#define OPTICAL_NO          0           //�ǹ��˻�
#define OPTICAL_LOCAL       1           //���˽���
#define OPTICAL_REMOTE      2           //����Զ��

//������"��ʾ�����õĴ�������"����
#define UI_USART_0          0           //��ʾ���Ӵ���0
#define UI_USART_1          1           //��ʾ���Ӵ���1
#define UI_USART_2          2           //��ʾ���Ӵ���2

typedef struct
{
    UCHAR  ucSetFreqType;               //ѡƵ����
    UCHAR  ucGenType;                   //2G����3G
    UCHAR  ucCommRoleType;              //ͨ�����ӻ�
    UCHAR  ucRFRoleType;                //��Ƶ���ӻ�
    UCHAR  ucChannelType;               //�ŵ�������
    UCHAR  ucOpticalType;               //���˻�����
    UCHAR  ucIsCoupling;                //ֱ����ϻ���
    UCHAR  ucUIUSARTType;               //��ʾ�����õĴ�������
}DEV_TYPE_TABLE_ST;
//MCM-11_20061101_zhangjie_end

#endif
