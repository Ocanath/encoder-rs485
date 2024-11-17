#include "init.h"
#include "m_uart.h"
#include "PPP.h"

#define NUM_ADC 2

#define DUMMY_ADDRESS 0x0001

uint16_t dma_adc_raw[NUM_ADC] = {0};

static uint8_t request_received = 0;

void ppp_rx_cplt_callback(void)
{
	uint16_t * pbu16 = (uint16_t*)(&m_huart2.ppp_unstuff_buf[0]);
	if(pbu16[0] == DUMMY_ADDRESS)
	{
		request_received = 1;
	}
}

/*
Generic hex checksum calculation.
TODO: use this in the psyonic API
 */
uint16_t get_checksum16(uint16_t* arr, int size)
{
	int16_t checksum = 0;
	for (int i = 0; i < size; i++)
		checksum += (int16_t)arr[i];
	return -checksum;
}


int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_USART2_UART_Init();

	uint32_t led_ts = 0;

	uint16_t uart_tx_buf[1+NUM_ADC+1] = {0};	//8 bytes. Two address, four payload, two checksum
	uint8_t uart_stuff_buf[ sizeof(uart_tx_buf) * 2 + 2] = {0};

	while (1)
	{
		uint32_t tick = HAL_GetTick();

		HAL_ADC_Start_DMA(&hadc1, (uint32_t * )dma_adc_raw, NUM_ADC);

		if(request_received != 0)
		{
			uart_tx_buf[0] = DUMMY_ADDRESS;
			uart_tx_buf[1] = dma_adc_raw[0];
			uart_tx_buf[2] = dma_adc_raw[1];
			uart_tx_buf[3] = get_checksum16(uart_tx_buf, 3);
			int num_bytes = PPP_stuff((uint8_t*)uart_tx_buf, sizeof(uart_tx_buf), uart_stuff_buf, sizeof(uart_stuff_buf));
			m_uart_tx_start(&m_huart2, uart_stuff_buf, num_bytes);
			request_received = 0;
		}

		if(tick - led_ts > 1000)
		{
			led_ts = tick;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		}
	}
}

