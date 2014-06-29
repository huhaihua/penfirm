#include "stm32f10x.h"
#include "pwm.h"

void pwm_init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure; 
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
    TIM_OCInitTypeDef  TIM_OCInitStructure;  
    
 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
 
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           // 复用推挽输出 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);     

    TIM_TimeBaseStructure.TIM_Period =0x1000;  
    TIM_TimeBaseStructure.TIM_Prescaler = 2;                                    //设置预分频：预分频=2，即为72/3=24MHz  
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                                //设置时钟分频系数：不分频  
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;                 //向上计数溢出模式  
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  
    /* PWM1 Mode configuration: Channel1 */  
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                           //配置为PWM模式1  
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  
    TIM_OCInitStructure.TIM_Pulse = 0x1000;                                       //设置通道2的电平跳变值，输出另外一个占空比的PWM  
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;                    //当定时器计数值小于CCR2时为低电平 
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);                                    //使能通道2  
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  
    TIM_ARRPreloadConfig(TIM2, ENABLE);                                         //使能TIM3重载寄存器ARR  
    /* TIM3 enable counter */  
    TIM_Cmd(TIM2, ENABLE);
    //设置，数值越小，占空比越大，当数值为0x1000（与TIM_Period相等时）占空比为0。
    TIM_SetCompare2(TIM2,0x1000);//设置占空比为0
}
    
void pwm_set(u16 DutyCycle){

	TIM_SetCompare2(TIM2,DutyCycle);//设置占空比为(1-DutyCycle/4096)
}