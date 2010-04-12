/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
};

struct sprite {
    struct sprite_frame *frames;
    int n_frames;
    Uint32 frame_len;

    struct sprite_point *coll_checkpts;
    int n_coll_checkpts;

    void (*free_data)(struct sprite*);
};

struct sprite *load_heli(void);
void animate_sprite(struct sprite *s, SDL_Surface *canvas, SDL_Rect *position);

struct sprite *load_sprite_from_cfgfile(const char *fname);

struct sprite *find_sprite(const char *name);

extern struct sprite *classic_heli_sprite;

#endif

