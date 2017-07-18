#include "mb_slave_event.h"
#include "cmsis_os.h"
#include "app_log.h"

typedef (void *ptr_regholding_write_event_handler_t)(void);



void mb_slave_event_init()
{
  
 APP_LOG_DEBUG("mb_slave_event_init!\r\n"); 
  
}

/*****************************************   ��ַ         �Ĵ���   ************/
void set_evt_rm_switch_regholding_write(void);        //0x1000      ���׻�����
void set_evt_rb1_selection_regholding_write(void);    //0x1001      �ײ�ѡ��

void set_evt_rw_regholding_write(void);               //0x1002      ����������
void set_evt_rw_regholding_write(void);               //0x1003      ����������

void set_evt_rl_regholding_write(void);               //0x1004      ���׷ֶ�ֵ

void set_evt_rm_fault_code_regholding_write(void);     //0x1005      ���׻��������
void set_evt_rm_fault_code_regholding_write(void);     //0x1006      ���׻��������

void set_evt_rm_motor_switch_regholding_write(void);   //0x1007      ���׻����/������
void set_evt_rb1_1_switch_regholding_write(void);      //0x1008      1���ײַ���
void set_evt_rb1_2_switch_regholding_write(void);      //0x1009      2���ײַ���
void set_evt_rb2_switch_regholding_write(void);        //0x100a      2���ײַ���
void set_evt_uv_lamp_switch_regholding_write(void);    //0x100b      ����ƿ���
void set_evt_e_lamp_switch_regholding_write(void);     //0x100c      �����ƿ���
void set_evt_oh_door_switch_regholding_write(void);    //0x100d      �����ſ���
void set_evt_r_tare_regholding_write(void);            //0x100e      ȥƤ
void set_evt_z_clearing_regholding_write(void);        //0x100f      ����
void set_evt_w_threshold_regholding_write(void);       //0x1010      ��������ֵ��
void set_evt_w_threshold_regholding_write(void);       //0x1011      ��������ֵ��




void set_evt_rm_switch_regholding_write(void)        //0x1000      ���׻�����
{
 osSignalSet( rm_execute_thread_handle, RM_SWITCH_SETUP_EVENT); 
}
void set_evt_rb1_selection_regholding_write(void)    //0x1001      �ײ�ѡ��
{
osSignalSet( rm_execute_thread_handle, RB1_SELECTION_EVENT);   
}

void set_evt_rw_regholding_write(void)               //0x1002      ��������
{
 osSignalSet( rm_execute_thread_handle, RW_SETUP_EVENT);  
}


void set_evt_rl_regholding_write(void)               //0x1004      ���׷ֶ�ֵ
{
 osSignalSet( rm_execute_thread_handle,RL_SETUP_EVENT);  
}

void set_evt_rm_fault_code_regholding_write(void)     //0x1005      ���׻�������
{
 osSignalSet( rm_execute_thread_handle,RM_FAULT_CODE_SETUP_EVENT);  
}


void set_evt_rm_motor_switch_regholding_write(void)   //0x1007      ���׻����/������
{
  osSignalSet( rm_execute_thread_handle,RM_MOTOR_SWITCH_SETUP_EVENT);   
}
void set_evt_rb1_1_switch_regholding_write(void)      //0x1008      1���ײַ���
{
  osSignalSet( rm_execute_thread_handle,RB1_1_SWITCH_SETUP_EVENT); 
}
void set_evt_rb1_2_switch_regholding_write(void)      //0x1009      2���ײַ���
{
 osSignalSet( rm_execute_thread_handle,RB1_2_SWITCH_SETUP_EVENT);   
}
void set_evt_rb2_switch_regholding_write(void)        //0x100a      2���ײַ���
{
  osSignalSet( rm_execute_thread_handle,RB2_SWITCH_SETUP_EVENT);    
}
void set_evt_uv_lamp_switch_regholding_write(void)    //0x100b      ����ƿ���
{
  osSignalSet( rm_execute_thread_handle,UV_LAMP_SWITCH_SETUP_EVENT);  
}
void set_evt_e_lamp_switch_regholding_write(void)     //0x100c      �����ƿ���
{
  osSignalSet( rm_execute_thread_handle,E_LAMP_SWITCH_SETUP_EVENT);  
}
void set_evt_oh_door_switch_regholding_write(void)    //0x100d      �����ſ���
{
  osSignalSet( rm_execute_thread_handle,OH_DOOR_SWITCH_SETUP_EVENT);   
}
void set_evt_r_tare_regholding_write(void)            //0x100e      ȥƤ
{
 osSignalSet( rm_execute_thread_handle,R_TARE_SETUP_EVENT);    
}
void set_evt_z_clearing_regholding_write(void)        //0x100f      ����
{
 osSignalSet( rm_execute_thread_handle,Z_CLEARING_SETUP_EVENT);   
}
void set_evt_w_threshold_regholding_write(void)       //0x1010      ��������ֵ
{
 osSignalSet( rm_execute_thread_handle,W_THRESHOLD_SETUP_EVENT);    
}

osEvent osSignalWait (int32_t signals, uint32_t millisec)