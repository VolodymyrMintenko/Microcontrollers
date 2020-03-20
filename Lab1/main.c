#include "stm32f4xx.h"                  
static __IO uint32_t usTicks;
 
void SysTick_Handler()
{
    if (usTicks != 0)
    {
        usTicks--;
    }
}
void Delayinit()
{
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);
}
 
void msDelay(uint32_t ms)
{
    usTicks = ms;
    while (usTicks);
}

void h_drv_SPI_CS_Disable (void)
{
	GPIO_SetBits(GPIOD, GPIO_Pin_7);
}

void h_drv_SPI_CS_Enable (void)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_7);
}


void h_drv_SPI_Initialization (void)
{
	GPIO_InitTypeDef 	GPIO_Init_LED;
	SPI_InitTypeDef		SPI_Init_user;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	GPIO_Init_LED.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init_LED.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init_LED.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init_LED.GPIO_OType = GPIO_OType_PP;
	GPIO_Init_LED.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_Init_LED);
	
	GPIO_Init_LED.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
	GPIO_Init_LED.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init_LED.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init_LED.GPIO_OType = GPIO_OType_PP;
	GPIO_Init_LED.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_Init_LED);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
	
	
	GPIO_Init_LED.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init_LED.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init_LED.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init_LED.GPIO_OType = GPIO_OType_PP;
	GPIO_Init_LED.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_Init_LED);
	
	h_drv_SPI_CS_Disable();
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	SPI_Init_user.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_Init_user.SPI_Mode = SPI_Mode_Master;
	SPI_Init_user.SPI_DataSize = SPI_DataSize_8b;
	SPI_Init_user.SPI_CPOL = SPI_CPOL_High;
	SPI_Init_user.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_Init_user.SPI_NSS = SPI_NSS_Soft;
	SPI_Init_user.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_Init_user.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init_user.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_Init_user);
	SPI_Cmd(SPI1, ENABLE);
}


uint8_t h_drv_SPI_Write_Byte (uint8_t Data)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}
  SPI_I2S_SendData(SPI1, Data);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}
  return SPI_I2S_ReceiveData(SPI1);
}


uint8_t h_drv_Read_Status_Register (void)
{
	uint8_t Status = 0;
	
	h_drv_SPI_CS_Enable();
	h_drv_SPI_Write_Byte(0x05);
	Status = h_drv_SPI_Write_Byte(0);
	h_drv_SPI_CS_Disable();
	return Status;
}

void send_wrdi() {
	h_drv_SPI_CS_Enable();
	h_drv_SPI_Write_Byte(0x04);		
	h_drv_SPI_CS_Disable();
}

// TODO : run on real device
void check_wren()   
{   
    uint8_t byte;      
    while((byte = h_drv_Read_Status_Register()) != 0x42) {}
}   

void write_data_initial_aai(long dest, uint8_t byte1, uint8_t byte2)
{
	h_drv_SPI_CS_Enable();
	h_drv_SPI_Write_Byte(0xAD);			/* send AAI command */
	h_drv_SPI_Write_Byte(((dest & 0xFFFFFF) >> 16)); 	/* address programming */
	h_drv_SPI_Write_Byte(((dest & 0xFFFF) >> 8));
	h_drv_SPI_Write_Byte(dest & 0xFF);
	h_drv_SPI_Write_Byte(byte1);			/* first_chunk */	
	h_drv_SPI_Write_Byte(byte2);			/* second_chunk */
	h_drv_SPI_CS_Disable();		
}

void write_data_aai(uint8_t byte1, uint8_t byte2)   
{   
    check_wren();   
    h_drv_SPI_CS_Enable();          
    h_drv_SPI_Write_Byte(0xAD);            /* send AAI command */   
    h_drv_SPI_Write_Byte(byte1);           /* send 1st byte to be programmed */   
    h_drv_SPI_Write_Byte(byte2);           /* send 2nd byte to be programmed */   
    h_drv_SPI_CS_Disable();                
}   

 
uint8_t write_full_data(long startPosition, uint8_t *data, uint8_t totalLength) {
	if ((startPosition < 0) || (startPosition> 0xFFFFFF))
		return -1; 
	if ((totalLength > 0) && ((long)(startPosition + data) <  0xFFFFFF))
	{
		write_data_initial_aai(0, *(data), *(data+1));
		for (int i = 0; i < totalLength; i=i+2) {
			write_data_aai(*(data+i),*(data+i+1));
		}
		return 0;
	}	
	return -1;
}

void EnableWrite ()
{
	h_drv_SPI_CS_Enable();
	h_drv_SPI_Write_Byte(0x06);
	h_drv_SPI_CS_Disable();
	
	h_drv_SPI_CS_Enable();
  h_drv_SPI_Write_Byte(0x50);
	h_drv_SPI_CS_Disable();
	
	h_drv_SPI_CS_Enable();
	h_drv_SPI_Write_Byte(0x01);
	h_drv_SPI_Write_Byte(0x00);
	h_drv_SPI_CS_Disable();
}

uint8_t read_byte(long add)
{
	uint8_t result = 0;
	h_drv_SPI_CS_Enable();
	h_drv_SPI_Write_Byte(0x03);
	h_drv_SPI_Write_Byte ((add>>16)&0xFF);
	h_drv_SPI_Write_Byte ((add>>8)&0xFF);
	h_drv_SPI_Write_Byte (add&0xFF);
	
	result = h_drv_SPI_Write_Byte(0x00);	
	
	h_drv_SPI_CS_Disable();
	return result;
}


int8_t read_buffer(long StartAdd, uint8_t *Data, uint8_t Len) {
if ((StartAdd < 0) || (StartAdd> 0xFFFFFF))
		return -1; 
	if ((Len > 0) && ((StartAdd+Len) <  0xFFFFFF))
	{
		for (int i = 0; i<Len; i++)
		{
			*(Data+i) = read_byte(StartAdd+i);
		}
	}	
	return -1;
}

	

void chipErase(void) {
	h_drv_SPI_CS_Enable();
	h_drv_SPI_Write_Byte(0x06);
	h_drv_SPI_CS_Disable();
	
	h_drv_SPI_CS_Enable();
	h_drv_SPI_Write_Byte(0x50);
	h_drv_SPI_CS_Disable();
}


uint8_t data[] = {1,2,3,4,5,6,7,8,9,10};
uint8_t status = 0;
uint8_t read[10] = {0};

int main()
{
	Delayinit();
	h_drv_SPI_Initialization();
	EnableWrite();
	status = h_drv_Read_Status_Register();
	
	chipErase();
	msDelay(40);
	write_full_data(0, data, sizeof(data));
	read_buffer(0, read, sizeof(read));

	while (1);	
}



