#ifndef R_DISPLAY_H_
#define R_DISPLAY_H_

#define SCREEN_W 640
#define SCREEN_H 480
#define SCREEN_DEPTH 32

#define HELI_W 38
#define HELI_H 24

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

void init_SDL();
void paint_game();
void paint_banner(SDL_Surface *banner, int duration);
SDL_Surface *img_from_mem(void *mem, int size, int alpha);

extern SDL_Surface *win_banner;
extern SDL_Surface *fail_banner;
extern TTF_Font *menu_font;

#endif