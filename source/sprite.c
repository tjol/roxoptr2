/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "img/heli.png.h"
#include "img/heli.xbm"
#include "display.h"
#include "sprite.h"
#include "cfgparser.h"
#include "filesys.h"

Sprite *classic_heli_sprite;

void free_surfaces (Sprite *);

void gen_bits_from_checkpts(Sprite *s);

Sprite *load_heli(void)
{
    struct sprite *h;
    SDL_Surface *heli_img;

    h = malloc(sizeof(struct sprite));

    heli_img = img_from_mem(heli_png, heli_png_len, true);
    h->frames = malloc(4 * sizeof(struct sprite_frame));

    for (int i = 0; i < 4; ++i) {
	h->frames[i].s = heli_img;
	h->frames[i].rect = malloc(sizeof(SDL_Rect));
	h->frames[i].rect->x = i * 38;
	h->frames[i].rect->y = 0;
	h->frames[i].rect->w = heli_width;
	h->frames[i].rect->h = heli_height;
	h->frames[i].coll_bits = heli_bits;
    }

    h->n_frames = 4;
    h->frame_len = 60;
    h->n_coll_checkpts = 13;
    h->current_frame = 0;
    h->switch_time = 0;
    /* h->free_data = &free_surfaces; */
    h->free_data = NULL;

    return h;
}

void free_surfaces(Sprite *s)
{
    int i;

    for (i=0; i<(s->n_frames); ++i) {
	SDL_FreeSurface(s->frames[i].s);
    }

    s->frames = NULL;
}

void animate_sprite (Sprite *s, SDL_Surface *canvas, SDL_Rect *position)
{
    Uint32 now;

    if (s->n_frames == 1) {
	SDL_BlitSurface(s->frames[0].s, s->frames[0].rect, canvas, position);
	return;
    }

    now = SDL_GetTicks();

    if (now > s->switch_time) {
	if (++s->current_frame >= s->n_frames) s->current_frame = 0;
	s->switch_time = now + s->frame_len;
    }

    SDL_BlitSurface(s->frames[s->current_frame].s, s->frames[s->current_frame].rect,
		    canvas, position);
}

static SDL_Surface **surfaces_ = NULL;
static int n_surfaces_;

static bool
sprite_load_cb (struct cfg_section *sect,
		const char *key, const char *val,
		Sprite *sp)
{
    int i;
    int surface;
    int x,y,w,h;

    switch (sect->id) {
	case 1: /* img files */
	    if (tolower(key[0]) == 'n') {
		n_surfaces_ = atoi(val);
		if (n_surfaces_ == 0) {
		    fprintf(stderr, "ERROR: No sprite image. \n");
		    return false;
		}
		surfaces_ = malloc(n_surfaces_ * sizeof(SDL_Surface *));
	    } else if (!surfaces_) {
		fprintf(stderr, "ERROR: Number of sprite images must be specified first.\n");
		return false;
	    } else if ((i = atoi(key)) >= n_surfaces_) {
		fprintf(stderr, "ERROR: Index out of bounds: %d.\n", i);
		return false;
	    } else {
		if (!(surfaces_[i] = img_from_file(val))) {
		    fprintf(stderr, "ERROR: Failed to load file %s.\n", val);
		    return false;
		}
	    }
	    break;;

	case 2: /* frames */
	    if (tolower(key[0]) == 'n') {
		sp->n_frames = atoi(val);
		if (sp->n_frames == 0) {
		    fprintf(stderr, "ERROR: No frames. \n");
		    return false;
		}
		sp->frames = malloc(sp->n_frames * sizeof(struct sprite_frame));
	    } else if (!(sp->frames)) {
		fprintf(stderr, "ERROR: Number of frames must be specified first.\n");
		return false;
	    } else if ((i = atoi(key)) >= sp->n_frames) {
		fprintf(stderr, "ERROR: Index out of bounds: %d.\n", i);
		return false;
	    } else {
		sscanf(val, "%d %d %d %d %d", &surface, &x, &y, &w, &h);
		if (surface >= n_surfaces_) {
		    fprintf(stderr, "ERROR: No such surface: %d.\n", surface);
		    return false;
		}
		sp->frames[i].s = surfaces_[surface];
		sp->frames[i].rect = malloc(sizeof(SDL_Rect));
		sp->frames[i].rect->x = x;
		sp->frames[i].rect->y = y;
		sp->frames[i].rect->w = w;
		sp->frames[i].rect->h = h;
	    }
	    break;;

	case 3: /* coll pts */
	    if (tolower(key[0]) == 'n') {
		sp->n_coll_checkpts = atoi(val);
		if (sp->n_coll_checkpts == 0) {
		    /* This would be a silly arrangement for the main sprite,
		     * but allowing "no collision points" might be useful later?
		     *  => This is OK. */
		} else {
		    sp->coll_checkpts = malloc(sp->n_coll_checkpts 
						* sizeof(struct sprite_point));
		}
	    } else if (!(sp->coll_checkpts)) {
		fprintf(stderr, "ERROR: Number of checkpoints must be specified first.\n");
		return false;
	    } else if ((i = atoi(key)) >= sp->n_coll_checkpts) {
		fprintf(stderr, "ERROR: Index out of bounds: %d.\n", i);
		return false;
	    } else {
		sscanf(val, "%d %d", &x, &y);
		sp->coll_checkpts[i].relx = x;
		sp->coll_checkpts[i].rely = y;
	    }
	    break;;

	case 4: /* animation */
	    if (strcasecmp(key, "frame ms") != 0) {
		fprintf(stderr, "ERROR: Unknown setting: %s\n", key); 
		return false;
	    }
	    sp->frame_len = atol(val);
	    break;;
    }

    return true;
}


Sprite *
load_sprite_from_cfgfile(const char *fname)
{
    static struct cfg_section sections[] = {
	{ 1, "img files" },
	{ 2, "frames" },
	{ 3, "coll pts" },
	{ 4, "animation" },
	{ 0, NULL } /* sentinel */
    };

    FILE *fp;
    char *file_name;
    char *cp, *cp0 = NULL;
    char *old_pwd;
    Sprite *sp;

    if (!(fp = fopen(fname, "r"))) {
	perror(0);
	return NULL;
    }

    /* chdir to the right place, since paths are relative */
    file_name = strdup(fname);
    for (cp = file_name; *cp; ++cp) if (*cp == '/') cp0 = cp;
    if (cp0)
	*cp0 = '\0';
    old_pwd = get_pwd();
    chdir(file_name);
    *cp0 = '/';

    sp = calloc(1, sizeof(struct sprite));

    surfaces_ = NULL;
    n_surfaces_ = 0;
    if (!read_cfg_file(fp, sections, (cfg_callback_f)&sprite_load_cb, sp)) {
	free(sp);
	sp = NULL;
	/* return NULL below */
    }

    chdir(old_pwd);
    fclose(fp);

    if (sp->coll_checkpts) {
	gen_bits_from_checkpts(sp);
    }

    return sp;
}

static Sprite *
find_sprite_(const char *name, bool athome)
{
    /* easiest case: it's a valid file path. */
    if (access(name, R_OK) == 0) return load_sprite_from_cfgfile(name);

    int len = strlen(name);
    char *str;
    struct sprite *ret;

    /* .cfg missing? */
    str = malloc(len+5);
    strcpy(str, name);
    strcpy(str+len, ".cfg");
    if (access(str, R_OK) == 0) {
	ret = load_sprite_from_cfgfile(str);
	free(str);
	return ret;
    }
    free(str);

    /* try from the standard place: ${inst}/sprites */
    if (!athome) {
	str = malloc(8+len+1);
	strcpy(str, "sprites/");
	strcpy(str+8, name);
	ret = find_sprite_(path_from_home(str), true);
	free(str);
	return ret;
    }

    /* can't find it. */
    return NULL;
}

Sprite *
find_sprite(const char *name)
{
    return find_sprite_(name, false);
}

void
gen_bits_from_checkpts(Sprite *s)
{
    unsigned int w = s->frames[0].rect->w;
    unsigned int h = s->frames[0].rect->h;
    unsigned int byte_w = (w / 8) + ((w % 8) == 0 ? 0 : 1);

    unsigned char *bits;
    unsigned int i;
    unsigned int x, y;

    size_t sz;

    sz = (h * byte_w);
    bits = calloc(sz, sizeof(char));

    for (i=0; i < s->n_coll_checkpts; ++i) {
	x = s->coll_checkpts[i].relx;
	y = s->coll_checkpts[i].rely;
	bits [ (byte_w*y) + (x/8) ] |= (1 << (x%8));
    }

    for (i=0; i < s->n_frames; ++i) {
	s->frames[i].coll_bits = bits;
    }

    return;
}

