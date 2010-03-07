#include "menu.h"

Menu *menu_root;
Menu *menu_current;

extern Level *load_lv0();

void menu_tic()
{
    static int painted = 0;
    static directional_t old_controls = 0;
 
    if (!painted) {
	paint_menu();
	painted = 1;
    }
    
    if (controls_held != old_controls) {
	old_controls = controls_held;
	painted = 0;
	if (controls_held & DIR_UP) {
	    menu_current = menu_current->up;
	}
	if (controls_held & DIR_DOWN) {
	    menu_current = menu_current->down;
	}
	if (controls_held & CTRL_ENTER) {
	    menu_current->exec();
	}
    }
}

static void startgame()
{
    printf ("resetting game\n");
    thegame.xpos = thegame.ypos = 0;
    thegame.current_level = load_lv0();
    thegame.heli_xpos = thegame.current_level->heli_x0;
    thegame.heli_ypos = thegame.current_level->heli_y0;
    thegame.running = 1;
    thegame.in_menu = 0;
}

static void quitgame()
{
    thegame.running = 0;
}

void init_menu()
{
    Menu *mstart, *mquit;
    
    mstart = malloc(sizeof(Menu));
    mquit  = malloc(sizeof(Menu));
    
    mstart->text = "Start Game";
    mstart->up   = mstart;
    mstart->down = mquit;
    mstart->exec = &startgame;
    
    mquit->text = "Quit";
    mquit->up   = mstart;
    mquit->down = mquit;
    mquit->exec = &quitgame;
    
    menu_root = mstart;
    menu_current = menu_root;
}

void free_menu()
{
}
