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

#include <sys/types.h>

#include "cfgparser.h"
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

#include <SDL/SDL_image.h>

extern Level *load_lv0();
extern Level *load_lv1();

LevelList *levels = NULL;
LevelList *current_level = NULL;

void free_levels();

static LevelList *load_level_from_cfg(char *filename, LevelList *prev);

void init_levels()
{
    LevelList *lv0, *lv1;
    LevelList *prev;
    LevelList *cur;

    FILE *fp_levels_list;
    char lbuf[256];
    char *line;
    char *cp, *cp_nw;
    char *cwda;
    char *cwd_end;
    
    lv0 = malloc(sizeof(LevelList));
    lv1 = malloc(sizeof(LevelList));
    
    lv0->level = NULL;
    lv0->prev = NULL;
    lv0->next = lv1;
    lv0->load = &load_lv0;
    lv0->title = "Level #0";
    
    lv1->level = NULL;
    lv1->prev = lv0;
    lv1->next = NULL;
    lv1->load = &load_lv1;
    lv1->title = "Level #1";

    cur = NULL;
    prev = lv1;
    
    init_fs();

    cwda = malloc(512);
    if(!getcwd(cwda, 246)) { /* leave room for /leveldata/[etc etc] */
	if (errno == ERANGE) {
	    /* allocate longer buffer.
	     * on linux, this is 4K.  */
	    free(cwda);
	    cwda = malloc(PATH_MAX);
	    if(!getcwd(cwda, PATH_MAX-10)) {
		fprintf(stderr, "Unable to get CWD.\n");
		exit(1);
	    }
	}
	if(!getcwd(cwda, PATH_MAX-10)) {
		fprintf(stderr, "Unable to get CWD.\n");
		exit(1);
	}
    }
    strcat(cwda, "/leveldata/");
    for(cwd_end = cwda; *cwd_end; ++cwd_end);

    fp_levels_list = fopen("leveldata/levels.list", "r");
    if (!fp_levels_list) {
	perror(0);
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

int start_level(LevelList *ll)
{
    if (!ll) return 0;
    
    if (! ll->level) {
	ll->level = ll->load();
    }
    
    thegame.xpos = thegame.ypos = 0;
    thegame.current_level = ll->level;
    thegame.heli_xpos = thegame.current_level->heli_x0;
    thegame.heli_ypos = thegame.current_level->heli_y0;
   
    current_level = ll;
    
    return 1;
}

void
load_callback(struct cfg_section *sect, const char *key, const char *value, void *ll_)
{
    LevelList *ll = ll_;
    SDL_Surface *bg_l;
    int l;
    double xd,yd;
    unsigned int xu, yu;
    double visible;
    int ok;
    FILE *fp;
    SDL_RWops *rw;

    switch (sect->id) {
	case 1: /* [level] */
	    if (strcasecmp(key, "name") == 0) {
		ll->title = strdup(value);
	    } else if (strcasecmp(key, "bits") == 0) {
		/* file type? */
		l = strlen(value);
		if (strcasecmp(value+l-7, ".pbm.gz") == 0) {
		    ok = load_gzpbm(value, ll->level);
		} else if (strcasecmp(value+l-4, ".pbm") == 0) {
		    ok = load_pbm(value, ll->level);
		} else {
		    fprintf(stderr, "%s: error: unknown bit-map format.\n", value);
		    exit(1);
		}
		if (!ok) {
		    fprintf(stderr, "Error loading bit-map.\n");
		    exit(1);
		}
	    } else if (strcasecmp(key, "background") == 0) {
		/* bg_l = IMG_Load(value); */
		fp = fopen(value, "rb");
		if (!fp) {
		    perror(0);
		    exit(1);
		}
		rw = SDL_RWFromFP(fp, 1);
		bg_l = IMG_Load_RW(rw, 0);
		if (!bg_l) {
		    fprintf(stderr, "Error loading background: %s\n", IMG_GetError());
		    exit(1);
		}
		ll->level->bg = SDL_DisplayFormat(bg_l);
		SDL_FreeSurface(bg_l);
	    } else if (strcasecmp(key, "start") == 0) {
		sscanf(value, "%u %u", &xu, &yu);
		ll->level->heli_x0 = xu;
		ll->level->heli_y0 = yu;
	    } else if (strcasecmp(key, "end") == 0) {
		sscanf(value, "%u %u", &xu, &yu);
		ll->level->fin_x = xu;
		ll->level->fin_y = yu;
	    } else if (strcasecmp(key, "null_velocity") == 0) {
		sscanf(value, "%lf %lf", &xd, &yd);
		ll->level->vx = xd;
		ll->level->vy = yd;
	    } else if (strcasecmp(key, "velocity_delta") == 0) {
		sscanf(value, "%lf %lf", &xd, &yd);
		ll->level->Dvx = xd;
		ll->level->Dvy = yd;
	    }
	    break;;
	case 2: /* [viewport] */
	    sscanf(value, "%lf%%", &visible);
	    visible /= 100.0;
	    if (strcasecmp(key, "right") == 0) {
		ll->level->visible_r = visible;
	    } else if (strcasecmp(key, "left") == 0) {
		ll->level->visible_l = visible;
	    } else if (strcasecmp(key, "top") == 0) {
		ll->level->visible_t = visible;
	    } else if (strcasecmp(key, "bottom") == 0) {
		ll->level->visible_b = visible;
	    }
	    break;;
	case 3: /* [controls] */
	    switch(value[0]) {
		case '0':
		case 'n': 
		case 'N': 
		case 'f': 
		case 'F': 
		    ok = 0;
		    break;;
		default:
		    if (strcasecmp(value, "off") == 0) {
			ok = 0;
		    } else {
			ok = 1;
		    }
	    }
	    if (!ok) break;

	    /* else: */
	    if (strcasecmp(key, "right") == 0) {
		ll->level->controls |= DIR_RIGHT;
	    } else if (strcasecmp(key, "left") == 0) {
		ll->level->controls |= DIR_LEFT;
	    } else if (strcasecmp(key, "up") == 0) {
		ll->level->controls |= DIR_UP;
	    } else if (strcasecmp(key, "down") == 0) {
		ll->level->controls |= DIR_DOWN;
	    }

	    break;;
    }
}

void
_del_level(Level *l)
{
    SDL_FreeSurface(l->bg);
    free(l->bits);

    free(l);
}

static LevelList *
load_level_from_cfg(char *filename, LevelList *prev)
{
    FILE *f_cfg;
    char *cp, *cp0;

    LevelList *ll;
    Level *l;

    static struct cfg_section sections[] = {
	{ 1, "level" },
	{ 2, "viewport" },
	{ 3, "controls" },
	{ 0, NULL } /* sentinel */
    };

    if (!(f_cfg = fopen(filename, "r"))) {
	fprintf(stderr, "Failed to load %s. Continuing...\n", filename);
	return NULL;
    }

    /* chdir to the right place, since paths are relative */
    for (cp = filename; *cp; ++cp) if (*cp == '/') cp0 = cp;
    *cp0 = '\0';
    chdir(filename);
    *cp0 = '/';

    /* create scratch Level/LevelList */
    ll = malloc(sizeof(LevelList));
    l = malloc(sizeof(Level));
    ll->level = l;
    ll->prev = prev;
    ll->next = NULL;
    ll->load = NULL;

    l->visible_r = l->visible_l = l->visible_t = l->visible_b = 0.2;
    l->controls = 0;
    l->del = &_del_level;

    read_cfg_file(f_cfg, sections, &load_callback, ll);

    fclose(f_cfg);

    return ll;
}


