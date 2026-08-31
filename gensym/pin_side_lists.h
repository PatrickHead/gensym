/*
 *  Copyright 2026 - Patrick Head
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/**
 *  @file pin_side_lists.h
 *  @brief maintain list of pins on each side of box symbol
 */

#ifndef PIN_SIDE_LISTS_H
#define PIN_SIDE_LISTS_H

#include "symbol.h"

  /**
   * @typedef pin_side_lists
   * @brief creates a type for @p pin_side_list struct
   */

typedef struct pin_side_lists pin_side_lists;

  /**
   * @struct pin_side_lists
   * @brief side list data for each side of box symbol
   */

struct pin_side_lists
{
  pin_array *left;
  int left_width;
  pin_array *right;
  int right_width;
  pin_array *top;
  int top_width;
  pin_array *bottom;
  int bottom_width;
};

pin_side_lists *pin_side_lists_new(void);
pin_side_lists *pin_side_lists_new_from_symbol(symbol *sy,
                                               unsigned int slot_idx);
void pin_side_lists_free(pin_side_lists *psl);

#endif // PIN_SIDE_LISTS_H
