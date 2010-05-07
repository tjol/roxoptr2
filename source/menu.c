/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "menu.h"

#include "img/credits.png.h"

Menu *menu_root;
Menu *menu_current;

static bool menu_painted = false;

void menu_tic()
{
    static directional_t old_controls = 0;
 
    if (!menu_current) return; /* NULL menu */

    if (!menu_painted) {
	paint_menu();
	menu_painted = true;
    }
    
    if (controls_held != old_controls) {
	old_controls = controls_held;
	menu_painted = false;
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
	thegame.running = true;
	thegame.in_menu = false;
    }
}

static void quitgame(void *x)
{
    thegame.running = false;
}

static void levelmenu(void *);
static void menu_up(void *);

void free_root_menu()
{
    menu_up(0);
    free_menu(false);
}

void menu_escape()
{
    if (!thegame.in_menu) {
	thegame.in_menu = true;
    } else {
	menu_up(0);
    }
    menu_painted = false;
}

void init_menu()
{
    Menu *mstart, *mlevels, *mcredits, *mquit;
    
    mstart = malloc(sizeof(Menu));
    mlevels = malloc(sizeof(Menu));
    mcredits = malloc(sizeof(Menu));
    mquit  = malloc(sizeof(Menu));
    
    mstart->text = "Start Game";
    mstart->up   = mstart;
    mstart->down = mlevels;
    mstart->exec = &startgame;
    
    mlevels->text = "Select Level";
    mlevels->up   = mstart;
    mlevels->down = mcredits;
    mlevels->exec = &levelmenu;

    mcredits->text = "Credits";
    mcredits->up = mlevels;
    mcredits->down = mquit;
    mcredits->exec = &credits_page;
    
    mquit->text = "Quit";
    mquit->up   = mlevels;
    mquit->down = mquit;
    mquit->exec = &quitgame;
    
    menu_root = mstart;
    menu_current = menu_root;
    
    atexit(free_root_menu);
}

void free_menu(bool protect_root)
{
    Menu *m = menu_current;
    Menu *next = NULL;
        
    /* NULL menu */
    if (!m) return;
    /* rewind */
    for (; m != m->up; m = m->up);
    if (m == menu_root && protect_root) return;
    
    /* delete the lot */
    while (true) {
	next = m->down;
	if (next == m) break;
	free(m);
	m = next;
    }
    
    menu_current = NULL;
}

static void menu_up(void *x)
{
    free_menu(true);
    menu_current = menu_root;
}

static void menu_start_level(void *ll_)
{
    LevelList *ll;
    
    ll = (LevelList *) ll_;
    
    if (start_level(ll)) {
	thegame.running = true;
	thegame.in_menu = false;
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

void
credits_page(void *x)
{
    SDL_Surface *credits_s = img_from_mem(credits_png, credits_png_len, false);
    menu_current = NULL;
    paint_pixels(credits_s, 2000);
}

