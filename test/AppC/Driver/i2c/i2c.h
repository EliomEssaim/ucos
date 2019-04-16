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
  //I2C初始化工作
extern void I2cWriteOne(void* address, unsigned char wdata);
  //功能描述：往外围器件写入一个字节数据的完整函数
extern void I2cWriteFast(void* address, void *arm_address, unsigned char count);
  //功能描述：往外围器件“快速”写入若干字节数据，长度≤64
extern void I2cWriteSlow(void* address, void *arm_address, unsigned char count);
  //功能描述：往外围器件“慢速”写入若干字节数据，长度不限
extern unsigned char I2cReadOne(void* address);
  //功能描述：从外围器件读入一个字节数据的完整函数
extern unsigned char I2cRead(void);
  //功能描述：从外围器件当前位置读入一个字节数据
extern void I2cReadMore(void* address, void* arm_address, unsigned char count);
  //功能描述：从外围器件"连续"读入若干字节数据
  
#endif /* I2C_H_MY */