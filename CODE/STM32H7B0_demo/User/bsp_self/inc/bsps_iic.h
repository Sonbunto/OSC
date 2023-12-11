#ifndef __BSPS_IIC_H
#define __BSPS_IIC_H

void bsps_iic_init(void);

extern I2C_HandleTypeDef hi2c1;



 
/*设定使用的电容屏IIC设备地址*/
#define GTP_ADDRESS            0xBA
 
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))
 
/*I2C引脚*/
#define GTP_I2C_SCL_PIN                  GPIO_PIN_6                 
#define GTP_I2C_SCL_GPIO_PORT            GPIOB                     
 
#define GTP_I2C_SDA_PIN                  GPIO_PIN_7                 
#define GTP_I2C_SDA_GPIO_PORT            GPIOB                    
 
/*复位引脚*/
#define GTP_RST_GPIO_PORT                GPIOB
#define GTP_RST_GPIO_PIN                 GPIO_PIN_4
/*中断引脚*/
#define GTP_INT_GPIO_PORT                GPIOB
#define GTP_INT_GPIO_PIN                 GPIO_PIN_5

 
 
//软件IIC使用的宏
#define I2C_SCL_1()  HAL_GPIO_WritePin(GTP_I2C_SCL_GPIO_PORT, GTP_I2C_SCL_PIN,GPIO_PIN_SET)        /* SCL = 1 */
#define I2C_SCL_0()  HAL_GPIO_WritePin(GTP_I2C_SCL_GPIO_PORT, GTP_I2C_SCL_PIN,GPIO_PIN_RESET)        /* SCL = 0 */
 
#define I2C_SDA_1()  HAL_GPIO_WritePin(GTP_I2C_SDA_GPIO_PORT, GTP_I2C_SDA_PIN,GPIO_PIN_SET)        /* SDA = 1 */
#define I2C_SDA_0()  HAL_GPIO_WritePin(GTP_I2C_SDA_GPIO_PORT, GTP_I2C_SDA_PIN,GPIO_PIN_RESET)        /* SDA = 0 */
 
#define I2C_SDA_READ()  HAL_GPIO_ReadPin(GTP_I2C_SDA_GPIO_PORT, GTP_I2C_SDA_PIN)    /* 读SDA口线状态 */
 
//函数接口
void I2C_Touch_Init(void);
uint32_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite);
uint32_t I2C_ReadBytes(uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead);
void I2C_ResetChip(void);
void I2C_GTP_IRQDisable(void);
void I2C_GTP_IRQEnable(void);
 

#endif
