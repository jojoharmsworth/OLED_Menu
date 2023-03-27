#ifndef __KEY_H
#define __KEY_H
#include "stm32f1xx.h"

#define KEY_GPIO_PORT GPIOE
#define KEY0_Pin GPIO_PIN_3
#define KEY1_Pin GPIO_PIN_4
/*--按键标签--*/
#define KEY_NUM 2
#define KEY_NUM_MAX 3
#define KEY0_DOWN 0
#define KEY1_UP 1

#define HIGH_LEVEL 1
#define LOW_LEVEL 0

/*单位：0.1 ms*/
#define MIN_PRESS_TIME 200
#define MAX_PRESS_TIME 5000
#define RELEASE_TIME 3000

#define MY_BUTTON_CALL(func, argv) \
    do                             \
    {                              \
        if ((func) != NULL)        \
            func argv;             \
    } while (0)

typedef void (*callbakFunction)(void *);

typedef struct
{
    uint8_t val;
    uint8_t last_val;
} KEY_T;

typedef struct
{
    uint8_t id;
    uint8_t press;
    uint8_t update_flag;
    uint8_t res;
} KEY_MSG;

enum key_event
{
    NONE = 0,
    CLICK,
    DOUBLE_CLICK,
    LONG_CLICK,
};

struct my_key
{
    uint8_t id;            /*按键id*/
    uint8_t val;           /*当前按键状态*/
    uint8_t pre_val;       /*上一次按键状态*/
    uint8_t cnt;           /*按下计数，判断单双击*/
    uint32_t press_time;   /*按下时间*/
    uint32_t release_time; /*释放时间*/
    callbakFunction cb;    /*回调函数*/
    enum key_event event;  /*按键活动*/
};

extern KEY_MSG key_msg;

void key_gpio_init(void);
uint8_t getKeyValue(uint8_t num);
void keyScan(void);
void key_scan_v2(void);
void defaultFunction(void);
int my_button_register(struct my_key *key);
void callbackFunction(void (*pFunction)());

#endif
