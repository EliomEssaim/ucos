#ifndef ChH
#define ChH

//通信对象类型
#define COMM_OBJ_TYPE_USART 0//串口
#define COMM_OBJ_TYPE_NET   1//网口

//通信接口类型
#define USART_MODEM_OR_DIRECT   0//连接modem或者直连
#define USART_SLAVE_485         1//连接主机(通过485)
#define USART_SLAVE_OPT         2//连接主机(通过光端机)

//通信对象状态
#define COMM_STATE_SEND_BUSY 1//发送忙，上一个信息未发送完毕
#define COMM_STATE_SEND_IDLE 0//发送闲，上一个信息已发送完毕

typedef struct COMM_OBJ_T
{
    UCHAR ucType;           //通信对象类型:串口OR网口
    UCHAR ucInterfaceType;  //通信接口类型:主从，modem或者直连，光端机
    UCHAR ucState;          //通信对象状态      
    void *pvCOMMDev;        //指向通信接口的指针
}COMM_OBJ_ST;

//三种接口类型，他们不可以指向同一个通信设备
extern COMM_OBJ_ST g_stNorthInterface;
extern COMM_OBJ_ST g_stSouthInterface;
extern COMM_OBJ_ST g_stDebugInterface;

extern void CHTask(void *);
extern void CHTaskInit();

#endif
