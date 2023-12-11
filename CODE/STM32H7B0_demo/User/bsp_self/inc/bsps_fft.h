#ifndef __BSPS_FFT_H
#define __BSPS_FFT_H
#include "arm_const_structs.h"

extern float32_t testOutputMag_f32[4096];

void bsps_fft_cal(float *testInput_f32);

#endif
