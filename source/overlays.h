/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef R_OVERLAYS_H
#define R_OVERLAYS_H

#include <stdbool.h>

typedef struct level_overlay Overlay;

#include "level.h"

struct level_overlay {
    struct level_overlay *next;

    void (*paint)(Level *l, struct level_overlay *ov, SDL_Surface *canvas);
    void (*del)(struct level_overlay *ov);

    void *internal;
};

struct overlay_type {
    char *name;

    Overlay *(*alloc)(struct overlay_type *t);
    bool (*construct)(Overlay *o, const char *key, const char *value);
};

struct overlay_type *get_overlay_type(const char *name);
bool is_visible(int x, int y, int width, int height);
SDL_Rect display_rect(int x, int y);

extern struct overlay_type overlay_types[];
extern size_t overlay_types_len;

#endif

