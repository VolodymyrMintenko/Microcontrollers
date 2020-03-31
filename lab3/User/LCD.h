#ifndef LCD_H

#define LCD_H

#include "main.h"

#define LCD_CS_ON() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET)
#define LCD_CS_OFF() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET)
#define LCD_RST_ON() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET)
#define LCD_RST_OFF() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET)
#define LCD_DC_ON() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_SET)
#define LCD_DC_OFF() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_7,GPIO_PIN_RESET)
#define LCD_SCK_ON() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET)
#define LCD_SCK_OFF()HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET)

void SysTick_Handler(void);
void delay_ms(uint16_t delay_temp);
void LCD_MO(unsigned char OnOff);
void LCD_GPIO_Config(void);
void LCD_write_byte(unsigned char dat, unsigned char mode);
void LCD_clear(void);
void LCD_init(void);
void LCD_set_XY(unsigned char X,unsigned char Y);
void LCD_Write_Dec(unsigned int b);
void LCD_write_bitmap(const uint8_t* pic);
void LCD_bitmap_lines(const uint8_t* pic);
#endif
