#include "LCD.h"

uint16_t delay_count=0;

void SysTick_Handler(void)//1ms
{
	if (delay_count > 0){delay_count--;}
}

void delay_ms(uint16_t delay_temp)
{
	delay_count = delay_temp;
	while(delay_count){}
}

void LCD_MO(unsigned char OnOff)
{
	if (OnOff)
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
}

void LCD_GPIO_Config(void) 
{
	__GPIOA_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	GPIO_InitTypeDef GPIOA_Init;
	GPIO_InitTypeDef GPIOC_Init;
	GPIOA_Init.Mode = GPIO_MODE_OUTPUT_PP; 
	GPIOA_Init.Pull = GPIO_NOPULL;
	GPIOA_Init.Pin = GPIO_PIN_8; 
	GPIOA_Init.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOA, &GPIOA_Init);
	GPIOC_Init.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
	GPIOC_Init.Mode = GPIO_MODE_OUTPUT_PP; 
	GPIOC_Init.Pull = GPIO_NOPULL;
	GPIOC_Init.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC, &GPIOC_Init);
}
void LCD_write_byte(unsigned char dat, unsigned char mode)
{
	unsigned char i;
	LCD_CS_OFF();
	if (!mode) LCD_DC_OFF(); else LCD_DC_ON(); 
	for(i = 0; i < 8; i++)
	{
		LCD_MO(dat & 0x80);
		dat = dat<<1;
		LCD_SCK_OFF();
		LCD_SCK_ON();
	}
	LCD_CS_ON(); 
}
void LCD_clear()
{
  unsigned char i,j;
  for(i=0;i<6;i++)
    for(j=0;j<84;j++)
      LCD_write_byte(0,1);
}
void LCD_write_bitmap(const uint8_t* pic){
	unsigned char i,j;
  for(i=0;i<6;i++)
    for(j=0;j<84;j++)
      LCD_write_byte(pic[i*84+j],1);
}
void LCD_bitmap_lines(const uint8_t* pic){
	unsigned char i,j;
  for(i=0;i<6;i++)
    for(j=0;j<84;j++)
      LCD_write_byte(pic[i*84+j]&0xAA,1);
}

void LCD_init()
{
	LCD_GPIO_Config();
	LCD_DC_ON();
	LCD_MO(1);
	LCD_SCK_ON();
	LCD_CS_ON();
	LCD_RST_OFF() ;
	delay_ms(40);
	LCD_RST_ON();
	LCD_write_byte(0x21,0);
	LCD_write_byte(0xc6,0);
	LCD_write_byte(0x06,0);
	LCD_write_byte(0x13,0);
	LCD_write_byte(0x20,0);
	LCD_clear();
	LCD_write_byte(0x0c,0);
}

void LCD_set_XY(unsigned char X,unsigned char Y)
{
  unsigned char x;
  x = 6*X;

  LCD_write_byte(0x40|Y,0);
  LCD_write_byte(0x80|x,0);
}
