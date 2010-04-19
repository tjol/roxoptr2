/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*! \file */

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

/* include at this point since overlays.h uses type Level */
#include "overlays.h"

/*! \see #Level */
struct level_ {
    unsigned int w;	/*!< \brief width */
    unsigned int h;	/*!< \brief height */
    
    unsigned char *bits;
    SDL_Surface *bg;
    
    /* all speeds in pixels per second */
    double vx;		/*!< \brief default speed -> right */
    double vy;		/*!< \brief default speed -> down */
    double Dvx;		/*!< \brief speed Delta when button pressed (X) */
    double Dvy;		/*!< \brief speed Delta when button pressed (Y) */
    
    double visible_r;	/*!< \brief percent to be visible at all times, on the right */
    double visible_l;	/*!< \brief percent to be visible at all times, on the left */
    double visible_t;	/*!< \brief percent to be visible at all times, upward */
    double visible_b;	/*!< \brief percent to be visible at all times, downward */
    
    unsigned int fin_x; /*!< \brief position of horizontal finish line. */
    unsigned int fin_y; /*!< \brief position of vertical finish line. */
    
    unsigned int heli_x0; /*!< \brief initial position of heli (X value) */
    unsigned int heli_y0; /*!< \brief initial position of heli (Y value) */
    
    directional_t controls; /*!< \brief enabled controls */

    /*! \brief #Sprite to be used as helicopter.
     * If this is NULL, use the default heli.
     */
    Sprite *main_sprite;
    /*! \brief List of overlays to be displayed.
     * No extra overlays if this is NULL (obviously).
     * \see #Overlay
     */
    Overlay *overlays;
    
    /*! \brief Destructor function. */
    void (*del)(Level *);

    /*! \brief usable by custom levels. */
    void *internal;
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

#define GET_BIT(L,X,Y) (get_bit(L,X,Y))

typedef struct levellist_ LevelList;

/*! \brief Doubly linked list of levels. */
struct levellist_ {
    Level *level;
    
    LevelList *prev;
    LevelList *next;
    
    Level *(*load)(void);
    
    char *title;
};

/*! \brief Currently running level. (if in-game) */
extern LevelList *current_level;
/*! \brief List of installed levels. */
extern LevelList *levels;

void init_levels();
void del_levels();

int start_level(LevelList *ll);

#endif

