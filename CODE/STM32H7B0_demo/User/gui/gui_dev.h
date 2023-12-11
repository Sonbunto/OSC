#ifndef __GUI_DEV_H
#define __GUI_DEV_H

typedef struct
{
	void (*set_noload_point)( uint16_t x , uint16_t y , uint16_t color ); 
	uint16_t (*read_point)( uint16_t , uint16_t );
#if HARDWARE_ACCEL_SUPPLY	
	/* for hardware */
	void (*fill_rect)(uint16_t,uint16_t ,uint16_t ,uint16_t ,unsigned int);
	void (*fill_color)(uint16_t ,uint16_t,uint16_t,uint16_t,unsigned char * );
	void (*clear_display_dev)(uint16_t );
#endif	
	unsigned int width;
	unsigned int height;
	unsigned int display_type; /* 0 is LCD , 1 is VGA */
}gui_dev_t;

gui_dev_t * get_gui_dev(void);
int gui_dev_init(void);

#endif
