//����uC/OS���ڴ��ͷ�ʱ����ŵ�ǡ�����ڴ���ƿ飬�����ɾ��ʱ�û�����֪����
//�ڴ���Ǵ��������ģ��������������ڼǴ����ϵͳ���������Ӧ���н���ֱ��
//ʹ��uC/OS�ṩ��OSMemGet()��OSMemPut()�����ǲ��÷�װ��MyMalloc()��MyFree()��
//���û������ڴ��ʱ����ǰ����ĸ��ֽڽ����������ڴ���ƿ�ĵ�ַ���������ͷ�
//ʱʹ�ã��û�ʹ�õ��ڴ�ӵ�5�ֽڿ�ʼ������û�������ڴ������Ҫ��5�ֽ�

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

    *(void **)pv = pmem; //��ͷ4���ֽڴ���ڴ���ƿ��ַ

#if MY_CHECK_MEM_EN > 0
    MyCheckMem(pmem);
#endif

    return ((unsigned char *)pv + sizeof(void *)); //�û�ʵ��ʹ�õ��ڴ�ӵ�5�ֽڿ�ʼ
}

INT8U MyFree(void *pv)
{
    //���Ӷ�ɾ����ָ����ж�
    if(pv == 0)
    {
        return 0;
    }
    
    pv = (unsigned char *)pv - sizeof(void *); //��ָ����ǰ����4���ֽڣ����ڴ���׵�ַ
    OS_MEM *pmem = (OS_MEM *)*(void **)pv; //��ȡ��Ӧ���ڴ���ƿ��ַ

#if MY_CHECK_MEM_EN > 0
    MyCheckMem(pmem);
#endif
    
    return OSMemPut(pmem, pv);
}