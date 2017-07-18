#ifndef __RICE_MILL_FAULT_CODE_H__
#define __RICE_MILL_FAULT_CODE_H__



/****************************************************
*0x00000000 无故障
*Bit[0] 置1一号米仓空
*Bit[1] 置1二号米仓空
*Bit[2] 置1 一级仓阀门故障
*Bit[3] 置1 二级仓阀门故障
*Bit[4] 置1电子秤无响应
*Bit[5] 置1电子秤超量程
*Bit[6] 置1 电子秤数据错误
*Bit[7] 置1 主电机电流过载
*Bit[8] 置1 升降门上行超时
*Bit[9] 置1升降门下行超时
*Bit[10] 置1升降门电机堵转
*Bit[11] 置1分度调节器电机堵转
*Bit[12] 置1分度调节器运行超时
*Bit[13] 置1一号仓温度过高
*Bit[14] 置1一号仓湿度过高
*Bit[15] 置1二号仓温度过高
*Bit[16] 置1二号仓湿度过高
*******************************************************/
#define  ERROR_CODE_NO_ERROR                   0
#define  ERROR_CODE_RB1_1_EMPTY               (1<<0)
#define  ERROR_CODE_RB1_2_EMPTY               (1<<1)
#define  ERROR_CODE_RB1_SWITCH_FAULT          (1<<2)
#define  ERROR_CODE_RB2_SWITCH_FAULT          (1<<3)
#define  ERROR_CODE_EW_NO_RESPONSE            (1<<4)
#define  ERROR_CODE_EW_OVERLOAD               (1<<5)
#define  ERROR_CODE_EW_DATA_ILLEGAL           (1<<6)
#define  ERROR_CODE_MOTOR_OC_STALL            (1<<7)
#define  ERROR_CODE_OH_DOOR_UP_TIMEOUT        (1<<8)
#define  ERROR_CODE_OH_DOOR_DOWN_TIMEOUT      (1<<9)
#define  ERROR_CODE_OH_DOOR_MOTOR_STALL       (1<<10)
#define  ERROR_CODE_RL_MOTOR_STALL            (1<<11)
#define  ERROR_CODE_RL_RUN_TIMEOUT            (1<<12)
#define  ERROR_CODE_RB1_OT                    (1<<13)
#define  ERROR_CODE_RB1_OTH                   (1<<14)
#define  ERROR_CODE_RB2_OT                    (1<<15)
#define  ERROR_CODE_RB2_OTH                   (1<<16)














#endif