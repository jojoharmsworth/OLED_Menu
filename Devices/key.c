/*
 * @version: V1.0.0
 * @Author: harmsworth
 * @Date: 2023-03-22 13:56:26
 * @LastEditors: harmsworth
 * @LastEditTime: 2023-03-24 14:49:39
 * @company: null
 * @Mailbox: jojoharmsworth@gmail.com
 * @FilePath: \MDK-ARMg:\Program\Stm32\Module\UI\OLED_Menu_u8g2\Devices\key.c
 * @Descripttion:
 */
/*
 * @version: V1.0.0
 * @Author: harmsworth
 * @Date: 2023-03-22 13:56:26
 * @LastEditors: harmsworth
 * @LastEditTime: 2023-03-23 18:49:18
 * @company: null
 * @Mailbox: jojoharmsworth@gmail.com
 * @FilePath: \MDK-ARMg:\Program\Stm32\Module\UI\OLED_Menu_u8g2\Devices\key.c
 * @Descripttion:
 */
/*
 * @version: V1.0.0
 * @Author: harmsworth
 * @Date: 2023-03-22 13:56:26
 * @LastEditors: harmsworth
 * @LastEditTime: 2023-03-23 13:45:30
 * @company: null
 * @Mailbox: jojoharmsworth@gmail.com
 * @FilePath: \MDK-ARMg:\Program\Stm32\Module\UI\OLED_Menu_u8g2\Devices\key.c
 * @Descripttion:
 */
#include "key.h"
#include "tim.h"
#include "usart.h"

struct my_key_manage
{
    uint8_t num;                          /*已注册的按键数目*/
    struct my_key *key_list[KEY_NUM_MAX]; /*存储按键的指针数组*/
};

static struct my_key_manage key_manage;
extern KEY_MSG key_msg = {0};
KEY_T key[2] = {0};

void key_gpio_init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();

    /*Configure GPIO pins : PEPin PEPin */
    GPIO_InitStruct.Pin = KEY0_Pin | KEY1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStruct);
}

uint8_t getKeyValue(uint8_t num)
{
    switch (num)
    {
    // TODO:标签反了KEY
    case KEY0_DOWN:
        return HAL_GPIO_ReadPin(GPIOE, KEY1_Pin);
    case KEY1_UP:
        return HAL_GPIO_ReadPin(GPIOE, KEY0_Pin);
    }
}

void keyScan(void)
{
    int i; // i -> 按键个数 记录key状态
    for (i = 0; i < 2; i++)
    {
        key[i].val = getKeyValue(i);
        if (key[i].val != key[i].last_val)
        {
            key[i].last_val = key[i].val;
            if (key[i].val == 0)
            {
                key_msg.id = i;
                key_msg.press = 1;
                key_msg.update_flag = 1;
            }
        }
    }
}


int my_button_register(struct my_key *key)
{
    /* 初始化按键结构体 */
    key->cnt = 0;
    key->event = NONE;

    /* 添加按键到管理列表 */
    key_manage.key_list[key_manage.num++] = key;

    return 0;
}

/**
 * @brief: 按键扫描 0.1ms 1计数
 *          单击
 *          双击
 *          长按
 * @retval:
 */
void key_scan_v2(void)
{
    int i; // i -> 按键个数 记录key状态
    for (i = 0; i < key_manage.num; i++)
    {
        key_manage.key_list[i]->val;
        key_manage.key_list[i]->val = getKeyValue(i);
        if (key_manage.key_list[i]->val != key_manage.key_list[i]->pre_val) // 电平跳变
        {
            HAL_Delay(10);
            key_manage.key_list[i]->pre_val = key_manage.key_list[i]->val;
            if (key_manage.key_list[i]->val == LOW_LEVEL) // 下降沿 按键按下
            {
                key_manage.key_list[i]->id = i;
                if (key_manage.key_list[i]->release_time > RELEASE_TIME)
                    key_manage.key_list[i]->cnt = 0;

                // 初始化定时器, 计时按下时间
                tim_init(&htim6);
                __HAL_TIM_ENABLE(&htim6);
            }
            else if (key_manage.key_list[i]->val == HIGH_LEVEL) // 上升沿 按键释放
            {
                if (key_manage.key_list[i]->press_time > MIN_PRESS_TIME && key_manage.key_list[i]->press_time < MAX_PRESS_TIME || key_manage.key_list[i]->cnt == 1)
                    key_manage.key_list[i]->cnt++;

                // 初始定时器,计时松开电平时间
                tim_init(&htim6);
                __HAL_TIM_ENABLE(&htim6);
            }
        }
        else if (key_manage.key_list[i]->val == key_manage.key_list[i]->pre_val) // 电平不变
        {
            if (key_manage.key_list[i]->val == LOW_LEVEL) // 保持低电平 按键按下保持
            {
                key_manage.key_list[i]->press_time = __HAL_TIM_GET_COUNTER(&htim6);

                if (key_manage.key_list[i]->press_time > MAX_PRESS_TIME && key_manage.key_list[i]->cnt == 0)
                {
                    tim_init(&htim6);
                    key_manage.key_list[i]->cnt = 0;
                    key_manage.key_list[i]->event = LONG_CLICK;
                    MY_BUTTON_CALL(key_manage.key_list[i]->cb, (key_manage.key_list[i]));
                }
            }
            else if (key_manage.key_list[i]->val == HIGH_LEVEL) // 保持高电平 按键释放
            {
                key_manage.key_list[i]->release_time = __HAL_TIM_GET_COUNTER(&htim6);

                if (key_manage.key_list[i]->cnt == 1 && key_manage.key_list[i]->release_time > RELEASE_TIME)
                {
                    tim_init(&htim6);
                    key_manage.key_list[i]->cnt = 0;
                    key_manage.key_list[i]->event = CLICK;
                    MY_BUTTON_CALL(key_manage.key_list[i]->cb, (key_manage.key_list[i]));
                }
                else if (key_manage.key_list[i]->cnt == 2)
                {
                    tim_init(&htim6);
                    key_manage.key_list[i]->cnt = 0;
                    key_manage.key_list[i]->event = DOUBLE_CLICK;
                    MY_BUTTON_CALL(key_manage.key_list[i]->cb, (key_manage.key_list[i]));
                }
            }
        }
    }
}
