#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "tim.h"
#include "rm_bsp.h"


volatile uint16_t bsp_adc_result[BSP_ADC_CONVERT_NUM];
volatile uint16_t bsp_motor_cur_steps;
volatile uint16_t bsp_motor_tar_steps;
volatile uint8_t  bsp_motor_pwr_state;
extern TIM_HandleTypeDef htim8;
extern TIM_HandleTypeDef htim2;




uint8_t BSP_is_rb1_1_empty()
{
 GPIO_PinState pinstate;
 uint8_t ret;
 
 pinstate= HAL_GPIO_ReadPin(GPIOG,RB1_1_EMPTY_POS_Pin);
 if( pinstate == EMPTY_PIN_STATE)
   ret= BSP_TRUE;
 else
   ret=BSP_FALSE;
 
 return ret; 
}
uint8_t BSP_is_rb1_2_empty()
{
 GPIO_PinState pinstate;
 uint8_t ret;
 
 pinstate= HAL_GPIO_ReadPin(GPIOG,RB1_2_EMPTY_POS_Pin);
 if( pinstate == EMPTY_PIN_STATE)
   ret= BSP_TRUE;
 else
   ret=BSP_FALSE;
 
 return ret;  
}
uint8_t BSP_is_rb1_no1_turn_on()
{
 GPIO_PinState pinstate;
 uint8_t ret;
 
 pinstate= HAL_GPIO_ReadPin(GPIOG,RB1_1_TURN_ON_POS_Pin);
 if( pinstate == ON_POS_PIN_STATE)
   ret= BSP_TRUE;
 else
   ret=BSP_FALSE;
 
 return ret;  
}

uint8_t BSP_is_rb1_no2_turn_on()
{
 GPIO_PinState pinstate;
 uint8_t ret;
 
 pinstate= HAL_GPIO_ReadPin(GPIOG,RB1_2_TURN_ON_POS_Pin);
 if( pinstate == ON_POS_PIN_STATE)
   ret= BSP_TRUE;
 else
   ret=BSP_FALSE;
 
 return ret;   
}
uint8_t BSP_is_rb1_turn_off()
{
 GPIO_PinState pinstate;
 uint8_t ret;
 
 pinstate= HAL_GPIO_ReadPin(GPIOG,RB1_TURN_OFF_POS_Pin);
 if( pinstate == ON_POS_PIN_STATE)
   ret= BSP_TRUE;
 else
   ret=BSP_FALSE;
 
 return ret;  
}

uint8_t BSP_is_rb2_turn_off()
{
 GPIO_PinState pinstate;
 uint8_t ret;
 
 pinstate= HAL_GPIO_ReadPin(GPIOG,RB2_TURN_OFF_POS_Pin);
 if( pinstate == ON_POS_PIN_STATE)
   ret= BSP_TRUE;
 else
   ret=BSP_FALSE;
 
 return ret;  
}

uint8_t BSP_is_oh_door_turn_on()
{
 GPIO_PinState pinstate;
 uint8_t ret;
 
 pinstate= HAL_GPIO_ReadPin(GPIOG,OH_DOOR_TURN_ON_POS_Pin);
 if( pinstate == ON_POS_PIN_STATE)
   ret= BSP_TRUE;
 else
   ret=BSP_FALSE;
 
 return ret; 
}
uint8_t BSP_is_oh_door_turn_off()
{
 GPIO_PinState pinstate;
 uint8_t ret;
 
 pinstate= HAL_GPIO_ReadPin(GPIOG,OH_DOOR_TURN_OFF_POS_Pin);
 if( pinstate == ON_POS_PIN_STATE)
   ret= BSP_TRUE;
 else
   ret=BSP_FALSE;
 
 return ret; 
}

uint8_t BSP_is_oh_door_pathway_ok()
{
 GPIO_PinState pinstate;
 uint8_t ret;
 
 pinstate= HAL_GPIO_ReadPin(GPIOG,OH_DOOR_CLEAR_POS_Pin);
 if( pinstate == CLEAR_PIN_STATE)
   ret= BSP_TRUE;
 else
   ret=BSP_FALSE;
 
 return ret;  
}

uint8_t BSP_is_ew_signal_ok()
{
 GPIO_PinState pinstate;
 uint8_t ret;
 
 pinstate= HAL_GPIO_ReadPin(E_WEIGHT_SIGNAL_POS_GPIO_Port,E_WEIGHT_SIGNAL_POS_Pin);
 if( pinstate == SIGNAL_OK_PIN_STATE)
   ret= BSP_TRUE;
 else
   ret=BSP_FALSE;
 
 return ret;   
}
/*
 *步进电机操作
 */
uint8_t BSP_is_rl_in_rst_pos()
{
 GPIO_PinState pinstate;
 uint8_t ret;
 
 pinstate= HAL_GPIO_ReadPin(GPIOG,RL_RESET_POS_Pin);
 if( pinstate == ON_POS_PIN_STATE)
   ret= BSP_TRUE;
 else
   ret=BSP_FALSE;
 
 return ret; 
}

uint16_t BSP_rl_get_motor_cur_steps()
{
 return bsp_motor_cur_steps; 
}
void BSP_rl_motor_steps_increase()
{
  bsp_motor_cur_steps++;
}
void BSP_rl_motor_steps_decrease()
{
 bsp_motor_cur_steps--;
}
uint16_t BSP_rl_get_motor_tar_steps()
{
 return bsp_motor_tar_steps;  
}
void BSP_rl_reset_steps()
{
 bsp_motor_cur_steps=BSP_RL_MOTOR_STEPS_FOR_RL0; 
}
void BSP_rl_set_pwr_state(uint8_t pwr_state)
{
 if(pwr_state == BSP_PWR_ON_NEGATIVE || pwr_state == BSP_PWR_ON_POSITIVE)
 bsp_motor_pwr_state=pwr_state;
}
uint8_t BSP_rl_get_pwr_state()
{
  return bsp_motor_pwr_state;
}

uint8_t BSP_is_rl_in_tar_pos()
{
  uint8_t ret;
 if(BSP_rl_get_motor_tar_steps()==BSP_rl_get_motor_cur_steps())
 ret=BSP_TRUE;
 else
 ret=BSP_FALSE;
  
return ret; 
}
void BSP_rl_go_to_pos(uint16_t tar_steps)
{ 
  uint16_t cur_steps;
  HAL_TIM_PWM_Stop_IT(&htim8, TIM_CHANNEL_1);
  if(tar_steps == BSP_RL_MOTOR_STEPS_FOR_RL0 ||
     tar_steps == BSP_RL_MOTOR_STEPS_FOR_RL5 ||
     tar_steps == BSP_RL_MOTOR_STEPS_FOR_RL7 ||
     tar_steps == BSP_RL_MOTOR_STEPS_FOR_RL9 )
  {
 bsp_motor_tar_steps=tar_steps;
 cur_steps=BSP_rl_get_motor_cur_steps(); 
 if(cur_steps < tar_steps)//正转
 {
 BSP_rl_set_pwr_state(BSP_PWR_ON_POSITIVE);
//向目标点运行，到位后发送到位信号
 HAL_GPIO_WritePin(DIR_8711_POS_GPIO_Port, DIR_8711_POS_Pin, RL_DIR_POSITIVE_PIN_STATE);
 HAL_TIM_PWM_Start_IT(&htim8, TIM_CHANNEL_1);
 } 
 else if(cur_steps > tar_steps)//反转
 {
 BSP_rl_set_pwr_state(BSP_PWR_ON_NEGATIVE);
 HAL_GPIO_WritePin(DIR_8711_POS_GPIO_Port, DIR_8711_POS_Pin, RL_DIR_NEGATIVE_PIN_STATE);
 HAL_TIM_PWM_Start_IT(&htim8, TIM_CHANNEL_1);
 }
  }
}
/*
 *步进电机中断处理
 */
void BSP_RL_MOTOR_PWM_ISR()
{
 uint8_t pwr_state;
 uint16_t tar_steps,cur_steps;
 pwr_state= BSP_rl_get_pwr_state();
 tar_steps=BSP_rl_get_motor_tar_steps();
 cur_steps=BSP_rl_get_motor_cur_steps();
 if(tar_steps !=cur_steps)
 {
  if(pwr_state == BSP_PWR_ON_POSITIVE)
   BSP_rl_motor_steps_increase();
  if(pwr_state == BSP_PWR_ON_NEGATIVE)
   BSP_rl_motor_steps_decrease();
 }
 else
 {
  HAL_TIM_PWM_Stop_IT(&htim8, TIM_CHANNEL_1);
  
 }
}



void BSP_rb1_motor_pwr(uint8_t pwr_state)
{
  if(pwr_state==BSP_PWR_ON_POSITIVE)
  {
  HAL_GPIO_WritePin(RB1_PWR_ON_NEGATIVE_POS_GPIO_Port, RB1_PWR_ON_NEGATIVE_POS_Pin, PWR_DWN_PIN_STATE);
  osDelay(BSP_RELAY_TIMEOUT_VALUE);//等待继电器复位
  HAL_GPIO_WritePin(RB1_PWR_ON_POSITIVE_POS_GPIO_Port, RB1_PWR_ON_POSITIVE_POS_Pin, PWR_ON_POSITIVE_PIN_STATE); 
  }
  else if(pwr_state==BSP_PWR_ON_NEGATIVE)
  {
  HAL_GPIO_WritePin(RB1_PWR_ON_POSITIVE_POS_GPIO_Port ,RB1_PWR_ON_POSITIVE_POS_Pin , PWR_DWN_PIN_STATE);
  osDelay(BSP_RELAY_TIMEOUT_VALUE);//等待继电器复位
  HAL_GPIO_WritePin(RB1_PWR_ON_NEGATIVE_POS_GPIO_Port,RB1_PWR_ON_NEGATIVE_POS_Pin , PWR_ON_NEGATIVE_PIN_STATE); 
  }
  else if(pwr_state==BSP_PWR_DWN)
  {
  HAL_GPIO_WritePin(RB1_PWR_ON_POSITIVE_POS_GPIO_Port ,RB1_PWR_ON_POSITIVE_POS_Pin , PWR_DWN_PIN_STATE);
  HAL_GPIO_WritePin(RB1_PWR_ON_NEGATIVE_POS_GPIO_Port,RB1_PWR_ON_NEGATIVE_POS_Pin , PWR_DWN_PIN_STATE);   
  }
  
}
void BSP_rb2_motor_pwr(uint8_t pwr_state)
{
  if(pwr_state==BSP_PWR_ON_POSITIVE)
  {
  HAL_GPIO_WritePin(RB2_PWR_ON_NEGATIVE_POS_GPIO_Port, RB2_PWR_ON_NEGATIVE_POS_Pin, PWR_DWN_PIN_STATE);
  osDelay(BSP_RELAY_TIMEOUT_VALUE);//等待继电器复位
  HAL_GPIO_WritePin(RB2_PWR_ON_POSITIVE_POS_GPIO_Port, RB2_PWR_ON_POSITIVE_POS_Pin, PWR_ON_POSITIVE_PIN_STATE);   
  }
  else if(pwr_state==BSP_PWR_ON_NEGATIVE)
  {
  HAL_GPIO_WritePin(RB2_PWR_ON_POSITIVE_POS_GPIO_Port ,RB2_PWR_ON_POSITIVE_POS_Pin , PWR_DWN_PIN_STATE);
  osDelay(BSP_RELAY_TIMEOUT_VALUE);//等待继电器复位
  HAL_GPIO_WritePin(RB2_PWR_ON_NEGATIVE_POS_GPIO_Port,RB2_PWR_ON_NEGATIVE_POS_Pin , PWR_ON_NEGATIVE_PIN_STATE);   
  }
  else if(pwr_state==BSP_PWR_DWN)
  {
  HAL_GPIO_WritePin(RB2_PWR_ON_POSITIVE_POS_GPIO_Port ,RB2_PWR_ON_POSITIVE_POS_Pin , PWR_DWN_PIN_STATE);
  HAL_GPIO_WritePin(RB2_PWR_ON_NEGATIVE_POS_GPIO_Port,RB2_PWR_ON_NEGATIVE_POS_Pin , PWR_DWN_PIN_STATE);     
  }
  
}
void BSP_uv_lump_pwr(uint8_t pwr_state)
{
 if(pwr_state==BSP_PWR_ON)
 {
 HAL_GPIO_WritePin(UV_LAMP_PWR_POS_GPIO_Port ,UV_LAMP_PWR_POS_Pin , PWR_ON_PIN_STATE); 
 }
 else if(pwr_state==BSP_PWR_DWN)
 {
 HAL_GPIO_WritePin(UV_LAMP_PWR_POS_GPIO_Port ,UV_LAMP_PWR_POS_Pin , PWR_DWN_PIN_STATE);  
 }
}

void BSP_adv_lump_pwr(uint8_t pwr_state)
{
 if(pwr_state==BSP_PWR_ON)
 {
 HAL_GPIO_WritePin(E_LAMP_PWR_POS_GPIO_Port ,E_LAMP_PWR_POS_Pin , PWR_ON_PIN_STATE); 
 }
 else if(pwr_state==BSP_PWR_DWN)
 {
 HAL_GPIO_WritePin(E_LAMP_PWR_POS_GPIO_Port ,E_LAMP_PWR_POS_Pin , PWR_DWN_PIN_STATE);  
 }
}

void BSP_oh_door_motor_pwr( uint8_t pwr_state)
{
  if(pwr_state==BSP_PWR_ON_POSITIVE)
  {
  HAL_GPIO_WritePin(OH_DOOR_PWR_ON_NEGATIVE_POS_GPIO_Port, OH_DOOR_PWR_ON_NEGATIVE_POS_Pin, PWR_DWN_PIN_STATE);
  osDelay(BSP_RELAY_TIMEOUT_VALUE);//等待继电器复位
  HAL_GPIO_WritePin(OH_DOOR_PWR_ON_POSITIVE_POS_GPIO_Port, OH_DOOR_PWR_ON_POSITIVE_POS_Pin, PWR_ON_POSITIVE_PIN_STATE); 
  }
  else if(pwr_state==BSP_PWR_ON_NEGATIVE)
  {
  HAL_GPIO_WritePin(OH_DOOR_PWR_ON_POSITIVE_POS_GPIO_Port ,OH_DOOR_PWR_ON_POSITIVE_POS_Pin , PWR_DWN_PIN_STATE);
  osDelay(BSP_RELAY_TIMEOUT_VALUE);//等待继电器复位
  HAL_GPIO_WritePin(OH_DOOR_PWR_ON_NEGATIVE_POS_GPIO_Port,OH_DOOR_PWR_ON_NEGATIVE_POS_Pin , PWR_ON_NEGATIVE_PIN_STATE); 
  }
  else if(pwr_state==BSP_PWR_DWN)
  {
  HAL_GPIO_WritePin(OH_DOOR_PWR_ON_POSITIVE_POS_GPIO_Port ,OH_DOOR_PWR_ON_POSITIVE_POS_Pin , PWR_DWN_PIN_STATE);
  HAL_GPIO_WritePin(OH_DOOR_PWR_ON_NEGATIVE_POS_GPIO_Port,OH_DOOR_PWR_ON_NEGATIVE_POS_Pin , PWR_DWN_PIN_STATE);   
  }
}

void BSP_rm_motor_pwr(uint8_t pwr_state)
{
 if(pwr_state==BSP_PWR_ON)
 {
 HAL_GPIO_WritePin(RM_MOTOR_PWR_POS_GPIO_Port ,RM_MOTOR_PWR_POS_Pin , PWR_ON_PIN_STATE);//碾米电机
 HAL_GPIO_WritePin(RICE_FAN_PWR_POS_GPIO_Port ,RICE_FAN_PWR_POS_Pin , PWR_ON_PIN_STATE);//碾米风扇
 }
 else if(pwr_state==BSP_PWR_DWN)
 {
 HAL_GPIO_WritePin(RM_MOTOR_PWR_POS_GPIO_Port ,RM_MOTOR_PWR_POS_Pin , PWR_DWN_PIN_STATE);
 HAL_GPIO_WritePin(RICE_FAN_PWR_POS_GPIO_Port ,RICE_FAN_PWR_POS_Pin , PWR_DWN_PIN_STATE);
 }
}


void BSP_ac_fan1_pwr(uint8_t pwr_state)
{
 if(pwr_state==BSP_PWR_ON)
 {
 HAL_GPIO_WritePin(AC_FAN1_PWR_POS_GPIO_Port ,AC_FAN1_PWR_POS_Pin , PWR_ON_PIN_STATE); 
 }
 else if(pwr_state==BSP_PWR_DWN)
 {
 HAL_GPIO_WritePin(AC_FAN1_PWR_POS_GPIO_Port ,AC_FAN1_PWR_POS_Pin , PWR_DWN_PIN_STATE);  
 }
}

void BSP_ac_fan2_pwr(uint8_t pwr_state)
{
 if(pwr_state==BSP_PWR_ON)
 {
 HAL_GPIO_WritePin(AC_FAN2_PWR_POS_GPIO_Port ,AC_FAN2_PWR_POS_Pin , PWR_ON_PIN_STATE); 
 }
 else if(pwr_state==BSP_PWR_DWN)
 {
 HAL_GPIO_WritePin(AC_FAN2_PWR_POS_GPIO_Port ,AC_FAN2_PWR_POS_Pin , PWR_DWN_PIN_STATE);  
 }
}

void BSP_bl_pwm_start(uint16_t pulse)
{
  TIM_OC_InitTypeDef  sConfigOC;
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = pulse;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  
  HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start_IT(&htim2,TIM_CHANNEL_2);
}
void BSP_bl_pwm_stop()
{
  HAL_TIM_PWM_Start_IT(&htim2,TIM_CHANNEL_2);
}

uint16_t BSP_get_temperature()
{
 uint16_t adc,t;
 adc=bsp_adc_result[BSP_ADC_T_POS];
 t=VOLTAGE_REF*T_PER_VOLTAGE*adc/0x0fff;
 return t;
}

uint16_t BSP_get_relative_humidity()
{
 uint16_t adc,rh;
 adc=bsp_adc_result[BSP_ADC_RH_POS];
 rh=VOLTAGE_REF*RH_PER_VOLTAGE*adc/0x0fff;
 return rh;
}

uint8_t BSP_is_24v_oc()
{
 uint8_t ret;
 uint16_t adc,v24;
 adc=bsp_adc_result[BSP_ADC_24V_POS];
 v24=VOLTAGE_REF*adc/0x0fff;
 if(v24 > ADC_RESULT_24V_MAX)
 ret=BSP_TRUE;
 else
 ret=BSP_FALSE;
 
 return ret;
}
uint8_t BSP_is_oh_door_oc()
{
 uint8_t ret;
 uint16_t adc,oh_door;
 adc=bsp_adc_result[BSP_ADC_OH_DOOR_POS];
 oh_door=VOLTAGE_REF*adc/0x0fff;
 if(oh_door > ADC_RESULT_OH_DOOR_MAX)
 ret= BSP_TRUE;
 else
 ret= BSP_FALSE;
 
 return ret;
}
uint8_t BSP_is_rm_motor_oc()
{
 uint8_t ret;
 uint16_t adc,rm_motor;
 adc=bsp_adc_result[BSP_ADC_RM_MOTOR_POS];
 rm_motor=VOLTAGE_REF*adc/0x0fff;
 if(rm_motor > ADC_RESULT_RM_MOTOR_MAX)
 ret=BSP_TRUE;
 else
 ret=BSP_FALSE;
 return ret;
}
uint8_t BSP_is_bemf_oc()
{
 uint8_t ret;
 uint16_t adc,bemf;
 adc=bsp_adc_result[BSP_ADC_RM_MOTOR_POS];
 bemf=VOLTAGE_REF*adc/0x0fff;
 if(bemf > ADC_RESULT_BEMF_MAX)
 ret=BSP_TRUE;
 else
 ret=BSP_FALSE;
 return ret;
}


/* ADC1 channel config func */
/*
extern ADC_HandleTypeDef hadc1;  
void BSP_config_adc_channel(uint32_t channel)
{
  ADC_ChannelConfTypeDef sConfig;

  sConfig.Channel = channel;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}
*/



