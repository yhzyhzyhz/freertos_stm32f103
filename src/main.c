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
#include "semphr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
SemaphoreHandle_t ButtonSemaphore;

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

static void prvSetupHardware( void )
{
	// required by FreeRTOS
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

    /*
     * RCC setup
     *
     * HSE = 8MHz
     * PLL = HES*9 = 72MHz
     * SYSCLK = PLL = 72MHz
     * HCLK = SYSCLK = 72MHz
     * PCLK1 = HCLK/2 = 36MHz
     * PCLK2 = HCLK = 72MHz
     * ADCCLK = PCLK2/6 = 12MHz
     */
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    /* SysTick end of count event each 1ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

    STM_EVAL_LEDInit(LED2);
    STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
    Debug("INFO: HW initialized\n");
}

static void TestTask( void *pvParameters )
{
    for (;;)
    {
        Debug("---this is task test\n");
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

static void BlinkTask( void *pvParameters )
{
    TickType_t xLastWakeTime;
    for (;;)
    {
        xLastWakeTime = xTaskGetTickCount();
        // STM_EVAL_LEDToggle(LED2);
        Debug("---old blink task\n");
        vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS( 250 ) );
    }
}

static void TimerCallback(TimerHandle_t xTimer)
{
    STM_EVAL_LEDToggle(LED2);
}

static void ButtonInterruptTask(void *pvParameters)
{
    for(;;)
    {
        if(xSemaphoreTake(ButtonSemaphore, pdMS_TO_TICKS(1000)) == pdPASS)
        {
            Debug("Button pressed\n");
        }
        else
        {
            Debug("Button time out\n");
        }
    }
}

int main(void)
{
    /* Set up the clocks and memory interface. */
    prvSetupHardware();

    ButtonSemaphore = xSemaphoreCreateBinary();

    TimerHandle_t timer1 = xTimerCreate("timer", pdMS_TO_TICKS(500), pdTRUE, (void*)0, TimerCallback);

    xTaskCreate( TestTask, "test", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 1 ), NULL );
    xTaskCreate( BlinkTask, "Blink", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 2 ), NULL);

    if(timer1 != NULL)
    {
         xTimerStart(timer1 , 0);
    }

    if (ButtonSemaphore != NULL)
    {
        xTaskCreate(ButtonInterruptTask, "button", configMINIMAL_STACK_SIZE, NULL, ( tskIDLE_PRIORITY + 3 ), NULL);
    }

    /* Start the scheduler. */
    vTaskStartScheduler();

    for (;;);
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
