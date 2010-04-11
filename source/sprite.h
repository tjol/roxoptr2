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

struct sprite {
    SDL_Surface **frames;
    int n_frames;
    time_t frame_len;

    struct sprite_point *coll_checkpts;
    int n_coll_checkpts;

    void (*free_data)(struct sprite*);
};

struct sprite *load_heli(void);

extern struct sprite *main_sprite;

#endif

