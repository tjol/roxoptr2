/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*! \file */

#ifndef R_CFGPARSER_H_
#define R_CFGPARSER_H_

#include <stdbool.h>

#include <stdio.h>

#include <string.h>

/*! \see read_cfg_file() */
struct cfg_section {
    int id;	/*!< Not used by read_cfg_file(). Useful for callback code. */
    char *name; /*!< Section title as it appears in the CFG file. */
};

/*!
 * \brief	Function to be called by read_cfg_file().
 */
typedef bool (*cfg_callback_f)(struct cfg_section *, const char *, const char *, void *);

/*!
 * \brief Parse a .cfg (INI format) file.
 *
 * \param f		FILE pointer to read from.
 * \param sections	Array of file sections to read. Must be terminated by
 * 			<CODE>{ 0, NULL }</CODE>. Sections not listed are ignored.
 * \param cb		Function to be called for every <CODE>KEY = VALUE</CODE> line.
 */
bool read_cfg_file(FILE *f, struct cfg_section *sections, cfg_callback_f cb, void *cb_arg);

#endif

