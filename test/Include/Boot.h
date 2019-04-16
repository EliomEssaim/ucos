#ifndef BootH
#define BootH

#include "UserType.h"

#define FPD_FILE_NAME_LEN  20
#define FPD_COUNT          3
#define TIME_STAMP_LEN     7
#define FILE_VERSION_LEN   20

#define FILE_OLD           0//表示文件为旧，对ucNew赋值
#define FILE_NEW           1//表示文件为新下载对ucNew赋值

//文件分区描述结构：file partition descriptor
typedef struct
{
    CHAR  acFileName[FPD_FILE_NAME_LEN];     //文件名
    UCHAR aucTimeStamp[TIME_STAMP_LEN];      //文件创建的时间
    CHAR  acVersion[FILE_VERSION_LEN];       //文件版本
    UCHAR ucFileType;                        //文件类型，程序文件或数据文件
    UCHAR ucStatus;                          //文件状态
    UCHAR ucNew;                             //文件状态相同时1表示新，0表示旧
    ULONG ulFileSize;                        //文件大小
    ULONG ulFileAddr;                        //文件存放的位置
    UCHAR aucFileId[20];                     //CMCC下载文件的ID
    UCHAR ucIsRunning;                       //是否运行标志，1:正在运行，0:未运行
}FILE_PART_DESC_ST;

#define PROGRAM_STATUS_INVALID      0 //表示程序无效
#define PROGRAM_STATUS_RUNNABLE     1 //表示程序可以成功运行
#define PROGRAM_STATUS_VALID        2 //表示程序有效，准备升级
#define PROGRAM_STATUS_UPGRADE      3 //表示程序正在升级但是还没成功
#define PROGRAM_STATUS_UNRUNNABLE   4 //表示程序升级失败

#define PROGRAM_PART_1_IDX          0 //分区下标
#define PROGRAM_PART_2_IDX          1
#define DATA_PART_1_IDX             2

#define PROGRAM_PART_1_ADDR  (0x1010000) //在FLASH中的地址
#define PROGRAM_PART_2_ADDR  (0x1090000)
#define FILE_PART_TBL_ADDR   (0x1183800)//文件分区在FLASH的地址，在CMCC_1_Ext.h中定义

extern FILE_PART_DESC_ST g_astFilePartTbl[FPD_COUNT];
//extern ULONG g_ulP1Idx; //用于指示两个程序分区
//extern ULONG g_ulP2Idx;
extern ULONG g_ulUpgradePartIdx; //下载文件存放的分区下标



#endif