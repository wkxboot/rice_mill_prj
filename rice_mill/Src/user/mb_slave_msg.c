#include "cmsis_os.h"
#include "app_log.h"

#include "mb_slave_reg.h"
#include "mb_slave_msg.h"


/*****************************************   地址      寄存器       ************/
static void set_msg_rm_switch_regholding_write(void);        //0x1000      碾米机开关
static void set_msg_rb1_selection_regholding_write(void);    //0x1001      米仓选择

static void set_msg_rw_regholding_write(void);               //0x1002      出米重量低
static void set_msg_rw_regholding_write(void);               //0x1003      出米重量高

static void set_msg_rl_setup_regholding_write(void);         //0x1004      碾米分度值

static void set_msg_rm_fault_code_regholding_write(void);     //0x1005      碾米机故障码低
static void set_msg_rm_fault_code_regholding_write(void);     //0x1006      碾米机故障码高

static void set_msg_rm_motor_switch_regholding_write(void);   //0x1007      碾米机马达/吹糠机
static void set_msg_rb1_1_switch_regholding_write(void);      //0x1008      1号米仓阀门
static void set_msg_rb1_2_switch_regholding_write(void);      //0x1009      2号米仓阀门
static void set_msg_rb2_switch_regholding_write(void);        //0x100a      2级米仓阀门
static void set_msg_uv_lamp_switch_regholding_write(void);    //0x100b      紫外灯开关
static void set_msg_e_lamp_switch_regholding_write(void);     //0x100c      环境灯开关
static void set_msg_oh_door_switch_regholding_write(void);    //0x100d      升降门开关
static void set_msg_r_tare_regholding_write(void);            //0x100e      去皮
static void set_msg_z_clearing_regholding_write(void);        //0x100f      清零
static void set_msg_w_threshold_regholding_write(void);       //0x1010      重量定点值低
static void set_msg_w_threshold_regholding_write(void);       //0x1011      重量定点值高
static void set_msg_rl_control_regholding_write(void);        //0x1012      碾米分度值

ptr_regholding_write_handler_t ptr_msg_handler[REG_HOLDING_NREGS]=
{  
/*****************************************   地址         寄存器   ************/
set_msg_rm_switch_regholding_write,        //0x1000      碾米机开关
set_msg_rb1_selection_regholding_write,    //0x1001      米仓选择

set_msg_rw_regholding_write,               //0x1002      出米重量低
set_msg_rw_regholding_write,               //0x1003      出米重量高

set_msg_rl_setup_regholding_write,         //0x1004      碾米分度值

set_msg_rm_fault_code_regholding_write,    //0x1005      碾米机故障码低
set_msg_rm_fault_code_regholding_write,    //0x1006      碾米机故障码高

set_msg_rm_motor_switch_regholding_write,  //0x1007      碾米机马达/吹糠机
set_msg_rb1_1_switch_regholding_write,     //0x1008      1号米仓阀门
set_msg_rb1_2_switch_regholding_write,     //0x1009      2号米仓阀门
set_msg_rb2_switch_regholding_write,       //0x100a      2级米仓阀门
set_msg_uv_lamp_switch_regholding_write,   //0x100b      紫外灯开关
set_msg_e_lamp_switch_regholding_write,    //0x100c      环境灯开关
set_msg_oh_door_switch_regholding_write,   //0x100d      升降门开关
set_msg_r_tare_regholding_write,           //0x100e      去皮
set_msg_z_clearing_regholding_write,       //0x100f      清零
set_msg_w_threshold_regholding_write,      //0x1010      重量定点值低
set_msg_w_threshold_regholding_write,       //0x1011      重量定点值高
set_msg_rl_control_regholding_write,       //0x1012      碾米控制分度
};

QueueHandle_t rm_asyn_msg_queue_hdl;

void mb_slave_msg_init()
{
  osMessageQDef(rm_asyn_msg_queue, 20, uint8_t);
  
  rm_asyn_msg_queue_hdl= osMessageCreate (osMessageQ(rm_asyn_msg_queue),NULL);
  if(rm_asyn_msg_queue_hdl)
  {
  APP_LOG_DEBUG("MB 从机消息队列初始化成功!\r\n"); 
  }
  else
  {
  APP_LOG_DEBUG("MB 从机消息队列初始化失败!\r\n");  
  }
  
}


static void set_msg_rm_switch_regholding_write(void)        //0x1000      碾米机开关
{
  osStatus status; 
  uint16_t reg_value;
  reg_value= get_reg_value(RM_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("写的碾米开关值:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
   reg_value= get_rm_fault_code();
   if(!reg_value)
   {
   status= osMessagePut(rm_asyn_msg_queue_hdl ,MSG_TURN_ON_RM,0);
   APP_LOG_DEBUG("发送消息:开始碾米!status:%d\r\n",status);
   }
   else
   {
    APP_LOG_DEBUG("碾米机有错误，禁止启动!\r\n");
   }
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_RM,0);
   APP_LOG_DEBUG("发送消息:停止碾米!status:%d\r\n",status);
 }
 (void)status;
}

static void set_msg_rb1_selection_regholding_write(void)    //0x1001      米仓选择
{
   osStatus status;
   uint16_t reg_value;
   reg_value= get_reg_value(RB1_SELECTION_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
   APP_LOG_DEBUG("写的米仓选择值:%d\r\n",reg_value);
   if(reg_value==REG_VALUE_RB1_NO_1_ID)
   {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_RB1_SELECT_NO1,0);
   APP_LOG_DEBUG("发送消息:选择1号仓!status:%d\r\n",status);
   }
   else if(reg_value==REG_VALUE_RB1_NO_2_ID)
   {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_RB1_SELECT_NO2,0);
   APP_LOG_DEBUG("发送消息:选择2号仓!status:%d\r\n",status);
   } 
  (void)status;
}

static void set_msg_rw_regholding_write(void)               //0x1002      出米重量
{
   osStatus status;
   uint32_t reg_value;
   reg_value= get_reg_value(RW_REGHOLDING_ADDR, 2,REGHOLDING_MODE);
   APP_LOG_DEBUG("写的出米重量值:%d\r\n",reg_value);
   
   if(reg_value>REG_VALUE_RW_MIN && reg_value<REG_VALUE_RW_MAX)
   {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_SET_RW_VALUE,0);
   APP_LOG_DEBUG("发送消息:设置出米重量! status:%d\r\n",status);  
   }
   (void)status;
}
 static void set_msg_rl_setup_regholding_write(void)     //0x1004   碾米分度值
{
   osStatus status;
   uint32_t reg_value;
   reg_value= get_reg_value(RL_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
   APP_LOG_DEBUG("写的出米分度值:%d\r\n",reg_value);
   if(reg_value == REG_VALUE_RL_0 ||
      reg_value == REG_VALUE_RL_5 ||
      reg_value == REG_VALUE_RL_7 ||
      reg_value == REG_VALUE_RL_9 )
   {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_SETUP_RL,0);
   APP_LOG_DEBUG("发送消息:出米分度值! status:%d\r\n",status); 
   }
   (void)status;
   
  
}

static void set_msg_rl_control_regholding_write(void)     //0x1012   碾米控制分度值          
{
   uint16_t msg,reg_value;
   osStatus status;
   
   reg_value= get_reg_value(RL_CONTROL_REGHOLDING_ADDR, 1,REGHOLDING_MODE);   
   APP_LOG_DEBUG("写的分度器控制值:%d\r\n",reg_value);
   
   if(reg_value==REG_VALUE_RL_0)
   {
     msg=MSG_SET_RL_0;
   }
   else if(reg_value==REG_VALUE_RL_0)
   {
    msg=MSG_SET_RL_5; 
   }
   else if(reg_value==REG_VALUE_RL_0)
   {
     msg=MSG_SET_RL_7;  
   }
   else if(reg_value==REG_VALUE_RL_0)
   {
     msg=MSG_SET_RL_9;  
   }
   status= osMessagePut (rm_asyn_msg_queue_hdl ,msg,0);
   APP_LOG_DEBUG("发送消息:选择分度值%d! status:%d\r\n",msg,status);  
   (void)status;
}

static void set_msg_rm_fault_code_regholding_write(void)     //0x1005   碾米机故障码
{
   osStatus status;
   uint32_t reg_value;
   
   reg_value=  get_rm_fault_code();  
   APP_LOG_DEBUG("写的错误值:%d\r\n",reg_value);  
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_SET_RM_FAULT_CODE,0);
   APP_LOG_DEBUG("发送消息:设置错误码! status:%d\r\n",status); 
   (void)status;
}


static void set_msg_rm_motor_switch_regholding_write(void)   //0x1007  碾米机马达/吹糠机
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(RM_MOTOR_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("写的碾米机电机开关值:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_ON_RM_MOTOR,0);
  APP_LOG_DEBUG("发送消息:打开碾米电机! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_RM_MOTOR,0);
  APP_LOG_DEBUG("发送消息:关闭碾米电机! status:%d\r\n",status); 
 }
  (void)status;
}
static void set_msg_rb1_1_switch_regholding_write(void)      //0x1008      1号米仓阀门
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(RB1_1_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("写的一号仓开关值值:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_ON_RB1_1_SWITCH,0);
  APP_LOG_DEBUG("发送消息:打开1号仓! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_RB1_1_SWITCH,0);
  APP_LOG_DEBUG("发送消息:关闭1号仓! status:%d\r\n",status); 
 } 
  (void)status;
}
static void set_msg_rb1_2_switch_regholding_write(void)      //0x1009      2号米仓阀门
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(RB1_2_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("写的二号仓开关值值:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_ON_RB1_2_SWITCH,0);
  APP_LOG_DEBUG("发送消息:打开2号仓! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_RB1_2_SWITCH,0);
  APP_LOG_DEBUG("发送消息:关闭2号仓! status:%d\r\n",status); 
 }
  (void)status;
}

static void set_msg_rb2_switch_regholding_write(void)        //0x100a      2级米仓阀门
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(RB2_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("写的一号仓开关值值:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_ON_RB2_SWITCH,0);
  APP_LOG_DEBUG("发送消息:打开2级仓! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_RB2_SWITCH,0);
  APP_LOG_DEBUG("发送消息:关闭2级仓! status:%d\r\n",status); 
 }
  (void)status;
}
static void set_msg_uv_lamp_switch_regholding_write(void)    //0x100b      紫外灯开关
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(E_LAMP_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("写的紫外灯开关值:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_ON_E_LAMP,0);
  APP_LOG_DEBUG("发送消息:打开紫外灯! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_E_LAMP,0);
  APP_LOG_DEBUG("发送消息:关闭紫外灯! status:%d\r\n",status); 
 } 
  (void)status;
}
static void set_msg_e_lamp_switch_regholding_write(void)     //0x100c      环境灯开关
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(E_LAMP_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("写的环境灯开关值:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_ON_E_LAMP,0);
  APP_LOG_DEBUG("发送消息:打开环境灯! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_E_LAMP,0);
  APP_LOG_DEBUG("发送消息:关闭环境灯! status:%d\r\n",status); 
 }
  (void)status;
}

static void set_msg_oh_door_switch_regholding_write(void)    //0x100d      升降门开关
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(E_LAMP_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("写的升降门开关值:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_ON_OH_DOOR,0);
  APP_LOG_DEBUG("发送消息:打开升降门! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_OH_DOOR,0);
  APP_LOG_DEBUG("发送消息:关闭升降门! status:%d\r\n",status); 
 } 
  (void)status;
}
static void set_msg_r_tare_regholding_write(void)            //0x100e      去皮
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(E_LAMP_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("写的去皮值:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_FUNC_ENABLE )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_ENABLE_REMOVE_TARE,0);
  APP_LOG_DEBUG("发送消息:使能去皮! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_FUNC_DISABLE)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_DISABLE_REMOVE_TARE,0);
  APP_LOG_DEBUG("发送消息:失能去皮! status:%d\r\n",status); 
 } 
  (void)status;
}
static void set_msg_z_clearing_regholding_write(void)        //0x100f      清零
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(E_LAMP_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("写的清零值:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_FUNC_ENABLE )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_ENABLE_REMOVE_TARE,0);
  APP_LOG_DEBUG("发送消息:使能清零! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_FUNC_DISABLE)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_DISABLE_REMOVE_TARE,0);
  APP_LOG_DEBUG("发送消息:失能清零! status:%d\r\n",status); 
 } 
  (void)status;
}
static void set_msg_w_threshold_regholding_write(void)       //0x1010     重量定点值
{
  osStatus status;
  uint32_t reg_value;
  reg_value= get_reg_value(EW_THRESHOLD_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("写的定点值:%d\r\n",reg_value);
 if(reg_value> REG_VALUE_RW_MIN && reg_value< REG_VALUE_RW_MAX )
 {
  status= osMessagePut(rm_asyn_msg_queue_hdl ,MSG_SET_EW_THRESHOLD_VALUE,0);
  APP_LOG_DEBUG("发送消息:写定点值!status:%d\r\n",status);
 }   
 (void)status;
}
