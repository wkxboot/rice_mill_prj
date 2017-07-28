#include "cmsis_os.h"
#include "app_util.h"
#include "app_fifo.h"
#include "mb.h"
#include "app_log.h"
/* ----------------------- Defines ------------------------------------------*/
#define REG_HOLDING_START           0x1000
#define REG_HOLDING_NREGS           18
#define REG_INPUT_START             0x1100
#define REG_INPUT_NREGS             11




/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
/**************************************************************************/



typedef struct 
{
  uint16_t reg_addr;
  uint16_t reg_size;
  uint16_t *ptr_reg;
  ptr_regholding_write_handler_t ptr_handler;
}regholding_write_func_handler_t;


static ptr_regholding_write_handler_t[REG_HOLDING_NREGS]=
{  
/*****************************************   地址         寄存器   ************/
set_evt_rm_switch_regholding_write,        //0x1000      碾米机开关
set_evt_rb1_selection_regholding_write,    //0x1001      米仓选择

set_evt_rw_regholding_write,               //0x1002      出米重量低
set_evt_rw_regholding_write,               //0x1003      出米重量高

set_evt_rl_regholding_write,               //0x1004      碾米分度值

set_evt_rm_fault_code_regholding_write,    //0x1005      碾米机故障码低
set_evt_rm_fault_code_regholding_write,    //0x1006      碾米机故障码高

set_evt_rm_motor_switch_regholding_write,  //0x1007      碾米机马达/吹糠机
set_evt_rb1_1_switch_regholding_write,     //0x1008      1号米仓阀门
set_evt_rb1_2_switch_regholding_write,     //0x1009      2号米仓阀门
set_evt_rb2_switch_regholding_write,       //0x100a      2级米仓阀门
set_evt_uv_lamp_switch_regholding_write,   //0x100b      紫外灯开关
set_evt_e_lamp_switch_regholding_write,    //0x100c      环境灯开关
set_evt_oh_door_switch_regholding_write,   //0x100d      升降门开关
set_evt_r_tare_regholding_write,           //0x100e      去皮
set_evt_z_clearing_regholding_write,       //0x100f      清零
set_evt_w_threshold_regholding_write,      //0x1010      重量定点值低
set_evt_w_threshold_regholding_write       //0x1011      重量定点值高
};


typedef enum
{
 REGINPUT_MODE,
 REGHOLDING_MODE,
}reg_mode_t;
/**
* @brief  获取寄存器的值
* @param pucRegBuffer 
* @param 
* @return 
* @details 
* @see 
*/

uint32_t get_reg_value(uint16_t reg_addr,uint16_t reg_size,reg_mode_t reg_mode)
{
  uint32_t temp;
  uint8_t reg_idx;
  uint16_t *ptr_reg_buff;
  
 if(reg_mode== REGHOLDING_MODE)
 {
   reg_idx=reg_addr-REG_HOLDING_START;
   ptr_reg_buff=usRegHoldingBuf;
 }
 if(reg_mode== REGINPUT_MODE)
 {
   reg_idx=reg_addr-REG_INPUT_START;
    ptr_reg_buff=usRegInputBuf;
 }
   
  
  
 if(reg_size==1)
 {
 temp= usRegHoldingBuf[reg_idx] & 0xFFFF; 
 }
 else if(reg_size==2)
 {
 temp= *(uint32_t*)&ptr_reg_buff[reg_idx];
 temp= 0xFFFFFFFF & (temp>>16|temp<<16);//高低字节调整
 }
 
 return temp;
}


/**
* @brief 设置寄存器数值 
* @param reg_addr 
* @param reg_size 
* @param value 
* @return none 
* @details --
* @see --
*/
void set_reg_value(uint16_t reg_addr,uint16_t reg_size,uint32_t value,reg_mode_t reg_mode )
{
  uint32_t temp;
  uint8_t reg_idx;
  uint16_t *ptr_reg_buff;
 if(reg_mode== REGHOLDING_MODE)
   reg_idx=reg_addr-REG_HOLDING_START; 
 if(reg_mode== REGINPUT_MODE)
   reg_idx=reg_addr-REG_INPUT_START;
 
   temp=value;
  
 if(reg_size==1)
 {
    usRegHoldingBuf[reg_idx] = temp &0xFFFF; 
 }
 else if(reg_size==2)
 {
  temp= 0xFFFFFFFF & (temp>>16|temp<<16);//高低字节调整
 *(uint32_t*)&usRegHoldingBuf[reg_idx]=temp;
 }
 
}
/**
* @brief 设置错误码 
* @param -- 
* @return -- 
* @details --
* @see --
*/

void set_rm_fault_code(uint32_t err_code_bit)
{
  uint32_t err_code;
  err_code=get_reg_value( RM_FAULT_CODE_REGHOLDING_ADDR,2,REGHOLDING_MODE);
  err_code|=err_code_bit;
  set_reg_value(RM_FAULT_CODE_REGHOLDING_ADDR,2,err_code,REGHOLDING_MODE); 
}
/**
* @brief -- 
* @param -- 
* @return -- 
* @details --
* @see --
*/




/**
* @brief -- 
* @param -- 
* @return -- 
* @details --
* @see --
*/
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    
    int  iRegIndex;

    APP_LOG_INFO("read input reg! ADDR:%d NUM:%d\r\n",usAddress,usNRegs);
    
    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        APP_LOG_WARNING("read input protocol parse success!\r\n");
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                /******增加回调处理*****/
                ptr_regholding_write_handler_t[iRegIndex];
                iRegIndex++;
                usNRegs--;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}

