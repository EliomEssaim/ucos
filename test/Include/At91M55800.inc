


#------------------------
#- Peripheral Memory Map
#------------------------
# AIC, APMC, SF and EBI base addresses are described in periph header files.

.EQU	RTC_BASE,       0xFFFB8000      /* Real Time Clock Base Address */








#----------------------------------
# Internal Memory Mapping and Size
#----------------------------------

.EQU	RAM_BASE,       0
.EQU	RAM_SIZE,       (8*1024)
.EQU	RAM_LIMIT,      (RAM_BASE+RAM_SIZE)
.EQU	RAM_BASE_BOOT,  0x00300000



















#---------------------
# ARM Core : ARM7TDMI
#---------------------
#        .INCLUDE         "../periph/arm7tdmi/arm.inc"
#--------------------------------
#- ARM Core Mode and Status Bits
#--------------------------------

.equ				ARM_MODE_USER,		0x10
.equ				ARM_MODE_FIQ,		0x11
.equ				ARM_MODE_IRQ,		0x12
.equ				ARM_MODE_SVC,		0x13
.equ				ARM_MODE_ABORT,		0x17
.equ				ARM_MODE_UNDEF,		0x1B
.equ				ARM_MODE_SYS,		0x1F

.equ				I_BIT,				0x80
.equ				F_BIT,				0x40
.equ				T_BIT,				0x20














        

#-----------------------------
#- Special Function Registers
#-----------------------------
#        .INCLUDE         "../periph/special_function/sf.inc"

#---------------------------------------------------------
# Chip Identification User Interface Structure Definition
#---------------------------------------------------------
.equ	SF_CIDR,        0x00       /* Chip ID Register */
.equ	SF_EXID,        0x04       /* Chip ID Extension Register */
.equ	SF_RSR,         0x08       /* Reset Status Register */
#;                      0x0C       /* Reserved 0 */
#;                      0x10       /* Reserved 1 */
#;                      0x14       /* Reserved 2 */
.equ	SF_PMR,         0x18       /* Protect Mode Register */

#-------------------------------
#- Special Function Base Address 
#-------------------------------

.equ	SF_BASE,            0xFFF00000

#--------------------------------------------------------
#- SF_CIDR: Chip Identification Register Bits Definition
#--------------------------------------------------------

.equ	SF_VERSION,       0x0000001F      /* Version Number */
.equ	SF_NVPSIZ,        0x00000F00      /* Non Volatile Program Memory Size */
.equ	SF_NVDSIZ,        0x0000F000      /* Non Volatile Data Memory Size */
.equ	SF_VDSIZ,         0x000F0000      /* Volatile Data Memory Size */
.equ	SF_ARCH,          0x0FF00000      /* Architecture Code */
.equ	SF_NVPTYP,        0x70000000      /* Non Volatile Program Memory Type */
.equ	SF_EXT,           0x10000000      /* Extension Flag */

#---------------------------------------
#- SF_RSR: Reset Status Flag Definition
#---------------------------------------

.equ	SF_EXT_RESET,        0x6C            /* External pin Cause Reset */
.equ	SF_WD_RESET,         0x53            /* Internal WatchDog Cause Reset */

.equ	SF_VDSIZ_MASK_SHIFT, 0x3C00          /* SF_NVDSIZ shifted by 6 */

.equ	SF_ARCH_AT91x63,     0x06300000
.equ	SF_ARCH_AT91x40,     0x04000000
.equ	SF_ARCH_AT91x55,     0x05500000

#--------------------------------
#- SF_PMR: Protect Mode Register
#--------------------------------

.equ	SF_AIC,             0x20            /* Protect Mode Enable */

















        

#------------------------
# External Bus Interface
#------------------------
#        .INCLUDE         "../periph/ebi/ebi.inc"
#--------------------------------------------------------
#- Memory Controller User Interface Structure Definition
#--------------------------------------------------------
#                    ^       0
.equ	EBI_CSR,       0x00         /* Chip Select Register */
.equ	EBI_RCR,       0x20         /* Remap Command Register */
.equ	EBI_MCR,       0x24         /* Memory Control Register */

#------------------
#- EBI Base Address
#------------------
.equ	EBI_BASE,      0xFFE00000      /* External Bus Interface */

#---------------------------------
#- EBI_CSR: Chip Select Registers
#---------------------------------

.equ	EBI_DBW,       0x03        /* Data Bus Width */
.equ	EBI_NWS,       0x1C        /* Number of Wait States */
.equ	EBI_WSE,       0x20        /* Wait State Enable */
.equ	EBI_PAGES,     0x180       /* Page size */
.equ	EBI_TDF,       0xE00       /* Number of Data Float Output Time Clock Cycle */
.equ	EBI_BAT,       0x1000      /* Byte Access Type */
.equ	EBI_CSE,       0x2000      /* Chip Select Enable */
.equ	EBI_BA,        0xFFF00000  /* Base Address */

#----------------------------------
#- EBI_RCR: Remap Control Register
#----------------------------------

.equ	EBI_RCB,       0x01    /* Remap Command Bit */

#-----------------------------------
#- EBI_MCR: Memory Control Register
#-----------------------------------

.equ	EBI_ALE,       0x07    /* Address Line Enable */
.equ	EBI_DRP,       0x10    /* Data Read Protocol */
















        

#-----------------------------
# Power Management Controller
#-----------------------------
#        .INCLUDE         "../periph/power_saving/apmc55800.inc"
#--------------------------------------------------------------------------
#- Advanced Power Management Controller User Interface Structure Definition
#--------------------------------------------------------------------------
#               ^       0
.EQU	C_SCER,      0x00       /* Source Clock Enable Register */
.EQU	APMC_SCDR,   0x04       /* Source Clock Disable Register */
.EQU	APMC_SCSR,   0x08       /* Source Clock Mask Register */
                #    0x0C       /* Reserved */
.EQU	APMC_PCER,   0x10       /* Peripheral Clock Enable Register */
.EQU	APMC_PCDR,   0x14       /* Peripheral Clock Disable Register */
.EQU	APMC_PCSR,   0x18       /* Peripheral Clock Status Register */
                #    0x1C       /* Reserved */
.EQU	APMC_CGMR,   0x20       /* Clock Generator Mode Register */
                #    0x24       /* Reserved */
.EQU	APMC_PCR,    0x28       /* Power Control Register */
.EQU	APMC_PMR,    0x2C       /* Power Mode Register */
.EQU	APMC_SR,     0x30       /* Status Register   */
.EQU	APMC_IER,    0x34       /* Interrupt Enable Register   */
.EQU	APMC_IDR,    0x38       /* Interrupt Disable Register   */
.EQU	APMC_IMR,    0x40       /* Interrupt Mask Register   */

#-----------------------------------------
# Power Management Controller Base Address
#-----------------------------------------

.EQU	APMC_BASE,                   0xFFFF4000

#------------------------
#- System Clock Registers
#------------------------

.EQU	ARM7DIS,   		0x1

#------------------------------------------------------------
#- Advanced Power Managment Control Register Bits Definition 
#------------------------------------------------------------
.EQU	APMC_ARM7DIS,	0x1

#-------------------------------------------------------------------------
#- Advanced Power Managment Clock Generator Mode Register Bits Definition 
#-------------------------------------------------------------------------
.EQU	APMC_MOSC_BYP,	0x1     /* Main Oscillator Bypass */
.EQU	MOSCS_MASK,   	0x1
.EQU	PLL_LOCK_MASK,  0x2

.EQU	APMC_MOSC_EN,   0x2     /* Main Oscillator Enable  */
.EQU	APMC_MCKO_DIS,  0x4     /* Disable Master clock output (tri-state)  */

.EQU	APMC_PRES_NONE, 0x0     /* No prescaler  */
.EQU	APMC_PRES_DIV2, 0x10    /* Selected Clock Divided by 2  */
.EQU	APMC_PRES_DIV4, 0x20    /* Selected Clock Divided by 4  */
.EQU	APMC_PRES_DIV8, 0x30    /* Selected Clock Divided by 8  */
.EQU	APMC_PRES_DIV16,	0x40    /* Selected Clock Divided by 16  */
.EQU	APMC_PRES_DIV32,	0x50    /* Selected Clock Divided by 32  */
.EQU	APMC_PRES_DIV64,	0x60    /* Selected Clock Divided by 64  */

.EQU	APMC_CSS_LF,    0x0     /* Low-Frequency Clock provided by RTC  */
.EQU	APMC_CSS_MOSC,  0x4000  /* Main Oscillator Output or External clock  */
.EQU	APMC_CSS_PLL,   0x8000  /* Phase Lock Loop Output  */

.EQU	B_MUL,          8
.EQU	B_OSCOUNT,      16
.EQU	B_PLLCOUNT,     24

#-------------------------------------------------
#- APM Power Controller Register Bits Definition  
#-------------------------------------------------
.EQU	APMC_SHDALC,    0x1     /* Shut-down or alarm Command */ 
.EQU	APMC_WKACKC,    0x2     /* Wake-up or Alarm Acknowledge Command      */

#------------------------------------------------
#- APM Power Mode Register Bits Definition       
#------------------------------------------------
.EQU	APMC_SHDALS_OUT_TRIS,        0x0     /* SHDALS pin is Tri-State  */
.EQU	APMC_SHDALS_OUT_LEVEL_0,     0x1     /* SHDALS pin is LEVEL 0  */
.EQU	APMC_SHDALS_OUT_LEVEL_1,     0x2     /* SHDALS pin LEVEL 1  */

.EQU	APMC_WKACKS_OUT_TRIS,        0x0     /* WKACKS pin is Tri-State  */
.EQU	APMC_WKACKS_OUT_LEVEL_0,     0x4     /* WKACKS pin  is LEVEL 0  */
.EQU	APMC_WKACKS_OUT_LEVEL_1,     0x8     /* WKACKS pin  is LEVEL 1  */

.EQU	APMC_WKEN,                   0x10    /* Alarm Wake-up Enable  */
.EQU	APMC_ALSHEN,                 0x20    /* Alarm Shut-Down Enable  */

.EQU	APMC_WKEDG_NONE,             0x00    /* None. No edge is detected on Wake-up  */
.EQU	APMC_WKEDG_POS_EDG,          0x40    /* Positive edge detection  */
.EQU	APMC_WKEDG_NEG_EDG,          0x80    /* Negative edge detection  */

#-------------------------------------------------------
#- APM SR, IER, IDR and IMR Registers Bits Definition   
#-------------------------------------------------------
.EQU	APMC_MOSCS,                  0x1     /* Main Osillator Status  */
.EQU	APMC_PLL_LOCK,               0x2     /* PLL Lock Status  */























        


#-------------------------------------------------
# AIC Interrupt Sources and peripheral Identifier
#-------------------------------------------------
#        .INCLUDE         "../periph/aic/aic.inc"

#------------------------------------------
#- AIC User Interface Structure Definition
#------------------------------------------

.equ			AIC_SMR,		0x000				/* Source Mode Register */
.equ			AIC_SVR,		0x080				/* Source Vector Register */
.equ			AIC_IVR,		0x100				/* Interrupt Vector Register */
.equ			AIC_FVR,		0x104				/* Fast Interrupt Vector Register */
.equ			AIC_ISR,		0x108				/* Interrupt Status Register */
.equ			AIC_IPR,		0x10c				/* Interrupt Pending Register */
.equ			AIC_IMR,		0x110				/* Interrupt Mask Register */
.equ			AIC_CISR,		0x114				/* Core Interrupt Status Register */
#;								0x118				/* Reserved 0 */
#;								0x11C				/* Reserved 1 */
.equ			AIC_IECR,		0x120           	/* Interrupt Enable Command Register */
.equ			AIC_IDCR,		0x124           	/* Interrupt Disable Command Register */
.equ			AIC_ICCR,		0x128           	/* Interrupt Clear Command Register */
.equ			AIC_ISCR,		0x12c           	/* Interrupt Set Command Register */
.equ			AIC_EOICR,		0x130	          	/* End Of Interrupt Command Register */
.equ			AIC_SPU,		0x134	          	/* Spurious Vector Register */


.equ		AIC_BASE,			0xFFFFF000

#---------------------------------------------
#- AIC_SMR[]: Interrupt Source Mode Registers
#---------------------------------------------

.equ		AIC_PRIOR,									0x07			/* Priority */

.equ		AIC_SRCTYPE,								0x60			/* Source Type Definition */
.equ		AIC_SRCTYPE_INT_LEVEL_SENSITIVE,			0x00			/* Level Sensitive */
.equ		AIC_SRCTYPE_INT_EDGE_TRIGGERED,				0x20			/* Edge Triggered */
.equ		AIC_SRCTYPE_EXT_LOW_LEVEL,					0x00			/* Low Level */
.equ		AIC_SRCTYPE_EXT_NEGATIVE_EDGE,				0x20			/* Negative Edge */
.equ		AIC_SRCTYPE_EXT_HIGH_LEVEL,					0x40			/* High Level */
.equ		AIC_SRCTYPE_EXT_POSITIVE_EDGE,				0x60			/* Positive Edge */

#--------------------------------------
#-  AIC_ISR: Interrupt Status Register
#--------------------------------------

.equ		AIC_IRQID,			0x1F									/* Current source interrupt */

#-------------------------------------------
#- AIC_CISR: Interrupt Core Status Register
#-------------------------------------------

.equ		AIC_NFIQ,			0x01									/* Core FIQ Status */
.equ		AIC_NIRQ,			0x02									/* Core IRQ Status */

#--------------------------------------------
#- Advanced Interrupt COntroller BAse Address
#--------------------------------------------


.EQU	AIC_FIQ_ID,     0       /* Fast Interrupt */

.EQU	AIC_SWIRQ_ID,   1       /* Soft Interrupt (generated by the AIC) */

.EQU	AIC_US0IRQ_ID,  2       /* USART Channel 0 interrupt */
.EQU	AIC_US1IRQ_ID,  3       /* USART Channel 1 interrupt */
.EQU	AIC_US2IRQ_ID,  4       /* USART Channel 2 interrupt */

.EQU	AIC_SPIRQ_ID,   5       /* SPI interrupt */

.EQU	AIC_TC0IRQ_ID,  6       /* Timer Channel 0 interrupt */
.EQU	AIC_TC1IRQ_ID,  7       /* Timer Channel 1 interrupt */
.EQU	AIC_TC2IRQ_ID,  8       /* Timer Channel 2 interrupt */
.EQU	AIC_TC3IRQ_ID,  9       /* Timer Channel 3 interrupt */
.EQU	AIC_TC4IRQ_ID,  10      /* Timer Channel 4 interrupt */
.EQU	AIC_TC5IRQ_ID,  11      /* Timer Channel 5 interrupt */

.EQU	AIC_WDIRQ_ID,   12      /* Watchdog interrupt */

.EQU	AIC_PIOAIRQ_ID, 13      /* Parallel I/O Controller A interrupt */
.EQU	AIC_PIOBIRQ_ID, 14      /* Parallel I/O Controller B interrupt */

.EQU	AIC_AD0IRQ_ID,  15      /* Analog to Digital Converter 0 Interrupt */
.EQU	AIC_AD1IRQ_ID,  16      /* Analog to Digital Converter 1 Interrupt */

.EQU	AIC_DA0IRQ_ID,  17      /* Digital to Analog Converter 0 Interrupt */
.EQU	AIC_DA1IRQ_ID,  18      /* Digital to Analog Converter 1 Interrupt */

.EQU	AIC_RTCIRQ_ID,  19      /* Real Time Clock Interrupt */

.EQU	AIC_APMCIRQ_ID, 20      /* Advanced Power Management Controller Interrupt */

.EQU	AIC_IRQ6_ID,    23      /* External interrupt 6 */
.EQU	AIC_IRQ5_ID,    24      /* External interrupt 5 */
.EQU	AIC_IRQ4_ID,    25      /* External interrupt 4 */
.EQU	AIC_IRQ3_ID,    26      /* External interrupt 3 */
.EQU	AIC_IRQ2_ID,    27      /* External interrupt 2 */
.EQU	AIC_IRQ1_ID,    28      /* External interrupt 1 */
.EQU	AIC_IRQ0_ID,    29      /* External interrupt 0 */






















#-----------------
# PIO Controllers
#-----------------
#        .INCLUDE     "../periph/pio/pioa.inc"
#        .INCLUDE     "../periph/pio/piob.inc"

#------------------------------------------------------------
# Parallel IO Controller User Interface Structure Definition
#------------------------------------------------------------
.equ	PIO_PER,		0x00	       /* PIO Enable Register */
.equ	PIO_PDR,        0x04           /* PIO Disable Register */
.equ	PIO_PSR,       	0x08           /* PIO Status Register */
#;                     	0x0C           /* Reserved 0 */
.equ	PIO_OER,        0x10           /* Output Enable Register */
.equ	PIO_ODR,        0x14           /* Output Disable Register */
.equ	PIO_OSR,        0x18           /* Output Status Register */
#;                      0x1C           /* Reserved 1 */
.equ	PIO_IFER,       0x20           /* Input Filter Enable Register */
.equ	PIO_IFDR,       0x24           /* Input Filter Disable Register */
.equ	PIO_IFSR,       0x28           /* Input Filter Status Register */
#;                      0x2C           /* Reserved 2 */
.equ	PIO_SODR,       0x30           /* Set Output Data Register */
.equ	PIO_CODR,       0x34           /* Clear Output Data Register */
.equ	PIO_ODSR,       0x38           /* Output Data Status Register */
.equ	PIO_PDSR,       0x3C           /* Pin Data Status Register */
.equ	PIO_IER,        0x40           /* Interrupt Enable Register */
.equ	PIO_IDR,        0x44           /* Interrupt Disable Register */
.equ	PIO_IMR,        0x48           /* Interrupt Mask Register */
.equ	PIO_ISR,        0x4C           /* Interrupt Status Register */
.equ	PIO_MDER,       0x50           /* Interrupt Enable Register */
.equ	PIO_MDDR,       0x54           /* Interrupt Disable Register */
.equ	PIO_MDSR,       0x58           /* Interrupt Status Register */

.EQU	PIOB_BASE,      0xFFFF0000      /* Parallel I/O Controller B Base Address */
.EQU	PIOA_BASE,      0xFFFEC000      /* Parallel I/O Controller A Base Address */


#-------------------------------------------------
#- PIO Controller Descriptor Structure Definition
#-------------------------------------------------
.equ	PioBase,             0x00          /* Peripheral base address */
.equ	AsmPioHandler,       0x04          /* Assembly interrupt handler */
.equ	PioHandlerPt,        0x08
.equ	PeriphId,            0x09          /* Peripheral Identifier */
.equ	PioNumber,           0x0A


#-------------------------------------------------
#- PIOA
#-------------------------------------------------
.equ	PA0,                  (1<<0)
.equ	PA1,                  (1<<1)
.equ	PA2,                  (1<<2)
.equ	PA3,                  (1<<3)
.equ	PA4,                  (1<<4)
.equ	PA5,                  (1<<5)
.equ	PA6,                  (1<<6)
.equ	PA7,                  (1<<7)
.equ	PA8,                  (1<<8)
.equ	PA9,                  (1<<9)
.equ	PA10,                 (1<<10)
.equ	PA11,                 (1<<11)
.equ	PA12,                 (1<<12)
.equ	PA13,                 (1<<13)
.equ	PA14,                 (1<<14)
.equ	PA15,                 (1<<15)
.equ	PA16,                 (1<<16)
.equ	PA17,                 (1<<17)
.equ	PA18,                 (1<<18)
.equ	PA19,                 (1<<19)
.equ	PA20,                 (1<<20)
.equ	PA21,                 (1<<21)
.equ	PA22,                 (1<<22)
.equ	PA23,                 (1<<23)
.equ	PA24,                 (1<<24)
.equ	PA25,                 (1<<25)
.equ	PA26,                 (1<<26)
.equ	PA27,                 (1<<27)
.equ	PA28,                 (1<<28)
.equ	PA29,                 (1<<29)

#-------------------------------------------------
#- PIOB
#-------------------------------------------------
.equ	PB0,                  (1<<0)
.equ	PB1,                  (1<<1)
.equ	PB2,                  (1<<2)
.equ	PB3,                  (1<<3)
.equ	PB4,                  (1<<4)
.equ	PB5,                  (1<<5)
.equ	PB6,                  (1<<6)
.equ	PB7,                  (1<<7)
.equ	PB8,                  (1<<8)
.equ	PB9,                  (1<<9)
.equ	PB10,                 (1<<10)
.equ	PB11,                 (1<<11)
.equ	PB12,                 (1<<12)
.equ	PB13,                 (1<<13)
.equ	PB14,                 (1<<14)
.equ	PB15,                 (1<<15)
.equ	PB16,                 (1<<16)
.equ	PB17,                 (1<<17)
.equ	PB18,                 (1<<18)
.equ	PB19,                 (1<<19)
.equ	PB20,                 (1<<20)
.equ	PB21,                 (1<<21)
.equ	PB22,                 (1<<22)
.equ	PB23,                 (1<<23)
.equ	PB24,                 (1<<24)
.equ	PB25,                 (1<<25)
.equ	PB26,                 (1<<26)
.equ	PB27,                 (1<<27)


.EQU	NB_PIOA,        30      /* Number of PIO A Lines */
.EQU	NB_PIOB,        28      /* Number of PIO A Lines */

#--------
# Port A
#--------
.EQU	TCLK3,          PA0
.EQU	TIOA3,          PA1
.EQU	TIOB3,          PA2
.EQU	PIN_TC3,        (TIOA3 | TIOB3 | TCLK3)

.EQU	TCLK4,          PA3
.EQU	TIOA4,          PA4
.EQU	TIOB4,          PA5
.EQU	PIN_TC4,        (TIOA4 | TIOB4 | TCLK4)

.EQU	TCLK5,          PA6
.EQU	TIOA5,          PA7
.EQU	TIOB5,          PA8
.EQU	PIN_TC5,        (TIOA5 | TIOB5 | TCLK5)

.EQU	PIN_IRQ0,       PA9
.EQU	PIN_IRQ1,       PA10
.EQU	PIN_IRQ2,       PA11
.EQU	PIN_IRQ3,       PA12
.EQU	PIN_FIQ,        PA13

.EQU	SCK0,           PA14
.EQU	TXD0,           PA15
.EQU	RXD0,           PA16
.EQU	PIN_USART0,     (SCK0 | TXD0 | RXD0)

.EQU	SCK1,           PA17
.EQU	TXD1,           PA18
.EQU	RXD1,           PA19
.EQU	PIN_USART1,     (SCK1 | TXD1 | RXD1)

.EQU	SCK2,           PA20
.EQU	TXD2,           PA21
.EQU	RXD2,           PA22
.EQU	PIN_USART2,     (SCK2 | TXD2 | RXD2)

.EQU	SPCK,           PA23
.EQU	MISO,           PA24
.EQU	MOSI,           PA25
.EQU	NPCS0,          PA26
.EQU	NPCS1,          PA27
.EQU	NPCS2,          PA28
.EQU	NPCS3,          PA29

#--------
# Port B
#--------
.EQU	IRQ4,           PB3                         /* External Interrupt 4 */
.EQU	IRQ5,           PB4                         /* External Interrupt 5 */
.EQU	IRQ6,           PB5                         /* External Interrupt 6 */

.EQU	AD0TRIG,        PB6                         /* ADC0 External Trigger */
.EQU	AD1TRIG,        PB7                         /* ADC1 External Trigger */

.EQU	BMS,            PB18                        /* Boot Mode Select */

.EQU	TCLK0,          PB19
.EQU	TIOA0,          PB20
.EQU	TIOB0,          PB21
.EQU	PIN_TC0,        (TIOA0 | TIOB0 | TCLK0)

.EQU	TCLK1,          PB22
.EQU	TIOA1,          PB23
.EQU	TIOB1,          PB24
.EQU	PIN_TC1,        (TIOA1 | TIOB1 | TCLK1)

.EQU	TCLK2,          PB25
.EQU	TIOA2,          PB26
.EQU	TIOB2,          PB27
.EQU	PIN_TC2,        (TIOA2 | TIOB2 | TCLK2)




















#---------
#- USARTs
#---------
#- Definition file
#        .INCLUDE         "../periph/usart/usart.inc"
#-------------------------------------------
# USART User Interface Structure Definition
#-------------------------------------------
.EQU	US_CR,           	0x0       		/* - Control Register				*/
.EQU	US_MR,           	0x4       		/* - Mode Register                  */
.EQU	US_IER,          	0x8       		/* - Interrupt Enable Register      */
.EQU	US_IDR,          	0xc       		/* - Interrupt Disable Register     */
.EQU	US_IMR,          	0x10       		/* - Interrupt Mask Register        */
.EQU	US_CSR,          	0x14       		/* - Channel Status Register        */
.EQU	US_RHR,          	0x18       		/* - Receive Holding Register       */
.EQU	US_THR,          	0x1c       		/* - Transmit Holding Register      */
.EQU	US_BRGR,         	0x20       		/* - Baud Rate Generator Register   */
.EQU	US_RTOR,         	0x24       		/* - Receiver Timeout Register      */
.EQU	US_TTGR,         	0x28       		/* - Transmitter Time-guard Register*/
#;		                	0x2c       		/* - Reserved                       */
.EQU	US_RPR,          	0x30       		/* - Receiver Pointer Register      */
.EQU	US_RCR,          	0x34       		/* - Receiver Counter Register      */
.EQU	US_TPR,          	0x38       		/* - Transmitter Pointer Register   */
.EQU	US_TCR,          	0x3c       		/* - Transmitter Counter Register   */

.EQU	USART2_BASE,    0xFFFC8000      /* USART 2 Base Address */
.EQU	USART1_BASE,    0xFFFC4000      /* USART 1 Base Address */
.EQU	USART0_BASE,    0xFFFC0000      /* USART 0 Base Address */

#---------------------------
#- US_CR : Control Register
#---------------------------
.EQU    US_RSTRX,            0x0004      /* - Reset Receiver		*/
.EQU    US_RSTTX,            0x0008      /* - Reset Transmitter     */
.EQU    US_RXEN,             0x0010      /* - Receiver Enable       */
.EQU    US_RXDIS,            0x0020      /* - Receiver Disable      */
.EQU    US_TXEN,             0x0040      /* - Transmitter Enable    */
.EQU    US_TXDIS,            0x0080      /* - Transmitter Disable   */
.EQU    US_RSTSTA,           0x0100      /* - Reset Status Bits     */
.EQU    US_STTBRK,           0x0200      /* - Start Break           */
.EQU    US_STPBRK,           0x0400      /* - Stop Break            */
.EQU    US_STTTO,            0x0800      /* - Start Time-out        */
.EQU    US_SENDA,            0x1000      /* - Send Address          */

#------------------------
#- US_MR : Mode Register
#------------------------

.EQU    US_CLKS,             0x0030      /* - Clock Selection				*/
.EQU    US_CLKS_MCKI,        0x00        /* - Master Clock                  */
.EQU    US_CLKS_MCKI8,       0x10        /* - Master Clock divided by 8     */
.EQU    US_CLKS_SCK,         0x20        /* - External Clock                */
.EQU    US_CLKS_SLCK,        0x30        /* - Slow Clock                    */
                                                                            
.EQU    US_CHRL,             0x00C0      /* - Byte Length                   */
.EQU    US_CHRL_5,           0x00        /* - 5 bits                        */
.EQU    US_CHRL_6,           0x40        /* - 6 bits                        */
.EQU    US_CHRL_7,           0x80        /* - 7 bits                        */
.EQU    US_CHRL_8,           0xC0        /* - 8 bits                        */
                                                                            
.EQU    US_SYNC,             0x0100      /* - Synchronous Mode Enable       */
                                                                            
.EQU    US_PAR,              0x0E00      /* - Parity Mode                   */
.EQU    US_PAR_EVEN,         0x00        /* - Even Parity                   */
.EQU    US_PAR_ODD,          0x20        /* - Odd Parity                    */
.EQU    US_PAR_SPACE,        0x40        /* - Space Parity to 0             */
.EQU    US_PAR_MARK,         0x60        /* - Marked Parity to 1            */
.EQU    US_PAR_NO,           0x80        /* - No Parity                     */
.EQU    US_PAR_MULTIDROP,    0xA0        /* - Multi-drop Mode               */
                                                                            
.EQU    US_NBSTOP,           0x3000      /* - Stop Bit Number               */
.EQU    US_NBSTOP_1,         0x0000      /* - 1 Stop Bit                    */
.EQU    US_NBSTOP_1_5,       0x1000      /* - 1.5 Stop Bits                 */
.EQU    US_NBSTOP_2,         0x2000      /* - 2 Stop Bits                   */
                                                                            
.EQU    US_CHMODE,                   0xC000  /* - Channel Mode                */
.EQU    US_CHMODE_NORMAL,            0x0000  /* - Normal Mode                 */
.EQU    US_CHMODE_AUTOMATIC_ECHO,    0x4000  /* - Automatic Echo              */
.EQU    US_CHMODE_LOCAL_LOOPBACK,    0x8000  /* - Local Loopback              */
.EQU    US_CHMODE_REMOTE_LOOPBACK,   0xC000  /* - Remote Loopback             */
                                                                            
.EQU    US_MODE9,            0x20000     /* - 9 Bit Mode                    */
                                                                            
.EQU    US_CLKO,             0x40000     /* - Baud Rate Output Enable       */

#------------------------------------------------------------------
#-  US_IER, US_IDR, US_IMR, US_IMR: Status and Interrupt Registers
#------------------------------------------------------------------

.EQU    US_RXRDY,            0x001       /* - Receiver Ready					*/				
.EQU    US_TXRDY,            0x002       /* - Transmitter Ready                 */
.EQU    US_RXBRK,            0x004       /* - Receiver Break                    */
.EQU    US_ENDRX,            0x008       /* - End of Receiver PDC Transfer      */
.EQU    US_ENDTX,            0x010       /* - End of Transmitter PDC Transfer   */
.EQU    US_OVRE,             0x020       /* - Overrun Error                     */
.EQU    US_FRAME,            0x040       /* - Framing Error                     */
.EQU    US_PARE,             0x080       /* - Parity Error                      */
.EQU    US_TIMEOUT,          0x100       /* - Receiver Timeout                  */
.EQU    US_TXEMPTY,          0x200       /* - Transmitter Empty                 */

#------------------------------
#- USART Descriptor Structure
#------------------------------
.EQU    UsartDesc_UsartBase,      0x0       /* - Peripheral base address		*/
.EQU    UsartDesc_PioCtrl,        0x4       /* - IO controller descriptor     */
.EQU    UsartDesc_PinRXD,         0x8       /* - RXD pin number in the PIO    */
.EQU    UsartDesc_PinTXD,         0x9       /* - TXD pin number in the PIO    */
.EQU    UsartDesc_PinSCK,         0xa       /* - SCK pin number in the PIO    */
.EQU    UsartDesc_PeriphId,       0xb       /* - USART Peripheral Identifier  */





















        

#-----------
#- Watchdog
#-----------
#- Definition file
#        .INCLUDE         "../periph/watchdog/wd.inc"
#-----------------------------------------------
# Watch Dog User Interface Structure Definition
#-----------------------------------------------
.equ	WD_OMR,       0x00           /* Overflow Mode Register */
.equ	WD_CMR,       0x04           /* Counter Mode Register */
.equ	WD_CR,        0x08           /* Control Register */
.equ	WD_SR,        0x0C           /* Status Register */
#;                    0x10           /* Reserved */
.equ	WD_TLR,       0x14           /* Test Load Register : test purpose only */

.equ    WD_BASE,      0xFFFF8000       /* WatchDog */

#-----------------------------------------------------------
#- WD_OMR: Watch Dog Overflow Mode Register Bits Definition
#-----------------------------------------------------------

.equ	WD_WDEN,      0x01        	/* Watch Dog Enable */
.equ	WD_RSTEN,     0x02        	/* Internal Reset Enable */
.equ	WD_IRQEN,     0x04        	/* Interrupt Enable */
.equ	WD_EXTEN,     0x08        	/* External Signal Enable */
.equ	WD_OKEY,      0x2340      	/* Overflow Mode Register Access Key */

#---------------------------------------------------
#- WD_CMR: Watch Dog Clock Register Bits Definition
#---------------------------------------------------

.equ	WD_WDCLKS,                0x03        /* Clock Selection */
.equ	WD_WDCLKS_MCK8,           0x0
.equ	WD_WDCLKS_MCK32,          0x1
.equ	WD_WDCLKS_MCK128,         0x2
.equ	WD_WDCLKS_MCK1024,        0x3

.equ	WD_HPCV,                  (0x0F<<2)    /* High Preload Counter Value */

.equ	WD_CKEY,                  (0x06E<<7)   /* Clock Register Access Key */

#----------------------------------------------------
#- WD_CR: Watch Dog Control Register Bits Definition
#----------------------------------------------------

.equ	WD_RSTKEY,           0xC071      /* Watch Dog Restart Key */

#---------------------------------------------------
#- WD_SR: Watch Dog Status Register Bits Definition
#---------------------------------------------------

.equ	WD_WDOVF,            0x1         /* WatchDog Overflow Status */


#----------------------------------------------------
#-  WD_TLR: Test Load Register for test purpose only
#----------------------------------------------------

.equ	WD_TMRKEY,           0xD64A0000  /* Access Key */
.equ	WD_TESTEN,           0x2         /* Test Mode Enable */




























        

#----------------
#- Timer Counter
#----------------
#        .INCLUDE         "../periph/timer_counter/tc.inc"
#----------------------------------------------------
# Timer Counter  User Interface Structure Definition
#----------------------------------------------------
.equ		TC_CCR,       0x00           /* Channel Control Register */
.equ		TC_CMR,       0x04           /* Channel Mode Register */
#;                        0x08           /* Reserved 0 */
#;                        0x0C           /* Reserved 1 */
.equ		TC_CV,        0x10           /* Counter Value */
.equ		TC_RA,        0x14           /* Register A */
.equ		TC_RB,        0x18           /* Register B */
.equ		TC_RC,        0x1C           /* Register C */
.equ		TC_SR,        0x20           /* Status Register */
.equ		TC_IER,       0x24           /* Interrupt Enable Register */
.equ		TC_IDR,       0x28           /* Interrupt Disable Register */
.equ		TC_IMR,       0x2C           /* Interrupt Mask Register */
#;                        0x30           /* Reserved 2 */
#;                        0x34           /* Reserved 3 */
#;                        0x38           /* Reserved 4 */
#;                        0x3C           /* Reserved 5 */
.equ		TC_SIZE,      0x40

#------------------------------
# Timer Counter Block Structure
#------------------------------
.equ		NB_TC_CHANNEL,   3

.equ		TC0,          0
.equ		TC1,          TC_SIZE
.equ		TC2,          (TC_SIZE * 2)
.equ		TC_BCR,       (TC_SIZE * 3)
.equ		TC_BMR,       (TC_SIZE * 3 + 4)

.EQU	TCB1_BASE,      0xFFFD4000      /* Channels 3, 4, 5 Base Address */
.EQU	TCB0_BASE,      0xFFFD0000      /* Channels 0, 1, 2 Base Address */

#---------------------------------------------------------
#- TC_CCR: Timer Counter Control Register Bits Definition
#---------------------------------------------------------

.equ		TC_CLKEN,      0x1          /* Counter Clock Enable */
.equ		TC_CLKDIS,     0x2          /* Counter Clock Disable */
.equ		TC_SWTRG,      0x4          /* Software Trigger */

#------------------------------------------------------
#- TC_CMR: Timer Counter Mode Register Bits Definition
#------------------------------------------------------

.equ		TC_TCCLKS,     0x00000007      /* Clock Selection */
.equ		TC_CLKI,       0x00000008      /* Clock Inversion */
.equ		TC_BURST,      0x00000030      /* Burst Signal Selection */

.equ		TC_LDBSTOP,    0x00000040      /* Capture Mode : Counter Clock Stopped with RB Loading */
.equ		TC_CPCSTOP,    0x00000040      /* Waveform Mode : Counter Clock Stopped with RC Compare */

.equ		TC_LDBDIS,     0x00000080      /* Capture Mode : Counter Clock Disabled with RB Loading */
.equ		TC_CPCDIS,     0x00000080      /* Waveform Mode : Counter Clock Disabled with RC Compare */

.equ		TC_ETRGEDG,    0x00000300      /* Capture Mode : External Trigger Edge Selection */
.equ		TC_EEVTEDG,    0x00000300      /* Waveform Mode : External Event Edge Selection */

.equ		TC_ABETRG,     0x00000400      /* Capture Mode : TIOA or TIOB External Trigger Selection */

.equ		TC_EEVT,       0x00000C00      /* Waveform Mode : External Event Selection */
.equ		TC_ENETRG,     0x00001000      /* Waveform Mode : Enable Trigger on External Event */
.equ		TC_CPCTRG,     0x00004000      /* RC Compare Enable Trigger Enable */
.equ		TC_WAVE,       0x00008000      /* Mode Selection */
.equ		TC_LDRA,       0x00030000      /* Capture Mode : RA Loading Selection */
.equ		TC_ACPA,       0x00030000      /* Waveform Mode : RA Compare Effect on TIOA */
.equ		TC_LDRB,       0x000C0000      /* Capture Mode : RB Loading Selection */
.equ		TC_ACPC,       0x000C0000      /* Waveform Mode : RC Compare Effect on TIOA */
.equ		TC_AEEVT,      0x00300000      /* Waveform Mode : External Event Effect on TIOA */
.equ		TC_ASWTRG,     0x00C00000      /* Waveform Mode : Software Trigger Effect on TIOA */
.equ		TC_BCPB,       0x03000000      /* Waveform Mode : RB Compare Effect on TIOB */
.equ		TC_BCPC,       0x0C000000      /* Waveform Mode : RC Compare Effect on TIOB */
.equ		TC_BEEVT,      0x30000000      /* Waveform Mode : External Event Effect on TIOB */
.equ		TC_BSWTRG,     0xC0000000      /* Waveform Mode : Software Trigger Effect on TIOB */

#------------------------------------------------
#- TC_SR: Timer Counter Status Register Bits Definition
#------------------------------------------------

.equ		TC_COVFS,      0x01        /* Counter Overflow Status */
.equ		TC_LOVRS,      0x02        /* Load Overrun Status */
.equ		TC_CPAS,       0x04        /* RA Compare Status */
.equ		TC_CPBS,       0x08        /* RB Compare Status */
.equ		TC_CPCS,       0x10        /* RC Compare Status */
.equ		TC_LDRAS,      0x20        /* RA Loading Status */
.equ		TC_LDRBS,      0x40        /* RB Loading Status */
.equ		TC_ETRGS,      0x80        /* External Trigger Status */
.equ		TC_CLKSTA,     0x10000     /* Clock Status */
.equ		TC_MTIOA,      0x20000     /* TIOA Mirror */
.equ		TC_MTIOB,      0x40000     /* TIOB Status */

#----------------------------------------------------------------
#- TC_BCR: Timer Counter Block Control Register Bits Definition
#----------------------------------------------------------------

.equ		TC_SYNC,       0x1         /* Synchronisation Trigger */

#------------------------------------------------------------
#- TC_BMR: Timer Counter Block Mode Register Bits Definition
#------------------------------------------------------------

.equ		TC_TC0XC0S,    (0x3<<0)     /* External Clock Signal 0 Selection */
.equ		TC_TC1XC1S,    (0x3<<2)     /* External Clock Signal 1 Selection */
.equ		TC_TC2XC2S,    (0x3<<4)     /* External Clock Signal 2 Selection */

#- Timer Counter Channel Descriptor Structure
.equ		TCDesc_TCBase,         0x00       /* Peripheral base address */
.equ		TCDesc_PioCtrl,        0x04       /* IO controller descriptor */
.equ		TCDesc_AsmTCHandler,   0x08       /* Assembly interrupt handler */
.equ		TCDesc_TCHandler,      0x0C
.equ		TCDesc_PeriphId,       0x0D       /* Peripheral Identifier */
.equ		TCDesc_PioTioa,        0x0E       /* TIOA pin number in the PIO */
.equ		TCDesc_PioTiob,        0x0F       /* TIOB pin number in the PIO */
.equ		TCDesc_PioTclk,        0x10       /* TCLK pin number in the PIO */








#--------------------------------------
# Serial Peripheral Interface Structure
#--------------------------------------



.EQU	SPI_BASE,       0xFFFC8000      /* SPI Base Address */




#               END

