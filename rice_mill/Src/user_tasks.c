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
   APP_LOG_DEBUG("工控机轮询任务开始!\r\n");

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
   APP_LOG_DEBUG("电子秤轮询任务开始 !\r\n");
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

void ew_func_task(void const * argument)
{
 osEvent ew_msg;
 uint16_t reg_value;
 uint32_t threshold;
 uint16_t temp[2];
 uint16_t rice_gross_weight;
 uint16_t rice_net_weight;
 eMBMasterReqErrCode err_code;
 
 APP_LOG_DEBUG("电子秤功能任务开始 !\r\n");
 
 while(1)
 {
 ew_msg=osMessageGet(ew_queue_hdl,osWaitForever);
 if(ew_msg.status != osEventMessage)
 continue;
 err_code=MB_MRE_NO_ERR;
 switch(ew_msg.value.v)
 {
 case MSG_EW_OBTAIN_RICE_WEIGHT://毛重
 err_code=eMBMasterReqReadHoldingRegister(SLAVE_EW_ADDR,REG_GROSS_WEIGHT_ADDR,2,EW_GET_RESOURCE_TIMEOUT) 
 if(err_code==MB_MRE_NO_ERR)
 {
  rice_gross_weight=master_get_gross_weight(); 
  set_reg_value(EW_GROSS_WEIGHT_REGINPUT_ADDR,1, rice_gross_weight,REGINPUT_MODE);
 }
 break;
 case MSG_EW_OBTAIN_RICE_NET_WEIGHT://净重
 err_code=eMBMasterReqReadHoldingRegister(SLAVE_EW_ADDR,REG_NET_WEIGHT_ADDR,2,EW_GET_RESOURCE_TIMEOUT);
 if(err_code==MB_MRE_NO_ERR)
 {
   rice_net_weight= master_get_net_weight();
   set_reg_value(EW_NET_WEIGHT_REGINPUT_ADDR,1, rice_net_weight,REGINPUT_MODE);
 }
 break;
 case MSG_EW_SET_RICE_WEIGHT_THRESHOLD://设置定点值
 threshold=get_reg_value(EW_THRESHOLD_REGHOLDING_ADDR,2,REGHOLDING_MODE);
 temp[0]=(threshold>>16) & 0xffff;
 temp[1]=(threshold>>0)  & 0xffff;

 err_code=eMBMasterReqWriteMultipleHoldingRegister(SLAVE_EW_ADDR,REG_WEIGHT_THRESHOLD_ADDR,
                                                  2,
                                                  temp,
                                                  EW_GET_RESOURCE_TIMEOUT );
 if(err_code==MB_MRE_NO_ERR)
 {
  osSignalSet( rm_sync_task_hdl,SYNC_SET_EW_OK_EVT);
 }
 break;
 case MSG_EW_REMOVE_TARE://去皮重
 reg_value=0x0002;
 err_code=eMBMasterReqWriteHoldingRegister( SLAVE_EW_ADDR,REG_REMOVE_TARE_ADDR, reg_value, EW_GET_RESOURCE_TIMEOUT);
  if(err_code==MB_MRE_NO_ERR)
 {
  osSignalSet( rm_sync_task_hdl,SYNC_SET_EW_OK_EVT);
 }
 break;
 case MSG_EW_CLEARING_ZERO://清零
 reg_value=0x0000;
 err_code=eMBMasterReqWriteHoldingRegister( SLAVE_EW_ADDR,REG_CLEARING_ZERO_ADDR, reg_value, EW_GET_RESOURCE_TIMEOUT);
 if(err_code==MB_MRE_NO_ERR)
 {
  osSignalSet( rm_sync_task_hdl,SYNC_SET_EW_OK_EVT);
 }
 break; 
 }
 
 if(err_code==MB_MRE_TIMEDOUT)
 {
  set_rm_fault_code(FAULT_CODE_EW_NO_RESPONSE); 
  osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
 }

 } 
}

void sensor_info_task(void const * argument)
{
uint32_t fault_code;
uint16_t t,rh;

//xEventGroupClearBits(sync_event_hdl,SYNC_ALL_EVT);

/* 温湿度传感器*/
t=BSP_get_temperature();
rh=BSP_get_relative_humidity();

if(TEM_MAX >=t && t>= TEM_MIN)
{ 
  set_reg_value(RB1_1_TEMPERATURE_REGINPUT_ADDR,1, t,REGINPUT_MODE);
  if(t > TEM_WARNING)
  {
  set_rm_fault_code(ERROR_CODE_RB1_OT);
  osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
  }
}

if(RH_MAX >=rh || rh >= RH_MIN)
{
 set_reg_value( RB1_1_RH_REGINPUT_ADDR,1, rh,REGINPUT_MODE);
 if(rh > RH_WARNING)
 {
 set_rm_fault_code(ERROR_CODE_RB1_OTH); 
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
 }
}
/*1号米仓空检查*/

if(BSP_is_rb1_1_empty()==BSP_TRUE)
{
  set_rm_fault_code(ERROR_CODE_RB1_1_EMPTY); 
  osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
}
 /*2号米仓空检查*/

if(BSP_is_rb1_2empty()==BSP_TRUE)
{
  set_rm_fault_code(ERROR_CODE_RB1_2_EMPTY); 
  osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
}

/*1级米仓阀门电机位置/赌转检查*/

if(BSP_is_rb1_no1_turn_on()==BSP_TRUE)
{
 osTimerStop(rb1_1_turn_on_timer_hdl);
 osSignalSet( rm_sync_task_hdl,SYNC_OPEN_RB1_1_OK_EVT);
}
 if(BSP_is_rb1_no2_turn_on()==BSP_TRUE)
 {
 osTimerStop(rb1_2_turn_on_timer_hdl);
 osSignalSet( rm_sync_task_hdl,SYNC_OPEN_RB1_2_OK_EVT);
 }
 if(BSP_is_rb1_turn_off()==BSP_TRUE)
 {
  //set_reg_value( RB1_1_SWITCH_REGHOLDING_ADDR,1,REG_VALUE_SWITCH_OFF,REGHOLDING_MODE);
  //set_reg_value( RB1_2_SWITCH_REGHOLDING_ADDR,1,REG_VALUE_SWITCH_OFF,REGHOLDING_MODE);
  osTimerStop(rb1_turn_off_timer_hdl);
  osSignalSet( rm_sync_task_hdl,SYNC_CLOSE_RB1_OK_EVT);
 }

 if(BSP_is_rb1_block()==BSP_TRUE)
 {
  set_rm_fault_code(ERROR_CODE_RB1_SWITCH_BLOCK);
  osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
 }

/*2级仓阀门位置检测*/
if(BSP_is_rb2_turn_on()==BSP_TRUE)
{
  osTimerStop(rb2_turn_on_timer_hdl);
  osSignalSet( rm_sync_task_hdl,SYNC_OPEN_RB2_OK_EVT);
}
if(BSP_is_rb2_turn_off()==BSP_TRUE)
{
  osTimerStop(rb2_turn_off_timer_hdl);
  osSignalSet( rm_sync_task_hdl,SYNC_CLOSE_RB2_OK_EVT);
}
if(BSP_is_rb2_block()==BSP_TRUE)
{
 set_rm_fault_code(ERROR_CODE_RB2_SWITCH_BLOCK); 
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
}

/*升降门位置检测*/
if(BSP_is_oh_door_turn_on())
{
 osTimerStop(oh_door_turn_on_timer_hdl);
 osSignalSet( rm_sync_task_hdl,SYNC_OPEN_OH_DOOR_OK_EVT);
 
}

if(BSP_is_oh_door_turn_off())
{
 osTimerStop(oh_door_turn_off_timer_hdl);
 osSignalSet( rm_sync_task_hdl,SYNC_CLOSE_OH_DOOR_OK_EVT);
}

if(BSP_is_oh_door_block())
{
 set_rm_fault_code(FAULT_CODE_OH_DOOR_MOTOR_BLOCK); 
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);  
}

uint16_t oh_door_value;
oh_door_state=get_reg_value(OH_DOOR_SWITCH_REGHOLDING_ADDR,1,REGHOLDING_MODE);
                            
if(!BSP_is_oh_door_cleared() && oh_door_state==REG_VALUE_SWITCH_OFF && !BSP_is_oh_door_turn_off() )
{
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_OH_DOOR,0);
}
else if(BSP_is_oh_door_cleared() && oh_door_state==REG_VALUE_SWITCH_OFF && !BSP_is_oh_door_turn_off())
{
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_OH_DOOR,0); 
}


/*实时米重*/
rw_obtain_interval++;
if(rw_obtain_interval==RW_OBTAIN_INTERVAL)
{
osMessagePut(ew_queue_hdl,MSG_EW_OBTAIN_RICE_NET_WEIGHT,osWaitForever);
rw_obtain_interval=0;
}

uint16_t rw_cur,rw_tar;
rw_cur=get_reg_value(EW_NET_WEIGHT_REGINPUT_ADDR,1,REGINPUT_MODE) / 2;
rw_tar=get_reg_value(RW_REGHOLDING_ADDR,1,REGHOLDING_MODE);

 if(BSP_is_ew_signal_ok() && rw_cur >= rw_tar)
 osSignalSet( rm_sync_task_hdl,SYNC_OBTAIN_RW_OK_EVT);
}


/*******************************************************************************
 * 碾米机动作异步任务
 * 
 *******************************************************************************/
static void asyn_turn_on_rm()
{
  osStatus status;
  if(osThreadIsSuspended( rm_sync_task_id)==osOK)
  {
   status=osThreadResume(rm_sync_task_id);
   if(status==osOK)
   {
    APP_LOG_DEBUG("恢复碾米任务成功!\r\n");
   }
   else
   {
    APP_LOG_DEBUG("恢复碾米任务失败!\r\n"); 
   }
  }
}

void asyn_turn_off_rm()
{  
  osStatus status;
  if(osThreadIsSuspended( rm_sync_task_id)!=osOK)
  {
   status=osThreadSuspend (rm_sync_task_id);
   if(status==osOK)
   {
    APP_LOG_DEBUG("挂起碾米任务成功!\r\n");
   }
   else
   {
    APP_LOG_DEBUG("挂起碾米任务失败!\r\n");
   }
  }
}

static void asyn_rb1_select_no1()
{
 APP_LOG_DEBUG("选择 1 号仓!\r\n"); 
}
static void asyn_rb1_select_no2()
{
 APP_LOG_DEBUG("选择 2 号仓!\r\n"); 
}

static void asyn_set_rw_value()
{
 APP_LOG_DEBUG("设置米重!\r\n"); 
}

static void asyn_setup_rl()
{
APP_LOG_DEBUG("设置出米分度值!\r\n"); 
}
   
static void asyn_set_rl_common(uint16_t tar_steps);
{
 uint16_t steps;
 
 if(tar_steps==BSP_RL_MOTOR_STEPS_FOR_RL0)//0点特殊处理
 {
  if( BSP_is_rl_in_rst_pos()!=BSP_TRUE)
  {
  osTimerStart(rl_timer_hdl,RL_TIMEOUT_VALUE);
  BSP_rl_motor_run(BSP_RL_MOTOR_DIR_POSITIVE,BSP_RL_MOTOR_STEPS_FOR_RL0);//向复位点运行，到位后发送到位信号
  } 
  else
  {
  APP_LOG_DEBUG("分度值正确，忽略设定!");  
  } 
 }
 else
 {
 steps=BSP_rl_get_motor_steps(); 
 if(steps < tar_steps)
 {
 osTimerStart(rl_timer_hdl,RL_TIMEOUT_VALUE);
 BSP_rl_motor_run(BSP_RL_MOTOR_DIR_POSITIVE,tar_steps-steps);//向目标点运行，到位后发送到位信号
 
 } 
 else if(steps > tar_steps)
 {
 osTimerStart(rl_timer_hdl,RL_TIMEOUT_VALUE);
 BSP_rl_motor_run(BSP_RL_MOTOR_DIR_NEGATIVE,steps-tar_steps);//向目标点运行，到位后发送到位信号   
 }
 else
 {
  APP_LOG_DEBUG("分度值正确，忽略设定!");  
 }
 }
}
   
static void asyn_set_rl_0()
{
 asyn_set_rl_common(RL_MOTOR_STEPS_FOR_RL0);
 APP_LOG_DEBUG("执行设置分度为0!");  
}

static void asyn_set_rl_5()
{
 asyn_set_rl_common(RL_MOTOR_STEPS_FOR_RL5);
 APP_LOG_DEBUG("执行设置分度为5!");  
}
static void asyn_set_rl_7()
{
asyn_set_rl_common(RL_MOTOR_STEPS_FOR_RL7);
APP_LOG_DEBUG("执行设置分度为5!");  
}

static void asyn_set_rl_9()
{
asyn_set_rl_common(RL_MOTOR_STEPS_FOR_RL9);
APP_LOG_DEBUG("执行设置分度为9!");  
}

static void asyn_set_rm_fault_code()
{
 APP_LOG_DEBUG("错误码设置!");  
}

static void asyn_turn_on_rm_motor()
{
  BSP_rm_motor_pwr(BSP_PWR_ON);
  APP_LOG_DEBUG("启动碾米机电机!"); 
}

static void asyn_turn_off_rm_motor()
{
  BSP_rm_motor_pwr(BSP_PWR_DWN);
  APP_LOG_DEBUG("关闭碾米机电机!"); 
}

static void asyn_turn_on_rb1_1_switch()
{
osTimerStart(rb1_1_turn_on_timer_hdl,RB1_TIMEOUT_VALUE);
BSP_rb1_motor_pwr(BSP_PWR_ON_NEGATIVE);
APP_LOG_DEBUG("打开1号仓!"); 
}

static void asyn_turn_off_rb1_1_switch()
{
osTimerStart(rb1_1_turn_off_timer_hdl,RB1_TIMEOUT_VALUE);
BSP_rb1_motor_pwr(BSP_PWR_ON_POSITIVE);
APP_LOG_DEBUG("关闭1号仓!"); 
}

static void asyn_turn_on_rb1_2_switch()
{
osTimerStart(rb1_2_turn_on_timer_hdl,RB1_TIMEOUT_VALUE);
BSP_rb1_motor_pwr(BSP_PWR_ON_POSITIVE);
APP_LOG_DEBUG("打开2号仓!"); 
}

static void asyn_turn_off_rb1_2_switch()
{
osTimerStart(rb1_2_turn_off_timer_hdl,RB1_TIMEOUT_VALUE);
BSP_rb1_motor_pwr(BSP_PWR_ON_NEGATIVE); 
APP_LOG_DEBUG("关闭2号仓!"); 
}
static void asyn_pwr_dwn_rb1_switch()
{
 BSP_rb1_motor_pwr(BSP_PWR_DWN);
 APP_LOG_DEBUG("断电1级仓电机!"); 
}

static void asyn_turn_on_rb2_switch()
{
 osTimerStart(rb2_turn_on_timer_hdl,RB2_TIMEOUT_VALUE);
 BSP_rb2_motor_pwr(BSP_PWR_ON_POSITIVE);
 APP_LOG_DEBUG("打开2级仓!"); 
}

static void asyn_turn_off_rb2_switch()
{
 osTimerStart(rb2_turn_off_timer_hdl,RB2_TIMEOUT_VALUE);
 BSP_rb2_motor_pwr(BSP_PWR_ON_NEGATIVE);
 APP_LOG_DEBUG("关闭2级仓!"); 
}
//断电2级米仓电机
static void asyn_pwr_dwn_rb2_switch()
{
BSP_rb2_motor_pwr(BSP_PWR_DWN); 
APP_LOG_DEBUG("断电2级仓电机!"); 
}

static void asyn_turn_on_uv_lamp()
{
 BSP_uv_lump_pwr(BSP_PWR_ON); 
 APP_LOG_DEBUG("打开紫外灯!");  
}
static void asyn_turn_off_uv_lamp()
{
 BSP_uv_lump_pwr(BSP_PWR_DWN);  
 APP_LOG_DEBUG("关闭紫外灯!");   
}

static void asyn_turn_on_e_lamp()
{
BSP_e_lump_pwr(BSP_PWR_ON); 
APP_LOG_DEBUG("打开环境灯!");  
}

static void asyn_turn_off_e_lamp()
{
BSP_e_lump_pwr(BSP_PWR_DWN);  
APP_LOG_DEBUG("关闭环境灯!");  
}

static void asyn_turn_on_oh_door()
{
 osTimerStart(oh_door_turn_on_timer_hdl,OH_DOOR_TIMEOUT_VALUE);
 BSP_oh_door_motor_pwr( BSP_PWR_ON_POSITIVE);
 APP_LOG_DEBUG("打开升降门!");  
}
static void asyn_turn_off_oh_door()
{
 osTimerStart(oh_door_turn_off_timer_hdl,OH_DOOR_TIMEOUT_VALUE);
 BSP_oh_door_motor_pwr( BSP_PWR_ON_POSITIVE); 
 APP_LOG_DEBUG("关闭升降门!");  
}
//断电升降门电机
static void  asyn_pwr_dwn_oh_door()
{
 BSP_oh_door_motor_pwr(BSP_PWR_DWN);
 APP_LOG_DEBUG("断电升降门电机!");  
}
 break; 

static void asyn_enable_remove_tare()
{
 osStatus status;
 status=osMessagePut(ew_queue_hdl,MSG_EW_REMOVE_TARE,0);
 if(status==osOk)
 {
 APP_LOG_DEBUG("使能去皮!");  
 }
else
{
  set_rm_fault_code(FAULT_CODE_EW_NO_RESPONSE); 
  osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT); 
}
}

static void asyn_disable_remove_tare()
{
 APP_LOG_DEBUG("失能去皮!");  
}

static void asyn_enable_zero_clearing()
{
 osStatus status;
 status= osMessagePut(ew_queue_hdl,MSG_EW_CLEARING_ZERO,0);
 if(status==osOk)
 {
 APP_LOG_DEBUG("使能清零!");  
 }
 else
{
  set_rm_fault_code(FAULT_CODE_EW_NO_RESPONSE); 
  osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT); 
}

}

static void asyn_disable_zero_clearing()
{
 APP_LOG_DEBUG("失能清零!");  
}

static asyn_set_ew_threshold_value()
{
osStatus status;
status=osMessagePut(ew_queue_hdl,MSG_EW_SET_RICE_WEIGHT_THRESHOLD,0);
 if(status==osOk)
 {
 APP_LOG_DEBUG("设置定点值:%d!",get_reg_value(EW_THRESHOLD_REGHOLDING_ADDR,2,REGHOLDING_MODE);    
 }
 else
 {
  set_rm_fault_code(FAULT_CODE_EW_NO_RESPONSE); 
  osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT); 
 }

}


static void rice_mill_async_task(void const * argument)
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
 case MSG_TURN_ON_RM:
 asyn_turn_on_rm();  
 break;   
 case MSG_TURN_OFF_RM:
 asyn_turn_off_rm();    
 break;   
 case MSG_RB1_SELECT_NO1:
 asyn_rb1_select_no1();  
 break;   
 case MSG_RB1_SELECT_NO2:
 asyn_rb1_select_no2();    
 break;
 case MSG_SET_RW_VALUE:
 asyn_set_rw_value();    
 break;   
 case MSG_SET_RL_0://复位
 asyn_set_rl_0();  
 break;
 case MSG_SET_RL_5:
 asyn_set_rl_5();    
 break;   
 case MSG_SET_RL_7:
 asyn_set_rl_7();    
 break;
 case MSG_SET_RL_9:
 asyn_set_rl_9();   
 break;   
 case MSG_SET_RM_FAULT_CODE:
 asyn_set_rm_fault_code(); 
 break;
 case MSG_TURN_ON_RM_MOTOR:
 asyn_turn_on_rm_motor();  
 break;   
 case MSG_TURN_OFF_RM_MOTOR:
 asyn_turn_off_rm_motor();    
 break;
 case MSG_TURN_ON_RB1_1_SWITCH:
 asyn_turn_on_rb1_1_switch();    
 break;   
 case MSG_TURN_OFF_RB1_1_SWITCH:
 asyn_turn_off_rb1_1_switch();  
 break;
 case MSG_TURN_ON_RB1_2_SWITCH:
 asyn_turn_on_rb1_2_switch();   
 break;
 case MSG_TURN_OFF_RB1_2_SWITCH:
 asyn_turn_off_rb1_2_switch();  
 break;   
 case MSG_PWR_DWN_RB1_SWITCH://断电1级米仓电机
 asyn_pwr_dwn_rb1_switch();  
 break;  
 case MSG_TURN_ON_RB2_SWITCH:
 asyn_turn_on_rb2_switch();   
 break;
 case MSG_TURN_OFF_RB2_SWITCH:
 asyn_turn_off_rb2_switch();  
 break;   
 case MSG_PWR_DWN_RB2_SWITCH://断电2级米仓电机
 asyn_pwr_dwn_rb2_switch(); 
 break; 
 case MSG_TURN_ON_UV_LAMP:
 asyn_turn_on_uv_lamp();  
 break;   
 case MSG_TURN_OFF_UV_LAMP:
 asyn_turn_off_uv_lamp(); 
 break;
 case MSG_TURN_ON_E_LAMP:
 asyn_turn_on_e_lamp();   
 break;
 case MSG_TURN_OFF_E_LAMP:
 asyn_turn_off_e_lamp(); 
 break;   
 case MSG_TURN_ON_OH_DOOR:
 asyn_turn_on_oh_door();  
 break;
 case MSG_TURN_OFF_OH_DOOR:
 asyn_turn_off_oh_door();  
 break;   
 case MSG_PWR_DWN_OH_DOOR:////断电升降门电机
 asyn_pwr_dwn_oh_door();  
 break; 
 case MSG_ENABLE_REMOVE_TARE:
 asyn_enable_remove_tare();  
 break; 
 case MSG_DISABLE_REMOVE_TARE:
 asyn_disable_remove_tare();  
 break;
 case MSG_ENABLE_ZERO_CLEARING:
 asyn_enable_zero_clearing();   
 break;   
 case MSG_DISABLE_ZERO_CLEARING:
 asyn_disable_zero_clearing(); 
 break; 
 case MSG_SET_EW_THRESHOLD_VALUE:
 asyn_set_ew_threshold_value(); 
 break; 
 case MSG_SETUP_RL:
 asyn_setup_rl(); 
 break; 
 }
 }
 
}

/********************************************************************
碾米机同步执行任务
********************************************************************/

void sync_shutdown()
{
  set_reg_value(RM_EXECUTE_PROGRESS_REGINPUT_ADDR,1,RM_PROGRESS_ERROR,REGINPUT_MODE); //碾米进度错误
  set_reg_value(RM_SWITCH_REGHOLDING_ADDR,1,REG_VALUE_SWITCH_OFF,REGHOLDING_MODE);//碾米开关
  osThreadSuspend(NULL);//挂起自己
}
void sync_busy()
{
 set_reg_value(RM_EXECUTE_PROGRESS_REGINPUT_ADDR,1,RM_PROGRESS_EXECUTING,REGINPUT_MODE); //碾米进度进行中 
}
void sync_complete()
{
  set_reg_value(RM_EXECUTE_PROGRESS_REGINPUT_ADDR,1,RM_PROGRESS_COMPLETED,REGINPUT_MODE); //碾米进度完成
  set_reg_value(RM_SWITCH_REGHOLDING_ADDR,1,REG_VALUE_SWITCH_OFF,REGHOLDING_MODE);//碾米开关
  osThreadSuspend(NULL);//挂起自己  
}

void sync_set_rw()
 {
 uint32_t rm_w;//设置电子秤阈值
 rm_w=get_reg_value( RW_REGHOLDING_ADDR,2,REGHOLDING_MODE);
 set_reg_value(EW_THRESHOLD_REGHOLDING_ADDR,2,rm_w,REGHOLDING_MODE); 
 }
void sync_set_rb1(uint16_t reg_value)
{
   uint16_t rb1;
   rb1=get_reg_value( RB1_SELECTION_REGHOLDING_ADDR,1,REGHOLDING_MODE);
   if(rb1==REG_VALUE_RB1_NO_1_ID)
   set_reg_value(RB1_1_SWITCH_REGHOLDING_ADDR,1,reg_value,REGHOLDING_MODE); 
   else
   set_reg_value(RB1_2_SWITCH_REGHOLDING_ADDR,1,reg_value,REGHOLDING_MODE); 
}

static void sync_set_wait_ew_weight_signal()
{
 //由传感器任务提供所需信号
 osTimerstart() 
}

static void sync_set_rl()
{
 uint16_t rl;
 rl=get_reg_value( RL_REGHOLDING_ADDR,1,REGHOLDING_MODE); 
 set_reg_value(RL_CONTROL_REGHOLDING_ADDR, 1,rl,REGHOLDING_MODE);
}
void sync_set_motor(uint16_t reg_value)
{
 set_reg_value(RM_MOTOR_SWITCH_REGHOLDING_ADDR, 1,reg_value,REGHOLDING_MODE); 
}

void sync_set_rb2(uint16_t reg_value)
{
 set_reg_value(RB2_SWITCH_REGHOLDING_ADDR, 1,reg_value,REGHOLDING_MODE);  
}

void sync_set_oh_door(uint16_t reg_value)
{
 set_reg_value(OH_DOOR_SWITCH_REGHOLDING_ADDR, 1,reg_value,REGHOLDING_MODE);  
}



void rice_mill_sync_task(void const * argument)
{
 static uint8_t cur_rice_weight_obtain_times=0;
 static uint8_t rice_is_take_away=BSP_NO;
 static osEvent ret;
    
while(1)
{
 sync_complete();//等待开始
 sync_busy();
do
{
 sync_set_rl();//调节对应的碾米分度值
 osSignalWait(SYNC_ALL_EVT,0);//clear all signals exsited
 ret=osSignalWait(SYNC_HALT_EVT |SYNC_SET_RL_OK_EVT,osWaitForever);//等待分度调节器到位
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
  sync_shutdown();
}while(!(ret.status == osEventSignal && ret.value.signals & SYNC_SET_RL_OK_EVT));

while(cur_rice_weight_obtain_times != RICE_WEIGHT_OBTAIN_TIMES_EXPIRED)
{
do
{
 sync_set_ew_threshold();//设置出米重量
 osSignalWait(SYNC_ALL_EVT,0);//clear all signals  exsited
 ret=osSignalWait(SYNC_HALT_EVT|SYNC_SET_EW_OK_EVT,osWaitForever );//等待出米重量设置完成
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 sync_shutdown();
 }while(!(ret.status == osEventSignal && ret.value.signals & SYNC_SET_EW_OK_EVT));

 do
 {
 sync_set_rb1(REG_VALUE_SWITCH_ON);//打开对应的1级仓 1号或者2号
 sync_set_wait_ew_weight_signal();//启动米重感知和超时
 sSignalWait(SYNC_ALL_EVT,0);//clear all signals  exsited
 ret=osSignalWait(SYNC_HALT_EVT |SYNC_OBTAIN_RW_OK_EVT,osWaitForever );//等待米重达标,不处理打开到位信号
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 {
  sync_set_rb1(REG_VALUE_SWITCH_OFF);//关闭对应的1级仓 1号或者2号
  sync_shutdown();
 }
 }while(!(ret.status == osEventSignal && ret.value.signals & SYNC_OBTAIN_RW_OK_EVT));
 
do
{
 sync_set_rb1(REG_VALUE_SWITCH_ON);//关闭对应的1级仓 1号或者2号
 osSignalWait(SYNC_ALL_EVT,0);//clear all signals  exsited
 ret=sSignalWait(SYNC_HALT_EVT |SYNC_CLOSE_RB1_OK_EVT,osWaitForever );//等待一级仓关闭到位
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
  sync_shutdown();
}while(!(ret.status == osEventSignal && ret.value.signals & SYNC_CLOSE_RB1_OK_EVT));

do
{
 sync_set_motor(REG_VALUE_SWITCH_ON);//启动碾米电机
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_HALT_EVT,MOTOR_WAIT_EXPIRED_TIMEOUT_VALUE);//等待 1 s 电机稳定转动
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 {
 sync_set_motor(REG_VALUE_SWITCH_OFF);//关闭碾米电机
 rm_mill_shutdown();
 }
}
while(!(ret.status == osEventTimeout));
 
do
{
 sync_set_rb2(REG_VALUE_SWITCH_ON);//打开2级仓门
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_HALT_EVT |SYNC_OPEN_RB2_OK_EVT,osWaitForever );//等待打开2级仓门到位
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 {
  sync_set_motor(REG_VALUE_SWITCH_OFF);//关闭碾米电机
  sync_set_rb2(REG_VALUE_SWITCH_OFF);//关闭2级仓门
  rm_mill_shutdown();
 }
}while(!(ret.status == osEventSignal && ret.value.signals & SYNC_OPEN_RB2_OK_EVT))

do
{
 sync_set_rb2(REG_VALUE_SWITCH_OFF);//关闭2级仓
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_HALT_EVT |SYNC_CLOSE_RB2_OK_EVT,osWaitForever );//等待2级仓关闭到位
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 {
  rm_mill_shutdown();
 }
}while(!(ret.status == osEventSignal && ret.value.signals  & SYNC_CLOSE_RB2_OK_EVT))
 
 cur_rice_weight_obtain_times++;
}
 cur_rice_weight_obtain_times=0;

 do
 {
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_HALT_EVT,MOTOR_RUN_EXPIRED_TIMEOUT_VALUE );//等待碾米时间到
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 rm_mill_shutdown();
 }while(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 
 do
 {
 sync_rm_motor(REG_VALUE_SWITCH_OFF);//关闭碾米电机马达
 sync_set_rb2(REG_VALUE_SWITCH_OFF);//关闭2级仓
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_HALT_EVT |SYNC_CLOSE_RB2_OK_EVT,osWaitForever );//等待2级仓关闭到位
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 rm_mill_shutdown();
 }while(!(ret.status == osEventSignal && ret.value.signals & SYNC_CLOSE_RB2_OK_EVT));
  
 do
 {
 do
 {
 sync_set_oh_door(REG_VALUE_SWITCH_OFF);//打开升降门
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_HALT_EVT |SYNC_OPEN_OH_DOOR_OK_EVT,osWaitForever );//等待门上升到位
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 sync_shutdown();
 }while(!(ret.status == osEventSignal && ret.value.signals & SYNC_OPEN_OH_DOOR_OK_EVT));
   
 sync_set_take_rice_away();//启动米移走感知
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_HALT_EVT |SYNC_TAKE_RICE_AWAY_OK_EVT,RICE_TAKE_AWAY_TIMEOUT_VALUE);//等待米被拿走 
 
 if(ret.status ==osEventSignal && ret.value.signals & SYNC_TAKE_RICE_AWAY_OK_EVT)
  is_rice_take_away= BSP_TRUE;
 else
  is_rice_take_away= BSP_FALSE;
 do
 {
 sync_set_oh_door(REG_VALUE_SWITCH_OFF);//关闭升降门
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_HALT_EVT |SYNC_CLOSE_OH_DOOR_OK_EVT,osWaitForever );
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 sync_shutdown();
 }while(!(ret.status == osEventSignal && ret.value.signals & SYNC_CLOSE_OH_DOOR_OK_EVT));
 
 if(is_rice_take_away == BSP_FALSE)
  sync_shutdown();
 
 }while(rice_is_take_away!=BSP_TRUE);//如果没有取走米 重新启动后，继续开门拿米操作
 
 rice_is_take_away=BSP_FALSE;
 
 }
}

