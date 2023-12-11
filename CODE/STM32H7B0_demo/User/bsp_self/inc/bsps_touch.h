#ifndef __BSPS_TOUCH_H
#define __BSPS_TOUCH_H

/*I2C句柄*/
#define		GT911_I2C			hi2c1

#define		GT911_DIV_ID	0XBA	//设备地址 //0X28 //0XBA

#define 	GT911_DIV_W		(GT911_DIV_ID | 0)	//写地址
#define 	GT911_DIV_R		(GT911_DIV_ID | 1)	//读地址



//GT911 部分寄存器定义 
#define GT_CTRL_REG 	0X8040   	//GT911控制寄存器
#define GT_CFGS_REG 	0X8047   	//GT911配置起始地址寄存器
#define GT_CHECK_REG 	0X80FF   	//GT911校验和寄存器
#define GT_PID_REG 		0X8140   	//GT911产品ID寄存器
 
#define GT_GSTID_REG 	0X814E   	//GT911当前检测到的触摸情况,第7位是触摸标志位，低4位是触摸点数个数

#define GT_TPD_Sta		0X8150		//触摸点起始数据地址
#define GT_TP1_REG 		0X8150  	//第一个触摸点数据地址
#define GT_TP2_REG 		0X8158		//第二个触摸点数据地址
#define GT_TP3_REG 		0X8160		//第三个触摸点数据地址
#define GT_TP4_REG 		0X8168		//第四个触摸点数据地址
#define GT_TP5_REG 		0X8170		//第五个触摸点数据地址


#define GT_TOUCH_MAX			5			//对于gt911，最多同时获取5个触摸点的数据

typedef enum
{
	X_L = 0,
	X_H = 1,
	Y_L = 2,
	Y_H = 3,
	S_L	= 4,
	S_H = 5
}bias_xys_t;	//数据X、Y、触摸大小数据偏移量

typedef enum
{
	TOUCH__NO		= 0x00,	//没有触摸
	TOUCH_ING		= 0x80	//被触摸	
}touch_sta_t;	//触摸状态

typedef struct
{
	uint16_t	X_Point;	//X坐标
	uint16_t	Y_Point;	//Y坐标
	uint16_t	S_Point;	//触摸点大小
}xys_data_t;	//触摸点坐标


/*触摸结构体*/
typedef struct 
{
	uint8_t Touch_State				;	//触摸状态
	uint8_t Touch_Number			;	//触摸数量
	xys_data_t Touch_XY[GT_TOUCH_MAX]	;	//触摸的x坐标，对于gt911最多5个点的坐标
}touch_data_t;	//触摸信息结构体


/*外部变量区*/
extern touch_data_t	User_Touch;
extern __IO uint16_t osc_mode;
extern __IO uint16_t osc_sa_rate;
extern __IO uint16_t g_sa_rate_spec;
extern __IO uint16_t g_sa_rate_wave;

void bsps_gt911_scanf(void);
static int bsps_gt911_init(void);
void bsps_touch_osc_mode(void);
void bsps_touch_area_task(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usY2,touch_data_t touch,void(*task)(void),uint8_t depth);
void bsps_touch_menu_enter(uint8_t depth);

#endif
