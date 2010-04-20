/* ROXOPTR2 LEVEL

	lv0

*/

/*************** meta-data: *************/
#	define VX 80
#	define VY 80
#	define D_VX 180
#	define D_VY 180

#	define VI_R 0.6
#	define VI_L 0
#	define VI_B 0
#	define VI_T 0

#	define FIN_X 1200
#	define FIN_Y 480

#	define X0 38
#	define Y0 200
    
#	define CONTROLS DIR_UP
/****************************************/

#include "levels/lv0/lv0.gif.h"
#include "lv0.xbm"

#	define WIDTH	lv0_width
#	define HEIGHT	lv0_height
#	define BITS	lv0_bits

#	define BG_SIZE	lv0_gif_len
#	define BG_BYTES	lv0_gif

#include "../level_tpl.h"

Level *load_lv0() { return load_(); }
