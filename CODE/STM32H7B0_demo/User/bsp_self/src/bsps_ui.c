/*
 * @Author: SUN  BI4NEG@gmail.com
 * @Date: 2023-09-11 10:17:02
 * @LastEditTime: 2023-12-26 17:36:13
 * @Description: 请填写简介
 */
/*
 * @Author: SUN  BI4NEG@gmail.com
 * @Date: 2023-09-11 10:17:02
 * @LastEditTime: 2023-11-25 14:14:29
 * @Description: 请填写简介
 */
#include "bsp.h"

uint16_t last_trig_pos = 20;
static uint8_t buf[100];
uint16_t g_menu_depth = 0;
uint16_t base_fre = 20;
float max_fre_amp = 200;
uint16_t sa_rate_spec = 0;
uint8_t flag_isfist = 0;
__IO uint16_t x_s, y_s;

widget_t trig_lines[6];
window_t win_main, right_menu, ch12_menu[2], trig_menu;
static gui_dev_t *dev;
uint16_t trig_lines_hold_time_s = 0;
widget_t wd1;
widget_t right_btn[6];
widget_t ch12_btn[2];
widget_t ch1_bck_btn[3];
widget_t ch1_btn[6];
widget_t ch2_bck_btn[3];
widget_t ch2_btn[6];
widget_t trig_btn;
widget_t trig_bck_btn[3];
widget_t trig_sel_btn[8];
widget_t mea_bck_btn[8];
const char *ch12_btn_text[6] = {"AC", "X10", "ON", "DC", "X1", "OFF"};
const char *trig_btn_text[8] = {"AT", "↑", "C1", "NR", "↓", "C2", "SG", "UF"};

void osc_calculate_main_size(gui_dev_t *dev, window_t *win, unsigned short wf);
void osc_calculate_trig_line(window_t *pwin, widget_t *wd, int chn);
void osc_ui_l1_btn_draw(widget_t *wd);
static void osc_cfg_task(void);
void osc_ui_right_btn_calculate(widget_t wid[]);
void osc_ui_right_menu_calculate(window_t *win);
void osc_ui_ch12_ctl_btn_calculate(widget_t wid[]);
void osc_ui_ch12_menu_calculate(window_t *win, widget_t *wid);
void osc_ui_ch12_btn_calculate(widget_t wid[], widget_t wid_btn[], window_t *par);
void osc_ui_trig_btn_calculate(widget_t *wid);
void osc_ui_trig_menu_calculate(window_t *win, widget_t *wid);
void osc_ui_trig_menu_btn_calculate(widget_t wid[], widget_t wid_btn[], window_t *par);
void osc_ui_mea_btn_calculate(widget_t wid[]);

OS_INIT_REGISTER("ui_wave_init", bsps_ui_wave_init, 0, 1);

OS_TSK_REGISTER(bsps_ui_is_trig, PRIORITY_3, 500);
OS_TSK_REGISTER(osc_cfg_task, PRIORITY_4, 1000);

// 主窗口绘制
void bsps_ui_main_win_draw(void)
{
	// 绘制窗口
	bsp_lcd_draw_rect(20, 40, 400, 700, CL_GREY);

	// 绘制xy轴
	if (!osc_mode)
	{
		bsp_lcd_draw_line(20, 240, 720, 240, CL_GREY);
		bsp_lcd_draw_line(370, 40, 370, 440, CL_GREY);
	}

	// 绘制纵轴网格
	for (uint16_t i = 40; i < 440; i++)
	{
		if (!(i % 10))
		{
			bsp_lcd_set_pixel(70, i, CL_GREY);
			bsp_lcd_set_pixel(120, i, CL_GREY);
			bsp_lcd_set_pixel(170, i, CL_GREY);
			bsp_lcd_set_pixel(220, i, CL_GREY);
			bsp_lcd_set_pixel(270, i, CL_GREY);
			bsp_lcd_set_pixel(320, i, CL_GREY);
			bsp_lcd_set_pixel(370, i, CL_GREY);
			bsp_lcd_set_pixel(420, i, CL_GREY);
			bsp_lcd_set_pixel(470, i, CL_GREY);
			bsp_lcd_set_pixel(520, i, CL_GREY);
			bsp_lcd_set_pixel(570, i, CL_GREY);
			bsp_lcd_set_pixel(620, i, CL_GREY);
			bsp_lcd_set_pixel(670, i, CL_GREY);
		}
	}
	// 绘制横轴网格
	for (uint16_t i = 20; i < 720; i++)
	{
		if (!(i % 10))
		{
			bsp_lcd_set_pixel(i, 90, CL_GREY);
			bsp_lcd_set_pixel(i, 140, CL_GREY);
			bsp_lcd_set_pixel(i, 190, CL_GREY);
			bsp_lcd_set_pixel(i, 240, CL_GREY);
			bsp_lcd_set_pixel(i, 290, CL_GREY);
			bsp_lcd_set_pixel(i, 340, CL_GREY);
			bsp_lcd_set_pixel(i, 390, CL_GREY);
		}
	}
}

// 触发图标绘制
void bsps_ui_trig_icon_draw(uint16_t pos)
{
	bsp_lcd_draw_tri(10, last_trig_pos - 10, 20, CL_BLACK);
	bsp_lcd_draw_tri(10, pos - 10, 20, CL_ORIG);
	last_trig_pos = pos;
}

// 触发线绘制
void bsps_ui_trig_line_draw(uint16_t pos, uint8_t is_dis)
{
	for (uint16_t i = 21; i < 719; i++)
	{
		if (!(i % 3))
		{
			if (is_dis)
			{
				bsp_lcd_set_pixel(i, pos, CL_ORIG);
			}
			else
			{
				bsp_lcd_set_pixel(i, pos, CL_BLACK);
			}
		}
	}
}

//// 按钮绘制 legacy
// void bsps_ui_btn_draw(void)
//{
//	const unsigned short mod0 = 0x0364;
//	const unsigned short mod1 = 0x4630;
//	const unsigned short mod2 = 0x0C62;
//	const unsigned short mod3 = 0x26C0;

//	uint16_t x_offset = 720, y_offset = 40, x_size = 70, y_size = 60;

//	run_msg_t *run_msg = bsps_get_run_msg();

//	FONT_T tFont;
//	tFont.FontCode = FC_ST_24;
//	tFont.FrontColor = CL_GREY;
//	tFont.BackColor = CL_MASK;
//	tFont.Space = 0;

//	bsp_lcd_fill_rect(721, 45, 400, 80, CL_BLACK);

//	for (uint8_t k = 0; k < 6; k++)
//	{
//		uint16_t pos_x = 5 + x_offset;
//		uint16_t pos_y = (400 / 6 + 2) * k + y_offset;

//		for (int i = 0; i < 16; i++)
//		{
//			if ((mod0 << i) & 0x8000) // i == 6 7 9 10 13		(2,1) (3,1) (1,1) (2,2) (1,3)
//			{
//				bsp_lcd_set_pixel(i % 4 + pos_x, i / 4 + pos_y, CL_GREY);
//			}
//			if ((mod1 << i) & 0x8000)
//			{
//				bsp_lcd_set_pixel(i % 4 + pos_x, i / 4 + pos_y + y_size - 4, CL_GREY);
//			}
//			if ((mod2 << i) & 0x8000)
//			{
//				bsp_lcd_set_pixel(i % 4 + pos_x + x_size - 4, i / 4 + pos_y, CL_GREY);
//			}
//			if ((mod3 << i) & 0x8000)
//			{
//				bsp_lcd_set_pixel(i % 4 + pos_x + x_size - 4, i / 4 + pos_y + y_size - 4, CL_GREY);
//			}
//		}

//		for (int i = 0; i < x_size - 8; i++)
//		{
//			bsp_lcd_set_pixel(pos_x + 4 + i, pos_y, CL_GREY);
//			bsp_lcd_set_pixel(pos_x + 4 + i, pos_y + y_size - 1, CL_GREY);
//		}
//		/* line left and right */
//		for (int i = 0; i < y_size - 8; i++)
//		{
//			bsp_lcd_set_pixel(pos_x, pos_y + 4 + i, CL_GREY);
//			bsp_lcd_set_pixel(pos_x + x_size - 1, pos_y + 4 + i, CL_GREY);
//		}
//	}

//	// bsp_lcd_draw_rect(730, 375, 50, 60, CL_GREY);
//	// bsp_lcd_draw_rect(730, 295, 50, 60, CL_GREY);
//	// bsp_lcd_draw_rect(730, 215, 50, 60, CL_GREY);
//	// bsp_lcd_draw_rect(730, 135, 50, 60, CL_GREY);
//	// bsp_lcd_draw_rect(730, 55, 50, 60, CL_GREY);

//	if (run_msg->run_mode)
//	{
//		switch (run_msg->menu_depth)
//		{
//		case 0:
//			LCD_DispStr(743, 407, "采样", &tFont);
//			LCD_DispStr(743, 327, "NULL", &tFont);
//			LCD_DispStr(743, 247, "NULL", &tFont);
//			LCD_DispStr(743, 167, "NULL", &tFont);
//			LCD_DispStr(743, 87, "波形", &tFont);
//			break;
//		case 1:
//			LCD_DispStr(743, 407, "速率+", &tFont);
//			LCD_DispStr(743, 327, "速率-", &tFont);
//			LCD_DispStr(743, 247, "点数+", &tFont);
//			LCD_DispStr(743, 167, "点数-", &tFont);
//			LCD_DispStr(743, 87, "返回", &tFont);
//			break;
//		}
//	}
//	else
//	{
//		switch (run_msg->menu_depth)
//		{
//		case 0:
//			LCD_DispStr(743, 418, "CH1", &tFont);
//			LCD_DispStr(743, 350, "CH2", &tFont);
//			LCD_DispStr(735, 283, "TRIG", &tFont);
//			LCD_DispStr(735, 215, "SPEC", &tFont);
//			LCD_DispStr(735, 148, "UTIL", &tFont);
//			LCD_DispStr(730, 80, "", &tFont);
//			break;
//		case 1:
//			LCD_DispStr(743, 407, "COUP", &tFont);
//			LCD_DispStr(743, 327, "RATIO", &tFont);
//			LCD_DispStr(743, 247, "", &tFont);
//			LCD_DispStr(743, 167, "", &tFont);
//			LCD_DispStr(743, 87, "BACK", &tFont);
//			break;
//		case 2:
//			LCD_DispStr(743, 407, "COUP", &tFont);
//			LCD_DispStr(743, 327, "RATIO", &tFont);
//			LCD_DispStr(743, 247, "", &tFont);
//			LCD_DispStr(743, 167, "", &tFont);
//			LCD_DispStr(743, 87, "BACK", &tFont);
//			break;
//		case 3:
//			LCD_DispStr(743, 407, "TYPE", &tFont);
//			LCD_DispStr(743, 327, "SRC", &tFont);
//			LCD_DispStr(743, 247, "EDGE", &tFont);
//			LCD_DispStr(743, 167, "", &tFont);
//			LCD_DispStr(743, 87, "BACK", &tFont);
//			break;
//		}
//	}
//}

// 是否触发图标
static void bsps_ui_is_trig(void)
{
	run_msg_t *run_msg = bsps_get_run_msg();

	if (run_msg->run_mode)
	{
		return;
	}

	FONT_T tFont;
	tFont.FontCode = FC_ST_24;
	tFont.FrontColor = CL_CH12_BTN_BLOD;
	tFont.BackColor = CL_CH12_BTN_BLOD;
	tFont.Space = 0;

	LCD_DispStr(LTDC_LAYER_1, trig_btn.msg.x + 5, 470, "T", &tFont);

	if (flag_isfist)
	{
		if (!run_msg->is_trig || run_msg->is_pause || (run_msg->ch1_sta == CH_OFF && run_msg->ch2_sta == CH_OFF) || run_msg->trig_mode == TRIG_SIGLE)
		{
			tFont.BackColor = CL_RED;
		}
		else
		{
			tFont.BackColor = CL_GREEN;
		}
		LCD_DispStr(LTDC_LAYER_1, trig_btn.msg.x + 5, 470, "T", &tFont);
		flag_isfist = 0;
	}
	else
	{
		flag_isfist = 1;
	}
}

// acdc图标绘制
void bsps_ui_ch12_acdc_draw(uint8_t is_ac, uint8_t ch)
{
	FONT_T tFont;
	tFont.FontCode = FC_ST_12;
	tFont.FrontColor = CL_YELLOW;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;

	if (is_ac)
	{
		tFont.FrontColor = CL_CH12_BTN_BLOD;
		LCD_DispStr(LTDC_LAYER_1, ch12_btn[ch].msg.x + 35, ch12_btn[ch].msg.y + ch12_btn[ch].msg.y_size - 5, "DC", &tFont);
		tFont.FrontColor = CL_GREY2;
		LCD_DispStr(LTDC_LAYER_1, ch12_btn[ch].msg.x + 35, ch12_btn[ch].msg.y + ch12_btn[ch].msg.y_size - 5, "AC", &tFont);
	}
	else
	{
		tFont.FrontColor = CL_CH12_BTN_BLOD;
		LCD_DispStr(LTDC_LAYER_1, ch12_btn[ch].msg.x + 35, ch12_btn[ch].msg.y + ch12_btn[ch].msg.y_size - 5, "AC", &tFont);
		tFont.FrontColor = CL_GREY2;
		LCD_DispStr(LTDC_LAYER_1, ch12_btn[ch].msg.x + 35, ch12_btn[ch].msg.y + ch12_btn[ch].msg.y_size - 5, "DC", &tFont);
	}
}

// x1x10图标绘制
void bsps_ui_ch12_ratio_draw(uint8_t is_x10, uint8_t ch)
{
	FONT_T tFont;
	tFont.FontCode = FC_ST_12;
	tFont.FrontColor = CL_YELLOW;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;

	if (is_x10)
	{
		tFont.FrontColor = CL_CH12_BTN_BLOD;
		LCD_DispStr(LTDC_LAYER_1, ch12_btn[ch].msg.x + 35, ch12_btn[ch].msg.y + ch12_btn[ch].msg.y_size - 20, "x1", &tFont);
		tFont.FrontColor = CL_GREY2;
		LCD_DispStr(LTDC_LAYER_1, ch12_btn[ch].msg.x + 35, ch12_btn[ch].msg.y + ch12_btn[ch].msg.y_size - 20, "x10", &tFont);
	}
	else
	{
		tFont.FrontColor = CL_CH12_BTN_BLOD;
		LCD_DispStr(LTDC_LAYER_1, ch12_btn[ch].msg.x + 35, ch12_btn[ch].msg.y + ch12_btn[ch].msg.y_size - 20, "x10", &tFont);
		tFont.FrontColor = CL_GREY2;
		LCD_DispStr(LTDC_LAYER_1, ch12_btn[ch].msg.x + 35, ch12_btn[ch].msg.y + ch12_btn[ch].msg.y_size - 20, "x1", &tFont);
	}
}

// 时基数据绘制
void bsps_ui_timebase_draw(char *str)
{
	FONT_T tFont;
	tFont.FontCode = FC_ST_24;
	tFont.FrontColor = CL_GREY2;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;

	bsp_lcd_fill_rect(LTDC_LAYER_1, ch12_btn[1].msg.x + ch12_btn[1].msg.x_size + 20, 0, 35, 130, CL_BLACK);

	LCD_DispStr(LTDC_LAYER_1, ch12_btn[1].msg.x + ch12_btn[1].msg.x_size + 30, 470, "T:", &tFont);
	LCD_DispStr(LTDC_LAYER_1, ch12_btn[1].msg.x + ch12_btn[1].msg.x_size + 30 + 20, 472, str, &tFont);
}

// 采样率数据绘制
void bsps_ui_sa_rate_draw(uint16_t rate)
{
	FONT_T tFont;
	tFont.FontCode = FC_ST_24;
	tFont.FrontColor = CL_YELLOW;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;

	bsp_lcd_fill_rect(LTDC_LAYER_1, 500, 0, 35, 150, CL_BLACK);

	if (rate >= 1000)
	{
		sprintf((char *)buf, "Sa:%d Msps", rate / 1000);
		LCD_DispStr(LTDC_LAYER_1, 500, 470, (char *)buf, &tFont);
	}
	else
	{
		sprintf((char *)buf, "Sa:%d Ksps", rate);
		LCD_DispStr(LTDC_LAYER_1, 500, 470, (char *)buf, &tFont);
	}
}

// 触发电压数据绘制
void bsps_ui_trig_vol_draw(float vol)
{
	FONT_T tFont;
	tFont.FontCode = FC_ST_16;
	tFont.FrontColor = CL_GREY2;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;

	bsp_lcd_fill_rect(LTDC_LAYER_1, trig_btn.msg.x + 25, 481 + 2 - (trig_btn.msg.y + trig_btn.msg.y_size), 16, 100, CL_CH12_BTN_BLOD);

	sprintf((char *)buf, "Vol:%.2fV", vol);
	LCD_DispStr(LTDC_LAYER_1, trig_btn.msg.x + 25, trig_btn.msg.y - 3 + trig_btn.msg.y_size, (char *)buf, &tFont);
}

// 垂直电压控制数据绘制
void bsps_ui_ch12_vol_gain_draw(char *str, uint8_t ch)
{
	FONT_T tFont;
	tFont.FontCode = FC_ST_16;
	tFont.FrontColor = CL_GREY2;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;
	bsp_lcd_fill_rect(LTDC_LAYER_1, ch12_btn[ch].msg.x + 55, 481 - (ch12_btn[ch].msg.y + ch12_btn[ch].msg.y_size), ch12_btn[ch].msg.y_size, 40, CL_CH12_BTN_BLOD);
	// LCD_DispStr(LTDC_LAYER_1, 20, 35, "↑", &tFont);
	LCD_DispStr(LTDC_LAYER_1, ch12_btn[ch].msg.x + 60, ch12_btn[ch].msg.y + ch12_btn[ch].msg.y_size / 2 + 6, (char *)str, &tFont);
}

void bsps_ui_trig_mode_draw(uint8_t mode)
{
	FONT_T tFont;
	tFont.FontCode = FC_ST_16;
	tFont.FrontColor = CL_GREY2;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;

	bsp_lcd_fill_rect(LTDC_LAYER_1, trig_btn.msg.x + 60, 481 - (trig_btn.msg.y + trig_btn.msg.y_size) + 18, 15, 35, CL_CH12_BTN_BLOD);

	if (mode == TRIG_AUTO)
	{
		LCD_DispStr(LTDC_LAYER_1, trig_btn.msg.x + 60, (trig_btn.msg.y + trig_btn.msg.y_size) - 19, "AUTO", &tFont);
	}
	else if (mode == TRIG_NORMAL)
	{
		LCD_DispStr(LTDC_LAYER_1, trig_btn.msg.x + 60, (trig_btn.msg.y + trig_btn.msg.y_size) - 19, "NORM", &tFont);
	}
	else if (mode == TRIG_SIGLE)
	{
		LCD_DispStr(LTDC_LAYER_1, trig_btn.msg.x + 60, (trig_btn.msg.y + trig_btn.msg.y_size) - 19, "SING", &tFont);
	}
}

void bsps_ui_trig_ch_draw(uint8_t ch)
{
	FONT_T tFont;
	tFont.FontCode = FC_ST_16;
	tFont.FrontColor = CL_CH12_BTN_BLOD;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;

	if (ch == TRIG_CH1)
	{
		bsp_lcd_fill_rect(LTDC_LAYER_1, trig_btn.msg.x + 23, 481 - (trig_btn.msg.y + trig_btn.msg.y_size) + 18, 15, 26, CL_CH1);
		LCD_DispStr(LTDC_LAYER_1, trig_btn.msg.x + 25, (trig_btn.msg.y + trig_btn.msg.y_size) - 19, "CH1", &tFont);
	}
	else if (ch == TRIG_CH2)
	{
		bsp_lcd_fill_rect(LTDC_LAYER_1, trig_btn.msg.x + 23, 481 - (trig_btn.msg.y + trig_btn.msg.y_size) + 18, 15, 26, CL_CH2);
		LCD_DispStr(LTDC_LAYER_1, trig_btn.msg.x + 25, (trig_btn.msg.y + trig_btn.msg.y_size) - 19, "CH2", &tFont);
	}
}

void bsps_ui_trig_edge_draw(uint8_t edge)
{
	FONT_T tFont;
	tFont.FontCode = FC_ST_16;
	tFont.FrontColor = CL_GREY2;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;

	if (edge == TRIG_EDGE_RISE)
	{
		bsp_lcd_fill_rect(LTDC_LAYER_1, trig_btn.msg.x + 95, 481 - (trig_btn.msg.y + trig_btn.msg.y_size) + 18, 16, 32, CL_CH12_BTN_BLOD);
		LCD_DispStr(LTDC_LAYER_1, trig_btn.msg.x + 95, (trig_btn.msg.y + trig_btn.msg.y_size) - 19, "↑", &tFont);
	}
	else if (edge == TRIG_EDGE_FALL)
	{
		bsp_lcd_fill_rect(LTDC_LAYER_1, trig_btn.msg.x + 95, 481 - (trig_btn.msg.y + trig_btn.msg.y_size) + 18, 16, 32, CL_CH12_BTN_BLOD);
		LCD_DispStr(LTDC_LAYER_1, trig_btn.msg.x + 95, (trig_btn.msg.y + trig_btn.msg.y_size) - 19, "↓", &tFont);
	}
	else if(edge == TRIG_EDGE_RISE_FALL)
	{
		bsp_lcd_fill_rect(LTDC_LAYER_1, trig_btn.msg.x + 95, 481 - (trig_btn.msg.y + trig_btn.msg.y_size) + 18, 16, 32, CL_CH12_BTN_BLOD);
		LCD_DispStr(LTDC_LAYER_1, trig_btn.msg.x + 95, (trig_btn.msg.y + trig_btn.msg.y_size) - 19, "↑↓", &tFont);
	}
}

void bsps_ui_base_fre_draw(void)
{
	FONT_T tFont;
	tFont.FontCode = FC_ST_16;
	tFont.FrontColor = CL_BLACK;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;

	bsps_mea_find_max();

	sprintf((char *)buf, "%dHz %.3fV", (int)(base_fre * sa_rate_spec / 2.048), max_fre_amp / 1024.0);
	LCD_DispStr(LTDC_LAYER_1, base_fre + 30, max_fre_amp / 6 + 60, (char *)buf, &tFont);

	tFont.FrontColor = CL_YELLOW;
	sprintf((char *)buf, "%dHz %.3fV", (int)(g_base_fre * g_sa_rate_spec / 2.048), g_max_fre_amp / 1024.0);
	LCD_DispStr(LTDC_LAYER_1, g_base_fre + 30, g_max_fre_amp / 6 + 60, (char *)buf, &tFont);

	base_fre = g_base_fre;
	max_fre_amp = g_max_fre_amp;
	sa_rate_spec = g_sa_rate_spec;
}

// 固定字绘制
void bsps_ui_char_draw(void)
{
	FONT_T tFont;
	tFont.FontCode = FC_ST_24;
	tFont.FrontColor = CL_YELLOW;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;

	LCD_DispStr(LTDC_LAYER_1, 10, 470, "OSC", &tFont);
	LCD_DispStr(LTDC_LAYER_1, 46, 472, "1.0", &tFont);

	if (!osc_mode)
	{
		tFont.FontCode = FC_ST_16;
		LCD_DispStr(LTDC_LAYER_1, 180, 30, "峰峰值=", &tFont);
		LCD_DispStr(LTDC_LAYER_1, 330, 30, "最大值=", &tFont);
		LCD_DispStr(LTDC_LAYER_1, 480, 30, "最小值=", &tFont);
		LCD_DispStr(LTDC_LAYER_1, 630, 30, "频率值=", &tFont);
	}
}

// 波形ui初始化
int bsps_ui_wave_init(void)
{
	run_msg_t *run_msg = bsps_get_run_msg();
	dev = get_gui_dev();

	// bsp_lcd_clr_scr(CL_BLACK);
	// bsps_ui_main_win_draw();
	// bsps_ui_trig_icon_draw(238);
	// bsps_ui_char_draw();
	// bsps_ui_acdc_draw(run_msg->acdc);
	// bsps_ui_timebase_draw(osc_tim_table[run_msg->timebase].str);
	// bsps_ui_trig_vol_draw(0);
	// if (run_msg->ratio == RATIO_10X)
	// {
	// 	bsps_ui_vol_gain_draw(osc_vol_scale_table[run_msg->vol_scale[2]].strX10);
	// }
	// else
	// {
	// 	bsps_ui_vol_gain_draw(osc_vol_scale_table[run_msg->vol_scale[2]].str);
	// }
	// bsps_ui_btn_draw();

	// osc_dev_l2_enable(0,0,200,200,255);

	osc_calculate_main_size(dev, &win_main, 0);
	osc_ui_right_menu_calculate(&right_menu);
	osc_calculate_trig_line(&win_main, &trig_lines[0], 1);
	osc_ui_right_btn_calculate(right_btn);
	osc_ui_ch12_ctl_btn_calculate(ch12_btn);
	osc_ui_ch12_menu_calculate(&ch12_menu[0], &ch12_btn[0]);
	osc_ui_ch12_menu_calculate(&ch12_menu[1], &ch12_btn[1]);
	osc_ui_ch12_btn_calculate(ch1_bck_btn, ch1_btn, &ch12_menu[0]);
	osc_ui_ch12_btn_calculate(ch2_bck_btn, ch2_btn, &ch12_menu[1]);
	osc_ui_trig_btn_calculate(&trig_btn);
	osc_ui_trig_menu_calculate(&trig_menu, &trig_btn);
	osc_ui_trig_menu_btn_calculate(trig_bck_btn, trig_sel_btn, &trig_menu);
	osc_ui_mea_btn_calculate(mea_bck_btn);

	// gui_show_win(&ch12_menu[1]);

	// gui_hide_win(&ch12_btn[1]);

	// osc_ui_ch1_btn_sel(3);

	// wd.msg.wflags &= ~0xF000;
	// wd1.msg.x = 100;
	// wd1.msg.y = 100;
	// wd1.msg.x_size = 70;
	// wd1.msg.y_size = 50;
	// wd1.draw = osc_ui_l1_btn_draw;
	// wd1.peer_linker = 0;
	// wd1.parent = &win_main;

	// gui_wid_creater(&wd1);

	return OS_OK;
}

// 频谱ui初始化
void bsps_ui_spec_init(void)
{
	bsp_lcd_clr_scr(CL_BLACK);
	bsps_ui_main_win_draw();
	bsps_ui_char_draw();
	//	bsps_ui_btn_draw();
	bsps_ui_sa_rate_draw(1000);
}

void osc_calculate_main_size(gui_dev_t *dev, window_t *win, unsigned short wf)
{
	win->msg.x = 0;
	win->msg.y = 0;
	win->msg.x_size = dev->width;
	win->msg.y_size = dev->height;
	win->dev = dev;

	win->draw = bsps_ui_main_win_draw;

	win->msg.wflags = wf | 0xE0;

	gui_win_creater(win);
}

static void osc_draw_trig_line(widget_t *wd)
{
	uint8_t rehide_flag = 0;

	const uint16_t color_table_trig_lines[2][2] =
		{
			{
				CL_ORIG,
				CL_ORIG,
			},
			{
				CL_ORIG,
				CL_ORIG,
			},
		};
	// 当 wd->msg.mark_flag == 2 时为移动横线，当 REHIDE 置位时为仅消除横线
	if (CHECK_REHIDE(wd->msg.wflags) || wd->msg.mark_flag == 2)
	{
		rehide_flag = 1;

		if (CHECK_REHIDE(wd->msg.wflags))
		{
			wd->msg.mark_flag = 1;
		}

		CLEAR_REHIDE(wd->msg.wflags);
	}
	else
	{
		wd->msg.mark_flag = 1;
	}

	int color_index = 0;

	const uint16_t *chn = color_table_trig_lines[color_index];

	while (wd->msg.mark_flag)
	{

		uint8_t xy = 0;
		uint16_t start_xy = 20;
		uint16_t stop_xy = 720;

		for (int i = start_xy; i < stop_xy; i += 2)
		{
			uint16_t piox;

			if (xy == 0)
			{
				piox = wd->dev->read_point(i, wd->msg.y);
			}
			else
			{
				piox = wd->dev->read_point(wd->msg.x, i);
			}

			if (rehide_flag == 0)
			{
				if (xy == 0)
				{
					if (piox == CL_GREY)
					{
						wd->dev->set_noload_point(i, wd->msg.y, chn[0]);
					}
					else if (piox == CL_BLACK)
					{
						wd->dev->set_noload_point(i, wd->msg.y, chn[1]);
					}
					else
					{
					}
				}
				else
				{

					if (piox == CL_GREY)
					{
						wd->dev->set_noload_point(wd->msg.x, i, chn[0]);
					}
					else if (piox == CL_BLACK)
					{
						wd->dev->set_noload_point(wd->msg.x, i, chn[1]);
					}
					else
					{
					}
				}
			}
			else
			{
				if (xy == 0)
				{
					if (piox == chn[0])
					{
						wd->dev->set_noload_point(i, wd->msg.y, CL_GREY);
					}
					else if (piox == chn[1])
					{
						wd->dev->set_noload_point(i, wd->msg.y, CL_BLACK);
					}
					else
					{
					}
				}
				else
				{

					if (piox == chn[0])
					{
						wd->dev->set_noload_point(wd->msg.x, i, CL_GREY);
					}
					else if (piox == chn[1])
					{
						wd->dev->set_noload_point(wd->msg.x, i, CL_BLACK);
					}
					else
					{
					}
				}
			}
		}

		wd->msg.mark_flag--;

		if (wd->msg.mark_flag)
		{
			if (xy == 0)
			{
				wd->msg.y = wd->msg.my;
			}
			else
			{
				wd->msg.x = wd->msg.mx;
			}
			rehide_flag = 0;
		}
	}
}

void osc_calculate_trig_line(window_t *pwin, widget_t *wd, int chn)
{
	wd->msg.wflags &= ~0xF000;

	if (chn == 1)
	{
		wd->msg.wflags |= 0x0000;
		wd->msg.y = 100;
	}
	else if (chn == 2)
	{
		wd->msg.wflags |= 0x1000;
		wd->msg.y = 0x16A + 6;
	}

	if (chn < 3)
	{
		SET_HIDE(wd->msg.wflags);
	}

	wd->dev = pwin->dev;
	wd->parent = pwin;
	wd->draw = osc_draw_trig_line;

	wd->peer_linker = 0;

	gui_wid_creater(wd);
}

void osc_ui_move_trig_lines(uint8_t chn, uint64_t posy)
{
	if (chn < 2)
	{
		gui_move_widget(&trig_lines[chn], 0, posy);
	}
}

void osc_ui_trig_lines_show(unsigned char chn, unsigned char mode)
{
	/* limit */
	if (chn < 2)
	{
		/* mode */
		if (mode == 0)
		{
			gui_hide_widget(&trig_lines[chn]);
		}
		else
		{
			gui_show_widget(&trig_lines[chn]);
		}
	}
}

static void osc_cfg_task(void)
{
	if (trig_lines_hold_time_s == 0)
	{
		osc_ui_trig_lines_show(0, 0);
		osc_ui_trig_lines_show(1, 0);
	}
	else
	{
		trig_lines_hold_time_s--;
	}
}

/**
 * @description: 按钮绘制api
 * @param {widget_t} *wd
 * @return {*}
 */
void osc_ui_l1_btn_draw(widget_t *wd)
{
	// 圆角取模值
	const unsigned short mod0 = 0x0364;
	const unsigned short mod1 = 0x4630;
	const unsigned short mod2 = 0x0C62;
	const unsigned short mod3 = 0x26C0;

	// 读取组件位置信息
	uint16_t x = wd->msg.x;
	uint16_t y = wd->msg.y;
	uint16_t x_size = wd->msg.x_size;
	uint16_t y_size = wd->msg.y_size;

	// 绘制按钮边框圆角
	for (int i = 0; i < 16; i++)
	{
		if ((mod0 << i) & 0x8000) // i == 6 7 9 10 13		(2,1) (3,1) (1,1) (2,2) (1,3)
		{
			bsp_lcd_set_pixel(i % 4 + x, i / 4 + y, wd->msg.color);
		}
		if ((mod1 << i) & 0x8000)
		{
			bsp_lcd_set_pixel(i % 4 + x, i / 4 + y + y_size - 4, wd->msg.color);
		}
		if ((mod2 << i) & 0x8000)
		{
			bsp_lcd_set_pixel(i % 4 + x + x_size - 4, i / 4 + y, wd->msg.color);
		}
		if ((mod3 << i) & 0x8000)
		{
			bsp_lcd_set_pixel(i % 4 + x + x_size - 4, i / 4 + y + y_size - 4, wd->msg.color);
		}
	}

	// 绘制按钮边框直线
	for (int i = 0; i < x_size - 8; i++)
	{
		bsp_lcd_set_pixel(x + 4 + i, y, wd->msg.color);
		bsp_lcd_set_pixel(x + 4 + i, y + y_size - 1, wd->msg.color);
	}
	for (int i = 0; i < y_size - 8; i++)
	{
		bsp_lcd_set_pixel(x, y + 4 + i, wd->msg.color);
		bsp_lcd_set_pixel(x + x_size - 1, y + 4 + i, wd->msg.color);
	}

	if (wd->msg.wflags & GUI_BTN_BOLD)
	{
		for (uint16_t i = x + 1; i <= x + x_size - 2; i++)
		{
			for (uint16_t j = y + 1; j <= y + y_size - 2; j++)
			{
				if ((i == x + 1) && (j == y + 1))
				{
				}
				else if ((i == x + 1) && (j == y + y_size - 2))
				{
				}
				else if ((i == x + x_size - 2) && (j == y + 1))
				{
				}
				else if ((i == x + x_size - 2) && (j == y + y_size - 2))
				{
				}
				else
				{
					bsp_lcd_set_pixel(i, j, wd->msg.color);
				}
			}
		}
	}

	FONT_T tFont;
	tFont.FontCode = FC_ST_16;
	tFont.FrontColor = wd->msg.color;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;

	// 绘制按钮字
	if (wd->msg.color == CL_R_BTN)
	{
		LCD_DispStr(LTDC_LAYER_1, x + 5, y + 43, wd->msg.pri_data, &tFont);
	}

	tFont.FrontColor = CL_CH12_BTN_BLOD;
	if (wd->msg.x == 200)
	{
		bsp_lcd_fill_rect(LTDC_LAYER_1, wd->msg.x + 3, 481 - (wd->msg.y + wd->msg.y_size) + 4, 27, 27, CL_YELLOW);
		LCD_DispStr(LTDC_LAYER_1, wd->msg.x + 5, (wd->msg.y + wd->msg.y_size) - 11, "CH1", &tFont);
		bsps_ui_ch12_acdc_draw(1, 0);
		bsps_ui_ch12_ratio_draw(1, 0);
		bsps_ui_ch12_vol_gain_draw("5V", 0);
	}
	else if (wd->msg.x == 330)
	{
		bsp_lcd_fill_rect(LTDC_LAYER_1, wd->msg.x + 3, 481 - (wd->msg.y + wd->msg.y_size) + 4, 27, 27, CL_CH2);
		LCD_DispStr(LTDC_LAYER_1, wd->msg.x + 5, (wd->msg.y + wd->msg.y_size) - 11, "CH2", &tFont);
		bsps_ui_ch12_acdc_draw(1, 1);
		bsps_ui_ch12_ratio_draw(1, 1);
		bsps_ui_ch12_vol_gain_draw("5V", 1);
	}
	else if (wd->msg.x == 580)
	{
		bsps_ui_trig_vol_draw(0);
		FONT_T tFont;
		tFont.FontCode = FC_ST_16;
		tFont.FrontColor = CL_CH12_BTN_BLOD;
		tFont.BackColor = CL_MASK;
		tFont.Space = 0;

		bsp_lcd_fill_rect(LTDC_LAYER_1, wd->msg.x + 23, 481 - (wd->msg.y + wd->msg.y_size) + 18, 15, 26, CL_YELLOW);
		LCD_DispStr(LTDC_LAYER_1, wd->msg.x + 25, (wd->msg.y + wd->msg.y_size) - 19, "CH1", &tFont);

		tFont.FrontColor = CL_GREY2;
		tFont.BackColor = CL_CH12_BTN_BLOD;
		LCD_DispStr(LTDC_LAYER_1, wd->msg.x + 60, (wd->msg.y + wd->msg.y_size) - 19, "AUTO", &tFont);

		LCD_DispStr(LTDC_LAYER_1, trig_btn.msg.x + 100, (trig_btn.msg.y + trig_btn.msg.y_size) - 19, "↑", &tFont);
	}
}

/**
 * @description: 绘制右侧按钮
 * @param {widget_t} wid
 * @return {*}
 */
void osc_ui_right_btn_calculate(widget_t wid[])
{
	uint16_t x_offset = 720;
	uint16_t y_offset = 0;

	for (uint16_t i = 0; i < 6; i++)
	{
		wid[i].msg.x = x_offset + 5;
		wid[i].msg.y = y_offset + 480 / 6 * i + 5;
		wid[i].msg.x_size = 70;
		wid[i].msg.y_size = 70;
		wid[i].msg.color = CL_R_BTN;
		wid[i].msg.wflags &= ~GUI_BTN_BOLD;
		wid[i].draw = osc_ui_l1_btn_draw;
		wid[i].peer_linker = 0;
		wid[i].parent = &right_menu;
		wid[i].msg.pri_data = "Sample";
		wid[i].msg.layer = LTDC_LAYER_1;
		gui_wid_creater(&wid[i]);
	}
}

/**
 * @description: 创建右侧按钮栏窗口
 * @param {window_t} *win
 * @return {*}
 */
void osc_ui_right_menu_calculate(window_t *win)
{
	win->msg.x = 720;
	win->msg.y = 0;
	win->msg.x_size = 80;
	win->msg.y_size = 480;
	win->dev = dev;
	win->draw = 0;
	win->msg.layer = LTDC_LAYER_1;
	gui_win_creater(win);
}

/**
 * @description: 计算ch12控制按钮
 * @param {widget_t} wid
 * @return {*}
 */
void osc_ui_ch12_ctl_btn_calculate(widget_t wid[])
{
	uint16_t x_offset = 200;
	uint16_t y_offset = 440;

	for (uint16_t i = 0; i < 2; i++)
	{
		wid[i].msg.x = x_offset + 130 * i;
		wid[i].msg.y = y_offset + 2;
		wid[i].msg.x_size = 100;
		wid[i].msg.y_size = 35;
		wid[i].msg.color = CL_CH12_BTN_BLOD;
		wid[i].msg.wflags |= GUI_BTN_BOLD;
		wid[i].draw = osc_ui_l1_btn_draw;
		wid[i].peer_linker = 0;
		wid[i].parent = &win_main;
		wid[i].msg.pri_data = "CH12";
		wid[i].msg.layer = LTDC_LAYER_1;
		gui_wid_creater(&wid[i]);
	}
}

/**
 * @description: ch12通用菜单窗口背景绘制函数
 * @param {window_t} *win
 * @return {*}
 */
void osc_ui_ch12_menu_draw(window_t *win)
{
	x_s = win->msg.x_size;
	y_s = win->msg.y_size;

	osc_dev_l2_enable(win->msg.x, win->msg.y, x_s, y_s, 255);

	// for (uint16_t i = 0; i < win->msg.x_size; i++)
	// {
	// 	for (uint16_t j = 0; j < win->msg.y_size; j++)
	// 	{
	// 		bsp_lcd_set_pixel_l2(win->msg.y_size, win->msg.x_size, i, j, CL_GREEN);
	// 	}
	// }

	// 圆角取模值
	const unsigned short mod0 = 0x0364;
	const unsigned short mod1 = 0x4630;
	const unsigned short mod2 = 0x0C62;
	const unsigned short mod3 = 0x26C0;

	// 读取窗口位置信息
	uint16_t x = 0;
	uint16_t y = 0;
	uint16_t x_size = win->msg.x_size;
	uint16_t y_size = win->msg.y_size;

	// 绘制窗口边框圆角
	for (int i = 0; i < 16; i++)
	{
		if ((mod0 << i) & 0x8000) // i == 6 7 9 10 13		(2,1) (3,1) (1,1) (2,2) (1,3)
		{
			bsp_lcd_set_pixel_l2(win->msg.y_size, win->msg.x_size, i % 4 + x, i / 4 + y, win->msg.color);
		}
		if ((mod1 << i) & 0x8000)
		{
			bsp_lcd_set_pixel_l2(win->msg.y_size, win->msg.x_size, i % 4 + x, i / 4 + y + y_size - 4, win->msg.color);
		}
		if ((mod2 << i) & 0x8000)
		{
			bsp_lcd_set_pixel_l2(win->msg.y_size, win->msg.x_size, i % 4 + x + x_size - 4, i / 4 + y, win->msg.color);
		}
		if ((mod3 << i) & 0x8000)
		{
			bsp_lcd_set_pixel_l2(win->msg.y_size, win->msg.x_size, i % 4 + x + x_size - 4, i / 4 + y + y_size - 4, win->msg.color);
		}
	}

	// 绘制窗口边框直线
	for (int i = 0; i < x_size - 8; i++)
	{
		bsp_lcd_set_pixel_l2(win->msg.y_size, win->msg.x_size, x + 4 + i, y, win->msg.color);
		bsp_lcd_set_pixel_l2(win->msg.y_size, win->msg.x_size, x + 4 + i, y + y_size - 1, win->msg.color);
	}
	for (int i = 0; i < y_size - 8; i++)
	{
		bsp_lcd_set_pixel_l2(win->msg.y_size, win->msg.x_size, x, y + 4 + i, win->msg.color);
		bsp_lcd_set_pixel_l2(win->msg.y_size, win->msg.x_size, x + x_size - 1, y + 4 + i, win->msg.color);
	}

	for (uint16_t i = x + 1; i <= x + x_size - 2; i++)
	{
		for (uint16_t j = y + 1; j <= y + y_size - 2; j++)
		{
			if ((i == x + 1) && (j == y + 1))
			{
			}
			else if ((i == x + 1) && (j == y + y_size - 2))
			{
			}
			else if ((i == x + x_size - 2) && (j == y + 1))
			{
			}
			else if ((i == x + x_size - 2) && (j == y + y_size - 2))
			{
			}
			else
			{
				bsp_lcd_set_pixel_l2(win->msg.y_size, win->msg.x_size, i, j, win->msg.color);
			}
		}
	}
}

/**
 * @description: l2按钮绘制api
 * @param {widget_t} *wd 要绘制的按钮组件
 * @return {*}
 */
void osc_ui_l2_btn_draw(widget_t *wd)
{
	// 圆角取模值
	const unsigned short mod0 = 0x0364;
	const unsigned short mod1 = 0x4630;
	const unsigned short mod2 = 0x0C62;
	const unsigned short mod3 = 0x26C0;

	// 读取组件位置信息
	uint16_t x = wd->msg.x;
	uint16_t y = wd->msg.y;
	uint16_t x_size = wd->msg.x_size;
	uint16_t y_size = wd->msg.y_size;

	// 绘制按钮边框圆角
	for (int i = 0; i < 16; i++)
	{
		if ((mod0 << i) & 0x8000) // i == 6 7 9 10 13		(2,1) (3,1) (1,1) (2,2) (1,3)
		{
			bsp_lcd_set_pixel_l2(wd->parent->msg.y_size, wd->parent->msg.x_size, i % 4 + x, i / 4 + y, wd->msg.color);
		}
		if ((mod1 << i) & 0x8000)
		{
			bsp_lcd_set_pixel_l2(wd->parent->msg.y_size, wd->parent->msg.x_size, i % 4 + x, i / 4 + y + y_size - 4, wd->msg.color);
		}
		if ((mod2 << i) & 0x8000)
		{
			bsp_lcd_set_pixel_l2(wd->parent->msg.y_size, wd->parent->msg.x_size, i % 4 + x + x_size - 4, i / 4 + y, wd->msg.color);
		}
		if ((mod3 << i) & 0x8000)
		{
			bsp_lcd_set_pixel_l2(wd->parent->msg.y_size, wd->parent->msg.x_size, i % 4 + x + x_size - 4, i / 4 + y + y_size - 4, wd->msg.color);
		}
	}

	// 绘制按钮边框直线
	for (int i = 0; i < x_size - 8; i++)
	{
		bsp_lcd_set_pixel_l2(wd->parent->msg.y_size, wd->parent->msg.x_size, x + 4 + i, y, wd->msg.color);
		bsp_lcd_set_pixel_l2(wd->parent->msg.y_size, wd->parent->msg.x_size, x + 4 + i, y + y_size - 1, wd->msg.color);
	}
	for (int i = 0; i < y_size - 8; i++)
	{
		bsp_lcd_set_pixel_l2(wd->parent->msg.y_size, wd->parent->msg.x_size, x, y + 4 + i, wd->msg.color);
		bsp_lcd_set_pixel_l2(wd->parent->msg.y_size, wd->parent->msg.x_size, x + x_size - 1, y + 4 + i, wd->msg.color);
	}

	if (wd->msg.wflags & GUI_BTN_BOLD)
	{
		for (uint16_t i = x + 1; i <= x + x_size - 2; i++)
		{
			for (uint16_t j = y + 1; j <= y + y_size - 2; j++)
			{
				if ((i == x + 1) && (j == y + 1))
				{
				}
				else if ((i == x + 1) && (j == y + y_size - 2))
				{
				}
				else if ((i == x + x_size - 2) && (j == y + 1))
				{
				}
				else if ((i == x + x_size - 2) && (j == y + y_size - 2))
				{
				}
				else
				{
					bsp_lcd_set_pixel_l2(wd->parent->msg.y_size, wd->parent->msg.x_size, i, j, wd->msg.color);
				}
			}
		}
	}

	FONT_T tFont;
	tFont.FontCode = FC_ST_16;
	tFont.FrontColor = CL_GREY2;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;

	if ((wd->msg.y == 135) && (wd->msg.x_size == 120))
	{
		LCD_DispStr(LTDC_LAYER_2, wd->msg.x + 5, (wd->msg.y + wd->msg.y_size) + 16, "MODE:", &tFont);
	}
	else if (wd->msg.y == 75 && (wd->msg.x_size == 120))
	{
		LCD_DispStr(LTDC_LAYER_2, wd->msg.x + 5, (wd->msg.y + wd->msg.y_size) + 16, "EDGE:", &tFont);
	}
	else if (wd->msg.y == 15 && (wd->msg.x_size == 80))
	{
		LCD_DispStr(LTDC_LAYER_2, wd->msg.x + 5, (wd->msg.y + wd->msg.y_size) + 16, "CH:", &tFont);
	}
}

/**
 * @description: 计算ch12菜单窗口背景
 * @param {window_t} *win
 * @return {*}
 */
void osc_ui_ch12_menu_calculate(window_t *win, widget_t *wid)
{
	win->msg.x = wid->msg.x;
	win->msg.y = wid->msg.y - 10 - 140;
	win->msg.x_size = ch12_btn[0].msg.x_size;
	win->msg.y_size = 140;
	win->msg.color = CL_CH12_BTN_BLOD;
	win->draw = osc_ui_ch12_menu_draw;
	win->dev = dev;
	win->msg.layer = LTDC_LAYER_2;
	SET_HIDE(win->msg.wflags);
	gui_win_creater(win);
}

/**
 * @description: 菜单高亮按钮通用绘制函数
 * @param {widget_t} *wid 要绘制的按钮
 * @return {*}
 */
void osc_ui_highlight_btn_draw(widget_t *wid)
{
	if (!(wid->msg.wflags & GUI_CH_STA))
	{
		wid->msg.color = CL_CH1_BTN_BLOD;
	}
	else
	{
		wid->msg.color = CL_BLUE2;
	}
	osc_ui_l2_btn_draw(wid);

	FONT_T tFont;
	tFont.FontCode = FC_ST_16;
	tFont.FrontColor = CL_GREY;
	tFont.BackColor = CL_MASK;
	tFont.Space = 0;

	LCD_DispStr(LTDC_LAYER_2, wid->msg.x + 13, wid->msg.y + wid->msg.y_size / 2 + 5, wid->msg.pri_data, &tFont);
}

/**
 * @description: 计算ch12的菜单内部按钮
 * @param {widget_t} wid
 * @param {widget_t} wid_btn
 * @return {*}
 */
void osc_ui_ch12_btn_calculate(widget_t wid[], widget_t wid_btn[], window_t *par)
{
	for (uint16_t i = 0; i < 3; i++)
	{
		wid[i].parent = par;
		wid[i].msg.x = 0 + 5;
		wid[i].msg.y = (0 + 5) + i * 45;
		wid[i].msg.x_size = 90;
		wid[i].msg.y_size = 40;
		wid[i].msg.color = CL_CH1_BTN_BLOD;
		wid[i].draw = osc_ui_l2_btn_draw;
		wid[i].msg.wflags |= GUI_BTN_BOLD;
		wid[i].peer_linker = 0;
		wid[i].msg.pri_data = "ON";
		gui_wid_creater(&wid[i]);
	}

	for (uint16_t i = 0; i < 6; i++)
	{
		wid_btn[i].parent = par;
		wid_btn[i].msg.x_size = 45;
		wid_btn[i].msg.y_size = 40;
		wid_btn[i].draw = osc_ui_highlight_btn_draw;
		wid_btn[i].peer_linker = 0;
		wid_btn[i].msg.wflags |= GUI_BTN_BOLD;

		if (i < 3)
		{
			wid_btn[i].msg.color = CL_BLUE2;
			wid_btn[i].msg.x = 0 + 5;
			wid_btn[i].msg.y = (0 + 5) + i * 45;
			wid_btn[i].msg.pri_data = ch12_btn_text[i];
			wid_btn[i].msg.wflags |= GUI_CH_STA;
		}
		else if (i >= 3 && i < 6)
		{
			wid_btn[i].msg.color = CL_CH1_BTN_BLOD;
			wid_btn[i].msg.x = 0 + 50;
			wid_btn[i].msg.y = (0 + 5) + (i - 3) * 45;
			wid_btn[i].msg.pri_data = ch12_btn_text[i];
		}
		gui_wid_creater(&wid_btn[i]);
	}
}

/**
 * @description: 通道1菜单中按钮选择
 * @param {uint8_t} index 要高亮的按钮，见字符数组顺序
 * @return {*}
 */
void osc_ui_ch1_btn_sel(uint8_t index)
{
	ch1_btn[index].msg.wflags |= GUI_CH_STA;
	ch1_btn[(index >= 3) ? (index - 3) : (index + 3)].msg.wflags &= ~GUI_CH_STA;
	CLEAR_DRAWED(ch1_btn[index].msg.wflags);
	CLEAR_DRAWED(ch1_btn[(index >= 3) ? (index - 3) : (index + 3)].msg.wflags);
	gui_create_event();
}

/**
 * @description: 通道2菜单中按钮选择
 * @param {uint8_t} index 要高亮的按钮
 * @return {*}
 */
void osc_ui_ch2_btn_sel(uint8_t index)
{
	ch2_btn[index].msg.wflags |= GUI_CH_STA;
	ch2_btn[(index >= 3) ? (index - 3) : (index + 3)].msg.wflags &= ~GUI_CH_STA;
	CLEAR_DRAWED(ch2_btn[index].msg.wflags);
	CLEAR_DRAWED(ch2_btn[(index >= 3) ? (index - 3) : (index + 3)].msg.wflags);
	gui_create_event();
}

/**
 * @description: 菜单按钮展开选择
 * @param {uint8_t} index 要展开的菜单
 * @return {*}
 */
void osc_ui_btn_ctl_sel(uint8_t index)
{
	if (index == 0)
	{
		SET_HIDE(trig_menu.msg.wflags);
		SET_HIDE(ch12_menu[1].msg.wflags);
		SET_HIDE(ch12_menu[0].msg.wflags);
		osc_dev_l2_disable();
	}
	else if (index == 1)
	{
		SET_HIDE(trig_menu.msg.wflags);
		SET_HIDE(ch12_menu[1].msg.wflags);
		gui_show_win(&ch12_menu[0]);
	}
	else if (index == 2)
	{
		SET_HIDE(trig_menu.msg.wflags);
		SET_HIDE(ch12_menu[0].msg.wflags);
		gui_show_win(&ch12_menu[1]);
	}
	else if (index == 3)
	{
		SET_HIDE(ch12_menu[0].msg.wflags);
		SET_HIDE(ch12_menu[1].msg.wflags);
		gui_show_win(&trig_menu);
	}
}

/**
 * @description: 触发按钮组件计算
 * @param {widget_t} *wid 触发按钮组件
 * @return {*}
 */
void osc_ui_trig_btn_calculate(widget_t *wid)
{
	wid->msg.x = ch12_btn[1].msg.x + ch12_btn[1].msg.x_size + 150;
	wid->msg.y = ch12_btn[1].msg.y;
	wid->msg.x_size = 130;
	wid->msg.y_size = 35;
	wid->msg.color = CL_CH12_BTN_BLOD;
	wid->msg.wflags |= GUI_BTN_BOLD;
	wid->draw = osc_ui_l1_btn_draw;
	wid->peer_linker = 0;
	wid->parent = &win_main;
	wid->msg.pri_data = "TRIG";
	gui_wid_creater(wid);
}

/**
 * @description: 触发菜单窗口计算
 * @param {window_t} *win 触发菜单窗口
 * @param {widget_t} *wid 触发按钮组件
 * @return {*}
 */
void osc_ui_trig_menu_calculate(window_t *win, widget_t *wid)
{
	win->msg.x = wid->msg.x;
	win->msg.y = wid->msg.y - 10 - 200;
	win->msg.x_size = wid->msg.x_size; // 130
	win->msg.y_size = 200;
	win->msg.color = CL_CH12_BTN_BLOD;
	win->draw = osc_ui_ch12_menu_draw;
	win->dev = dev;
	SET_HIDE(win->msg.wflags);
	gui_win_creater(win);
}

/**
 * @description: 触发菜单中按钮控件计算
 * @param {widget_t} wid 背景按钮
 * @param {widget_t} wid_btn 高亮按钮
 * @param {window_t} *par 父窗口
 * @return {*}
 */
void osc_ui_trig_menu_btn_calculate(widget_t wid[], widget_t wid_btn[], window_t *par)
{
	for (uint16_t i = 0; i < 3; i++)
	{
		wid[i].parent = par;
		wid[i].msg.x = 0 + 5;
		wid[i].msg.y = par->msg.y_size - 5 - (i + 1) * 60; // 135 75 15
		wid[i].msg.x_size = 120;
		if (i == 2)
		{
			wid[i].msg.x_size = 80;
		}
		wid[i].msg.y_size = 40;
		wid[i].msg.color = CL_CH1_BTN_BLOD;
		wid[i].draw = osc_ui_l2_btn_draw;
		wid[i].msg.wflags |= GUI_BTN_BOLD;
		wid[i].peer_linker = 0;
		gui_wid_creater(&wid[i]);
	}

	for (uint16_t i = 0; i < 8; i++)
	{
		wid_btn[i].parent = par;
		wid_btn[i].msg.x_size = 40;
		wid_btn[i].msg.y_size = 40;
		wid_btn[i].draw = osc_ui_highlight_btn_draw;
		wid_btn[i].peer_linker = 0;
		wid_btn[i].msg.wflags |= GUI_BTN_BOLD;

		if (i < 3)
		{
			wid_btn[i].msg.color = CL_BLUE2;
			wid_btn[i].msg.x = 0 + 5;
			wid_btn[i].msg.y = par->msg.y_size - 5 - (i + 1) * 60;
			wid_btn[i].msg.pri_data = trig_btn_text[i];
			wid_btn[i].msg.wflags |= GUI_CH_STA;
		}
		else if ((i >= 3) && (i < 6))
		{
			wid_btn[i].msg.color = CL_CH1_BTN_BLOD;
			wid_btn[i].msg.x = 0 + 45;
			wid_btn[i].msg.y = par->msg.y_size - 5 - (i + 1 - 3) * 60;
			wid_btn[i].msg.pri_data = trig_btn_text[i];
		}
		else if ((i >= 6) && (i < 8))
		{
			wid_btn[i].msg.color = CL_CH1_BTN_BLOD;
			wid_btn[i].msg.x = 0 + 85;
			wid_btn[i].msg.y = par->msg.y_size - 5 - (i + 1 - 6) * 60;
			wid_btn[i].msg.pri_data = trig_btn_text[i];
		}
		gui_wid_creater(&wid_btn[i]);
	}
}

/**
 * @description: 触发菜单按钮选择
 * @param {uint8_t} index 要选中高亮的按钮序号
 * @return {*}
 */
void osc_ui_trig_btn_sel(uint8_t index)
{
	if (index == 0 || index == 3 || index == 6)
	{
		for (uint8_t i = 0; i <= 6; i += 3)
		{
			trig_sel_btn[i].msg.wflags &= ~GUI_CH_STA;
			CLEAR_DRAWED(trig_sel_btn[i].msg.wflags);
		}
	}
	else if (index == 1 || index == 4 || index == 7)
	{
		for (uint8_t i = 1; i <= 7; i += 3)
		{
			trig_sel_btn[i].msg.wflags &= ~GUI_CH_STA;
			CLEAR_DRAWED(trig_sel_btn[i].msg.wflags);
		}
	}
	else if (index == 2 || index == 5)
	{
		for (uint8_t i = 2; i <= 5; i += 3)
		{
			trig_sel_btn[i].msg.wflags &= ~GUI_CH_STA;
			CLEAR_DRAWED(trig_sel_btn[i].msg.wflags);
		}
	}
	trig_sel_btn[index].msg.wflags |= GUI_CH_STA;
	gui_create_event();
}

void osc_ui_mea_btn_calculate(widget_t wid[])
{
	for (uint16_t i = 0; i < 4; i++)
	{
		wid[i].msg.x = 20 + 175 * i;
		wid[i].msg.y = 5;
		wid[i].msg.x_size = 150;
		wid[i].msg.y_size = 30;
		wid[i].msg.color = CL_CH12_BTN_BLOD;
		wid[i].msg.wflags |= GUI_BTN_BOLD;
		wid[i].draw = osc_ui_l1_btn_draw;
		wid[i].peer_linker = 0;
		wid[i].parent = &win_main;
		wid[i].msg.pri_data = "CH12";
		wid[i].msg.layer = LTDC_LAYER_1;
		gui_wid_creater(&wid[i]);
	}
}
