#include "bsp.h"

static gui_dev_t gui_dev_s;

static unsigned char inited_flag = 0;

int gui_dev_init(void)
{
#ifndef _VC_SIMULATOR_
	gui_dev_s.width = 800;
	gui_dev_s.height = 480;
	gui_dev_s.read_point = bsp_lcd_get_pixel;	
	gui_dev_s.set_noload_point = bsp_lcd_set_pixel;
#else

	gui_dev_s.width = 800;//dev_info->display_dev->pwidth;
	gui_dev_s.height = 480;//dev_info->display_dev->pheight;

	gui_dev_s.set_point = set_vc_point;
	gui_dev_s.read_point = 0;
#endif
#if HARDWARE_ACCEL_SUPPLY		
	gui_dev_s.fill_rect = fill_rect;
	gui_dev_s.fill_color = fill_color;
	gui_dev_s.clear_display_dev = clear_display_dev;
#endif	

	inited_flag = 1;

	return 0;
}

gui_dev_t * get_gui_dev(void)
{

	if( inited_flag == 0 )
	{
		gui_dev_init();
	}

	return &gui_dev_s;
}








