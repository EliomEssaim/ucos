/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

i2c.c

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "i2c.h"

#define I2C_SDA  PB22  //���ݽ����Ӷ��� PIOB��
#define I2C_SCK  PB23  //ʱ�ӽ����Ӷ��� PIOB��
#define I2C_WP  PB24  //д�����������Ӷ��� PIOB��

#define SETSDA  PioWrite( &g_stPioB, I2C_SDA, PIO_SET_OUT)  //��I2C_SDA������
  //#define SETSDA  MA_OUTWM( PIO_SODR_B, I2C_SDA, I2C_SDA )
#define CLRSDA  PioWrite( &g_stPioB, I2C_SDA, PIO_CLEAR_OUT)  //��I2C_SDA������
  //#define CLRSDA  MA_OUTWM( PIO_CODR_B, I2C_SDA, I2C_SDA )
#define READSDA  PioRead( &g_stPioB)  //��I2C_SDA�ĵ�ƽֵ
  //#define READSDA  MA_INWM( PIO_PDSR_B, I2C_SDA)

#define SetInputSDA  PioOpen( &g_stPioB, I2C_SDA, PIO_INPUT)  //����I2C_SDAΪ����
  //#define SetInputSDA  MA_OUTWM(PIO_ODR_B,I2C_SDA,I2C_SDA)
#define SetOutputSDA  PioOpen( &g_stPioB, I2C_SDA, PIO_OUTPUT)  //����I2C_SDAΪ���
  //#define SetOutputSDA  MA_OUTWM(PIO_OER_B,I2C_SDA,I2C_SDA)

#define SETSCL  PioWrite( &g_stPioB, I2C_SCK, PIO_SET_OUT)  //��I2C_SCK������
  //#define SETSCL  MA_OUTWM( PIO_SODR_B, I2C_SCK, I2C_SCK)
#define CLRSCL  PioWrite( &g_stPioB, I2C_SCK, PIO_CLEAR_OUT)  //��I2C_SCK������
  //#define CLRSCL  MA_OUTWM( PIO_CODR_B, I2C_SCK, I2C_SCK)

#define SETWP  PioWrite( &g_stPioB, I2C_WP, PIO_SET_OUT)  //��I2C_WP�����ߣ�д����
  //#define SETWP  MA_OUTWM( PIO_SODR_B, I2C_WP, I2C_WP)
#define CLRWP  PioWrite( &g_stPioB, I2C_WP, PIO_CLEAR_OUT)  //��I2C_WP�����㣬ȡ��д����
  //#define CLRWP  MA_OUTWM( PIO_CODR_B, I2C_WP, I2C_WP)

#define C_TIMER_NOP  30
#define SLAVE  0xa0 //��Ӳ����·������

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
//                ��ʱ����
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
//  �Ϳ�ʼ�źţ���ʱ�ӽ�Ϊ�ߵ�ƽʱ�����ݽ���һ�½���
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
//  �ͽ����źţ���ʱ�ӽ�Ϊ�ߵ�ƽʱ�����ݽ���һ������
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
//               I2C��ʼ������
//*********************************************
void I2cInit(void)
{
    PioOpen( &g_stPioB, I2C_SDA|I2C_SCK|I2C_WP, PIO_OUTPUT); //����DATA��CLK��WPΪ���״̬������PIO����
    
//    //ʹDATA��CLK��WP����PIO����
//    MA_OUTWM(PIO_PER_B,I2C_SDA|I2C_SCK|I2C_WP,I2C_SDA|I2C_SCK|I2C_WP);
//    //����DATA��CLK��WPΪ���״̬
//    MA_OUTWM(PIO_OER_B,I2C_SDA|I2C_SCK|I2C_WP,I2C_SDA|I2C_SCK|I2C_WP);
    
    //ȷ����ΧI2C��������ȷ�����Ժ�Ŀ�ʼ�źţ���ʼ��ʱ���͸������ź�
    i2c_send_stop();
}
//*********************************************
//     ����CPU����ΧоƬ������ʱ��CLOCK
//*********************************************
void i2c_clock_w(void)
{
    SETSCL;
    Delay10us();
    CLRSCL;
    Delay10us();
}
//*********************************************
//     ����CPU����ΧоƬ������ʱ��CLOCK
//  ����ֵ�� ��ǰʱ���£�CLK�����ϵĵ�ƽֵ
//*********************************************
unsigned int i2c_clock_r(void)
{
    unsigned int sm;
    SETSCL;
    Delay10us();
    sm = READSDA;     //��I2C_SDA�ĵ�ƽֵ
    CLRSCL;
    Delay10us();
    return(sm  & I2C_SDA);
}
//*********************************************
//������������������ data �еĵ� index λ�� 0 ���� 1 
//*********************************************
UCHAR i2c_get_bit(UCHAR data, UCHAR I2cIndex)	
{
    UCHAR data_out;
    data_out =data&(1<<I2cIndex);
    return data_out;
}
//*********************************************
//����������CPU����ΧI2C��������һ��8bit������
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
//����������CPU����ΧI2C��������һ��8bit������
//����ֵ��  �յ���8bit������
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
//     ���������ʹ����У�������ΧI2C������Ӧ��
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
//��������������Χ����д��һ���ֽ����ݵ���������
//���룺   SLAVE     -> Ƭѡ
//        address   -> ��ַ����Ч��ַΪ��16bit��
//        wdata     -> ��д��ĵ��ֽ�����
//����ֵ�� ��
//*********************************************
void I2cWriteOne(void* address, UCHAR wdata)
{
    unsigned int ack;
    
    CLRWP; //ȡ��д����
    i2c_send_start();
    i2c_send_data((UCHAR)SLAVE);
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((int)address / 256) ); // �͸�8λ��ַ
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((int)address % 256) ); // �͵�8λ��ַ
    ack=i2c_get_ack();
    i2c_send_data(wdata);
    ack=i2c_get_ack();
    i2c_send_stop();
  //!!!!!!!!!!!!! �˴�������ʱ4MS !!!!!!!!!!!!!!!  
    Delay5ms ();
    
    SETWP; //����д����
}
//*********************************************
//��������������Χ���������١�д�������ֽ����ݣ����ȡ�64
//����:    arm_address -> �������ڴ��еĵ�ַ
//         address    -> ��ַ����Ч��ַΪ��16bit��
//         count      -> ��д������ݳ���
//����ֵ��  ��
//*********************************************
void I2cWriteFast(void* address, void *arm_address, UCHAR count)
{
    unsigned int ack;
    
    CLRWP; //ȡ��д����
    
    i2c_send_start();
    i2c_send_data((UCHAR)SLAVE);
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((int)address / 256) ); // �͸�8λ��ַ
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((int)address % 256) ); // �͵�8λ��ַ
    ack=i2c_get_ack();
    while(count--)
    {
        i2c_send_data( *(UCHAR*)arm_address );
        ack=i2c_get_ack();
        ((UCHAR*)arm_address)++;
    }
    i2c_send_stop();
  //!!!!!!!!!!!!! �˴�������ʱ5MS !!!!!!!!!!!!!!!  
    Delay5ms ();
    SETWP; //����д����
}
//*********************************************
//��������������Χ���������١�д�������ֽ����ݣ����Ȳ���
//���룺   arm_address -> �������ڴ��еĵ�ַ
//        address    -> ��ַ����Ч��ַΪ��16bit��
//        count      -> ��д������ݳ���
//����ֵ�� ��
//*********************************************
void I2cWriteSlow(void* address, void *arm_address, UCHAR count)
{
    unsigned int ack;
    
    CLRWP; //ȡ��д����
    
    while(count--)
    {
        i2c_send_start();
        i2c_send_data((UCHAR)SLAVE);
        ack=i2c_get_ack();
        i2c_send_data( (UCHAR)((int)address / 256) ); // �͸�8λ��ַ
        ack=i2c_get_ack();
        i2c_send_data( (UCHAR)((int)address % 256) ); // �͵�8λ��ַ
        ack=i2c_get_ack();
        i2c_send_data( *(UCHAR*)arm_address );
        ack=i2c_get_ack();
        i2c_send_stop();
        ((UCHAR*)arm_address)++;
        ((UCHAR*)address)++;
   //!!!!!!!!!!!!! �˴�������ʱ4MS !!!!!!!!!!!!!!!  
        Delay5ms ();
    }
    SETWP; //����д����
}
//*********************************************
//��������������Χ��������һ���ֽ����ݵ���������
//���룺   SLAVE     -> Ƭѡ
//        address   -> ��ַ����Ч��ַΪ��16bit��
//����ֵ�� data      -> ����ĵ��ֽ�����
//*********************************************
UCHAR I2cReadOne(void* address)
{
    UCHAR data;
    unsigned int ack;
    i2c_send_start();
    i2c_send_data( (UCHAR)SLAVE );
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((unsigned int)address / 256) ); // �͸�8λ��ַ
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((unsigned int)address % 256) ); // �͵�8λ��ַ
    ack=i2c_get_ack();
    i2c_send_start();
    i2c_send_data( (UCHAR)SLAVE + 1);
    ack=i2c_get_ack();
    data=i2c_get_data(); //��������ֵ���
    i2c_no_ack(); 
    i2c_send_stop();
    return (data);
}
//*********************************************
//��������������Χ������ǰλ�ö���һ���ֽ�����
//         ����������ĵ�ǰ��ַ�Զ�+1
//���룺   ��
//����ֵ�� data      -> ����ĵ��ֽ�����
//*********************************************
UCHAR I2cRead(void)
{
    UCHAR data;
    unsigned int ack;
    i2c_send_start();
    i2c_send_data((UCHAR)SLAVE+1);
    ack=i2c_get_ack();
    data=i2c_get_data(); //��������ֵ�����data
    i2c_no_ack(); 
    i2c_send_stop();
    return (data);
}
//*********************************************
//��������������Χ����"����"���������ֽ�����
//���룺   arm_address -> �������ڴ��еĵ�ַ
//        address    -> ��ַ����Ч��ַΪ��16bit��
//        count      -> ��д������ݳ���
//����ֵ�� ��
//*********************************************
void I2cReadMore(void* address, void* arm_address, UCHAR count)
{
    UCHAR data;
    unsigned int ack;
    i2c_send_start();
    i2c_send_data( (UCHAR)SLAVE );
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((int)address / 256) ); // �͸�8λ��ַ
    ack=i2c_get_ack();
    i2c_send_data( (UCHAR)((int)address % 256) ); // �͵�8λ��ַ
    ack=i2c_get_ack();
    i2c_send_start();
    i2c_send_data((UCHAR)SLAVE + 1);
    ack=i2c_get_ack();
    data=i2c_get_data();
    *(UCHAR*)arm_address = data; //��������ֵ���
    i2c_no_ack(); 
    i2c_send_stop();
    ((UCHAR*)arm_address)++;
    //((UCHAR*)address)++;
    count--;
    while(count--)
    {
        *(UCHAR*)arm_address = I2cRead(); //��������ֵ���
        ((UCHAR*)arm_address)++;
    }

}
