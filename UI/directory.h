#ifndef __DIRECTORY_H
#define __DIRECTORY_H
#include "stm32f1xx.h"

#include <stdlib.h>

typedef void (*item_function)(void);

typedef struct _item
{
    int item_id;                // 当前条目号
    char *item_name;            // 当前条目名称
    item_function itemFunction; // 功能
    struct _item *parent;       // 父级菜单指针
    struct _item *child;        // 子级菜单指针
    struct _item *sibling_last; // 同级菜单下一项指针
    struct _item *sibling_next; // 同级菜单上一项指针

} menu_t;

menu_t *add_sibling_item(menu_t *item, void *fun, char *str);
menu_t *add_child_item(menu_t *item, char *str, void *fun, int num);
void set_fun(menu_t *menu, void *fun, char *str);

#endif