#ifndef __BH1750_H
#define __BH1750_H
#include "stm32f1xx.h"

// ALT  ADDRESS引脚接地时地址为0x46，接电源时地址为0xB8
/*bh1750地址*/
#define    BH1750_ADDR  0x46   //01000110
#define BH1750_I2Cx hi2c2

typedef enum
{
    POWER_OFF_CMD = 0x00,  // 断电：无激活状态
    POWER_ON_CMD = 0x01,   // 通电：等待测量指令
    RESET_REGISTER = 0x07, // 重置数字寄存器（在断电状态下不起作用）
    CONT_H_MODE = 0x10,    // 连续H分辨率模式：在11x分辨率下开始测量，测量时间120ms
    CONT_H_MODE2 = 0x11,   // 连续H分辨率模式2：在0.51x分辨率下开始测量，测量时间120ms
    CONT_L_MODE = 0x13,    // 连续L分辨率模式：在411分辨率下开始测量，测量时间16ms
    ONCE_H_MODE = 0x20,    // 一次高分辨率模式：在11x分辨率下开始测量，测量时间120ms，测量后自动设置为断电模式
    ONCE_H_MODE2 = 0x21,   // 一次高分辨率模式2：在0.51x分辨率下开始测量，测量时间120ms，测量后自动设置为断电模式
    ONCE_L_MODE = 0x23     // 一次低分辨率模式：在411x分辨率下开始测量，测量时间16ms，测量后自动设置为断电模式
} BH1750_MODE;

uint16_t Get_BH1750_Value(void);
void bh1750_config(void);

#endif
