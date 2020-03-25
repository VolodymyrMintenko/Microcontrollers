#include "main.h"
#include "pictures.h"
int main ()
{
  HAL_Init();
	LCD_init();
	LCD_set_XY(0, 0);
	LCD_bitmap_lines(message); // you can change  message
	while(1){}
}
