#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim3;

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}
