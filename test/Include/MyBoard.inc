/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   MyBoard.inc
    作者:     章杰
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  2006/09/28
       作者:  章杰
       描述:  编码完成
    2. 日期:  2006/11/07
       作者:  林玮
       描述:  修改问题MCM-20，
              增加按键触发上报功能，给读取按键分配地址
---------------------------------------------------------------------------*/
#------------------------------------------------------------------------------
#-         ATMEL Microcontroller Software Support  -  ROUSSET  -
#------------------------------------------------------------------------------
# The software is delivered "AS IS" without warranty or condition of any
# kind, either express, implied or statutory. This includes without
# limitation any warranty or condition with respect to merchantability or
# fitness for any particular purpose, or against the infringements of
# intellectual property rights of others.
#-----------------------------------------------------------------------------
#- File Name            : eb55.inc
#- Object               : AT91M55800 Evaluation Board Features Header File.
#-
#- 1.0 25/05/00   PF    : Creation
#- 1.1 31/10/00  JPP    : Additing EBI description
#-----------------------------------------------------------------------------

            .INCLUDE     "../include/At91M55800.inc"    
#开发板
#.EQU OUR_BOARD, 0            
#方技开发板
#.EQU FT_BOARD, 0
#英蓓特开发板
#.EQU EMBEST_BOARD, 1

.IFDEF OUR_BOARD
#----------------------------------------
#- AT91EB55 External Memories Definition
#----------------------------------------
#- Flash Memory : AT49BV1604 1Mx16
.EQU	FLASH_BASE,     	0x01000000

.EQU 	EXT_SRAM_BASE,      0x02000000
.EQU 	EXT_SRAM_SIZE,      0x00080000      		/* 512Kbytes */
.EQU 	EXT_SRAM_LIMIT,     (EXT_SRAM_BASE+EXT_SRAM_SIZE)

#------------------------------------------------------------------------------
#- EBI Initialization Data
#-------------------------
#- The EBI User Interface Image which is copied by the boot.
#- 25 MHz master clock assumed.
#- That's hardware! Details in the Electrical Datasheet of the AT91 device.
#- EBI Base Address is added at the end for commodity in copy code.
#------------------------------------------------------------------------------
.EQU   EBI_CSR_0,  		  (FLASH_BASE | 0x3529)    /* 0x01000000, 16MB, 2 tdf, 16 bits, 2 WS */
.EQU   EBI_CSR_1,  		  (EXT_SRAM_BASE | 0x3121) /* 0x02000000, 16MB, 0 hold, 16 bits, 1 WS*/
.EQU   EBI_CSR_2,  		  0x40002FBE  /* unused *//*NAND FLASH*/
.EQU   EBI_CSR_3,  		  0x30000000  /* unused */
#MCM-20_20061107_Linwei_begin
.EQU   EBI_CSR_4,  		  (0x04000000 | 0x3e3e)  /* 0x3e3d unused */
#MCM-20_20061107_Linwei_end
.EQU   EBI_CSR_5,  		  0x50000000  /* unused */
.EQU   EBI_CSR_6,  		  0x60000000  /* unused */
.EQU   EBI_CSR_7,  		  0x70000000  /* unused */

#------------------
#- Leds Definition
#------------------

.EQU 	LED1,       	  PB21         /* - DS1 */

    	
.EQU 	LED_PIO_CTRL,       1
.EQU 	LED_MASK,           LED1

#------------------------
#- Push Button Definition
#------------------------

.EQU  	BP_S1,     	   	PB20
.EQU  	BP_S2,     	   	PA9
.EQU  	BP_S3,     	   	PB17
.EQU  	BP_S4,     	   	PB19

.EQU	PB_PIO_CTRL,	1


#--------------------------
#- Serial EEPROM Definition
#--------------------------

.EQU	SCL,       	     PA1
.EQU	SDA,             PA2

.ENDIF   /*.IFDEF OUR_BOARD*/


.IFDEF FT_BOARD
#----------------------------------------
#- AT91EB55 External Memories Definition
#----------------------------------------
#- Flash Memory : AT49BV1604 1Mx16
.EQU	FLASH_BASE,     	0x01000000

.EQU 	EXT_SRAM_BASE,      0x02000000
.EQU 	EXT_SRAM_SIZE,      0x00080000      		/* 512Kbytes */
.EQU 	EXT_SRAM_LIMIT,     (EXT_SRAM_BASE+EXT_SRAM_SIZE)

#------------------------------------------------------------------------------
#- EBI Initialization Data
#-------------------------
#- The EBI User Interface Image which is copied by the boot.
#- 25 MHz master clock assumed.
#- That's hardware! Details in the Electrical Datasheet of the AT91 device.
#- EBI Base Address is added at the end for commodity in copy code.
#------------------------------------------------------------------------------
.EQU   EBI_CSR_0,  		  (FLASH_BASE | 0x3529)    /* 0x01000000, 16MB, 2 tdf, 16 bits, 2 WS */
.EQU   EBI_CSR_1,  		  (EXT_SRAM_BASE | 0x3121) /* 0x02000000, 16MB, 0 hold, 16 bits, 1 WS*/
.EQU   EBI_CSR_2,  		  0x20000000  /* unused */
.EQU   EBI_CSR_3,  		  0x30000000  /* unused */
.EQU   EBI_CSR_4,  		  0x40000000  /* unused */
.EQU   EBI_CSR_5,  		  0x50000000  /* unused */
.EQU   EBI_CSR_6,  		  0x60000000  /* unused */
.EQU   EBI_CSR_7,  		  0x70000000  /* unused */

#------------------
#- Leds Definition
#------------------

.EQU 	LED1,       	  PB8         /* - DS1 */
.EQU 	LED2,       	  PA4         /* - DS2 */
.EQU 	LED3,       	  PA5        /* - DS3 */
.EQU 	LED4,       	  PA6        /* - DS4 */
.EQU 	LED5,       	  PA7        /* - DS5 */
.EQU 	LED6,       	  PB13        /* - DS6 */
.EQU 	LED7,       	  PB14        /* - DS7 */
.EQU 	LED8,       	  PB15        /* - DS8 */
    	
.EQU 	LED_PIO_CTRL,       1
.EQU 	LED_MASK,           0x000000F0

#------------------------
#- Push Button Definition
#------------------------

.EQU  	BP_S1,     	   	PB20
.EQU  	BP_S2,     	   	PA9
.EQU  	BP_S3,     	   	PB17
.EQU  	BP_S4,     	   	PB19

.EQU	PB_PIO_CTRL,	1


#--------------------------
#- Serial EEPROM Definition
#--------------------------

.EQU	SCL,       	     PA1
.EQU	SDA,             PA2

.ENDIF   /*.IFDEF FT_BOARD*/





#英蓓特开发板
.IFDEF EMBEST_BOARD
#----------------------------------------
#- AT91EB55 External Memories Definition
#----------------------------------------
#- Flash Memory : AT49BV1604 1Mx16
.EQU	FLASH_BASE,     	0x01000000

.EQU 	EXT_SRAM_BASE,      0x02000000
.EQU 	EXT_SRAM_SIZE,      0x00040000      		/* 256Kbytes */
.EQU 	EXT_SRAM_LIMIT,     (EXT_SRAM_BASE+EXT_SRAM_SIZE)

#------------------------------------------------------------------------------
#- EBI Initialization Data
#-------------------------
#- The EBI User Interface Image which is copied by the boot.
#- 25 MHz master clock assumed.
#- That's hardware! Details in the Electrical Datasheet of the AT91 device.
#- EBI Base Address is added at the end for commodity in copy code.
#------------------------------------------------------------------------------
.EQU   EBI_CSR_0,  		  (FLASH_BASE | 0x2529)    /* 0x01000000, 16MB, 2 tdf, 16 bits, 2 WS */
.EQU   EBI_CSR_1,  		  (EXT_SRAM_BASE | 0x2121) /* 0x02000000, 16MB, 0 hold, 16 bits, 1 WS*/
.EQU   EBI_CSR_2,  		  0x20000000  /* unused */
.EQU   EBI_CSR_3,  		  0x30000000  /* unused */
.EQU   EBI_CSR_4,  		  0x40000000  /* unused */
.EQU   EBI_CSR_5,  		  0x50000000  /* unused */
.EQU   EBI_CSR_6,  		  0x60000000  /* unused */
.EQU   EBI_CSR_7,  		  0x70000000  /* unused */

#------------------
#- Leds Definition
#------------------

.EQU 	LED1,       	  PB8         /* - DS1 */
.EQU 	LED2,       	  PB9         /* - DS2 */
.EQU 	LED3,       	  PB10        /* - DS3 */
.EQU 	LED4,       	  PB11        /* - DS4 */
.EQU 	LED5,       	  PB12        /* - DS5 */
.EQU 	LED6,       	  PB13        /* - DS6 */
.EQU 	LED7,       	  PB14        /* - DS7 */
.EQU 	LED8,       	  PB15        /* - DS8 */
    	
.EQU 	LED_PIO_CTRL,       1
.EQU 	LED_MASK,           0x0000FF00

#------------------------
#- Push Button Definition
#------------------------

.EQU  	BP_S1,     	   	PB20
.EQU  	BP_S2,     	   	PA9
.EQU  	BP_S3,     	   	PB17
.EQU  	BP_S4,     	   	PB19

.EQU	PB_PIO_CTRL,	1


#--------------------------
#- Serial EEPROM Definition
#--------------------------

.EQU	SCL,       	     PA1
.EQU	SDA,             PA2

.ENDIF   /*.IFDEF EMBEST_BOARD*/

#            END
