#ifndef __MB_SLAVE_EVENT_H__
#define __MB_SLAVE_EVENT_H__


/*********************************************************
 *               碾米机操作消息
 *********************************************************/
#define  MSG_START_RM                        0x0000
#define  MSG_STOP_RM                         0x0001
#define  MSG_RB1_SELECT_NO1                  0x0002
#define  MSG_RB1_SELECT_NO2                  0x0003
#define  MSG_SET_RW_VALUE                    0x0004
#define  MSG_SET_RL_0                        0x0005         
#define  MSG_SET_RL_5                        0x0006 
#define  MSG_SET_RL_7                        0x0007 
#define  MSG_SET_RL_9                        0x0008
#define  MSG_SET_RM_FAULT_CODE               0x0009
#define  MSG_PWR_ON_RM_MOTOR                 0x000A
#define  MSG_PWR_DWN_RM_MOTOR                0x000B
#define  MSG_TURN_ON_RB1_1_SWITCH            0x000C
#define  MSG_TURN_OFF_RB1_1_SWITCH           0x000D
#define  MSG_TURN_ON_RB1_2_SWITCH            0x000E
#define  MSG_TURN_OFF_RB1_2_SWITCH           0x000F
#define  MSG_PWR_DWN_RB1                     0x0010
#define  MSG_TURN_ON_RB2_SWITCH              0x0011
#define  MSG_TURN_OFF_RB2_SWITCH             0x0012
#define  MSG_PWR_DWN_RB2_SWITCH              0x0013
#define  MSG_PWR_ON_UV_LAMP                  0x0014
#define  MSG_PWR_DWN_UV_LAMP                 0x0015
#define  MSG_TURN_ON_E_LAMP                  0x0016
#define  MSG_PWR_ON_E_LAMP                   0x0017
#define  MSG_TURN_ON_OH_DOOR                 0x0018
#define  MSG_TURN_OFF_OH_DOOR                0x0019
#define  MSG_PWR_DWN_OH_DOOR                 0x001A
#define  MSG_ENABLE_REMOVE_TARE              0x001B
#define  MSG_DISABLE_REMOVE_TARE             0x001C
#define  MSG_ENABLE_ZERO_CLEARING            0x001D
#define  MSG_DISABLE_ZERO_CLEARING           0x001E
#define  MSG_SET_EW_THRESHOLD_VALUE          0x001F
#define  MSG_SETUP_RL_VALUE                  0x0020
#define  MSG_PWR_ON_AC1                      0x0021                  
#define  MSG_PWR_DWN_AC1                     0x0022
#define  MSG_PWR_ON_AC2                      0x0023
#define  MSG_PWR_DWN_AC2                     0x0024






#endif