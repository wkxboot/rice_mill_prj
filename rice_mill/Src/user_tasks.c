/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "app_util.h"
#include "app_fifo.h"
#include "app_error.h"
#include "app_log.h"
#include "mb.h"
#include "mb_m.h"
#include "rm_bsp.h"
#include "mb_slave_reg.h"
#include "mb_slave_msg.h"
#include "rm_fault_code.h"
#include "adc.h"
#include "iwdg.h"
#include "user_tasks.h"

#if APP_LOG_ENABLED > 0    
#undef  APP_LOG_MODULE_NAME 
#undef  APP_LOG_MODULE_LEVEL
#define APP_LOG_MODULE_NAME   "[user_tasks]"
#define APP_LOG_MODULE_LEVEL   APP_LOG_LEVEL_DEBUG    
#endif

/* Variables -----------------------------------------------------------------*/
uint16_t master_get_gross_weight();
uint16_t master_get_net_weight();

//任务id
osThreadId com_with_ew_task_hdl;  //下位机从电子秤接收数据的任务handle;
osThreadId com_with_host_task_hdl;//下位机从主机接收数据的任务handle;

osThreadId sensor_info_task_hdl;  //传感器数据收集任务的handle;
osThreadId ew_func_task_hdl;      //电子秤功能任务的handle;
osThreadId rm_asyn_task_hdl;     //碾米机异步命令解析与分发任务的handle;
osThreadId rm_sync_task_hdl;      //碾米机同步操作任务的handle;
/**************************************************************************/
//消息队列id
osMessageQId ew_msg_queue_hdl;
osMessageQId rm_asyn_msg_queue_hdl;
osMessageQId rm_sync_msg_queue_hdl;

//定时器id
osTimerId MASTER_MB_timerHandle;
osTimerId SLAVE_MB_timerHandle;
osTimerId rb1_1_turn_on_timer_hdl;
osTimerId rb1_2_turn_on_timer_hdl;
osTimerId rb1_turn_off_timer_hdl;
osTimerId rb2_turn_on_timer_hdl;
osTimerId rb2_turn_off_timer_hdl;
osTimerId oh_door_turn_on_timer_hdl;
osTimerId oh_door_turn_off_timer_hdl;
osTimerId rl_timer_hdl;
osTimerId ew_timer_hdl;

extern uint16_t bsp_adc_result[BSP_ADC_CONVERT_NUM];
extern IWDG_HandleTypeDef hiwdg;

void communication_with_ew_task(void const * argument);
void communication_with_host_task(void const * argument);
void sensor_info_task(void const * argument);
void ew_func_task(void const * argument);
void rice_mill_asyn_task(void const * argument);
void rice_mill_sync_task(void const * argument);
void tasks_environment_init();




void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
  APP_LOG_ERROR("%s: STACK OVERFLOW!\r\n",pcTaskName );
  UNUSED_PARAMETER(xTask);
}
void vApplicationMallocFailedHook(void)
{
  APP_LOG_ERROR("APP MALLOC FAILED!\r\n" );
}



//创建用户任务
 void app_create_user_tasks(void)
 {
  tasks_environment_init();
  
  
  osThreadDef(communication_with_ew_task, communication_with_ew_task, osPriorityNormal, 0, 128);
  com_with_ew_task_hdl = osThreadCreate(osThread(communication_with_ew_task), NULL);
  
  osThreadDef(communication_with_host_task, communication_with_host_task, osPriorityNormal, 0, 128);
  com_with_host_task_hdl = osThreadCreate(osThread(communication_with_host_task), NULL);
  
  osThreadDef(sensor_info_task, sensor_info_task, osPriorityNormal, 0, 128);
  sensor_info_task_hdl = osThreadCreate(osThread(sensor_info_task), NULL);
 
  osThreadDef(ew_func_task, ew_func_task, osPriorityNormal, 0, 128);
  ew_func_task_hdl = osThreadCreate(osThread(ew_func_task), NULL);
  
  osThreadDef(rice_mill_asyn_task, rice_mill_asyn_task, osPriorityNormal, 0, 128);
  rm_asyn_task_hdl = osThreadCreate(osThread(rice_mill_asyn_task), NULL);
 
  osThreadDef(rice_mill_sync_task, rice_mill_sync_task, osPriorityNormal, 0, 128);
  rm_sync_task_hdl = osThreadCreate(osThread(rice_mill_sync_task), NULL);
 
 }

/*
 *定时器任务回调
 */
void MASTER_MB_timer_expired_callback(void const * argument);
void SLAVE_MB_timer_expired_callback(void const * argument);

void rb1_1_turn_on_timer_callback(void const * argument)
{
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT); 
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RB1,0);
 APP_LOG_DEBUG("rb1_1 turn on timeout!\r\n");
}
void rb1_2_turn_on_timer_callback(void const * argument)
{
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT); 
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RB2,0);
 APP_LOG_DEBUG("rb1_2 turn on timeout!\r\n");
}
void rb1_turn_off_timer_callback(void const * argument)
{
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT); 
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RB1,0);
 APP_LOG_DEBUG("rb1 turn off timeout!\r\n"); 
}
void rb2_turn_on_timer_callback(void const * argument)
{
 osSignalSet( rm_sync_task_hdl,SYNC_OPEN_RB2_OK_EVT);//二级仓通过时间确定 特殊处理
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RB2,0);
 APP_LOG_DEBUG("rb2 turn on OK!\r\n"); 
}
void rb2_turn_off_timer_callback(void const * argument)
{
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RB2,0);
 APP_LOG_DEBUG("rb2 turn off timeout!\r\n");
}
void oh_door_turn_on_timer_callback(void const * argument)
{
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT); 
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_OH_DOOR,0);
 APP_LOG_DEBUG("oh door turn on timeout!\r\n"); 
}
void oh_door_turn_off_timer_callback(void const * argument)
{
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT); 
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_OH_DOOR,0);
 APP_LOG_DEBUG("oh door turn off timeout!\r\n"); 
}
void rl_timer_callback(void const * argument)
{
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT); 
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RL,0);
 APP_LOG_DEBUG("rl opt timeout!\r\n");
}
void ew_timer_callback(void const * argument)
{
 osSignalSet(rm_sync_task_hdl,SYNC_FAULT_EVT); 

 APP_LOG_DEBUG("ew opt timeout!\r\n"); 
}


static void tasks_environment_init()
{
  APP_LOG_DEBUG("tasks_environment_init!\r\n");
  /* Create the queue(s) */

  /* definition and creation of ew_queue_hdl */
  osMessageQDef(ew_msg_queue, 16, uint16_t);
  ew_msg_queue_hdl = osMessageCreate(osMessageQ(ew_msg_queue), NULL); 
  /* definition and creation of ew_queue_hdl */
  osMessageQDef(rm_asyn_msg_queue, 16, uint16_t);
  rm_asyn_msg_queue_hdl = osMessageCreate(osMessageQ(rm_asyn_msg_queue), NULL); 
   /* definition and creation of ew_queue_hdl */
  osMessageQDef(rm_sync_msg_queue, 16, uint16_t);
  rm_sync_msg_queue_hdl = osMessageCreate(osMessageQ(rm_sync_msg_queue), NULL); 
  
  
  /* Create the timer(s) */
  /* definition and creation of MASTER_MB_timer */
  osTimerDef(MASTER_MB_timer, MASTER_MB_timer_expired_callback);
  MASTER_MB_timerHandle = osTimerCreate(osTimer(MASTER_MB_timer), osTimerOnce, NULL);
  /* definition and creation of SLAVE_MB_timer */
  osTimerDef(SLAVE_MB_timer, SLAVE_MB_timer_expired_callback);
  SLAVE_MB_timerHandle = osTimerCreate(osTimer(SLAVE_MB_timer), osTimerOnce, NULL);
  
  /* definition and creation of rb1_1_turn_on_timer */
  osTimerDef(rb1_1_turn_on_timer, rb1_1_turn_on_timer_callback);
  rb1_1_turn_on_timer_hdl = osTimerCreate(osTimer(rb1_1_turn_on_timer), osTimerOnce, NULL);
  /* definition and creation of rb1_2_turn_on_timer */
  osTimerDef(rb1_2_turn_on_timer, rb1_2_turn_on_timer_callback);
  rb1_2_turn_on_timer_hdl = osTimerCreate(osTimer(rb1_2_turn_on_timer), osTimerOnce, NULL);
  /* definition and creation of rb1_turn_off_timer */
  osTimerDef(rb1_turn_off_timer, rb1_turn_off_timer_callback);
  rb1_turn_off_timer_hdl = osTimerCreate(osTimer(rb1_turn_off_timer), osTimerOnce, NULL);
  /* definition and creation of rb2_turn_on_timer */
  osTimerDef(rb2_turn_on_timer, rb2_turn_on_timer_callback);
  rb2_turn_on_timer_hdl = osTimerCreate(osTimer(rb2_turn_on_timer), osTimerOnce, NULL);
  /* definition and creation of rb2_turn_off_timer */
  osTimerDef(rb2_turn_off_timer, rb2_turn_off_timer_callback);
  rb2_turn_off_timer_hdl = osTimerCreate(osTimer(rb2_turn_off_timer), osTimerOnce, NULL);
   /* definition and creation of oh_door_turn_on_timer */
  osTimerDef(oh_door_turn_on_timer, oh_door_turn_on_timer_callback);
  oh_door_turn_on_timer_hdl = osTimerCreate(osTimer(oh_door_turn_on_timer), osTimerOnce, NULL);
  /* definition and creation of oh_door_turn_on_timer */
  osTimerDef(oh_door_turn_off_timer, oh_door_turn_off_timer_callback);
  oh_door_turn_off_timer_hdl = osTimerCreate(osTimer(oh_door_turn_off_timer), osTimerOnce, NULL);
  /* definition and creation of oh_door_turn_on_timer */
  osTimerDef(rl_timer, rl_timer_callback);
  rl_timer_hdl = osTimerCreate(osTimer(rl_timer), osTimerOnce, NULL);
  /* definition and creation of oh_door_turn_on_timer */
  osTimerDef(ew_timer, ew_timer_callback);
  ew_timer_hdl = osTimerCreate(osTimer(ew_timer), osTimerOnce, NULL);
  
}

/******************************************************************
 * 工控机轮询任务
 ******************************************************************/

void communication_with_host_task(void const * argument)
{
   APP_LOG_DEBUG("communication_with_host_task START!\r\n");

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
/**********************************************************************
 * 电子秤轮询任务
 **********************************************************************/
void communication_with_ew_task(void const * argument)
{
   APP_LOG_DEBUG("communication_with_ew_task START !\r\n");
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

/************************************************************************
 *  电子秤功能任务
 ************************************************************************/
void ew_func_task(void const * argument)
{
 osEvent ew_msg;
 uint16_t reg_value;
 uint32_t threshold;
 uint16_t temp[2];
 uint16_t rice_gross_weight;
 uint16_t rice_net_weight;
 eMBMasterReqErrCode err_code;
 
 APP_LOG_DEBUG("ew_func_task START!\r\n");
 
 while(1)
 {
 ew_msg=osMessageGet(ew_msg_queue_hdl,osWaitForever);
 if(ew_msg.status != osEventMessage)
 continue;
 err_code=MB_MRE_NO_ERR;
 switch(ew_msg.value.v)
 {
 case MSG_EW_OBTAIN_RICE_GROSS_WEIGHT://毛重
 do
 {
 err_code=eMBMasterReqReadHoldingRegister(SLAVE_EW_ADDR,REG_GROSS_WEIGHT_ADDR,1,EW_GET_RESOURCE_TIMEOUT);
 if(err_code==MB_MRE_NO_ERR)
 {
  rice_gross_weight=master_get_gross_weight(); 
  set_reg_value(EW_GROSS_WEIGHT_REGINPUT_ADDR,1, rice_gross_weight,REGINPUT_MODE);
 }
 }while(err_code!=MB_MRE_NO_ERR);
 break;
 case MSG_EW_OBTAIN_RICE_NET_WEIGHT://净重
 do
 {
 err_code=eMBMasterReqReadHoldingRegister(SLAVE_EW_ADDR,REG_NET_WEIGHT_ADDR,1,EW_GET_RESOURCE_TIMEOUT);
 if(err_code==MB_MRE_NO_ERR)
 {
   rice_net_weight= master_get_net_weight();
   set_reg_value(EW_NET_WEIGHT_REGINPUT_ADDR,1, rice_net_weight,REGINPUT_MODE);
 }
 }while(err_code!=MB_MRE_NO_ERR);
 break;
 case MSG_EW_SET_RICE_WEIGHT_THRESHOLD://设置定点值
 do
 {
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
  osTimerStop(ew_timer_hdl);
 }
 }while(err_code!=MB_MRE_NO_ERR);
 break;
 case MSG_EW_REMOVE_TARE://去皮重
 do
 {
 reg_value=0x0002;
 err_code=eMBMasterReqWriteHoldingRegister( SLAVE_EW_ADDR,REG_REMOVE_TARE_ADDR, reg_value, EW_GET_RESOURCE_TIMEOUT);
  if(err_code==MB_MRE_NO_ERR)
 {
  osSignalSet( rm_sync_task_hdl,SYNC_SET_EW_OK_EVT);
  osTimerStop(ew_timer_hdl);
 }
 }while(err_code!=MB_MRE_NO_ERR);
 break;
 case MSG_EW_CLEARING_ZERO://清零
do
{
 reg_value=0x0001;
 err_code=eMBMasterReqWriteHoldingRegister( SLAVE_EW_ADDR,REG_CLEARING_ZERO_ADDR, reg_value, EW_GET_RESOURCE_TIMEOUT);
 if(err_code==MB_MRE_NO_ERR)
 {
  osSignalSet( rm_sync_task_hdl,SYNC_SET_EW_OK_EVT);
  osTimerStop(ew_timer_hdl);
 }
}while(err_code!=MB_MRE_NO_ERR);
 break; 
  }
 } 
}

void sensor_info_task(void const * argument)
{
uint16_t t,rh;
uint16_t rw_cur,rw_tar,oh_door_state;
osEvent ret;
static uint8_t rw_obtain_interval;

while(1)
{
/* 温度传感器 channel4 */
t=BSP_get_temperature();
if(TEM_MAX >=t && t> TEM_MIN)
{ 
  set_reg_value(RB1_1_TEMPERATURE_REGINPUT_ADDR,1, t,REGINPUT_MODE);
  set_reg_value(RB1_2_TEMPERATURE_REGINPUT_ADDR,1, t,REGINPUT_MODE);
  if(t > TEM_WARNING)
  {
  set_rm_fault_code(FAULT_CODE_RB1_OT);
  set_rm_fault_code(FAULT_CODE_RB1_OT);
  /*
  ret=osSignalWait(SENSOR_CHECK_RB_T_EVT,0);//是否需要检查温度过高错误，需要就发送错误信号
  if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_RB_T_EVT)//收到需要检查错误的信号，发送错误信号给同步碾米机
  */  
  osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
  }
}
/*湿度传感器 channel5 */
rh=BSP_get_relative_humidity();
if(RH_MAX >=rh || rh >  RH_MIN)
{
 set_reg_value( RB1_1_RH_REGINPUT_ADDR,1, rh,REGINPUT_MODE);
 set_reg_value( RB1_2_RH_REGINPUT_ADDR,1, rh,REGINPUT_MODE);
 if(rh > RH_WARNING)
 {
  set_rm_fault_code(FAULT_CODE_RB1_OTH); 
  set_rm_fault_code(FAULT_CODE_RB2_OTH);
  /*
  ret=osSignalWait(SENSOR_CHECK_RB_RH_EVT,0);
  if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_RB_RH_EVT)//检查相对湿度
  */
  osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
 }
}
/*24v 传感器 channel8 */

if(BSP_is_24v_oc() == BSP_TRUE)
{
 set_rm_fault_code(FAULT_CODE_BOARD_OC);
 ret=osSignalWait(SENSOR_CHECK_24V_OC_EVT,0);
 if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_24V_OC_EVT)
 {
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT); 
 //关闭所有24v供电
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RB1,0);
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RB2,0);
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RM_MOTOR,0);
 }
}

/*升降门传感器 channel9*/
if(BSP_is_oh_door_oc()== BSP_TRUE)
{
 set_rm_fault_code(FAULT_CODE_OH_DOOR_MOTOR_OC); 
 ret=osSignalWait(SENSOR_CHECK_OH_DOOR_OC_EVT,0);
 if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_OH_DOOR_OC_EVT)
 {
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_OH_DOOR,0);
 }
}
/*碾米电机传感器 channel6*/
if(BSP_is_rm_motor_oc()== BSP_TRUE)
{
 set_rm_fault_code(FAULT_CODE_OH_DOOR_MOTOR_OC);
 ret=osSignalWait(SENSOR_CHECK_RM_MOTOR_OC_EVT,0);
 if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_RM_MOTOR_OC_EVT)
 {
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RM_MOTOR,0); 
 }
}
/*交流2传感器 channel7*/


/*步进电机BEMF传感器channel14*/
if(BSP_is_bemf_oc()== BSP_TRUE)
{
 set_rm_fault_code(FAULT_CODE_RL_MOTOR_OC); 
 ret=osSignalWait(SENSOR_CHECK_BEMF_OC_EVT,0);
 if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_BEMF_OC_EVT)
 {
 osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RL,0); 
 }
}

/*1号米仓空检查*/

if(BSP_is_rb1_1_empty()==BSP_TRUE)
{
  set_rm_fault_code(FAULT_CODE_RB1_1_EMPTY);
    /*
  ret=osSignalWait(SENSOR_CHECK_RB1_1_EMPTY_EVT,0);
  if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_RB1_1_EMPTY_EVT)
  */
  osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
}
 /*2号米仓空检查*/

if(BSP_is_rb1_2_empty()==BSP_TRUE)
{
  set_rm_fault_code(FAULT_CODE_RB1_2_EMPTY);
   /*
  ret=osSignalWait(SENSOR_CHECK_RB1_2_EMPTY_EVT,0);
  if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_RB1_2_EMPTY_EVT)
  */
  osSignalSet( rm_sync_task_hdl,SYNC_FAULT_EVT);
}

/*1级米仓阀门电机位置检查*/

if(BSP_is_rb1_no1_turn_on()==BSP_TRUE)
{
  ret=osSignalWait(SENSOR_CHECK_RB1_1_TURN_ON_EVT,0);
  if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_RB1_1_TURN_ON_EVT)
  {
  osTimerStop(rb1_1_turn_on_timer_hdl);
  osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RB1,0);
  osSignalSet( rm_sync_task_hdl,SYNC_OPEN_RB1_OK_EVT);
  }
}
 if(BSP_is_rb1_no2_turn_on()==BSP_TRUE)
 {
 osTimerStop(rb1_2_turn_on_timer_hdl);
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RB1,0);
 osSignalSet( rm_sync_task_hdl,SYNC_OPEN_RB1_OK_EVT);
 }
 if(BSP_is_rb1_turn_off()==BSP_TRUE)
 {
   ret=osSignalWait(SENSOR_CHECK_RB1_2_TURN_ON_EVT,0);
  if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_RB1_2_TURN_ON_EVT)
  {
  osTimerStop(rb1_turn_off_timer_hdl);
  osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RB1,0);
  osSignalSet( rm_sync_task_hdl,SYNC_CLOSE_RB1_OK_EVT);
  }
 }

/*2级仓阀门位置检测 目前由时间估算，没有位置检测*/
/*
if(BSP_is_rb2_turn_on()==BSP_TRUE)
{
  ret=osSignalWait(SENSOR_CHECK_RB2_TURN_ON_EVT,0);
  if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_RB2_TURN_ON_EVT)
 {
  osTimerStop(rb2_turn_on_timer_hdl);
  osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RB2,0);
  osSignalSet( rm_sync_task_hdl,SYNC_OPEN_RB2_OK_EVT);
 }
}
*/
if(BSP_is_rb2_turn_off()==BSP_TRUE)
{
   ret=osSignalWait(SENSOR_CHECK_RB2_TURN_OFF_EVT,0);
  if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_RB2_TURN_OFF_EVT)
  {
  osTimerStop(rb2_turn_off_timer_hdl);
  osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RB2,0);
  osSignalSet( rm_sync_task_hdl,SYNC_CLOSE_RB2_OK_EVT);
  }
}

/*升降门位置检测*/
if(BSP_is_oh_door_turn_on())
{
 ret=osSignalWait(SENSOR_CHECK_OH_DOOR_TURN_ON_EVT,0);
 if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_OH_DOOR_TURN_ON_EVT)
 {
 osTimerStop(oh_door_turn_on_timer_hdl);
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_OH_DOOR,0);
 osSignalSet( rm_sync_task_hdl,SYNC_OPEN_OH_DOOR_OK_EVT); 
 }
}

if(BSP_is_oh_door_turn_off())
{
 ret=osSignalWait(SENSOR_CHECK_OH_DOOR_TURN_OFF_EVT,0);
 if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_OH_DOOR_TURN_OFF_EVT)
 {
 osTimerStop(oh_door_turn_off_timer_hdl);
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_OH_DOOR,0);
 osSignalSet( rm_sync_task_hdl,SYNC_CLOSE_OH_DOOR_OK_EVT);
 }
}
/*
 ret=osSignalWait(SENSOR_CHECK_OH_DOOR_PATHWAY_EVT,0);
 if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_OH_DOOR_PATHWAY_EVT)
*/
oh_door_state=get_reg_value(OH_DOOR_SWITCH_REGHOLDING_ADDR,1,REGHOLDING_MODE);                          
if(!BSP_is_oh_door_pathway_ok() && oh_door_state==REG_VALUE_SWITCH_OFF && !BSP_is_oh_door_turn_off() )
{
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_OH_DOOR,0);
}
else if(BSP_is_oh_door_pathway_ok() && oh_door_state==REG_VALUE_SWITCH_OFF && !BSP_is_oh_door_turn_off())
{
 osMessagePut(rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_OH_DOOR,0); 
}
                      
/** 分度调节器***/
if( BSP_is_rl_in_rst_pos()==BSP_TRUE && BSP_rl_get_motor_tar_steps()==BSP_RL_MOTOR_STEPS_FOR_RL0)
{
  if(BSP_rl_get_motor_cur_steps()!=BSP_RL_MOTOR_STEPS_FOR_RL0)
  {
    taskENTER_CRITICAL();
    BSP_rl_reset_steps();
    taskEXIT_CRITICAL();   
  //stop_pwm
  }
}

if( BSP_is_rl_in_tar_pos()==BSP_TRUE)
{
  ret=osSignalWait(SENSOR_CHECK_RL_ON_TAR_POS_EVT,0);
 if(ret.status == osEventSignal && ret.value.signals & SENSOR_CHECK_RL_ON_TAR_POS_EVT)
 {
 osTimerStop(rl_timer_hdl);
 osSignalSet( rm_sync_task_hdl,SYNC_SET_RL_OK_EVT); 
 }
   
}
/*实时米重 发送信号 开始读取电子秤净重重*/
rw_obtain_interval++;
if(rw_obtain_interval==RW_OBTAIN_INTERVAL)
{
osMessagePut(ew_msg_queue_hdl,MSG_EW_OBTAIN_RICE_NET_WEIGHT,0);
rw_obtain_interval=0;
}

 rw_cur=get_reg_value(EW_NET_WEIGHT_REGINPUT_ADDR,1,REGINPUT_MODE);
 rw_tar=get_reg_value(RW_REGHOLDING_ADDR,1,REGHOLDING_MODE)/RICE_WEIGHT_OBTAIN_TIMES_EXPIRED;
 if(BSP_is_ew_signal_ok() && rw_cur >= rw_tar)
 {
 ret=osSignalWait(SYNC_OBTAIN_RW_OK_EVT,0);
 if(ret.status == osEventSignal && ret.value.signals & SYNC_OBTAIN_RW_OK_EVT) 
 osSignalSet( rm_sync_task_hdl,SYNC_OBTAIN_RW_OK_EVT);
 }
 
/*************** 堵转电压获取*********************/ 
  HAL_ADC_Start_DMA(&hadc1 ,(uint32_t*)bsp_adc_result,BSP_ADC_CONVERT_NUM);  
//喂狗
  HAL_IWDG_Refresh(&hiwdg);
//睡眠  
  osDelay(SENSOR_CHECK_TIMEOUT_VALUE);
 }

}
                                                                          



/*******************************************************************************
 * 碾米机动作异步任务
 * 
 *******************************************************************************/
static void asyn_start_rm()
{
  osStatus status;
  if(osThreadIsSuspended( rm_sync_task_hdl)==osOK)
  {
   status=osThreadResume(rm_sync_task_hdl);
   if(status==osOK)
   {
    APP_LOG_DEBUG("excute resume rm task successed!\r\n");
   }
   else
   {
    APP_LOG_DEBUG("excute resume rm task failed!\r\n"); 
   }
  }
}

void asyn_stop_rm()
{  
  osStatus status;
  if(osThreadIsSuspended( rm_sync_task_hdl)!=osOK)
  {
   status=osThreadSuspend (rm_sync_task_hdl);
   if(status==osOK)
   {
    APP_LOG_DEBUG("excute suspend rm task successed!\r\n");
   }
   else
   {
    APP_LOG_DEBUG("excute suspend rm task failed!\r\n");
   }
  }
}

static void asyn_rb1_select_no1()
{
 APP_LOG_DEBUG("excute select no1!\r\n"); 
}
static void asyn_rb1_select_no2()
{
 APP_LOG_DEBUG("excute select no2!\r\n"); 
}

static void asyn_set_rw_value()
{
 APP_LOG_DEBUG("excute set rw value!\r\n"); 
}

static void asyn_setup_rl()
{
APP_LOG_DEBUG("excute set rl value!\r\n"); 
}
     
static void asyn_set_rl_0()
{
 BSP_rl_go_to_pos(BSP_RL_MOTOR_STEPS_FOR_RL0);
 osTimerStart(rl_timer_hdl,RL_TIMEOUT_VALUE);
 APP_LOG_DEBUG("excute set rl 0!");  
}

static void asyn_set_rl_5()
{
 BSP_rl_go_to_pos(BSP_RL_MOTOR_STEPS_FOR_RL5);
 osTimerStart(rl_timer_hdl,RL_TIMEOUT_VALUE);
 APP_LOG_DEBUG("excute set rl 5!");  
}
static void asyn_set_rl_7()
{
BSP_rl_go_to_pos(BSP_RL_MOTOR_STEPS_FOR_RL7);
osTimerStart(rl_timer_hdl,RL_TIMEOUT_VALUE);
APP_LOG_DEBUG("excute set rl 7!\r\n");  
}

static void asyn_set_rl_9()
{
BSP_rl_go_to_pos(BSP_RL_MOTOR_STEPS_FOR_RL9);
osTimerStart(rl_timer_hdl,RL_TIMEOUT_VALUE);
APP_LOG_DEBUG("excute set rl 9!");  
}

static void asyn_set_rl_stop()
{
 BSP_rl_go_to_pos(BSP_RL_MOTOR_STEPS_FOR_STOP);
 APP_LOG_DEBUG("excute SET RL STOP!\r\n");  
}

static void asyn_set_rm_fault_code()
{
 APP_LOG_DEBUG("excute set fault code !\r\n");  
}

static void asyn_pwr_on_rm_motor()
{
 BSP_rm_motor_pwr(BSP_PWR_ON);
 osSignalSet(sensor_info_task_hdl,SENSOR_CHECK_24V_OC_EVT|SENSOR_CHECK_RM_MOTOR_OC_EVT);
 APP_LOG_DEBUG("excute pwr on rm motor!\r\n"); 
}

static void asyn_pwr_dwn_rm_motor()
{
  BSP_rm_motor_pwr(BSP_PWR_DWN);
  APP_LOG_DEBUG("excute pwr dwn rm motor!\r\n"); 
}

static void asyn_turn_on_rb1_1_switch()
{
BSP_rb1_motor_pwr(BSP_PWR_ON_NEGATIVE);
osSignalSet(sensor_info_task_hdl,SENSOR_CHECK_RB1_1_TURN_ON_EVT|SENSOR_CHECK_24V_OC_EVT);
osTimerStart(rb1_1_turn_on_timer_hdl,RB1_TIMEOUT_VALUE);

APP_LOG_DEBUG("excute open no 1\r\n!"); 
}

static void asyn_turn_off_rb1_1_switch()
{
BSP_rb1_motor_pwr(BSP_PWR_ON_POSITIVE);
osSignalSet(sensor_info_task_hdl,SENSOR_CHECK_RB1_TURN_OFF_EVT|SENSOR_CHECK_24V_OC_EVT);
osTimerStart(rb1_turn_off_timer_hdl,RB1_TIMEOUT_VALUE);
APP_LOG_DEBUG("excute close no 1\r\n!"); 
}

static void asyn_turn_on_rb1_2_switch()
{
BSP_rb1_motor_pwr(BSP_PWR_ON_POSITIVE);
osSignalSet(sensor_info_task_hdl,SENSOR_CHECK_RB1_2_TURN_ON_EVT|SENSOR_CHECK_24V_OC_EVT);
osTimerStart(rb1_2_turn_on_timer_hdl,RB1_TIMEOUT_VALUE);

APP_LOG_DEBUG("excute open no 2\r\n!"); 
}

static void asyn_turn_off_rb1_2_switch()
{
BSP_rb1_motor_pwr(BSP_PWR_ON_NEGATIVE); 
osTimerStart(rb1_turn_off_timer_hdl,RB1_TIMEOUT_VALUE);
osSignalSet(sensor_info_task_hdl,SENSOR_CHECK_RB1_TURN_OFF_EVT|SENSOR_CHECK_24V_OC_EVT);
APP_LOG_DEBUG("excute close no 2\r\n!"); 
}
static void asyn_pwr_dwn_rb1()
{
 BSP_rb1_motor_pwr(BSP_PWR_DWN);
 APP_LOG_DEBUG("excute pwr dwn rb1!\r\n"); 
}

static void asyn_turn_on_rb2_switch()
{
 BSP_rb2_motor_pwr(BSP_PWR_ON_POSITIVE);
 osTimerStart(rb2_turn_on_timer_hdl,RB2_TIMEOUT_VALUE);
 osSignalSet(sensor_info_task_hdl,SENSOR_CHECK_24V_OC_EVT);
 APP_LOG_DEBUG("excute turn on rb2!\r\n"); 
}

static void asyn_turn_off_rb2_switch()
{
 BSP_rb2_motor_pwr(BSP_PWR_ON_NEGATIVE);
 osTimerStart(rb2_turn_off_timer_hdl,RB2_TIMEOUT_VALUE);
 osSignalSet(sensor_info_task_hdl,SENSOR_CHECK_RB2_TURN_OFF_EVT|SENSOR_CHECK_24V_OC_EVT);
 APP_LOG_DEBUG("excute turn off rb2!\r\n"); 
}
//断电2级米仓电机
static void asyn_pwr_dwn_rb2()
{
BSP_rb2_motor_pwr(BSP_PWR_DWN); 
APP_LOG_DEBUG("excute pwr dwn rb2!"); 
}

static void asyn_pwr_on_uv_lamp()
{
 BSP_uv_lump_pwr(BSP_PWR_ON); 
 APP_LOG_DEBUG("excute pwr on uv lamp!\r\n");  
}
static void asyn_pwr_dwn_uv_lamp()
{
 BSP_uv_lump_pwr(BSP_PWR_DWN);  
 APP_LOG_DEBUG("excute pwr dwn uv lamp!\r\n");   
}

static void asyn_pwr_on_adv_lamp()
{
BSP_adv_lump_pwr(BSP_PWR_ON); 
APP_LOG_DEBUG("excute open adv lamp!\r\n");  
}

static void asyn_pwr_dwn_adv_lamp()
{
BSP_adv_lump_pwr(BSP_PWR_DWN);  
APP_LOG_DEBUG("excute close adv lamp!\r\n");  
}

static void asyn_turn_on_oh_door()
{
 BSP_oh_door_motor_pwr( BSP_PWR_ON_POSITIVE);
 osTimerStart(oh_door_turn_on_timer_hdl,OH_DOOR_TIMEOUT_VALUE);
 osSignalSet(sensor_info_task_hdl,SENSOR_CHECK_OH_DOOR_TURN_ON_EVT|SENSOR_CHECK_24V_OC_EVT);
 APP_LOG_DEBUG("excute turn on oh door!\r\n");  
}
static void asyn_turn_off_oh_door()
{
 BSP_oh_door_motor_pwr( BSP_PWR_ON_NEGATIVE); 
 osTimerStart(oh_door_turn_off_timer_hdl,OH_DOOR_TIMEOUT_VALUE);
 osSignalSet(sensor_info_task_hdl,SENSOR_CHECK_OH_DOOR_TURN_OFF_EVT|SENSOR_CHECK_24V_OC_EVT);
 APP_LOG_DEBUG("excute turn off oh door!\r\n");  
}
//断电升降门电机
static void  asyn_pwr_dwn_oh_door()
{
 BSP_oh_door_motor_pwr(BSP_PWR_DWN);
 APP_LOG_DEBUG("excute pwr dwn oh door!\r\n");  
}


static void asyn_enable_remove_tare()
{
 osMessagePut(ew_msg_queue_hdl,MSG_EW_REMOVE_TARE,0);
 osTimerStart(ew_timer_hdl,EW_TIMEOUT_VALUE);
 APP_LOG_DEBUG("excute enable remove tare!\r\n");  
}

static void asyn_disable_remove_tare()
{
 APP_LOG_DEBUG("excute disable remove tare!\r\n");  
}

static void asyn_enable_zero_clearing()
{
 osMessagePut(ew_msg_queue_hdl,MSG_EW_CLEARING_ZERO,0);
 osTimerStart(ew_timer_hdl,EW_TIMEOUT_VALUE);
 APP_LOG_DEBUG("excute enable clear 0!\r\n");  

}

static void asyn_disable_zero_clearing()
{
 APP_LOG_DEBUG("excute disable clear 0!\r\n");  
}

static void asyn_set_ew_threshold_value()
{

 osMessagePut(ew_msg_queue_hdl,MSG_EW_SET_RICE_WEIGHT_THRESHOLD,0);
 osTimerStart(ew_timer_hdl,EW_TIMEOUT_VALUE);
 APP_LOG_DEBUG("excute set ew threshold:%d!\r\n",get_reg_value(EW_THRESHOLD_REGHOLDING_ADDR,2,REGHOLDING_MODE));    
}
               
/*交流电1*/
static void  asyn_pwr_on_ac_fan1()
{
 BSP_ac_fan1_pwr(BSP_PWR_ON); 
 APP_LOG_DEBUG("excute pwr on ac fan1!\r\n");  
}
static void  asyn_pwr_dwn_ac_fan1()
{
 BSP_ac_fan1_pwr(BSP_PWR_DWN); 
 APP_LOG_DEBUG("excute pwr dwn ac fan1!\r\n"); 
}
/*交流电2*/
static void  asyn_pwr_on_ac_fan2()
{
BSP_ac_fan2_pwr(BSP_PWR_ON);
APP_LOG_DEBUG("excute pwr on ac fan2!\r\n"); 
}
static void  asyn_pwr_dwn_ac_fan2()
{
BSP_ac_fan2_pwr(BSP_PWR_DWN);
APP_LOG_DEBUG("excute pwr dwn ac fan2!\r\n"); 
}

static void asyn_pwr_on_bl()
{
 APP_LOG_DEBUG("excute pwr on bl!\r\n"); 
}
static void asyn_pwr_dwn_bl()
{
 APP_LOG_DEBUG("excute pwr dwn bl!\r\n");  
}  

static void rice_mill_asyn_task(void const * argument)
{
 osEvent event;
 APP_LOG_DEBUG("rice_mill_asyn_task START!\r\n");
 while(1)
 {
 event= osMessageGet(rm_asyn_msg_queue_hdl,1000);
 if(event.status!=osEventMessage)
 {
   continue ;
 }
  //continue;//调试点
 switch(event.value.v)
 {
 case MSG_START_RM:
 asyn_start_rm();  
 break;   
 case MSG_STOP_RM:
 asyn_stop_rm();    
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
 case MSG_PWR_DWN_RL:
 asyn_set_rl_stop();   
 break;  
 case MSG_SET_RM_FAULT_CODE:
 asyn_set_rm_fault_code(); 
 break;
 case MSG_PWR_ON_RM_MOTOR:
 asyn_pwr_on_rm_motor();  
 break;   
 case MSG_PWR_DWN_RM_MOTOR://断电碾米电机
 asyn_pwr_dwn_rm_motor();    
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
 case MSG_PWR_DWN_RB1://断电1级米仓电机
 asyn_pwr_dwn_rb1();  
 break;  
 case MSG_TURN_ON_RB2_SWITCH:
 asyn_turn_on_rb2_switch();   
 break;
 case MSG_TURN_OFF_RB2_SWITCH:
 asyn_turn_off_rb2_switch();  
 break;   
 case MSG_PWR_DWN_RB2://断电2级米仓电机
 asyn_pwr_dwn_rb2(); 
 break; 
 case MSG_PWR_ON_UV_LAMP:
 asyn_pwr_on_uv_lamp();  
 break;   
 case MSG_PWR_DWN_UV_LAMP:
 asyn_pwr_dwn_uv_lamp(); 
 break;
 case MSG_PWR_ON_ADV_LAMP:
 asyn_pwr_on_adv_lamp();   
 break;
 case MSG_PWR_DWN_ADV_LAMP:
 asyn_pwr_dwn_adv_lamp(); 
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
 case MSG_SETUP_RL_VALUE:
 asyn_setup_rl(); 
 break; 
 case MSG_PWR_ON_AC_FAN1:
 asyn_pwr_on_ac_fan1(); 
 break; 
 case MSG_PWR_DWN_AC_FAN1:
 asyn_pwr_dwn_ac_fan1(); 
 break; 
 case MSG_PWR_ON_AC_FAN2:
 asyn_pwr_on_ac_fan2(); 
 break; 
 case MSG_PWR_DWN_AC_FAN2:
 asyn_pwr_dwn_ac_fan2(); 
 break; 
 case MSG_PWR_ON_BL:
 asyn_pwr_on_bl(); 
 break; 
 case MSG_PWR_DWN_BL:
 asyn_pwr_dwn_bl(); 
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

void sync_set_ew_threshold()
 {
 uint32_t rm_w;//设置电子秤阈值
 rm_w=get_reg_value( RW_REGHOLDING_ADDR,2,REGHOLDING_MODE);
 set_reg_value(EW_THRESHOLD_REGHOLDING_ADDR,2,rm_w,REGHOLDING_MODE);
 osTimerStart(ew_timer_hdl,EW_TIMEOUT_VALUE);
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

}

static void sync_set_rl()
{
 uint16_t rl;
 rl=get_reg_value( RL_REGHOLDING_ADDR,1,REGHOLDING_MODE); 
 set_reg_value(RL_CONTROL_REGHOLDING_ADDR, 1,rl,REGHOLDING_MODE);
}
void sync_set_rm_motor(uint16_t reg_value)
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
 static uint8_t is_rice_take_away=BSP_TRUE;
 static osEvent ret;
    
while(1)
{
 APP_LOG_DEBUG("rice_mill_sync_task START!\r\n");
 sync_complete();//等待开始
 sync_busy();
do
{
 sync_set_rl();//调节对应的碾米分度值
 osSignalWait(SYNC_ALL_EVT,0);//clear all signals exsited
 ret=osSignalWait(SYNC_FAULT_EVT |SYNC_SET_RL_OK_EVT,osWaitForever);//等待分度调节器到位
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
  sync_shutdown();
}while(!(ret.status == osEventSignal && ret.value.signals & SYNC_SET_RL_OK_EVT));

while(cur_rice_weight_obtain_times < RICE_WEIGHT_OBTAIN_TIMES_EXPIRED)
{
do
{
 sync_set_ew_threshold();//设置出米重量
 osSignalWait(SYNC_ALL_EVT,0);//clear all signals  exsited
 ret=osSignalWait(SYNC_FAULT_EVT|SYNC_SET_EW_OK_EVT,osWaitForever );//等待出米重量设置完成
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 sync_shutdown();
 }while(!(ret.status == osEventSignal && ret.value.signals & SYNC_SET_EW_OK_EVT));

 do
 {
 sync_set_rb1(REG_VALUE_SWITCH_ON);//打开对应的1级仓 1号或者2号
 sync_set_wait_ew_weight_signal();//启动米重感知和超时
 osSignalWait(SYNC_ALL_EVT,0);//clear all signals  exsited
 ret=osSignalWait(SYNC_FAULT_EVT |SYNC_OBTAIN_RW_OK_EVT,osWaitForever );//等待米重达标,不处理打开到位信号
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
 ret=osSignalWait(SYNC_FAULT_EVT |SYNC_CLOSE_RB1_OK_EVT,osWaitForever );//等待一级仓关闭到位
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
  sync_shutdown();
}while(!(ret.status == osEventSignal && ret.value.signals & SYNC_CLOSE_RB1_OK_EVT));

do
{
 sync_set_rm_motor(REG_VALUE_SWITCH_ON);//启动碾米电机
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_FAULT_EVT,MOTOR_WAIT_EXPIRED_TIMEOUT_VALUE);//等待 1 s 电机稳定转动
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 {
 sync_set_rm_motor(REG_VALUE_SWITCH_OFF);//关闭碾米电机
 sync_shutdown();
 }
}
while(!(ret.status == osEventTimeout));
 
do
{
 sync_set_rb2(REG_VALUE_SWITCH_ON);//打开2级仓门
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_FAULT_EVT |SYNC_OPEN_RB2_OK_EVT,osWaitForever );//等待打开2级仓门到位
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 {
  sync_set_rm_motor(REG_VALUE_SWITCH_OFF);//关闭碾米电机
  sync_set_rb2(REG_VALUE_SWITCH_OFF);//关闭2级仓门
  sync_shutdown();
 }
}while(!(ret.status == osEventSignal && ret.value.signals & SYNC_OPEN_RB2_OK_EVT));

do
{
 sync_set_rb2(REG_VALUE_SWITCH_OFF);//关闭2级仓
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_FAULT_EVT |SYNC_CLOSE_RB2_OK_EVT,osWaitForever );//等待2级仓关闭到位
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 {
  sync_shutdown();
 }
}while(!(ret.status == osEventSignal && ret.value.signals  & SYNC_CLOSE_RB2_OK_EVT));
 
 cur_rice_weight_obtain_times++;
}
 cur_rice_weight_obtain_times=0;

 do
 {
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_FAULT_EVT,MOTOR_RUN_EXPIRED_TIMEOUT_VALUE );//等待碾米时间到
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 sync_shutdown();
 }while(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT);
 
 do
 {
 sync_set_rm_motor(REG_VALUE_SWITCH_OFF);//关闭碾米电机马达
 sync_set_rb2(REG_VALUE_SWITCH_OFF);//关闭2级仓
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_FAULT_EVT |SYNC_CLOSE_RB2_OK_EVT,osWaitForever );//等待2级仓关闭到位
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 sync_shutdown();
 }while(!(ret.status == osEventSignal && ret.value.signals & SYNC_CLOSE_RB2_OK_EVT));
  
 do
 {
 do
 {
 sync_set_oh_door(REG_VALUE_SWITCH_OFF);//打开升降门
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_FAULT_EVT |SYNC_OPEN_OH_DOOR_OK_EVT,osWaitForever );//等待门上升到位
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 sync_shutdown();
 }while(!(ret.status == osEventSignal && ret.value.signals & SYNC_OPEN_OH_DOOR_OK_EVT));
   
 //sync_set_take_rice_away();//启动米移走感知
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_FAULT_EVT |SYNC_TAKE_RICE_AWAY_OK_EVT,RICE_TAKE_AWAY_TIMEOUT_VALUE);//等待米被拿走 
 
 if(ret.status ==osEventSignal && ret.value.signals & SYNC_TAKE_RICE_AWAY_OK_EVT)
  is_rice_take_away= BSP_TRUE;
 else
  is_rice_take_away= BSP_FALSE;
 do
 {
 sync_set_oh_door(REG_VALUE_SWITCH_OFF);//关闭升降门
 osSignalWait(SYNC_ALL_EVT,0);  //clear all signals  exsited
 ret=osSignalWait(SYNC_FAULT_EVT |SYNC_CLOSE_OH_DOOR_OK_EVT,osWaitForever );
 if(ret.status == osEventSignal && ret.value.signals & SYNC_FAULT_EVT)
 sync_shutdown();
 }while(!(ret.status == osEventSignal && ret.value.signals & SYNC_CLOSE_OH_DOOR_OK_EVT));
 
 if(is_rice_take_away == BSP_FALSE)
  sync_shutdown();
 
 }while(is_rice_take_away!=BSP_TRUE);//如果没有取走米 重新启动后，继续开门拿米操作
 
 is_rice_take_away=BSP_FALSE;
 
 }
}

