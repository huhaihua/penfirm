#include "stm32f10x.h"
#include "encode.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "drv8830.h"
#include <stdlib.h>
int quan=0;
extern EventGroupHandle_t xCreatedEventGroup;
extern int direct_flag;//电机转动方向
extern u8 rounds;//电机转动圈数
//extern int curr_pos;//电机当前位置
extern int next_pos;//电机下一时刻位置
int taskdone=1;
#define BIT_0	( 1 << 0 )

void TIM1_UP_IRQHandler(void);
void ENC_init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Enable the TIM3 Update Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Timer configuration in Encoder mode */
  TIM_DeInit(TIM1);
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling 
  TIM_TimeBaseStructure.TIM_Period = TIM1_PERIOD;  
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
 
  TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, 
                             TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = ICx_FILTER;
  TIM_ICInit(TIM1, &TIM_ICInitStructure);
  
 // Clear all pending interrupts
  TIM_ClearFlag(TIM1, TIM_FLAG_Update);
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
  //Reset counter
  TIM1->CNT = COUNTER_RESET;
  
//  ENC_Clear_Speed_Buffer();
  
  TIM_Cmd(TIM1, ENABLE);  
}

void TIM1_UP_IRQHandler(void){
	/* Clear the interrupt pending flag */
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	quan=quan+direct_flag;
	if(abs(next_pos-quan)>20){
			DRV8830_test(STANDBY);
			next_pos=quan;
	}
	if((quan<=next_pos&&direct_flag==-1)||(quan>=next_pos&&direct_flag==1)){
		DRV8830_test(BREAK);
		taskdone=0;
//  				xEventGroupSetBits(
//                     xCreatedEventGroup,    /* The event group being updated. */
//                     BIT_0);/* The bits being set. */
	}
}

volatile s16 speed;
int encode_get_current_pos()
{
    return quan;
}

s16 encode_get_current_speed()
{
  static  u16   lastCount = 0;
  u16  curCount = TIM1->CNT;
  s32 dAngle = curCount - lastCount;
  if(dAngle >= MAX_STEP){
    dAngle -= TIM1_PERIOD;
  }else if(dAngle < -MAX_STEP){
    dAngle += TIM1_PERIOD;
  }
  lastCount = curCount;
  return (s16)dAngle;  
}



