/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2010 Thomas Jollans                                       *
 *                                                                         *
 * This program is free software. You may redistribute and/or modify       *
 * it under the terms of the ``MIT License'' as it is reproduced in the    *
 * file ``COPYING''.                                                       *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*! \file */

#ifndef R_OVERLAYS_H
#define R_OVERLAYS_H

#include <stdbool.h>

typedef struct level_overlay Overlay;

#include "level.h"

/*! \brief Linked list of overlays to be displayed/applied in a level. */
struct level_overlay {
    struct level_overlay *next;

    /*! \brief Display things on-screen.
     *
     * If there is nothing to display, exit ASAP.
     *
     * \see is_visible()
     * \see display_rect()
     */
    void (*paint)(Level *l, struct level_overlay *ov, SDL_Surface *screen);

    /*! \brief run additional game logic
     *
     * Check for heli/overlay collisions etc.
     *
     * \see collide_bitmaps()
     * \returns false to signalize that the player has lost, true otherwise
     */
    bool (*keep_running)(Level *l, struct level_overlay *ov); 

    void (*del)(struct level_overlay *ov);/*! \brief Destructor. */

    void *internal;
};

/*! \brief Configuration-visible overlay type. */
struct overlay_type {
    char *name; /*!< \brief Type name as used in level cfg files. */

    Overlay *(*alloc)(struct overlay_type *t); /*!< \brief Create and return a new overlay */
    /*! \brief Configure a new overlay object.
     * Called for each line in the level cfg <CODE>[OVERLAY]</CODE> section.
     */
    bool (*construct)(Overlay *o, const char *key, const char *value);
};

/*! \returns The #overlay_type with the given name */
struct overlay_type *get_overlay_type(const char *name);

/*! \brief Check whether a rectangle would be visible on-screen. */
bool is_visible(int x, int y, int width, int height);
/*! \brief Map level-internal coordinates to on-screen coordinates.
 *
 * Use this before blitting in an Overlay::paint() function
 */
SDL_Rect display_rect(int x, int y);

extern struct overlay_type overlay_types[];
extern size_t overlay_types_len;

#endif

