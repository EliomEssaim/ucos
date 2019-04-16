/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   AppMain.cpp
    ����:     ����
    �ļ�˵��: ���ļ�����ʵ�ֶ�ʱ�����ƵĴ���
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  
       ����:  ����
       ����:  �������
    2. ����:  2006/02/16
       ����:  �½�
       ����:  �޸�����MCM-13��
              �����ڴ�ʹ�ã������ڴ��ķ���ռ䡣
    3. ����:  2006/11/07
       ����:  ����
       ����:  �޸�����MCM-20��
              ���Ӱ��������ϱ�����
---------------------------------------------------------------------------*/
#ifndef AppMainH
#define AppMainH

#include "includes.h"
#include "MyBoard.h"
#include "Timer/Timer.h"
#include "../periph/usart/Usart.h"

/*OSÿTICK�ĺ�����*/
#define MS_PER_TICK   10

/*����OSTICK��Ӳ����ʱ����ʼֵ*/
/*��ʱ����8��Ƶ���Ͷ�ʱ����ʼ���й�*/
#define TC_TICK_INIT_VALUE   ((MCKKHz/8)*MS_PER_TICK)

//*****************************�궨��*****************************//

//ģ��ID
#define MID_OH          0
#define MID_CH          1
#define MID_TC          2
#define MID_AC          3
#define MID_LOG         4
#define MID_LAMP        5
#define MID_KEY         6
#define MID_UI          7

//����ID����
#define OH_TASK_ID      MID_OH
#define CH_TASK_ID      MID_CH
#define TC_TASK_ID      MID_TC
#define AC_TASK_ID      MID_AC
#define LOG_TASK_ID     MID_LOG
#define LAMP_TASK_ID    MID_LAMP
#define KEY_TASK_ID     MID_KEY
#define UI_TASK_ID      MID_UI
#define MAX_TASK_ID     (UI_TASK_ID + 1) //��������ID��1

//���г���
#define OH_QUE_SIZE     30//test
#define AC_QUE_SIZE     10
#define CH_NTH_QUE_SIZE 30//test
#define CH_STH_QUE_SIZE 10
#define CH_DEB_QUE_SIZE 10
#define LOG_QUE_SIZE    10
#define UI_QUE_SIZE     10

//�������ȼ�
#define TC_PRIO   4
#define CH_PRIO   6
#define KEY_PRIO  7
#define OH_PRIO   8
#define LAMP_PRIO 9
#define UI_PRIO   10
#define AC_PRIO   11
#define RC_PRIO   12
#define LOG_PRIO  14

//�ڴ���������ڴ��ʵ�ʵ���Ϣ���ݣ������4�ֽ������ڴ����Ŀ��������MemMgmt.cpp
#define MEM_BLK_SIZE_16        20 
#define MEM_BLK_SIZE_16_COUNT  10
#define MEM_BLK_SIZE_256       260
//MCM-13_20061106_zhangjie_begin
#define MEM_BLK_SIZE_256_COUNT 40//100
#define MEM_BLK_SIZE_512       USART_SIZE_BUFFER//notice
#define MEM_BLK_SIZE_512_COUNT 6
//MCM-13_20061106_zhangjie_end

//**************************ȫ�ֱ�������**************************//
//����Ķ���
extern OS_EVENT *g_pstOHQue;
extern OS_EVENT *g_pstACQue;
extern OS_EVENT *g_pstCHNthQue;
extern OS_EVENT *g_pstCHSthQue;
extern OS_EVENT *g_pstCHDebQue;
extern OS_EVENT *g_pstLogQue;
extern OS_EVENT *g_pstUIQue;
extern OS_EVENT *g_pstUICHQue;

extern void *g_apvOHQue[];
extern void *g_apvACQue[];
extern void *g_apvCHNthQue[];//����ӿ���Ϣ����
extern void *g_apvCHSthQue[];//����ӿ���Ϣ����
extern void *g_apvCHDebQue[];//���Խӿ���Ϣ����
extern void *g_apvLogQue[];  //��־��Ϣ����
extern void *g_apvUIQue[];
extern void *g_apvUICHQue[];

//��ʱ��������
#define TIMER_UNIT_MS 100//��ʱ����λʱ�䣬��λ:MS
extern TimerManager g_TimerMngr;

//uC/OS��API��ʹ�õı���
extern UCHAR g_ucErr;

//�ڴ����
extern OS_MEM *g_pstMemPool16;
extern OS_MEM *g_pstMemPool256;
extern OS_MEM *g_pstMemPool512;


#endif
