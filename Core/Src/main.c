#include "init.h"
#include "m_uart.h"

#define NUM_ADC 2

uint16_t dma_adc_raw[NUM_ADC] = {0};


int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_USART2_UART_Init();

	HAL_ADC_Start_DMA(&hadc1, (uint32_t * )dma_adc_raw, NUM_ADC);


	uint32_t uart_tx_ts = 0;
	uint32_t led_ts = 0;
	while (1)
	{
		uint32_t tick = HAL_GetTick();

		if( (tick - uart_tx_ts) > 5)
		{
			uart_tx_ts = tick;
			m_uart_tx_start(&m_huart2, (uint8_t*)"Hello\r\n", 7);

		}


		if(tick - led_ts > 1000)
		{
			led_ts = tick;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		}
	}
}

