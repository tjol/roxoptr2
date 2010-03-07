#include "game.h"
#include "main.h"

directional_t controls_held = 0;

void game_tic()
{
    double vx, vy;
    int delta;
    
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
    
    thegame.heli_xpos = (unsigned int) xpos;
    thegame.heli_ypos = (unsigned int) ypos;
    
    /**** check for collisions *************************/
    
    /* checking the 4 corners should do the trick */
    if (   GET_BIT(lv, thegame.heli_xpos,        thegame.heli_ypos)
	|| GET_BIT(lv, thegame.heli_xpos+HELI_W, thegame.heli_ypos)
	|| GET_BIT(lv, thegame.heli_xpos+HELI_W, thegame.heli_ypos+HELI_H)
	|| GET_BIT(lv, thegame.heli_xpos,        thegame.heli_ypos+HELI_H)
	|| thegame.heli_xpos+HELI_W > lv->w
	|| thegame.heli_ypos+HELI_H > lv->h                                ) {
	
	thegame.running = 0;
	paint_banner(fail_banner, 3000);
	
    }
    
    /**** adjust visible area **************************/
    
    if (   (delta = SCREEN_W - thegame.heli_xpos + thegame.xpos
		    - (lv->visible_r * SCREEN_W)) < 0
	&&  thegame.xpos + SCREEN_W < lv->w ) {
	    thegame.xpos -= delta; /* delta is negative */
    }
    if (   (delta = thegame.heli_xpos - thegame.xpos
		    - (lv->visible_l * SCREEN_W)) < 0
	&&  thegame.xpos > 0 ) {
	    thegame.xpos += delta; /* delta is negative */
    }
    if (   (delta = SCREEN_H - thegame.heli_ypos + thegame.ypos
		    - (lv->visible_b * SCREEN_H)) < 0
	&&  thegame.ypos + SCREEN_H < lv->h ) {
	    thegame.ypos -= delta; /* delta is negative */
    }
    if (   (delta = thegame.heli_ypos - thegame.ypos
		    - (lv->visible_t * SCREEN_H)) < 0
	&&  thegame.ypos > 0 ) {
	    thegame.ypos += delta; /* delta is negative */
    }
    
    
    
    /**** reached finish line? *************************/
    if (   thegame.heli_xpos >= lv->fin_x
	|| thegame.heli_ypos >= lv->fin_y ) {
	
	thegame.running = 0;
	paint_banner(win_banner, 5000);
    }
    
}
