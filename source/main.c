#include "main.h"

gamestate_t thegame;
time_t iter_ticks = ITER_MIN_TICKS;

static time_t sleep_until_ = 0;

void poll_events();

int main (int argc, char **argv)
{
    time_t iter_start, delta; /* times */
    
    init_menu();
    init_levels();
    init_SDL();
    
    thegame.running = 1;
    thegame.in_menu = 1;
    
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
		    thegame.running = 1;
		    thegame.in_menu = 1;
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
		    case SDLK_ESCAPE:	thegame.in_menu = 1;		break;
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
    if (held & WPAD_BUTTON_A)		controls_held |= CTRL_ENTER;
    
    if (held & WPAD_BUTTON_PLUS)	thegame.in_menu = 1;
    
    if (held & WPAD_BUTTON_HOME) {
	thegame.running = 0;
    }

    
  
# endif
}

void sleep_for(time_t ms)
{
    sleep_until_ = SDL_GetTicks() + ms;
}
