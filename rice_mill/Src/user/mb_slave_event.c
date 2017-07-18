#include "mb_slave_event.h"
#include "cmsis_os.h"
#include "app_log.h"

typedef (void *ptr_regholding_write_event_handler_t)(void);



void mb_slave_event_init()
{
  
 APP_LOG_DEBUG("mb_slave_event_init!\r\n"); 
  
}

/*****************************************   地址         寄存器   ************/
void set_evt_rm_switch_regholding_write(void);        //0x1000      碾米机开关
void set_evt_rb1_selection_regholding_write(void);    //0x1001      米仓选择

void set_evt_rw_regholding_write(void);               //0x1002      出米重量低
void set_evt_rw_regholding_write(void);               //0x1003      出米重量高

void set_evt_rl_regholding_write(void);               //0x1004      碾米分度值

void set_evt_rm_fault_code_regholding_write(void);     //0x1005      碾米机故障码低
void set_evt_rm_fault_code_regholding_write(void);     //0x1006      碾米机故障码高

void set_evt_rm_motor_switch_regholding_write(void);   //0x1007      碾米机马达/吹糠机
void set_evt_rb1_1_switch_regholding_write(void);      //0x1008      1号米仓阀门
void set_evt_rb1_2_switch_regholding_write(void);      //0x1009      2号米仓阀门
void set_evt_rb2_switch_regholding_write(void);        //0x100a      2级米仓阀门
void set_evt_uv_lamp_switch_regholding_write(void);    //0x100b      紫外灯开关
void set_evt_e_lamp_switch_regholding_write(void);     //0x100c      环境灯开关
void set_evt_oh_door_switch_regholding_write(void);    //0x100d      升降门开关
void set_evt_r_tare_regholding_write(void);            //0x100e      去皮
void set_evt_z_clearing_regholding_write(void);        //0x100f      清零
void set_evt_w_threshold_regholding_write(void);       //0x1010      重量定点值低
void set_evt_w_threshold_regholding_write(void);       //0x1011      重量定点值高




void set_evt_rm_switch_regholding_write(void)        //0x1000      碾米机开关
{
 osSignalSet( rm_execute_thread_handle, RM_SWITCH_SETUP_EVENT); 
}
void set_evt_rb1_selection_regholding_write(void)    //0x1001      米仓选择
{
osSignalSet( rm_execute_thread_handle, RB1_SELECTION_EVENT);   
}

void set_evt_rw_regholding_write(void)               //0x1002      出米重量
{
 osSignalSet( rm_execute_thread_handle, RW_SETUP_EVENT);  
}


void set_evt_rl_regholding_write(void)               //0x1004      碾米分度值
{
 osSignalSet( rm_execute_thread_handle,RL_SETUP_EVENT);  
}

void set_evt_rm_fault_code_regholding_write(void)     //0x1005      碾米机故障码
{
 osSignalSet( rm_execute_thread_handle,RM_FAULT_CODE_SETUP_EVENT);  
}


void set_evt_rm_motor_switch_regholding_write(void)   //0x1007      碾米机马达/吹糠机
{
  osSignalSet( rm_execute_thread_handle,RM_MOTOR_SWITCH_SETUP_EVENT);   
}
void set_evt_rb1_1_switch_regholding_write(void)      //0x1008      1号米仓阀门
{
  osSignalSet( rm_execute_thread_handle,RB1_1_SWITCH_SETUP_EVENT); 
}
void set_evt_rb1_2_switch_regholding_write(void)      //0x1009      2号米仓阀门
{
 osSignalSet( rm_execute_thread_handle,RB1_2_SWITCH_SETUP_EVENT);   
}
void set_evt_rb2_switch_regholding_write(void)        //0x100a      2级米仓阀门
{
  osSignalSet( rm_execute_thread_handle,RB2_SWITCH_SETUP_EVENT);    
}
void set_evt_uv_lamp_switch_regholding_write(void)    //0x100b      紫外灯开关
{
  osSignalSet( rm_execute_thread_handle,UV_LAMP_SWITCH_SETUP_EVENT);  
}
void set_evt_e_lamp_switch_regholding_write(void)     //0x100c      环境灯开关
{
  osSignalSet( rm_execute_thread_handle,E_LAMP_SWITCH_SETUP_EVENT);  
}
void set_evt_oh_door_switch_regholding_write(void)    //0x100d      升降门开关
{
  osSignalSet( rm_execute_thread_handle,OH_DOOR_SWITCH_SETUP_EVENT);   
}
void set_evt_r_tare_regholding_write(void)            //0x100e      去皮
{
 osSignalSet( rm_execute_thread_handle,R_TARE_SETUP_EVENT);    
}
void set_evt_z_clearing_regholding_write(void)        //0x100f      清零
{
 osSignalSet( rm_execute_thread_handle,Z_CLEARING_SETUP_EVENT);   
}
void set_evt_w_threshold_regholding_write(void)       //0x1010      重量定点值
{
 osSignalSet( rm_execute_thread_handle,W_THRESHOLD_SETUP_EVENT);    
}

osEvent osSignalWait (int32_t signals, uint32_t millisec)