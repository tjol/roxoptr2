#include "level.h"
#include "main.h"

extern Level *load_lv0();
extern Level *load_lv1();

LevelList *levels = NULL;
LevelList *current_level = NULL;

void free_levels();

void init_levels()
{
    LevelList *lv0, *lv1;
    
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

