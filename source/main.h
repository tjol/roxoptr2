/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef R_MAIN_H
#define R_MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#ifdef WII
#   include <gccore.h>
#   include <wiiuse/wpad.h>
#endif 

#include "level.h"
#include "display.h"
#include "game.h"
#include "menu.h"

#define ITER_MIN_TICKS 15
extern time_t iter_ticks;

typedef struct {
    Level *current_level;
    
    unsigned int xpos;
    unsigned int ypos;
    
    unsigned int heli_xpos;
    unsigned int heli_ypos;
    
    int running;
    int in_menu;
} gamestate_t;

extern gamestate_t thegame;

void sleep_for(time_t);

#endif

