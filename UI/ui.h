#ifndef __UI_H
#define __UI_H
#include "stm32f1xx.h"

#include <stdio.h>
#include "i2c.h"
#include "u8g2.h"
#include "u8x8.h"
#include "directory.h"

extern uint8_t temp;
extern uint8_t humi;

typedef struct
{
    short x;             // x位置
    short y;             // y位置
    short frame_len;     // frame_len 框的宽度
    short frame_len_trg; // frame_len_trg框的目标宽度
    short frame_y;       // frame_y 框的y位置
    short frame_y_trg;   // frame_y_trg 框的目标y位置
}frame;

int uiRun(short *a, short *a_trg, uint8_t step, uint8_t slow_cnt);
int uiEffectChangeSpeed(short *a, short *a_trg, uint8_t step, uint8_t slow_cnt, uint8_t fast_cnt);
void menu_oled(menu_t *p);

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_byte_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif