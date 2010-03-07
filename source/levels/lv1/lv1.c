/* ROXOPTR2 LEVEL

	lv1

*/

/*************** meta-data: *************/
#	define lv1_vx 0
#	define lv1_vy 50
#	define lv1_Dvx 80
#	define lv1_Dvy 60

#	define lv1_vr 0.4
#	define lv1_vl 0.4
#	define lv1_vb 0.3
#	define lv1_vt 0.3

#	define lv1_fin_x 1300
#	define lv1_fin_y 920

#	define lv1_x0 304
#	define lv1_y0 50

#	define lv1_controls	DIR_UP | DIR_LEFT | DIR_RIGHT
/****************************************/

#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "../../level.h"

#include "lv1.xpm"
#include "lv1.xbm"

static void del_lv1(Level *lv)
{
    SDL_FreeSurface(lv->bg);
    free(lv);
}

Level *load_lv1()
{
    Level *level;
    SDL_Surface *optimized_xpm;
    SDL_Surface *xpm_surface;
    
    xpm_surface = IMG_ReadXPMFromArray(lv1_xpm);
    optimized_xpm = xpm_surface;
    /*optimized_xpm = SDL_DisplayFormat(xpm_surface);
    SDL_FreeSurface(xpm_surface);*/
    
    level = malloc(sizeof(Level));
    
    level->w = lv1_width;
    level->h = lv1_height;
    level->bits = lv1_bits;
    level->bg = optimized_xpm;
    level->vx = lv1_vx;
    level->vy = lv1_vy;
    level->Dvx = lv1_Dvx;
    level->Dvy = lv1_Dvy;
    level->visible_r = lv1_vr;
    level->visible_l = lv1_vl;
    level->visible_b = lv1_vb;
    level->visible_t = lv1_vt;
    level->fin_x = lv1_fin_x;
    level->fin_y = lv1_fin_y;
    level->heli_x0 = lv1_x0;
    level->heli_y0 = lv1_y0;
    level->controls = lv1_controls;
    level->del = del_lv1;
        
    return level;
}
