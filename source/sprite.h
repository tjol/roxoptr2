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

typedef struct sprite {
    struct sprite_frame *frames;
    int n_frames;
    Uint32 frame_len;

    struct sprite_point *coll_checkpts;
    int n_coll_checkpts;

    void (*free_data)(struct sprite*);

    Uint32 switch_time;
    int current_frame;
} Sprite;

Sprite *load_heli(void);
void animate_sprite(Sprite *s, SDL_Surface *canvas, SDL_Rect *position);

Sprite *load_sprite_from_cfgfile(const char *fname);

Sprite *find_sprite(const char *name);

extern Sprite *classic_heli_sprite;

#endif

