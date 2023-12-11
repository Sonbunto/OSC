#include "bsp.h"
#include "fonts.h"

#define WIDTH 800
#define HEIGHT 480

ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) uint16_t gram[WIDTH * HEIGHT]);
ALIGN_32BYTES(__attribute__((section(".RAM_D1"))) uint16_t gram2[200 * 200]);
DMA2D_HandleTypeDef hdma2d;
LTDC_HandleTypeDef hltdc;

extern __IO uint16_t x_s,y_s;

OS_INIT_REGISTER("lcd_init", bsp_lcd_init, bsp_lcd_bcl_on, 2);

void bsp_lcd_dma2d_init(void);

// ��ʼ���ڴ�����
void bsp_lcd_ram_init(void)
{
	for (int i = 0; i < 384000; i++)
	{
		gram[i] = 0;
	}
}

// ���Ƶ���
void bsp_lcd_set_pixel(uint16_t x, uint16_t y, uint16_t color)
{
	gram[(480 - y) * WIDTH + x] = color;
}

// ��ȡ����
uint16_t bsp_lcd_get_pixel(uint16_t x, uint16_t y)
{
	return gram[(480 - y) * WIDTH + x];
}

// ���Ƶ���l2
void bsp_lcd_set_pixel_l2(uint16_t pheight, uint16_t pwidth, uint16_t x, uint16_t y, uint16_t color)
{
	gram2[(pheight - y) * pwidth + x] = color;
}

// ��ȡ����l2
uint16_t bsp_lcd_get_pixel_l2(uint16_t pheight, uint16_t pwidth, uint16_t x, uint16_t y)
{
	return gram2[(100 - y) * pwidth + x];
}

// �򿪱���
static int bsp_lcd_bcl_on(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
	return OS_OK;
}

// �رձ���
static int bsp_lcd_bcl_off(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
	return OS_OK;
}

// ��Ļ��ɫ����
void bsp_lcd_color_test(void)
{
	bsp_lcd_clr_scr(CL_BLUE);
	HAL_Delay(1000);
	bsp_lcd_clr_scr(CL_RED);
	HAL_Delay(1000);
	bsp_lcd_clr_scr(CL_GREEN);
	HAL_Delay(1000);
}

// ����ͼ��͸���� ֵ�� 0x00 - 0xFF
void bsp_lcd_set_tran(uint8_t tran)
{
	HAL_LTDC_SetAlpha(&hltdc, tran, LTDC_LAYER_1);
}

// ���봰��ģʽ
void bsp_lcd_enter_win(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth)
{
	HAL_LTDC_SetWindowSize_NoReload(&hltdc, _usWidth, _usHeight, LTDC_LAYER_1); /* ���������� */
	HAL_LTDC_SetWindowPosition(&hltdc, _usX, _usY, LTDC_LAYER_1);			   /* �������� */
}

// �˳�����ģʽ
void bsp_lcd_quit_win(void)
{
	HAL_LTDC_SetWindowSize_NoReload(&hltdc, Width, Height, LTDC_LAYER_1); /* ���������� */
	HAL_LTDC_SetWindowPosition(&hltdc, 0, 0, LTDC_LAYER_1);				 /* �������� */
}

// ���ƴ�ֱ��
void bsp_lcd_draw_line_h(uint16_t _usX, uint16_t _usY, uint16_t _usLen, uint16_t _usColor)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		bsp_lcd_set_pixel(_usX + i, _usY, _usColor);
	}
}

// ����ˮƽ��
void bsp_lcd_draw_line_v(uint16_t _usX, uint16_t _usY, uint16_t _usLen, uint16_t _usColor)
{
	uint16_t i;

	for (i = 0; i < _usLen; i++)
	{
		bsp_lcd_set_pixel(_usX, _usY + i, _usColor);
	}
}
// ���ƾ���
void bsp_lcd_draw_rect(uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	/*
	 ---------------->---
	|(_usX��_usY)        |
	V                    V  _usHeight
	|                    |
	 ---------------->---
		  _usWidth
	*/
	bsp_lcd_draw_line_h(_usX, _usY, _usWidth, _usColor);
	bsp_lcd_draw_line_v(_usX + _usWidth - 1, _usY, _usHeight, _usColor);
	bsp_lcd_draw_line_h(_usX, _usY + _usHeight - 1, _usWidth, _usColor);
	bsp_lcd_draw_line_v(_usX, _usY, _usHeight, _usColor);
}

// ͨ��DMA2D��ǰ���㸴��ָ���������ɫ���ݵ�Ŀ������
static void DMA2D_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)
{
	uint32_t PixelFormat;

	PixelFormat = LTDC_PIXEL_FORMAT_RGB565;

	/* ��ɫ��� */
	DMA2D->CR = 0x00030000UL | (1 << 9);
	DMA2D->OCOLR = ColorIndex;

	/* ����������ɫĿ�ĵ�ַ */
	DMA2D->OMAR = (uint32_t)pDst;

	/* Ŀ����ƫ�Ƶ�ַ */
	DMA2D->OOR = OffLine;

	/* ������ɫ��ʽ */
	DMA2D->OPFCCR = PixelFormat;

	/* ��������С */
	DMA2D->NLR = (uint32_t)(xSize << 16) | (uint16_t)ySize;

	DMA2D->CR |= DMA2D_CR_START;

	/* �ȴ�DMA2D������� */
	while (DMA2D->CR & DMA2D_CR_START)
	{
	}
}

// ʹ��DMA2D������
void bsp_lcd_fill_rect(uint32_t layer, uint16_t _usX, uint16_t _usY, uint16_t _usHeight, uint16_t _usWidth, uint16_t _usColor)
{
	/* ʹ��DMA2DӲ�������� */
	uint32_t Xaddress = 0;
	uint16_t OutputOffset;
	uint16_t NumberOfLine;
	uint16_t PixelPerLine;

	Xaddress = gram + (Width * _usY + _usX);
	OutputOffset = Width - _usWidth;
	NumberOfLine = _usHeight;
	PixelPerLine = _usWidth;

	// DMA2D_FillBuffer(uint32_t LayerIndex, void * pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)
	DMA2D_FillBuffer(layer, (void *)Xaddress, PixelPerLine, NumberOfLine, OutputOffset, _usColor);
}

// ����
void bsp_lcd_clr_scr(uint16_t _usColor)
{
	bsp_lcd_fill_rect(LTDC_LAYER_1,0, 0, Height, Width, _usColor);
}

// ���� Bresenham �㷨����������ֱ��
void bsp_lcd_draw_line(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2, uint16_t _usColor)
{
	int32_t dx, dy;
	int32_t tx, ty;
	int32_t inc1, inc2;
	int32_t d, iTag;
	int32_t x, y;

	/* ���� Bresenham �㷨����2��仭һ��ֱ�� */

	bsp_lcd_set_pixel(_usX1, _usY1, _usColor);

	/* ��������غϣ���������Ķ�����*/
	if (_usX1 == _usX2 && _usY1 == _usY2)
	{
		return;
	}

	iTag = 0;
	/* dx = abs ( _usX2 - _usX1 ); */
	if (_usX2 >= _usX1)
	{
		dx = _usX2 - _usX1;
	}
	else
	{
		dx = _usX1 - _usX2;
	}

	/* dy = abs ( _usY2 - _usY1 ); */
	if (_usY2 >= _usY1)
	{
		dy = _usY2 - _usY1;
	}
	else
	{
		dy = _usY1 - _usY2;
	}

	if (dx < dy) /*���dyΪ�Ƴ������򽻻��ݺ����ꡣ*/
	{
		uint16_t temp;

		iTag = 1;
		temp = _usX1;
		_usX1 = _usY1;
		_usY1 = temp;
		temp = _usX2;
		_usX2 = _usY2;
		_usY2 = temp;
		temp = dx;
		dx = dy;
		dy = temp;
	}
	tx = _usX2 > _usX1 ? 1 : -1; /* ȷ������1���Ǽ�1 */
	ty = _usY2 > _usY1 ? 1 : -1;
	x = _usX1;
	y = _usY1;
	inc1 = 2 * dy;
	inc2 = 2 * (dy - dx);
	d = inc1 - dx;
	while (x != _usX2) /* ѭ������ */
	{
		if (d < 0)
		{
			d += inc1;
		}
		else
		{
			y += ty;
			d += inc2;
		}
		if (iTag)
		{
			bsp_lcd_set_pixel(y, x, _usColor);
		}
		else
		{
			bsp_lcd_set_pixel(x, y, _usColor);
		}
		x += tx;
	}
}

// ���� Bresenham �㷨������һ��㣬���ڲ�����ʾ
void bsp_lcd_draw_lines(uint16_t *x, uint16_t *y, uint16_t _usSize, uint16_t _usColor)
{
	uint16_t i;

	for (i = 0; i < _usSize - 1; i++)
	{
		bsp_lcd_draw_line(x[i], y[i] <= 470 ? y[i] : 470, x[i + 1], y[i + 1] <= 470 ? y[i + 1] : 470, _usColor);
	}
}

// ����Բ
void bsp_lcd_draw_circle(uint16_t _usX, uint16_t _usY, uint16_t _usRadius, uint16_t _usColor)
{
	int32_t D;	   /* Decision Variable */
	uint32_t CurX; /* ��ǰ X ֵ */
	uint32_t CurY; /* ��ǰ Y ֵ */

	D = 3 - (_usRadius << 1);
	CurX = 0;
	CurY = _usRadius;

	while (CurX <= CurY)
	{
		bsp_lcd_set_pixel(_usX + CurX, _usY + CurY, _usColor);
		bsp_lcd_set_pixel(_usX + CurX, _usY - CurY, _usColor);
		bsp_lcd_set_pixel(_usX - CurX, _usY + CurY, _usColor);
		bsp_lcd_set_pixel(_usX - CurX, _usY - CurY, _usColor);
		bsp_lcd_set_pixel(_usX + CurY, _usY + CurX, _usColor);
		bsp_lcd_set_pixel(_usX + CurY, _usY - CurX, _usColor);
		bsp_lcd_set_pixel(_usX - CurY, _usY + CurX, _usColor);
		bsp_lcd_set_pixel(_usX - CurY, _usY - CurX, _usColor);

		if (D < 0)
		{
			D += (CurX << 2) + 6;
		}
		else
		{
			D += ((CurX - CurY) << 2) + 10;
			CurY--;
		}
		CurX++;
	}
}

void bsp_lcd_draw_tri(uint16_t _usX, uint16_t _usY, uint16_t _usLen, uint16_t _usColor)
{
	for (uint16_t i = 0; i < _usLen / 2; i++)
	{
		bsp_lcd_draw_line_h(_usX, _usY + i, i, _usColor);
		bsp_lcd_draw_line_h(_usX, _usY + i + _usLen / 2, _usLen / 2 - i, _usColor);
	}
}

uint16_t LCD_GetFontHeight(FONT_T *_tFont)
{
	uint16_t height = 16;

	switch (_tFont->FontCode)
	{
	case FC_ST_12:
		height = 12;
		break;

	case FC_ST_16:
	case FC_RA8875_16:
		height = 16;
		break;

	case FC_RA8875_24:
	case FC_ST_24:
		height = 24;
		break;

	case FC_ST_32:
	case FC_RA8875_32:
		height = 32;
		break;
	}
	return height;
}

uint16_t LCD_GetFontWidth(FONT_T *_tFont)
{
	uint16_t font_width = 16;

	switch (_tFont->FontCode)
	{
	case FC_ST_12:
		font_width = 12;
		break;

	case FC_ST_16:
	case FC_RA8875_16:
		font_width = 16;
		break;

	case FC_RA8875_24:
	case FC_ST_24:
		font_width = 24;
		break;

	case FC_ST_32:
	case FC_RA8875_32:
		font_width = 32;
		break;
	}
	return font_width;
}

uint16_t LCD_GetStrWidth(char *_ptr, FONT_T *_tFont)
{
	char *p = _ptr;
	uint16_t width = 0;
	uint8_t code1, code2;
	uint16_t font_width;

	font_width = LCD_GetFontWidth(_tFont);

	while (*p != 0)
	{
		code1 = *p;		  /* ��ȡ�ַ������ݣ� �����ݿ�����ascii���룬Ҳ���ܺ��ִ���ĸ��ֽ� */
		if (code1 < 0x80) /* ASCII */
		{
			switch (_tFont->FontCode)
			{
			case FC_ST_12:
				font_width = 6;
				break;

			case FC_ST_16:
				font_width = 8;
				break;

			case FC_ST_24:
				font_width = 12;
				break;

			case FC_ST_32:
				font_width = 16;
				break;

			default:
				font_width = 8;
				break;
			}
		}
		else /* ���� */
		{
			code2 = *++p;
			if (code2 == 0)
			{
				break;
			}
			font_width = LCD_GetFontWidth(_tFont);
		}
		width += font_width;
		p++;
	}

	return width;
}

static void _LCD_ReadAsciiDot(uint8_t _code, uint8_t _fontcode, uint8_t *_pBuf)
{
	const uint8_t *pAscDot;
	uint8_t font_bytes = 0;
	uint16_t m;
	uint16_t address;
	uint8_t fAllHz = 0; /* 1��ʾ��������Ƕȫ����ASCII�ַ��� */

	pAscDot = 0;
	switch (_fontcode)
	{
	case FC_ST_12: /* 12���� */
		font_bytes = 24 / 2;
		pAscDot = g_Ascii12;
		fAllHz = 1;
		break;

	case FC_ST_16:
		/* ȱʡ��16���� */
		font_bytes = 32 / 2;
		pAscDot = g_Ascii16;
		fAllHz = 1;
		break;

	case FC_ST_24:
		font_bytes = 48;
		pAscDot = g_Ascii24;
		break;

	case FC_ST_32:
		font_bytes = 64;
		pAscDot = g_Ascii32;
		break;

	default:
		return;
	}

	if (fAllHz == 1) /* ��Ƕȫ��ASCII�ַ����� */
	{
		/* ��CPU�ڲ�Flash�е�ascii�ַ������Ƶ�buf */
		memcpy(_pBuf, &pAscDot[_code * (font_bytes)], (font_bytes));
	}
	else /* ��Ƕ�����ַ�����ģ�������ֽ���ASCII�� */
	{
		m = 0;
		while (1)
		{
			address = m * (font_bytes + 1);
			m++;
			if (_code == pAscDot[address + 0])
			{
				address += 1;
				memcpy(_pBuf, &pAscDot[address], font_bytes);
				break;
			}
			else if ((pAscDot[address + 0] == 0xFF) && (pAscDot[address + 1] == 0xFF))
			{
				/* �ֿ�������ϣ�δ�ҵ��������ȫFF */
				memset(_pBuf, 0xFF, font_bytes);
				break;
			}
		}
	}
}

static void _LCD_ReadHZDot(uint8_t _code1, uint8_t _code2, uint8_t _fontcode, uint8_t *_pBuf)
{
#ifdef USE_SMALL_FONT /* ʹ��CPU �ڲ�Flash С�ֿ� */
	uint8_t *pDot;
	uint8_t font_bytes = 0;
	uint32_t address;
	uint16_t m;

	pDot = 0; /* �������ڱ���澯 */
	switch (_fontcode)
	{
	case FC_ST_12: /* 12���� */
		font_bytes = 24;
		pDot = (uint8_t *)g_Hz12;
		break;

	case FC_ST_16:
		font_bytes = 32;
		pDot = (uint8_t *)g_Hz16;
		break;

	case FC_ST_24:
		font_bytes = 72;
		pDot = (uint8_t *)g_Hz24;
		break;

	case FC_ST_32:
		font_bytes = 128;
		pDot = (uint8_t *)g_Hz32;
		break;

	case FC_RA8875_16:
	case FC_RA8875_24:
	case FC_RA8875_32:
		return;
	}

	m = 0;
	while (1)
	{
		address = m * (font_bytes + 2);
		m++;
		if ((_code1 == pDot[address + 0]) && (_code2 == pDot[address + 1]))
		{
			address += 2;
			memcpy(_pBuf, &pDot[address], font_bytes);
			break;
		}
		else if ((pDot[address + 0] == 0xFF) && (pDot[address + 1] == 0xFF))
		{
			/* �ֿ�������ϣ�δ�ҵ��������ȫFF */
			memset(_pBuf, 0xFF, font_bytes);
			break;
		}
	}
#else /* ��ȫ�ֿ� */
	uint8_t *pDot = 0;
	uint8_t font_bytes = 0;

	switch (_fontcode)
	{
	case FC_ST_12: /* 12���� */
		font_bytes = 24;
		pDot = (uint8_t *)HZK12_ADDR;
		break;

	case FC_ST_16:
		font_bytes = 32;
		pDot = (uint8_t *)HZK16_ADDR;
		break;

	case FC_ST_24:
		font_bytes = 72;
		pDot = (uint8_t *)HZK24_ADDR;
		break;

	case FC_ST_32:
		font_bytes = 128;
		pDot = (uint8_t *)HZK32_ADDR;
		break;

	case FC_RA8875_16:
	case FC_RA8875_24:
	case FC_RA8875_32:
		return;
	}

	/* �˴���Ҫ�����ֿ��ļ����λ�ý����޸� */
	if (_code1 >= 0xA1 && _code1 <= 0xA9 && _code2 >= 0xA1)
	{
		pDot += ((_code1 - 0xA1) * 94 + (_code2 - 0xA1)) * font_bytes;
	}
	else if (_code1 >= 0xB0 && _code1 <= 0xF7 && _code2 >= 0xA1)
	{
		pDot += ((_code1 - 0xB0) * 94 + (_code2 - 0xA1) + 846) * font_bytes;
	}
	memcpy(_pBuf, pDot, font_bytes);
	// sf_ReadBuffer((uint8_t *)_pBuf, (uint32_t)pDot, font_bytes);
#endif
}

void LCD_DispStrEx(uint32_t layer,uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont, uint16_t _Width,
				   uint8_t _Align)
{
	uint32_t i;
	uint8_t code1;
	uint8_t code2;
	uint8_t buf[32 * 32 / 8]; /* ���֧��32������ */
	uint8_t width;
	uint16_t m;
	uint8_t font_width = 0;
	uint8_t font_height = 0;
	uint16_t x, y;
	uint16_t offset;
	uint16_t str_width; /* �ַ���ʵ�ʿ��  */

	uint8_t line_bytes;
	uint8_t asc_bytes = 0;
	uint8_t hz_bytes = 0;

	switch (_tFont->FontCode)
	{
	case FC_ST_12: /* 12���� */
		font_height = 12;
		font_width = 12;
		asc_bytes = 1;
		hz_bytes = 2;
		break;

	case FC_ST_16:
		font_height = 16;
		font_width = 16;
		asc_bytes = 1;
		hz_bytes = 2;
		break;

	case FC_ST_24:
		font_height = 24;
		font_width = 24;
		asc_bytes = 2;
		hz_bytes = 3;
		break;

	case FC_ST_32:
		font_height = 32;
		font_width = 32;
		asc_bytes = 2;
		hz_bytes = 4;
		break;

	case FC_RA8875_16:
	case FC_RA8875_24:
	case FC_RA8875_32:
		break;
	}

	str_width = LCD_GetStrWidth(_ptr, _tFont); /* �����ַ���ʵ�ʿ��(RA8875�ڲ�ASCII������Ϊ�䳤 */
	offset = 0;
	if (_Width > str_width)
	{
		if (_Align == ALIGN_RIGHT) /* �Ҷ��� */
		{
			offset = _Width - str_width;
		}
		else if (_Align == ALIGN_CENTER) /* ����� */
		{
			offset = (_Width - str_width) / 2;
		}
		else /* ����� ALIGN_LEFT */
		{
			;
		}
	}

	/* ������ɫ, �м������ұ߶���  */
	if (offset > 0)
	{
		bsp_lcd_fill_rect(layer,_usX, _usY, LCD_GetFontHeight(_tFont), offset, _tFont->BackColor);
		_usX += offset;
	}

	/* �Ҳ����ɫ */
	if (_Width > str_width)
	{
		bsp_lcd_fill_rect(layer,_usX + str_width, _usY, LCD_GetFontHeight(_tFont), _Width - str_width - offset, _tFont->BackColor);
	}

	/* ʹ��CPU�ڲ��ֿ�. ������Ϣ��CPU��ȡ */
	{
		/* ��ʼѭ�������ַ� */
		while (*_ptr != 0)
		{
			code1 = *_ptr; /* ��ȡ�ַ������ݣ� �����ݿ�����ascii���룬Ҳ���ܺ��ִ���ĸ��ֽ� */
			if (code1 < 0x80)
			{
				/* ��ascii�ַ������Ƶ�buf */
				_LCD_ReadAsciiDot(code1, _tFont->FontCode, buf); /* ��ȡASCII�ַ����� */
				width = font_width / 2;
				line_bytes = asc_bytes;
			}
			else
			{
				code2 = *++_ptr;
				if (code2 == 0)
				{
					break;
				}
				/* ��1�����ֵĵ��� */
				_LCD_ReadHZDot(code1, code2, _tFont->FontCode, buf);
				width = font_width;
				line_bytes = hz_bytes;
			}

			y = _usY;
			/* ��ʼˢLCD */
			for (m = 0; m < font_height; m++) /* �ַ��߶� */
			{
				x = _usX;
				for (i = 0; i < width; i++) /* �ַ���� */
				{
					if ((buf[m * line_bytes + i / 8] & (0x80 >> (i % 8))) != 0x00)
					{
						// bsp_lcd_set_pixel(x, y, _tFont->FrontColor); /* ����������ɫΪ����ɫ */
						if(layer == LTDC_LAYER_1)
						{
							bsp_lcd_set_pixel(x, y, _tFont->FrontColor); /* ����������ɫΪ����ɫ */
						}
						else if(layer == LTDC_LAYER_2)
						{
							bsp_lcd_set_pixel_l2(y_s,x_s,x, y, _tFont->FrontColor);
						}
					}
					else
					{
						if (_tFont->BackColor != CL_MASK) /* ͸��ɫ */
						{
							// bsp_lcd_set_pixel(x, y, _tFont->BackColor); /* ����������ɫΪ���ֱ���ɫ */
							if(layer == LTDC_LAYER_1)
							{
								bsp_lcd_set_pixel(x, y, _tFont->BackColor); /* ����������ɫΪ���ֱ���ɫ */
							}
							else if(layer == LTDC_LAYER_2)
							{
								bsp_lcd_set_pixel_l2(y_s,x_s,x, y, _tFont->BackColor);
							}
						}
					}

					x++;
				}
				y--;
			}

			if (_tFont->Space > 0)
			{
				/* ������ֵ�ɫ��_tFont->usBackColor�������ּ����ڵ���Ŀ�ȣ���ô��Ҫ������֮�����(��ʱδʵ��) */
			}
			_usX += width + _tFont->Space; /* �е�ַ���� */
			_ptr++;						   /* ָ����һ���ַ� */
		}
	}
}

void LCD_DispStr(uint32_t layer,uint16_t _usX, uint16_t _usY, char *_ptr, FONT_T *_tFont)
{
	LCD_DispStrEx(layer,_usX, _usY, _ptr, _tFont, 0, 0);
}

static int bsp_lcd_init(void)
{

	LTDC_LayerCfgTypeDef pLayerCfg = {0};
	LTDC_LayerCfgTypeDef pLayerCfg2 = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	__HAL_RCC_GPIOH_CLK_ENABLE();

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLL3.PLL3M = 5;
	PeriphClkInitStruct.PLL3.PLL3N = 30;
	PeriphClkInitStruct.PLL3.PLL3P = 2;
	PeriphClkInitStruct.PLL3.PLL3Q = 2;
	PeriphClkInitStruct.PLL3.PLL3R = 6;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	hltdc.Instance = LTDC;
	hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
	hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
	hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
	hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
	hltdc.Init.HorizontalSync = (HSYNC_W - 1);
	hltdc.Init.VerticalSync = (VSYNC_W - 1);
	hltdc.Init.AccumulatedHBP = (HSYNC_W + HBP - 1);
	hltdc.Init.AccumulatedVBP = (VSYNC_W + VBP - 1);
	hltdc.Init.AccumulatedActiveH = (Height + VSYNC_W + VBP - 1);
	hltdc.Init.AccumulatedActiveW = (Width + HSYNC_W + HBP - 1);
	hltdc.Init.TotalHeigh = (Height + VSYNC_W + VBP + VFP - 1);
	hltdc.Init.TotalWidth = (Width + HSYNC_W + HBP + HFP - 1);
	hltdc.Init.Backcolor.Blue = 0;
	hltdc.Init.Backcolor.Green = 0;
	hltdc.Init.Backcolor.Red = 0;
	if (HAL_LTDC_Init(&hltdc) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}

	pLayerCfg.WindowX0 = 0;
	pLayerCfg.WindowX1 = 800;
	pLayerCfg.WindowY0 = 0;
	pLayerCfg.WindowY1 = 480;
	pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
	pLayerCfg.Alpha = 255;
	pLayerCfg.Alpha0 = 0;
	pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
	pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
	pLayerCfg.FBStartAdress = (unsigned int)gram;
	pLayerCfg.ImageWidth = 800;
	pLayerCfg.ImageHeight = 480;
	pLayerCfg.Backcolor.Blue = 0;
	pLayerCfg.Backcolor.Green = 0;
	pLayerCfg.Backcolor.Red = 0;
	if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}

	pLayerCfg2.WindowX0 = 0;
	pLayerCfg2.WindowX1 = 100;
	pLayerCfg2.WindowY0 = 0;
	pLayerCfg2.WindowY1 = 100;
	pLayerCfg2.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
	pLayerCfg2.Alpha = 0;
	pLayerCfg2.Alpha0 = 0;
	pLayerCfg2.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
	pLayerCfg2.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
	pLayerCfg2.FBStartAdress = (unsigned int)gram2;
	pLayerCfg2.ImageWidth = 100;
	pLayerCfg2.ImageHeight = 100;
	pLayerCfg2.Backcolor.Blue = 0;
	pLayerCfg2.Backcolor.Green = 0;
	pLayerCfg2.Backcolor.Red = 0;
	if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg2, 1) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}

	bsp_lcd_dma2d_init();

	return OS_OK;
}

void osc_dev_l2_enable(unsigned short x0, unsigned short y0, unsigned short x0_size, unsigned short y0_size, unsigned char alpha)
{
	memset(gram2,0,sizeof(gram2) / sizeof(uint16_t));
	HAL_LTDC_SetWindowPosition(&hltdc, x0, 480 - y0 - y0_size, LTDC_LAYER_2);
	HAL_LTDC_SetWindowSize(&hltdc, x0_size, y0_size, LTDC_LAYER_2);
	HAL_LTDC_SetAlpha(&hltdc, alpha, LTDC_LAYER_2);
}

void osc_dev_l2_disable(void)
{
	HAL_LTDC_SetAlpha(&hltdc, 0, LTDC_LAYER_2);
}

void HAL_LTDC_MspInit(LTDC_HandleTypeDef *ltdcHandle)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
	if (ltdcHandle->Instance == LTDC)
	{

		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
		PeriphClkInitStruct.PLL3.PLL3M = 2;
		PeriphClkInitStruct.PLL3.PLL3N = 12;
		PeriphClkInitStruct.PLL3.PLL3P = 2;
		PeriphClkInitStruct.PLL3.PLL3Q = 2;
		PeriphClkInitStruct.PLL3.PLL3R = 6;
		PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_3;
		PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
		PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}

		//    /* LTDC clock enable */
		//    __HAL_RCC_LTDC_CLK_ENABLE();

		//    __HAL_RCC_GPIOC_CLK_ENABLE();
		//    __HAL_RCC_GPIOA_CLK_ENABLE();
		//    __HAL_RCC_GPIOB_CLK_ENABLE();
		//    __HAL_RCC_GPIOE_CLK_ENABLE();
		//    __HAL_RCC_GPIOD_CLK_ENABLE();
		//    /**LTDC GPIO Configuration
		//    PC0     ------> LTDC_G2
		//    PC1     ------> LTDC_G5
		//    PA3     ------> LTDC_B5
		//    PA4     ------> LTDC_VSYNC
		//    PA5     ------> LTDC_R4
		//    PC4     ------> LTDC_R7
		//    PC5     ------> LTDC_DE
		//    PB0     ------> LTDC_R3
		//    PB1     ------> LTDC_R6
		//    PE11     ------> LTDC_G3
		//    PE12     ------> LTDC_B4
		//    PE14     ------> LTDC_CLK
		//    PB10     ------> LTDC_G4
		//    PB15     ------> LTDC_G7
		//    PD10     ------> LTDC_B3
		//    PC6     ------> LTDC_HSYNC
		//    PC7     ------> LTDC_G6
		//    PA9     ------> LTDC_R5
		//    PA15     ------> LTDC_B6
		//    PD2     ------> LTDC_B7
		//    */
		//    GPIO_InitStruct.Pin = GPIO_PIN_0;
		//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		//    GPIO_InitStruct.Pull = GPIO_NOPULL;
		//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		//    GPIO_InitStruct.Alternate = GPIO_AF11_LTDC;
		//    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		//    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
		//                          |GPIO_PIN_7;
		//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		//    GPIO_InitStruct.Pull = GPIO_NOPULL;
		//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		//    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		//    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		//    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_9
		//                          |GPIO_PIN_15;
		//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		//    GPIO_InitStruct.Pull = GPIO_NOPULL;
		//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		//    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		//    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
		//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		//    GPIO_InitStruct.Pull = GPIO_NOPULL;
		//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		//    GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
		//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		//    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_14;
		//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		//    GPIO_InitStruct.Pull = GPIO_NOPULL;
		//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		//    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		//    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

		//    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_15;
		//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		//    GPIO_InitStruct.Pull = GPIO_NOPULL;
		//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		//    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		//    GPIO_InitStruct.Pin = GPIO_PIN_10;
		//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		//    GPIO_InitStruct.Pull = GPIO_NOPULL;
		//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		//    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		//    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

		//    GPIO_InitStruct.Pin = GPIO_PIN_2;
		//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		//    GPIO_InitStruct.Pull = GPIO_NOPULL;
		//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		//    GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
		//    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
		//
		//	GPIO_InitStruct.Pin = GPIO_PIN_3;
		//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		//	GPIO_InitStruct.Pull = GPIO_NOPULL;
		//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		//	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* LTDC clock enable */
		__HAL_RCC_LTDC_CLK_ENABLE();

		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOE_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
		/**LTDC GPIO Configuration
		PC0     ------> LTDC_G2
		PC1     ------> LTDC_G5
		PA3     ------> LTDC_B5
		PA4     ------> LTDC_VSYNC
		PA5     ------> LTDC_R4
		PC4     ------> LTDC_R7
		PC5     ------> LTDC_DE
		PB0     ------> LTDC_R3
		PB1     ------> LTDC_R6
		PE11     ------> LTDC_G3
		PE12     ------> LTDC_B4
		PE14     ------> LTDC_CLK
		PB10     ------> LTDC_G4
		PB15     ------> LTDC_G7
		PC6     ------> LTDC_HSYNC
		PC7     ------> LTDC_G6
		PA8     ------> LTDC_B3
		PA9     ------> LTDC_R5
		PA15     ------> LTDC_B6
		PD2     ------> LTDC_B7
		*/

		GPIO_InitStruct.Pin = GPIO_PIN_0;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF11_LTDC;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_9 | GPIO_PIN_15;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_15 | GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_8;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF13_LTDC;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		//    GPIO_InitStruct.Pin = GPIO_PIN_2;
		//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		//    GPIO_InitStruct.Pull = GPIO_NOPULL;
		//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		//    GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
		//    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 1);
	}
}

void bsp_lcd_dma2d_init(void)
{
	__HAL_RCC_DMA2D_CLK_ENABLE();

	hdma2d.Instance = DMA2D;
	hdma2d.Init.Mode = DMA2D_R2M;
	hdma2d.Init.ColorMode = DMA2D_INPUT_RGB565;
	hdma2d.Init.OutputOffset = 0;
	if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}
}

void LTDC_IRQHandler(void)
{
	HAL_LTDC_IRQHandler(&hltdc);
}
