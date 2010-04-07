/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef WII

#include "cfgparser.h"

#include <string.h>
#include <ctype.h>

#define LINE_LENGTH 256

void read_cfg_file(FILE *f, struct cfg_section *sections, 
                   void (*cb)(struct cfg_section *, const char *, const char *, void *),
                   void *cb_arg)
{
    char line[LINE_LENGTH];

    char buf1[LINE_LENGTH];
    char buf2[LINE_LENGTH];

    char *ln;
    char *cpi, *cpo;
    char *cp;

    struct cfg_section *s = NULL;

    while (!feof(f)) {
	if (fgets(line, LINE_LENGTH, f) == NULL) {
	    return;
	}

	/* strip padding whitespace */
	for (ln = line; isspace(*ln); ++ln);
	cp = ln-1; /* if all chars are whitespace, set ln[0] to '\0' */
	for (cpi = ln; *cpi != '\0'; ++cpi)
	    if (!isspace(*cpi))
		cp = cpi;
	*(cp+1) = '\0';

	switch (ln[0]) {
	    case ';': /* comment */
	    case '\0':
		break;

	    case '[': /* section */
		for (cpi=ln+1, cpo=buf1;
		     *cpi != ']' && *cpi != '\0';
		     *(cpo++) = *(cpi++));
		*cpo = '\0';
		for (s = sections;
		     s->name != NULL;
		     ++s) {
		    if (strcasecmp(buf1, s->name) == 0) {
			break;
		    }
		}
		if (s->name == NULL) {
		    s = NULL;
		}
		break;

	    default:
		/* non-empty, non-comment, non-section line. */
		if (s == NULL) { /* not in a known section => ignore */
		    break;
		}
		cp = buf1-1;
		for (cpi = ln, cpo = buf1;
		     *cpi != '\0' && *cpi != '=';
		     *(cpo++) = *(cpi++))
			if (!isspace(*cpi))
			    cp = cpo;
		*(cp+1) = '\0';
		if (*cpi == '=') {
		    while (isspace(*(++cpi)));
		    for (cpo = buf2;
			 *cpi != '\0';
			 *(cpo++) = *(cpi++));
		    *cpo = '\0';
		} else {
		    buf2[0] = '\0';
		}
		(*cb)(s, buf1, buf2, cb_arg);
	}
    }
}

#endif

