/*
 * @Author: SUN  BI4NEG@gmail.com
 * @Date: 2023-11-09 18:12:36
 * @LastEditTime: 2023-12-26 15:04:45
 * @Description: ÇëÌîÐ´¼ò½é
 */
#include "bsp.h"

extern uint16_t disp_data[2][700];
uint16_t disp_x_data[700];
static run_msg_t run_msg;
__IO uint8_t off_clear = 0;

int bsps_thread_init(void);

OS_INIT_REGISTER("thread_init", bsps_thread_init, 0, 2);
OS_TSK_REGISTER(bsps_thread, PRIORITY_4, 50);

void bsps_thread(void)
{
    run_msg_t *run_msg = bsps_get_run_msg();

    if (OSC_DMA_CHECK_TC())
    {
        OSC_DMA_CLEAR_TC();
        if (run_msg->ch1_sta == CH_ON && run_msg->ch2_sta == CH_ON)
        {
            bsp_lcd_fill_rect(LTDC_LAYER_1, 20, 40, 400, 700, CL_BLACK);
            bsps_ui_main_win_draw();
            bsps_sa_trig_read();
            bsp_lcd_draw_lines(disp_x_data, disp_data[1], 700, CL_YELLOW);
            bsp_lcd_draw_lines(disp_x_data, disp_data[0], 700, CL_CH2);
            off_clear = 0;
        }
        else if(run_msg->ch1_sta == CH_ON)
        {
            bsp_lcd_fill_rect(LTDC_LAYER_1, 20, 40, 400, 700, CL_BLACK);
            bsps_ui_main_win_draw();
            bsps_sa_trig_read();
            bsp_lcd_draw_lines(disp_x_data, disp_data[1], 700, CL_YELLOW);
            off_clear = 0;
        }
        else if(run_msg->ch2_sta == CH_ON)
        {
            bsp_lcd_fill_rect(LTDC_LAYER_1, 20, 40, 400, 700, CL_BLACK);
            bsps_ui_main_win_draw();
            bsps_sa_trig_read();
            bsp_lcd_draw_lines(disp_x_data, disp_data[0], 700, CL_CH2);
            off_clear = 0;
        }
        if(run_msg->ch1_sta == CH_OFF && run_msg->ch2_sta == CH_OFF && off_clear == 0)
        {
            bsp_lcd_fill_rect(LTDC_LAYER_1, 20, 40, 400, 700, CL_BLACK);
            bsps_ui_main_win_draw();
            off_clear = 1;
        }
    }
}

static void bsps_xdata_init(void)
{
    for (uint16_t i = 20; i < 720; i++)
    {
        disp_x_data[i - 20] = i;
    }
}

int bsps_thread_init(void)
{
    bsps_xdata_init();

    run_msg.is_trig = NO_TRIG;
    run_msg.is_pause = NO_PAUSE;
    run_msg.run_mode = MODE_WAVE;
    run_msg.trig_mode = TRIG_AUTO;
    run_msg.trig_edge = TRIG_EDGE_RISE;
    run_msg.trig_src = TRIG_CH1;
    run_msg.menu_depth = 0;
    run_msg.ratio = RATIO_10X;
    run_msg.acdc = AC_COUPLE;
    bsps_sa_oh_ac_ch2();
    run_msg.timebase = 5;
    bsps_time_psc_set(5);
    run_msg.vol_scale[0] = 5;
    run_msg.vol_scale[1] = 5;
    run_msg.offset_lev[0] = 0;
    run_msg.offset_lev[1] = 0;
    bsps_vol_scale_set_ch2(5);
    run_msg.ch1_sta = CH_ON;
    run_msg.ch2_sta = CH_ON;

    return OS_OK;
}

run_msg_t *bsps_get_run_msg(void)
{
    return &run_msg;
}
