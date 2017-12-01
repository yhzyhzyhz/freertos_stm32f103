/**
    ******************************************************************************
    * @file    main.c 
    * @author  MCD Application Team
    * @version V1.0.0
    * @date    11-February-2014
    * @brief   Main program body
    ******************************************************************************
    * @attention
    *
    * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
    *
    * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
    * You may not use this file except in compliance with the License.
    * You may obtain a copy of the License at:
    *
    *        http://www.st.com/software_license_agreement_liberty_v2
    *
    * Unless required by applicable law or agreed to in writing, software 
    * distributed under the License is distributed on an "AS IS" BASIS, 
    * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    * See the License for the specific language governing permissions and
    * limitations under the License.
    *
    ******************************************************************************
    */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_nucleo.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void Debug(const char* str)
{
    while (*str != 0)
    {
        ITM_SendChar(*str);
        str++;
    }
}

/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware( void )
{
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

    /* Configure HCLK clock as SysTick clock source. */
    SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );

    STM_EVAL_LEDInit(LED2);
    Debug("INFO: HW initialized\n");
}

static void prvTestTask( void *pvParameters )
{
    while(1)
    {
        Debug("this is task test\n");
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void prvBlinkTask( void *pvParameters )
{
	TickType_t xLastWakeTime;
    while(1)
    {
    	xLastWakeTime = xTaskGetTickCount();
        // STM_EVAL_LEDToggle(LED2);
    	Debug("old blink task\n");
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 250 ) );
    }
}

void timerCallback(TimerHandle_t xTimer)
{
	STM_EVAL_LEDToggle(LED2);
}


int main(void)
{
    /* Set up the clocks and memory interface. */
    prvSetupHardware();

    TimerHandle_t timer1 = xTimerCreate("timer", pdMS_TO_TICKS(500), pdTRUE, (void*)0, timerCallback);

    xTaskCreate( prvTestTask, "test", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1 ), NULL );
    xTaskCreate( prvBlinkTask, "Blink", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 2 ), NULL);

    if(timer1 != NULL)
    {
    	 xTimerStart(timer1 , 0);
    }

    /* Start the scheduler. */
    vTaskStartScheduler();

    while(1);
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    /* This function will get called if a task overflows its stack.   If the
    parameters are corrupt then inspect pxCurrentTCB to find which was the
    offending task. */

    ( void ) pxTask;
    ( void ) pcTaskName;

    for( ;; );
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
