/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "app_util.h"
#include "app_fifo.h"
#include "app_error.h"
#include "app_log.h"
#include "mb.h"
#include "user_tasks.h"

#if APP_LOG_ENABLED > 0    
#undef  APP_LOG_MODULE_NAME 
#undef  APP_LOG_MODULE_LEVEL
#define APP_LOG_MODULE_NAME   "[user_tasks]"
#define APP_LOG_MODULE_LEVEL   APP_LOG_LEVEL_DEBUG    
#endif

/* Variables -----------------------------------------------------------------*/
osThreadId com_with_ew_task_handle;        //下位机从电子秤接收数据的任务handle;
osThreadId com_with_host_task_handle;      //下位机从主机接收数据的任务handle;

osThreadId sensor_info_collect_task_handle;//传感器数据收集任务的handle;
osThreadId cmd_parse_dispatch_task_handle; //命令解析与分发任务的handle;
osThreadId rm_execute_task_handle;         //碾米机操作任务的handle;
/**************************************************************************/
osMessageQId rm_opt_cmd_queue_handle;

void communication_with_ew_task(void const * argument);
void communication_with_host_task(void const * argument);
void sensor_info_collect_task(void const * argument);
void cmd_parse_dispatch_task(void const * argument);
void rice_mill_execute_task(void const * argument);


static void tasks_environment_init();



void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
  APP_LOG_ERROR("%s: STACK OVERFLOW!\r\n",pcTaskName );
  UNUSED_PARAMETER(xTask);
}
void vApplicationMallocFailedHook(void)
{
  APP_LOG_ERROR("APP MALLOC FAILED!\r\n" );
}




 void app_create_user_tasks(void)//创建用户任务
 {
  
  osThreadDef(communication_with_ew_task, communication_with_ew_task, osPriorityNormal, 0, 128);
  com_with_ew_task_handle = osThreadCreate(osThread(communication_with_ew_task), NULL);
  
  osThreadDef(communication_with_host_task, communication_with_host_task, osPriorityNormal, 0, 128);
  com_with_host_task_handle = osThreadCreate(osThread(communication_with_host_task), NULL);
  
  osThreadDef(sensor_info_collect_task, sensor_info_collect_task, osPriorityNormal, 0, 128);
  sensor_info_collect_task_handle = osThreadCreate(osThread(sensor_info_collect_task), NULL);
  
  osThreadDef(cmd_parse_dispatch_task, cmd_parse_dispatch_task, osPriorityNormal, 0, 128);
  cmd_parse_dispatch_task_handle = osThreadCreate(osThread(cmd_parse_dispatch_task), NULL);
 
  osThreadDef(rice_mill_execute_task, rice_mill_execute_task, osPriorityNormal, 0, 128);
  rm_execute_task_handle = osThreadCreate(osThread(rice_mill_execute_task), NULL);
  
  
  /****消息队列*******/
  osMessageQDef(rm_opt_cmd_queue,RM_OPT_CMD_QUEUE_SIZE,uint8_t);
  rm_opt_cmd_queue_handle= osMessageCreate (osMessageQ(rm_opt_cmd_queue), NULL);
  
  
  
 }

static void tasks_environment_init()
{
  APP_LOG_DEBUG("tasks_environment_init!\r\n");
   
  
}

/**
* @brief -- 
* @param -- 
* @return -- 
* @details --
* @see --
*/

void communication_with_host_task(void const * argument)
{
   APP_LOG_DEBUG("MODBUS SLAVE TASK START!\r\n");
    osEvent =osSignalWait(SIGNAL_RX_FROM_EW_COMPLETED,osWaitForever); //无超时
   
   if( osEvent.status !=  osEventSignal)
   {
     APP_LOG_ERROR("recv signal error!\r\n");
   }
   else
   {
     APP_LOG_DEBUG("recv signal success!\r\n");;        
   }
    
    /* Select either ASCII or RTU Mode. */
    ( void )eMBInit( MB_RTU, 0x0A, 0, 9600, MB_PAR_NONE );
    /* Enable the Modbus Protocol Stack. */
    ( void )eMBEnable();
    for( ;; )
    {
      /* Call the main polling loop of the Modbus protocol stack. */
      ( void )eMBPoll();
    }
  }

void communication_with_ew_task(void const * argument)
{
   APP_LOG_DEBUG("MODBUS MASTER TASK START!\r\n");
   /* Select either ASCII or RTU Mode. */
   (void)eMBMasterInit();
    /* Enable the Modbus Protocol Stack. */
   (void)eMBMasterEnable();
    for( ;; )
    {
     /* Call the main polling loop of the Modbus protocol stack. */  
    (void)eMBMasterPoll();
   }
  
}

void sensor_info_collect_task(void const * argument)
{
uint16_t t,rh;
/* 温湿度传感器*/
t=bsp_get_temperature();
rh=bsp_get_relative_humidity();

if(TEM_MAX >=t && t>= TEM_MIN)
{
  set_reg_value( RB1_1_TEMPERATURE_REGINPUT_ADDR,1, t,REGINPUT_MODE);
}
else
{
  set_reg_value( RB1_1_TEMPERATURE_REGINPUT_ADDR,1, t,REGINPUT_MODE);
}
if(RH_MAX >=rh || rh >= RH_MIN)
{
 set_reg_value( RB1_1_RH_REGINPUT_ADDR,1, rh);   
}
 





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

void rm_rb1_switch_asyn_task(void const * argument)
{
  osSignalWait(RM_RB1_1_OPEN_EVT|RM_RB1_1_CLOSE_EVT|RM_RB1_2_OPEN_EVT|RM_RB1_2_CLOSE_EVT,oSWaitForever);//等待碾米开始
  
}

/*
 * 碾米机动作异步命令执行任务
 */
void rice_mill_execute_asyn_task(void const * argument)
{
 osEvent event;
 while(1)
 {
 event= osMessageGet(rm_opt_cmd_queue_handle,1000);
 if(event.status!=osEventMessage)
 {
   return ;
 }
  
 switch(event.value.v)
 {
 case CMD_TURN_ON_RM:
 rm_exe_turn_on_rm();  
 break;   
 case CMD_TURN_OFF_RM:
 rm_exe_turn_off_rm();    
 break;   
 case CMD_RB1_SELECT_NO1:
 rm_exe_rb1_select_no1();  
 break;   
 case CMD_RB1_SELECT_NO2:
 rm_exe_rb1_select_no2();    
 break;
 case CMD_SET_RW_VALUE:
 rm_exe_set_rw_value();    
 break;   
 case CMD_SET_RL_0:
 rm_exe_set_rl_0();  
 break;
 case CMD_SET_RL_5:
 rm_exe_set_rl_5();    
 break;   
 case CMD_SET_RL_7:
 rm_exe_set_rl_7();    
 break;
 case CMD_SET_RL_9:
 rm_exe_set_rl_9();   
 break;   
 case CMD_SET_RM_FAULT_CODE:
 rm_exe_set_rm_fault_code(); 
 break;
 case CMD_TURN_ON_RM_MOTOR:
 rm_exe_turn_on_rm_motor();  
 break;   
 case CMD_TURN_OFF_RM_MOTOR:
 rm_exe_turn_off_rm_motor();    
 break;
 case CMD_TURN_ON_RB1_1_SWITCH:
 rm_exe_turn_on_rb1_1_switch();    
 break;   
 case CMD_TURN_OFF_RB1_1_SWITCH:
 rm_exe_turn_off_rb1_1_switch();  
 break;
 case CMD_TURN_ON_RB1_2_SWITCH:
 rm_exe_turn_on_rb1_2_switch();   
 break;
 case CMD_TURN_OFF_RB1_2_SWITCH:
 rm_exe_turn_off_rb1_2_switch();  
 break;   
 case CMD_TURN_ON_RB2_SWITCH:
 rm_exe_turn_on_rb2_switch();   
 break;
 case CMD_TURN_OFF_RB2_SWITCH:
 rm_exe_turn_off_rb2_switch  
 break;   
 case CMD_TURN_ON_UV_LAMP:
 rm_exe_turn_on_uv_lamp();  
 break;   
 case CMD_TURN_OFF_UV_LAMP:
 rm_exe_turn_off_uv_lamp(); 
 break;
 case CMD_TURN_ON_E_LAMP:
 rm_exe_turn_on_e_lamp();   
 break;
 case CMD_TURN_OFF_E_LAMP:
 rm_exe_turn_off_e_lamp(); 
 break;   
 case CMD_TURN_ON_OH_DOOR:
 rm_exe_turn_on_oh_door();  
 break;
 case CMD_TURN_OFF_OH_DOOR:
 rm_exe_turn_off_oh_door();  
 break;   
 case CMD_ENABLE_REMOVE_TARE:
 rm_exe_enable_remove_tare();  
 break; 
 case CMD_DISABLE_REMOVE_TARE:
 rm_exe_disable_remove_tare();  
 break;
 case CMD_ENABLE_ZERO_CLEARING:
 rm_exe_enable_zero_clearing();   
 break;   
 case CMD_DISABLE_ZERO_CLEARING:
 rm_exe_disable_zero_clearing(); 
 break; 
 case CMD_SET_EW_THRESHOLD_VALUE:
 rm_exe_set_ew_value(); 
 break; 
 }
 }
 
}

/********************************************************************
碾米机同步执行任务
********************************************************************/
#define  RM_EXE_START_EVT                (1<<0)
#define  RM_STEP_HALT_EVT                (1<<1)
#define  RM_EXE_RB1_1_OPEN_OK_EVT        (1<<2)
#define  RM_EXE_RB1_2_OPEN_OK_EVT        (1<<4)
#deifne  RM_EXE_RB1_2_OPEN_ERROR_EVT     (1<<5)
#define  RM_EXE_RB1_CLOSE_OK_EVT         (1<<6) 
#define  RM_EXE_RB1_CLOSE_ERROR_EVT      (1<<7)
#define  RM_EXE_RB2_OPEN_OK_EVT          (1<<8)
#define  RM_EXE_RB2_OPEN_ERROR_EVT       (1<<9)
#define  RM_EXE_RB2_CLOSE_OK_EVT         (1<<10)
#define  RM_EXE_RB2_CLOSE_ERROR_EVT      (1<<11)
#define  RM_EXE_EW_SET_OK_EVT            (1<<12)
#define  RM_EXE_EW_SET_ERROR_EVT         (1<<13)
#define  RM_EXE_EW_OVERLOAD_EVT          (1<<14)
#define  RM_EXE_EW_VALUE_INVALID_EVT     (1<<15)
#define  RM_EXE_RUN_TIME_OK_EVT          (1<<16)
#define  RM_EXE_RUN_TIME_ERROR_EVT       (1<<17)
#define  RM_EXE_RUN_SET_FAULT_EVT        (1<<18)
#define  RM_EXE_RUN_CLEAR_FAULT_EVT      (1<<19)
#define  RM_EXE_RUN_GET_RW_OK_EVT
#define  RM_EXE_RUN_GET_RW_TIMEOUT_EVT

void rm_mill_shutdown()
{
  set_reg_value(); //碾米进度错误
  set_reg_value();//碾米开关
  osThreadSuspend(NULL);//挂起自己
}
void rm_mill_busy()
{
set_reg_value(); //碾米进度进行中 
}
void rm_mill_opt_complete()
{
  set_reg_value(); //碾米进度完成
  set_reg_value();//碾米开关
  osThreadSuspend(NULL);//挂起自己  
}

void rm_step_set_rw()
 {
 uint32_t rm_w;//设置电子秤阈值
 rm_w=get_reg_value( RW_REGHOLDING_ADDR,2,REGHOLDING_MODE);
 set_reg_value(EW_THRESHOLD_REGHOLDING_ADDR,2,rm_w,REGHOLDING_MODE); 
 }
void rm_step_opt_rb1(uint16_t reg_value)
{
   uint16_t rb1;
   rb1=get_reg_value( RB1_SELECTION_REGHOLDING_ADDR,1,REGHOLDING_MODE);
   if(rb1==RB1_NO1_ID)
   set_reg_value(RB1_1_SWITCH_REGHOLDING_ADDR,1,reg_value,REGHOLDING_MODE); 
   else
   set_reg_value(RB1_2_SWITCH_REGHOLDING_ADDR,1,reg_value,REGHOLDING_MODE); 
}

void rm_step_wait_ew_weight()
{
 //由传感器任务提供所需信号
}

void rm_step_opt_rl()
{
 uint16_t rl;
 rl=get_reg_value( RL_REGHOLDING_ADDR,1,REGHOLDING_MODE); 
 set_reg_value(RL_CONTROL_REGHOLDING_ADDR, 1,rl,REGHOLDING_MODE);
}
void rm_step_opt_rm_motor(uint16_t reg_value)
{
 set_reg_value(RM_MOTOR_SWITCH_REGHOLDING_ADDR, 1,reg_value,REGHOLDING_MODE); 
}

void rm_step_opt_rb2(uint16_t reg_value)
{
 set_reg_value(RB2_SWITCH_REGHOLDING_ADDR, 1,reg_value,REGHOLDING_MODE);  
}

void rm_step_opt_oh_door(uint16_t reg_value)
{
 set_reg_value(OH_DOOR_SWITCH_REGHOLDING_ADDR, 1,reg_value,REGHOLDING_MODE);  
}




void rice_mill_execute_sync_task(void const * argument)
{
    
while(1)
{
 rm_mill_opt_complete();//wait
 EventBits_t xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear ) 

 rm_step_set_ew_threshold();
 event_bits=xEventGroupWaitBits(rm_sync_event_hdl,RM_STEP_HALT_EVT|RM_STEP_SET_EW_THRESHOLD_OK_EVT,pdTRUE,pdFALSE,TIMEOUT_VALUE );//等待出米重量设置完成
 if(event_bits & RM_STEP_HALT_EVT ||!(event_bits & RM_EXE_EW_SET_OK_EVT))//timeout or error
 {
  rm_mill_shutdown();
 }

 rm_step_opt_rb1(REG_VALUE_SWITCH_ON);
 event_bits=xEventGroupWaitBits(rm_sync_event_hdl,RM_STEP_HALT_EVT |RM_STEP_OPEN_RB1_OK_EVT,pdTRUE,pdFALSE,TIMEOUT_VALUE );//一级仓打开到位
 if(event_bits & RM_STEP_HALT_EVT ||!(event_bits & RM_STEP_OPEN_RB1_OK_EVT))//timeout
 {
  rm_mill_shutdown();  
  continue;
 }
 
 rm_step_wait_ew_weight();//等待米到达重量
 event_bits=xEventGroupWaitBits(rm_sync_event_hdl,RM_STEP_HALT_EVT |RM_STEP_WAIT_RW_OK_EVT,pdTRUE,pdFALSE,TIMEOUT_VALUE );//达到米重
 if(event_bits & RM_STEP_HALT_EVT ||!(event_bits & RM_STEP_WAIT_RW_OK_EVT))//timeout
 {
  rm_mill_shutdown();
 }
 
 rm_step_opt_rb1(REG_VALUE_SWITCH_OFF);
 event_bits=xEventGroupWaitBits(rm_sync_event_hdl,RM_STEP_HALT_EVT |RM_STEP_CLOSE_RB1_OK_EVT,pdTRUE,pdFALSE,TIMEOUT_VALUE );//一级仓关闭到位
 if(event_bits & RM_STEP_HALT_EVT ||!(event_bits & RM_STEP_CLOSE_RB1_OK_EVT))//timeout
 {
  rm_mill_shutdown();
 }
 
 rm_step_opt_rl();
 event_bits=xEventGroupWaitBits(rm_sync_event_hdl,RM_STEP_HALT_EVT |RM_STEP_OPT_RL_OK_EVT,pdTRUE,pdFALSE,TIMEOUT_VALUE );//分度调节器到位
 if(event_bits & RM_STEP_HALT_EVT ||!(event_bits & RM_STEP_OPT_RL_OK_EVT))//timeout
 {
  rm_mill_shutdown();
 }
 
 rm_step_opt_rm_motor(REG_VALUE_SWITCH_ON);
 osDelay(1000);//wait 1 s
 rm_step_opt_rb2(REG_VALUE_SWITCH_ON);
 event_bits=xEventGroupWaitBits(rm_sync_event_hdl,RM_STEP_HALT_EVT |RM_STEP_OPEN_RB2_OK_EVT,pdTRUE,pdFALSE,TIMEOUT_VALUE );//
 if(event_bits & RM_STEP_HALT_EVT ||!(event_bits & RM_STEP_OPEN_RB1_OK_EVT))//timeout
 {
  rm_mill_shutdown();
 }
 
  event_bits=xEventGroupWaitBits(rm_sync_event_hdl,RM_STEP_HALT_EVT |RM_STEP_RL_MOTOR_TIME_OK_EVT,pdTRUE,pdFALSE,TIMEOUT_VALUE );//
 if(event_bits & RM_STEP_HALT_EVT ||!(event_bits & RM_STEP_RL_MOTOR_TIME_OK_EVT))//timeout
 {
  rm_mill_shutdown();
 }
 rm_step_opt_rm_motor(REG_VALUE_SWITCH_OFF);
 rm_step_opt_rb2(REG_VALUE_SWITCH_OFF);
 event_bits=xEventGroupWaitBits(rm_sync_event_hdl,RM_STEP_HALT_EVT |RM_STEP_CLOSE_RB2_OK_EVT,pdTRUE,pdFALSE,TIMEOUT_VALUE );//
 if(event_bits & RM_STEP_HALT_EVT ||!(event_bits & RM_STEP_CLOSE_RB2_OK_EVT))//timeout
 {
  rm_mill_shutdown();
 }
 
 rm_step_opt_oh_door(REG_VALUE_SWITCH_ON);
 event_bits=xEventGroupWaitBits(rm_sync_event_hdl,RM_STEP_HALT_EVT |RM_STEP_OPEN_OH_DOOR_OK_EVT,pdTRUE,pdFALSE,TIMEOUT_VALUE );//
 if(event_bits & RM_STEP_HALT_EVT ||!(event_bits & RM_STEP_OPEN_OH_DOOR_OK_EVT))//timeout
 {
  rm_mill_shutdown();
 }
 
 rm_step_opt_oh_door(REG_VALUE_SWITCH_OFF);
 event_bits=xEventGroupWaitBits(rm_sync_event_hdl,RM_STEP_HALT_EVT |RM_STEP_CLOSE_OH_DOOR_OK_EVT,pdTRUE,pdFALSE,TIMEOUT_VALUE );//
 if(event_bits & RM_STEP_HALT_EVT ||!(event_bits & RM_STEP_CLOSE_OH_DOOR_OK_EVT))//timeout
 {
  rm_mill_shutdown();
 }
 
 }
}





void rm_exe_turn_on_rm()
{

}

void rm_exe_turn_off_rm()
{
  g_rm_status=RM_STATUS_TURN_OFF; 
  osMessagePut(rm_opt_cmd_queue_handle ,CMD_DISABLE_ZERO_CLEARING,0);
}

void rm_exe_turn_on_rb1_1_switch()
{
 osTimerStart (rm_exe_timeout_timer_handle,RM_EXE_TIMER_TIMEOUT_VALUE);//打开超时定时器
 BSP_motor_turn_on_rb1_1();   
}

void rm_exe_turn_off_rb1_1_switch()
{
 osTimerStop (rm_exe_timeout_timer_handle);//关闭超时定时器
 BSP_motor_turn_off_rb1_1();  

 if(g_rm_status==RM_STATUS_TURN_ON)
 {
 rm_(rm_opt_cmd_queue_handle ,CMD_TURN_ON_RM_MOTOR,0); //准备下一步 开启碾米电机
 }
}