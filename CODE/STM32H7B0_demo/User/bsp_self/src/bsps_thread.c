/*
 * @Author: SUN  BI4NEG@gmail.com
 * @Date: 2023-11-09 18:12:36
 * @LastEditTime: 2023-12-10 17:42:00
 * @Description: «ÎÃÓ–¥ºÚΩÈ
 */
#include "bsp.h"

extern uint16_t disp_data[2][700];
uint16_t disp_x_data[700];
static run_msg_t run_msg;

int bsps_thread_init(void);

OS_INIT_REGISTER("thread_init", bsps_thread_init, 0, 2);
OS_TSK_REGISTER(bsps_thread, PRIORITY_4, 50);

void bsps_thread(void)
{
    if (OSC_DMA_CHECK_TC())
    {
        OSC_DMA_CLEAR_TC();
        bsp_lcd_fill_rect(LTDC_LAYER_1,20,40,400,700,CL_BLACK);
        bsps_ui_main_win_draw();
        bsps_sa_trig_read();
        bsp_lcd_draw_lines(disp_x_data, disp_data[1], 700, CL_YELLOW);
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
    run_msg.menu_depth = 0;
    run_msg.ratio = RATIO_10X;
    run_msg.acdc = AC_COUPLE;
    bsps_sa_oh_ac_ch2();
    run_msg.timebase = 5;
    bsps_time_psc_set(5);
    run_msg.vol_scale[0] = 5;
    run_msg.vol_scale[1] = 5;
    bsps_vol_scale_set_ch2(5);

    return OS_OK;
}

run_msg_t * bsps_get_run_msg(void)
{
    return &run_msg;
}
