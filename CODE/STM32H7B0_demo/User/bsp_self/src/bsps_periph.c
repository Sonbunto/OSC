#include "bsp.h"


static TIM_HandleTypeDef TIM_Handle;
TIM_HandleTypeDef htim3;
DAC_HandleTypeDef hdac2;


uint8_t cnt_time = 3;
uint8_t cnt_vol_scale = 5;

int bsps_sa_periph_init(void);

OS_INIT_REGISTER("periph_init", bsps_sa_periph_init, 0, 1);

const osc_time_t osc_tim_table[] = 
{
	{
		.str = "1.25us",
		.osc_time = 1.25f , /*  base on 750ns */
		.osc_unit = OSC_UNIT_US,
		.osc_trig_delay = 35,
	},
	{
		.str = "2.5us ",
		.osc_time = 2.5f ,
		.osc_unit = OSC_UNIT_US,
		.osc_trig_delay = 15,
	},
	{
		.str = "5us   ",
		.osc_time = 5 ,
		.osc_unit = OSC_UNIT_US,
		.osc_trig_delay = 7,
	},	
	{
		.str = "10us  ",
		.osc_time = 10 ,
		.osc_unit = OSC_UNIT_US,
		.osc_trig_delay = 3,
	},
	{
		.str = "25us  ",
		.osc_time = 25 ,
		.osc_unit = OSC_UNIT_US,
		.osc_trig_delay = 1,
	},
	{
		.str = "50us  ",
		.osc_time = 50 ,
		.osc_unit = OSC_UNIT_US,
		.osc_trig_delay = 0,
	},
	{
		.str = "100us ",
		.osc_time = 100 ,
		.osc_unit = OSC_UNIT_US,
		.osc_trig_delay = 0,
	},	
	{
		.str = "250us ",
		.osc_time = 250 ,
		.osc_unit = OSC_UNIT_US,
		.osc_trig_delay = 0,
	},
	{
		.str = "500us ",
		.osc_time = 500 ,
		.osc_unit = OSC_UNIT_US,
		.osc_trig_delay = 0,
	}
};

osc_vol_scale_t osc_vol_scale_table[] = 
{
	/* 1 */
	{
		.str = "10m",
		.strX10 = "100m",
		.mv_int = 10,
		.rly_att = 1,
		.att_sel[0] = 4,
		.att_sel[1] = 0,
	},
	/* 2 */
	{
		.str = "20m",
		.strX10 = "200m",
		.mv_int = 20,
		.rly_att = 1,
		.att_sel[0] = 6,
		.att_sel[1] = 1,
	},
	/* 4 */
	{
		.str = "50m",
		.strX10 = "500m",
		.mv_int = 50,
		.rly_att = 1,
		.att_sel[0] = 1,
		.att_sel[1] = 3,
	},
	/* 5 */
	{
		.str = "100m",
		.strX10 = "1V",
		.mv_int = 100,
		.rly_att = 1,
		.att_sel[0] = 3,
		.att_sel[1] = 4,
	},
	/* 6 */
	{
		.str = "200m",
		.strX10 = "2V",
		.mv_int = 200,
		.rly_att = 1,
		.att_sel[0] = 0,
		.att_sel[1] = 5,
	},
/*---------------------------------------------------------------*/
	/* 1 */
	{
		.str = "500m",
		.strX10 = "5V",
		.mv_int = 500,
		.rly_att = 0,
		.att_sel[0] = 4,
		.att_sel[1] = 0,
	},
	/* 2 */
	{
		.str = "1V",
		.strX10 = "10V",
		.mv_int = 1000,
		.rly_att = 0,
		.att_sel[0] = 6,
		.att_sel[1] = 1,
	},
	/* 3 */
	{
		.str = "2V",
		.strX10 = "20V",
		.mv_int = 2000,
		.rly_att = 0,
		.att_sel[0] = 2,
		.att_sel[1] = 2,
	},
	/* 4 */
	{
		.str = "5V",
		.strX10 = "50V",
		.mv_int = 5000,
		.rly_att = 0,
		.att_sel[0] = 3,
		.att_sel[1] = 4,
	},
	/* 5 */
	{
		.str = "10V",
		.strX10 = "100V",
		.mv_int = 200,
		.rly_att = 0,
		.att_sel[0] = 0,
		.att_sel[1] = 5,
	},	
};

// 模拟前端及后续外设初始化
int bsps_sa_periph_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitTypeDef GPIO_Handle;
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
	DAC_ChannelConfTypeDef sConfig = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_DAC2_CLK_ENABLE();

	// IO_RLY1 2
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	bsps_sa_rly_att_ch1();
	bsps_sa_rly_att_ch2();

	// HC_ABC1 2
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	bsps_sa_att_sel_ch1(0);
	bsps_sa_att_sel_ch2(0);

	// HC_S01
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	bsps_sa_sw_only_ch2();

	// IO_OH1 2
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	bsps_sa_oh_ac_ch1();
	bsps_sa_oh_ac_ch2();

	// DAC_TRIG
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	hdac2.Instance = DAC2;
	if (HAL_DAC_Init(&hdac2) != HAL_OK)
	{
	}
	sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_DISABLE;
	sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;
	if (HAL_DAC_ConfigChannel(&hdac2, &sConfig, DAC_CHANNEL_1) != HAL_OK)
	{
	}

	HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 1024);
	HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);

	bsps_sa_set_trig_vol(1.25);

	// IO_S12
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	bsps_sa_adc_mode_ch1_ch2_align();

	// PWM_DAC1
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 7 - 1;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 1000 - 1;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
	{
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	{
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 500;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
	{
	}
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
	{
	}
	bsps_sa_set_pwm_pulse_ch1(530);
	bsps_sa_set_pwm_pulse_ch2(533);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

	// exti
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);

    return OS_OK;
}

// CH1继电器跳到衰减回路
void bsps_sa_rly_att_ch1(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 0);
}

// CH1继电器跳到非衰减回路
void bsps_sa_rly_no_att_ch1(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);
}

// CH1继电器选择
void bsps_sa_rly_att_sel_ch1(uint8_t mode)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, mode);
}

// CH2继电器跳到衰减回路
void bsps_sa_rly_att_ch2(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, 0);
}

// CH2继电器跳到非衰减回路
void bsps_sa_rly_no_att_ch2(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, 1);
}

// CH2继电器选择
void bsps_sa_rly_att_sel_ch2(uint8_t mode)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, mode);
}

// CH1切换到交流耦合
void bsps_sa_oh_ac_ch1(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);
}

// CH1切换到直流耦合
void bsps_sa_oh_dc_ch1(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);
}

// CH2切换到交流耦合
void bsps_sa_oh_ac_ch2(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}

// CH2切换到直流耦合
void bsps_sa_oh_dc_ch2(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

// Ch1衰减倍数回路选择
void bsps_sa_att_sel_ch1(uint8_t att)
{
	switch (att)
	{
	case 0:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
		break;
	case 5:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
		break;
	}
}

// CH2衰减倍数回路选择
void bsps_sa_att_sel_ch2(uint8_t att)
{
	switch (att)
	{
	case 0:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
		break;                               
	case 1:                                  
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, 0);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
		break;                               
	case 2:                                  
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, 0);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
		break;                               
	case 3:                                  
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
		break;                               
	case 4:                                  
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, 0);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
		break;                               
	case 5:                                  
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
		break;
	}
}

// 仅CH1通道进入ADC
void bsps_sa_sw_only_ch1(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, 1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, 1);
}

// 仅CH2通道进入ADC
void bsps_sa_sw_only_ch2(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, 0);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, 0);
}

// CH1进入CH1，CH2进入CH2
void bsps_sa_sw_ch1_ch2(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, 0);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, 1);
}

// CH2进入CH1，CH2进入CH1
void bsps_sa_sw_ch2_ch1(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, 1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, 0);
}

// 设置触发电压
void bsps_sa_set_trig_vol(float vol)
{
	HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint16_t)(vol / 2.5 * 4096));
}

// ADC模式 关闭
void bsps_sa_adc_mode_standby(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2 | GPIO_PIN_3, 0);
}

// ADC模式 仅CH1
void bsps_sa_adc_mode_only_ch1(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, 0);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, 1);
}

// ADC模式 双通道不对齐
void bsps_sa_adc_mode_ch1_ch2_no_align(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, 1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, 0);
}

// ADC模式 双通道对齐
void bsps_sa_adc_mode_ch1_ch2_align(void)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2 | GPIO_PIN_3, 1);
}

// 设置DAC_PWM占空比以控制电压
void bsps_sa_set_pwm_pulse_ch1(uint16_t pulse)
{
	TIM3->CCR2 = pulse;
}

// 设置DAC_PWM占空比以控制电压
void bsps_sa_set_pwm_pulse_ch2(uint16_t pulse)
{
	TIM3->CCR3 = pulse;
}

//设置时基及垂直增益参数
void bsps_time_psc_set(uint8_t index)
{
	run_msg_t *run_msg = bsps_get_run_msg();

	run_msg->timebase = index;

	if( index >= sizeof(osc_tim_table) / sizeof(osc_tim_table[0]) )
	{
		/* return */
		return;
	}

	uint16_t psc = 0;

    bsps_ui_timebase_draw(osc_tim_table[index].str);

	if(osc_tim_table[index].osc_unit == OSC_UNIT_US)
	{
		psc = (uint16_t)(5.6 * osc_tim_table[index].osc_time) / 2 - 1;
	}
	else if(osc_tim_table[index].osc_unit == OSC_UNIT_MS)
	{
		psc = (uint16_t)(5600.0 * osc_tim_table[index].osc_time);
	}
	else if(osc_tim_table[index].osc_time == OSC_UNIT_S)
	{
		psc = (uint16_t)(5600000.0 * osc_tim_table[index].osc_time);
	}
	else if(osc_tim_table[index].osc_time == OSC_UNIT_NS)
	{
		//...
	}
	else
	{
		return;
	}

	bsps_sa_set_psc(psc);
}

//时基循环加
void bsps_time_inc(void)
{
	if(cnt_time++ >= 8)
	{
		cnt_time = 8;
	}
	bsps_time_psc_set(cnt_time);
}

//时基循环减
void bsps_time_dec(void)
{
	if(cnt_time-- <= 0)
	{
		cnt_time = 0;
	}
	bsps_time_psc_set(cnt_time);
}

//垂直电压增益设置
void bsps_vol_scale_set_ch2(uint8_t index)
{
    run_msg_t *run_msg = bsps_get_run_msg();

    run_msg->vol_scale[1] = index;
    
    if(run_msg->ratio == RATIO_10X)
	{
		bsps_ui_ch12_vol_gain_draw(osc_vol_scale_table[index].strX10,0);
	}	
	else
	{
		bsps_ui_ch12_vol_gain_draw(osc_vol_scale_table[index].str,0);
	}
	bsps_sa_rly_att_sel_ch2(osc_vol_scale_table[index].rly_att);
	bsps_sa_att_sel_ch2(osc_vol_scale_table[index].att_sel[1]);
}

//垂直电压增益循环加
void bsps_vol_scale_inc_ch2(void)
{
	if(cnt_vol_scale++ >= 9)
	{
		cnt_vol_scale = 9;
	}
	bsps_vol_scale_set_ch2(cnt_vol_scale);
}

//垂直电压增益循环减
void bsps_vol_scale_dec_ch2(void)
{
	if(cnt_vol_scale-- <= 0)
	{
		cnt_vol_scale = 0;
	}
	bsps_vol_scale_set_ch2(cnt_vol_scale);
}

void bsps_trig_lev_set(uint16_t trig)
{
    bsps_sa_set_trig_vol((float)trig / 400.0f * 1.024f + 0.738f);
}

void bsps_ratio_set(uint8_t ratio)
{
    run_msg_t *run_msg = bsps_get_run_msg();

    run_msg->ratio = ratio;
}




