/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <zlib.h>
#include "main.h"
#include "sprite.h"

static SDL_Surface *screen = NULL;

TTF_Font *menu_font = NULL;
TTF_Font *small_font = NULL;
TTF_Font *huge_font = NULL;

#include "data/DejaVuSans-Bold.ttf.h"

static void quit()
{
    if (main_sprite) {
	main_sprite->free_data(main_sprite);
	free(main_sprite);
	main_sprite = NULL;
    }
    if(menu_font) {
	TTF_CloseFont(menu_font);
	TTF_CloseFont(small_font);
	TTF_CloseFont(huge_font);
	menu_font = NULL;
	small_font = NULL;
	huge_font = NULL;
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
    gzFile icon_file;
    unsigned char *icon_buf;
    unsigned int bufsize = 4096;
    unsigned int len = 0;
    unsigned int d;
    SDL_Surface *icon;
    SDL_RWops *icon_rw;

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
    
    menu_font = TTF_OpenFontRW(SDL_RWFromConstMem(DejaVuSans_Bold_ttf,
						  DejaVuSans_Bold_ttf_len),
			       1, 40);
    small_font= TTF_OpenFontRW(SDL_RWFromConstMem(DejaVuSans_Bold_ttf,
						  DejaVuSans_Bold_ttf_len),
			       1, 24);
    huge_font = TTF_OpenFontRW(SDL_RWFromConstMem(DejaVuSans_Bold_ttf,
						  DejaVuSans_Bold_ttf_len),
			       1, 200);
    
    if (!(menu_font && small_font && huge_font)) {
	fprintf(stderr, "Unable to load font: %s\n", TTF_GetError());
	exit(1);
    }

# ifndef WII
    SDL_WM_SetCaption("roxoptr2", "roxoptr2");

    icon_buf = malloc(bufsize);
    icon_file = gzopen("icon.bmp.gz", "rb");
    while (d = gzread(icon_file, icon_buf+len, bufsize-len)) {
	len += d;
	if (len == bufsize) {
	    bufsize += 4096;
	    icon_buf = realloc(icon_buf, bufsize);
	}
    }
    icon_rw = SDL_RWFromMem(icon_buf, len);
    icon = SDL_LoadBMP_RW(icon_rw, 0);
    SDL_FreeRW(icon_rw);
    gzclose(icon_file);
    free(icon_buf);
    SDL_SetColorKey(icon, SDL_SRCCOLORKEY, SDL_MapRGB(icon->format, 0xff, 0xff, 0x00));
    SDL_WM_SetIcon(icon, NULL);
# endif   
    
    screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, SCREEN_DEPTH, SDL_DOUBLEBUF | SDL_HWSURFACE);
    if (!screen) {
	fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
	exit(1);
    }

    main_sprite = load_heli();
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
    /*heli_dest.w = HELI_W;
    heli_dest.h = HELI_H;*/
    
    SDL_BlitSurface(thegame.current_level->bg, &bg_rect, screen, NULL);
    SDL_BlitSurface(main_sprite->frames[0], NULL, screen, &heli_dest);
    
    SDL_Flip(screen);
}

void paint_menu()
{
    int i;
    
    SDL_Surface *s;
    SDL_Rect dst;
    SDL_Color red = {0xff,0,0}, grey = {0x60, 0x60, 0x60}, bg = {0xff, 0xff, 0xff};
    Menu *m;
    int y = 50;
    
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));
    
    m = menu_current;
    for (i = 0; i < 3 && m != m->up; ++i) m = m->up;
    
    while (1) {
	s = TTF_RenderText_Shaded(menu_font, m->text, (m == menu_current ? red : grey), bg);
	dst.x = 320 - s->w/2;
	dst.y = y;
	y += 90;
	
	SDL_BlitSurface(s, NULL, screen, &dst);
	SDL_FreeSurface(s);
	
	if (m->down == m) break;
	
	m = m->down;
    }
    
    SDL_Flip(screen);
}

void paint_banner(char *text1, char *text2, int r2, int g2, int b2, int delay)
{
    SDL_Color bg = {0,0,0};
    SDL_Color white = {255,255,255};
    SDL_Color fg;
    SDL_Surface *s;
    SDL_Rect dst;
    
    fg.r = r2; fg.g = g2; fg.b = b2;
    
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, bg.r, bg.g, bg.b));
    
    s = TTF_RenderText_Shaded(small_font, text1, white, bg);
    dst.x = 320 - s->w/2;
    dst.y = 100;
    SDL_BlitSurface(s, NULL, screen, &dst);
    SDL_FreeSurface(s);
    
    s = TTF_RenderText_Shaded(huge_font, text2, fg, bg);
    dst.x = 320 - s->w/2;
    dst.y = 160;
    SDL_BlitSurface(s, NULL, screen, &dst);
    SDL_FreeSurface(s);
    
    SDL_Flip(screen);
    
    sleep_for(delay);
}
