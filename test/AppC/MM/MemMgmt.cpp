//由于uC/OS的内存释放时必须放到恰当的内存控制块，因此在删除时用户必须知道该
//内存块是从哪里分配的，这样很容易由于记错造成系统崩溃。因此应用中将不直接
//使用uC/OS提供的OSMemGet()和OSMemPut()，而是采用封装的MyMalloc()和MyFree()。
//在用户申请内存块时，最前面的四个字节将用来保存内存控制块的地址，用于在释放
//时使用，用户使用的内存从第5字节开始。因此用户申请的内存块至少要有5字节

#include "../include/MyCfg.h"
#include "MemMgmt.h"

extern INT8U g_ucErr;

#if MY_CHECK_MEM_EN > 0
void MyCheckMem(OS_MEM *pmem);
#endif

void *MyMalloc(OS_MEM *pmem)
{
    void *pv = OSMemGet(pmem, &g_ucErr);
    if(pv == 0)
    {
        return pv;
    }

    *(void **)pv = pmem; //在头4个字节存放内存控制块地址

#if MY_CHECK_MEM_EN > 0
    MyCheckMem(pmem);
#endif

    return ((unsigned char *)pv + sizeof(void *)); //用户实际使用的内存从第5字节开始
}

INT8U MyFree(void *pv)
{
    //增加对删除空指针的判断
    if(pv == 0)
    {
        return 0;
    }
    
    pv = (unsigned char *)pv - sizeof(void *); //将指针向前调整4个字节，到内存块首地址
    OS_MEM *pmem = (OS_MEM *)*(void **)pv; //获取对应的内存控制块地址

#if MY_CHECK_MEM_EN > 0
    MyCheckMem(pmem);
#endif
    
    return OSMemPut(pmem, pv);
}