/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "app_util.h"
#include "app_fifo.h"
#include "app_error.h"
#include "app_log.h"
#include "mb.h"
#include "user_tasks.h"

#if APP_LOG_ENABLED > 0    
#undef  APP_LOG_MODULE_NAME 
#undef  APP_LOG_MODULE_LEVEL
#define APP_LOG_MODULE_NAME   "[user_tasks]"
#define APP_LOG_MODULE_LEVEL   APP_LOG_LEVEL_DEBUG    
#endif

/* Variables -----------------------------------------------------------------*/
osThreadId rx_from_ew_task_handle;      //ew = electronic weigher 下位机从电子秤接收数据的任务handle;
osThreadId rx_from_host_task_handle;    //下位机从主机接收数据的任务handle;

osThreadId tx_to_ew_task_handle;      //下位机向电子秤发送数据的任务handle;
osThreadId tx_to_host_task_handle;    //下位机向主机发送数据的任务handle;
/**************************************************************************/
app_fifo_t  fifo_rx_from_ew,fifo_rx_from_host;
uint8_t buffer_rx_from_ew[BUFFER_SIZE_RX_FROM_EW];
uint8_t buffer_rx_from_host[BUFFER_SIZE_RX_FROM_HOST];

/* ----------------------- Defines ------------------------------------------*/
#define REG_HOLDING_START           0x1000
#define REG_HOLDING_NREGS           8
#define REG_INPUT_START             0x1100
#define REG_INPUT_NREGS             8

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
/**************************************************************************/
// 
void rx_from_ew_task(void const * argument);
void rx_from_host_task(void const * argument);
void tx_to_ew_task(void const * argument);
void tx_to_host_task(void const * argument);

static void tasks_environment_init();



void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
  APP_LOG_ERROR("%s: STACK OVERFLOW!\r\n",pcTaskName );
  UNUSED_PARAMETER(xTask);
}
void vApplicationMallocFailedHook(void)
{
  APP_LOG_ERROR("APP MALLOC FAILED!\r\n" );
}




 void app_create_user_tasks(void)//创建用户任务
 {
   /*
  osThreadDef(rx_from_ew_task_val, rx_from_ew_task, osPriorityNormal, 0, 256);
  rx_from_ew_task_handle = osThreadCreate(osThread(rx_from_ew_task_val), NULL);
  
  osThreadDef(rx_from_host_task_val, rx_from_host_task, osPriorityNormal, 0, 256);
  rx_from_host_task_handle = osThreadCreate(osThread(rx_from_host_task_val), NULL);
  
  osThreadDef(tx_to_ew_task_val, tx_to_ew_task, osPriorityNormal, 0, 256);
  tx_to_ew_task_handle = osThreadCreate(osThread(tx_to_ew_task_val), NULL);
  
  osThreadDef(tx_to_host_task_val, tx_to_host_task, osPriorityNormal, 0, 256);
  tx_to_host_task_handle = osThreadCreate(osThread(tx_to_host_task_val), NULL);
  */
  osThreadDef(rx_from_host_task_val, rx_from_host_task, osPriorityNormal, 0, 256);
  rx_from_host_task_handle = osThreadCreate(osThread(rx_from_host_task_val), NULL);
 }

static void tasks_environment_init()
{
  /*
  uint32_t status;
 APP_LOG_DEBUG("任务环境初始化开始！\r\n");
  
 status = app_fifo_init(&fifo_rx_from_ew,buffer_rx_from_ew,BUFFER_SIZE_RX_FROM_EW);
 APP_ERROR_CHECK(status);
 status=app_fifo_init(&fifo_rx_from_host,buffer_rx_from_host,BUFFER_SIZE_RX_FROM_HOST);
 APP_ERROR_CHECK(status);
 */
  APP_LOG_DEBUG("任务环境初始化开始！\r\n");
  for(int i=0;i<REG_INPUT_NREGS;i++)
  {
   usRegInputBuf[i]=i;
  }
  
  
}

uint32_t parse_protocol_rx_from_ew()
{
  /*
  uint32_t status;
  uint16_t read_len;
  uint8_t temp_buf[BUFFER_SIZE_RX_FROM_EW];
  uint8_t ptr_buf;
  read_len=BUFFER_SIZE_RX_FROM_EW;
  ptr_buf=temp_buf;
  
  status=app_fifo_read(&fifo_rx_from_ew,ptr_buf,&read_len);//只会读出1帧数据
  if(status==APP_SUCCESS)
  {
    APP_LOG_DEBUG("从电子秤fifo读的数据长度:%2d\r\n");
#if APP_LOG_ENANBLED > 0
    APP_LOG_DEBUG("数据:\r\n");
    for(uint16_t i=0;i<buf_len;i++)
    {
      APP_LOG_DEBUG("%d",*(ptr_buf+i));
    }    
#endif
    if(read_size < EW_PROTOCOL_MIN_LEN)
    {
      APP_LOG_ERROR("协议数据长度错误!\r\n");
      
      return APP_ERROR_INVALID_LENGTH;
    }
    if( *ptr_buf== EW_PROTOCOL_PART1 && 
    
    
    
  }
  else
  {
   APP_LOG_ERROR("从电子秤fifo读数据出错\r\n"); 
  }
  */
  
  
  
  
  
  return 0;
}


void rx_from_ew_task(void const * argument)
{
  /*
  while(1)
  {
   osEvent =osSignalWait(SIGNAL_RX_FROM_EW_COMPLETED,osWaitForever); //无超时
   
   if( osEvent.status !=  osEventSignal)
   {
     APP_LOG_ERROR("接收通知 SIGNAL_RX_FROM_EW_COMPLETED 错误!\r\n");
   }
   else
   {
     APP_LOG_DEBUG("接收 SIGNAL_RX_FROM_EW_COMPLETED 成功!\r\n");        
     if(parse_protocol_rx_from_ew()==APP_SUCCESS)
     {
       APP_LOG_DEBUG("电子秤协议解析成功！\r\n");
     }
     else
     {
       APP_LOG_ERROR("电子秤协议解析失败！\r\n"); 
     }
       
   }
    
  }
  */
    tasks_environment_init();
    /* Select either ASCII or RTU Mode. */
    ( void )eMBInit( MB_RTU, 0x0A, 0, 115200, MB_PAR_NONE );
    /* Enable the Modbus Protocol Stack. */
    ( void )eMBEnable();
    for( ;; )
    {
      /* Call the main polling loop of the Modbus protocol stack. */
      ( void )eMBPoll();
    }
}



void rx_from_host_task(void const * argument)
{
   APP_LOG_DEBUG("MODBUS TASK START!\r\n");
  
    /* Select either ASCII or RTU Mode. */
    ( void )eMBInit( MB_RTU, 0x0A, 0, 38400, MB_PAR_EVEN );
    /* Enable the Modbus Protocol Stack. */
    ( void )eMBEnable(  );
    for( ;; )
    {
       /* Call the main polling loop of the Modbus protocol stack. */
      ( void )eMBPoll(  );
    } 
    
  
}

void tx_to_ew_task(void const * argument)
{
  
  
}

void tx_to_host_task(void const * argument)
{
  
  
}
        
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

