/*
 * @Author: SUN  BI4NEG@gmail.com
 * @Date: 2023-11-09 18:12:25
 * @LastEditTime: 2023-12-25 13:58:56
 * @Description: «ÎÃÓ–¥ºÚΩÈ
 */
#ifndef __BSPS_THREAD_H
#define __BSPS_THREAD_H

#define NO_TRIG 0
#define TRIG 1
#define NO_PAUSE 0
#define PAUSE 1
#define MODE_WAVE 0
#define MODE_SPEC 1
#define CH_OFF 0
#define CH_ON 1
#define AC_COUPLE 1
#define DC_COUPLE 0
#define RATIO_1X 0
#define RATIO_10X 1
#define TRIG_AUTO 0
#define TRIG_NORMAL 1
#define TRIG_SIGLE 2
#define TRIG_EDGE_RISE 0
#define TRIG_EDGE_FALL 1
#define TRIG_EDGE_RISE_FALL 2
#define TRIG_CH1 0
#define TRIG_CH2 1

typedef struct
{
    uint16_t trig_lev[2];
    int offset_lev[2];
    uint8_t vol_scale[2];
    uint8_t timebase;
    uint8_t ch1_sta;
    uint8_t ch2_sta;
    uint8_t is_trig;
    uint8_t is_pause;
    uint8_t trig_mode;
    uint8_t trig_edge;
    uint8_t trig_src;
    uint8_t run_mode;
    uint8_t acdc;
    uint8_t ratio;
    uint8_t menu_depth;
} run_msg_t;

void bsps_thread(void);
run_msg_t *bsps_get_run_msg(void);

#endif
