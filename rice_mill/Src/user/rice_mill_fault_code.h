#ifndef __RICE_MILL_FAULT_CODE_H__
#define __RICE_MILL_FAULT_CODE_H__



/****************************************************
*0x00000000 �޹���
*Bit[0] ��1һ���ײֿ�
*Bit[1] ��1�����ײֿ�
*Bit[2] ��1 һ���ַ��Ź���
*Bit[3] ��1 �����ַ��Ź���
*Bit[4] ��1���ӳ�����Ӧ
*Bit[5] ��1���ӳӳ�����
*Bit[6] ��1 ���ӳ����ݴ���
*Bit[7] ��1 �������������
*Bit[8] ��1 ���������г�ʱ
*Bit[9] ��1���������г�ʱ
*Bit[10] ��1�����ŵ����ת
*Bit[11] ��1�ֶȵ����������ת
*Bit[12] ��1�ֶȵ��������г�ʱ
*Bit[13] ��1һ�Ų��¶ȹ���
*Bit[14] ��1һ�Ų�ʪ�ȹ���
*Bit[15] ��1���Ų��¶ȹ���
*Bit[16] ��1���Ų�ʪ�ȹ���
*******************************************************/
#define  ERROR_CODE_NO_ERROR                   0
#define  ERROR_CODE_RB1_1_EMPTY               (1<<0)
#define  ERROR_CODE_RB1_2_EMPTY               (1<<1)
#define  ERROR_CODE_RB1_SWITCH_FAULT          (1<<2)
#define  ERROR_CODE_RB2_SWITCH_FAULT          (1<<3)
#define  ERROR_CODE_EW_NO_RESPONSE            (1<<4)
#define  ERROR_CODE_EW_OVERLOAD               (1<<5)
#define  ERROR_CODE_EW_DATA_ILLEGAL           (1<<6)
#define  ERROR_CODE_MOTOR_OC_STALL            (1<<7)
#define  ERROR_CODE_OH_DOOR_UP_TIMEOUT        (1<<8)
#define  ERROR_CODE_OH_DOOR_DOWN_TIMEOUT      (1<<9)
#define  ERROR_CODE_OH_DOOR_MOTOR_STALL       (1<<10)
#define  ERROR_CODE_RL_MOTOR_STALL            (1<<11)
#define  ERROR_CODE_RL_RUN_TIMEOUT            (1<<12)
#define  ERROR_CODE_RB1_OT                    (1<<13)
#define  ERROR_CODE_RB1_OTH                   (1<<14)
#define  ERROR_CODE_RB2_OT                    (1<<15)
#define  ERROR_CODE_RB2_OTH                   (1<<16)














#endif