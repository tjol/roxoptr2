/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef R_MENU_H_
#define R_MENU_H_

#include <stdbool.h>
#include "main.h"

typedef struct menu Menu;
typedef void (*Menuaction_f)(void*);

struct menu {
    char *text;
    
    Menu *up;
    Menu *down;
    
    Menuaction_f exec;
    void *exec_arg;
};

extern Menu *menu_root;
extern Menu *menu_current;

void init_menu();
void free_menu(bool protect_root);

void menu_escape();

void menu_tic();

void credits_page(void *);

#endif

