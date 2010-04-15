/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef R_LEVEL_H_
#define R_LEVEL_H_

#include "sprite.h"
#include <SDL/SDL.h>

typedef unsigned char directional_t;
#define DIR_UP		1
#define DIR_DOWN	2
#define DIR_LEFT	4
#define DIR_RIGHT	8

typedef struct level_ Level;

struct level_ { /* Level */
    unsigned int w;	/* width */
    unsigned int h;	/* height */
    
    unsigned char *bits;
    SDL_Surface *bg;
    
    /* all speeds in pixels per second */
    double vx;		/* default speed -> right */
    double vy;		/* default speed -> down */
    double Dvx;		/* speed Delta when button pressed (X) */
    double Dvy;		/* speed Delta when button pressed (Y) */
    
    double visible_r;	/* percent to be visible at all times, on the right */
    double visible_l;   /*                                     on the left  */
    double visible_t;	/* percent to be visible at all times, upward */
    double visible_b;   /*                                     downward  */
    
    unsigned int fin_x;
    unsigned int fin_y;
    
    unsigned int heli_x0;
    unsigned int heli_y0;
    
    directional_t controls;

    struct sprite *main_sprite;
    
    void (*del)(Level *);
};

static inline int
get_bit(Level *lv, unsigned int x, unsigned int y) 
{
    unsigned int byte_w = (lv->w / 8) + ((lv->w % 8) == 0 ? 0 : 1);
    unsigned int idx = (byte_w * y) + (x / 8);
    unsigned int bit = x % 8;
    
    /* negative x, y will overflow and therefore return -1; */
    return (x < lv->w && y < lv->h) ? (lv->bits[idx] >> bit) & 1 : -1;
}
/*
#define GET_BIT(L,X,Y) (((L)->bits[(((L)->w/8)+(((L)->w%8)==0?0:1))*(Y)+(X)/8] >> ((X)%8)) & 1)
*/
#define GET_BIT(L,X,Y) (get_bit(L,X,Y))

typedef struct levellist_ LevelList;

struct levellist_ {
    Level *level;
    
    LevelList *prev;
    LevelList *next;
    
    Level *(*load)(void);
    
    char *title;
};

extern LevelList *current_level;
extern LevelList *levels;

void init_levels();
void del_levels();

int start_level(LevelList *ll);

#endif

