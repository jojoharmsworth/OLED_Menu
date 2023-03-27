/*
 * @version: V1.0.0
 * @Author: harmsworth
 * @Date: 2023-03-22 09:44:05
 * @LastEditors: harmsworth
 * @LastEditTime: 2023-03-24 15:40:20
 * @company: null
 * @Mailbox: jojoharmsworth@gmail.com
 * @FilePath: \MDK-ARMg:\Program\Stm32\Module\UI\OLED_Menu_u8g2\UI\directory.c
 * @Descripttion:
 */
#include "directory.h"
#include "usart.h"

/**
 * @brief: 添加同级菜单目录, 没有会自己创建一个
 * @retval: 返回添加的菜单指针
 * @param menu_t *item 当前页面条目指针头节点
 * @param void *fun 函数
 * @param char *str 添加条目的名称
 */
menu_t *add_sibling_item(menu_t *item, void *fun, char *str)
{
    menu_t *p = NULL;
    menu_t *pr = item;
    p = (menu_t *)malloc(sizeof(menu_t));
    if (p == NULL)
    {
        usart_print(&huart1, "No Enough Momery to Allocate!\n");
        exit(0);
    }

    if (item == NULL)
    {
        item = p;
        usart_print(&huart1, "创建目录头节点\n");
    }
    else
    {
        int i = 1;
        while (pr->sibling_next != NULL) // 找同级目录最后的表
        {
            i++;
            pr = pr->sibling_next;
        }

        pr->sibling_next = p; // 在同级目录尾部创建一个表
        p->sibling_last = pr;
        // FIXME:父级指针
        p->parent = item;
        p->item_id = i;
    }

    p->itemFunction = fun;
    p->item_name = str;
    p->sibling_next = NULL; // 下一个条目指针 NULL
    p->child = NULL;        // 子级条目指针 NULL
    return p;
}

/**
 * @brief: 添加子级菜单条目头指针
 * @retval: 该头节点指针
 *          添加的子级菜单指针
 * @param menu_t *item 当前页面条目的头指针
 * @param int num 要添加子级条目的位置
 *                子级id 默认0
 */
menu_t *add_child_item(menu_t *item, char *str, void *fun, int num)
{
    // 跳转到需要添加子菜单的条目位置
    for (int i = 0; i < num; i++)
    {
        if (item->sibling_next != NULL)
            item = item->sibling_next;
        else
            usart_print(&huart1, "超出当前条目编号!\r\n");
    }

    // 添加子级条目
    if (item->child != NULL)
        item = item->child;
    else
    {
        usart_print(&huart1, "创建子菜单\r\n");
        menu_t *p = NULL;
        p = (menu_t *)malloc(sizeof(menu_t));
        if (p == NULL)
        {
            usart_print(&huart1, "No Enough Momery to Allocate!\r\n");
            exit(0);
        }

        item->child = p;
        p->parent = item;
        p->item_id = 0; // 子级菜单第一条目
        p->item_name = str;
        p->itemFunction = fun;
        p->sibling_last = NULL;
        p->sibling_next = NULL;
        return p;
    }
    return item;
}

/**
 * @brief: 设置函数和功能
 * @retval:
 * @param menu_t *menu 菜单条目
 * @param void *fun 条目函数
 * @param char *str 条目名称
 */
void set_fun(menu_t *menu, void *fun, char *str)
{
    menu->itemFunction = fun;
    menu->item_name = str;
}
