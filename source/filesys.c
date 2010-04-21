/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "filesys.h"
#include "level.h"

#include <stdio.h>
#include <zlib.h>
#ifdef WII
# include <fat.h>
#endif

#define R_CHUNK 512


static bool load_pbm_from(get_chunk_f get_more, void *arg, Level *l);

static char *game_inst_path;

bool init_fs()
{
#ifdef WII
    static bool fs_initialized = false;

    if (!fs_initialized) {
	if (!fatInitDefault()) {
	    return false;
	}
	if (chdir("/roxoptr2") != 0) { /* this replaces the normal CWD */
	    chdir("/");
	}
	fs_initialized = true;
    }
#else
#  ifdef ROXOPTR2_DATADIR
    if (access("icon.bmp.gz", R_OK) != 0 || access("leveldata", R_OK) != 0) {
	/* This isn't the installation directory. chdir to the right place. */
	chdir(ROXOPTR2_DATADIR);
    }
#  endif
#endif
    game_inst_path = get_pwd();
    return true;
}

char *get_pwd_w_extra(size_t alloc_extra)
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

void chdir_home()
{
    (void)chdir(game_inst_path);
}

char *path_from_home(const char *relp)
{
    char *ret = malloc(strlen(game_inst_path) + strlen(relp) + 2);
    char *cpi, *cpo;
    for (cpi = game_inst_path, cpo = ret; *cpi; *(cpo++) = *(cpi++));
    *(cpo++) = '/';
    strcpy(cpo, relp);
    return ret;
}


bool
load_gzpbm(const char *fname, Level *l)
{
    gzFile pf;

    if (!(pf = gzopen(fname, "r"))) {
	fprintf(stderr, "Error opening gz file.\n");
	return false;
    }

    return load_pbm_from((get_chunk_f)&gz_getchunk, pf, l);
}

bool
load_pbm(const char *fname, Level *l)
{
    FILE *pf;

    if (!(pf = fopen(fname, "r"))) {
	perror(0);
	return false;
    }

    return load_pbm_from((get_chunk_f)&f_getchunk, pf, l);
}

static bool
load_pbm_from(get_chunk_f get_more, void *arg, Level *l)
{
    unsigned char *buf;
    unsigned char *end;
    size_t len;

    int is_ascii;

    int eofp = 0;

    int in_header = 1;
    int ignore_line;
    char size_line[20];
    int i;

    unsigned char *bits;
    unsigned char *lastplus1;
    unsigned char *thisbyte;
    int w, h;
    int x;
    int shift;

    get_more(&buf, &len, arg);
    end = buf + len;

    if (buf[0] == 'P' && buf[1] == '1') {
	is_ascii = 1;
    } else if (buf[0] == 'P' && buf[1] == '4') {
	is_ascii = 0;
    } else {
	fprintf(stderr, "Error: This is not a PBM file.\n");
	return false;
    }

    buf += 2;
    ignore_line = 1;

    while (in_header) {
	if (buf == end) {
	    get_more(&buf, &len, arg);
	    end = buf + len;
	}
	if (ignore_line) {
	    if (*(buf++) == '\n') {
		switch (*buf) {
		    case '#':
			ignore_line = 1;
			break;
		    default:
			i = 0;
			ignore_line = 0;
			break;
		}
	    }
	} else {
	    /* I'm not ignoring the line. This must be the size info. */
	    if (*buf == '\n') {
		in_header = 0;
		size_line[i] = '\0';
		sscanf(size_line, "%d %d", &w, &h);
		l->w = w;
		l->h = h;
		/*temp*/x = ((w+7) >> 3) * h;
		bits = malloc(x);
		thisbyte = bits;
		lastplus1 = bits + x;
	    }
	    size_line[i++] = *buf;
	    ++buf;
	}
    }

    x = 0;
    shift = 0;

    while ((!eofp) && thisbyte != lastplus1) {
	if (buf == end) {
	    get_more(&buf, &len, arg);
	    end = buf + len;
	    if (len == 0) {
		eofp = 1;
		break;
	    }
	}
	if (is_ascii)
	    switch (*(buf++)) {
		case ' ':
		case '\n':
		case '\t':
		    break;
		case '1':
		    *thisbyte |= 1 << shift;
		case '0':
		    if ( (++x) == w		/* end of line */
		       ||(++shift) > 7) {	/* end of byte */
			*(++thisbyte) = 0;
			shift = 0;
		    }
		    break;
	    }
	else
	    /* P4 netpbm fills up the last byte of a row just like
	     * xbm, so we don't have to do any work ourselves. */
	    *(thisbyte++) = *(buf++); 
    }

    l->bits = bits;
    return true;
}

size_t
f_getchunk(unsigned char **pbuf, size_t *plen, FILE *arg)
{
    static unsigned char buf[R_CHUNK];
    size_t len;
    FILE *pf = arg;

    len = fread(buf, 1, R_CHUNK, pf);
    *plen = len;
    *pbuf = buf;
    return len;
}

size_t
gz_getchunk(unsigned char **pbuf, size_t *plen, gzFile arg)
{
    static unsigned char buf[R_CHUNK];
    size_t len;
    gzFile pf = arg;

    len = gzread(pf, buf, R_CHUNK);
    *plen = len;
    *pbuf = buf;
    return len;
}

size_t
zlib_getchunk(unsigned char **pbuf, size_t *plen, void *arg)
{
    static unsigned char in[R_CHUNK];
    static unsigned char out[R_CHUNK];
    static z_stream *pstrm;

    FILE *pf;
    int ret;

    if (pbuf == NULL) {
	if (plen == NULL) {
	    /* init call. */
	    pstrm = arg;
	} else if (*plen == 0) {
	    /* finalizing call */
	    pf = arg;
	    fclose(pf);
	    inflateEnd(pstrm);
	}
	return -1;
    }

    pf = arg;

    if (pstrm->avail_out != 0) {
	/* space in the out buffer => needs more input. */
	pstrm->avail_in = fread(in, 1, R_CHUNK, pf);
	if(pstrm->avail_in == 0) { /* EOF or I/O error. */
	    if (ferror(pf)) {
		perror(0);
	    }
	    return 0;
	}
	pstrm->next_in = in;
    }

    pstrm->avail_out = R_CHUNK;
    pstrm->next_out = out;
    ret = inflate(pstrm, Z_NO_FLUSH);
    switch (ret) {
	case Z_NEED_DICT:
	case Z_DATA_ERROR:
	case Z_MEM_ERROR:
	case Z_STREAM_ERROR:
	    fprintf(stderr, "Zlib error.\n");
	    return 0;
    }

    /* got output. return. */
    *plen = R_CHUNK - pstrm->avail_out;
    *pbuf = out;
    return *plen;
}

void *
init_zlib_getchunk(const char *fname)
{
    FILE *pf;
    z_stream strm;
    
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;

    if (inflateInit(&strm) != Z_OK) {
	fprintf(stderr, "Error initializing zlib.\n");
	return 0;
    }

    if (!(pf = fopen(fname, "rb"))) {
	perror(0);
	return 0;
    }

    /* initialize _read_gz state */
    zlib_getchunk(NULL, NULL, &strm);

    return pf;
}

void
finalize_zlib_getchunk(void *arg)
{
    static size_t cmd = 0;

    zlib_getchunk(NULL, &cmd, arg);
}


#if ! HAVE_ACCESS
int access(const char *pathname, int mode)
{
    FILE *fp;
    int ret;
    fp = fopen(pathname, "rb");
    if (fp == NULL) {
	ret = -1;
    } else {
	ret = 0;
	fclose(fp);
    }
    return ret;
    
}
#endif

