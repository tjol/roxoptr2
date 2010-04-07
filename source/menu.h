#ifndef R_MENU_H_
#define R_MENU_H_

#include "main.h"

typedef struct menu Menu;
typedef void (*Menuaction_f)(void*);

struct menu {
    char *text;
    
    Menu *up;
    Menu *down;
    
    Menuaction_f exec;
    void *exec_arg;
};

extern Menu *menu_root;
extern Menu *menu_current;

void init_menu();
void free_menu();

void menu_tic();

#endif

