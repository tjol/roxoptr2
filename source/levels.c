#include "level.h"
#include "main.h"

#include <sys/types.h>

#ifndef WII
# include "cfgparser.h"
# include <unistd.h>
# include <dirent.h>
# include <stdio.h>
# include <limits.h>
# include <errno.h>
# include <string.h>
# include <zlib.h>
#endif

#include <SDL/SDL_image.h>

#define R_CHUNK 512

extern Level *load_lv0();
extern Level *load_lv1();

LevelList *levels = NULL;
LevelList *current_level = NULL;

void free_levels();

#ifndef WII
static LevelList *load_level_from_dir(const char *dirname, LevelList *prev);
#endif

void init_levels()
{
    LevelList *lv0, *lv1;
    LevelList *prev;
    LevelList *cur;

#   ifndef WII
    DIR *lvdat;
    char *cwda;
    char *ld_dir;
    int ld_n_len;
    char *dirname;
    struct dirent de;
    struct dirent *dep;
#   endif
    
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

#   ifndef WII
    cur = NULL;
    prev = lv1;
    
    /* find levels */
    cwda = malloc(256);
    if(!getcwd(cwda, 246)) { /* +10 ch extra for '/leveldata' */
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
    ld_dir = cwda;
    cwda = strdup(ld_dir);
    strcat(ld_dir, "/leveldata");
    ld_n_len = strlen(ld_dir);

    if (!(lvdat = opendir(ld_dir))) {
	fprintf(stderr, "The leveldata/ directory is missing. What a shame.\n");
    } else {
	for (;;) {
	    if (readdir_r(lvdat, &de, &dep) != 0) {
		/* error */
		break;
	    }
	    if (dep == NULL) {
		/* done */
		break;
	    }

	    if (de.d_type == DT_DIR) {
		if (   strcmp(de.d_name, ".") == 0
		    || strcmp(de.d_name, "..") == 0 ) continue;
		dirname = malloc(ld_n_len + 258);
		strcpy(dirname, ld_dir);
		strcat(dirname, "/");
		strcat(dirname, de.d_name);
		if (cur = load_level_from_dir(dirname, prev)) {
		    /* success ! */
		    prev->next = cur;
		    prev = cur;
		}
		free(dirname);
	    }
	}
    }
    chdir(cwda);
    free(cwda);
    free(ld_dir);

#   endif

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

#ifndef WII
int load_gzpbm(const char *fname, Level *l);
int load_zpbm(const char *fname, Level *l);
int load_pbm(const char *fname, Level *l);

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
		bg_l = IMG_Load(value);
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
load_level_from_dir(const char *dirname, LevelList *prev)
{
    FILE *f_cfg;
    /*char *f_cfg_name;*/

    LevelList *ll;
    Level *l;

    static struct cfg_section sections[] = {
	{ 1, "level" },
	{ 2, "viewport" },
	{ 3, "controls" },
	{ 0, NULL } /* sentinel */
    };

    chdir(dirname);
    if (!(f_cfg = fopen("level.cfg", "r"))) {
	fprintf(stderr, "Failed to load %s. Continuing...\n", dirname);
	return NULL;
    }

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

static int _load_pbm(size_t (*get_more)(unsigned char **, size_t *, void *),
		     void *arg, Level *l);

static size_t _read_gz(unsigned char **pbuf, size_t *plen, void *arg);
static size_t _read_f (unsigned char **pbuf, size_t *plen, void *arg);

int
load_gzpbm(const char *fname, Level *l)
{
    gzFile pf;

    if (!(pf = gzopen(fname, "r"))) {
	fprintf(stderr, "Error opening gz file.\n");
	return 0;
    }

    return _load_pbm(&_read_gz, pf, l);
}

int
load_pbm(const char *fname, Level *l)
{
    FILE *pf;

    if (!(pf = fopen(fname, "r"))) {
	perror(0);
	return 0;
    }

    return _load_pbm(&_read_f, pf, l);
}

static int
_load_pbm(size_t (*get_more)(unsigned char **, size_t *, void *),
	  void *arg, Level *l)
{
    unsigned char *buf;
    unsigned char *end;
    size_t len;

    int eofp = 0;

    int in_header = 1;
    int ignore_line;
    char size_line[20];
    int i;

    unsigned char *bits;
    unsigned char *thisbyte;
    int w, h;
    int x;
    int shift;

    get_more(&buf, &len, arg);
    end = buf + len;

    if (! (buf[0] == 'P' && buf[1] == '1') ) {
	fprintf(stderr, "Error: This is not a PBM file.\n");
	for (; buf != end; ++buf) putchar(*buf);
	return 0;
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
		bits = malloc(((w+7) >> 3) * h);
		thisbyte = bits;
	    }
	    size_line[i++] = *buf;
	    ++buf;
	}
    }

    x = 0;
    shift = 0;

    while (!eofp) {
	if (buf == end) {
	    get_more(&buf, &len, arg);
	    end = buf + len;
	    if (len == 0) {
		eofp = 1;
		break;
	    }
	}
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
    }

    l->bits = bits;
    return 1;
}

static size_t
_read_f (unsigned char **pbuf, size_t *plen, void *arg)
{
    static unsigned char buf[R_CHUNK];
    size_t len;
    FILE *pf = arg;

    len = fread(buf, 1, R_CHUNK, pf);
    *plen = len;
    *pbuf = buf;
    return len;
}

static size_t
_read_gz (unsigned char **pbuf, size_t *plen, void *arg)
{
    static unsigned char buf[R_CHUNK];
    size_t len;
    gzFile pf = arg;

    len = gzread(pf, buf, R_CHUNK);
    *plen = len;
    *pbuf = buf;
    return len;
}

/* The same thing with the pure zlib format, for your amusement.
 * Not particularly useful since nobody actually creates files like these.
 */

#if 0

static size_t _read_zlib(unsigned char **pbuf, size_t *plen, void *arg);

int 
load_zpbm(const char *fname, Level *l)
{
    FILE *pf;
    z_stream strm;
    int ret;
    
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
    _read_gz(NULL, NULL, &strm);

    ret = _load_pbm(_read_gz, pf, l);
    inflateEnd(&strm);
    return ret;
}

static size_t
_read_zlib(unsigned char **pbuf, size_t *plen, void *arg)
{
    static unsigned char in[R_CHUNK];
    static unsigned char out[R_CHUNK];
    static z_stream *pstrm;

    FILE *pf;
    int ret;

    if (pbuf == NULL && plen == NULL) {
	/* init call. */
	pstrm = arg;
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

#endif

#endif /* !WII */

