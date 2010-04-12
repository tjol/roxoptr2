/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef R_FILESYS_H_
#define R_FILESYS_H_

#include "level.h"
#include <zlib.h>
#ifdef _WINDOWS
#  include <direct.h>
#else
#  include <unistd.h>
#endif
#include <errno.h>

int init_fs();

int load_gzpbm(const char *fname, Level *l);
int load_zpbm(const char *fname, Level *l);
int load_pbm(const char *fname, Level *l);

typedef size_t (*get_chunk_f)(unsigned char **, size_t *, void *);

size_t f_getchunk (unsigned char **pbuf, size_t *plen, FILE *arg);
size_t gz_getchunk (unsigned char **pbuf, size_t *plen, gzFile arg);
size_t zlib_getchunk (unsigned char **pbuf, size_t *plen, void *arg);
void *init_zlib_getchunk(const char *fname);
void finalize_zlib_getchunk(void *arg);

static inline char *get_pwd_w_extra(size_t alloc_extra)
{
    char *cwd;
    size_t len = 256;

    cwd = malloc(256);
    while (!getcwd(cwd, len-alloc_extra)) {
	if (errno == ERANGE) {
	    len += 256;
	    cwd = realloc(cwd, len);
	} else {
	    free(cwd);
	    return NULL;
	}
    }

    return cwd;
}

#define get_pwd() get_pwd_w_extra(0)

#endif


