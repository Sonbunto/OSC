#ifndef __BSPS_LCD_DRAW_H
#define __BSPS_LCD_DRAW_H

extern uint16_t g_menu_depth;

void bsps_ui_main_win_draw(void);
//void bsps_ui_btn_draw(void);
void bsps_ui_trig_icon_draw(uint16_t pos);
int bsps_ui_wave_init(void);
void bsps_ui_spec_init(void);
void bsps_ui_trig_line_draw(uint16_t pos,uint8_t is_dis);
void bsps_ui_ch12_acdc_draw(uint8_t is_ac,uint8_t ch);
void bsps_ui_timebase_draw(char * str);
void bsps_ui_trig_vol_draw(float vol);
void bsps_ui_ch12_vol_gain_draw(char *str,uint8_t ch);
void bsps_ui_base_fre_draw(void);
void bsps_ui_sa_rate_draw(uint16_t rate);

static void bsps_ui_is_trig(void);

void osc_ui_move_trig_lines(uint8_t chn,uint64_t posy);
void osc_ui_trig_lines_show(unsigned char chn,unsigned char mode);
void osc_ui_ch1_btn_sel(uint8_t index);
void osc_ui_ch12_ctl_sel(uint8_t index);
void bsps_ui_ch12_ratio_draw(uint8_t is_x10,uint8_t ch);
void osc_ui_trig_btn_sel(uint8_t index);

#endif
