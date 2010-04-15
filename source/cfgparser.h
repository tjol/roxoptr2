/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef R_CFGPARSER_H_
#define R_CFGPARSER_H_

#include <stdbool.h>

#include <stdio.h>

#include <string.h>

struct cfg_section {
    int id;
    char *name;
};

typedef bool (*cfg_callback_f)(struct cfg_section *, const char *, const char *, void *);

bool read_cfg_file(FILE *f, struct cfg_section *sections, cfg_callback_f cb, void *cb_arg);

#endif

