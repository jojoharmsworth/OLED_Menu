#ifndef __DHT11_H
#define __DHT11_H
#include "stm32f1xx.h"

#define HIGH 1
#define LOW 0

#define OUTPUT 1
#define INPUT 0

#define MS 1
#define US 0

#define OVER_TIME 100

#define DHT11_GPIO_PIN GPIO_PIN_11
#define DHT11_GPIO_PORT GPIOG

#define DHT11_Dout_0 HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, LOW)
#define DHT11_Dout_1 HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, HIGH)
#define DHT11_Dout_IN() HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN)

/************************** DHT11 数据类型定义********************************/
typedef struct
{
	uint8_t  humi_int;		//湿度的整数部分
	uint8_t  humi_deci;	 	//湿度的小数部分
	uint8_t  temp_int;	 	//温度的整数部分
	uint8_t  temp_deci;	 	//温度的小数部分
	uint8_t  check_sum;	 	//校验和
		                 
} DHT11_Data_TypeDef;


void DHT11_Init(void);
static void DHT11_GPIO_Config(void);
static void DHT11_Mode_IPU(void);
static void DHT11_Mode_Out_PP(void);
void dht11StartSignal(void);
static uint8_t DHT11_ReadByte(void);
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data);
void HAL_Delay_us(uint32_t nus);
void HAL_Delay_ms(uint32_t nms);

#endif
