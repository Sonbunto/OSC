#ifndef __BSPS_MEA_H
#define __BSPS_MEA_H

//void bsps_mea_vpp(void);
static void bsps_mea_vpp(void);
void bsps_mea_vmax(void);
void bsps_mea_vmin(void);
void bsps_mea_fre(void);
void bsps_mea_find_max(void);

extern float g_max_fre_amp;
extern uint16_t g_base_fre;

#endif

