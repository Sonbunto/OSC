#include "bsp.h"

uint8_t enc_flag, a_lev, b_lev = 0;

OS_INIT_REGISTER("enc_init", bsps_enc_gpio_init, 0, 5);

int bsps_enc_gpio_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_Handle;

    // EC1A
    GPIO_Handle.Pin = GPIO_PIN_0;
    GPIO_Handle.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_Handle.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_Handle);
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    // EC1B
    GPIO_Handle.Pin = GPIO_PIN_1;
    GPIO_Handle.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOA, &GPIO_Handle);

    // EC2A
    GPIO_Handle.Pin = GPIO_PIN_3;
    GPIO_Handle.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_Handle.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_Handle);
    HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    // EC2B
    GPIO_Handle.Mode = GPIO_MODE_INPUT;
    GPIO_Handle.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOB, &GPIO_Handle);

    // EC3A
    GPIO_Handle.Pin = GPIO_PIN_6;
    GPIO_Handle.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_Handle.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE, &GPIO_Handle);
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    // EC3B
    GPIO_Handle.Mode = GPIO_MODE_INPUT;
    GPIO_Handle.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOH, &GPIO_Handle);

    // // EC4A
    // GPIO_Handle.Pin = GPIO_PIN_11;
    // GPIO_Handle.Mode = GPIO_MODE_IT_RISING_FALLING;
    // GPIO_Handle.Pull = GPIO_PULLUP;
    // HAL_GPIO_Init(GPIOB, &GPIO_Handle);
    // HAL_NVIC_SetPriority(EXTI3_IRQn, 4, 0);
    // HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    // // EC4B
    // GPIO_Handle.Mode = GPIO_MODE_INPUT;
    // GPIO_Handle.Pin = GPIO_PIN_3;
    // HAL_GPIO_Init(GPIOC, &GPIO_Handle);

    // // EC1_KEY
    // GPIO_Handle.Pin = GPIO_PIN_4;
    // GPIO_Handle.Mode = GPIO_MODE_IT_FALLING;
    // GPIO_Handle.Pull = GPIO_PULLUP;
    // HAL_GPIO_Init(GPIOE, &GPIO_Handle);
    // //EC2_KEY
    // GPIO_Handle.Pin = GPIO_PIN_5;
    // HAL_GPIO_Init(GPIOE, &GPIO_Handle);
    // //EC3_KEY
    // GPIO_Handle.Pin = GPIO_PIN_2;
    // HAL_GPIO_Init(GPIOA, &GPIO_Handle);

    return OS_OK;
}

void bsps_enc_callback(GPIO_TypeDef *GPIOxA, uint16_t GPIO_PinA, GPIO_TypeDef *GPIOxB, uint16_t GPIO_PinB, void (*func_forward)(void), void (*func_reverse)(void))
{
    if (!HAL_GPIO_ReadPin(GPIOxA, GPIO_PinA) && !enc_flag)
    {
        enc_flag++;
        b_lev = 0;
        if (HAL_GPIO_ReadPin(GPIOxB, GPIO_PinB))
        {
            b_lev = 1;
        }
    }

    if (HAL_GPIO_ReadPin(GPIOxA, GPIO_PinA) && enc_flag)
    {
        enc_flag = 0;
        if (b_lev && !HAL_GPIO_ReadPin(GPIOxB, GPIO_PinB))
        {
            func_reverse();
        }
        if (!b_lev && HAL_GPIO_ReadPin(GPIOxB, GPIO_PinB))
        {
            func_forward();
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    run_msg_t *run_msg = bsps_get_run_msg();

    switch (GPIO_Pin)
    {
    case GPIO_PIN_0:
        bsps_enc_callback(GPIOA, GPIO_PIN_0, GPIOA, GPIO_PIN_1, bsps_time_inc, bsps_time_dec);
        break;
    case GPIO_PIN_3:
        bsps_enc_callback(GPIOB, GPIO_PIN_3, GPIOB, GPIO_PIN_4, bsps_vol_scale_dec_ch2, bsps_vol_scale_inc_ch2);
        break;
    case GPIO_PIN_6:
        bsps_enc_callback(GPIOE, GPIO_PIN_6, GPIOH, GPIO_PIN_1, bsps_vol_scale_inc_ch1, bsps_vol_scale_dec_ch1);
        break;
    case GPIO_PIN_10:
        if(run_msg->trig_src == TRIG_CH2)
        {
            bsps_sa_exti_callbcak_trig();
        }
        break;
    case GPIO_PIN_11:
        if(run_msg->trig_src == TRIG_CH1)
        {
            bsps_sa_exti_callbcak_trig();
        }
        break;
    default:
        break;
    }
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
}
