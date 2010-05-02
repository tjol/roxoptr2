/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*!< \file */
#ifndef R_SPRITE_H
#define R_SPRITE_H

#include <SDL/SDL.h>

struct sprite_point {
    int relx;
    int rely;
};

struct sprite_frame {
    SDL_Surface *s;
    SDL_Rect *rect;
    unsigned char *coll_bits;
};

/*! An in-game sprite. */
typedef struct sprite {
    struct sprite_frame *frames;
    int n_frames;
    Uint32 frame_len; /*!< \brief Amount of time after which to switch frames. (in ms) */

    struct sprite_point *coll_checkpts;
    int n_coll_checkpts;

    void (*free_data)(struct sprite*);

    Uint32 switch_time;
    int current_frame;
} Sprite;

/*! \brief Load standard heli sprite. */
Sprite *load_heli(void);

/*! \brief Display a sprite on-screen
 *
 * \param s		The sprite to display
 * \param canvas	The #SDL_Surface to blit to.
 * \param position	The area/position to blit to.
 * \see display_rect()
 */
void animate_sprite(Sprite *s, SDL_Surface *canvas, SDL_Rect *position);

/*! \brief Load a sprite configuration */
Sprite *load_sprite_from_cfgfile(const char *fname);

/*! \brief Attempt to find and load a sprite.
 *
 * \param name		a file name relative to the current directory or to 
 * 			<CODE>sprites/</CODE> in the installation. <CODE>.cfg</CODE>
 * 			is appended if required.
 */
Sprite *find_sprite(const char *name);

extern Sprite *classic_heli_sprite;

#endif

