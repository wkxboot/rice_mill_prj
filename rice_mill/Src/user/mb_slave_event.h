#ifndef __MB_SLAVE_EVENT_H__
#define __MB_SLAVE_EVENT_H__


#define  RM_SWITCH_SETUP_EVENT           (1<<0)
#define  RB1_SELECTION_EVENT             (1<<1)
#define  RW_SETUP_EVENT                  (1<<2)
#define  RL_SETUP_EVENT                  (1<<3)
#define  RM_FAULT_CODE_SETUP_EVENT       (1<<4)
#define  RM_MOTOR_SWITCH_SETUP_EVENT     (1<<5)
#define  RB1_1_SWITCH_SETUP_EVENT        (1<<6)
#define  RB1_2_SWITCH_SETUP_EVENT        (1<<7)
#define  RB2_SWITCH_SETUP_EVENT          (1<<8)
#define  UV_LAMP_SWITCH_SETUP_EVENT      (1<<9)
#define  E_LAMP_SWITCH_SETUP_EVENT       (1<<10)
#define  OH_DOOR_SWITCH_SETUP_EVENT      (1<<11)
#define  R_TARE_SETUP_EVENT              (1<<12)
#define  Z_CLEARING_SETUP_EVENT          (1<<13)
#define  W_THRESHOLD_SETUP_EVENT         (1<<14)

#define ALL_RM_EVENT                     (0xFFFF)







#endif