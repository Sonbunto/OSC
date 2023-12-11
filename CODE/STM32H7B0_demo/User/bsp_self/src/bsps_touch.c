/*
 * @Author: SUN  BI4NEG@gmail.com
 * @Date: 2023-10-03 16:37:46
 * @LastEditTime: 2023-12-10 19:33:39
 * @Description: 请填写简介
 */
/*
 * @Author: SUN  BI4NEG@gmail.com
 * @Date: 2023-10-03 16:37:46
 * @LastEditTime: 2023-12-09 20:46:57
 * @Description: 触摸屏
 */
#include "bsp.h"

touch_data_t User_Touch;
int timebase_cnt = 0;
int gain_cnt = 3;
int spec_sa_cnt = 0;
extern uint16_t trig_lines_hold_time_s;
extern widget_t right_btn[6];
extern widget_t ch12_btn[2];
extern widget_t ch1_bck_btn[3];
extern widget_t ch2_bck_btn[6];

uint8_t _temp = 0; // 中间变量
__IO uint8_t touch_flag = 0;
__IO uint16_t last_x_pos = 0;
__IO uint16_t osc_mode = 0;
__IO uint16_t g_sa_rate_wave = 1000; // 时域采样率 单位Ksps
__IO uint16_t g_sa_rate_spec = 1000; // 频域采样率 单位Ksps
__IO uint8_t ch1_ctl_btn, ch2_ctl_btn = 0;
__IO uint8_t ch1_en, ch2_en, ch1_ratio, ch2_ratio, ch1_coup, ch2_coup = 0;
uint8_t tmp_cnt = 0;

void bsps_touch_task(void);

uint8_t acdc_flag = 0;

OS_INIT_REGISTER("touch_init", bsps_gt911_init, 0, 3);

OS_TSK_REGISTER(bsps_gt911_scanf, PRIORITY_2, 50);

void bsps_gt911_w_reg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen)
{
	HAL_I2C_Mem_Write(&hi2c1, GT911_DIV_W, _usRegAddr, I2C_MEMADD_SIZE_16BIT, _pRegBuf, _ucLen, 0xff);
}

void bsps_gt911_r_reg(uint16_t _usRegAddr, uint8_t *_pRegBuf, uint8_t _ucLen)
{
	HAL_I2C_Mem_Read(&hi2c1, GT911_DIV_R, _usRegAddr, I2C_MEMADD_SIZE_16BIT, _pRegBuf, _ucLen, 0xff);
}

void bsps_soft_reset(void)
{
	uint8_t _temp = 2; // 中间变量
	// 往gt911中寄存器0x8040中写入2,使之复位
	bsps_gt911_w_reg(GT_CTRL_REG, &_temp, 1);
}

void bsps_soft_reset_stop(void)
{
	uint8_t _temp = 0; // 中间变量
	// 往gt911中寄存器0x8040中写入0,使之结束复位
	bsps_gt911_w_reg(GT_CTRL_REG, &_temp, 1);
}

void bsps_bsps_hard_reset(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 0);
}

void bsps_bsps_hard_reset_stop(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, 1);
}

// GT911初始化（硬复位）
static int bsps_gt911_init(void)
{
	bsps_iic_init();

	bsps_bsps_hard_reset();
	HAL_Delay(10);
	bsps_bsps_hard_reset_stop();
	HAL_Delay(10);

	return OS_OK;
}

void bsps_touch_spec_sa_rae(uint8_t dir)
{
	if (dir)
	{
		spec_sa_cnt++;
	}
	else
	{
		spec_sa_cnt--;
	}

	if (spec_sa_cnt > 9)
	{
		spec_sa_cnt = 9;
	}
	else if (spec_sa_cnt < 0)
	{
		spec_sa_cnt = 0;
	}

	switch (spec_sa_cnt)
	{
	case 0:
		TIM6->ARR = 10 - 1;
		g_sa_rate_spec = 1000;
		bsps_ui_sa_rate_draw(g_sa_rate_spec);
		break;
	case 1:
		TIM6->ARR = 20 - 1;
		g_sa_rate_spec = 500;
		bsps_ui_sa_rate_draw(g_sa_rate_spec);
		break;
	case 3:
		TIM6->ARR = 50 - 1;
		g_sa_rate_spec = 200;
		bsps_ui_sa_rate_draw(g_sa_rate_spec);
		break;
	case 4:
		TIM6->ARR = 100 - 1;
		g_sa_rate_spec = 100;
		bsps_ui_sa_rate_draw(g_sa_rate_spec);
		break;
	case 5:
		TIM6->ARR = 200 - 1;
		g_sa_rate_spec = 50;
		bsps_ui_sa_rate_draw(g_sa_rate_spec);
		break;
	case 6:
		TIM6->ARR = 500 - 1;
		g_sa_rate_spec = 20;
		bsps_ui_sa_rate_draw(g_sa_rate_spec);
		break;
	case 7:
		TIM6->ARR = 1000 - 1;
		g_sa_rate_spec = 10;
		bsps_ui_sa_rate_draw(g_sa_rate_spec);
		break;
	case 8:
		TIM6->ARR = 2000 - 1;
		g_sa_rate_spec = 5;
		bsps_ui_sa_rate_draw(g_sa_rate_spec);
		break;
	case 9:
		TIM6->ARR = 5000 - 1;
		g_sa_rate_spec = 1;
		bsps_ui_sa_rate_draw(g_sa_rate_spec);
		break;
	}
}

// 触摸触发线任务
void bsps_touch_trig_task(void)
{
	run_msg_t *run_msg = bsps_get_run_msg();

	g_trig_bias = 480 - User_Touch.Touch_XY[0].Y_Point;

	if (g_trig_bias > 440)
	{
		g_trig_bias = 241;
	}
	else if (g_trig_bias < 40)
	{
		g_trig_bias = 241;
	}
	bsps_ui_trig_icon_draw(g_trig_bias);
	osc_ui_trig_lines_show(0, 1);
	osc_ui_move_trig_lines(0, g_trig_bias);
	trig_lines_hold_time_s = 3;
	// bsps_ui_trig_line_draw(g_trig_bias,1);
	if (run_msg->ratio == RATIO_10X)
	{
		bsps_ui_trig_vol_draw((float)((g_trig_bias - 240) * osc_vol_scale_table[run_msg->vol_scale[1]].mv_int) / 50.0 / 1000.0 * 10.0);
	}
	else if (run_msg->ratio == RATIO_1X)
	{
		bsps_ui_trig_vol_draw((float)((g_trig_bias - 240) * osc_vol_scale_table[run_msg->vol_scale[1]].mv_int) / 50.0 / 1000.0);
	}

	bsps_trig_lev_set(g_trig_bias - 40);
}

// 触摸电压偏移任务
void bsps_touch_bias_task(void)
{
	g_vol_bias = 307 - User_Touch.Touch_XY[0].Y_Point;
	if (g_vol_bias >= 200)
	{
		g_vol_bias = 200;
	}
	else if (g_vol_bias <= -200)
	{
		g_vol_bias = -200;
	}
}

// 模式切换 0为波形显示 1为频谱显示
void bsps_touch_osc_mode(void)
{
	osc_mode = !osc_mode;
	if (osc_mode)
	{
		bsps_ui_spec_init();
	}
	else
	{
		bsps_ui_wave_init();
	}
}

// 暂停触摸事件
void bsps_touch_pause(void)
{
	if (g_is_pause)
	{
		g_is_pause = 0;
	}
	else
	{
		g_flag_trig_en = 0;
		//		bsps_ui_is_trig_draw();
		g_is_pause = 1;
	}
}

// 重新运行按钮任务
void bsps_touch_restore(void)
{
	g_vol_bias = 0;
	g_trig_bias = 307;
	g_vol_gain = 1.0;
	bsps_ui_trig_vol_draw((g_trig_bias - 307) / 120.0);
	bsps_ui_trig_icon_draw(g_trig_bias);

	TIM6->ARR = 10 - 1;
	bsps_ui_timebase_draw(50);
}

// 探头衰减设置按钮任务
void bsps_touch_ratio_task(void)
{
	run_msg_t *run_msg = bsps_get_run_msg();

	if (run_msg->ratio == RATIO_10X)
	{
		run_msg->ratio = RATIO_1X;
	}
	else if (run_msg->ratio == RATIO_1X)
	{
		run_msg->ratio = RATIO_10X;
	}

	if (run_msg->ratio == RATIO_10X)
	{
		bsps_ui_ch12_vol_gain_draw(osc_vol_scale_table[run_msg->vol_scale[1]].strX10, 0);
		bsps_ui_trig_vol_draw((float)((g_trig_bias - 307) * osc_vol_scale_table[run_msg->vol_scale[1]].mv_int) / 50.0 / 1000.0 * 10.0);
	}
	else
	{
		bsps_ui_ch12_vol_gain_draw(osc_vol_scale_table[run_msg->vol_scale[1]].str, 0);
		bsps_ui_trig_vol_draw((float)((g_trig_bias - 307) * osc_vol_scale_table[run_msg->vol_scale[1]].mv_int) / 50.0 / 1000.0);
	}
}

static void bsps_touch_trig_edge_task(void)
{
	run_msg_t *run_msg = bsps_get_run_msg();

	if (run_msg->trig_edge == EDGE_RISE)
	{
		run_msg->trig_edge = EDGE_FALL;
	}
	else if (run_msg->trig_edge == EDGE_FALL)
	{
		run_msg->trig_edge = EDGE_RISE_FALL;
	}
	else if (run_msg->trig_edge == EDGE_RISE_FALL)
	{
		run_msg->trig_edge = EDGE_RISE;
	}

	bsps_sa_trig_edge_set(run_msg->trig_edge);
}

// 设置菜单深度总任务
void bsps_touch_menu_enter(uint8_t depth)
{
	if (depth == 255)
	{
		return;
	}
	run_msg_t *run_msg = bsps_get_run_msg();

	run_msg->menu_depth = depth;
}

/**
 * @description: ch1控制按钮切换任务
 * @return {*}
 */
void bsps_touch_ch1_ctl_task(void)
{
	ch1_ctl_btn = !ch1_ctl_btn;
	if (ch1_ctl_btn)
	{
		osc_ui_ch12_ctl_sel(1);
	}
	else
	{
		osc_ui_ch12_ctl_sel(0);
	}
}

/**
 * @description: ch2控制按钮切换任务
 * @return {*}
 */
void bsps_touch_ch2_ctl_task(void)
{
	ch2_ctl_btn = !ch2_ctl_btn;
	if (ch2_ctl_btn)
	{
		osc_ui_ch12_ctl_sel(2);
	}
	else
	{
		osc_ui_ch12_ctl_sel(0);
	}
}

/**
 * @description: ch1开关
 * @return {*}
 */
void bsps_touch_ch1_en_task(void)
{
	ch1_en = !ch1_en;
	if (ch1_en)
	{
		osc_ui_ch1_btn_sel(5);
	}
	else
	{
		osc_ui_ch1_btn_sel(2);
	}
}

/**
 * @description: ch1探头衰减切换
 * @return {*}
 */
void bsps_touch_ch1_ratio_task(void)
{
	run_msg_t *run_msg = bsps_get_run_msg();
	ch1_ratio = !ch1_ratio;
	if (ch1_ratio == RATIO_10X)
	{
		bsps_ui_ch12_vol_gain_draw(osc_vol_scale_table[run_msg->vol_scale[1]].strX10, 0);
		bsps_ui_trig_vol_draw((float)((g_trig_bias - 307) * osc_vol_scale_table[run_msg->vol_scale[1]].mv_int) / 50.0 / 1000.0 * 10.0);
		bsps_ui_ch12_ratio_draw(RATIO_1X, 0);
		osc_ui_ch1_btn_sel(4);
	}
	else if (ch1_ratio == RATIO_1X)
	{
		bsps_ui_ch12_vol_gain_draw(osc_vol_scale_table[run_msg->vol_scale[1]].str, 0);
		bsps_ui_trig_vol_draw((float)((g_trig_bias - 307) * osc_vol_scale_table[run_msg->vol_scale[1]].mv_int) / 50.0 / 1000.0);
		bsps_ui_ch12_ratio_draw(RATIO_10X, 0);
		osc_ui_ch1_btn_sel(1);
	}
}

/**
 * @description: ch1耦合切换
 * @return {*}
 */
void bsps_touch_ch1_coup_task(void)
{
	ch1_coup = !ch1_coup;
	if (ch1_coup == DC_COUPLE)
	{
		bsps_sa_oh_dc_ch2();
		bsps_ui_ch12_acdc_draw(DC_COUPLE, 0);
		osc_ui_ch1_btn_sel(3);
	}
	else if (ch1_coup == AC_COUPLE)
	{
		bsps_sa_oh_ac_ch2();
		bsps_ui_ch12_acdc_draw(AC_COUPLE, 0);
		osc_ui_ch1_btn_sel(0);
	}
}

/**
 * @description: ch2开关
 * @return {*}
 */
void bsps_touch_ch2_en_task(void)
{
	ch2_en = !ch2_en;
	if (ch2_en)
	{
		osc_ui_ch2_btn_sel(5);
	}
	else
	{
		osc_ui_ch2_btn_sel(2);
	}
}

/**
 * @description: ch2探头衰减切换
 * @return {*}
 */
void bsps_touch_ch2_ratio_task(void)
{
	ch2_ratio = !ch2_ratio;
	if (ch2_ratio)
	{
		bsps_ui_ch12_ratio_draw(0, 1);
		osc_ui_ch2_btn_sel(4);
	}
	else
	{
		bsps_ui_ch12_ratio_draw(0, 1);
		osc_ui_ch2_btn_sel(1);
	}
}

/**
 * @description: ch2耦合切换
 * @return {*}
 */
void bsps_touch_ch2_coup_task(void)
{
	ch2_coup = !ch2_coup;
	if (ch2_coup)
	{
		bsps_ui_ch12_acdc_draw(0, 1);
		osc_ui_ch2_btn_sel(3);
	}
	else
	{
		bsps_ui_ch12_acdc_draw(1, 1);
		osc_ui_ch2_btn_sel(0);
	}
}

/**
 * @description: 退出菜单
 * @return {*}
 */
void bsps_touch_ch12_exit_menu_task(void)
{
	osc_ui_ch12_ctl_sel(0);
	ch1_ctl_btn = ch2_ctl_btn = 0;
}

void touch_test_task(void)
{
	tmp_cnt += 3;
	if(tmp_cnt > 6)
	{
		tmp_cnt = 0;
	}
	osc_ui_trig_btn_sel(tmp_cnt);
}

/**
 * @description: 触摸扫描函数,此函数每100ms执行一次
 * @return {*}
 */
void bsps_gt911_scanf(void)
{

	bsps_ui_trig_line_draw(g_trig_bias, 0);
	bsps_gt911_r_reg(GT_GSTID_REG, &_temp, 1);

	User_Touch.Touch_State = (_temp & 0x80);  // 触摸状态
	User_Touch.Touch_Number = (_temp & 0x0f); // 获取触摸点数

	switch (User_Touch.Touch_State) // 判断是否有触摸数据
	{
	case TOUCH__NO: // 没有数据

		break;
	case TOUCH_ING: // 触摸中~后，有数据，并读出数据

		for (uint8_t i = 0; i < User_Touch.Touch_Number; i++)
		// 读出触摸点数的所有数据
		{
			bsps_gt911_r_reg((GT_TPD_Sta + i * 8 + X_L), &_temp, 1); // 读出触摸x坐标的低8位
			User_Touch.Touch_XY[i].X_Point = _temp;
			bsps_gt911_r_reg((GT_TPD_Sta + i * 8 + X_H), &_temp, 1); // 读出触摸x坐标的高8位
			User_Touch.Touch_XY[i].X_Point |= (_temp << 8);

			bsps_gt911_r_reg((GT_TPD_Sta + i * 8 + Y_L), &_temp, 1); // 读出触摸y坐标的低8位
			User_Touch.Touch_XY[i].Y_Point = _temp;
			bsps_gt911_r_reg((GT_TPD_Sta + i * 8 + Y_H), &_temp, 1); // 读出触摸y坐标的高8位
			User_Touch.Touch_XY[i].Y_Point |= (_temp << 8);

			bsps_gt911_r_reg((GT_TPD_Sta + i * 8 + S_L), &_temp, 1); // 读出触摸大小数据的低8位
			User_Touch.Touch_XY[i].S_Point = _temp;
			bsps_gt911_r_reg((GT_TPD_Sta + i * 8 + S_H), &_temp, 1); // 读出触摸大小数据的高8位
			User_Touch.Touch_XY[i].S_Point |= (_temp << 8);
		}

		_temp = 0;
		bsps_gt911_w_reg(GT_GSTID_REG, &_temp, 1); // 清除数据标志位
		break;
	}

	// 下方为触摸任务
	if (User_Touch.Touch_State == 0x80)
	{
		bsps_touch_task();

		User_Touch.Touch_State = 0;
		User_Touch.Touch_Number = 0;

		// bsps_ui_btn_draw();
	}
}

/**
 * @description: 触摸任务，轮询执行
 * @return {*}
 */
void bsps_touch_task(void)
{
	run_msg_t *run_msg = bsps_get_run_msg();

	// 频谱模式
	if (run_msg->run_mode)
	{
		switch (run_msg->menu_depth)
		{
		case 0:
			break;
		case 1:
			break;
		}
	}
	// 波形模式
	else
	{
		switch (run_msg->menu_depth)
		{
		case 0:
			bsps_touch_area_task(right_btn[0].msg.x, right_btn[0].msg.y, right_btn[0].msg.x + right_btn[0].msg.x_size, right_btn[0].msg.y + right_btn[0].msg.y_size, User_Touch, touch_test_task, 255);
			bsps_touch_area_task(right_btn[1].msg.x, right_btn[1].msg.y, right_btn[1].msg.x + right_btn[1].msg.x_size, right_btn[1].msg.y + right_btn[1].msg.y_size, User_Touch, NULL, 255);
			bsps_touch_area_task(right_btn[2].msg.x, right_btn[2].msg.y, right_btn[2].msg.x + right_btn[2].msg.x_size, right_btn[2].msg.y + right_btn[2].msg.y_size, User_Touch, NULL, 255);
			bsps_touch_area_task(right_btn[3].msg.x, right_btn[3].msg.y, right_btn[3].msg.x + right_btn[3].msg.x_size, right_btn[3].msg.y + right_btn[3].msg.y_size, User_Touch, NULL, 255);
			bsps_touch_area_task(right_btn[4].msg.x, right_btn[4].msg.y, right_btn[4].msg.x + right_btn[4].msg.x_size, right_btn[4].msg.y + right_btn[4].msg.y_size, User_Touch, NULL, 255);
			bsps_touch_area_task(right_btn[5].msg.x, right_btn[5].msg.y, right_btn[5].msg.x + right_btn[5].msg.x_size, right_btn[5].msg.y + right_btn[5].msg.y_size, User_Touch, NULL, 255);
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		}
		// 波形电压偏移
		//  bsps_touch_area_task(370, 440, 720, 40, User_Touch, bsps_touch_bias_task, 255);
		// 通道控制按钮
		bsps_touch_area_task(ch12_btn[0].msg.x, ch12_btn[0].msg.y, ch12_btn[0].msg.x + ch12_btn[0].msg.x_size, ch12_btn[0].msg.y + ch12_btn[0].msg.y_size, User_Touch, bsps_touch_ch1_ctl_task, 255);
		bsps_touch_area_task(ch12_btn[1].msg.x, ch12_btn[1].msg.y, ch12_btn[1].msg.x + ch12_btn[1].msg.x_size, ch12_btn[1].msg.y + ch12_btn[1].msg.y_size, User_Touch, bsps_touch_ch2_ctl_task, 255);
		if (ch1_ctl_btn)
		{
			// 通道1菜单选择
			bsps_touch_area_task(ch1_bck_btn[0].parent->msg.x + ch1_bck_btn[0].msg.x, ch1_bck_btn[0].parent->msg.y + ch1_bck_btn[0].msg.y, ch1_bck_btn[0].parent->msg.x + ch1_bck_btn[0].msg.x + ch1_bck_btn[0].msg.x_size, ch1_bck_btn[0].parent->msg.y + ch1_bck_btn[0].msg.y + ch1_bck_btn[0].msg.y_size, User_Touch, bsps_touch_ch1_coup_task, 255);
			bsps_touch_area_task(ch1_bck_btn[1].parent->msg.x + ch1_bck_btn[1].msg.x, ch1_bck_btn[1].parent->msg.y + ch1_bck_btn[1].msg.y, ch1_bck_btn[1].parent->msg.x + ch1_bck_btn[1].msg.x + ch1_bck_btn[1].msg.x_size, ch1_bck_btn[1].parent->msg.y + ch1_bck_btn[1].msg.y + ch1_bck_btn[1].msg.y_size, User_Touch, bsps_touch_ch1_ratio_task, 255);
			bsps_touch_area_task(ch1_bck_btn[2].parent->msg.x + ch1_bck_btn[2].msg.x, ch1_bck_btn[2].parent->msg.y + ch1_bck_btn[2].msg.y, ch1_bck_btn[2].parent->msg.x + ch1_bck_btn[2].msg.x + ch1_bck_btn[2].msg.x_size, ch1_bck_btn[2].parent->msg.y + ch1_bck_btn[2].msg.y + ch1_bck_btn[2].msg.y_size, User_Touch, bsps_touch_ch1_en_task, 255);
			bsps_touch_area_task(20, 40, 170, 440, User_Touch, bsps_touch_ch12_exit_menu_task, 255);
			bsps_touch_area_task(170, 40, 370, 290, User_Touch, bsps_touch_ch12_exit_menu_task, 255);
			bsps_touch_area_task(370, 40, 720, 440, User_Touch, bsps_touch_ch12_exit_menu_task, 255);
		}
		else if (ch2_ctl_btn)
		{
			// 通道2菜单选择
			bsps_touch_area_task(ch2_bck_btn[0].parent->msg.x + ch2_bck_btn[0].msg.x, ch2_bck_btn[0].parent->msg.y + ch2_bck_btn[0].msg.y, ch2_bck_btn[0].parent->msg.x + ch2_bck_btn[0].msg.x + ch2_bck_btn[0].msg.x_size, ch2_bck_btn[0].parent->msg.y + ch2_bck_btn[0].msg.y + ch2_bck_btn[0].msg.y_size, User_Touch, bsps_touch_ch2_coup_task, 255);
			bsps_touch_area_task(ch2_bck_btn[1].parent->msg.x + ch2_bck_btn[1].msg.x, ch2_bck_btn[1].parent->msg.y + ch2_bck_btn[1].msg.y, ch2_bck_btn[1].parent->msg.x + ch2_bck_btn[1].msg.x + ch2_bck_btn[1].msg.x_size, ch2_bck_btn[1].parent->msg.y + ch2_bck_btn[1].msg.y + ch2_bck_btn[1].msg.y_size, User_Touch, bsps_touch_ch2_ratio_task, 255);
			bsps_touch_area_task(ch2_bck_btn[2].parent->msg.x + ch2_bck_btn[2].msg.x, ch2_bck_btn[2].parent->msg.y + ch2_bck_btn[2].msg.y, ch2_bck_btn[2].parent->msg.x + ch2_bck_btn[2].msg.x + ch2_bck_btn[2].msg.x_size, ch2_bck_btn[2].parent->msg.y + ch2_bck_btn[2].msg.y + ch2_bck_btn[2].msg.y_size, User_Touch, bsps_touch_ch2_en_task, 255);
			bsps_touch_area_task(20, 40, 320, 440, User_Touch, bsps_touch_ch12_exit_menu_task, 255);
			bsps_touch_area_task(320, 40, 420, 290, User_Touch, bsps_touch_ch12_exit_menu_task, 255);
			bsps_touch_area_task(420, 40, 720, 440, User_Touch, bsps_touch_ch12_exit_menu_task, 255);
		}
		else
		{
			// 触发线
			bsps_touch_area_task(20, 40, 370, 440, User_Touch, bsps_touch_trig_task, 255);
		}
	}
}

/**
 * @description: 触摸点击任务,轮询执行,向右上判定
 * @param {uint16_t} _usX1
 * @param {uint16_t} _usY1
 * @param {uint16_t} _usX2
 * @param {uint16_t} _usY2
 * @param {touch_data_t} touch
 * @param {uint8_t} depth
 * @return {*}
 */
void bsps_touch_area_task(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2, touch_data_t touch, void (*task)(void), uint8_t depth)
{
	// 防止重复执行任务
	if (last_x_pos != touch.Touch_XY[0].X_Point)
	{
		touch_flag = 1;
	}

	if ((touch.Touch_XY[0].X_Point >= _usX1) && (touch.Touch_XY[0].X_Point <= _usX2) && (480 - touch.Touch_XY[0].Y_Point <= _usY2) && (480 - touch.Touch_XY[0].Y_Point >= _usY1) && touch_flag)
	{
		if (depth == 255)
		{
			task();
		}
		else
		{
			bsps_touch_menu_enter(depth);
		}

		last_x_pos = touch.Touch_XY[0].X_Point;
		touch_flag = 0;
	}
}
