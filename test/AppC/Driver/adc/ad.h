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

//以下三个管脚，负责外部3个模拟开关芯片（U6、U7、U30）的片选
#define ADC_SE  PB25  //PB口 控制3个模拟开关的A、B组输入的切换，高电平选择A组
#define ADC_EN1 PB26  //PB口 电路图中U6、U7的片选，低电平选中
#define ADC_EN2 PB27  //PB口 电路图中U30的片选，低电平选中

#define SELECT_A  PioWrite( &g_stPioB, ADC_SE, PIO_CLEAR_OUT) //将ADC_SE脚拉高，选择A组
//#define SELECT_A  PioWrite( PIO_SODR_B, ADC_SE, ADC_SE) //将ADC_SE脚拉高，选择A组
#define SELECT_B  PioWrite( &g_stPioB, ADC_SE, PIO_SET_OUT) //将ADC_SE脚清零，选择B组
//#define SELECT_B  MA_OUTWM( PIO_CODR_B, ADC_SE, ADC_SE) //将ADC_SE脚清零，选择B组

#define DISSELECT_U6U7  PioWrite( &g_stPioB, ADC_EN1, PIO_SET_OUT) //将ADC_EN1脚拉高，不选中U6、U7
//#define DISSELECT_U6U7 MA_OUTWM( PIO_SODR_B, ADC_EN1, ADC_EN1) //将ADC_EN1脚拉高，不选中U6、U7
#define SELECT_U6U7  PioWrite( &g_stPioB, ADC_EN1, PIO_CLEAR_OUT) //将ADC_EN1脚清零，选中U6、U7
//#define SELECT_U6U7    MA_OUTWM( PIO_CODR_B, ADC_EN1, ADC_EN1) //将ADC_EN1脚清零，选中U6、U7

#define DISSELECT_U30  PioWrite( &g_stPioB, ADC_EN2, PIO_SET_OUT) //将ADC_EN2脚拉高，不选中U30
//#define DISSELECT_U30 MA_OUTWM( PIO_SODR_B, ADC_EN2, ADC_EN2) //将ADC_EN2脚拉高，不选中U30
#define SELECT_U30  PioWrite( &g_stPioB, ADC_EN2, PIO_CLEAR_OUT) //将ADC_EN2脚清零，选中U30
//#define SELECT_U30    MA_OUTWM( PIO_CODR_B, ADC_EN2, ADC_EN2) //将ADC_EN2脚清零，选中U30



#endif /* AD_H */