/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef R_GAME_H_
#define R_GAME_H_

#include "level.h"

#define CTRL_ENTER 16
extern directional_t controls_held;

bool
collide_bitmaps (Uint8 *a, int a_w, int a_h, int a_x, int a_y,
		 Uint8 *b, int b_w, int b_h, int b_x, int b_y,
		 bool stay_within);

bool collide_main_sprite (Uint8 *other, int w, int h, int x, int y);

void game_tic();

#endif

