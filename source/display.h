/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef R_DISPLAY_H_
#define R_DISPLAY_H_

#define SCREEN_W 640
#define SCREEN_H 480
#define SCREEN_DEPTH 32

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

void init_SDL();
void paint_game();
void paint_menu();
void paint_banner(char *text1, char *text2, int r2, int g2, int b2, int delay);
SDL_Surface *img_from_mem(void *mem, int size, int alpha);

static inline SDL_Surface *img_from_file (const char *fname)
{
  /* IMG_Load doesn't exist on the Wii.
   * the second approach causes problems on Windows/VC++, for
   * some reason, but works file on UNIX */
#  ifndef WII
    return IMG_Load(fname);
#  else
    FILE *fp;
    SDL_RWops *rw;
    SDL_Surface *ret;

    fp = fopen(fname, "rb");
    if (!fp) {
	perror(0);
	return NULL;
    }
    rw = SDL_RWFromFP(fp, 1);
    ret = IMG_Load_RW(rw, 0);
    SDL_FreeRW(rw);
    fclose(fp);
    return ret;
#  endif
}

extern TTF_Font *menu_font;

#endif

