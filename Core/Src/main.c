#include "init.h"
#include "m_uart.h"




int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_USART2_UART_Init();

	uint32_t uart_tx_ts = 0;
	while (1)
	{
		uint32_t tick = HAL_GetTick();

		if( (tick - uart_tx_ts) > 15)
		{
			uart_tx_ts = tick;
			m_uart_tx_start(&m_huart2, (uint8_t*)"Hello\n", 6);
		}

	}
}

