/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "app_util.h"
#include "app_fifo.h"
#include "app_error.h"
#include "app_log.h"
#include "user_tasks.h"

/* Variables -----------------------------------------------------------------*/
osThreadId rx_from_ew_task_handle;      //ew = electronic weigher ��λ���ӵ��ӳӽ������ݵ�����handle;
osThreadId rx_from_host_task_handle;    //��λ���������������ݵ�����handle;

osThreadId tx_to_ew_task_handle;      //��λ������ӳӷ������ݵ�����handle;
osThreadId tx_to_host_task_handle;    //��λ���������������ݵ�����handle;
/**************************************************************************/
app_fifo_t  fifo_rx_from_ew,fifo_rx_from_host;
uint8_t buffer_rx_from_ew[BUFFER_SIZE_RX_FROM_EW];
uint8_t buffer_rx_from_host[BUFFER_SIZE_RX_FROM_HOST];

typedef struct 
{
  uint8_t dev_addr; 
  uint8_t dev_opt;
  
  union 
  {
  
  }
  uint8_t 
  
}
/**************************************************************************/
// 
void rx_from_ew_task(void const * argument);
void rx_from_host_task(void const * argument);
void tx_to_ew_task(void const * argument);
void tx_to_host_task(void const * argument);





void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
  APP_LOG_ERROR("%s: STACK OVERFLOW!\r\n" );
  UNUSED_PARAMETER(xTask);
}
void vApplicationMallocFailedHook(void)
{
  APP_LOG_ERROR("APP MALLOC FAILED!\r\n" );
}




 void app_create_user_tasks(void)//�����û�����
 {
  osThreadDef(rx_from_ew_task_val, rx_from_ew_task, osPriorityNormal, 0, 256);
  rx_from_ew_task_handle = osThreadCreate(osThread(rx_from_ew_task_val), NULL);
  
  osThreadDef(rx_from_host_task_val, rx_from_host_task, osPriorityNormal, 0, 256);
  rx_from_host_task_handle = osThreadCreate(osThread(rx_from_host_task_val), NULL);
  
  osThreadDef(tx_to_ew_task_val, tx_to_ew_task, osPriorityNormal, 0, 256);
  tx_to_ew_task_handle = osThreadCreate(osThread(tx_to_ew_task_val), NULL);
  
  osThreadDef(tx_to_host_task_val, tx_to_host_task, osPriorityNormal, 0, 256);
  tx_to_host_task_handle = osThreadCreate(osThread(tx_to_host_task_val), NULL);
  

 }

static void tasks_environment_init()
{
  uint32_t status;
 APP_LOG_DEBUG("���񻷾���ʼ����ʼ��\r\n");
  
 status = app_fifo_init(&fifo_rx_from_ew,buffer_rx_from_ew,BUFFER_SIZE_RX_FROM_EW);
 APP_ERROR_CHECK(status);
 status=app_fifo_init(&fifo_rx_from_host,buffer_rx_from_host,BUFFER_SIZE_RX_FROM_HOST);
 APP_ERROR_CHECK(status);
 
  
}

uint32_t parse_protocol_rx_from_ew()
{
  uint32_t status;
  uint16_t read_len;
  uint8_t temp_buf[BUFFER_SIZE_RX_FROM_EW];
  uint8_t ptr_buf;
  read_len=BUFFER_SIZE_RX_FROM_EW;
  ptr_buf=temp_buf;
  
  status=app_fifo_read(&fifo_rx_from_ew,ptr_buf,&read_len);//ֻ�����1֡����
  if(status==APP_SUCCESS)
  {
    APP_LOG_DEBUG("�ӵ��ӳ�fifo�������ݳ���:%2d\r\n");
#if APP_LOG_ENANBLED > 0
    APP_LOG_DEBUG("����:\r\n");
    for(uint16_t i=0;i<buf_len;i++)
    {
      APP_LOG_DEBUG("%d",*(ptr_buf+i));
    }    
#endif
    if(read_size < EW_PROTOCOL_MIN_LEN)
    {
      APP_LOG_ERROR("Э�����ݳ��ȴ���!\r\n");
      
      return APP_ERROR_INVALID_LENGTH;
    }
    if( *ptr_buf== EW_PROTOCOL_PART1 && 
    
    
    
  }
  else
  {
   APP_LOG_ERROR("�ӵ��ӳ�fifo�����ݳ���\r\n"); 
  }
  
}


void rx_from_ew_task(void const * argument)
{
  
  while(1)
  {
   osEvent =osSignalWait(SIGNAL_RX_FROM_EW_COMPLETED,osWaitForever); //�޳�ʱ
   
   if( osEvent.status !=  osEventSignal)
   {
     APP_LOG_ERROR("����֪ͨ SIGNAL_RX_FROM_EW_COMPLETED ����!\r\n");
   }
   else
   {
     APP_LOG_DEBUG("���� SIGNAL_RX_FROM_EW_COMPLETED �ɹ�!\r\n");        
     if(parse_protocol_rx_from_ew()==APP_SUCCESS)
     {
       APP_LOG_DEBUG("���ӳ�Э������ɹ���\r\n");
     }
     else
     {
       APP_LOG_ERROR("���ӳ�Э�����ʧ�ܣ�\r\n"); 
     }
       
   }
    
  }
  
}

void rx_from_host_task(void const * argument)
{
  
  
}

void tx_to_ew_task(void const * argument)
{
  
  
}

void tx_to_host_task(void const * argument)
{
  
  
}
