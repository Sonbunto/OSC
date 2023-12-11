#ifndef __BSPS_AD9288_H
#define __BSPS_AD9288_H

void osc_curve_test(void);
char OSC_DMA_CHECK_TC(void);
void OSC_DMA_CLEAR_TC(void);
void bsps_sa_set_psc(unsigned int psc);



void bsps_sa_trig_exti_set(uint8_t flag);

void bsps_sa_trig_read(void);
void bsps_sa_trig_edge_set(uint8_t mode);
void bsps_sa_exti_callbcak_trig(void);



#endif
