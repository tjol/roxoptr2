/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*! \file */

#ifndef R_FILESYS_H_
#define R_FILESYS_H_

#include <zlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include <stdbool.h>

/*! \brief Initialize file system.
 *
 * It is safe to call this function multiple times. If in doubt, call
 * before loading any files. (Crucial on the Wii)
 */
bool init_fs();

bool load_bitmap(const char *fname, unsigned *w, unsigned *h, unsigned char **bitsbuf);
bool load_gzpbm(const char *fname, unsigned *w, unsigned *h, unsigned char **bitsbuf);
/*bool load_zpbm(const char *fname, unsigned *w, unsigned *h, unsigned char **bitsbuf);*/
bool load_pbm(const char *fname, unsigned *w, unsigned *h, unsigned char **bitsbuf);

typedef size_t (*get_chunk_f)(unsigned char **, size_t *, void *);

size_t f_getchunk (unsigned char **pbuf, size_t *plen, FILE *arg);
size_t gz_getchunk (unsigned char **pbuf, size_t *plen, gzFile arg);
size_t zlib_getchunk (unsigned char **pbuf, size_t *plen, void *arg);
void *init_zlib_getchunk(const char *fname);
void finalize_zlib_getchunk(void *arg);

/*! \brief Get the current working directory
 *
 * \param alloc_extra	Extra space to allocate. (in case you want to append something)
 */
char *get_pwd_w_extra(size_t alloc_extra);

/*! \see get_pwd_w_extra(size_t) */
#define get_pwd() get_pwd_w_extra(0)

/*! \brief Go back to the installation directory. */
void chdir_home();
/*! \brief Make a path relative to the installation directory absolute. */
char *path_from_home(const char *relp);

#if ! HAVE_ACCESS
int access(const char *pathname, int mode);
#endif

#endif


