/*
 * @version: V1.0.0
 * @Author: harmsworth
 * @Date: 2023-03-27 13:07:17
 * @LastEditors: harmsworth
 * @LastEditTime: 2023-03-27 13:20:45
 * @company: null
 * @Mailbox: jojoharmsworth@gmail.com
 * @FilePath: \MDK-ARMg:\Program\Stm32\Projects\OLED_Menu_u8g2\Devices\dht11.c
 * @Descripttion: 
 */
#include "dht11.h"

/* 可以在下面的宏定义中把后面的延时函数替换换SysTick的延时函数，就是想用那个就换成那个的 */

#define DHT11_DELAY_US(us) CPU_TS_Tmr_Delay_US(us)
#define DHT11_DELAY_MS(ms) CPU_TS_Tmr_Delay_MS(ms)

static void DHT11_GPIO_Config(void);
static void DHT11_Mode_IPU(void);
static void DHT11_Mode_Out_PP(void);
static uint8_t DHT11_ReadByte(void);

/**
 * @brief  DHT11 初始化函数
 * @param  无
 * @retval 无
 */
void DHT11_Init(void)
{
	DHT11_GPIO_Config();

	DHT11_Dout_1; // 拉高GPIOB10
}

/*
 * 函数名：DHT11_GPIO_Config
 * 描述  ：配置DHT11用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
static void DHT11_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOG_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_11, GPIO_PIN_SET);

	/*Configure GPIO pin : PG11 */
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

/*
 * 函数名：DHT11_Mode_IPU
 * 描述  ：使DHT11-DATA引脚变为上拉输入模式
 * 输入  ：无
 * 输出  ：无
 */
static void DHT11_Mode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	/*选择要控制的DHT11_Dout_GPIO_PORT引脚*/
	GPIO_InitStructure.Pin = GPIO_PIN_11;

	/*设置引脚模式为浮空输入模式*/
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;

	/*调用库函数，初始化DHT11_Dout_GPIO_PORT*/
	HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/*
 * 函数名：DHT11_Mode_Out_PP
 * 描述  ：使DHT11-DATA引脚变为推挽输出模式
 * 输入  ：无
 * 输出  ：无
 */
static void DHT11_Mode_Out_PP(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*选择要控制的DHT11_Dout_GPIO_PORT引脚*/
	GPIO_InitStructure.Pin = GPIO_PIN_11;

	/*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;

	/*设置引脚速率为50MHz */
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;

	/*调用库函数，初始化DHT11_Dout_GPIO_PORT*/
	HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/*
 * 从DHT11读取一个字节，MSB先行
 */
static uint8_t DHT11_ReadByte(void)
{
	uint8_t i, temp = 0;

	for (i = 0; i < 8; i++)
	{
		/*每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束*/
		while (DHT11_Dout_IN() == LOW)
			;

		/*DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，
		 *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时
		 */
		HAL_Delay_us(40); // 延时x us 这个延时需要大于数据0持续的时间即可

		if (DHT11_Dout_IN() == HIGH) /* x us后仍为高电平表示数据“1” */
		{
			/* 等待数据1的高电平结束 */
			while (DHT11_Dout_IN() == HIGH)
				;

			temp |= (uint8_t)(0x01 << (7 - i)); // 把第7-i位置1，MSB先行
		}
		else // x us后为低电平表示数据“0”
		{
			temp &= (uint8_t) ~(0x01 << (7 - i)); // 把第7-i位置0，MSB先行
		}
	}

	return temp;
}

/*
 * 一次完整的数据传输为40bit，高位先出
 * 8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和
 */
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{
	/*输出模式*/
	DHT11_Mode_Out_PP();
	/*主机拉低*/
	DHT11_Dout_0;
	/*延时18ms*/
	HAL_Delay_ms(18);

	/*总线拉高 主机延时30us*/
	DHT11_Dout_1;

	HAL_Delay_us(30); // 延时30us

	/*主机设为输入 判断从机响应信号*/
	DHT11_Mode_IPU();

	/*判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行*/
	if (DHT11_Dout_IN() == LOW)
	{
		/*轮询直到从机发出 的80us 低电平 响应信号结束*/
		while (DHT11_Dout_IN() == LOW)
			;

		/*轮询直到从机发出的 80us 高电平 标置信号结束*/
		while (DHT11_Dout_IN() == HIGH)
			;

		/*开始接收数据*/
		DHT11_Data->humi_int = DHT11_ReadByte();

		DHT11_Data->humi_deci = DHT11_ReadByte();

		DHT11_Data->temp_int = DHT11_ReadByte();

		DHT11_Data->temp_deci = DHT11_ReadByte();

		DHT11_Data->check_sum = DHT11_ReadByte();

		/*读取结束，引脚改为输出模式*/
		DHT11_Mode_Out_PP();
		/*主机拉高*/
		DHT11_Dout_1;

		/*检查读取的数据是否正确*/
		if (DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int + DHT11_Data->temp_deci)
			return SUCCESS;
		else
			return ERROR;
	}

	else
		return ERROR;
}

void HAL_Delay_us(uint32_t nus)
{
	//设置定时1us中断一次
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000000);
    //调用系统自带的延时函数
	HAL_Delay(nus - 1);
    //将定时中断恢复为1ms中断
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

}

void HAL_Delay_ms(uint32_t nms)
{
	HAL_Delay_us(1000);
}
