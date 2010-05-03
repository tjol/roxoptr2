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
#include <stdbool.h>

directional_t controls_held = 0;

bool
collide_bitmaps (Uint8 *a, int a_w, int a_h,
		 Uint8 *b, int b_w, int b_h, int b_x, int b_y);

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
    
    struct sprite_frame *frm = &(thegame.main_sprite->frames
				    [thegame.main_sprite->current_frame]);
    if (collide_bitmaps(lv->bits, lv->w, lv->h,
			frm->coll_bits, frm->rect->w, frm->rect->h,
					thegame.heli_xpos, thegame.heli_ypos)) {
	thegame.running = false;
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
    if (   (lv->heli_x0 < (signed) lv->fin_x) == (thegame.heli_xpos > (signed) lv->fin_x)
	|| (lv->heli_y0 < (signed) lv->fin_y) == (thegame.heli_ypos > (signed) lv->fin_y)) {
	paint_banner("You actually made it!", "WIN", 0, 255, 0, 5000);
	
	thegame.running = start_level(current_level->next);
    }
    
}


bool
collide_bitmaps (Uint8 *a, int a_w, int a_h,
		 Uint8 *b, int b_w, int b_h, int b_x, int b_y)
{
    int x, y;

    unsigned a_byte_w = (a_w / 8) + ((a_w % 8) == 0 ? 0 : 1);
    unsigned b_byte_w = (b_w / 8) + ((b_w % 8) == 0 ? 0 : 1);

    Uint8 *a_byte;
    Uint8 *b_byte;
    Uint16 shifted;

    int bit_delta = 8 - (b_x % 8);

    for (y = b_y; y < b_y + b_h && y < a_h; ++y) {
	for (x = b_x; x < b_x + b_w && x < a_w; x+=8) {
	    a_byte = a + y*a_byte_w + x/8;
	    b_byte = b + (y-b_y)*b_byte_w + (x-b_x)/8;
	    /* don't just interpret the address as a short: this only works on
	     * big-endian machines. */
	    shifted = ((*a_byte << 8 | ((x/8+1 < a_byte_w) ? *(a_byte+1) : 0))
					>> bit_delta) & 0xff;

	    if ((*b_byte) & shifted) {
		return true;
	    }
	}
    }

    return false;
}



