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
/* ----------------------- Platform includes --------------------------------*/
#include "port_m.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mbport_m.h"
#include "mbconfig_m.h"

#include "cmsis_os.h"
#include "app_log.h"

#if APP_LOG_ENABLED > 0    
#undef  APP_LOG_MODULE_NAME 
#undef  APP_LOG_MODULE_LEVEL
#define APP_LOG_MODULE_NAME   "[porttimer]"
#define APP_LOG_MODULE_LEVEL   APP_LOG_LEVEL_DEBUG    
#endif

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Variables ----------------------------------------*/

extern osTimerId MASTER_MB_timerHandle;
static void prvvTIMERExpiredISR(void);


/* ----------------------- static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortTimersInit()
{
  APP_LOG_DEBUG("MB MASTER timer init!\r\n");
    return TRUE;
}

void vMBMasterPortTimersT35Enable()
{
   osTimerStart ( MASTER_MB_timerHandle, 3);
}

void vMBMasterPortTimersConvertDelayEnable()
{
    uint32_t time = MB_MASTER_DELAY_MS_CONVERT;

   osTimerStart ( MASTER_MB_timerHandle, time);
}

void vMBMasterPortTimersRespondTimeoutEnable()
{
    uint32_t time = MB_MASTER_TIMEOUT_MS_RESPOND;

   osTimerStart ( MASTER_MB_timerHandle, time);
}

 void vMBMasterPortTimersDisable()
{
  APP_LOG_DEBUG("MB MASTER timer stop!\r\n");
  osTimerStop (MASTER_MB_timerHandle);
}

void prvvTIMERExpiredISR(void)
{
    (void) pxMBMasterPortCBTimerExpired();
}

void MASTER_MB_timer_expired_callback(void const * argument)
{
 APP_LOG_DEBUG("MB MASTER timer expired!\r\n");
 prvvTIMERExpiredISR();
}

#endif
