#include "bsp.h"

static uint8_t buf[100];
long min = 65535;
long max = 0;
uint16_t vol = 0;
float g_max_fre_amp = 0;
uint16_t g_base_fre = 0;

// OS_TSK_REGISTER(bsps_mea_vpp,PRIORITY_3,1000);
// OS_TSK_REGISTER(bsps_mea_vmax,PRIORITY_3,1000);
// OS_TSK_REGISTER(bsps_mea_vmin,PRIORITY_3,1000);
// OS_TSK_REGISTER(bsps_mea_fre,PRIORITY_3,1000);

static void bsps_mea_vpp(void)
{
	if(osc_mode)
	{
		return;
	}
	
	max = 0;
	min = 65535;
	for(uint16_t i = 0 ; i < 2048 ; i++)
	{
		if(ADCxValues[i] > max)
		{
			max = ADCxValues[i];
		}
		else if(ADCxValues[i] < min)
		{
			min = ADCxValues[i];
		}
	}
	
	vol = 3.3 * (max - min) / 65535.0 * 1000.0;
	
	FONT_T tFont;
	tFont.FontCode = FC_ST_16;		
	tFont.FrontColor = CL_YELLOW;	
	tFont.BackColor = CL_MASK;	 	
	tFont.Space = 0;
	
	bsp_lcd_fill_rect(LTDC_LAYER_1,240,441,35,80,CL_BLACK);
	if(vol < 1000)
	{
		sprintf((char *)buf, "%.1f mV",(float)vol);
	}
	else
	{
		sprintf((char *)buf, " %.3f V",(float)vol / 1000.0);
	}
	LCD_DispStr(LTDC_LAYER_1,240, 30, (char *)buf, &tFont);
}

void bsps_mea_vmax(void)
{
	if(osc_mode)
	{
		return;
	}
	
	max = 3.3 * max / 65535.0 * 1000.0 - 1000;
	
	FONT_T tFont;
	tFont.FontCode = FC_ST_16;		
	tFont.FrontColor = CL_YELLOW;	
	tFont.BackColor = CL_MASK;	 	
	tFont.Space = 0;
	
	bsp_lcd_fill_rect(LTDC_LAYER_1,390,441,35,80,CL_BLACK);
	if(max < 1000)
	{
		sprintf((char *)buf, "%.1f mV",(float)max);
	}
	else
	{
		sprintf((char *)buf, " %.3f V",(float)max / 1000.0);
	}
	LCD_DispStr(LTDC_LAYER_1,390, 30, (char *)buf, &tFont);
}

void bsps_mea_vmin(void)
{
	if(osc_mode)
	{
		return;
	}
	
	if(osc_mode)
	{
		return;
	}
	
	min = 3.3 * min / 65535.0 * 1000.0 - 1000;
	
	FONT_T tFont;
	tFont.FontCode = FC_ST_16;		
	tFont.FrontColor = CL_YELLOW;	
	tFont.BackColor = CL_MASK;	 	
	tFont.Space = 0;
	
	bsp_lcd_fill_rect(LTDC_LAYER_1,540,441,35,80,CL_BLACK);
	if(min < 1000)
	{
		sprintf((char *)buf, "%.1f mV",(float)min);
	}
	else
	{
		sprintf((char *)buf, " %.3f V",(float)min / 1000.0);
	}
	LCD_DispStr(LTDC_LAYER_1,540, 30, (char *)buf, &tFont);
}

void bsps_mea_fre(void)
{
	if(osc_mode)
	{
		return;
	}
	
	//这里后续通过硬件比较器斩成方波后使用定时器读取次数
	
	uint32_t fre = 0;
	FONT_T tFont;
	tFont.FontCode = FC_ST_16;		
	tFont.FrontColor = CL_YELLOW;
	tFont.BackColor = CL_MASK;	 	
	tFont.Space = 0;
	
	bsps_fft_cal(actu_data);
	bsps_mea_find_max();
	
	fre = g_base_fre * g_sa_rate_wave / 2.048;
	
	bsp_lcd_fill_rect(LTDC_LAYER_1,690,441,35,100,CL_BLACK);
	
	if(fre >= 1000)
	{
		sprintf((char *)buf, "%.3f KHz",fre / 1000.0);
	}
	else if(fre >= 1000000)
	{
		sprintf((char *)buf, "%.3f MHz",fre / 1000000.0);
	}
	else
	{
		sprintf((char *)buf, "%d Hz",fre);
	}
	LCD_DispStr(LTDC_LAYER_1,690, 30, (char *)buf, &tFont);
}

void bsps_mea_find_max(void)
{
	g_max_fre_amp = 0;
	
	for(uint16_t i = 3 ; i < 1024 ; i++)			//从1开始去除直流分量
	{
		if(g_max_fre_amp < testOutputMag_f32[i])
		{
			g_max_fre_amp = testOutputMag_f32[i];
			g_base_fre = i;
		}
	}
	g_max_fre_amp = testOutputMag_f32[g_base_fre] * 1.852;
//	printf("fre:%d amp:%.1f",g_base_fre,g_max_fre_amp);
}




