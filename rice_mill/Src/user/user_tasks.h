#ifndef __USER_TASKS_H__
#define __USER_TASKS_H__

/*
 *碾米机同步任务事件
 */
#define  RICE_TAKE_AWAY_TIMEOUT_VALUE    30000 //30s
#define  MOTOR_EXPIRED_TIMEOUT_VALUE     5000  //5s



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


/********电子秤寄存器地址**********************/

#define  






#endif