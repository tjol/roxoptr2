/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "game.h"
#include "main.h"
#include "sprite.h"

directional_t controls_held = 0;

void game_tic()
{
    double vx, vy;
    int delta;
    int collision;
    int i;
    
    static double xpos = -1;
    static double ypos = -1;
        
    Level *lv = thegame.current_level;
    
    if ((int)xpos != thegame.heli_xpos || (int)ypos != thegame.heli_ypos) {
	xpos = thegame.heli_xpos;
	ypos = thegame.heli_ypos;
    }
    
    /**** move heli ************************************/
    controls_held &= lv->controls;
    
    vx = lv->vx;
    if (controls_held & DIR_RIGHT) vx += lv->Dvx;
    if (controls_held & DIR_LEFT)  vx -= lv->Dvx;
    vy = lv->vy;
    if (controls_held & DIR_DOWN)  vy += lv->Dvy;
    if (controls_held & DIR_UP)    vy -= lv->Dvy;
    
    xpos += vx * (double) iter_ticks / 1000.0;
    ypos += vy * (double) iter_ticks / 1000.0;
    
    thegame.heli_xpos = (int) xpos;
    thegame.heli_ypos = (int) ypos;
    
    /**** check for collisions *************************/
    
    collision = 0;
    for (i=0; i < thegame.main_sprite->n_coll_checkpts; ++i) {
	collision |= GET_BIT(lv, thegame.heli_xpos + thegame.main_sprite->coll_checkpts[i].relx,
				 thegame.heli_ypos + thegame.main_sprite->coll_checkpts[i].rely);
    }

    if (collision) {
	thegame.running = 0;
	paint_banner("You hit an obstacle", "FAIL", 255, 0, 0, 3000);
    }    

    /**** adjust visible area **************************/
    
    if (   (delta = SCREEN_W - thegame.heli_xpos + thegame.xpos
		    - (int)(lv->visible_r * SCREEN_W)) < 0
	&&  thegame.xpos + SCREEN_W < lv->w ) {
	    thegame.xpos -= delta; /* delta is negative */
    }
    if (   (delta = thegame.heli_xpos - thegame.xpos
		    - (int)(lv->visible_l * SCREEN_W)) < 0
	&&  thegame.xpos > 0 ) {
	    thegame.xpos += delta; /* delta is negative */
    }
    if (   (delta = SCREEN_H - thegame.heli_ypos + thegame.ypos
		    - (int)(lv->visible_b * SCREEN_H)) < 0
	&&  thegame.ypos + SCREEN_H < lv->h ) {
	    thegame.ypos -= delta; /* delta is negative */
    }
    if (   (delta = thegame.heli_ypos - thegame.ypos
		    - (int)(lv->visible_t * SCREEN_H)) < 0
	&&  thegame.ypos > 0 ) {
	    thegame.ypos += delta; /* delta is negative */
    }
    
    
    
    /**** reached finish line? *************************/
    if(( (lv->heli_x0 < (signed) lv->fin_x) ? (thegame.heli_xpos >= (signed) lv->fin_x)
					    : (thegame.heli_xpos <= (signed) lv->fin_x) )
    || ( (lv->heli_y0 < (signed) lv->fin_y) ? (thegame.heli_ypos >= (signed) lv->fin_y)
					    : (thegame.heli_ypos <= (signed) lv->fin_y) )) {
	paint_banner("You actually made it!", "WIN", 0, 255, 0, 5000);
	
	thegame.running = start_level(current_level->next);
    }
    
}
