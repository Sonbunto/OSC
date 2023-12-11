#include "bsp.h"

#define TEST_LENGTH_SAMPLES 2048    /* 采样点数 */

static uint32_t ifftFlag = 0; 
static uint32_t fftSize = 0;

ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) static float32_t testOutput_f32[TEST_LENGTH_SAMPLES*2]); 
ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) float32_t testOutputMag_f32[TEST_LENGTH_SAMPLES*2]); 

//static float32_t Phase_f32[TEST_LENGTH_SAMPLES*2]; /* 相位*/ 

void PowerPhaseRadians_f32(float32_t *_ptr, float32_t *_phase, uint16_t _usFFTPoints, float32_t _uiCmpValue);
void PowerPhaseRadians_f64(float64_t *_ptr, float64_t *_phase, uint16_t _usFFTPoints, float64_t _uiCmpValue);

void bsps_fft_cal(float *testInputMag_f32)
{
	uint32_t i = 0;
	uint32_t max_index = 0;
	float max_data = 0;
	
	arm_rfft_fast_instance_f32 S_FFT;
	
	/* 正变换 */
    ifftFlag = 0; 
	
	/* 初始化结构体S中的参数 */
 	arm_rfft_fast_init_f32(&S_FFT, TEST_LENGTH_SAMPLES);
	
	/* 1024点实序列快速FFT */ 
	arm_rfft_fast_f32(&S_FFT, testInputMag_f32, testOutput_f32, ifftFlag);
	
	/* 为了方便跟函数arm_cfft_f32计算的结果做对比，这里求解了1024组模值，实际函数arm_rfft_fast_f32
	   只求解出了512组  
	*/ 
	arm_cmplx_mag_f32(testOutput_f32, testOutputMag_f32, TEST_LENGTH_SAMPLES);
	
//	/* 求相频 */
//	PowerPhaseRadians_f32(testOutput_f32, Phase_f32, TEST_LENGTH_SAMPLES, 5.0f);
	
//	for(i = 0; i < TEST_LENGTH_SAMPLES / 2 ; i++)
//	{
//		testOutputMag_f32[i] = testOutputMag_f32[i];
////		printf("%.3f\r\n",Phase_f32[i]);
////		testOutputMag_f32[i] = 20.0 * log10(testOutputMag_f32[i]);
////		printf("%f\r\n", testOutputMag_f32[i]);
//	}
}

/*
*********************************************************************************************************
*	函 数 名: PowerPhaseRadians_f32
*	功能说明: 求相位
*	形    参：_ptr  复位地址，含实部和虚部
*             _phase 求出相位，单位角度制，范围(-180, 180]
*             _usFFTPoints  复数个数，每个复数是两个float32_t数值
*             _uiCmpValue  比较值，需要求出相位的数值
*	返 回 值: 无
*********************************************************************************************************
*/
void PowerPhaseRadians_f32(float32_t *_ptr, float32_t *_phase, uint16_t _usFFTPoints, float32_t _uiCmpValue)		
{
	float32_t lX, lY;
	uint16_t i;
	float32_t phase;
	float32_t mag;
	
	
	for (i=0; i <_usFFTPoints; i++)
	{
		lX= _ptr[2*i];  	  /* 实部 */
		lY= _ptr[2*i + 1];    /* 虚部 */ 
		
 		phase = atan2f(lY, lX);    		  				 /* atan2求解的结果范围是(-pi, pi], 弧度制 */
		arm_sqrt_f32((float32_t)(lX*lX+ lY*lY), &mag);   /* 求模 */
		
		if(_uiCmpValue > mag)
		{
			_phase[i] = 0;			
		}
		else
		{
			_phase[i] = phase* 180.0f/3.1415926f;   /* 将求解的结果由弧度转换为角度 */
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: PowerPhaseRadians_f64
*	功能说明: 求相位
*	形    参：_ptr  复位地址，含实部和虚部
*             _phase 求出相位，单位角度制，范围(-180, 180]
*             _usFFTPoints  复数个数，每个复数是两个float64_t数值
*             _uiCmpValue  比较值，需要求出相位的数值
*	返 回 值: 无
*********************************************************************************************************
*/
void PowerPhaseRadians_f64(float64_t *_ptr, float64_t *_phase, uint16_t _usFFTPoints, float64_t _uiCmpValue)		
{
	float64_t lX, lY;
	uint16_t i;
	float64_t phase;
	float64_t mag;
	
	
	for (i=0; i <_usFFTPoints; i++)
	{
		lX= _ptr[2*i];  	  /* 实部 */
		lY= _ptr[2*i + 1];    /* 虚部 */ 
		
 		phase = atan2(lY, lX);      /* atan2求解的结果范围是(-pi, pi], 弧度制 */
		mag = sqrt(lX*lX+ lY*lY);   /* 求模 */
		
		if(_uiCmpValue > mag)
		{
			_phase[i] = 0;			
		}
		else
		{
			_phase[i] = phase* 180.0/3.1415926;  /* 将求解的结果由弧度转换为角度 */
		}
	}
}



