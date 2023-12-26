#ifndef __BSP_LTDC_H__
#define __BSP_LTDC_H__

#define Width 800
#define Height 480
#define	HSYNC_W 4
#define HBP 8
#define HFP 8
#define VSYNC_W 4
#define VBP 8
#define VFP 8

#define LCD_LAYER_1     0x0000		/* ���� */
#define LCD_LAYER_2		0x0001		/* ��2�� */


#define RGB(R,G,B)	(((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3))	/* ��8λR,G,Bת��Ϊ 16λRGB565��ʽ */
enum
{
	CL_R_BTN		= RGB( 98, 98, 98),
	CL_R_BTN_BLOD   = RGB(165,178,176),
	CL_CH12_BTN_BLOD= RGB(35,35,35),
	CL_CH1_BTN_BLOD= RGB(50,50,50),


	CL_WHITE        = RGB(255,255,255),	/* ��ɫ */
	CL_BLACK        = RGB(  0,  0,  0),	/* ��ɫ */
	CL_RED          = RGB(255,	0,  0),	/* ��ɫ */
	CL_GREEN        = RGB(  0,255,  0),	/* ��ɫ */
	CL_BLUE         = RGB(  0,	0,255),	/* ��ɫ */
	CL_YELLOW       = RGB(255,255,  0),	/* ��ɫ */
	CL_ORIG       	= RGB(255,97,  0),	/* ��ɫ */

	CL_GREY			= RGB( 98, 98, 98), 	/* ���ɫ */
	CL_GREY1		= RGB( 150, 150, 150), 	/* ǳ��ɫ */
	CL_GREY2		= RGB( 180, 180, 180), 	/* ǳ��ɫ */
	CL_GREY3		= RGB( 200, 200, 200), 	/* ��ǳ��ɫ */
	CL_GREY4		= RGB( 230, 230, 230), 	/* ��ǳ��ɫ */

	CL_BUTTON_GREY	= RGB( 220, 220, 220), /* WINDOWS ��ť�����ɫ */

	CL_MAGENTA      = 0xF81F,	/* ����ɫ�����ɫ */
	CL_CYAN         = 0x7FFF,	/* ����ɫ����ɫ */

	CL_BLUE1        = RGB(  0,  0, 240),		/* ����ɫ */
	CL_BLUE2        = RGB(  0,  0, 128),		/* ����ɫ */
	CL_BLUE3        = RGB(  68, 68, 255),		/* ǳ��ɫ1 */
	CL_BLUE4        = RGB(  0, 64, 128),		/* ǳ��ɫ1 */

	CL_BTN_FACE		= RGB(236, 233, 216),	/* ��ť������ɫ(��) */
	
	CL_BTN_FONT		= CL_BLACK,				/* ��ť������ɫ���ڣ� */
	
	CL_BOX_BORDER1	= RGB(172, 168,153),	/* �����������ɫ */
	CL_BOX_BORDER2	= RGB(255, 255,255),	/* �������Ӱ����ɫ */

	CL_CH1       	= RGB(255, 255,  0),			/* CH1����ɫ */
	CL_CH2        	= RGB(  0, 251, 255),		/* CH2����ɫ */


	CL_MASK			= 0x9999	/* ��ɫ���룬�������ֱ���͸�� */
};

/* ������� */
typedef enum
{
	FC_ST_12 = 0,		/* ����12x12���� ����x�ߣ� */
	FC_ST_16,			/* ����15x16���� ����x�ߣ� */
	FC_ST_24,			/* ����24x24���� ����x�ߣ� -- ��ʱδ֧�� */
	FC_ST_32,			/* ����32x32���� ����x�ߣ� -- ��ʱδ֧�� */	
	
	FC_RA8875_16,		/* RA8875 �������� 16���� */
	FC_RA8875_24,		/* RA8875 �������� 24���� */
	FC_RA8875_32		/* RA8875 �������� 32���� */	
}FONT_CODE_E;

/* ���ֶ��뷽ʽ */
enum
{
	ALIGN_LEFT = 0,
	ALIGN_CENTER = 1,
	ALIGN_RIGHT = 2
};

/* �������Խṹ, ����LCD_DispStr() */
typedef struct
{
	FONT_CODE_E FontCode;	/* ������� FONT_CODE_E  */
	uint16_t FrontColor;/* ������ɫ */
	uint16_t BackColor;	/* ���ֱ�����ɫ��͸�� */
	uint16_t Space;		/* ���ּ�࣬��λ = ���� */
}FONT_T;



static int bsp_lcd_init(void);
void bsp_lcd_set_pixel(uint16_t x,uint16_t y,uint16_t color);
void bsp_lcd_set_pixel_l2(uint16_t pheight, uint16_t pwidth, uint16_t x, uint16_t y, uint16_t color);
uint16_t bsp_lcd_get_pixel(uint16_t x, uint16_t y);
uint16_t bsp_lcd_get_pixel_l2(uint16_t pheight, uint16_t pwidth, uint16_t x, uint16_t y);
void bsp_lcd_clr_scr(uint16_t _usColor);
void bsp_lcd_quit_win(void);
static int bsp_lcd_bcl_on(void);
static int bsp_lcd_bcl_off(void);
void bsp_lcd_color_test(void);
void bsp_lcd_set_tran(uint8_t tran);
void bsp_lcd_draw_circle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor);
void bsp_lcd_enter_win(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth);
void bsp_lcd_draw_line_h(uint16_t _usX, uint16_t _usY, uint16_t _usLen , uint16_t _usColor);
void bsp_lcd_draw_line_v(uint16_t _usX , uint16_t _usY , uint16_t _usLen , uint16_t _usColor);
void bsp_lcd_draw_lines(uint16_t *x, uint16_t *y, uint16_t _usSize, uint16_t _usColor);
void bsp_lcd_draw_line(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor);
void bsp_lcd_fill_rect(uint32_t layer,uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void bsp_lcd_draw_rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor);
void LCD_DispStr(uint32_t layer,uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont);
void bsp_lcd_draw_tri(uint16_t _usX, uint16_t _usY, uint16_t _usLen, uint16_t _usColor);
void osc_dev_l2_enable(unsigned short x0,unsigned short y0,unsigned short x0_size,unsigned short y0_size,unsigned char alpha);
void osc_dev_l2_disable(void);

#endif

