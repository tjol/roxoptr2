/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "menu.h"

Menu *menu_root;
Menu *menu_current;

void menu_tic()
{
    static int painted = 0;
    static directional_t old_controls = 0;
 
    if (!painted) {
	paint_menu();
	painted = 1;
    }
    
    if (controls_held != old_controls) {
	old_controls = controls_held;
	painted = 0;
	if (controls_held & DIR_UP) {
	    menu_current = menu_current->up;
	}
	if (controls_held & DIR_DOWN) {
	    menu_current = menu_current->down;
	}
	if (controls_held & CTRL_ENTER) {
	    menu_current->exec(menu_current->exec_arg);
	}
    }
}

static void startgame(void *x)
{
    if (start_level(current_level)) {
	thegame.running = 1;
	thegame.in_menu = 0;
    }
}

static void quitgame(void *x)
{
    thegame.running = 0;
}

static void levelmenu(void *);

void init_menu()
{
    Menu *mstart, *mlevels, *mquit;
    
    mstart = malloc(sizeof(Menu));
    mlevels = malloc(sizeof(Menu));
    mquit  = malloc(sizeof(Menu));
    
    mstart->text = "Start Game";
    mstart->up   = mstart;
    mstart->down = mlevels;
    mstart->exec = &startgame;
    
    mlevels->text = "Select Level";
    mlevels->up   = mstart;
    mlevels->down = mquit;
    mlevels->exec = &levelmenu;
    
    mquit->text = "Quit";
    mquit->up   = mlevels;
    mquit->down = mquit;
    mquit->exec = &quitgame;
    
    menu_root = mstart;
    menu_current = menu_root;
    
    atexit(free_menu);
}

void free_menu()
{
    Menu *m = menu_current;
    Menu *next = NULL;
        
    /* rewind */
    for (; m != m->up; m = m->up);
    
    /* delete the lot */
    while (1) {
	next = m->down;
	if (next == m) break;
	free(m);
	m = next;
    }
    
    menu_current = NULL;
}

static void menu_up(void *x)
{
    free_menu();
    menu_current = menu_root;
}

static void menu_start_level(void *ll_)
{
    LevelList *ll;
    
    ll = (LevelList *) ll_;
    
    if (start_level(ll)) {
	thegame.running = 1;
	thegame.in_menu = 0;
    }
}

static void levelmenu(void *x)
{
    Menu *mtop, *mprev, *m;
    LevelList *ll = levels;
    
    mtop = malloc(sizeof(Menu));
    mtop->text = "Back";
    mtop->up = mtop;
    mtop->down = mtop;
    mtop->exec = &menu_up;
    
    mprev = mtop;
    
    while (ll) {
	m = malloc(sizeof(Menu));
	m->text = ll->title;
	m->up = mprev;
	m->down = mprev->down = m;
	m->exec = &menu_start_level;
	m->exec_arg = (void *) ll;
	
	ll = ll->next;
	mprev = m;
    }
    
    menu_current = mtop;
}
