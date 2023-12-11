#include "bsp.h"

#define TEST_LENGTH_SAMPLES 2048    /* �������� */

static uint32_t ifftFlag = 0; 
static uint32_t fftSize = 0;

ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) static float32_t testOutput_f32[TEST_LENGTH_SAMPLES*2]); 
ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) float32_t testOutputMag_f32[TEST_LENGTH_SAMPLES*2]); 

//static float32_t Phase_f32[TEST_LENGTH_SAMPLES*2]; /* ��λ*/ 

void PowerPhaseRadians_f32(float32_t *_ptr, float32_t *_phase, uint16_t _usFFTPoints, float32_t _uiCmpValue);
void PowerPhaseRadians_f64(float64_t *_ptr, float64_t *_phase, uint16_t _usFFTPoints, float64_t _uiCmpValue);

void bsps_fft_cal(float *testInputMag_f32)
{
	uint32_t i = 0;
	uint32_t max_index = 0;
	float max_data = 0;
	
	arm_rfft_fast_instance_f32 S_FFT;
	
	/* ���任 */
    ifftFlag = 0; 
	
	/* ��ʼ���ṹ��S�еĲ��� */
 	arm_rfft_fast_init_f32(&S_FFT, TEST_LENGTH_SAMPLES);
	
	/* 1024��ʵ���п���FFT */ 
	arm_rfft_fast_f32(&S_FFT, testInputMag_f32, testOutput_f32, ifftFlag);
	
	/* Ϊ�˷��������arm_cfft_f32����Ľ�����Աȣ����������1024��ģֵ��ʵ�ʺ���arm_rfft_fast_f32
	   ֻ������512��  
	*/ 
	arm_cmplx_mag_f32(testOutput_f32, testOutputMag_f32, TEST_LENGTH_SAMPLES);
	
//	/* ����Ƶ */
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
*	�� �� ��: PowerPhaseRadians_f32
*	����˵��: ����λ
*	��    �Σ�_ptr  ��λ��ַ����ʵ�����鲿
*             _phase �����λ����λ�Ƕ��ƣ���Χ(-180, 180]
*             _usFFTPoints  ����������ÿ������������float32_t��ֵ
*             _uiCmpValue  �Ƚ�ֵ����Ҫ�����λ����ֵ
*	�� �� ֵ: ��
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
		lX= _ptr[2*i];  	  /* ʵ�� */
		lY= _ptr[2*i + 1];    /* �鲿 */ 
		
 		phase = atan2f(lY, lX);    		  				 /* atan2���Ľ����Χ��(-pi, pi], ������ */
		arm_sqrt_f32((float32_t)(lX*lX+ lY*lY), &mag);   /* ��ģ */
		
		if(_uiCmpValue > mag)
		{
			_phase[i] = 0;			
		}
		else
		{
			_phase[i] = phase* 180.0f/3.1415926f;   /* �����Ľ���ɻ���ת��Ϊ�Ƕ� */
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: PowerPhaseRadians_f64
*	����˵��: ����λ
*	��    �Σ�_ptr  ��λ��ַ����ʵ�����鲿
*             _phase �����λ����λ�Ƕ��ƣ���Χ(-180, 180]
*             _usFFTPoints  ����������ÿ������������float64_t��ֵ
*             _uiCmpValue  �Ƚ�ֵ����Ҫ�����λ����ֵ
*	�� �� ֵ: ��
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
		lX= _ptr[2*i];  	  /* ʵ�� */
		lY= _ptr[2*i + 1];    /* �鲿 */ 
		
 		phase = atan2(lY, lX);      /* atan2���Ľ����Χ��(-pi, pi], ������ */
		mag = sqrt(lX*lX+ lY*lY);   /* ��ģ */
		
		if(_uiCmpValue > mag)
		{
			_phase[i] = 0;			
		}
		else
		{
			_phase[i] = phase* 180.0/3.1415926;  /* �����Ľ���ɻ���ת��Ϊ�Ƕ� */
		}
	}
}



