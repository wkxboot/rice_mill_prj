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
void cmd_parse_dispatch_task(void const * argument)
{
 osEvent events;
 events= osSignalWait (ALL_RM_EVENT,oSWaitForever);
if(events.status!= osEventSignal)
return;

if(events & RM_SWITCH_SETUP_EVENT)
{
  
}
if(events & RB1_SELECTION_EVENT)
{
  
} 
if(events & RW_SETUP_EVENT)
{
  
} 
if(events & RL_SETUP_EVENT)
{
  
}
if(events & RM_FAULT_CODE_SETUP_EVENT)
{
  
}
if(events & RM_MOTOR_SWITCH_SETUP_EVENT)
{
  
}
if(events & RB1_1_SWITCH_SETUP_EVENT)
{
  
}
if(events & RB1_2_SWITCH_SETUP_EVENT)
{
  
}
if(events & RB2_SWITCH_SETUP_EVENT)
{
  
}
if(events & UV_LAMP_SWITCH_SETUP_EVENT)
{
  
}
if(events & E_LAMP_SWITCH_SETUP_EVENT)
{
  
}
if(events & OH_DOOR_SWITCH_SETUP_EVENT)
{
  
}
if(events & R_TARE_SETUP_EVENT)
{
  
}
if(events & Z_CLEARING_SETUP_EVENT)
{
  
}
if(events & W_THRESHOLD_SETUP_EVENT)
{
  
}
}
void rice_mill_execute_task(void const * argument)
{
  
  
  
}
