/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "img/heli.png.h"
#include "display.h"
#include "sprite.h"

struct sprite *main_sprite;

struct sprite_point std_heli_sprite_coll_checkpts[] = 
    {
	{0, 0},
	{38, 0},
	{0, 12},
	{16, 13},
	{16, 24},
	{38, 24}
    };

void free_surfaces (struct sprite *);

struct sprite *load_heli(void)
{
    struct sprite *h;
    SDL_Surface *heli_img;

    h = malloc(sizeof(struct sprite));

    heli_img = img_from_mem(heli_png, heli_png_len, 1);
    h->frames = malloc(sizeof(SDL_Surface*));

    *(h->frames) = heli_img;
    h->n_frames = 1;
    h->frame_len = 0;
    h->coll_checkpts = std_heli_sprite_coll_checkpts;
    h->n_coll_checkpts = 6;
    h->free_data = &free_surfaces;

    return h;
}

void free_surfaces(struct sprite *s)
{
    int i;

    for (i=0; i<(s->n_frames); ++i) {
	SDL_FreeSurface(s->frames[i]);
    }

    s->frames = NULL;
}

