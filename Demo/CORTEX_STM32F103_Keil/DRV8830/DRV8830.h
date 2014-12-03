#ifndef DRV8830_H
#define DRV8830_H

#include "stm32f10x.h"


#define DRV8830_SC_HIGH  GPIOA->BSRR  = GPIO_Pin_10
#define DRV8830_SC_LOW   GPIOA->BRR  = GPIO_Pin_10

#define DRV8830_SD_OUTPUT_HIGH   GPIOA->BSRR  = GPIO_Pin_11
#define DRV8830_SD_OUTPUT_LOW   GPIOA->BRR  = GPIO_Pin_11

#define DRV8830_SD_INPUT  (GPIOA->IDR& GPIO_Pin_11)

#define WRITE_ADD 0xC0
#define READ_ADD 0xC1
#define DRV2667_WRITE   0xB2
#define DRV2667_READ    0xB3 
#define CONTROL_SUB_ADD 0x0
#define FAULT_SUB_ADD 0x01

#define POSITIVE_DIRECTION 0x01
#define NEGATIVE_DIRECTION 0x02
#define STANDBY            0x00
#define BREAK   					 0x03

void DRV8830_init(void);
void DRV8830_transfer_stop(void);
void DRV8830_transfer_sendack(void);
u8 DRV8830_sendata(u8 data);
u8 DRV8830_receive_byte_ack(void);
u8 DRV8830_receive_byte_noack(void);
u8 DRV8830_reg_read_byte(u8 reg);
void DRV8830_reg_write(u8 reg,u8 data);
void DRV2667_reg_write(u8 reg,u8 data);
void Delay_DRV8830(u32 nMs);

void DRV8830_test(u8 direct);
void DRV2667_set(u8 frequency, u8 Amp);
void DRV2667_start();
void DRV2667_stop();




#endif
