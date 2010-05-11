/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "main.h"

gamestate_t thegame;
Uint32 iter_ticks = ITER_MIN_TICKS;

static Uint32 sleep_until_ = 0;

void poll_events();

int main (int argc, char **argv)
{
    Uint32 iter_start, delta; /* times */
    
    init_SDL();
    init_menu();
    init_levels();
    
    thegame.running = true;
    thegame.in_menu = true;
    
    while (thegame.running) {
	iter_start = SDL_GetTicks();
	
	if (iter_start >= sleep_until_) {
	    /* do stuff */
	    if (thegame.in_menu) {
		menu_tic();
	    } else {
		game_tic();
		if(thegame.running && iter_start >= sleep_until_) { /* active? */
		    paint_game();
		} else if (!thegame.running) {
		    /* level code cannot quit! */
		    thegame.running = true;
		    thegame.in_menu = true;
		}
	    }
	}
	
	delta = SDL_GetTicks() - iter_start;
	if (delta < ITER_MIN_TICKS) {
	    SDL_Delay(ITER_MIN_TICKS - delta);
	}
	
	iter_ticks = SDL_GetTicks() - iter_start;
	
	poll_events();
    }
    
    exit(0);
}


void poll_events()
{
# ifndef WII  

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
	switch (event.type) {
	    case SDL_KEYDOWN:
		switch (event.key.keysym.sym) {
		    case SDLK_UP:	controls_held |= DIR_UP;	break;
		    case SDLK_DOWN:	controls_held |= DIR_DOWN;	break;
		    case SDLK_LEFT:	controls_held |= DIR_LEFT;	break;
		    case SDLK_RIGHT:	controls_held |= DIR_RIGHT;	break;
		    case SDLK_RETURN:
		    case SDLK_SPACE:	controls_held |= CTRL_ENTER;	break;
		    case SDLK_ESCAPE:	menu_escape();			break;
		    default: break;
		};
		break;;
	    case SDL_KEYUP:
		switch (event.key.keysym.sym) {
		    case SDLK_UP:	controls_held &=~DIR_UP;	break;
		    case SDLK_DOWN:	controls_held &=~DIR_DOWN;	break;
		    case SDLK_LEFT:	controls_held &=~DIR_LEFT;	break;
		    case SDLK_RIGHT:	controls_held &=~DIR_RIGHT;	break;
		    case SDLK_RETURN:
		    case SDLK_SPACE:	controls_held &=~CTRL_ENTER;	break;
		    default: break;
		};
		break;;
	    case SDL_QUIT:
		thegame.running = false;
		break;;
	}
    }

# else
  
    WPAD_ScanPads();
    Uint32 held = WPAD_ButtonsHeld(0);
    
    controls_held = 0;
    if (held & WPAD_BUTTON_UP)		controls_held |= DIR_UP;
    if (held & WPAD_BUTTON_DOWN)	controls_held |= DIR_DOWN;
    if (held & WPAD_BUTTON_LEFT)	controls_held |= DIR_LEFT;
    if (held & WPAD_BUTTON_RIGHT)	controls_held |= DIR_RIGHT;
    if (held & WPAD_BUTTON_A)		controls_held |= CTRL_ENTER;
    
    if (held & WPAD_BUTTON_PLUS)	menu_escape();
    
    if (held & WPAD_BUTTON_HOME) {
	thegame.running = false;
    }

    
  
# endif
}

void sleep_for(Uint32 ms)
{
    sleep_until_ = SDL_GetTicks() + ms;
}
