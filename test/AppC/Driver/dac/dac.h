/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

dac.h

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


#ifndef DAC_H_MY
#define DAC_H_MY

#include "At91M55800.h"
#include "UserType.h"

extern void DacInit(void);
extern void Dac0Exe(USHORT usDataOut);
extern void Dac1Exe(USHORT usDataOut);

#endif /* AD_H */