/**
 * @FilePath     : /MDK-ARMc:/Users/19439/Desktop/STM32H7B0_demo/User/main.c
 * @Description  :
 * @Author       : SUN WENTAO BI4NEG@gmail.com
 * @Version      : 0.0.1
 * @LastEditTime : 2023-11-09 20:13:53
 **/

#include "bsp.h"

int main(void)
{

	bsp_Init();
	
	while (1)
	{

		EventStartA(0);
		//		bsps_gt911_scanf();

		//		HAL_Delay(30);
		//		bsps_sample_test();
		EventStopA(0);

		EventStartA(1);
		EventStopA(1);

		run_thead_priority_idle();
		
	}
}
