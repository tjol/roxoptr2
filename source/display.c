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
#include "filesys.h"

static SDL_Surface *screen = NULL;

TTF_Font *menu_font = NULL;
TTF_Font *small_font = NULL;
TTF_Font *huge_font = NULL;

static SDL_Surface *menu_bg = NULL;
static SDL_Surface *logo = NULL;

#include "data/DejaVuSans-Bold.ttf.h"
#include "img/menu_bg.png.h"
#include "img/logo.png.h"

static void quit()
{
    if (classic_heli_sprite) {
	if (classic_heli_sprite->free_data) {
	    classic_heli_sprite->free_data(classic_heli_sprite);
	}
	free(classic_heli_sprite);
	classic_heli_sprite = NULL;
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

SDL_Surface *img_from_mem(void *mem, int size, bool alpha)
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
# endif

    SDL_ShowCursor(SDL_DISABLE);

# ifndef WII
    SDL_WM_SetCaption("roxoptr2", "roxoptr2");

    init_fs();

    unsigned int bufsize = 4096;
    unsigned char *icon_buf = malloc(bufsize);
    gzFile icon_file = gzopen("icon.bmp.gz", "rb");
    unsigned int len = 0;
    unsigned int d;

    if (icon_file) {
	while (d = gzread(icon_file, icon_buf+len, bufsize-len)) {
	    len += d;
	    if (len == bufsize) {
		bufsize += 4096;
		icon_buf = realloc(icon_buf, bufsize);
	    }
	}
	SDL_RWops *icon_rw = SDL_RWFromMem(icon_buf, len);
	SDL_Surface *icon = SDL_LoadBMP_RW(icon_rw, 0);
	SDL_FreeRW(icon_rw);
	gzclose(icon_file);
	free(icon_buf);
	SDL_SetColorKey(icon, SDL_SRCCOLORKEY, SDL_MapRGB(icon->format, 0xff, 0xff, 0xff));
	SDL_WM_SetIcon(icon, NULL);
    } else {
	fprintf(stderr, "Cannot open icon file. Are we in the right directory?\n");
    }
# endif   
    
    screen = SDL_SetVideoMode(SCREEN_W, SCREEN_H, SCREEN_DEPTH, SDL_DOUBLEBUF | SDL_HWSURFACE);
    if (!screen) {
	fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
	exit(1);
    }

    menu_bg = img_from_mem(menu_bg_png, menu_bg_png_len, false);
    if (menu_bg) {
	SDL_BlitSurface(menu_bg, NULL, screen, NULL);
    } else {
	fprintf(stderr, "unable to load menu background!\n");
	exit(1);
    }

    logo = img_from_mem(logo_png, logo_png_len, true);
    if (logo) {
	SDL_Rect r;
	r.x = 114, r.y = 172;
	SDL_BlitSurface(logo, NULL, screen, &r);
	SDL_Flip(screen);
	sleep_for(3000);
    }
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

    classic_heli_sprite = load_heli();
    /*  this is the responsability of start_level().
    thegame.main_sprite = classic_heli_sprite; */
}

void paint_game()
{
    SDL_Rect bg_rect;
    SDL_Rect heli_dest;
    Overlay *lov;
    
    bg_rect.x = thegame.xpos;
    bg_rect.y = thegame.ypos;
    bg_rect.w = SCREEN_W;
    bg_rect.h = SCREEN_H;
    
    heli_dest.x = thegame.heli_xpos - thegame.xpos;
    heli_dest.y = thegame.heli_ypos - thegame.ypos;
    
    SDL_BlitSurface(thegame.current_level->bg, &bg_rect, screen, NULL);

    for (lov = thegame.current_level->overlays; lov; lov = lov->next) {
	lov->paint(thegame.current_level, lov, screen);
    }

    animate_sprite(thegame.main_sprite, screen, &heli_dest);

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
    
    /*SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0xFF, 0xFF, 0xFF));*/
    SDL_BlitSurface(menu_bg, NULL, screen, NULL);
    
    m = menu_current;
    for (i = 0; i < 3 && m != m->up; ++i) m = m->up;
    
    while (1) {
	s = TTF_RenderText_Blended(menu_font, m->text, (m == menu_current ? red : grey));
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

void paint_banner(char *text1, char *text2, int r2, int g2, int b2, Uint32 delay)
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

void paint_pixels(SDL_Surface *s, Uint32 delay)
{
    SDL_BlitSurface(s, NULL, screen, NULL);

    SDL_Flip(screen);
    sleep_for(delay);
}



