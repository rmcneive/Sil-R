/**
 * \file ui-map.c
 * \brief Writing level map info to the screen
 *
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"
#include "cave.h"
#include "grafmode.h"
#include "init.h"
#include "mon-predicate.h"
#include "mon-util.h"
#include "monster.h"
#include "obj-tval.h"
#include "obj-util.h"
#include "player-timed.h"
#include "trap.h"
#include "ui-display.h"
#include "ui-input.h"
#include "ui-map.h"
#include "ui-object.h"
#include "ui-output.h"
#include "ui-prefs.h"
#include "ui-term.h"


/**
 * Graphics in a mini-screenshot array
 */
static wchar_t mini_screenshot_char[7][7];
static uint8_t mini_screenshot_attr[7][7];

/**
 * Hack -- Hallucinatory monster
 */
static void hallucinatory_monster(int midx, int *a, wchar_t *c)
{
	/* Get the image monster */
	struct monster_race *race = cave_monster(cave, midx)->image_race;

	/* Skip non-entries */
	if (!race->name) return;

	/* Retrieve attr/char */
	*a = monster_x_attr[race->ridx];
	*c = monster_x_char[race->ridx];
}


/**
 * Hack -- Hallucinatory object
 */
static void hallucinatory_object(int *a, wchar_t *c)
{
	
	while (1) {
		/* Select a random object */
		struct object_kind *kind = &k_info[randint0(z_info->k_max - 1) + 1];

		/* Skip non-entries */
		if (!kind->name) continue;
		
		/* Retrieve attr/char (HACK - without flavors) */
		*a = kind_x_attr[kind->kidx];
		*c = kind_x_char[kind->kidx];
		
		/* HACK - Skip empty entries */
		if (*a == 0 || *c == 0) continue;

		return;
	}
}


/**
 * Get the graphics of a listed trap.
 *
 * We should probably have better handling of stacked traps, but that can
 * wait until we do, in fact, have stacked traps under normal conditions.
 */
static void get_trap_graphics(struct chunk *c, struct grid_data *g, int *a,
							  wchar_t *w)
{
    /* Trap is visible */
    if (trf_has(g->trap->flags, TRF_VISIBLE) ||
		trf_has(g->trap->flags, TRF_GLYPH)) {
		/* Get the graphics */
		*a = trap_x_attr[g->lighting][g->trap->kind->tidx];
		*w = trap_x_char[g->lighting][g->trap->kind->tidx];
    }
}

/**
 * Apply text lighting effects
 */
static void grid_get_attr(struct grid_data *g, int *a)
{
	/* Save the high-bit, since it's used for attr inversion in GCU */
	int a0 = *a & 0x80;

	/* Remove the high bit so we can add it back again at the end */
	*a = (*a & 0x7F);

	/* Play with fg colours for terrain affected by torchlight */
	if (feat_is_torch(g->f_idx)) {
		/* Brighten if torchlit, darken if out of LoS, super dark for UNLIGHT */
		switch (g->lighting) {
			case LIGHTING_TORCH: *a = get_color(*a, ATTR_LIGHT, 1); break;
			case LIGHTING_LIT: *a = get_color(*a, ATTR_DARK, 1); break;
			case LIGHTING_DARK: *a = get_color(*a, ATTR_DARK, 2); break;
			default: break;
		}
	}

	/* Add the attr inversion back for GCU */
	if (a0) {
		*a = a0 | *a;
	}
	/* Hybrid or block walls */
	if (use_graphics == GRAPHICS_NONE && feat_is_wall(g->f_idx)) {
		if (OPT(player, hybrid_walls))
			*a = *a + (MAX_COLORS * BG_DARK);
		else if (OPT(player, solid_walls))
			*a = *a + (MAX_COLORS * BG_SAME);
	}
}

/**
 * This function takes a pointer to a grid info struct describing the 
 * contents of a grid location (as obtained through the function map_info)
 * and fills in the character and attr pairs for display.
 *
 * ap and cp are filled with the attr/char pair for the monster, object or 
 * floor tile that is at the "top" of the grid (monsters covering objects, 
 * which cover floor, assuming all are present).
 *
 * tap and tcp are filled with the attr/char pair for the floor, regardless
 * of what is on it.  This can be used by graphical displays with
 * transparency to place an object onto a floor tile, is desired.
 *
 * Any lighting effects are also applied to these pairs, clear monsters allow
 * the underlying colour or feature to show through (ATTR_CLEAR and
 * CHAR_CLEAR), multi-hued colour-changing (ATTR_MULTI) is applied, and so on.
 * Technically, the flag "CHAR_MULTI" is supposed to indicate that a monster 
 * looks strange when examined, but this flag is currently ignored.
 *
 * NOTES:
 * This is called pretty frequently, whenever a grid on the map display
 * needs updating, so don't overcomplicate it.
 *
 * The "zero" entry in the feature/object/monster arrays are
 * used to provide "special" attr/char codes, with "monster zero" being
 * used for the player attr/char, "object zero" being used for the "pile"
 * attr/char, and "feature zero" being used for the "darkness" attr/char.
 *
 * TODO:
 * The transformations for tile colors, or brightness for the 16x16
 * tiles should be handled differently.  One possibility would be to
 * extend feature_type with attr/char definitions for the different states.
 * This will probably be done outside of the current text->graphics mappings
 * though.
 */
void grid_data_as_text(struct grid_data *g, int *ap, wchar_t *cp, int *tap,
					   wchar_t *tcp)
{
	struct feature *feat = &f_info[g->f_idx];

	int a = feat_x_attr[g->lighting][feat->fidx];
	wchar_t c = feat_x_char[g->lighting][feat->fidx];

	/* Get the colour for ASCII */
	if (use_graphics == GRAPHICS_NONE)
		grid_get_attr(g, &a);

	/* Save the terrain info for the transparency effects */
	(*tap) = a;
	(*tcp) = c;

	/* There is a trap in this grid, and we are not hallucinating */
	if (g->trap && (!g->hallucinate)) {
	    /* Change graphics to indicate visible traps */
	    get_trap_graphics(cave, g, &a, &c);
	}

	/* If there's an object, deal with that. */
	if (g->first_kind) {
		if (g->hallucinate) {
			/* Just pick a random object to display. */
			hallucinatory_object(&a, &c);
		} else if (g->multiple_objects) {
			/* Get the "pile" feature instead */
			a = object_kind_attr(pile_kind);
			c = object_kind_char(pile_kind);
		} else {
			/* Normal attr and char */
			a = object_kind_attr(g->first_kind);
			c = object_kind_char(g->first_kind);
		}
	}

	/* Handle monsters, the player and trap borders */
	if (g->m_idx > 0) {
		if (g->hallucinate) {
			/* Just pick a random monster to display. */
			hallucinatory_monster(g->m_idx, &a, &c);
		} else if (monster_is_listened(cave_monster(cave, g->m_idx))) {
			/* Simplest possible hack - NRM */
			a = COLOUR_SLATE;
			c = '*';
		} else {
			struct monster *mon = cave_monster(cave, g->m_idx);

			uint8_t da;
			wchar_t dc;

			/* Desired attr & char */
			da = monster_x_attr[mon->race->ridx];
			dc = monster_x_char[mon->race->ridx];

			/* Special handling of attrs and/or chars */
			if (da & 0x80) {
				/* Special attr/char codes */
				a = da;
				c = dc;
			} else if (rf_has(mon->race->flags, RF_ATTR_MULTI) ||
					   rf_has(mon->race->flags, RF_ATTR_FLICKER)) {
				/* Multi-hued monster */
				a = mon->attr ? mon->attr : da;
				c = dc;
			} else if (!flags_test(mon->race->flags, RF_SIZE, RF_CHAR_CLEAR,
								   FLAG_END)) {
				/* Normal monster (not "clear" in any way) */
				a = da;
				/* Desired attr & char. da is not used, should a be set to it?*/
				/*da = monster_x_attr[mon->race->ridx];*/
				dc = monster_x_char[mon->race->ridx];
				c = dc;
			} else if (a & 0x80) {
				/* Hack -- Bizarre grid under monster */
				a = da;
				c = dc;
			} else if (!rf_has(mon->race->flags, RF_CHAR_CLEAR)) {
				/* Normal char, Clear attr, monster */
				c = dc;
			}

			/* Store the drawing attr so we can use it elsewhere */
			mon->attr = a;
		}
	} else if (g->is_player) {
		struct monster_race *race = &r_info[0];

		/* Get the "player" attr */
		a = monster_x_attr[race->ridx];
		if (!(a & 0x80)) {
			a = health_attr(player->chp, player->mhp);
		}

		/* Get the "player" char */
		c = monster_x_char[race->ridx];
	}

	/* Result */
	(*ap) = a;
	(*cp) = c;
}


/**
 * Get dimensions of a small-scale map (i.e. display_map()'s result).
 * \param term Is the terminal displaying the map.
 * \param chunk Is the chunk to display.
 * \param tw Is the tile width in characters.
 * \param th Is the tile height in characters.
 * \param mw *mw is set to the width of the small-scale map.
 * \param mh *mh is set to the height of the small-scale map.
 */
static void get_minimap_dimensions(term *t, const struct chunk *c,
	int tw, int th, int *mw, int *mh)
{
	int map_height = t->hgt - 2;
	int map_width = t->wid - 2;
	int cave_height = c->height;
	int cave_width = c->width;
	int remainder;

	if (th > 1) {
		/*
		 * Round cave height up to a multiple of the tile height
		 * (ideally want no information truncated).
		 */
		remainder = cave_height % th;
		if (remainder > 0) {
			cave_height += th - remainder;
		}

		/*
		 * Round map height down to a multiple of the tile height
		 * (don't want partial tiles overwriting the map borders).
		 */
		map_height -= map_height % th;
	}
	if (tw > 1) {
		/* As above but for the width. */
		remainder = cave_width % tw;
		if (remainder > 0) {
			cave_width += tw - remainder;
		}
		map_width -= map_width % tw;
	}

	*mh = MIN(map_height, cave_height);
	*mw = MIN(map_width, cave_width);
}


/**
 * Move the cursor to a given map location.
 */
static void move_cursor_relative_map(int y, int x)
{
	int ky, kx;

	term *old;

	int j;

	/* Scan windows */
	for (j = 0; j < ANGBAND_TERM_MAX; j++) {
		term *t = angband_term[j];

		/* No window */
		if (!t) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_MAPS))) continue;

		if (window_flag[j] & PW_MAP) {
			/* Be consistent with display_map(). */
			int map_width, map_height;

			get_minimap_dimensions(t, cave, tile_width,
				tile_height, &map_width, &map_height);

			ky = (y * map_height) / cave->height;
			if (tile_height > 1) {
				ky = ky - (ky % tile_height) + 1;
			} else {
				++ky;
			}
			kx = (x * map_width) / cave->width;
			if (tile_width > 1) {
				kx = kx - (kx % tile_width) + 1;
			} else {
				++kx;
			}
		} else {
			/* Location relative to panel */
			ky = y - t->offset_y;
			if (tile_height > 1)
				ky = tile_height * ky;

			kx = x - t->offset_x;
			if (tile_width > 1)
				kx = tile_width * kx;
		}

		/* Verify location */
		if ((ky < 0) || (ky >= t->hgt)) continue;
		if ((kx < 0) || (kx >= t->wid)) continue;

		/* Go there */
		old = Term;
		Term_activate(t);
		(void)Term_gotoxy(kx, ky);
		Term_activate(old);
	}
}


/**
 * Move the cursor to a given map location.
 *
 * The main screen will always be at least 24x80 in size.
 */
void move_cursor_relative(int y, int x)
{
	int ky, kx;
	int vy, vx;

	/* Move the cursor on map sub-windows */
	move_cursor_relative_map(y, x);

	/* Location relative to panel */
	ky = y - Term->offset_y;

	/* Verify location */
	if ((ky < 0) || (ky >= SCREEN_HGT)) return;

	/* Location relative to panel */
	kx = x - Term->offset_x;

	/* Verify location */
	if ((kx < 0) || (kx >= SCREEN_WID)) return;

	/* Location in window */
	vy = ky + ROW_MAP;

	/* Location in window */
	vx = kx + COL_MAP;

	if (tile_width > 1)
		vx += (tile_width - 1) * kx;

	if (tile_height > 1)
		vy += (tile_height - 1) * ky;

	/* Go there */
	(void)Term_gotoxy(vx, vy);
}


/**
 * Display an attr/char pair at the given map location
 *
 * Note the inline use of "panel_contains()" for efficiency.
 *
 * Note the use of "Term_queue_char()" for efficiency.
 */
static void print_rel_map(wchar_t c, uint8_t a, int y, int x)
{
	int ky, kx;

	int j;

	/* Scan windows */
	for (j = 0; j < ANGBAND_TERM_MAX; j++) {
		term *t = angband_term[j];

		/* No window */
		if (!t) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_MAPS))) continue;

		if (window_flag[j] & PW_MAP) {
			/* Be consistent with display_map(). */
			int map_width, map_height;

			get_minimap_dimensions(t, cave, tile_width,
				tile_height, &map_width, &map_height);

			kx = (x * map_width) / cave->width;
			ky = (y * map_height) / cave->height;
			if (tile_width > 1) {
				kx = kx - (kx % tile_width) + 1;
			} else {
				++kx;
			}
			if (tile_height > 1) {
				ky = ky - (ky % tile_height) + 1;
			} else {
				++ky;
			}
		} else {
			/* Location relative to panel */
			ky = y - t->offset_y;

			if (tile_height > 1) {
				ky = tile_height * ky;
				if (ky + 1 >= t->hgt) continue;
			}

			kx = x - t->offset_x;

			if (tile_width > 1) {
				kx = tile_width * kx;
				if (kx + 1 >= t->wid) continue;
			}
		}

		/* Verify location */
		if ((ky < 0) || (ky >= t->hgt)) continue;
		if ((kx < 0) || (kx >= t->wid)) continue;

		/* Hack -- Queue it */
		Term_queue_char(t, kx, ky, a, c, 0, 0);

		if ((tile_width > 1) || (tile_height > 1))
			/*
			 * The overhead view can make use of the last row in
			 * the terminal.  Others leave it be.
			 */
			Term_big_queue_char(t, kx, ky, t->hgt -
				((window_flag[j] & PW_OVERHEAD) ? 0 : ROW_BOTTOM_MAP),
				a, c, 0, 0);
	}
}



/**
 * Display an attr/char pair at the given map location
 *
 * Note the inline use of "panel_contains()" for efficiency.
 *
 * Note the use of "Term_queue_char()" for efficiency.
 *
 * The main screen will always be at least 24x80 in size.
 */
void print_rel(wchar_t c, uint8_t a, int y, int x)
{
	int ky, kx;
	int vy, vx;

	/* Print on map sub-windows */
	print_rel_map(c, a, y, x);

	/* Location relative to panel */
	ky = y - Term->offset_y;

	/* Verify location */
	if ((ky < 0) || (ky >= SCREEN_HGT)) return;

	/* Location relative to panel */
	kx = x - Term->offset_x;

	/* Verify location */
	if ((kx < 0) || (kx >= SCREEN_WID)) return;

	/* Get right position */
	vx = COL_MAP + (tile_width * kx);
	vy = ROW_MAP + (tile_height * ky);

	/* Hack -- Queue it */
	Term_queue_char(Term, vx, vy, a, c, 0, 0);

	if ((tile_width > 1) || (tile_height > 1))
		Term_big_queue_char(Term, vx, vy, ROW_MAP + SCREEN_ROWS,
			a, c, 0, 0);
  
}


static void prt_map_aux(void)
{
	int a, ta;
	wchar_t c, tc;
	struct grid_data g;

	int y, x;
	int vy, vx;
	int ty, tx;

	int j;

	/* Scan windows */
	for (j = 0; j < ANGBAND_TERM_MAX; j++) {
		term *t = angband_term[j];
		int clipy;

		/* No window */
		if (!t) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_MAPS))) continue;

		if (window_flag[j] & PW_MAP) {
			term *old = Term;

			Term_activate(t);
			display_map(NULL, NULL);
			Term_activate(old);
			continue;
		}

		/* Assume screen */
		ty = t->offset_y + (t->hgt / tile_height);
		tx = t->offset_x + (t->wid / tile_width);

		/*
		 * The overhead view can use the last row of the terminal.
		 * Others can not.
		 */
		clipy = t->hgt - ((window_flag[j] & PW_OVERHEAD) ? 0 : ROW_BOTTOM_MAP);

		/* Dump the map */
		for (y = t->offset_y, vy = 0; y < ty; vy += tile_height, y++) {
			for (x = t->offset_x, vx = 0; x < tx; vx += tile_width, x++) {
				/* Check bounds */
				if (!square_in_bounds(cave, loc(x, y))) {
					Term_queue_char(t, vx, vy,
						t->attr_blank, t->char_blank,
						0, 0);
					if (tile_width > 1 || tile_height > 1) {
						Term_big_queue_char(t, vx, vy,
							clipy, t->attr_blank,
							t->char_blank, 0, 0);
					}
					continue;
				}

				/* Determine what is there */
				map_info(loc(x, y), &g);
				grid_data_as_text(&g, &a, &c, &ta, &tc);
				Term_queue_char(t, vx, vy, a, c, ta, tc);

				if ((tile_width > 1) || (tile_height > 1))
					Term_big_queue_char(t, vx, vy, clipy,
						255, -1, 0, 0);
			}
			/* Clear partial tile at the end of each line. */
			for (; vx < t->wid; ++vx) {
				Term_queue_char(t, vx, vy, t->attr_blank,
					t->char_blank, 0, 0);
			}
		}
		/* Clear row of partial tiles at the bottom. */
		for (; vy < t->hgt; ++vy) {
			for (vx = 0; vx < t->wid; ++vx) {
				Term_queue_char(t, vx, vy, t->attr_blank,
					t->char_blank, 0, 0);
			}
		}
	}
}



/**
 * Redraw (on the screen) the current map panel
 *
 * Note the inline use of "light_spot()" for efficiency.
 *
 * The main screen will always be at least 24x80 in size.
 */
void prt_map(void)
{
	int a, ta;
	wchar_t c, tc;
	struct grid_data g;

	int y, x;
	int vy, vx;
	int ty, tx;
	int clipy;

	/* Redraw map sub-windows */
	prt_map_aux();

	/* Assume screen */
	ty = Term->offset_y + SCREEN_HGT;
	tx = Term->offset_x + SCREEN_WID;

	/* Avoid overwriting the last row with padding for big tiles. */
	clipy = ROW_MAP + SCREEN_ROWS;

	/* Dump the map */
	for (y = Term->offset_y, vy = ROW_MAP; y < ty; vy += tile_height, y++)
		for (x = Term->offset_x, vx = COL_MAP; x < tx; vx += tile_width, x++) {
			/* Check bounds */
			if (!square_in_bounds(cave, loc(x, y))) continue;

			/* Determine what is there */
			map_info(loc(x, y), &g);
			grid_data_as_text(&g, &a, &c, &ta, &tc);

			/* Hack -- Queue it */
			Term_queue_char(Term, vx, vy, a, c, ta, tc);

			if ((tile_width > 1) || (tile_height > 1))
				Term_big_queue_char(Term, vx, vy, clipy, a, c,
					COLOUR_WHITE, L' ');
		}
}

/**
 * Display a "small-scale" map of the dungeon in the active Term.
 *
 * Note that this function must "disable" the special lighting effects so
 * that the "priority" function will work.
 *
 * Note the use of a specialized "priority" function to allow this function
 * to work with any graphic attr/char mappings, and the attempts to optimize
 * this function where possible.
 *
 * If "cy" and "cx" are not NULL, then returns the screen location at which
 * the player was displayed, so the cursor can be moved to that location,
 * and restricts the horizontal map size to SCREEN_WID.  Otherwise, nothing
 * is returned (obviously), and no restrictions are enforced.
 */
void display_map(int *cy, int *cx)
{
	int map_hgt, map_wid;
	int row, col;

	int x, y;
	struct grid_data g;

	int a, ta;
	wchar_t c, tc;

	uint8_t tp;

	struct monster_race *race = &r_info[0];

	/* Priority array */
	uint8_t **mp = mem_zalloc(cave->height * sizeof(uint8_t*));
	for (y = 0; y < cave->height; y++)
		mp[y] = mem_zalloc(cave->width * sizeof(uint8_t));

	/* Desired map height */
	get_minimap_dimensions(Term, cave, tile_width, tile_height,
		&map_wid, &map_hgt);

	/* Prevent accidents */
	if ((map_wid < 1) || (map_hgt < 1)) {
		for (y = 0; y < cave->height; y++)
			mem_free(mp[y]);
		mem_free(mp);
		return;
	}

	/* Nothing here */
	a = COLOUR_WHITE;
    c = L' ';
	ta = COLOUR_WHITE;
	tc = L' ';

	/* Draw a box around the edge of the term */
	window_make(0, 0, map_wid + 1, map_hgt + 1);

	/* Clear outside that boundary. */
	if (map_wid + 1 < Term->wid - 1) {
		for (y = 0; y < map_hgt + 1; y++) {
			Term_erase(map_wid + 2, y, Term->wid - map_wid - 2);
		}
	}
	if (map_hgt + 1 < Term->hgt - 1) {
		for (y = map_hgt + 2; y < Term->hgt; y++) {
			Term_erase(0, y, Term->wid);
		}
	}

	/* Analyze the actual map */
	for (y = 0; y < cave->height; y++) {
		row = (y * map_hgt) / cave->height;
		if (tile_height > 1) row = row - (row % tile_height);

		for (x = 0; x < cave->width; x++) {
			col = (x * map_wid) / cave->width;
			if (tile_width > 1) col = col - (col % tile_width);

			/* Get the attr/char at that map location */
			map_info(loc(x, y), &g);
			grid_data_as_text(&g, &a, &c, &ta, &tc);

			/* Get the priority of that attr/char */
			tp = f_info[g.f_idx].priority;

			/* Stuff on top of terrain gets higher priority */
			if ((a != ta) || (c != tc)) tp = 20;

			/* Save "best" */
			if (mp[row][col] < tp) {
				/* Hack - make every grid on the map lit */
				g.lighting = LIGHTING_LIT;
				grid_data_as_text(&g, &a, &c, &ta, &tc);

				Term_queue_char(Term, col + 1, row + 1, a, c, ta, tc);

				if ((tile_width > 1) || (tile_height > 1))
					Term_big_queue_char(Term, col + 1,
						row + 1, Term->hgt - 1,
						255, -1, 0, 0);

				/* Save priority */
				mp[row][col] = tp;
			}
		}
	}

	/*** Display the player ***/

	/* Player location */
	row = (player->grid.y * map_hgt / cave->height);
	col = (player->grid.x * map_wid / cave->width);

	if (tile_width > 1)
		col = col - (col % tile_width);
	if (tile_height > 1)
		row = row - (row % tile_height);

	/* Get the terrain at the player's spot. */
	map_info(player->grid, &g);
	g.lighting = LIGHTING_LIT;
	grid_data_as_text(&g, &a, &c, &ta, &tc);

	/* Get the "player" tile */
	a = monster_x_attr[race->ridx];
	c = monster_x_char[race->ridx];

	/* Draw the player */
	Term_queue_char(Term, col + 1, row + 1, a, c, ta, tc);

	if ((tile_width > 1) || (tile_height > 1))
		Term_big_queue_char(Term, col + 1, row + 1, Term->hgt - 1,
			255, -1, 0, 0);
  
	/* Return player location */
	if (cy != NULL) (*cy) = row + 1;
	if (cx != NULL) (*cx) = col + 1;

	for (y = 0; y < cave->height; y++)
		mem_free(mp[y]);
	mem_free(mp);
}


/**
 * Display a "small-scale" map of the dungeon.
 *
 * Note that the "player" is always displayed on the map.
 */
void do_cmd_view_map(void)
{
	int cy, cx;
	uint8_t w, h;
	const char *prompt = "Hit any key to continue";
	if (Term->view_map_hook) {
		(*(Term->view_map_hook))(Term);
		return;
	}
	/* Save screen */
	screen_save();

	/* Note */
	prt("Please wait...", 0, 0);

	/* Flush */
	Term_fresh();

	/* Clear the screen */
	Term_clear();

	/* store the tile multipliers */
	w = tile_width;
	h = tile_height;
	tile_width = 1;
	tile_height = 1;

	/* Display the map */
	display_map(&cy, &cx);

	/* Show the prompt */
	put_str(prompt, Term->hgt - 1, Term->wid / 2 - strlen(prompt) / 2);

	/* Highlight the player */
	Term_gotoxy(cx, cy);

	/* Get any key */
	(void)anykey();

	/* Restore the tile multipliers */
	tile_width = w;
	tile_height = h;

	/* Load screen */
	screen_load();
}


/**
 * Record a mini screenshot
 */
void mini_screenshot(game_event_type type, game_event_data *data, void *user)
{
	int x, y;
	
	/* Populate the arrays */
	if (!player->escaped) { 
		for (y = player->grid.y - 3; y <= player->grid.y + 3; y++) {
			for (x = player->grid.x - 3; x <= player->grid.x + 3; x++) {
				struct grid_data g;
				int a, ta;
				wchar_t c, tc;
				struct loc grid = loc(x, y);
				if (square_in_bounds(cave, grid)) {
					map_info(grid, &g);
					grid_data_as_text(&g, &a, &c, &ta, &tc);
					mini_screenshot_char[y][x] = c;
					mini_screenshot_attr[y][x] = a;
				} else {
					mini_screenshot_char[y][x] = ' ';
					mini_screenshot_attr[y][x] = COLOUR_DARK;
				}
			}
		}
	} else {
		for (y = player->grid.y - 3; y <= player->grid.y + 3; y++) {
			for (x = player->grid.x - 3; x <= player->grid.x + 3; x++) {
				/* Grass */
				mini_screenshot_char[y][x] = '.';
				mini_screenshot_attr[y][x] = COLOUR_L_GREEN;
			}
		}

		/* River */
		mini_screenshot_char[0][1] = '~';
		mini_screenshot_attr[0][1] = COLOUR_BLUE;
		mini_screenshot_char[0][2] = '~';
		mini_screenshot_attr[0][2] = COLOUR_BLUE;
		mini_screenshot_char[1][2] = '~';
		mini_screenshot_attr[1][2] = COLOUR_L_BLUE;
		mini_screenshot_char[1][3] = '~';
		mini_screenshot_attr[1][3] = COLOUR_BLUE;
		mini_screenshot_char[1][4] = '~';
		mini_screenshot_attr[1][4] = COLOUR_L_BLUE;
		mini_screenshot_char[2][4] = '~';
		mini_screenshot_attr[2][4] = COLOUR_BLUE;
		mini_screenshot_char[2][5] = '~';
		mini_screenshot_attr[2][5] = COLOUR_BLUE;
		mini_screenshot_char[3][5] = '~';
		mini_screenshot_attr[3][5] = COLOUR_L_BLUE;
		mini_screenshot_char[3][6] = '~';
		mini_screenshot_attr[3][6] = COLOUR_BLUE;

		/* Trees */
		mini_screenshot_char[4][1] = '+';
		mini_screenshot_attr[4][1] = COLOUR_GREEN;
		mini_screenshot_char[6][4] = '+';
		mini_screenshot_attr[6][4] = COLOUR_GREEN;
		
		/* Player */
		mini_screenshot_char[3][3] = '@';
		mini_screenshot_attr[3][3] = COLOUR_WHITE;
	}
}

void prt_mini_screenshot(int col, int row)
{
	int x, y;

	for (y = 0; y <= 6; y++) {
		for (x = 0; x <= 6; x++) {
			Term_putch(col + x, row + y, mini_screenshot_attr[y][x],
					   mini_screenshot_char[y][x]);
		}
	}
}

void file_mini_screenshot(ang_file *fff)
{
	int x, y;

	for (y = 0; y <= 6; y++) {
		file_putf(fff, "  ");
		for (x = 0; x <= 6; x++) {
			file_putf(fff, "%c", mini_screenshot_char[y][x]);
		}
		file_putf(fff, "\n");
	}
}


