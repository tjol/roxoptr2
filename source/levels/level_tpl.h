/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "../level.h"
#include "../display.h"

static void del_(Level *lv)
{
    SDL_FreeSurface(lv->bg);
    free(lv);
}

static Level *load_()
{
    Level *level;
    
    level = malloc(sizeof(Level));
    
    level->w = WIDTH;
    level->h = HEIGHT;
    level->bits = BITS;
# if defined(XPM)
    level->bg = IMG_ReadXPMFromArray(XPM);
# elif defined(BG_BYTES)
    level->bg = img_from_mem(BG_BYTES, BG_SIZE, 0);
# endif
    level->vx = VX;
    level->vy = VY;
    level->Dvx = D_VX;
    level->Dvy = D_VY;
    level->visible_r = VI_R;
    level->visible_l = VI_L;
    level->visible_b = VI_B;
    level->visible_t = VI_T;
    level->fin_x = FIN_X;
    level->fin_y = FIN_Y;
    level->heli_x0 = X0;
    level->heli_y0 = Y0;
    level->controls = CONTROLS;
    level->del = del_;
    level->main_sprite = NULL;
    level->overlays = NULL;
        
    return level;
}

