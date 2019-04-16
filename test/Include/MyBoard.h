/*---------------------------------------------------------------------------
    ��Ȩ 2006 - , �����ʿ�ͨ�ż������޹�˾
    �ļ���:   MyBoard.h
    ����:     �½�
    �ļ�˵��: 
    ����:
    �����б�:

    ��ʷ��¼:
    1. ����:  2006/09/28
       ����:  �½�
       ����:  �������
    2. ����:  2006/11/29
       ����:  �½�
       ����:  �޸�����MCM-41��
              �ѵ�Ӳ���ܽŵĺ궨���Ƶ����ļ�
---------------------------------------------------------------------------*/
#ifndef MyBoardH
#define MyBoardH

#include "UserType.h"
#include    "At91M55800.h"      /* library definition */


/*2006��ذ�*/
//#define OUR_BOARD
/*����������*/
//#define FT_BOARD
/*Ӣ���ؿ�����*/
//#define EMBEST_BOARD

#ifdef OUR_BOARD
/*---------------------------------------*/
/* AT91EB55 External Memories Definition */
/*---------------------------------------*/

/* Flash Memory : AT49BV1604 1M*16 */
#define FLASH_BASE      ((UINT32 *)0x01000000)
#define FLASH_SIZE      (2*1024*1024)                /* byte */

/* SRAM : size is depending on the devices fitted on the board */
#define EXT_SRAM_BASE           ((UINT32 *)0x02000000)
#define EXT_SRAM_DEFAULT_SIZE   (512*1024)
#define EXT_SRAM_SIZE   (512*1024)
#define EXT_SRAM_LIMIT  (EXT_SRAM_BASE+EXT_SRAM_SIZE)

/*---------------------------------------------------------------------------*/
/* EBI Initialization Data                                                   */
/*---------------------------------------------------------------------------*/
/* The EBI User Interface Image which is copied by the boot.                 */
/* 32 MHz master clock assumed.                                           */
/* That's hardware! Details in the Electrical Datasheet of the AT91 device.  */
/* EBI Base Address is added at the end for commodity in copy code.          */
/*---------------------------------------------------------------------------*/
#define EBI_CSR_0       ((UINT32 *)(FLASH_BASE | 0x3529)     /* 0x01000000, 16MB, 2 tdf, 16 bits, 3 WS  */
#define EBI_CSR_1       ((UINT32 *)(EXT_SRAM_BASE | 0x3121)  /* 0x02000000, 16MB, 0 hold, 16 bits, 1 WS */
#define EBI_CSR_2       ((UINT32 *)0x20000000)               /* unused */
#define EBI_CSR_3       ((UINT32 *)0x30000000)               /* unused */
#define EBI_CSR_4       ((UINT32 *)0x40000000)               /* unused */
#define EBI_CSR_5       ((UINT32 *)0x50000000)               /* unused */
#define EBI_CSR_6       ((UINT32 *)0x60000000)               /* unused */
#define EBI_CSR_7       ((UINT32 *)0x70000000)               /* unused */

/*-----------------*/
/* Leds Definition */
/*-----------------*/

#define LED1  PB21

#define LED_PIO_CTRL    g_stPioB
#define LED_MASK        (LED1)

#define LED_ON          PIO_CLEAR_OUT
#define LED_OFF         PIO_SET_OUT

/*-------------------------*/
/* ��˻�ʹ�ܽŶ��� */
/*-------------------------*/
#define OPT_ENABLE  PA20

#define OPT_PIO_CTRL    g_stPioA

#define OPT_ENABLE_ON          PIO_SET_OUT
#define OPT_ENABLE_OFF         PIO_CLEAR_OUT

/*-------------------------*/
/* 485����ʹ�ܽŶ��� */
/*-------------------------*/
#define U485_USART1_ENABLE  PA17
#define U485_USART2_ENABLE  PA14

#define U485_PIO_CTRL    g_stPioA
#define U485_MASK        (U485_USART1_ENABLE|U485_USART2_ENABLE)

#define U485_ENABLE_ON          PIO_SET_OUT
#define U485_ENABLE_OFF         PIO_CLEAR_OUT

/*-------------------------*/
/* modem�����Ŷ��� */
/*-------------------------*/
#define MODEM_RESET_PIN  PB19

#define MODEMRESET_PIO_CTRL    g_stPioB

//MCM-41_20061129_zhangjie_begin
//********************************************
//               LAMP�ܽ��������           **
//********************************************
#define POWER_LAMP      PA29  //������Դ�澯ָʾ��
#define DL_IN_PWR_LAMP  PA28  //���빦�ʸ澯ָʾ��
#define UL_OUT_PWR_LAMP PA27  //����������ʸ澯ָʾ��
#define DL_OUT_PWR_LAMP PA26  //����������ʸ澯ָʾ��
#define UL_SWR_LAMP     PA25  //����פ���ȸ澯ָʾ��
#define DL_SWR_LAMP     PA24  //����פ���ȸ澯ָʾ��
#define REPORT_LAMP     PA23  //�ϱ�״ָ̬ʾ��

#define ALL_SIGNAL_LAMP PA29|PA28|PA27|PA26|PA25|PA24|PA23

#define PIO_STATE &g_stPioA  //�����ڷ�������ʱ��PIOB����
//MCM-41_20061129_zhangjie_end

/*-------------------------*/
/* Push Buttons Definition */
/*-------------------------*/


/*--------------------------*/
/* Serial EEPROM Definition */
/*--------------------------*/



/*--------------*/
/* Battery      */
/*--------------*/



/*-----------------*/
/*  A/D Converter  */
/*-----------------*/


/*--------------*/
/* Master Clock */
/*--------------*/

#define MCK             32000000
#define MCKKHz          (MCK/1000)

#define EXT_QUARTZ      16000000


/*������RAM�����д�*/
#define P_RUN_ADDR (0x2000000)

#endif/*#ifdef OUR_BOARD*/



#ifdef FT_BOARD
/*---------------------------------------*/
/* AT91EB55 External Memories Definition */
/*---------------------------------------*/

/* Flash Memory : AT49BV1604 1M*16 */
#define FLASH_BASE      ((UINT32 *)0x01000000)
#define FLASH_SIZE      (2*1024*1024)                /* byte */

/* SRAM : size is depending on the devices fitted on the board */
#define EXT_SRAM_BASE           ((UINT32 *)0x02000000)
#define EXT_SRAM_DEFAULT_SIZE   (512*1024)
#define EXT_SRAM_SIZE   (512*1024)
#define EXT_SRAM_LIMIT  (EXT_SRAM_BASE+EXT_SRAM_SIZE)

/*---------------------------------------------------------------------------*/
/* EBI Initialization Data                                                   */
/*---------------------------------------------------------------------------*/
/* The EBI User Interface Image which is copied by the boot.                 */
/* 32 MHz master clock assumed.                                           */
/* That's hardware! Details in the Electrical Datasheet of the AT91 device.  */
/* EBI Base Address is added at the end for commodity in copy code.          */
/*---------------------------------------------------------------------------*/
#define EBI_CSR_0       ((UINT32 *)(FLASH_BASE | 0x3529)     /* 0x01000000, 16MB, 2 tdf, 16 bits, 3 WS  */
#define EBI_CSR_1       ((UINT32 *)(EXT_SRAM_BASE | 0x3121)  /* 0x02000000, 16MB, 0 hold, 16 bits, 1 WS */
#define EBI_CSR_2       ((UINT32 *)0x20000000)               /* unused */
#define EBI_CSR_3       ((UINT32 *)0x30000000)               /* unused */
#define EBI_CSR_4       ((UINT32 *)0x40000000)               /* unused */
#define EBI_CSR_5       ((UINT32 *)0x50000000)               /* unused */
#define EBI_CSR_6       ((UINT32 *)0x60000000)               /* unused */
#define EBI_CSR_7       ((UINT32 *)0x70000000)               /* unused */

/*-----------------*/
/* Leds Definition */
/*-----------------*/
#define LED1            PA4
#define LED2            PA4
#define LED3            PA5
#define LED4            PA6
#define LED5            PA7


#define LED_PIO_CTRL    g_stPioA
#define LED_MASK        (LED2|LED3|LED4|LED5)

#define LED_ON          PIO_CLEAR_OUT
#define LED_OFF         PIO_SET_OUT

/*-------------------------*/
/* Push Buttons Definition */
/*-------------------------*/
#define SW1_MASK        PB20
#define SW2_MASK        PA9
#define SW3_MASK        PB17
#define SW4_MASK        PB19
#define SW_MASK         (SW1_MASK|SW2_MASK|SW3_MASK)

#define PIO_SW1         20
#define PIO_SW2         9
#define PIO_SW3         17
#define PIO_SW4         19

#define SW1_PIO_CTRL    PIOB_CTRL
#define SW2_PIO_CTRL    PIOA_CTRL
#define SW3_PIO_CTRL    PIOB_CTRL
#define SW4_PIO_CTRL    PIOB_CTRL

/*--------------------------*/
/* Serial EEPROM Definition */
/*--------------------------*/

#define SCL             PA1
#define SDA             PA2
#define PIO_SCL         1
#define PIO_SDA         2

/*--------------*/
/* Battery      */
/*--------------*/

#define FASTCHG_CTRL    PB18
#define FASTCHG_ON      0x0
#define FASTCHG_OFF     0x1

/*-----------------*/
/*  A/D Converter  */
/*-----------------*/
#define AD_NCONVST      PB19
#define START_CONV      0x0

#define LOOP_DAC1_AD0   //* with CB6 Closed
#define LOOP_DAC0_AD4   //* with CB5 Closed

/*--------------*/
/* Master Clock */
/*--------------*/

#define MCK             18432000
#define MCKKHz          (MCK/1000)

#define EXT_QUARTZ      18432000

/*������RAM�����д�*/
#define P_RUN_ADDR (0x2000000)

#endif/*#ifdef FT_BOARD*/







/*Ӣ���ؿ�����*/
#ifdef EMBEST_BOARD
/*---------------------------------------*/
/* AT91EB55 External Memories Definition */
/*---------------------------------------*/

/* Flash Memory : AT49BV1604 1M*16 */
#define FLASH_BASE      ((UINT32 *)0x01000000)
#define FLASH_SIZE      (2*1024*1024)                /* byte */

/* SRAM : size is depending on the devices fitted on the board */
#define EXT_SRAM_BASE           ((UINT32 *)0x02000000)
#define EXT_SRAM_DEFAULT_SIZE   (256*1024)
#define EXT_SRAM_SIZE   (256*1024)
#define EXT_SRAM_LIMIT  (EXT_SRAM_BASE+EXT_SRAM_SIZE)

/*---------------------------------------------------------------------------*/
/* EBI Initialization Data                                                   */
/*---------------------------------------------------------------------------*/
/* The EBI User Interface Image which is copied by the boot.                 */
/* 32 MHz master clock assumed.                                           */
/* That's hardware! Details in the Electrical Datasheet of the AT91 device.  */
/* EBI Base Address is added at the end for commodity in copy code.          */
/*---------------------------------------------------------------------------*/
#define EBI_CSR_0       ((UINT32 *)(FLASH_BASE | 0x2529)     /* 0x01000000, 16MB, 2 tdf, 16 bits, 3 WS  */
#define EBI_CSR_1       ((UINT32 *)(EXT_SRAM_BASE | 0x2121)  /* 0x02000000, 16MB, 0 hold, 16 bits, 1 WS */
#define EBI_CSR_2       ((UINT32 *)0x20000000)               /* unused */
#define EBI_CSR_3       ((UINT32 *)0x30000000)               /* unused */
#define EBI_CSR_4       ((UINT32 *)0x40000000)               /* unused */
#define EBI_CSR_5       ((UINT32 *)0x50000000)               /* unused */
#define EBI_CSR_6       ((UINT32 *)0x60000000)               /* unused */
#define EBI_CSR_7       ((UINT32 *)0x70000000)               /* unused */

/*-----------------*/
/* Leds Definition */
/*-----------------*/
#define LED1            PB8
#define LED2            PB9
#define LED3            PB10
#define LED4            PB11
#define LED5            PB12
#define LED6            PB13
#define LED7            PB14
#define LED8            PB15

#define LED_PIO_CTRL    g_stPioB
#define LED_MASK        (LED1|LED2|LED3|LED4|LED5|LED6|LED7|LED8)

#define LED_ON          PIO_CLEAR_OUT
#define LED_OFF         PIO_SET_OUT

/*-------------------------*/
/* Push Buttons Definition */
/*-------------------------*/
#define SW1_MASK        PB20
#define SW2_MASK        PA9
#define SW3_MASK        PB17
#define SW4_MASK        PB19
#define SW_MASK         (SW1_MASK|SW2_MASK|SW3_MASK)

#define PIO_SW1         20
#define PIO_SW2         9
#define PIO_SW3         17
#define PIO_SW4         19

#define SW1_PIO_CTRL    g_stPioB
#define SW2_PIO_CTRL    g_stPioA
#define SW3_PIO_CTRL    g_stPioB
#define SW4_PIO_CTRL    g_stPioB

/*--------------------------*/
/* Serial EEPROM Definition */
/*--------------------------*/

#define SCL             PA1
#define SDA             PA2
#define PIO_SCL         1
#define PIO_SDA         2

/*--------------*/
/* Battery      */
/*--------------*/

#define FASTCHG_CTRL    PB18
#define FASTCHG_ON      0x0
#define FASTCHG_OFF     0x1

/*-----------------*/
/*  A/D Converter  */
/*-----------------*/
#define AD_NCONVST      PB19
#define START_CONV      0x0

#define LOOP_DAC1_AD0   //* with CB6 Closed
#define LOOP_DAC0_AD4   //* with CB5 Closed

/*--------------*/
/* Master Clock */
/*--------------*/

#define MCK             32768000
#define MCKKHz          (MCK/1000)

#define EXT_QUARTZ      16000000

/*������RAM�����д�*/
#define P_RUN_ADDR (0x2000000)


#endif/*#ifdef EMBEST_BOARD*/

#endif /* MyBoardH */
