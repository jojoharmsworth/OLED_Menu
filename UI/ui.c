#include "ui.h"
#include <stdio.h>

static frame rframe = {.x = 0, .y = 15,.frame_y = 0};
extern u8g2_t u8g2;

/**
 * @brief: 过渡动画
 * @retval: 1：完成
 *          0：失败
 * @param short *a 当前位置
 * @param short *a_trg 目标位置
 * @param uint8_t step 控制位置变化速度
 * @param uint8_t slow_cnt 阈值,超过阈值减速运动
 */
int uiRun(short *a, short *a_trg, uint8_t step, uint8_t slow_cnt)
{
    uint8_t temp;

    temp = abs(*a - 0) > slow_cnt ? step : 1;
    if (*a < *a_trg)
    {
        *a += temp;
    }
    else if (*a > *a_trg)
    {
        *a -= temp;
    }
    else
    {
        return 0;
    }
    return 1;
}

/**
 * @brief: 加速进入，减速停止动画
 * @retval:
 * @param short *a
 * @param short *a_trg
 * @param uint8_t step
 * @param uint8_t slow_cnt
 * @param uint8_t fast_cnt
 */
int uiEffectChangeSpeed(short *a, short *a_trg, uint8_t step, uint8_t slow_cnt, uint8_t fast_cnt)
{
    uint8_t temp;

    // temp = abs(*a_trg - *a) > slow_cnt ? step : 1;
    if (abs(*a_trg - *a) > fast_cnt)
        temp = 10;
    else if (abs(*a_trg - *a) < slow_cnt)
        temp = 1;
    else
        temp = step;

    if (*a < *a_trg)
        *a += temp;
    else if (*a > *a_trg)
        *a -= temp;
    else
        return 0;
    return 1;
}


void menu_oled(menu_t *p)
{
    menu_t *pHead = p;

    while (pHead->sibling_last != NULL)
    {
        pHead = pHead->sibling_last;
    }

    rframe.frame_y_trg = p->item_id * 14;
    rframe.frame_len_trg = strlen(p->item_name) * 10;

    u8g2_SetFont(&u8g2, u8g2_font_t0_16_tf);
    u8g2_ClearBuffer(&u8g2); // 清除内部缓冲区

    while (pHead != NULL)
    {
        u8g2_DrawStr(&u8g2, rframe.x + 2, rframe.y + pHead->item_id * 14, pHead->item_name);
        pHead = pHead->sibling_next;
    }
    u8g2_DrawRFrame(&u8g2, rframe.x, rframe.frame_y + 2, rframe.frame_len, 16, 3);

    uiEffectChangeSpeed(&rframe.frame_y, &rframe.frame_y_trg, 2, 5, 5);
    uiEffectChangeSpeed(&rframe.frame_len, &rframe.frame_len_trg, 2, 5, 5);
    u8g2_SendBuffer(&u8g2);
}

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch (msg)
  {
  case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
    __NOP();
    break;
  case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
    Delay_us(10);
    break;
  case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
    HAL_Delay(1);
    break;
  case U8X8_MSG_DELAY_I2C:      // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
    break;                      // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
  case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
    if (arg_int = 0)
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
    else if (arg_int = 1)
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
    break;                     // arg_int=1: Input dir with pullup high for I2C clock pin
  case U8X8_MSG_GPIO_I2C_DATA: // arg_int=0: Output low at I2C data pin
    if (arg_int == 1)          // arg_int=1: Input dir with pullup high for I2C data pin
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    else if (arg_int == 0)
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
    break;
  case U8X8_MSG_GPIO_MENU_SELECT:
    u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
    break;
  case U8X8_MSG_GPIO_MENU_NEXT:
    u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
    break;
  case U8X8_MSG_GPIO_MENU_PREV:
    u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
    break;
  case U8X8_MSG_GPIO_MENU_HOME:
    u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
    break;
  default:
    u8x8_SetGPIOResult(u8x8, 1); // default return value
    break;
  }
  return 1;
}

uint8_t u8x8_byte_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  static uint8_t buffer[32]; /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
  static uint8_t buf_idx;
  uint8_t *data;

  switch (msg)
  {
  case U8X8_MSG_BYTE_SEND:
    data = (uint8_t *)arg_ptr;
    while (arg_int > 0)
    {
      buffer[buf_idx++] = *data;
      data++;
      arg_int--;
    }
    break;
  case U8X8_MSG_BYTE_INIT:
    /* add your custom code to init i2c subsystem */
    break;
  case U8X8_MSG_BYTE_START_TRANSFER:
    buf_idx = 0;
    break;
  case U8X8_MSG_BYTE_END_TRANSFER:
    HAL_I2C_Master_Transmit(&hi2c1, u8x8_GetI2CAddress(u8x8), buffer, buf_idx, 1000);
    // while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY);
    // HAL_I2C_Master_Transmit_DMA(&hi2c1, u8x8_GetI2CAddress(u8x8), buffer, buf_idx);
    break;
  default:
    return 0;
  }
  return 1;
}
