#include "rm_bsp.h"

uint8_t BSP_is_rb1_1_empty()
{
  
}
uint8_t BSP_is_rb1_2_empty()
{
  
}
uint8_t BSP_is_rb1_no1_turn_on()
{
  
}
uint8_t BSP_is_rb1_no1_turn_off()
{
  
}
uint8_t BSP_is_rb1_block()
{
  
}

uint8_t BSP_is_rb1_no2_turn_on()
{
  
}
uint8_t BSP_is_rb1_no2_turn_off()
{
  
}
uint8_t BSP_is_rb2_block()
{
  
}

uint8_t BSP_is_oh_door_turn_on()
{
  
}
uint8_t BSP_is_oh_door_turn_off()
{
  
}
uint8_t BSP_is_oh_door_block()
{
  
}
uint8_t BSP_is_oh_door_cleared()
{
  
}

uint8_t BSP_is_ew_signal_ok()
{
  
}

uint8_t BSP_is_rl_in_rst_pos()
{
 GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
}

uint16_t BSP_rl_get_motor_steps()
{
  
}
void BSP_rl_motor_run(uint8_t dir,uint16_t steps)
{
  
 if(dir==BSP_RL_MOTOR_DIR_POSITIVE)
 {
   
 }
 else if(dir==BSP_RL_MOTOR_DIR_NEGATIVE)
 {
   
 }
}
void BSP_rb1_motor_pwr(uint8_t pwr_state)
{
  if(pwr_state==BSP_PWR_ON_POSITIVE)
  {
    
  }
  else if(pwr_state==BSP_PWR_ON_NEGATIVE)
  {
    
  }
  else if(pwr_state==BSP_PWR_DWN)
  {
    
  }
  
}
void BSP_rb2_motor_pwr(uint8_t pwr_state)
{
  if(pwr_state==BSP_PWR_ON_POSITIVE)
  {
    
  }
  else if(pwr_state==BSP_PWR_ON_NEGATIVE)
  {
    
  }
  else if(pwr_state==BSP_PWR_DWN)
  {
    
  }
  
}
void BSP_uv_lump_pwr(uint8_t pwr_state)
{
 if(pwr_state==BSP_PWR_ON)
 {
 }
 else if(pwr_state==BSP_PWR_DWN)
 {
   
 }
}
void BSP_e_lump_pwr(uint8_t pwr_state)
{
  if(pwr_state==BSP_PWR_ON)
 {
 }
 else if(pwr_state==BSP_PWR_DWN)
 {
   
 } 
}

void BSP_oh_door_motor_pwr( uint8_t pwr_state)
{
   if(pwr_state==BSP_PWR_ON_POSITIVE)
  {
    
  }
  else if(pwr_state==BSP_PWR_ON_NEGATIVE)
  {
    
  }
  else if(pwr_state==BSP_PWR_DWN)
  {
    
  } 
}










