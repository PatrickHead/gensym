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
 *  @file pin_side_lists.c
 *  @brief maintain list of pins on each side of box symbol
 */

#include <stdlib.h>
#include <string.h>

#include "pin_side_lists.h"

  /**
   *  @fn pin_side_lists *pin_side_lists_new(void)
   *  @brief creates a new @p pin_side_lists instance
   *
   *  @par Parameters
   *       None.
   *
   *  @return pointer to @p pin_side_lists structure
   */

pin_side_lists *pin_side_lists_new(void)
{
  pin_side_lists *psl = NULL;

  psl = malloc(sizeof(pin_side_lists));
  if (psl) memset(psl, 0, sizeof(pin_side_lists));

  return psl;
}

  /**
   *  @fn pin_side_lists *pin_side_lists_new_from_symbol(symbol *sy,
                                                         unsigned int slot_idx)
   *  @brief creates a new @p pin_side_lists instance from @p sy using
   *         @p slot_idx
   *
   *  @param sy - pointer to @p symbol structure
   *  @param slot_idx - slot # to use for generation
   *
   *  @return pointer to @p pin_side_lists structure
   */

pin_side_lists *pin_side_lists_new_from_symbol(symbol *sy, unsigned int slot_idx)
{
  pin_side_lists *psl = NULL;
  slot *sl;
  pin *p;
  int i, j;
  int left_slot;
  int right_slot;
  int top_slot;
  int bottom_slot;
  pin dummy_p;
  int max;

  if (!sy || (slot_idx >= sy->slots->n)) goto exit;

  psl = pin_side_lists_new();
  if (!psl) goto exit;

  memset(&dummy_p, 0, sizeof(pin));
  left_slot = right_slot = top_slot = bottom_slot = 0;

  psl->left = pin_array_new();
  psl->right = pin_array_new();
  psl->top = pin_array_new();
  psl->bottom = pin_array_new();

  for (i = 0; i < sy->slots->n; i++)
  {
    if (!slot_idx || (i == slot_idx))
    {
      sl = sy->slots->item[i];
      for (j = 0; j < sl->pins->n; j++)
      {
        p = sl->pins->item[j];
        max = 0;
        if (p->name) max = strlen(p->name);
        if (p->label) max = max > strlen(p->label) ? max : strlen(p->label);
        switch (p->location)
        {
          case pin_location_left:
            if (i > left_slot)
            {
              if (left_slot) pin_array_add(psl->left, &dummy_p);
              left_slot = i;
            }
            pin_array_add(psl->left, p);
            if (max > psl->left_width)
              psl->left_width = max;
            break;

          case pin_location_top:
            if (i > top_slot)
            {
              if (top_slot) pin_array_add(psl->top, &dummy_p);
              top_slot = i;
            }
            pin_array_add(psl->top, p);
            if (max > psl->top_width)
              psl->top_width = max;
            break;

          case pin_location_bottom:
            if (i > bottom_slot)
            {
              if (bottom_slot) pin_array_add(psl->bottom, &dummy_p);
              bottom_slot = i;
            }
            pin_array_add(psl->bottom, p);
            if (max > psl->bottom_width)
              psl->bottom_width = max;
            break;

          case pin_location_right:
          default:
            if (i > right_slot)
            {
              if (right_slot) pin_array_add(psl->right, &dummy_p);
              right_slot = i;
            }
            pin_array_add(psl->right, p);
            if (max > psl->right_width)
              psl->right_width = max;
            break;
        }
      }
    }
  }

exit:
  return psl;
}

  /**
   *  @fn void pin_side_lists_free(pin_side_lists *psl)
   *  @brief frees all memory allocated to @psl
   *
   *  @param psl - pointer to @a pin_side_lists structure
   *
   *  @par Returns
   *       Nothing.
   */

void pin_side_lists_free(pin_side_lists *psl)
{
  if (!psl) return;

  if (psl->left) pin_array_free(psl->left);
  if (psl->right) pin_array_free(psl->right);
  if (psl->top) pin_array_free(psl->top);
  if (psl->bottom) pin_array_free(psl->bottom);
}

