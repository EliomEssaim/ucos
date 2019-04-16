/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

自定义的关于AD的寄存器

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef AD_H_MY
#define AD_H_MY

//------------------------------------------------------
#define  ADC_BASE_0   0xfffb0000
#define  ADC_BASE_1   0xfffb4000

#define ADC_CR_0     	(ADC_BASE_0 + 0x00)
#define ADC_MR_0     	(ADC_BASE_0 + 0x04)
#define ADC_CHER_0     	(ADC_BASE_0 + 0x10)
#define ADC_CHDR_0     	(ADC_BASE_0 + 0x14)
#define ADC_CHSR_0     	(ADC_BASE_0 + 0x18)
#define ADC_SR_0     	(ADC_BASE_0 + 0x20)
#define ADC_IER_0     	(ADC_BASE_0 + 0x24)
#define ADC_IDR_0     	(ADC_BASE_0 + 0x28)
#define ADC_IMR_0     	(ADC_BASE_0 + 0x2c)
#define ADC_CDR0_0     	(ADC_BASE_0 + 0x30)
#define ADC_CDR1_0     	(ADC_BASE_0 + 0x34)
#define ADC_CDR2_0     	(ADC_BASE_0 + 0x38)
#define ADC_CDR3_0     	(ADC_BASE_0 + 0x3c)	

#define ADC_CR_1     	(ADC_BASE_1 + 0x00)
#define ADC_MR_1     	(ADC_BASE_1 + 0x04)
#define ADC_CHER_1     	(ADC_BASE_1 + 0x10)
#define ADC_CHDR_1     	(ADC_BASE_1 + 0x14)
#define ADC_CHSR_1     	(ADC_BASE_1 + 0x18)
#define ADC_SR_1     	(ADC_BASE_1 + 0x20)
#define ADC_IER_1     	(ADC_BASE_1 + 0x24)
#define ADC_IDR_1     	(ADC_BASE_1 + 0x28)
#define ADC_IMR_1     	(ADC_BASE_1 + 0x2c)
#define ADC_CDR0_1     	(ADC_BASE_1 + 0x30)
#define ADC_CDR1_1     	(ADC_BASE_1 + 0x34)
#define ADC_CDR2_1     	(ADC_BASE_1 + 0x38)
#define ADC_CDR3_1     	(ADC_BASE_1 + 0x3c)

//#define ADC_SWRST 	0x01
//#define ADC_START  	0x02

//#define ADC_PRESCAL 	0x3f00

#endif
