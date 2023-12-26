#include "bsp.h"

#define FIFO_DBS_0 (62 * 1024) // 62K * 4 = 248KBpts

ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) uint16_t src_fifo0_0[FIFO_DBS_0]);
// ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) uint16_t src_fifo0_1[FIFO_DBS_0]);
// ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) uint16_t src_fifo0_2[FIFO_DBS_0]);
// ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) uint16_t src_fifo0_3[FIFO_DBS_0]);
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) static uint16_t x[700]);
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) static uint16_t y[700]);
static uint8_t cache_fifo[2][700];
uint16_t disp_data[2][700];
uint16_t cnt_success, cnt_fail = 0;

static unsigned char osc_sample_mode = 0;
static unsigned int osc_flag_dma_tc = 0;
uint8_t os_trig_start_flag = 0;
uint16_t dma_trig_ndtr = 0;

static TIM_HandleTypeDef TIM_Handle;

static int bsps_sa_init(void);
void osc_curve_test(void);
void bsps_sa_dma_restart(void);
static void bsps_sa_dma_init(unsigned char freq_mode, unsigned char trig_mode, unsigned int PDA_DPH, unsigned int NORC_Mode);

OS_INIT_REGISTER("sa_init", bsps_sa_init, 0, 3);

// OS_TSK_REGISTER(osc_curve_test,PRIORITY_3,1000);

static int bsps_sa_init(void)
{
	// 等待RC电路充电完成 !必要
	HAL_Delay(100);

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_Handle;
	TIM_OC_InitTypeDef TIM_OC_Handle;
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	memset(&TIM_OC_Handle, 0, sizeof(TIM_OC_Handle));
	memset(&sMasterConfig, 0, sizeof(sMasterConfig));

	/*	PA10 --> IO_CLK2
		PE9  --> IO_CLK1  */

	GPIO_Handle.Pin = GPIO_PIN_9;
	GPIO_Handle.Mode = GPIO_MODE_AF_PP;
	GPIO_Handle.Pull = GPIO_NOPULL;
	GPIO_Handle.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_Handle.Alternate = GPIO_AF1_TIM1;
	HAL_GPIO_Init(GPIOE, &GPIO_Handle);
	GPIO_Handle.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOA, &GPIO_Handle);

	GPIO_Handle.Pin = 0xffff;
	GPIO_Handle.Mode = GPIO_MODE_INPUT;
	GPIO_Handle.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_Handle);

	TIM_Handle.Instance = TIM1;
	TIM_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TIM_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM_Handle.Init.Period = 2 - 1;
	TIM_Handle.Init.Prescaler = 14 * 10 - 1;
	HAL_TIM_PWM_Init(&TIM_Handle);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_ENABLE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&TIM_Handle, &sMasterConfig) != HAL_OK)
	{
	}

	TIM_OC_Handle.OCMode = TIM_OCMODE_PWM1;
	TIM_OC_Handle.OCPolarity = TIM_OCPOLARITY_LOW;
	TIM_OC_Handle.Pulse = 1;

	HAL_TIM_PWM_ConfigChannel(&TIM_Handle, &TIM_OC_Handle, TIM_CHANNEL_1);
	HAL_TIM_PWM_ConfigChannel(&TIM_Handle, &TIM_OC_Handle, TIM_CHANNEL_3);

	__HAL_TIM_ENABLE_DMA(&TIM_Handle, TIM_DMA_CC1);

	HAL_TIM_PWM_Start(&TIM_Handle, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TIM_Handle, TIM_CHANNEL_3);

	bsps_sa_dma_init(0, 1, DMA_PDATAALIGN_HALFWORD, DMA_CIRCULAR);

	bsps_time_psc_set(3);

	return OS_OK;
}

static void MX_DMA_NVIC_Init(unsigned char mode)
{
	__HAL_RCC_DMA1_CLK_ENABLE();
	__HAL_RCC_DMA2_CLK_ENABLE();
	if (mode == 0)
	{
		HAL_NVIC_DisableIRQ(DMA2_Stream0_IRQn);
	}
	else
	{
		HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 1, 1);
		HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	}
}

static void bsps_sa_dma_init(unsigned char freq_mode, unsigned char trig_mode, unsigned int PDA_DPH, unsigned int NORC_Mode)
{
	/* Define initialization structure */
	DMA_HandleTypeDef hdma_cho;
	/* Set sampling frequency */
	MX_DMA_NVIC_Init(trig_mode);
	/* fill with blank */
	memset(&hdma_cho, 0, sizeof(hdma_cho));
	/* check mode */
	hdma_cho.Instance = DMA2_Stream0;
	hdma_cho.Init.Request = DMA_REQUEST_TIM3_CH4;
	hdma_cho.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_cho.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_cho.Init.MemInc = DMA_MINC_ENABLE;
	hdma_cho.Init.PeriphDataAlignment = PDA_DPH;
	hdma_cho.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_cho.Init.Mode = NORC_Mode; // DMA_NORMAL;//DMA_CIRCULAR;
	hdma_cho.Init.Priority = DMA_PRIORITY_VERY_HIGH;
	hdma_cho.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
	hdma_cho.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	hdma_cho.Init.MemBurst = DMA_MBURST_SINGLE;
	hdma_cho.Init.PeriphBurst = DMA_PBURST_SINGLE;
	/* enable */
	if (freq_mode == 0)
	{
		hdma_cho.Init.Request = DMA_REQUEST_TIM1_CH1;
	}
	/* INIT */
	HAL_DMA_Init(&hdma_cho);
	/* start DMA with buffer */
	unsigned int len_NDTR = FIFO_DBS_0 / 2; // test mode ignore the 8 byte mode
	/* start */
	if (freq_mode == 0)
	{
		HAL_DMA_Start_IT(&hdma_cho, (uint32_t)&GPIOD->IDR, src_fifo0_0, FIFO_DBS_0); // test , mode
		DMA2_Stream0->CR |= (1 << 3);														   // 半传输完成中断开启
		return;
	}
}

void bsps_sa_dma_restart(void)
{
	DMA1->LIFCR = DMA_FLAG_TCIF0_4;
	DMA1->LIFCR = DMA_FLAG_TCIF1_5;
	DMA2->LIFCR = DMA_FLAG_TCIF0_4;
	DMA2->LIFCR = DMA_FLAG_TCIF1_5;
	if (osc_sample_mode == 0)
	{
		DMA2->LIFCR = DMA_FLAG_HTIF0_4;
		DMA2_Stream0->NDTR = FIFO_DBS_0;
		DMA2_Stream0->CR |= 0x1;
	}
	else
	{
		DMA1_Stream0->NDTR = FIFO_DBS_0 / 2; // 传输总数量设置
		DMA1_Stream1->NDTR = FIFO_DBS_0 / 2;
		DMA2_Stream0->NDTR = FIFO_DBS_0 / 2;
		DMA2_Stream1->NDTR = FIFO_DBS_0 / 2;

		DMA1_Stream0->CR |= 0x1; // 开启DMA传输
		DMA1_Stream1->CR |= 0x1;
		DMA2_Stream0->CR |= 0x1;
		DMA2_Stream1->CR |= 0x1;
	}
	TIM1->CR1 |= 0x1;
}

void OSC_DMA_SET_TC(void)
{
	osc_flag_dma_tc = 1;
}

void OSC_DMA_CLEAR_TC(void)
{
	osc_flag_dma_tc = 0;
}

char OSC_DMA_CHECK_TC(void)
{
	return osc_flag_dma_tc;
}

void OSC_DMA_STOP(void)
{
	TIM3->CR1 &= ~0x1;
	TIM1->CR1 &= ~0x1;
	TIM1->CNT = 0;
	TIM3->CNT = 0;
}

void osc_copy_from_fifo(unsigned char *data, unsigned char chn, unsigned int start_pos, unsigned int len)
{
	/* Judge the current sampling mode*/
	if (osc_sample_mode == 0)
	{
		/* base src */
		unsigned int base_addr = (unsigned int)&src_fifo0_0[start_pos];
		/* chn */
		unsigned char *chn_base = (chn == 0) ? (unsigned char *)(base_addr + 1) : (unsigned char *)base_addr;
		/* low freq mode and data is fifo_0 */
		for (int i = 0; i < len; i++)
		{
			/* copy data at stupy */
			data[i] = *chn_base;
			/* increamer */
			chn_base += 2;
			/* enf of that */
		}
	}
	else
	{
		/* double and fifth buffer mode */
		unsigned int ind = 0, idf = 0, inc = 0;
		/* copy data */
		for (int i = start_pos; i < start_pos + len; i++)
		{
			/* Get row and column information */
			ind = i % 4;
			idf = i / 4;
			/* Get row and column information */
			switch (ind)
			{
			case 0:
				data[inc] = (unsigned char)(src_fifo0_0[idf] >> 8);
				break;
				//				case 1:
				//					data[inc] = (unsigned char)(src_fifo0_1[idf] >> 8);
				//					break;
				//				case 2:
				//					data[inc] = (unsigned char)(src_fifo0_2[idf] >> 8);
				//					break;
				//				case 3:
				//					data[inc] = (unsigned char)(src_fifo0_3[idf] >> 8);
				//					break;
			default:
				break;
			}
			/* increate */
			inc++;
		}
	}
}

unsigned char dstg[1024];

void osc_curve_test(void)
{
	DISABLE_INT();
	bsp_lcd_clr_scr(CL_BLACK);
	osc_copy_from_fifo(dstg, 1, 8196, 1024);
	for (int i = 0; i < 1024; i++)
	{
		// printf("%.3f\r\n", (dstg[i]) * 1.25 / 256.0 - 0.625);
		y[i] = ((dstg[i]) * 1.25 / 256.0 - 0.625) * 100 + 200;
	}
	bsp_lcd_draw_lines(x, y, 700, CL_YELLOW);
	ENABLE_INT();
}

// 设置分频系数控制采样率
void bsps_sa_set_psc(unsigned int psc)
{
	TIM1->PSC = psc;
}

// 设置触发使能标志位
void bsps_sa_trig_exti_set(uint8_t flag)
{
	if (flag)
	{
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
	else
	{
		HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
	}
}

// 设置触发边沿
void bsps_sa_trig_edge_set(uint8_t mode)
{
	if (mode == TRIG_EDGE_RISE)
	{
		EXTI->RTSR1 |= ((1 << 10) | (1 << 11));
		EXTI->FTSR1 &= ~((1 << 10) | (1 << 11));
	}
	else if (mode == TRIG_EDGE_FALL)
	{
		EXTI->FTSR1 |= ((1 << 10) | (1 << 11));
		EXTI->RTSR1 &= ~((1 << 10) | (1 << 11));
	}
	else if (mode == TRIG_EDGE_RISE_FALL)
	{
		EXTI->FTSR1 |= ((1 << 10) | (1 << 11));
		EXTI->RTSR1 |= ((1 << 10) | (1 << 11));
	}
}

// 读取触发后采集的数据
void bsps_sa_trig_read(void)
{
	run_msg_t *run_msg = bsps_get_run_msg();

	if (dma_trig_ndtr != 0 && dma_trig_ndtr < ((62 * 1024) - 700))
	{
		// 触发成功
		if ((run_msg->trig_mode == TRIG_SIGLE && run_msg->is_trig == NO_TRIG) || run_msg->trig_mode == TRIG_AUTO || run_msg->trig_mode == TRIG_NORMAL)
		{
			osc_copy_from_fifo(cache_fifo[0], 0, dma_trig_ndtr - 350 + 2 - osc_tim_table[run_msg->timebase].osc_trig_delay, 700);
			osc_copy_from_fifo(cache_fifo[1], 1, dma_trig_ndtr - 350 + 2 - osc_tim_table[run_msg->timebase].osc_trig_delay, 700);
			dma_trig_ndtr = 0;
			run_msg->is_trig = TRIG;
			cnt_success++;
		}
	}
	else
	{
		// 触发失败
		if (run_msg->trig_mode != TRIG_NORMAL && run_msg->trig_mode != TRIG_SIGLE)
		{
			osc_copy_from_fifo(cache_fifo[0], 0, 700, 700);
			osc_copy_from_fifo(cache_fifo[1], 1, 700, 700);
			cnt_fail++;
		}

		if(run_msg->trig_mode == TRIG_SIGLE)
		{
			memset(cache_fifo[0],0,700);
			memset(cache_fifo[1],0,700);
		}
		run_msg->is_trig = NO_TRIG;
	}

	// 转换显示数据
	for (uint16_t i = 0; i < 700; i++)
	{
		disp_data[0][i] = ((float)cache_fifo[0][i] / 256.0f * 1.24f - 0.62) * 400 + 240 + run_msg->offset_lev[1];
		disp_data[1][i] = ((float)cache_fifo[1][i] / 256.0f * 1.24f - 0.62) * 400 + 240 + run_msg->offset_lev[0];
		if (disp_data[1][i] >= 440)
		{
			disp_data[1][i] = 440;
		}
		else if (disp_data[1][i] <= 40)
		{
			disp_data[1][i] = 40;
		}
		if (disp_data[0][i] >= 440)
		{
			disp_data[0][i] = 440;
		}
		else if (disp_data[0][i] <= 40)
		{
			disp_data[0][i] = 40;
		}
	}
}

void bsps_sa_exti_callbcak_trig(void)
{
	// ts2 = hal_sys_time_us();
	// ts1 = ts2 - ts0;
	// ts0 = ts2;
	if (os_trig_start_flag)
	{
		os_trig_start_flag = 0;
		bsps_sa_trig_exti_set(0);
		dma_trig_ndtr = 62 * 1024 - DMA2_Stream0->NDTR;
	}
}

void DMA2_Stream0_IRQHandler(void)
{
	if ((DMA2->LISR & DMA_FLAG_TCIF0_4) != RESET)
	{
		DMA2->LIFCR = DMA_FLAG_TCIF0_4;
		if ((DMA2_Stream0->CR & DMA_SxCR_CT) == RESET)
		{
			SCB_CleanInvalidateDCache();
			OSC_DMA_SET_TC();
		}
		else
		{
		}
	}
	if ((DMA2->LISR & DMA_FLAG_HTIF0_4) != RESET)
	{
		DMA2->LIFCR = DMA_FLAG_HTIF0_4;
		if ((DMA2_Stream0->CR & DMA_SxCR_CT) == RESET)
		{
			// SCB_CleanInvalidateDCache();
			bsps_sa_trig_exti_set(1);
			os_trig_start_flag = 1; // start trig
		}
		else
		{
		}
	}
}
