#include "DRV8830.h"


void Delay_DRV8830(u32 nMs)
{
    u32 i;
    for(;nMs>0;nMs--)
    {
        i=1000;
        while(i--);
    }
}

enum ENUM_TWI_BUS_STATE
{
    DRV8830_READY=0,
    DRV8830_BUS_BUSY=1,
    DRV8830_BUS_ERROR=2
};

void DRV8830_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE);


    //PA12 PA11  DRV8830 IIC SC SD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_Init(GPIOA,&GPIO_InitStructure);

    /*
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
    */

    DRV8830_SC_HIGH;
    DRV8830_SD_OUTPUT_HIGH;
    
   

}



u8 DRV8830_transfer_start(void)
{
    DRV8830_SD_OUTPUT_HIGH;
    Delay_DRV8830(1);
    DRV8830_SC_HIGH;
    Delay_DRV8830(1);

    if(!DRV8830_SD_INPUT)
    {
        return DRV8830_BUS_BUSY;
    }

    DRV8830_SD_OUTPUT_LOW;
    Delay_DRV8830(1);
    DRV8830_SC_LOW;
    Delay_DRV8830(1);

    if(DRV8830_SD_INPUT)
    {
        return DRV8830_BUS_ERROR;
    }

    return DRV8830_READY;
}


void DRV8830_transfer_stop(void) { DRV8830_SC_LOW;
    Delay_DRV8830(1);
    DRV8830_SD_OUTPUT_LOW;
    Delay_DRV8830(1);
    DRV8830_SC_HIGH;
    Delay_DRV8830(1);
    DRV8830_SD_OUTPUT_HIGH;
    Delay_DRV8830(1);

}

void DRV8830_transfer_sendack(void)
{
    DRV8830_SD_OUTPUT_LOW;
    Delay_DRV8830(1);
    DRV8830_SC_HIGH;
    Delay_DRV8830(1);
    DRV8830_SC_LOW;
    Delay_DRV8830(1);
}

u8 DRV8830_sendata(u8 data)
{
    u8 i;
    for (i = 0; i <8; i++) 
    {
        DRV8830_SC_LOW;
        Delay_DRV8830(1);
        if(data&0x80)
        {
            DRV8830_SD_OUTPUT_HIGH;
        }
        else
        {
            DRV8830_SD_OUTPUT_LOW;
        }
        data<<=1;
        Delay_DRV8830(1);

        DRV8830_SC_HIGH;
        Delay_DRV8830(1);
    }


    DRV8830_SC_LOW;
    Delay_DRV8830(1);
    
    DRV8830_SD_OUTPUT_HIGH;
    Delay_DRV8830(1);

    DRV8830_SC_HIGH;
    Delay_DRV8830(1);

    if(DRV8830_SD_INPUT) 
    {
        DRV8830_SC_LOW;
        return 1;
    }
    else
    {
        DRV8830_SC_LOW;
        return 0;
    }
}


u8 DRV8830_receive_byte_ack(void)
{
    u8 i,data;

    DRV8830_SD_OUTPUT_HIGH;
    data=0;
    for(i=0;i<8;i++)
    {
        data<<=1;
        DRV8830_SC_LOW;
        Delay_DRV8830(1);
        DRV8830_SC_HIGH;
        Delay_DRV8830(1);
        if(DRV8830_SD_INPUT)
        {
            data|=0x01;
        }
    }
    DRV8830_SC_LOW;
    Delay_DRV8830(1);

    DRV8830_SD_OUTPUT_LOW;
    Delay_DRV8830(1);
    DRV8830_SC_HIGH;
    Delay_DRV8830(1);
    DRV8830_SC_LOW;
    Delay_DRV8830(1);

    return data;

}

u8 DRV8830_receive_byte_noack(void)
{
    u16 i,data;

    DRV8830_SD_OUTPUT_HIGH;
    data=0;
    for(i=0;i<8;i++)
    {
        data<<=1;
        DRV8830_SC_LOW;
        Delay_DRV8830(1);
        DRV8830_SC_HIGH;
        Delay_DRV8830(1);
        if(DRV8830_SD_INPUT)
        {
            data|=0x01;
        }
    }
    DRV8830_SC_LOW;
    Delay_DRV8830(1);

    DRV8830_SD_OUTPUT_HIGH;
    Delay_DRV8830(1);
    DRV8830_SC_HIGH;
    Delay_DRV8830(1);
    DRV8830_SC_LOW;
    Delay_DRV8830(1);

    return data;

}

u8 DRV8830_reg_read_byte(u8 reg)
{
    u8 data;

    DRV8830_transfer_start();
    data=DRV8830_sendata(WRITE_ADD);
    data=DRV8830_sendata(reg);
    DRV8830_transfer_start();
    data=DRV8830_sendata(READ_ADD);
    data=DRV8830_receive_byte_noack();
    DRV8830_transfer_stop();
    return  data;
}


void DRV8830_reg_write(u8 reg,u8 data)
{
    DRV8830_transfer_start();
    DRV8830_sendata(WRITE_ADD);
    DRV8830_sendata(reg);
    DRV8830_sendata(data);
    DRV8830_transfer_stop();
}

void DRV2667_reg_write(u8 reg,u8 data)
{
    DRV8830_transfer_start();
    DRV8830_sendata(DRV2667_WRITE);
    DRV8830_sendata(reg);
    DRV8830_sendata(data);
    DRV8830_transfer_stop();
}

void DRV2667_start(){
	DRV2667_reg_write(0x02,0x00);
	DRV2667_reg_write(0x01,0x02);
	DRV2667_reg_write(0x03,0x01);
	DRV2667_reg_write(0x04,0x00);
	
	
	DRV2667_reg_write(0xFF,0x01);
	DRV2667_reg_write(0x00,0x05);
	DRV2667_reg_write(0x01,0x80);
	DRV2667_reg_write(0x02,0x06);
	DRV2667_reg_write(0x03,0x00);
	DRV2667_reg_write(0x04,0x09);
	DRV2667_reg_write(0x05,0x00);
	
	
	DRV2667_reg_write(0x06,0x00);
	DRV2667_reg_write(0x07,0x01);
	DRV2667_reg_write(0x08,0x01);
	DRV2667_reg_write(0x09,0x00);
	
	DRV2667_reg_write(0xFF,0x00);
	//DRV2667_reg_write(0x02,0x00);
}

void DRV2667_stop(){
	DRV2667_reg_write(0xFF,0x00);
	DRV2667_reg_write(0x02,0x00);
}

void DRV2667_set(u8 frequency, u8 Amp){
	
	DRV2667_reg_write(0xFF,0x01);
	
	
	DRV2667_reg_write(0x06,Amp);
	DRV2667_reg_write(0x07,frequency);
	
	DRV2667_reg_write(0xFF,0x00);
	DRV2667_reg_write(0x02,0x01);
}

u16 DRV8830_tmp;
void DRV8830_test(u8 direct)
{


//   	DRV8830_reg_write(FAULT_SUB_ADD,0x80);
//  	DRV8830_tmp=DRV8830_reg_read_byte(CONTROL_SUB_ADD);
//    DRV8830_reg_write(CONTROL_SUB_ADD,0x32<<2|0x1);
    DRV8830_reg_write(CONTROL_SUB_ADD,0x28<<2|direct);
 //   DRV8830_reg_write(CONTROL_SUB_ADD,0x32<<2|0x2);
   // DRV8830_reg_write(CONTROL_SUB_ADD,0x32<<2|0x3);
   // DRV8830_reg_write(CONTROL_SUB_ADD,0x32<<2|0);
   	DRV8830_tmp=DRV8830_reg_read_byte(CONTROL_SUB_ADD);
   	DRV8830_tmp=DRV8830_reg_read_byte(FAULT_SUB_ADD);

}

