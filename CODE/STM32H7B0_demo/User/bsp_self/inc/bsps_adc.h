/*
 * @Author: SUN  BI4NEG@gmail.com
 * @Date: 2023-09-10 13:40:48
 * @LastEditTime: 2023-12-31 14:54:02
 * @Description: 请填写简介
 */
#ifndef __BSPS_ADC_H
#define __BSPS_ADC_H

#if 0

#define g_sample_counts 2048		//采样个数
#define tim6_psc 28-1			//分频值
#define tim6_arr 10-1			//计数值
#define test 0					//DMA单次触发

extern uint16_t ADCxValues[g_sample_counts];
extern float actu_data[g_sample_counts];
extern __IO int g_vol_bias;
extern __IO uint16_t g_trig_bias;
extern __IO uint16_t g_dc_bias;
extern __IO float g_vol_gain;
extern __IO uint16_t g_flag_trig_en;
extern __IO uint8_t g_is_pause;
extern __IO uint16_t g_flag_is_trig;

void bsps_sample_test(void);
static int bsps_adc_init(void);
static int bsps_adc_sample_start(void);
static void bsps_refresh(void);
static void bsps_refresh_src(void);

#endif

#endif


