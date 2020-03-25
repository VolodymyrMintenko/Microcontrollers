#ifndef MAIN_H
#define MAIN_H

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "misc.h"

#define BUTTON_READ()			GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)

#endif
