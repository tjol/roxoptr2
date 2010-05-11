/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "level.h"
#include "main.h"
#include "filesys.h"
#include "overlays.h"

#include <sys/types.h>

#include "cfgparser.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include <SDL/SDL_image.h>

extern Level *load_lv0();

LevelList *levels = NULL;
LevelList *current_level = NULL;

void free_levels();

static LevelList *load_level_from_cfg(char *filename, LevelList *prev);

void init_levels()
{
    LevelList *lv0;
    LevelList *prev;
    LevelList *cur;

    FILE *fp_levels_list;
    char lbuf[256];
    char *line;
    char *cp, *cp_nw;
    char *cwda;
    char *cwd_end;
    
    lv0 = malloc(sizeof(LevelList));
    
    lv0->level = NULL;
    lv0->prev = NULL;
    lv0->next = NULL;
    lv0->load = &load_lv0;
    lv0->title = "Level #0";
    
    cur = NULL;
    prev = lv0;
    
    init_fs();

    cwda = get_pwd_w_extra(10);

    strcat(cwda, "/leveldata/");
    for(cwd_end = cwda; *cwd_end; ++cwd_end);

    fp_levels_list = fopen("leveldata/levels.list", "r");
    if (!fp_levels_list) {
	perror("leveldata/levels.list");
    } else {
	while (line = fgets(lbuf, 256, fp_levels_list)) {
	    /* strip spaces */
	    while (isspace(*line)) line++;
	    cp_nw = line-1;
	    for (cp = line; *cp != '\0'; ++cp) if (!isspace(*cp)) cp_nw = cp;
	    *(cp_nw+1) = '\0';

	    if (line[0] == '\0') continue;

	    strcpy(cwd_end, line);

	    cur = load_level_from_cfg(cwda, prev);
	    if (cur) {
		prev->next = cur;
		prev = cur;
	    }

	    *cwd_end = '\0';
	}
    }
    free(cwda);
    chdir_home();

    current_level = levels = lv0;
    
    atexit(free_levels);
}

void free_levels()
{
    LevelList *l, *next;
    
    for (l = levels; l != NULL; l = next) {
	if (l->level != NULL) {
	    l->level->del(l->level);
	}
	next = l->next;
	free(l);
    }
    
    levels = NULL;
}

void unload_level(LevelList *ll)
{
    if (ll->level != NULL) {
	ll->level->unload(ll->level);
	ll->level = NULL;
    }
}

bool start_level(LevelList *ll)
{
    LevelList *list;

    if (!ll) return 0;

    for (list = levels; list; list = list->next) {
	if (list != ll && list->level != NULL) {
	    unload_level(list);
	}
    }
    
    if (! ll->level) {
	ll->level = ll->load(ll->load_arg);
    }
    
    thegame.xpos = thegame.ypos = 0;
    thegame.current_level = ll->level;
    thegame.heli_xpos = thegame.current_level->heli_x0;
    thegame.heli_ypos = thegame.current_level->heli_y0;
    if (ll->level->main_sprite) {
	thegame.main_sprite = ll->level->main_sprite;
    } else {
	thegame.main_sprite = classic_heli_sprite;
    }
   
    current_level = ll;
    
    return 1;
}

struct level_cfg_info {
    Level *base_l;
    char *working_dir;
    char *bitmap_path;
    char *bg_path;
};

bool
load_callback(struct cfg_section *sect, const char *key, const char *value, void *ll_)
{
    LevelList *ll = ll_;
    struct level_cfg_info *infop = ll->load_arg;

    double xd,yd;
    unsigned int xu, yu;
    double visible;
    bool ok;

    struct ov_info {
	struct overlay_type *t;
	struct level_overlay *o; } *ov_infop;

    struct level_overlay *lop;

    switch (sect->id) {
	case 1: /* [level] */
	    if (strcasecmp(key, "name") == 0) {
		ll->title = strdup(value);
	    } else if (strcasecmp(key, "bits") == 0) {
		/* lazy-load bitmap */
		infop->bitmap_path = strdup(value);
	    } else if (strcasecmp(key, "background") == 0) {
		infop->bg_path = strdup(value);
	    } else if (strcasecmp(key, "start") == 0) {
		sscanf(value, "%u %u", &xu, &yu);
		infop->base_l->heli_x0 = xu;
		infop->base_l->heli_y0 = yu;
	    } else if (strcasecmp(key, "end") == 0) {
		sscanf(value, "%u %u", &xu, &yu);
		infop->base_l->fin_x = xu;
		infop->base_l->fin_y = yu;
		infop->base_l->fin_both = false;
	    } else if (strcasecmp(key, "end_corner") == 0) {
		sscanf(value, "%u %u", &xu, &yu);
		infop->base_l->fin_x = xu;
		infop->base_l->fin_y = yu;
		infop->base_l->fin_both = true;
	    } else if (strcasecmp(key, "null_velocity") == 0) {
		sscanf(value, "%lf %lf", &xd, &yd);
		infop->base_l->vx = xd;
		infop->base_l->vy = yd;
	    } else if (strcasecmp(key, "velocity_delta") == 0) {
		sscanf(value, "%lf %lf", &xd, &yd);
		infop->base_l->Dvx = xd;
		infop->base_l->Dvy = yd;
	    } else if (strcasecmp(key, "sprite") == 0) {
		if (!(infop->base_l->main_sprite = find_sprite(value))) {
		    fprintf(stderr, "Error loading sprite\n");
		    return false;
		}
	    }
	    break;;
	case 2: /* [viewport] */
	    sscanf(value, "%lf%%", &visible);
	    visible /= 100.0;
	    if (strcasecmp(key, "right") == 0) {
		infop->base_l->visible_r = visible;
	    } else if (strcasecmp(key, "left") == 0) {
		infop->base_l->visible_l = visible;
	    } else if (strcasecmp(key, "top") == 0) {
		infop->base_l->visible_t = visible;
	    } else if (strcasecmp(key, "bottom") == 0) {
		infop->base_l->visible_b = visible;
	    }
	    break;;
	case 3: /* [controls] */
	    switch(value[0]) {
		case '0':
		case 'n': 
		case 'N': 
		case 'f': 
		case 'F': 
		    ok = false;
		    break;;
		default:
		    if (strcasecmp(value, "off") == 0) {
			ok = false;
		    } else {
			ok = true;
		    }
	    }
	    if (!ok) break;

	    /* else: */
	    if (strcasecmp(key, "right") == 0) {
		infop->base_l->controls |= DIR_RIGHT;
	    } else if (strcasecmp(key, "left") == 0) {
		infop->base_l->controls |= DIR_LEFT;
	    } else if (strcasecmp(key, "up") == 0) {
		infop->base_l->controls |= DIR_UP;
	    } else if (strcasecmp(key, "down") == 0) {
		infop->base_l->controls |= DIR_DOWN;
	    }

	    break;;

	case 4: /* overlay */
	    ov_infop = infop->base_l->internal;
	    if (strcasecmp(key, "type") == 0) {
		if (infop->base_l->overlays == NULL) {
		    ov_infop = malloc(sizeof(struct ov_info));
		}
		if (!(ov_infop->t = get_overlay_type(value))) {
		    fprintf(stderr, "Unknown overlay type: %s\n", value);
		    return false;
		}
		lop = ov_infop->t->alloc(ov_infop->t);
		if (infop->base_l->overlays == NULL) {
		    infop->base_l->overlays = lop;
		} else {
		    ov_infop->o->next = lop;
		}
		ov_infop->o = lop;
		infop->base_l->internal = ov_infop;
	    } else {
		ov_infop->t->construct(ov_infop->o, key, value);
	    }
	    break;;
    }

    return true;
}

Level *
_load_level(struct level_cfg_info *infop)
{
    Level *l = infop->base_l;
    bool ok;
    SDL_Surface *bg_l;

    chdir(infop->working_dir);

    ok = load_bitmap(infop->bitmap_path, &l->w, &l->h, &l->bits);
    if (!ok) {
	fprintf(stderr, "Error loading bit-map.\n");
	return NULL;
    }

    if (!(bg_l = img_from_file(infop->bg_path))) {
	fprintf(stderr, "Error loading background: %s\n", IMG_GetError());
	return false;
    }
    l->bg = SDL_DisplayFormat(bg_l);
    SDL_FreeSurface(bg_l);

    l->internal = infop;

    chdir_home();

    return l;
}

void
_unload_level(Level *l)
{
    SDL_FreeSurface(l->bg);
    free(l->bits);
}


void
_del_level(Level *l)
{
    _unload_level(l);
    free(l->internal);
    free(l);
}

static LevelList *
load_level_from_cfg(char *filename, LevelList *prev)
{
    FILE *f_cfg;
    char *cp, *cp0;

    LevelList *ll;
    Level *l;
    struct level_cfg_info *infop;

    static struct cfg_section sections[] = {
	{ 1, "level" },
	{ 2, "viewport" },
	{ 3, "controls" },
	{ 4, "overlay" },
	{ 0, NULL } /* sentinel */
    };

    if (!(f_cfg = fopen(filename, "r"))) {
	fprintf(stderr, "Failed to load %s. Continuing...\n", filename);
	return NULL;
    }

    /* create scratch Level/LevelList */
    ll = malloc(sizeof(LevelList));
    l = malloc(sizeof(Level));
    infop = malloc(sizeof(struct level_cfg_info));
    infop->base_l = l;
    ll->level = NULL;
    ll->prev = prev;
    ll->next = NULL;
    ll->load = &_load_level;
    ll->load_arg = infop;

    l->visible_r = l->visible_l = l->visible_t = l->visible_b = 0.2;
    l->controls = 0;
    l->del = &_del_level;
    l->unload = &_unload_level;
    l->main_sprite = NULL; 
    l->overlays = NULL;

    /* chdir to the right place, since paths are relative */
    for (cp = filename; *cp; ++cp) if (*cp == '/') cp0 = cp;
    *cp0 = '\0';
    chdir(filename);
    infop->working_dir = strdup(filename);
    *cp0 = '/';

    if (!read_cfg_file(f_cfg, sections, &load_callback, ll)) {
	free(ll->level);
	free(ll);
	ll = NULL;
    }

    fclose(f_cfg);

    return ll;
}


