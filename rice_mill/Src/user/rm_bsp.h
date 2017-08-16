#ifndef  __RM_BSP_H__
#define  __RM_BSP_H__



#define  BSP_TRUE                          1
#define  BSP_FALSE                         0

#define  BSP_PWR_ON                        0
#define  BSP_PWR_DWN                       1
#define  BSP_PWR_ON_POSITIVE               2
#define  BSP_PWR_ON_NEGATIVE               3

#define  RL_DIR_NEGATIVE_PIN_STATE         GPIO_PIN_RESET                   
#define  RL_DIR_POSITIVE_PIN_STATE         GPIO_PIN_SET    


#define  BSP_RL_MOTOR_STEPS_FOR_RL0         100
#define  BSP_RL_MOTOR_STEPS_FOR_RL5         500
#define  BSP_RL_MOTOR_STEPS_FOR_RL7         700
#define  BSP_RL_MOTOR_STEPS_FOR_RL9         900



#define  BSP_RELAY_TIMEOUT_VALUE             20//继电器复位需要时间 单位ms


#define  BSP_ADC_CONVERT_NUM                 7

#define  BSP_ADC_T_POS                       0
#define  BSP_ADC_RH_POS                      1
#define  BSP_ADC_RM_MOTOR_POS                2                   
#define  BSP_ADC_AC2_POS                     3
#define  BSP_ADC_24V_POS                     4
#define  BSP_ADC_OH_DOOR_POS                 5
#define  BSP_ADC_BEMF_POS                    6








#endif