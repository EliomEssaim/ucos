/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

I2C.h

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef I2C_H_MY
#define I2C_H_MY

#include "../../include/UserType.h"
#include "../../include/at91m55800.h"
#include "../../include/MyBoard.h"
#include "../../periph/pio/pio.h"

extern void I2cInit(void);
  //I2C��ʼ������
extern void I2cWriteOne(void* address, unsigned char wdata);
  //��������������Χ����д��һ���ֽ����ݵ���������
extern void I2cWriteFast(void* address, void *arm_address, unsigned char count);
  //��������������Χ���������١�д�������ֽ����ݣ����ȡ�64
extern void I2cWriteSlow(void* address, void *arm_address, unsigned char count);
  //��������������Χ���������١�д�������ֽ����ݣ����Ȳ���
extern unsigned char I2cReadOne(void* address);
  //��������������Χ��������һ���ֽ����ݵ���������
extern unsigned char I2cRead(void);
  //��������������Χ������ǰλ�ö���һ���ֽ�����
extern void I2cReadMore(void* address, void* arm_address, unsigned char count);
  //��������������Χ����"����"���������ֽ�����
  
#endif /* I2C_H_MY */