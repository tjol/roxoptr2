#ifndef R_CFGPARSER_H_
#define R_CFGPARSER_H_

#include <stdio.h>

struct cfg_section {
    int id;
    char *name;
};

void read_cfg_file(FILE *f, struct cfg_section *sections, 
                   void (*cb)(struct cfg_section *, const char *, const char *, void *),
                   void *cb_arg);

#endif

