/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <string.h>
#include "overlays.h"
#include "sprite.h"
#include "main.h"

static Overlay *alloc_fixed(struct overlay_type *t);
static bool construct_fixed(Overlay *o, const char *key, const char *value);
static void paint_fixed(Level *l, Overlay *o, SDL_Surface *screen);
static bool run_fixed(Level *l, Overlay *o);
static void del_fixed(Overlay *o);

struct overlay_type overlay_types[] = {
    { "fixed", &alloc_fixed, &construct_fixed }
};

size_t overlay_types_len = 1;


struct overlay_type *
get_overlay_type(const char *name)
{
    int i;

    for (i=0; i<overlay_types_len; ++i) {
	if (strcasecmp(name, overlay_types[i].name) == 0) {
	    return &overlay_types[i];
	}
    }

    return NULL;
}

inline bool
is_visible(int x, int y, int width, int height)
{
    return ( x < (thegame.xpos + SCREEN_W)
	  && (x + width) > thegame.xpos
	  && y < (thegame.ypos + SCREEN_H)
	  && (y + height) > thegame.ypos );
}

inline SDL_Rect
display_rect(int x, int y)
{
    SDL_Rect ret;

    ret.x = x - thegame.xpos;
    ret.y = y - thegame.ypos;

    return ret;
}

struct fixed_overlay_info {
    Sprite *s;
    int x;
    int y;
};

static Overlay *
alloc_fixed(struct overlay_type *t)
{
    Overlay *ret;
    struct fixed_overlay_info *ret_info;

    ret = malloc(sizeof(Overlay));
    ret->next = NULL;
    ret->paint = &paint_fixed;
    ret->keep_running = &run_fixed;
    ret->del = &del_fixed;

    ret_info = malloc(sizeof(struct fixed_overlay_info));
    ret->internal = ret_info;
    return ret;
}

static bool
construct_fixed(Overlay *o, const char *key, const char *value)
{
    struct fixed_overlay_info *inf = o->internal;

    if (strcasecmp(key, "position") == 0) {
	sscanf(value, "%d %d", &(inf->x), &(inf->y));
    } else if (strcasecmp(key, "sprite") == 0) {
	if (!(inf->s = find_sprite(value))) {
	    fprintf(stderr, "Failed to load sprite: %s\n", value);
	}
    } else {
	fprintf(stderr, "Invalid argument: %s\n", key);
	return false;
    }
    return true;
}

static void
paint_fixed(Level *l, Overlay *o, SDL_Surface *screen)
{
    struct fixed_overlay_info *inf = o->internal;

    if (!is_visible(inf->x, inf->y, inf->s->frames[0].rect->w, inf->s->frames[0].rect->h)) {
	return;
    }

    SDL_Rect r = display_rect(inf->x, inf->y);
    animate_sprite(inf->s, screen, &r);
}

static bool
run_fixed(Level *l, Overlay *o)
{
    struct fixed_overlay_info *inf = o->internal;

    struct sprite_frame *frm = &(inf->s->frames[inf->s->current_frame]);
    
    if (collide_main_sprite(frm->coll_bits, frm->rect->w, frm->rect->h,
					    inf->x, inf->y)) {
	return false;
    } else {
	return true;
    }
}

static void
del_fixed(Overlay *o)
{
    struct fixed_overlay_info *inf = o->internal;

    if (inf->s->free_data) {
	inf->s->free_data(inf->s);
    }
    free(inf->s);
    free(inf);
    free(o);
}

