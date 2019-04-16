/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

i2c.c

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "i2c.h"

#define I2C_SDA  PB22  //数据脚连接定义 PIOB口
#define I2C_SCK  PB23  //时钟脚连接定义 PIOB口
#define I2C_WP  PB24  //写保护引脚连接定义 PIOB口

#define SETSDA  PioWrite( &g_stPioB, I2C_SDA, PIO_SET_OUT)  //将I2C_SDA脚拉高
  //#define SETSDA  MA_OUTWM( PIO_SODR_B, I2C_SDA, I2C_SDA )
#define CLRSDA  PioWrite( &g_stPioB, I2C_SDA, PIO_CLEAR_OUT)  //将I2C_SDA脚清零
  //#define CLRSDA  MA_OUTWM( PIO_CODR_B, I2C_SDA, I2C_SDA )
#define READSDA  PioRead( &g_stPioB)  //读I2C_SDA的电平值
  //#define READSDA  MA_INWM( PIO_PDSR_B, I2C_SDA)

#define SetInputSDA  PioOpen( &g_stPioB, I2C_SDA, PIO_INPUT)  //设置I2C_SDA为输入
  //#define SetInputSDA  MA_OUTWM(PIO_ODR_B,I2C_SDA,I2C_SDA)
#define SetOutputSDA  PioOpen( &g_stPioB, I2C_SDA, PIO_OUTPUT)  //设置I2C_SDA为输出
  //#define SetOutputSDA  MA_OUTWM(PIO_OER_B,I2C_SDA,I2C_SDA)

#define SETSCL  PioWrite( &g_stPioB, I2C_SCK, PIO_SET_OUT)  //将I2C_SCK脚拉高
  //#define SETSCL  MA_OUTWM( PIO_SODR_B, I2C_SCK, I2C_SCK)
#define CLRSCL  PioWrite( &g_stPioB, I2C_SCK, PIO_CLEAR_OUT)  //将I2C_SCK脚清零
  //#define CLRSCL  MA_OUTWM( PIO_CODR_B, I2C_SCK, I2C_SCK)

#define SETWP  PioWrite( &g_stPioB, I2C_WP, PIO_SET_OUT)  //将I2C_WP脚拉高，写保护
  //#define SETWP  MA_OUTWM( PIO_SODR_B, I2C_WP, I2C_WP)
#define CLRWP  PioWrite( &g_stPioB, I2C_WP, PIO_CLEAR_OUT)  //将I2C_WP脚清零，取消写保护
  //#define CLRWP  MA_OUTWM( PIO_CODR_B, I2C_WP, I2C_WP)

#define C_TIMER_NOP  30
#define SLAVE  0xa0 //由硬件电路决定的

void Delay5ms (void);
void Delay10us (void);
void i2c_send_start(void);
void i2c_send_stop(void);
void I2cInit(void);
void i2c_clock_w(void);
unsigned int i2c_clock_r(void);
UCHAR i2c_get_bit(UCHAR data, UCHAR I2cIndex);
void i2c_send_data(UCHAR data);
UCHAR i2c_get_data(void);
unsigned int i2c_get_ack(void);
void i2c_no_ack(void);
void I2cWriteOne(void* address, UCHAR wdata);
void I2cWriteFast(void* address, void *arm_address, UCHAR count);
void I2cWriteSlow(void* address, void *arm_address, UCHAR count);
UCHAR I2cReadOne(void* address);
UCHAR I2cRead(void);
void I2cReadMore(void* address, void* arm_address, UCHAR count);


//*********************************************
//                延时程序
//*********************************************
void Delay5ms (void) 
{
    UINT32 i;
    for(i=0;i<7000;i++);
}

void Delay10us (void) 
{
    UINT32 i;
    for(i=0;i<5;i++);
}
//*********************************************
//  送开始信号，即时钟脚为高电平时，数据脚送一下降沿
//*********************************************
void i2c_send_start(void)
{
    SETSDA;
    SETSCL;
    Delay10us();
    CLRSDA;
    Delay10us();
    CLRSCL;
}
//*********************************************
//  送结束信号，即时钟脚为高电平时，数据脚送一上升沿
//*********************************************
void i2c_send_stop(void)
{
    CLRSDA;
    SETSCL;
    Delay10us();
    SETSDA;
    Delay10us();
   // CLRSCL;
}
//*********************************************
//               I2C初始化工作
//*********************************************
void I2cInit(void)
{
    PioOpen( &g_stPioB, I2C_SDA|I2C_SCK|I2C_WP, PIO_OUTPUT); //设置DATA、CLK和WP为输出状态，并受PIO控制
    
//    //使DATA、CLK和WP脚受PIO控制
//    MA_OUTWM(PIO_PER_B,I2C_SDA|I2C_SCK|I2C_WP,I2C_SDA|I2C_SCK|I2C_WP);
//    //设置DATA、CLK和WP为输出状态
//    MA_OUTWM(PIO_OER_B,I2C_SDA|I2C_SCK|I2C_WP,I2C_SDA|I2C_SCK|I2C_WP);
    
    //确保外围I2C器件能正确接收以后的开始信号，初始化时先送个结束信号
    i2c_send_stop();
}
//*********************************************
//     生成CPU往外围芯片送数据时的CLOCK
//*********************************************
void i2c_clock_w(void)
{
    SETSCL;
    Delay10us();
    CLRSCL;
    Delay10us();
}
//*********************************************
//     生成CPU往外围芯片送数据时的CLOCK
//  返回值： 当前时钟下，CLK引脚上的电平值
//*********************************************
unsigned int i2c_clock_r(void)
{
    unsigned int sm;
    SETSCL;
    Delay10us();
    sm = READSDA;     //读I2C_SDA的电平值
    CLRSCL;
    Delay10us();
    return(sm  & I2C_SDA);
}
//*********************************************
//功能描述：返回数据 data 中的第 index 位的 0 或者 1 
//*********************************************
UCHAR i2c_get_bit(UCHAR data, UCHAR I2cIndex)	
{
    UCHAR data_out;
    data_out =data&(1<<I2cIndex);
    return data_out;
}
//*********************************************
//功能描述：CPU向外围I2C器件发送一个8bit的数据
//*********************************************
void i2c_send_data(UCHAR data)
{
    UCHAR i;
    for (i=0;i<8;i++)
    { 
        if(i2c_get_bit(data,7-i))
        {
            SETSDA;
        }
        else
        {
            CLRSDA;
        }
        i2c_clock_w();
    }
}
//*********************************************
//功能描述：CPU从外围I2C器件接收一个8bit的数据
//返回值：  收到的8bit的数据
//*********************************************
UCHAR i2c_get_data(void)
{
    UCHAR i; 
    unsigned int ss;
    UCHAR rdata;
	
    rdata=0;
    SetInputSDA;
    for (i=0;i<8;i++)
    {
        ss=i2c_clock_r();
        rdata=rdata<<1;
        if (ss) rdata=rdata+1;
    }
    SetOutputSDA;
    //SETSDA;
	
    return rdata;
}
//*********************************************
//     在整个访问过程中，接收外围I2C器件的应答
//*********************************************
unsigned int i2c_get_ack(void)
{
    unsigned int bb;
    //SETSDA;
    SetInputSDA;
    bb=i2c_clock_r();
    SetOutputSDA;
    //SETSDA;
    return (bb);
}

void i2c_no_ack(void)
{
    SETSDA;
    SETSCL;
    Delay10us();
    CLRSCL;
    CLRSDA;
}
//*********************************************
//功能描述：往外围器件写入一个字节数据的完整函数
//输入：   SLAVE     -> 片选
//        address   -> 地址（有效地址为低16bit）
//        wdata     -> 欲写入的单字节数据
//返回值： 无
//*********************************************
void I2cWriteOne(void* address, UCHAR wdata)
{
    unsigned int ack;
    
    CLRWP; //取消写保护
    i2c_send_start();
    i2c_send_data((UCHAR)SLAVE);
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((int)address / 256) ); // 送高8位地址
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((int)address % 256) ); // 送低8位地址
    ack=i2c_get_ack();
    i2c_send_data(wdata);
    ack=i2c_get_ack();
    i2c_send_stop();
  //!!!!!!!!!!!!! 此处加入延时4MS !!!!!!!!!!!!!!!  
    Delay5ms ();
    
    SETWP; //重新写保护
}
//*********************************************
//功能描述：往外围器件“快速”写入若干字节数据，长度≤64
//输入:    arm_address -> 数据在内存中的地址
//         address    -> 地址（有效地址为低16bit）
//         count      -> 欲写入的数据长度
//返回值：  无
//*********************************************
void I2cWriteFast(void* address, void *arm_address, UCHAR count)
{
    unsigned int ack;
    
    CLRWP; //取消写保护
    
    i2c_send_start();
    i2c_send_data((UCHAR)SLAVE);
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((int)address / 256) ); // 送高8位地址
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((int)address % 256) ); // 送低8位地址
    ack=i2c_get_ack();
    while(count--)
    {
        i2c_send_data( *(UCHAR*)arm_address );
        ack=i2c_get_ack();
        ((UCHAR*)arm_address)++;
    }
    i2c_send_stop();
  //!!!!!!!!!!!!! 此处加入延时5MS !!!!!!!!!!!!!!!  
    Delay5ms ();
    SETWP; //重新写保护
}
//*********************************************
//功能描述：往外围器件“慢速”写入若干字节数据，长度不限
//输入：   arm_address -> 数据在内存中的地址
//        address    -> 地址（有效地址为低16bit）
//        count      -> 欲写入的数据长度
//返回值： 无
//*********************************************
void I2cWriteSlow(void* address, void *arm_address, UCHAR count)
{
    unsigned int ack;
    
    CLRWP; //取消写保护
    
    while(count--)
    {
        i2c_send_start();
        i2c_send_data((UCHAR)SLAVE);
        ack=i2c_get_ack();
        i2c_send_data( (UCHAR)((int)address / 256) ); // 送高8位地址
        ack=i2c_get_ack();
        i2c_send_data( (UCHAR)((int)address % 256) ); // 送低8位地址
        ack=i2c_get_ack();
        i2c_send_data( *(UCHAR*)arm_address );
        ack=i2c_get_ack();
        i2c_send_stop();
        ((UCHAR*)arm_address)++;
        ((UCHAR*)address)++;
   //!!!!!!!!!!!!! 此处加入延时4MS !!!!!!!!!!!!!!!  
        Delay5ms ();
    }
    SETWP; //重新写保护
}
//*********************************************
//功能描述：从外围器件读入一个字节数据的完整函数
//输入：   SLAVE     -> 片选
//        address   -> 地址（有效地址为低16bit）
//返回值： data      -> 读入的单字节数据
//*********************************************
UCHAR I2cReadOne(void* address)
{
    UCHAR data;
    unsigned int ack;
    i2c_send_start();
    i2c_send_data( (UCHAR)SLAVE );
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((unsigned int)address / 256) ); // 送高8位地址
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((unsigned int)address % 256) ); // 送低8位地址
    ack=i2c_get_ack();
    i2c_send_start();
    i2c_send_data( (UCHAR)SLAVE + 1);
    ack=i2c_get_ack();
    data=i2c_get_data(); //将读出的值输出
    i2c_no_ack(); 
    i2c_send_stop();
    return (data);
}
//*********************************************
//功能描述：从外围器件当前位置读入一个字节数据
//         读完后，器件的当前地址自动+1
//输入：   无
//返回值： data      -> 读入的单字节数据
//*********************************************
UCHAR I2cRead(void)
{
    UCHAR data;
    unsigned int ack;
    i2c_send_start();
    i2c_send_data((UCHAR)SLAVE+1);
    ack=i2c_get_ack();
    data=i2c_get_data(); //将读出的值输出给data
    i2c_no_ack(); 
    i2c_send_stop();
    return (data);
}
//*********************************************
//功能描述：从外围器件"连续"读入若干字节数据
//输入：   arm_address -> 数据在内存中的地址
//        address    -> 地址（有效地址为低16bit）
//        count      -> 欲写入的数据长度
//返回值： 无
//*********************************************
void I2cReadMore(void* address, void* arm_address, UCHAR count)
{
    UCHAR data;
    unsigned int ack;
    i2c_send_start();
    i2c_send_data( (UCHAR)SLAVE );
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((int)address / 256) ); // 送高8位地址
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((int)address % 256) ); // 送低8位地址
    ack=i2c_get_ack();
    i2c_send_start();
    i2c_send_data((UCHAR)SLAVE + 1);
    ack=i2c_get_ack();
    data=i2c_get_data();
    *(UCHAR*)arm_address = data; //将读出的值输出
    i2c_no_ack(); 
    i2c_send_stop();
    ((UCHAR*)arm_address)++;
    //((UCHAR*)address)++;
    count--;
    while(count--)
    {
        *(UCHAR*)arm_address = I2cRead(); //将读出的值输出
        ((UCHAR*)arm_address)++;
    }

}
