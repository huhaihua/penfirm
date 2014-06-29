#ifndef ENC_H
#define ENC_H



#define TIM1_PERIOD 0x0640
#define MAX_STEP TIM1_PERIOD+300
#define COUNTER_RESET   (u16)0
#define ICx_FILTER      (u8) 6 // 6<-> 670nsec




void ENC_init(void);
s16 encode_get_current_speed();
int encode_get_current_pos();

#endif
