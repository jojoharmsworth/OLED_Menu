#include "bh1750.h"
#include "i2c.h"

I2C_HandleTypeDef hi2c2;

/**
 * @brief  向BH1750发送一条指令
 * @param  cmd —— BH1750工作模式指令（在BH1750_MODE中枚举定义）
 * @retval 成功返回HAL_OK
 */
static uint8_t BH1750_Send_Cmd(BH1750_MODE cmd)
{
    return HAL_I2C_Master_Transmit(&BH1750_I2Cx, BH1750_ADDR, (uint8_t *)&cmd, 1, 0xFFFF);
}

/**
 * @brief  从BH1750接收一次光强数据
 * @param  dat —— 存储光照强度的地址（两个字节数组）
 * @retval 成功 —— 返回HAL_OK
 */
static uint8_t BH1750_Read_Dat(uint8_t *dat)
{
    return HAL_I2C_Master_Receive(&BH1750_I2Cx, BH1750_ADDR, dat, 2, 0xFFFF);
}

/**
 * @brief  将BH1750的两个字节数据转换为光照强度值（0-65535）
 * @param  dat  —— 存储光照强度的地址（两个字节数组）
 * @retval 成功 —— 返回光照强度值
 */
static uint16_t BH1750_Dat_To_Lux(uint8_t *dat)
{
    uint16_t lux = 0;
    lux = dat[0];
    lux <<= 8;
    lux |= dat[1];
    lux = (int)(lux / 1.2);

    return lux;
}

/**
 * @brief  返回光照强度值
 * @param  无
 * @retval 成功 —— 返回光照强度值
 */
uint16_t Get_BH1750_Value(void)
{
    uint8_t dat[2] = {0}; // dat[0]是高字节，dat[1]是低字节
    uint16_t lux;

    if (HAL_OK != BH1750_Send_Cmd(ONCE_H_MODE))
    {
        return 0;
    }
    HAL_Delay(120);
    if (HAL_OK != BH1750_Read_Dat(dat))
    {
        return 0;
    }

    lux = BH1750_Dat_To_Lux(dat);
    return lux;
}

void bh1750_config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /* I2C2 clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();

    hi2c2.Instance = I2C2;
    hi2c2.Init.ClockSpeed = 400000;
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c2) != HAL_OK)
    {
        Error_Handler();
    }
}
