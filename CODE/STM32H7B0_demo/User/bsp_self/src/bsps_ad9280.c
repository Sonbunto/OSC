#include "bsp.h"

DMA_HandleTypeDef hdma_tim1_up;
TIM_HandleTypeDef htim;

__IO uint8_t dma_ad9280_flag = 0;

ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) uint8_t ADCValues[4096]);

void bsps_ad9280_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	//PD0-7 -> D0-D7
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	//PB12 -> OTR
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
//	//PB13 -> CLk
//	GPIO_InitStruct.Pin =GPIO_PIN_13;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPsULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	
	GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM12;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
}



void bsps_ad9280_init(void)
{
	
	bsps_ad9280_gpio_init();

	DMA_HandleTypeDef DMA_Handle = {0};
	HAL_DMA_MuxRequestGeneratorConfigTypeDef dmamux_ReqGenParams = {0};
	
	__HAL_RCC_DMA1_CLK_ENABLE();
	DMA_Handle.Instance          = DMA1_Stream0;            /* ʹ�õ�DMA1 Stream0 */
	DMA_Handle.Init.Request      = DMA_REQUEST_GENERATOR0;  /* �������Ͳ��õ�DMAMUX��������ͨ��0 */  
	DMA_Handle.Init.Direction           = DMA_PERIPH_TO_MEMORY;/* ���䷽���ǴӴ洢�������� */  
	DMA_Handle.Init.PeriphInc           = DMA_PINC_DISABLE;    /* �����ַ������ֹ */ 
	DMA_Handle.Init.MemInc              = DMA_MINC_ENABLE;     /* �洢����ַ����ʹ�� */  
	DMA_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; /* �������ݴ���λ��ѡ���֣���32bit */     
	DMA_Handle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;  /* �洢�����ݴ���λ��ѡ���֣���32bit */    
	DMA_Handle.Init.Mode                = DMA_NORMAL;        /* ѭ��ģʽ */   
	DMA_Handle.Init.Priority            = DMA_PRIORITY_LOW;    /* ���ȼ��� */  
	DMA_Handle.Init.FIFOMode     = DMA_FIFOMODE_DISABLE;     /* ��ֹFIFO*/
	DMA_Handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL; /* ��ֹFIFO��λ�������ã��������÷�ֵ */
	DMA_Handle.Init.MemBurst      = DMA_MBURST_SINGLE; /* ��ֹFIFO��λ�������ã����ڴ洢��ͻ�� */
	DMA_Handle.Init.PeriphBurst   = DMA_PBURST_SINGLE; /* ��ֹFIFO��λ�������ã���������ͻ�� */
	
	/* ��ʼ��DMA */
	if(HAL_DMA_Init(&DMA_Handle) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);     
	}
	
	/* ����DMA1 Stream1���ж� */
	HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn); 
	
	
	/*##-4- ����DMAMUX ###########################################################*/
	dmamux_ReqGenParams.SignalID  = HAL_DMAMUX1_REQ_GEN_TIM12_TRGO;  /* ���󴥷���ѡ��LPTIM2_OUT */
	dmamux_ReqGenParams.Polarity  = HAL_DMAMUX_REQ_GEN_RISING;       /* �����ش���  */
	dmamux_ReqGenParams.RequestNumber = 1;                          /* �����󣬴������1��DMA���� */
	
	HAL_DMAEx_ConfigMuxRequestGenerator(&DMA_Handle, &dmamux_ReqGenParams);/* ����DMAMUX */
	HAL_DMAEx_EnableMuxRequestGenerator (&DMA_Handle);                      /* ʹ��DMAMUX�������� */   
	
	
	/*##-4- ����DMA˫���崫�� ################################################*/
	/*
		1���˺����Ὺ��DMA��TC��TE��DME�ж�
		2������û������˻ص�����DMA_Handle.XferHalfCpltCallback����ô����HAL_DMA_Init�Ὺ���봫����
		���жϡ�
		3������û�ʹ����DMAMUX��ͬ��ģʽ���˺����Ὺ��ͬ������жϡ�
		4������û�ʹ����DMAMUX�������������˺����Ὺʼ������������жϡ�
	*/
	HAL_DMAEx_MultiBufferStart_IT(&DMA_Handle,(uint32_t)&GPIOD->IDR,(uint32_t)ADCValues,(uint32_t)&GPIOD->IDR,4096);
	
	/* �ò������жϿ���ֱ�ӹر� */
//	DMA1_Stream1->CR &= ~DMA_IT_DME; 
//	DMA1_Stream1->CR &= ~DMA_IT_TE;
//	DMAMUX1_RequestGenerator0->RGCR &= ~DMAMUX_RGxCR_OIE;
	
	
	
	//////////////////////////////////////////
	
	
	
	
	
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
	
	__HAL_RCC_TIM12_CLK_ENABLE();
	
	HAL_TIM_Base_DeInit(&htim);
	
	htim.Instance = TIM12;
	htim.Init.Period = 10-1;
	htim.Init.Prescaler = 24-1;
	htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&htim);
	
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim, &sClockSourceConfig) != HAL_OK)
	{
		
	}
	
	if (HAL_TIM_PWM_Init(&htim) != HAL_OK)
	{
		
	}
	
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 4;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		
	}
	
	
	/* ����OC1 */
	if(HAL_TIM_PWM_Start(&htim, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}
	
	/* TIM12��TRGO���ڴ���DMAMUX���������� */
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1REF;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	
	HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig);
	
	
}

void bsps_ad9280_sample_test(void)
{
	uint16_t i = 0;
	uint16_t sum = 0;
	if(dma_ad9280_flag)
	{
		DISABLE_INT();
		for(i=0;i<4096;i++)
		{
			printf("%.3f\r\n",ADCValues[i] / 255.0 * 2.0);
		}
		ENABLE_INT();
		dma_ad9280_flag = 0;
	}   
}


void DMA1_Stream0_IRQHandler(void)
{
	/* ��������ж� */
    if((DMA1->LISR & DMA_FLAG_TCIF0_4) != RESET)
    {
        /* �����־ */
		HAL_TIM_PWM_Stop_IT(&htim,TIM_CHANNEL_1);
        DMA1->LIFCR = DMA_FLAG_TCIF0_4;
		dma_ad9280_flag = 1;
//		SCB_CleanInvalidateDCache();
    }

    /* �봫������ж� */    
    if((DMA1->LISR & DMA_FLAG_HTIF0_4) != RESET)
    {
        /* �����־ */
        DMA1->LISR = DMA_FLAG_HTIF0_4;
    }

    /* ��������ж� */
    if((DMA1->LISR & DMA_FLAG_TEIF0_4) != RESET)
    {
        /* �����־ */
        DMA1->LISR = DMA_FLAG_TEIF0_4;
    }

    /* ֱ��ģʽ�����ж� */
    if((DMA1->LISR & DMA_FLAG_DMEIF0_4) != RESET)
    {
        /* �����־ */
        DMA1->LISR = DMA_FLAG_DMEIF0_4;
    }
}


