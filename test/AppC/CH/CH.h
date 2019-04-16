#ifndef ChH
#define ChH

//ͨ�Ŷ�������
#define COMM_OBJ_TYPE_USART 0//����
#define COMM_OBJ_TYPE_NET   1//����

//ͨ�Žӿ�����
#define USART_MODEM_OR_DIRECT   0//����modem����ֱ��
#define USART_SLAVE_485         1//��������(ͨ��485)
#define USART_SLAVE_OPT         2//��������(ͨ����˻�)

//ͨ�Ŷ���״̬
#define COMM_STATE_SEND_BUSY 1//����æ����һ����Ϣδ�������
#define COMM_STATE_SEND_IDLE 0//�����У���һ����Ϣ�ѷ������

typedef struct COMM_OBJ_T
{
    UCHAR ucType;           //ͨ�Ŷ�������:����OR����
    UCHAR ucInterfaceType;  //ͨ�Žӿ�����:���ӣ�modem����ֱ������˻�
    UCHAR ucState;          //ͨ�Ŷ���״̬      
    void *pvCOMMDev;        //ָ��ͨ�Žӿڵ�ָ��
}COMM_OBJ_ST;

//���ֽӿ����ͣ����ǲ�����ָ��ͬһ��ͨ���豸
extern COMM_OBJ_ST g_stNorthInterface;
extern COMM_OBJ_ST g_stSouthInterface;
extern COMM_OBJ_ST g_stDebugInterface;

extern void CHTask(void *);
extern void CHTaskInit();

#endif
