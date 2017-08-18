#ifndef __USER_TASKS_H__
#define __USER_TASKS_H__
//温度
#define  TEM_MAX            50
#define  TEM_MIN            0
#define  TEM_WARNING        15
//相对湿度
#define  RH_MAX              100
#define  RH_MIN              0
#define  RH_WARNING          60





/****************************************************
 *碾米机同步任务
 ***************************************************/
#define  RICE_TAKE_AWAY_TIMEOUT_VALUE        30000 
#define  MOTOR_RUN_EXPIRED_TIMEOUT_VALUE     5000 
#define  RL_SET_EXPIRED_TIMEOUT_VALUE        5000  
#define  RW_SET_EXPIRED_TIMEOUT_VALUE        1000
#define  RW_WAIT_EXPIRED_TIMEOUT_VALUE       5000
#define  RB1_RUN_EXPIRED_TIMEOUT_VALUE       5000
#define  MOTOR_WAIT_EXPIRED_TIMEOUT_VALUE    1000 
#define  OH_DOOR_RUN_EXPIRED_TIMEOUT_VALUE   5000

#define  RICE_WEIGHT_OBTAIN_TIMES_EXPIRED    2
   

#define  SYNC_FAULT_EVT                  (1<<0)
#define  SYNC_SET_RL_OK_EVT              (1<<1)
#define  SYNC_SET_EW_OK_EVT              (1<<2)
#define  SYNC_OBTAIN_RW_OK_EVT           (1<<3)
#define  SYNC_OPEN_RB1_OK_EVT            (1<<4)
#define  SYNC_CLOSE_RB1_OK_EVT           (1<<5)
#define  SYNC_OPEN_RB2_OK_EVT            (1<<6) 
#define  SYNC_CLOSE_RB2_OK_EVT           (1<<7)
#define  SYNC_OPEN_OH_DOOR_OK_EVT        (1<<8)
#define  SYNC_CLOSE_OH_DOOR_OK_EVT       (1<<9)
#define  SYNC_TAKE_RICE_AWAY_OK_EVT      (1<<10)  
#define  SYNC_ALL_EVT                   (0xFFF)
   
/************** 碾米机异步任务 *******************************/
#define  RB1_TIMEOUT_VALUE                 3000
#define  RB2_TIMEOUT_VALUE                 3000
#define  RL_TIMEOUT_VALUE                  3000
#define  OH_DOOR_TIMEOUT_VALUE             5000
#define  EW_TIMEOUT_VALUE                  2000

#define  SENSOR_CHECK_TIMEOUT_VALUE           50//暂定每秒检查20次

/********电子秤寄存器地址**********************/
#define  EW_GET_RESOURCE_TIMEOUT           50 //为了获取电子秤通信权而等待的时间 单位ms
#define  SLAVE_EW_ADDR                     02
#define  REG_GROSS_WEIGHT_ADDR             0x0000     
#define  REG_NET_WEIGHT_ADDR               0x0001
#define  REG_WEIGHT_THRESHOLD_ADDR         0x000C
#define  REG_REMOVE_TARE_ADDR              0x0060
#define  REG_CLEARING_ZERO_ADDR            0x0060   
   
/********电子秤功能任务消息**********************/ 

#define  RW_OBTAIN_INTERVAL                (200/SENSOR_CHECK_TIMEOUT_VALUE)//200m查询一次电子秤重量

#define  MSG_EW_OBTAIN_RICE_GROSS_WEIGHT   0x0001
#define  MSG_EW_OBTAIN_RICE_NET_WEIGHT     0x0002
#define  MSG_EW_SET_RICE_WEIGHT_THRESHOLD  0x0003
#define  MSG_EW_REMOVE_TARE                0x0004
#define  MSG_EW_CLEARING_ZERO              0x0005

/********传感器任务消息**************************/
#define  SENSOR_CHECK_RICE_GROSS_WEIGHT_EVT      (1<<0)
#define  SENSOR_CHECK_RICE_NET_WEIGHT_EVT        (1<<1)
#define  SENSOR_CHECK_RB_T_EVT                   (1<<2)
#define  SENSOR_CHECK_RB_RH_EVT                  (1<<3)
#define  SENSOR_CHECK_24V_OC_EVT                 (1<<4)
#define  SENSOR_CHECK_OH_DOOR_OC_EVT             (1<<5)
#define  SENSOR_CHECK_OH_DOOR_PATHWAY_EVT        (1<<6)
#define  SENSOR_CHECK_RM_MOTOR_OC_EVT            (1<<7)
#define  SENSOR_CHECK_AC2_OC_EVT                 (1<<8)
#define  SENSOR_CHECK_BEMF_OC_EVT                (1<<9)
#define  SENSOR_CHECK_RB1_1_EMPTY_EVT            (1<<10)
#define  SENSOR_CHECK_RB1_2_EMPTY_EVT            (1<<11)
#define  SENSOR_CHECK_RB1_1_TURN_ON_EVT          (1<<12)
#define  SENSOR_CHECK_RB1_2_TURN_ON_EVT          (1<<13)
#define  SENSOR_CHECK_RB1_TURN_OFF_EVT           (1<<14)
#define  SENSOR_CHECK_RB2_TURN_ON_EVT            (1<<15)
#define  SENSOR_CHECK_RB2_TURN_OFF_EVT           (1<<16)
#define  SENSOR_CHECK_OH_DOOR_TURN_ON_EVT        (1<<17)
#define  SENSOR_CHECK_OH_DOOR_TURN_OFF_EVT       (1<<18)
#define  SENSOR_CHECK_RL_ON_TAR_POS_EVT          (1<<19)


#endif