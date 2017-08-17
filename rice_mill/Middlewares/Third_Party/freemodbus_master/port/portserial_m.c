/*******************************************************************************
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * modify by wkxboot 2017.7.11 
 * *****************************************************************************
 */
/* ----------------------- Modbus includes ----------------------------------*/

#include "port_m.h"
#include "mb_m.h"




#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stm32f1xx.h"

#include "app_log.h"

#if APP_LOG_ENABLED > 0    
#undef  APP_LOG_MODULE_NAME 
#undef  APP_LOG_MODULE_LEVEL
#define APP_LOG_MODULE_NAME   "[portserial]"
#define APP_LOG_MODULE_LEVEL   APP_LOG_LEVEL_DEBUG    
#endif
/* ----------------------- Defines ------------------------------------------*/
extern UART_HandleTypeDef huart2;

UART_HandleTypeDef * ptr_master_modbus_uart_handle =&huart2;


#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Static variables ---------------------------------*/
/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortSerialInit()
{
    APP_LOG_INFO("MB_MASTER serial init!\r\n!");
    return TRUE;
}

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
 
    if(xRxEnable)
    {
      /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(ptr_master_modbus_uart_handle, UART_IT_RXNE);
        /* Enable the UART Transmit data register empty Interrupt */
    //__HAL_UART_DISABLE_IT(ptr_master_modbus_uart_handle, UART_IT_TXE);
    
    APP_LOG_WARNING("MASTER MB [enable RX ]!\r\n"); 
    }
    else
    {
    /* Enable the UART Data Register not empty Interrupt */
   // __HAL_UART_ENABLE_IT(ptr_master_modbus_uart_handle, UART_IT_RXNE);
        /* Enable the UART Transmit data register empty Interrupt */
    __HAL_UART_DISABLE_IT(ptr_master_modbus_uart_handle, UART_IT_RXNE);
    
    APP_LOG_WARNING("MASTER MB [disable RX ]!\r\n");  
    }
   if(xTxEnable)
    {
         /* Enable the UART Data Register not empty Interrupt */
    //__HAL_UART_DISABLE_IT(ptr_master_modbus_uart_handle, UART_IT_RXNE);
        /* Enable the UART Transmit data register empty Interrupt */
    __HAL_UART_ENABLE_IT(ptr_master_modbus_uart_handle, UART_IT_TXE);
    
     APP_LOG_WARNING("MASTER MB [enable TX ]!\r\n"); 
    }
    else
    {
     /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_DISABLE_IT(ptr_master_modbus_uart_handle, UART_IT_TXE);
        /* Enable the UART Transmit data register empty Interrupt */
    //__HAL_UART_ENABLE_IT(ptr_master_modbus_uart_handle, UART_IT_TXE);
    
     APP_LOG_WARNING("MASTER MB [disable TX ]!\r\n"); 
    }
}

void vMBMasterPortClose(void)
{
     APP_LOG_WARNING(" master mb serial close!\r\n"); 
}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
    ptr_master_modbus_uart_handle->Instance->DR = ucByte;
    return TRUE;
}

BOOL xMBMasterPortSerialGetByte(CHAR * pucByte)
{
    *pucByte = (uint8_t)(ptr_master_modbus_uart_handle->Instance->DR & (uint8_t)0x00FF);
    return TRUE;
}

void MASTER_MODBUS_USARTIRQHandler( void )
{
uint32_t tmp_flag = 0, tmp_it_source = 0; 
  
  tmp_flag = __HAL_UART_GET_FLAG(ptr_master_modbus_uart_handle, UART_FLAG_RXNE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(ptr_master_modbus_uart_handle, UART_IT_RXNE);
  /* UART in mode Receiver ---------------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  { 
    pxMBMasterFrameCBByteReceived();
  }

  tmp_flag = __HAL_UART_GET_FLAG(ptr_master_modbus_uart_handle, UART_FLAG_TXE);
  tmp_it_source = __HAL_UART_GET_IT_SOURCE(ptr_master_modbus_uart_handle, UART_IT_TXE);
  /* UART in mode Transmitter ------------------------------------------------*/
  if((tmp_flag != RESET) && (tmp_it_source != RESET))
  {
    pxMBMasterFrameCBTransmitterEmpty();
  }  
}
#endif
