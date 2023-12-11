#ifndef __BSPS_TOUCH_H
#define __BSPS_TOUCH_H

/*I2C���*/
#define		GT911_I2C			hi2c1

#define		GT911_DIV_ID	0XBA	//�豸��ַ //0X28 //0XBA

#define 	GT911_DIV_W		(GT911_DIV_ID | 0)	//д��ַ
#define 	GT911_DIV_R		(GT911_DIV_ID | 1)	//����ַ



//GT911 ���ּĴ������� 
#define GT_CTRL_REG 	0X8040   	//GT911���ƼĴ���
#define GT_CFGS_REG 	0X8047   	//GT911������ʼ��ַ�Ĵ���
#define GT_CHECK_REG 	0X80FF   	//GT911У��ͼĴ���
#define GT_PID_REG 		0X8140   	//GT911��ƷID�Ĵ���
 
#define GT_GSTID_REG 	0X814E   	//GT911��ǰ��⵽�Ĵ������,��7λ�Ǵ�����־λ����4λ�Ǵ�����������

#define GT_TPD_Sta		0X8150		//��������ʼ���ݵ�ַ
#define GT_TP1_REG 		0X8150  	//��һ�����������ݵ�ַ
#define GT_TP2_REG 		0X8158		//�ڶ������������ݵ�ַ
#define GT_TP3_REG 		0X8160		//���������������ݵ�ַ
#define GT_TP4_REG 		0X8168		//���ĸ����������ݵ�ַ
#define GT_TP5_REG 		0X8170		//��������������ݵ�ַ


#define GT_TOUCH_MAX			5			//����gt911�����ͬʱ��ȡ5�������������

typedef enum
{
	X_L = 0,
	X_H = 1,
	Y_L = 2,
	Y_H = 3,
	S_L	= 4,
	S_H = 5
}bias_xys_t;	//����X��Y��������С����ƫ����

typedef enum
{
	TOUCH__NO		= 0x00,	//û�д���
	TOUCH_ING		= 0x80	//������	
}touch_sta_t;	//����״̬

typedef struct
{
	uint16_t	X_Point;	//X����
	uint16_t	Y_Point;	//Y����
	uint16_t	S_Point;	//�������С
}xys_data_t;	//����������


/*�����ṹ��*/
typedef struct 
{
	uint8_t Touch_State				;	//����״̬
	uint8_t Touch_Number			;	//��������
	xys_data_t Touch_XY[GT_TOUCH_MAX]	;	//������x���꣬����gt911���5���������
}touch_data_t;	//������Ϣ�ṹ��


/*�ⲿ������*/
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
