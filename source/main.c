#include "main.h"

gamestate_t thegame;
time_t iter_ticks = ITER_MIN_TICKS;

extern Level *load_lv0();

void poll_events();

int main (int argc, char **argv)
{
    time_t iter_start, delta; /* times */
    
    init_SDL();
    
    thegame.xpos = thegame.ypos = 0;
    thegame.current_level = load_lv0();
    thegame.heli_xpos = thegame.current_level->heli_x0;
    thegame.heli_ypos = thegame.current_level->heli_y0;
    thegame.running = 1;
    thegame.in_menu = 0;
    
    while (thegame.running) {
	iter_start = SDL_GetTicks();
	
	poll_events();
	
	/* do stuff */
	game_tic();
	
	if(thegame.running) paint_game();
	
	delta = SDL_GetTicks() - iter_start;
	if (delta < ITER_MIN_TICKS) {
	    SDL_Delay(ITER_MIN_TICKS - delta);
	}
	
	iter_ticks = SDL_GetTicks() - iter_start;
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
		};
		break;;
	    case SDL_KEYUP:
		switch (event.key.keysym.sym) {
		    case SDLK_UP:	controls_held &=~DIR_UP;	break;
		    case SDLK_DOWN:	controls_held &=~DIR_DOWN;	break;
		    case SDLK_LEFT:	controls_held &=~DIR_LEFT;	break;
		    case SDLK_RIGHT:	controls_held &=~DIR_RIGHT;	break;
		};
		break;;
	    case SDL_QUIT:
		thegame.running = 0;
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
    
    if (held & WPAD_BUTTON_HOME) {
	thegame.running = 0;
    }
    
  
# endif
}
