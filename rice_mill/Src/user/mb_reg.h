#ifndef __MB_SLAVE_REG_H__
#define __MB_SLAVE_REG_H__

/********保持寄存器（读写寄存器）*****************/
#define  RM_SWITCH_REGHOLDING_ADDR           0x1000
#define  RB1_SELECTION_REGHOLDING_ADDR       0x1001
#define  RW_REGHOLDING_ADDR                  0x1002
#define  RL_REGHOLDING_ADDR                  0x1004
#define  RM_FAULT_CODE_REGHOLDING_ADDR       0x1005
#define  RM_MOTOR_SWITCH_REGHOLDING_ADDR     0x1007
#define  RB1_1_SWITCH_REGHOLDING_ADDR        0x1008
#define  RB1_2_SWITCH_REGHOLDING_ADDR        0x1009
#define  RB2_SWITCH_REGHOLDING_ADDR          0x100A
#define  UV_LAMP_SWITCH_REGHOLDING_ADDR      0x100B
#define  E_LAMP_SWITCH_REGHOLDING_ADDR       0x100C
#define  OH_DOOR_SWITCH_REGHOLDING_ADDR      0x100D
#define  R_TARE_REGHOLDING_ADDR              0x100E         //0x100e      去皮
#define  Z_CLEARING_REGHOLDING_ADDR          0x100F         //0x100f      清零
#define  EW_THRESHOLD_REGHOLDING_ADDR        0x1010         //0x1010      重量定点值

/********输入寄存器（只读寄存器）*****************/
#define  FIREMWARE_VERSION_REGINPUT_ADDR     0x1100
#define  RM_DEVICE_ID_REGINPUT_ADDR          0x1102
#define  RM_EXECUTE_PROGRESS_REGINPUT_ADDR   0x1104
#define  RB1_1_TEMPERATURE_REGINPUT_ADDR     0x1105
#define  RB1_1_RH_REGINPUT_ADDR              0x1106
#define  RB1_2_TEMPERATURE_REGINPUT_ADDR     0x1107
#define  RB1_2_RH_REGINPUT_ADDR              0x1108
#define  EW_GROSS_WEIGHT_REGINPUT_ADDR       0x1109
#define  EW_NET_WEIGHT_REGINPUT_ADDR         0x110A


/********保持寄存器的取值*************************/
#define  REG_VALUE_SWITCH_ON                 0x0000
#define  REG_VALUE_SWITCH_OFF                0xFFFF
#define  RB1_NO_1_ID                         0x0001
#define  RB1_NO_2_ID                         0x0002
#define  RW_VALUE_MIN                        0
#define  RW_VALUE_MAX                        0x05DC
#define  RL_VALUE_0                          0x0000
#define  RL_VALUE_5                          0x0005
#define  RL_VALUE_7                          0x0007
#define  RL_VALUE_9                          0x0009
#define  REG_VALUE_FUNC_ENABLE               0x0000
#define  REG_VALUE_FUNC_DISABLE              0xFFFF

/********碾米机操作命令***************************/
#define  CMD_TURN_ON_RM                      0x0000
#define  CMD_TURN_OFF_RM                     0x0001
#define  CMD_RB1_SELECT_NO1                  0x0002
#define  CMD_RB1_SELECT_NO2                  0x0003
#define  CMD_SET_RW_VALUE                    0x0004
#deifne  CMD_SET_RL_0                        0x0005         
#deifne  CMD_SET_RL_5                        0x0006 
#deifne  CMD_SET_RL_7                        0x0007 
#deifne  CMD_SET_RL_9                        0x0008
#define  CMD_SET_RM_FAULT_CODE               0x0009
#define  CMD_TURN_ON_RM_MOTOR                0x000A
#define  CMD_TURN_OFF_RM_MOTOR               0x000B
#define  CMD_TURN_ON_RB1_1_SWITCH            0x000C
#define  CMD_TURN_OFF_RB1_1_SWITCH           0x000D
#define  CMD_TURN_ON_RB1_2_SWITCH            0x000E
#define  CMD_TURN_OFF_RB1_2_SWITCH           0x000F
#define  CMD_TURN_ON_RB2_SWITCH              0x0010
#define  CMD_TURN_OFF_RB2_SWITCH             0x0011
#define  CMD_TURN_ON_UV_LAMP                 0x0012
#define  CMD_TURN_OFF_UV_LAMP                0x0013
#define  CMD_TURN_ON_E_LAMP                  0x0014
#define  CMD_TURN_OFF_E_LAMP                 0x0015
#define  CMD_TURN_ON_OH_DOOR                 0x0016
#define  CMD_TURN_OFF_OH_DOOR                0x0017
#define  CMD_ENABLE_REMOVE_TARE              0x0018
#define  CMD_DISABLE_REMOVE_TARE             0x0019
#define  CMD_ENABLE_ZERO_CLEARING            0x0018
#define  CMD_DISABLE_ZERO_CLEARING           0x0019
#define  CMD_SET_EW_THRESHOLD_VALUE          0x001A




#endif