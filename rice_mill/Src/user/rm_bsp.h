#ifndef  __RM_BSP_H__
#define  __RM_BSP_H__



#define  BSP_TRUE                          1
#define  BSP_FALSE                         0

#define  BSP_PWR_ON                        0
#define  BSP_PWR_DWN                       1
#define  BSP_PWR_ON_POSITIVE               2
#define  BSP_PWR_ON_NEGATIVE               3

#define  VOLTAGE_REF                       33 //3.3v
#define  T_PER_VOLTAGE                     1  //传感器 摄氏度/V
#define  RH_PER_VOLTAGE                    1  //传感器 相对湿度/V



#define  EMPTY_PIN_STATE                   GPIO_PIN_RESET 
#define  ON_POS_PIN_STATE                  GPIO_PIN_RESET
#define  CLEAR_PIN_STATE                   GPIO_PIN_RESET
#define  SIGNAL_OK_PIN_STATE               GPIO_PIN_SET
#define  PWR_DWN_PIN_STATE                 GPIO_PIN_RESET
#define  PWR_ON_POSITIVE_PIN_STATE         GPIO_PIN_SET
#define  PWR_ON_NEGATIVE_PIN_STATE         GPIO_PIN_SET
#define  PWR_ON_PIN_STATE                  GPIO_PIN_SET


#define  RL_DIR_NEGATIVE_PIN_STATE         GPIO_PIN_RESET                   
#define  RL_DIR_POSITIVE_PIN_STATE         GPIO_PIN_SET    


#define  BSP_RL_MOTOR_STEPS_FOR_RL0         100
#define  BSP_RL_MOTOR_STEPS_FOR_RL5         500
#define  BSP_RL_MOTOR_STEPS_FOR_RL7         700
#define  BSP_RL_MOTOR_STEPS_FOR_RL9         900
#define  BSP_RL_MOTOR_STEPS_FOR_STOP        0


#define  BSP_RELAY_TIMEOUT_VALUE             20//继电器复位需要时间 单位ms

#define  ADC_RESULT_24V_MAX                  0xff
#define  ADC_RESULT_OH_DOOR_MAX              0xff
#define  ADC_RESULT_RM_MOTOR_MAX             0xff  
#define  ADC_RESULT_BEMF_MAX                 0xff

#define  BSP_ADC_CONVERT_NUM                 7

#define  BSP_ADC_T_POS                       0
#define  BSP_ADC_RH_POS                      1
#define  BSP_ADC_RM_MOTOR_POS                2                   
#define  BSP_ADC_AC2_POS                     3
#define  BSP_ADC_24V_POS                     4
#define  BSP_ADC_OH_DOOR_POS                 5
#define  BSP_ADC_BEMF_POS                    6

/*
 * 声明
 */
uint8_t BSP_is_rb1_1_empty();
uint8_t BSP_is_rb1_2_empty();
uint8_t BSP_is_rb1_no1_turn_on();
uint8_t BSP_is_rb1_no2_turn_on();
uint8_t BSP_is_rb1_turn_off();

uint8_t BSP_is_rb2_turn_off();

uint8_t BSP_is_oh_door_turn_on();
uint8_t BSP_is_oh_door_turn_off();
uint8_t BSP_is_oh_door_cleared();
uint8_t BSP_is_ew_signal_ok();
uint8_t BSP_is_rl_in_rst_pos();
uint16_t BSP_rl_get_motor_cur_steps();
uint16_t BSP_rl_get_motor_tar_steps();
void BSP_rl_reset_steps();
void BSP_rl_go_to_pos(uint16_t tar_steps);
void BSP_rb1_motor_pwr(uint8_t pwr_state);
void BSP_rb2_motor_pwr(uint8_t pwr_state);
void BSP_uv_lump_pwr(uint8_t pwr_state);
void BSP_adv_lump_pwr(uint8_t pwr_state);//广告屏
void BSP_oh_door_motor_pwr( uint8_t pwr_state);
void BSP_rm_motor_pwr(uint8_t pwr_state);
void BSP_ac_fan1_pwr(uint8_t pwr_state);
void BSP_ac_fan2_pwr(uint8_t pwr_state);
void BSP_bl_pwm_start(uint16_t pulse);//呼吸灯
void BSP_bl_pwm_stop();
uint16_t BSP_get_temperature();
uint16_t BSP_get_relative_humidity();
uint8_t BSP_is_24v_oc();
uint8_t BSP_is_oh_door_oc();
uint8_t BSP_is_rm_motor_oc();
uint8_t BSP_is_bemf_oc();






#endif