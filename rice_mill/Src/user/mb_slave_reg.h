#ifndef __MB_SLAVE_REG_H__
#define __MB_SLAVE_REG_H__


/* ----------------------- Defines ------------------------------------------*/
#define REG_HOLDING_START           0x1000
#define REG_HOLDING_NREGS           19
#define REG_INPUT_START             0x1100
#define REG_INPUT_NREGS             11

typedef  void (*ptr_regholding_write_handler_t)(void);
extern ptr_regholding_write_handler_t ptr_msg_handler[REG_HOLDING_NREGS];

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
#define  RL_CONTROL_REGHOLDING_ADDR          0x1012

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
#define  REG_VALUE_RB1_NO_1_ID               0x0001
#define  REG_VALUE_RB1_NO_2_ID               0x0002
#define  REG_VALUE_RW_MIN                         0
#define  REG_VALUE_RW_MAX                    0x05DC
#define  REG_VALUE_RL_0                      0x0000
#define  REG_VALUE_RL_5                      0x0005
#define  REG_VALUE_RL_7                      0x0007
#define  REG_VALUE_RL_9                      0x0009
#define  REG_VALUE_FUNC_ENABLE               0x0000
#define  REG_VALUE_FUNC_DISABLE              0xFFFF
/********输入寄存器的取值*************************/
#define  REG_VALUE_RM_PROGRESS_COMPLETED     0x00
#define  REG_VALUE_RM_PROGRESS_EXECUTING     0x01
#define  REG_VALUE_RM_PROGRESS_ERROR         0x02
/**************************************************/
typedef enum
{
 REGINPUT_MODE,
 REGHOLDING_MODE,
}reg_mode_t;


uint32_t get_reg_value(uint16_t reg_addr,uint16_t reg_size,reg_mode_t reg_mode);
void set_reg_value(uint16_t reg_addr,uint16_t reg_size,uint32_t value,reg_mode_t reg_mode);
void set_rm_fault_code(uint32_t err_code_bit);
uint32_t get_rm_fault_code();

#endif