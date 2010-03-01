#include "main.h"

static SDL_Surface *screen = NULL;
static SDL_Surface *heli = NULL;

SDL_Surface *win_banner = NULL;
SDL_Surface *fail_banner = NULL;
TTF_Font *menu_font = NULL;

#include "img/heli.png.h"
#include "img/win.gif.h"
#include "img/fail.gif.h"
#include "data/DejaVuSans-Bold.ttf.h"

static void quit()
{
    if (heli) {
	SDL_FreeSurface(heli);
	SDL_FreeSurface(win_banner);
	SDL_FreeSurface(fail_banner);
    }
    if (thegame.current_level) {
	thegame.current_level->del(thegame.current_level);
    }
    if(menu_font) {
	TTF_CloseFont(menu_font);
	menu_font = NULL;
    }
    TTF_Quit();
    SDL_Quit();
}

SDL_Surface *img_from_mem(void *mem, int size, int alpha)
{
    SDL_Surface *load, *ret;
    SDL_RWops *rw;
    
    rw = SDL_RWFromMem(mem, size);
    load = IMG_Load_RW(rw, 0);
    if (alpha) {
	ret = SDL_DisplayFormatAlpha(load);
    } else {
	ret = SDL_DisplayFormat(load);
    }
    SDL_FreeRW(rw);
    SDL_FreeSurface(load);
    
    return ret;
}

void init_SDL()
{    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
	fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
	exit(1);
    }
    
    atexit(quit);
    
# ifdef WII
    WPAD_Init();
    SDL_Delay(2000);
    SDL_ShowCursor(SDL_DISABLE);
# endif

    if (TTF_Init() != 0) {
	fprintf(stderr, "Unable to initialize TTF subsystem: %s\n", TTF_GetError());
	exit(1);
    }
    menu_font = TTF_OpenFontRW(SDL_RWFromMem(DejaVuSans_Bold_ttf,
					     DejaVuSans_Bold_ttf_len),
			       1, 16);
    if (!menu_font) {
	fprintf(stderr, "Unable to load menu font: %s\n", TTF_GetError());
	exit(1);
    }
    
    screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, SCREEN_DEPTH, SDL_DOUBLEBUF | SDL_HWSURFACE);
    if (!screen) {
	fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
	exit(1);
    }
    
    heli = img_from_mem(heli_png, heli_png_len, 1);
    win_banner = img_from_mem(win_gif, win_gif_len, 0);
    fail_banner = img_from_mem(fail_gif, fail_gif_len, 0);
}

void paint_game()
{
    SDL_Rect bg_rect;
    SDL_Rect heli_dest;
    
    bg_rect.x = thegame.xpos;
    bg_rect.y = thegame.ypos;
    bg_rect.w = SCREEN_W;
    bg_rect.h = SCREEN_H;
    
    heli_dest.x = thegame.heli_xpos - thegame.xpos;
    heli_dest.y = thegame.heli_ypos - thegame.ypos;
    heli_dest.w = HELI_W;
    heli_dest.h = HELI_H;
    
    SDL_BlitSurface(thegame.current_level->bg, &bg_rect, screen, NULL);
    SDL_BlitSurface(heli, NULL, screen, &heli_dest);
    
    SDL_Flip(screen);
}

void paint_banner(SDL_Surface *banner, int duration)
{
    SDL_BlitSurface(banner, NULL, screen, NULL);
    SDL_Flip(screen);
    SDL_Delay(duration);
}
