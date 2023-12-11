#include "bsp.h"
#include "dsp/window_functions.h"

/*
	实现功能：基本的ADC+DMA+TIM的采样功能
	因电路设计缺陷，PC0对应的ADC1_IN10最高仅支持1Mps采样率，将在下一版中改进
*/

// 采样数据 注意要放在IRAM2段，如放在IRAM1段则DMA无法访问
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) uint16_t ADCxValues[g_sample_counts]);
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) uint16_t x[700]);
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) uint16_t y[700]);
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) uint16_t y_old[700]);
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) float actu_data[g_sample_counts]);
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) float tmp_win[g_sample_counts]);

__IO uint8_t dma_flag = 0;				// DMA搬运完成标志位
__IO uint8_t g_is_pause = 0;			// 是否暂停标志位
__IO uint8_t restart_flag = 0;			// 是否第一次采集标志位
__IO int g_vol_bias = 0;				// 波形偏移电压
__IO uint16_t g_trig_bias = 240;		// 触发电压
__IO uint16_t g_dc_bias = 1;			// AC/DC耦合切换
__IO float g_vol_gain = 1.0;			// 电压增益控制
uint16_t cnt_trig_en, cnt_trig_dis = 0; // 搬运波形计数
__IO uint16_t g_flag_trig_en = 0;		// 找到触发电压 搬运波形使能
__IO uint16_t g_flag_is_trig = 0;		// 是否触发
__IO uint16_t g_wave_is_refresh = 0;	// 波形是否更新标志位
uint16_t tmp, tmp1 = 0;					// 暂时比较变量
uint16_t cnt = 0;

ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim6;
DMA_HandleTypeDef hdma_adc1;

// OS_INIT_REGISTER("adc_init",bsps_adc_init,bsps_adc_sample_start,1);

// OS_TSK_REGISTER(bsps_refresh_src,PRIORITY_2,50);

void bsps_adc1_init(void)
{
	ADC_MultiModeTypeDef multimode = {0};
	ADC_ChannelConfTypeDef sConfig = {0};

	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc1.Init.Resolution = ADC_RESOLUTION_16B;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T6_TRGO;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_FALLING;
#if test == 1
	hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_ONESHOT;
#else
	hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
#endif
	hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
	hadc1.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}

	//	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED) != HAL_OK)
	//	{
	//		Error_Handler(__FILE__, __LINE__);
	//	}
	multimode.Mode = ADC_MODE_INDEPENDENT;
	if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	sConfig.OffsetSignedSaturation = DISABLE;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	}

	for (uint16_t i = 20; i < 720; i++)
	{
		x[i - 20] = i;
		y_old[i - 20] = 10;
	}
}

// DMA以及IO初始化，此函数会被HAL_ADC_Init()调用
void HAL_ADC_MspInit(ADC_HandleTypeDef *adcHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	if (adcHandle->Instance == ADC1)
	{
		/* USER CODE BEGIN ADC1_MspInit 0 */

		/* USER CODE END ADC1_MspInit 0 */

		/** Initializes the peripherals clock
		 */
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
		PeriphClkInitStruct.PLL2.PLL2M = 6;
		PeriphClkInitStruct.PLL2.PLL2N = 36;
		PeriphClkInitStruct.PLL2.PLL2P = 5;
		PeriphClkInitStruct.PLL2.PLL2Q = 2;
		PeriphClkInitStruct.PLL2.PLL2R = 2;
		PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
		PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
		PeriphClkInitStruct.PLL2.PLL2FRACN = 0.0;
		PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		{
		}

		/* ADC1 clock enable */
		__HAL_RCC_ADC12_CLK_ENABLE();
		__HAL_RCC_DMA1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**ADC1 GPIO Configuration
		PA6     ------> ADC1_INP3
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* ADC1 DMA Init */
		/* ADC1 Init */
		hdma_adc1.Instance = DMA1_Stream2;
		hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
		hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_adc1.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD;
#if test == 1
		hdma_adc1.Init.Mode = DMA_NORMAL;
#else
		hdma_adc1.Init.Mode = DMA_CIRCULAR;
#endif
		hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
		hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
		{
		}

		HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);

		__HAL_LINKDMA(adcHandle, DMA_Handle, hdma_adc1);

		/* USER CODE END ADC1_MspInit 1 */
	}
}

void bsps_tim6_init(void)
{
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	__HAL_RCC_TIM6_CLK_ENABLE();

	htim6.Instance = TIM6;
	htim6.Init.Prescaler = tim6_psc;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = tim6_arr;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}
}

void bsps_wave_task(void)
{
	if (!g_wave_is_refresh)
	{
		// AD数据预处理
		for (uint16_t i = 200; i < g_sample_counts; i++)
		{

			tmp = (actu_data[i] - g_dc_bias) * 120 * g_vol_gain + 240;
			if ((tmp >= g_trig_bias && tmp <= g_trig_bias + 3 && tmp < tmp1) || g_flag_trig_en)
			{
				y[cnt_trig_en] = tmp + g_vol_bias + 2000;
				if (y[cnt_trig_en] >= 2440)
				{
					y[cnt_trig_en] = 440;
				}
				else if (y[cnt_trig_en] <= 2040)
				{
					y[cnt_trig_en] = 40;
				}
				else
				{
					y[cnt_trig_en] -= 2000;
				}
				cnt_trig_en++;
				g_flag_trig_en = 1;
				if (cnt_trig_en == 700)
				{
					break;
				}
				g_flag_is_trig = 1;
			}
			else
			{
				y[cnt_trig_dis] = tmp + g_vol_bias + 2000;
				if (y[cnt_trig_dis] >= 2440)
				{
					y[cnt_trig_dis] = 440;
				}
				else if (y[cnt_trig_dis] <= 2040)
				{
					y[cnt_trig_dis] = 40;
				}
				else
				{
					y[cnt_trig_dis] -= 2000;
				}
				cnt_trig_dis++;
				if (cnt_trig_dis == 700)
				{
					break;
				}
				g_flag_is_trig = 0;
			}
			tmp1 = tmp;
			//			printf("%f\r\n",(3.3 * ADCxValues[i] / 65535.0));
		}
		g_wave_is_refresh = 1;
	}

	// 标志位初始化
	g_flag_trig_en = 0;
	cnt_trig_en = 0;
	cnt_trig_dis = 0;
	tmp = tmp1 = 0;
}

static void bsps_refresh_src(void)
{
	if (osc_mode || !g_wave_is_refresh)
	{
		return;
	}
	bsps_ui_main_win_draw();
	bsp_lcd_draw_lines(x, y_old, 700, CL_BLACK);
	bsp_lcd_draw_lines(x, y, 700, CL_YELLOW);
	for (uint16_t i = 0; i < 700; i++)
	{
		y_old[i] = y[i];
	}
	g_wave_is_refresh = 0;
}

void bsps_spec_task(void)
{

	arm_hamming_f32(tmp_win, g_sample_counts);

	for (uint16_t i = 0; i < g_sample_counts; i++)
	{
		actu_data[i] *= tmp_win[i];
	}

	// FFT
	bsps_fft_cal(actu_data);

	cnt++;
	if (cnt >= 30)
	{
		bsps_ui_base_fre_draw();
		cnt = 0;
	}

	// 频谱数据处理
	for (uint16_t i = 0; i < 700; i++)
	{
		y[i] = (uint16_t)testOutputMag_f32[i] / 6.0 * 1.852 + 40;
		//		printf("%d\r\n",y[i]);
	}

	// 重新绘制波形及边框
	bsp_lcd_draw_lines(x, y_old, 700, CL_BLACK);
	bsps_ui_main_win_draw();
	bsp_lcd_draw_lines(x, y, 700, CL_YELLOW);

	// 更新旧数组
	for (uint16_t i = 0; i < 700; i++)
	{
		y_old[i] = y[i];
	}
}

void bsps_sample_test(void)
{

	if (dma_flag)
	{
		EventStartA(2);

		// 暂停采样
		HAL_TIM_Base_Stop(&htim6);

		// AD数据预处理
		for (uint16_t i = 0; i < g_sample_counts; i++)
		{
			tmp_win[i] = actu_data[i] = 3.3 * ADCxValues[i] / 65535.0;
		}

		// 模式选择
		if (!g_is_pause)
		{
			if (osc_mode)
			{
				bsps_spec_task();
			}
			else
			{
				bsps_wave_task();
			}
		}
		// 适应屏幕刷新率
		HAL_Delay(7);

		dma_flag = 0;

		// 开启下一轮采样
		bsps_adc_sample_start();

		EventStopA(2);
	}
}

static int bsps_adc_init(void)
{
	bsps_adc1_init();
	bsps_tim6_init();

	return OS_OK;
}

static int bsps_adc_sample_start(void)
{
	if (!restart_flag)
	{
		if (HAL_TIM_Base_Start(&htim6) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}
		if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADCxValues, g_sample_counts) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}
		restart_flag = 1;
	}
	else
	{
		if (HAL_TIM_Base_Start(&htim6) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}
	}

	return OS_OK;
}

void DMA1_Stream2_IRQHandler(void)
{
	/* 传输完成中断 */
	if ((DMA1->LISR & DMA_FLAG_TCIF2_6) != RESET)
	{
		/*
		   1、使用此函数要特别注意，第1个参数地址要32字节对齐，第2个参数要是32字节的整数倍。
		   2、进入传输完成中断，当前DMA正在使用缓冲区的前半部分，用户可以操作后半部分。
		*/
		SCB_CleanInvalidateDCache();

		dma_flag = 1;

		DMA1->LIFCR = DMA_FLAG_TCIF2_6;
	}

	/* 半传输完成中断 */
	if ((DMA1->LISR & DMA_FLAG_HTIF2_6) != RESET)
	{
		DMA1->LIFCR = DMA_FLAG_HTIF2_6;
	}
	/* 传输错误中断 */
	if ((DMA1->LISR & DMA_FLAG_TEIF2_6) != RESET)
	{
		DMA1->LIFCR = DMA_FLAG_TEIF2_6;
	}

	/* 直接模式错误中断 */
	if ((DMA1->LISR & DMA_FLAG_DMEIF2_6) != RESET)
	{
		DMA1->LIFCR = DMA_FLAG_DMEIF2_6;
	}
}
