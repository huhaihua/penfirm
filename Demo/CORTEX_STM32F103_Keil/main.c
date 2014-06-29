/*
    FreeRTOS V8.0.1 - Copyright (C) 2014 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the standard demo application tasks.
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Fast Interrupt Test" - A high frequency periodic interrupt is generated
 * using a free running timer to demonstrate the use of the
 * configKERNEL_INTERRUPT_PRIORITY configuration constant.  The interrupt
 * service routine measures the number of processor clocks that occur between
 * each interrupt - and in so doing measures the jitter in the interrupt timing.
 * The maximum measured jitter time is latched in the ulMaxJitter variable, and
 * displayed on the LCD by the 'Check' task as described below.  The
 * fast interrupt is configured and handled in the timertest.c source file.
 *
 * "LCD" task - the LCD task is a 'gatekeeper' task.  It is the only task that
 * is permitted to access the display directly.  Other tasks wishing to write a
 * message to the LCD send the message on a queue to the LCD task instead of
 * accessing the LCD themselves.  The LCD task just blocks on the queue waiting
 * for messages - waking and displaying the messages as they arrive.
 *
 * "Check" task -  This only executes every five seconds but has the highest
 * priority so is guaranteed to get processor time.  Its main function is to
 * check that all the standard demo tasks are still operational.  Should any
 * unexpected behaviour within a demo task be discovered the 'check' task will
 * write an error to the LCD (via the LCD task).  If all the demo tasks are
 * executing with their expected behaviour then the check task writes PASS
 * along with the max jitter time to the LCD (again via the LCD task), as
 * described above.
 *
 */

/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

/* Library includes. */

/* Demo app includes. */
// #include "lcd.h"
// #include "LCD_Message.h"
// #include "BlockQ.h"
 #include "death.h"
// #include "integer.h"
// #include "blocktim.h"
 #include "partest.h"
// #include "semtest.h"
// #include "PollQ.h"
// #include "flash.h"
// #include "comtest2.h"
#include "serial.h"
#include "adc.h"
#include "DRV8830.h"
#include "encode.h"
#include "pwm.h"

/* Task priorities. */
#define mainBULETOOH_TASK_PRIORITY			( tskIDLE_PRIORITY + 4 )
#define mainADC_TASK_PRIORITY						( tskIDLE_PRIORITY + 4 )
#define mainVibration_TASK_PRIORITY			( tskIDLE_PRIORITY + 4 )
#define mainDRIVERMOT_TASK_PRIORITY			( tskIDLE_PRIORITY + 3 )

/* Constants related to the LCD. */
// #define mainMAX_LINE						( 240 )
// #define mainROW_INCREMENT					( 24 )
// #define mainMAX_COLUMN						( 20 )
// #define mainCOLUMN_START					( 319 )
// #define mainCOLUMN_INCREMENT 				( 16 )

/* The maximum number of message queue length. */
#define mainUART_QUEUE_SIZE					( 40 )

/* The check task uses the sprintf function so requires a little more stack. */
#define mainCHECK_TASK_STACK_SIZE			( configMINIMAL_STACK_SIZE + 50 )

/* Dimensions the buffer into which the jitter time is written. */
//#define mainMAX_MSG_LEN						25

/* The time between cycles of the 'check' task. */
//#define mainCHECK_DELAY						( ( TickType_t ) 5000 / portTICK_PERIOD_MS )

/* The number of nano seconds between each processor clock. */
//#define mainNS_PER_CLOCK ( ( unsigned long ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )

/* Baud rate used by the comtest tasks. */
#define mainCOM_TEST_BAUD_RATE		( 9600 )

/* The LED used by the comtest tasks. See the comtest.c file for more
information. */
//#define mainCOM_TEST_LED			( 3 )

#define serNO_BLOCK						( ( TickType_t ) ( 50 / portTICK_RATE_MS ) )

/* Declare a variable to hold the created event group. */
EventGroupHandle_t xCreatedEventGroup;
#define BIT_0	( 1 << 0 )
#define BIT_1	( 1 << 1 )

/*-----------------------------------------------------------*/

/*
 * Configure the clocks, GPIO and other peripherals as required by the demo.
 */
static void prvSetupHardware( void );

/*
 * Configure the LCD as required by the demo.
 */
//static void prvConfigureLCD( void );

/*
 * The LCD is written two by more than one task so is controlled by a
 * 'gatekeeper' task.  This is the only task that is actually permitted to
 * access the LCD directly.  Other tasks wanting to display a message send
 * the message to the gatekeeper.
 */
//static void vLCDTask( void *pvParameters );

/*
 * Retargets the C library printf function to the USART.
 */
//int fputc( int ch, FILE *f );

/*
 * User Tasks Defination
 */
static void vBluetoohTask( void *pvParameters );
static void vadcTask( void *pvParameters );
static void vVibrationTask(void *pvParameters);
static void vDrivemotorTask(void *pvParameters);
/*
 * Configures the timers and interrupts for the fast interrupt test as
 * described at the top of this file.
 */
//extern void vSetupTimerTest( void );


int direct_flag=0;//电机转动方向
u8 rounds=0;//电机转动圈数
int curr_pos=0;//电机当前位置
int next_pos=0;//电机下一时刻位置
extern int taskdone;
TaskHandle_t xTask;
/*-----------------------------------------------------------*/

int main( void )
{
#ifdef DEBUG
  debug();
#endif

	prvSetupHardware();
	/* Attempt to create the event group. */
  xCreatedEventGroup = xEventGroupCreate();

	/* Start the standard demo tasks. */
// 	vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );
//     vCreateBlockTimeTasks();
//     vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );
//     vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
//     vStartIntegerMathTasks( mainINTEGER_TASK_PRIORITY );
// 	vStartLEDFlashTasks( mainFLASH_TASK_PRIORITY );
// 	vAltStartComTestTasks( mainCOM_TEST_PRIORITY, mainCOM_TEST_BAUD_RATE, mainCOM_TEST_LED );
	
	/* Was the event group created successfully? */
    if( xCreatedEventGroup == NULL )
    {
        /* The event group was not created because there was insufficient
        FreeRTOS heap available. */
			  return 0;
    }
    else
    {
        /* The event group was created. */
				/* Start the tasks defined within this file/specific to this demo. */
				xTaskCreate( vBluetoohTask, "bluetooh", mainCHECK_TASK_STACK_SIZE, NULL, mainBULETOOH_TASK_PRIORITY, NULL );
				xTaskCreate( vadcTask, "ADC", configMINIMAL_STACK_SIZE, NULL, mainADC_TASK_PRIORITY, NULL );
				xTaskCreate( vVibrationTask, "Vibration", configMINIMAL_STACK_SIZE, &xTask, mainVibration_TASK_PRIORITY, &xTask );
				/* Start the scheduler. */
				vTaskStartScheduler();
    }

	/* Will only get here if there was not enough heap space to create the
	idle task. */
	return 0;
}
/*-----------------------------------------------------------*/

void vBluetoohTask( void *pvParameters )
{
	u8 rx;
	const TickType_t xTicksToWait = 1000 / portTICK_PERIOD_MS;
	EventBits_t uxBits;
	portTickType xLastWakeTime;
	

	for( ;; )
	{
//  vSerialPutString( USART2, "hello\n", 6 );
		if(xSerialGetChar(USART2,&rx,serNO_BLOCK)==pdTRUE){
				
				if(xSerialGetChar(USART2,&rounds,serNO_BLOCK)==pdTRUE){
						curr_pos=encode_get_current_pos();
						if(rx==0xfd){//振动
								vTaskResume(xTask);
								continue;
						}	
						if(rx==0xff){//正向转动
								direct_flag=1;
								next_pos=next_pos+direct_flag*rounds;
								if(next_pos>curr_pos){
									xSerialPutChar( USART2,'a',0);
									DRV8830_test(POSITIVE_DIRECTION);
									while(taskdone);
									taskdone=1;
								}								
						}
						if(rx==0xfe){//反向转动
								direct_flag=-1;
								next_pos=next_pos+direct_flag*rounds;
								if(next_pos<curr_pos){
									xSerialPutChar( USART2,'b',0);
									DRV8830_test(NEGATIVE_DIRECTION);
									while(taskdone);
									taskdone=1;
								}								
						}
						if(rx==0xfc){//归零								
								next_pos=0;
								if(next_pos<curr_pos){
									direct_flag=-1;
									xSerialPutChar( USART2,'d',0);
									DRV8830_test(NEGATIVE_DIRECTION);
									while(taskdone);
									taskdone=1;
								}else if(next_pos>curr_pos){
									direct_flag=1;
									xSerialPutChar( USART2,'e',0);
									DRV8830_test(POSITIVE_DIRECTION);
									while(taskdone);
									taskdone=1;
								}else{
									xSerialPutChar( USART2,'f',0);
								}									
						}
//						vTaskDelayUntil( &xLastWakeTime, ( 1000 / portTICK_RATE_MS ) );
//						DRV8830_test(STANDBY);
//						xTaskCreate( vGetmotorPosTask, "GetmotorPos", configMINIMAL_STACK_SIZE, NULL, mainGETPOS_TASK_PRIORITY, &xTaskHandle );
						/* Wait a maximum of 1000ms for either bit 0 to be set within
						the event group.  Clear the bits before exiting. */	
// 						uxBits = xEventGroupWaitBits(
// 												xCreatedEventGroup,   /* The event group being tested. */
// 												BIT_0 , 				 /* The bits within the event group to wait for. */
// 												pdTRUE,        /* BIT_0 and BIT_4 should be cleared before returning. */
// 												pdFALSE,       /* Don't wait for both bits, either bit will do. */
// 												xTicksToWait );/* Wait a maximum of 1000ms for either bit to be set. */
// 						DRV8830_test(STANDBY);
// 						if( ( uxBits & BIT_0 ) == BIT_0 )
// 						{
// 								/* xEventGroupWaitBits() returned because both bits were set. */
// 								vSerialPutString( USART2, "Task run success\n", 17 );
// //								 vTaskDelete(xTaskHandle);
// 						}else{
// 								vSerialPutString( USART2, "Task run error\n", 15 );
// //								DRV8830_test(STANDBY);
// //								vTaskDelete(xTaskHandle);
//						}
						
				}
		}
//		xSerialPutChar(USART2,rx,serNO_BLOCK);// for test

	}
}
/*-----------------------------------------------------------*/

float ADValue1=0.0,ADValue2=0.0,ADValue3=0.0;

void vadcTask(void *pvParameters)
{
    int i;
portTickType xLastWakeTime;
xLastWakeTime = xTaskGetTickCount();
    for(;;)
    {
				ADC_RegularChannelConfig(ADC1,ADC_Channel_4,1,ADC_SampleTime_55Cycles5);
        ADC_SoftwareStartConvCmd(ADC1,ENABLE);
        vTaskDelayUntil( &xLastWakeTime, ( 2 / portTICK_RATE_MS ) );
        ADValue1 = VREF*ADC_GetConversionValue(ADC1)/0x0fff;

				ADC_RegularChannelConfig(ADC1,ADC_Channel_5,1,ADC_SampleTime_55Cycles5);			
			  ADC_SoftwareStartConvCmd(ADC1,ENABLE);
        vTaskDelayUntil( &xLastWakeTime, ( 2 / portTICK_RATE_MS ) );
        ADValue2 = VREF*ADC_GetConversionValue(ADC1)/0x0fff;

				ADC_RegularChannelConfig(ADC1,ADC_Channel_6,1,ADC_SampleTime_55Cycles5);
				ADC_SoftwareStartConvCmd(ADC1,ENABLE);
        vTaskDelayUntil( &xLastWakeTime, ( 2 / portTICK_RATE_MS ) );
        ADValue3 = VREF*ADC_GetConversionValue(ADC1)/0x0fff;
			
				vTaskDelayUntil( &xLastWakeTime, ( 200 / portTICK_RATE_MS ) );
        
    }
}
/*-----------------------------------------------------------*/

void vVibrationTask(void *pvParameters)
{ 
portTickType xLastWakeTime;
xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
				vTaskSuspend(xTask);
				xSerialPutChar( USART2,'c',0);
				pwm_set(0x0001);
				vTaskDelay(( 100 / portTICK_RATE_MS ) );
				pwm_set(0x1000);

    }
}
/*-----------------------------------------------------------*/

void vDrivemotorTask(void *pvParameters){
	
}

/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Start with the clocks in their expected state. */
	RCC_DeInit();

	/* Enable HSE (high speed external clock). */
	RCC_HSEConfig( RCC_HSE_ON );

	/* Wait till HSE is ready. */
	while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) == RESET )
	{
	}

	/* 2 wait states required on the flash. */
	*( ( unsigned long * ) 0x40022000 ) = 0x02;

	/* HCLK = SYSCLK */
	RCC_HCLKConfig( RCC_SYSCLK_Div1 );

	/* PCLK2 = HCLK */
	RCC_PCLK2Config( RCC_HCLK_Div1 );

	/* PCLK1 = HCLK/2 */
	RCC_PCLK1Config( RCC_HCLK_Div2 );

	/* PLLCLK = 8MHz * 9 = 72 MHz. */
	RCC_PLLConfig( RCC_PLLSource_HSE_Div1, RCC_PLLMul_9 );

	/* Enable PLL. */
	RCC_PLLCmd( ENABLE );

	/* Wait till PLL is ready. */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}

	/* Select PLL as system clock source. */
	RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );

	/* Wait till PLL is used as system clock source. */
	while( RCC_GetSYSCLKSource() != 0x08 )
	{
	}

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
							| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE );

	/* SPI2 Periph clock enable */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );


	/* Set the Vector Table base address at 0x08000000 */
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0 );

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );

	vParTestInitialise();
	//初始化串口
	xSerialPortInitMinimal( mainCOM_TEST_BAUD_RATE, mainUART_QUEUE_SIZE );
	
	//初始化其他外围设备
	adc_init();
	ENC_init();
	DRV8830_init();
	pwm_init();

}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

// int fputc( int ch, FILE *f )
// {

// 	return ch;
// }
/*-----------------------------------------------------------*/

#ifdef  DEBUG
/* Keep the linker happy. */
void assert_failed( unsigned char* pcFile, unsigned long ulLine )
{
	for( ;; )
	{
	}
}
#endif
