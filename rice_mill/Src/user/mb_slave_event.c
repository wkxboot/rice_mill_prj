#include "mb_slave_event.h"
#include "cmsis_os.h"
#include "app_log.h"

typedef void (*ptr_regholding_write_event_handler_t)(void);



void mb_slave_event_init()
{
  
 APP_LOG_DEBUG("mb_slave_event_init!\r\n"); 
  
}

/*****************************************   地址      寄存器       ************/
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
 osSignalSet( rm_execute_thread_handle,EW_THRESHOLD_SETUP_EVENT);    
}


/*
 *碾米机命令解析分发任务
 */

void cmd_parse_dispatch_task(void const * argument)
{
 osEvent events;
 osStatus status;
 uint32_t reg_value;
 
while(1)
{
 events= osSignalWait(ALL_RM_EVENT,oSWaitForever);
if(events.status!= osEventSignal)
{
 continue; 
}

if(events.value.signals & RM_SWITCH_SETUP_EVENT)
{
 reg_value= get_reg_value(RM_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
   reg_value= get_rm_fault_code();
   if(!reg_value)
   {
   status= osMessagePut(rm_opt_cmd_queue_handle ,CMD_TURN_ON_RM,0);
   APP_LOG_DEBUG("RM_SWITCH_SETUP_EVENT  ON!status:%d\r\n",status);
   }
   else
   {
   APP_LOG_DEBUG("RM_SWITCH_SETUP_EVENT IGNORE!\r\n");  
   }
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_OFF_RM,0);
   APP_LOG_DEBUG("RM_SWITCH_SETUP_EVENT  OFF!status:%d\r\n",status);
 }
}


if(events.value.signals & RB1_SELECTION_EVENT)
{
   reg_value= get_reg_value(RB1_SELECTION_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
   APP_LOG_DEBUG("RB1_SELECTION_EVENT!\r\n");
   APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
   if(reg_value==RB1_NO_1_ID)
   {
   status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_RB1_SELECT_NO1,0);
   APP_LOG_DEBUG("rb1 select NO1!status:%d\r\n",status);
   }
   else if(reg_value==RB1_NO_2_ID)
   {
   status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_RB1_SELECT_NO2,0);
   APP_LOG_DEBUG("rb1 select NO2!status:%d\r\n",status);
   }
}

if(events.value.signals & RW_SETUP_EVENT)
{
   reg_value= get_reg_value(RW_REGHOLDING_ADDR, 2,REGHOLDING_MODE);
   
   APP_LOG_DEBUG("RW_SETUP_EVENT!\r\n");
   APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
   
   if(reg_value>RW_VALUE_MIN && reg_value<RW_VALUE_MAX)
   {
   status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_SET_RW_VALUE,0);
   APP_LOG_DEBUG("weight is valied! status:%d\r\n",status);
   }
} 
if(events.value.signals & RL_SETUP_EVENT)
{
   uint16_t cmd;
   reg_value= get_reg_value(RL_REGHOLDING_ADDR, 1,REGHOLDING_MODE);

   APP_LOG_DEBUG("RL_SETUP_EVENT!\r\n");
   APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
}
if(events.value.signals & RL_CONTROL_EVENT)
{
    uint16_t cmd;
   reg_value= get_reg_value(RL_CONTROL_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
   
   APP_LOG_DEBUG("RL_SETUP_EVENT!\r\n");
   APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
   
   if(reg_value==RL_VALUE_0)
   {
     cmd=CMD_SET_RL_0;
   }
   else if(reg_value==RL_VALUE_5)
   {
    cmd=CMD_SET_RL_5; 
   }
   else if(reg_value==RL_VALUE_7)
   {
     cmd=CMD_SET_RL_7;  
   }
   else if(reg_value==RL_VALUE_9)
   {
     cmd=CMD_SET_RL_9;  
   }
   status= osMessagePut (rm_opt_cmd_queue_handle ,cmd,0);
   APP_LOG_DEBUG("cmd is valied! status:%d\r\n",status);  
   
}
if(events.value.signals & RM_FAULT_CODE_SETUP_EVENT)
{
   reg_value=  get_rm_fault_code();
   
   APP_LOG_DEBUG("RM_FAULT_CODE_SETUP_EVENT!\r\n");
   APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
   
   if(reg_value)
   {
   set_reg_value(RM_SWITCH_REGHOLDING_ADDR, 1,REG_VALUE_SWITCH_OFF,REGHOLDING_MODE); //修改值为REG_VALUE_SWITCH_OFF
   APP_LOG_DEBUG("internal turn off rm_switch as rm fault!\r\n");
   }
   status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_SET_RM_FAULT_CODE,0);
   APP_LOG_DEBUG("set rm fault code! status:%d\r\n",status);  
}

if(events.value.signals & RM_MOTOR_SWITCH_SETUP_EVENT)
{
  reg_value= get_reg_value(RM_MOTOR_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("RM_MOTOR_SWITCH_SETUP_EVENT!\r\n");
  APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_ON_RM_MOTOR,0);
  APP_LOG_DEBUG("turn on rm motor!status:%d\r\n",status);
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_OFF_RM_MOTOR,0);
   APP_LOG_DEBUG("turn off rm motor!status:%d\r\n",status);
 } 
}
if(events.value.signals & RB1_1_SWITCH_SETUP_EVENT)
{
  reg_value= get_reg_value(RB1_1_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("RB1_1_SWITCH_SETUP_EVENT!\r\n");
  APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_ON_RB1_1_SWITCH,0);
  APP_LOG_DEBUG("turn on rb1_1!status:%d\r\n",status);
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_OFF_RB1_1_SWITCH,0);
   APP_LOG_DEBUG("turn off rb1_1!status:%d\r\n",status);
 }  
}
if(events.value.signals & RB1_2_SWITCH_SETUP_EVENT)
{
  reg_value= get_reg_value(RB1_2_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("RB1_2_SWITCH_SETUP_EVENT!\r\n");
  APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_ON_RB1_2_SWITCH,0);
  APP_LOG_DEBUG("turn on rm motor!status:%d\r\n",status);
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_ON_RB1_2_SWITCH,0);
   APP_LOG_DEBUG("turn off rm motor!status:%d\r\n",status);
 } 
}
if(events.value.signals & RB2_SWITCH_SETUP_EVENT)
{
   reg_value= get_reg_value(RB2_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("RB2_SWITCH_SETUP_EVENT!\r\n");
  APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_ON_RB2_SWITCH,0);
  APP_LOG_DEBUG("turn on rb2!status:%d\r\n",status);
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_OFF_RB2_SWITCH,0);
   APP_LOG_DEBUG("turn off rb2!status:%d\r\n",status);
 } 
}
if(events.value.signals & UV_LAMP_SWITCH_SETUP_EVENT)
{
  reg_value= get_reg_value(UV_LAMP_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("UV_LAMP_SWITCH_SETUP_EVENT!\r\n");
  APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_ON_UV_LAMP,0);
  APP_LOG_DEBUG("turn on uv lamp!status:%d\r\n",status);
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_OFF_UV_LAMP,0);
   APP_LOG_DEBUG("turn off uv lamp!status:%d\r\n",status);
 }  
}
if(events.value.signals & E_LAMP_SWITCH_SETUP_EVENT)
{
   reg_value= get_reg_value(E_LAMP_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("E_LAMP_SWITCH_SETUP_EVENT!\r\n");
  APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_ON_E_LAMP,0);
  APP_LOG_DEBUG("turn on e lamp!status:%d\r\n",status);
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_ON_E_LAMP,0);
   APP_LOG_DEBUG("turn off e lamp!status:%d\r\n",status);
 } 
}
if(events.value.signals & OH_DOOR_SWITCH_SETUP_EVENT)
{
  reg_value= get_reg_value(OH_DOOR_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("OH_DOOR_SWITCH_SETUP_EVENT!\r\n");
  APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_ON_OH_DOOR,0);
  APP_LOG_DEBUG("turn on rm motor!status:%d\r\n",status);
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_opt_cmd_queue_handle ,CMD_TURN_OFF_OH_DOOR,0);
   APP_LOG_DEBUG("turn off rm motor!status:%d\r\n",status);
 } 
}
if(events.value.signals & R_TARE_SETUP_EVENT)
{
  reg_value= get_reg_value(R_TARE_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("R_TARE_SETUP_EVENT!\r\n");
  APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_FUNC_ENABLE )
 {
  status= osMessagePut(rm_opt_cmd_queue_handle ,CMD_ENABLE_REMOVE_TARE,0);
  APP_LOG_DEBUG("enable remove tare!status:%d\r\n",status);
 }
 else if(reg_value==REG_VALUE_FUNC_DISABLE )
 {
  status= osMessagePut(rm_opt_cmd_queue_handle ,CMD_DISABLE_REMOVE_TARE,0);
  APP_LOG_DEBUG("disable remove tare!status:%d\r\n",status); 
 }
}
if(events.value.signals & Z_CLEARING_SETUP_EVENT)
{
  reg_value= get_reg_value(Z_CLEARING_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("Z_CLEARING_SETUP_EVENT!\r\n");
  APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_FUNC_ENABLE )
 {
  status= osMessagePut(rm_opt_cmd_queue_handle ,CMD_ENABLE_ZERO_CLEARING,0);
  APP_LOG_DEBUG("enable z clearing!status:%d\r\n",status);
 } 
 else if(reg_value==REG_VALUE_FUNC_DISABLE )
 {
  status= osMessagePut(rm_opt_cmd_queue_handle ,CMD_DISABLE_ZERO_CLEARING,0);
  APP_LOG_DEBUG("disable z clearing!status:%d\r\n",status); 
 }
}
if(events.value.signals & EW_THRESHOLD_SETUP_EVENT)
{
  reg_value= get_reg_value(EW_THRESHOLD_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("EW_THRESHOLD_SETUP_EVENT!\r\n");
  APP_LOG_DEBUG("reg_value:%d\r\n",reg_value);
 if(reg_value> RW_VALUE_MIN && reg_value< RW_VALUE_MAX )
 {
  status= osMessagePut(rm_opt_cmd_queue_handle ,CMD_SET_EW_THRESHOLD_VALUE,0);
  APP_LOG_DEBUG("set ew threshold!status:%d\r\n",status);
 }  
}
}
}