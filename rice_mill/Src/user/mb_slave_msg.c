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
static void set_msg_adv_lamp_switch_regholding_write(void);     //0x100c      环境灯开关
static void set_msg_oh_door_switch_regholding_write(void);    //0x100d      升降门开关
static void set_msg_r_tare_regholding_write(void);            //0x100e      去皮
static void set_msg_z_clearing_regholding_write(void);        //0x100f      清零
static void set_msg_w_threshold_regholding_write(void);       //0x1010      重量定点值低
static void set_msg_w_threshold_regholding_write(void);       //0x1011      重量定点值高
static void set_msg_rl_control_regholding_write(void);        //0x1012      碾米分度值
static void set_msg_bl_regholding_write();                    //0x1013     呼吸灯
static void set_msg_ac_fan1_regholding_write();               //0x1014     交流风扇1
static void set_msg_ac_fan2_regholding_write();               //0x1015     交流风扇2

ptr_regholding_write_handler_t ptr_msg_handler[REG_HOLDING_NREGS]=
{  
/*****************************************   地址         寄存器   ************/
set_msg_rm_switch_regholding_write,        //0x1000      碾米机开关
set_msg_rb1_selection_regholding_write,    //0x1001      米仓选择

NULL,                                      //0x1002      出米重量低
set_msg_rw_regholding_write,               //0x1003      出米重量高

set_msg_rl_setup_regholding_write,         //0x1004      碾米分度值

set_msg_rm_fault_code_regholding_write,    //0x1005      碾米机故障码低
set_msg_rm_fault_code_regholding_write,    //0x1006      碾米机故障码高

set_msg_rm_motor_switch_regholding_write,  //0x1007      碾米机马达/吹糠机
set_msg_rb1_1_switch_regholding_write,     //0x1008      1号米仓阀门
set_msg_rb1_2_switch_regholding_write,     //0x1009      2号米仓阀门
set_msg_rb2_switch_regholding_write,       //0x100a      2级米仓阀门
set_msg_uv_lamp_switch_regholding_write,   //0x100b      紫外灯开关
set_msg_adv_lamp_switch_regholding_write,  //0x100c      广告灯开关
set_msg_oh_door_switch_regholding_write,   //0x100d      升降门开关
set_msg_r_tare_regholding_write,           //0x100e      去皮
set_msg_z_clearing_regholding_write,       //0x100f      清零
NULL,                                       //0x1010     重量定点值低
set_msg_w_threshold_regholding_write,      //0x1011      重量定点值高
set_msg_rl_control_regholding_write,       //0x1012      碾米控制分度
set_msg_bl_regholding_write,                //0x1013     呼吸灯
set_msg_ac_fan1_regholding_write,           //0x1014     交流风扇1
set_msg_ac_fan2_regholding_write            //0x1015     交流风扇2
};

extern QueueHandle_t rm_asyn_msg_queue_hdl;

void mb_slave_msg_init()
{
  osMessageQDef(rm_asyn_msg_queue, 20, uint8_t);
  
  rm_asyn_msg_queue_hdl= osMessageCreate (osMessageQ(rm_asyn_msg_queue),NULL);
  if(rm_asyn_msg_queue_hdl)
  {
  APP_LOG_DEBUG("rm asyn msg queue init successed!\r\n"); 
  }
  else
  {
  APP_LOG_DEBUG("rm asyn msg queue init failed!\r\n");  
  }
  
}


static void set_msg_rm_switch_regholding_write(void)        //0x1000      碾米机开关
{
  osStatus status; 
  uint16_t reg_value;
  reg_value= get_reg_value(RM_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("write rm reg value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
   reg_value= get_rm_fault_code();
   if(!reg_value)
   {
   status= osMessagePut(rm_asyn_msg_queue_hdl ,MSG_PWR_ON_RM_MOTOR,0);
   APP_LOG_DEBUG("send msg:Start rm!status:%d\r\n",status);
   }
   else
   {
    APP_LOG_DEBUG("rm in error, canot start!\r\n");
   }
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RM_MOTOR,0);
   APP_LOG_DEBUG("send msg:stop rm!status:%d\r\n",status);
 }
 (void)status;
}

static void set_msg_rb1_selection_regholding_write(void)    //0x1001      米仓选择
{
   osStatus status;
   uint16_t reg_value;
   reg_value= get_reg_value(RB1_SELECTION_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
   APP_LOG_DEBUG("write rb1 selection reg value:%d\r\n",reg_value);
   if(reg_value==REG_VALUE_RB1_NO_1_ID)
   {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_RB1_SELECT_NO1,0);
   APP_LOG_DEBUG("send msg:select no 1!status:%d\r\n",status);
   }
   else if(reg_value==REG_VALUE_RB1_NO_2_ID)
   {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_RB1_SELECT_NO2,0);
   APP_LOG_DEBUG("send msg:select no 2!status:%d\r\n",status);
   } 
  (void)status;
}

static void set_msg_rw_regholding_write(void)               //0x1002      出米重量
{
   osStatus status;
   uint32_t reg_value;
   reg_value= get_reg_value(RW_REGHOLDING_ADDR, 2,REGHOLDING_MODE);
   APP_LOG_DEBUG("write rw reg value value:%d\r\n",reg_value);
   
   if(reg_value>REG_VALUE_RW_MIN && reg_value<REG_VALUE_RW_MAX)
   {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_SET_RW_VALUE,0);
   APP_LOG_DEBUG("send msg:set rw! status:%d\r\n",status);  
   }
   (void)status;
}
 static void set_msg_rl_setup_regholding_write(void)     //0x1004   碾米分度值
{
   osStatus status;
   uint32_t reg_value;
   reg_value= get_reg_value(RL_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
   APP_LOG_DEBUG("write rl setup reg value:%d\r\n",reg_value);
   if(reg_value == REG_VALUE_RL_0 ||
      reg_value == REG_VALUE_RL_5 ||
      reg_value == REG_VALUE_RL_7 ||
      reg_value == REG_VALUE_RL_9 )
   {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_SETUP_RL_VALUE,0);
   APP_LOG_DEBUG("send msg:rl value setup! status:%d\r\n",status); 
   }
   (void)status;
   
  
}

static void set_msg_rl_control_regholding_write(void)     //0x1012   碾米控制分度值          
{
   uint16_t msg,reg_value;
   osStatus status;
   
   reg_value= get_reg_value(RL_CONTROL_REGHOLDING_ADDR, 1,REGHOLDING_MODE);   
   APP_LOG_DEBUG("write rl control reg value:%d\r\n",reg_value);
   
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
   APP_LOG_DEBUG("send msg:control rl = %d! status:%d\r\n",msg,status);  
   (void)status;
}

static void set_msg_rm_fault_code_regholding_write(void)     //0x1005   碾米机故障码
{
   osStatus status;
   uint32_t reg_value;
   
   reg_value=  get_rm_fault_code();  
   APP_LOG_DEBUG("write fault code reg value:%d\r\n",reg_value);  
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_SET_RM_FAULT_CODE,0);
   APP_LOG_DEBUG("send msg:set fault code! status:%d\r\n",status); 
   (void)status;
}


static void set_msg_rm_motor_switch_regholding_write(void)   //0x1007  碾米机马达/吹糠机
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(RM_MOTOR_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("write rm motor reg value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_PWR_ON_RM_MOTOR,0);
  APP_LOG_DEBUG("send msg:pwr on rm motor! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_RM_MOTOR,0);
  APP_LOG_DEBUG("send msg:pwr dwn rm motor! status:%d\r\n",status); 
 }
  (void)status;
}
static void set_msg_rb1_1_switch_regholding_write(void)      //0x1008      1号米仓阀门
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(RB1_1_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("write rb1_1 reg value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_ON_RB1_1_SWITCH,0);
  APP_LOG_DEBUG("send msg:turn on no1! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_RB1_1_SWITCH,0);
  APP_LOG_DEBUG("send msg:turn off no1! status:%d\r\n",status); 
 } 
  (void)status;
}
static void set_msg_rb1_2_switch_regholding_write(void)      //0x1009      2号米仓阀门
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(RB1_2_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("write rb1_2 reg value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_ON_RB1_2_SWITCH,0);
  APP_LOG_DEBUG("send msg:turn on no2! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_RB1_2_SWITCH,0);
  APP_LOG_DEBUG("send msg:turn off no2! status:%d\r\n",status); 
 }
  (void)status;
}

static void set_msg_rb2_switch_regholding_write(void)        //0x100a      2级米仓阀门
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(RB2_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("write rb2 reg value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_ON_RB2_SWITCH,0);
  APP_LOG_DEBUG("send msg:turn on rb2! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_RB2_SWITCH,0);
  APP_LOG_DEBUG("send msg:turn off rb2! status:%d\r\n",status); 
 }
  (void)status;
}
static void set_msg_uv_lamp_switch_regholding_write(void)    //0x100b      紫外灯开关
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(UV_LAMP_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("write uv lamp reg value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_PWR_ON_UV_LAMP,0);
  APP_LOG_DEBUG("send msg:pwr on uv lamp! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_UV_LAMP,0);
  APP_LOG_DEBUG("send msg:pwr dwn uv lamp! status:%d\r\n",status); 
 } 
  (void)status;
}
static void set_msg_adv_lamp_switch_regholding_write(void)     //0x100c      环境灯开关
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(ADV_LAMP_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("write adv reg value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_PWR_ON_ADV_LAMP,0);
  APP_LOG_DEBUG("send msg:pwr on adv lamp! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_ADV_LAMP,0);
  APP_LOG_DEBUG("send msg:pwr dwn adv lamp! status:%d\r\n",status); 
 }
  (void)status;
}

static void set_msg_oh_door_switch_regholding_write(void)    //0x100d      升降门开关
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(ADV_LAMP_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("write oh door reg value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_ON_OH_DOOR,0);
  APP_LOG_DEBUG("send msg:turn on oh door! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_TURN_OFF_OH_DOOR,0);
  APP_LOG_DEBUG("send msg:turn off oh door! status:%d\r\n",status); 
 } 
  (void)status;
}
static void set_msg_r_tare_regholding_write(void)            //0x100e      去皮
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(ADV_LAMP_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("write remove tare reg value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_FUNC_ENABLE )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_ENABLE_REMOVE_TARE,0);
  APP_LOG_DEBUG("send msg:enable remove tare! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_FUNC_DISABLE)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_DISABLE_REMOVE_TARE,0);
  APP_LOG_DEBUG("send msg:disable remove tare! status:%d\r\n",status); 
 } 
  (void)status;
}
static void set_msg_z_clearing_regholding_write(void)        //0x100f      清零
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(ADV_LAMP_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("write clrearig zero reg value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_FUNC_ENABLE )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_ENABLE_REMOVE_TARE,0);
  APP_LOG_DEBUG("send msg:enable clearing zero! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_FUNC_DISABLE)
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_DISABLE_REMOVE_TARE,0);
  APP_LOG_DEBUG("send msg:disable clearing zero! status:%d\r\n",status); 
 } 
  (void)status;
}
static void set_msg_w_threshold_regholding_write(void)       //0x1010     重量定点值
{
  osStatus status;
  uint32_t reg_value;
  reg_value= get_reg_value(EW_THRESHOLD_REGHOLDING_ADDR, 2,REGHOLDING_MODE);
  APP_LOG_DEBUG("write ew threshold reg value:%d\r\n",reg_value);
 if(reg_value> REG_VALUE_RW_MIN && reg_value< REG_VALUE_RW_MAX )
 {
  status= osMessagePut(rm_asyn_msg_queue_hdl ,MSG_SET_EW_THRESHOLD_VALUE,0);
  APP_LOG_DEBUG("send msg:set ew threshold!status:%d\r\n",status);
 }   
 (void)status;
}
static void set_msg_bl_regholding_write()                //0x1013     呼吸灯
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(BL_SWITCH_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("write bl value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_PWR_ON_BL,0);
  APP_LOG_DEBUG("send msg:open bl! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_BL,0);
  APP_LOG_DEBUG("send msg:close bl! status:%d\r\n",status); 
 }
  (void)status;  
  
}
static void set_msg_ac_fan1_regholding_write()           //0x1014     交流风扇1
{
  osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(AC_FAN1_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("write fan1 value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_PWR_ON_AC_FAN1,0);
  APP_LOG_DEBUG("send msg:open fan1! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_AC_FAN1,0);
  APP_LOG_DEBUG("send msg:close fan1! status:%d\r\n",status); 
 }
  (void)status;  
}
static void set_msg_ac_fan2_regholding_write()          //0x1015     交流风扇2
{
   osStatus status;
  uint16_t reg_value;
  reg_value= get_reg_value(AC_FAN2_REGHOLDING_ADDR, 1,REGHOLDING_MODE);
  APP_LOG_DEBUG("write fan2 value:%d\r\n",reg_value);
 if(reg_value==REG_VALUE_SWITCH_ON )
 {
  status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_PWR_ON_AC_FAN2,0);
  APP_LOG_DEBUG("send msg:open fan2! status:%d\r\n",status); 
 }
 else if(reg_value==REG_VALUE_SWITCH_OFF)
 {
   status= osMessagePut (rm_asyn_msg_queue_hdl ,MSG_PWR_DWN_AC_FAN2,0);
  APP_LOG_DEBUG("send msg:close fan2! status:%d\r\n",status); 
 }
  (void)status;  
}