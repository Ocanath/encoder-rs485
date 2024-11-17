/*
 * init.h
 *
 *  Created on: Nov 16, 2024
 *      Author: ocanath
 */

#ifndef INC_INIT_H_
#define INC_INIT_H_

#include "main.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

extern UART_HandleTypeDef huart2;

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_DMA_Init(void);
void MX_ADC1_Init(void);
void MX_USART2_UART_Init(void);


#endif /* INC_INIT_H_ */
