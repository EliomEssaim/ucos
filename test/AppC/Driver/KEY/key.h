/*---------------------------------------------------------------------------
    版权 2006 - , 福建邮科通信技术有限公司
    文件名:   Key.h
    作者:     林玮
    文件说明: 
    其它:
    函数列表:

    历史记录:
    1. 日期:  
       作者:  林玮
       描述:  代码编写完成
---------------------------------------------------------------------------*/
#ifndef KeyH
#define KeyH


/*****************************************/

#define KEY_BASE ((UCHAR *)0x04000000)
#define KEY_MASK 0x0F

//扫描到的按键状态
#define KEY_STATUS_SINGLE      0x01
#define KEY_STATUS_COMPOUNDING 0x02
#define KEY_STATUS_LONGPRESS   0x03

//单一键值定义
#define KEY_VALUE_OK     0x0800 //功能键“确定”
#define KEY_VALUE_CANCEL 0x0008 //功能键“返回”
#define KEY_VALUE_UP     0x8000 //功能键“△”
#define KEY_VALUE_DOWN   0x0080 //功能键“▽”
#define KEY_VALUE_LEFT   0x0001 //功能键“ <”
#define KEY_VALUE_RIGHT  0x0004 //功能键“ >”

#define KEY_VALUE_0  0x0002 //数字“0”
#define KEY_VALUE_1  0x1000 //数字“1”
#define KEY_VALUE_2  0x2000 //数字“2”
#define KEY_VALUE_3  0x4000 //数字“3”
#define KEY_VALUE_4  0x0100 //数字“4”
#define KEY_VALUE_5  0x0200 //数字“5”
#define KEY_VALUE_6  0x0400 //数字“6”
#define KEY_VALUE_7  0x0010 //数字“7”
#define KEY_VALUE_8  0x0020 //数字“8”
#define KEY_VALUE_9  0x0040 //数字“9”

//组合键值定义
#define KEY_VALUE_A  KEY_VALUE_LEFT+KEY_VALUE_1 //英文“A”
#define KEY_VALUE_B  KEY_VALUE_LEFT+KEY_VALUE_2 //英文“B”
#define KEY_VALUE_C  KEY_VALUE_LEFT+KEY_VALUE_3 //英文“C”
#define KEY_VALUE_D  KEY_VALUE_LEFT+KEY_VALUE_4 //英文“D”
#define KEY_VALUE_E  KEY_VALUE_LEFT+KEY_VALUE_5 //英文“E”
#define KEY_VALUE_F  KEY_VALUE_LEFT+KEY_VALUE_6 //英文“F”

#define KEY_VALUE_STA_INIT         KEY_VALUE_LEFT+KEY_VALUE_7 //开站上报
#define KEY_VALUE_INSPECTION       KEY_VALUE_LEFT+KEY_VALUE_8 //巡检上报
#define KEY_VALUE_REPAIR_CONFIRMED KEY_VALUE_LEFT+KEY_VALUE_9 //修复上报
#define KEY_VALUE_CONFIG_CHANGED   KEY_VALUE_LEFT+KEY_VALUE_0 //配置变更上报

#define KEY_VALUE_OLED_OPEN  KEY_VALUE_LEFT+KEY_VALUE_RIGHT //显示器点亮


#endif


