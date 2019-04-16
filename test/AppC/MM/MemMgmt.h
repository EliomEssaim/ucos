#ifndef MemMgmtH
#define MemMgmtH

#include "ucos_ii.h"

void *MyMalloc(OS_MEM *pmem);
INT8U MyFree(void *pv);

#endif