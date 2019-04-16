/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

ad.h

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/**===========================================================================

 Revisions of ad.h
 Version  		Name       	Date		Description
 1.0  			Eric	  			  	Initial Version  

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifndef AD_H
#define AD_H

#include "../../include/UserType.h"
#include "../../include/at91m55800.h"
#include "../../include/MyBoard.h"
#include "../../periph/pio/pio.h"
#include "ad_55800.h"
#include "comdef.h"

//���������ܽţ������ⲿ3��ģ�⿪��оƬ��U6��U7��U30����Ƭѡ
#define ADC_SE  PB25  //PB�� ����3��ģ�⿪�ص�A��B��������л����ߵ�ƽѡ��A��
#define ADC_EN1 PB26  //PB�� ��·ͼ��U6��U7��Ƭѡ���͵�ƽѡ��
#define ADC_EN2 PB27  //PB�� ��·ͼ��U30��Ƭѡ���͵�ƽѡ��

#define SELECT_A  PioWrite( &g_stPioB, ADC_SE, PIO_CLEAR_OUT) //��ADC_SE�����ߣ�ѡ��A��
//#define SELECT_A  PioWrite( PIO_SODR_B, ADC_SE, ADC_SE) //��ADC_SE�����ߣ�ѡ��A��
#define SELECT_B  PioWrite( &g_stPioB, ADC_SE, PIO_SET_OUT) //��ADC_SE�����㣬ѡ��B��
//#define SELECT_B  MA_OUTWM( PIO_CODR_B, ADC_SE, ADC_SE) //��ADC_SE�����㣬ѡ��B��

#define DISSELECT_U6U7  PioWrite( &g_stPioB, ADC_EN1, PIO_SET_OUT) //��ADC_EN1�����ߣ���ѡ��U6��U7
//#define DISSELECT_U6U7 MA_OUTWM( PIO_SODR_B, ADC_EN1, ADC_EN1) //��ADC_EN1�����ߣ���ѡ��U6��U7
#define SELECT_U6U7  PioWrite( &g_stPioB, ADC_EN1, PIO_CLEAR_OUT) //��ADC_EN1�����㣬ѡ��U6��U7
//#define SELECT_U6U7    MA_OUTWM( PIO_CODR_B, ADC_EN1, ADC_EN1) //��ADC_EN1�����㣬ѡ��U6��U7

#define DISSELECT_U30  PioWrite( &g_stPioB, ADC_EN2, PIO_SET_OUT) //��ADC_EN2�����ߣ���ѡ��U30
//#define DISSELECT_U30 MA_OUTWM( PIO_SODR_B, ADC_EN2, ADC_EN2) //��ADC_EN2�����ߣ���ѡ��U30
#define SELECT_U30  PioWrite( &g_stPioB, ADC_EN2, PIO_CLEAR_OUT) //��ADC_EN2�����㣬ѡ��U30
//#define SELECT_U30    MA_OUTWM( PIO_CODR_B, ADC_EN2, ADC_EN2) //��ADC_EN2�����㣬ѡ��U30



#endif /* AD_H */