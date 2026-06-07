#include "init.h"
#include "fds.h"
#include "dartt_map.h"
#include "dartt.h"
#include "cobs.h"
#include "uart_mem.h"
#include "tle_encoder.h"




dartt_map_t gl_dp = {
		.angle = 0,
		.dma_adc_raw = {0},
		.fds = {
				.address = 0,
				.sin_min = 0,
				.sin_max = 0,
				.cos_min = 0,
				.cos_max = 0,
				.oerr = 0,
				.baud = 921600
		},
		.tick = 0
};

dartt_mem_t gl_dp_alias = {
		.buf = (unsigned char *)(&gl_dp),
		.size = sizeof(gl_dp)
};



/*
 * Helper function to load all the fds_mp params
 * Must go before CAN init for can ID to work properly
 * */
void load_flash_params(void)
{
	if(is_page_empty(sizeof(gl_dp.fds)/sizeof(uint32_t)) == 0)
		m_read_flash((uint32_t*)&gl_dp.fds,sizeof(fds_t)/sizeof(uint32_t));
	else
		m_write_flash((uint64_t*)&gl_dp.fds,sizeof(fds_t)/sizeof(uint64_t));
}

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	load_flash_params();	//read, write default. load before UART
	uint32_t dartt_misc_address = dartt_get_complementary_address((uint32_t)gl_dp.fds.address);
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_USART2_UART_Init();
	uint32_t led_ts = 0;


	while (1)
	{
		uint32_t tick = HAL_GetTick();

		HAL_ADC_Start_DMA(&hadc1, (uint32_t * )gl_dp.dma_adc_raw, NUM_ADC);

		/*Handle DARTT over UART*/
		if(m_huart2.rx_decoded.length != 0)
		{
			if(m_huart2.rx_decoded.buf[0] == dartt_misc_address)
			{

				//TODO: guard this in a dma disable
				gl_dp.angle = theta_rel_14b();
				//TODO: guard above

				int rc = dartt_frame_to_payload(&m_huart2.rx_decode_alias, TYPE_SERIAL_MESSAGE, PAYLOAD_ALIAS, &m_huart2.rx_pld_msg);
				if(rc == DARTT_PROTOCOL_SUCCESS)
				{
					rc = dartt_parse_general_message(&m_huart2.rx_pld_msg, TYPE_SERIAL_MESSAGE, &gl_dp_alias, &m_huart2.tx_buf_alias);
				}
				if(rc == DARTT_PROTOCOL_SUCCESS)
				{
					if(m_huart2.tx_buf_alias.len != 0)
					{
						m_huart2.tx_mem.length = m_huart2.tx_buf_alias.len;
						rc = cobs_encode_single_buffer(&m_huart2.tx_mem);
						if(rc == COBS_SUCCESS)
						{
							m_uart_dma_transmit(&m_huart2);
						}
					}
				}
			}
			else if(m_huart2.rx_decoded.buf[0] == gl_dp.fds.address)
			{
				//TODO: Guard this with some dma disable?
				gl_dp.angle = theta_rel_14b();
				//TODO: see above
				dartt_buffer_t * txb = &m_huart2.tx_buf_alias;
				txb->len = 0;
				txb->buf[txb->len++] = MASTER_MOTOR_ADDRESS;
				txb->buf[txb->len++] = gl_dp.angle & 0xFF;
				txb->buf[txb->len++] = ((gl_dp.angle & 0xFF00) >> 8);
				append_crc(txb);
				m_huart2.tx_mem.length = txb->len;
				cobs_encode_single_buffer(&m_huart2.tx_mem);
				m_uart_dma_transmit(&m_huart2);
			}
			m_huart2.rx_decoded.length = 0;
		}

		if(tick - led_ts > 333)
		{
			led_ts = tick;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		}
	}
}

