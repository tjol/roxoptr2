/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*! \file */

#ifndef R_DISPLAY_H_
#define R_DISPLAY_H_

#define SCREEN_W 640
#define SCREEN_H 480
#define SCREEN_DEPTH 32

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <stdbool.h>

/*! \brief Initialize graphics subsystem. */
void init_SDL();

void paint_game();
void paint_menu();

/*! \brief Draw a banner and pause the game.
 * 
 * \param text1		Top line (small) to be displayed.
 * \param text2		Bottom line (large) to be displayed.
 * \param delay		Time (in ms) to sleep for.
 */
void paint_banner(char *text1, char *text2, int r2, int g2, int b2, Uint32 delay);

/*! \brief paint something and pause the game.
 *
 * Essentially, a glorified BlitSurface function.
 *
 * \param s		The SDL_Surface to blit.
 * \param delay		Time (in ms) to sleep for.
 */
void paint_pixels(SDL_Surface *s, Uint32 delay);

/*! \brief Load an image from memory.
 *
 * Create an #SDL_Surface from an image stored in memory, typically compiled
 * in. Use only after init_SDL().
 *
 * \param mem	Memory location to load image from.
 * \param size	Length (in bytes) of the file.
 * \param alpha	If this is false, ignore the alpha channel.
 */
SDL_Surface *img_from_mem(void *mem, int size, bool alpha);

/*!
 * \brief Load an image from a file.
 *
 * Create an #SDL_Surface from an image stored in the file system. Use only after
 * init_SDL() and init_fs().
 *
 * \param fname		File name.
 */
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

