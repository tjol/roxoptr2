/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*! \file */

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
extern Uint32 iter_ticks;

typedef struct {
    Level *current_level;
    
    int xpos;
    int ypos;
    
    int heli_xpos;
    int heli_ypos;
    
    bool running;
    bool in_menu;

    Sprite *main_sprite;
} gamestate_t;

/*! \brief State of the game. */
extern gamestate_t thegame;

/*! \brief Pause the game.
 *
 * This function instructs the main loop to sleep and returns immediately.
 * Useful for displaying something meant to be read.
 *
 * \param tics	Time (in ms) to sleep.
 * \see paint_banner()
 */
void sleep_for(Uint32 tics);

#endif

