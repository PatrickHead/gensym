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
 *  @file debug.c
 *  @brief functions used to help develop and debug
 */

#ifdef DEBUG

#include <stdio.h>
#include <string.h>

#include "debug.h"

void symbol_dump(symbol *sy, int indent)
{
  if (!sy) goto exit;

  do_indent(indent);
  printf("symbol\n");

  indent += 2;

  do_indent(indent);
  printf("refdes: %s\n", sy->refdes ? sy->refdes : "[empty]");

  do_indent(indent);
  printf("text_size_mult: %f\n", sy->text_size_mult);

  slots_dump(sy->slots, indent);

exit:
  return;
}

void attributes_dump(attribute_array *as, int indent)
{
  int i;

  do_indent(indent);
  printf("attributes: (%d)\n", as ? as->n : 0);

  if (!as) goto exit;

  indent += 2;

  for (i = 0; i < as->n; i++)
    attribute_dump(as->item[i], indent);

exit:
  return;
}

void attribute_dump(attribute *a, int indent)
{
  if (!a) goto exit;

  do_indent(indent);
  printf("attribute\n");

  indent += 2;

  do_indent(indent);
  printf("name: %s\n", a->name ? a->name : "[empty]");

  do_indent(indent);
  printf("value: %s\n", a->value ? a->value : "[empty]");

  do_indent(indent);
  printf("visibility: %s\n", attribute_visibility_to_string(a->visibility));

exit:
  return;
}

void pinalign_dump(pinalign *pa, int indent)
{
  if (!pa) goto exit;

  do_indent(indent);
  printf("pinalign\n");

  indent += 2;

  do_indent(indent);
  printf("side: %s\n", pinalign_side_to_string(pa->side));

  do_indent(indent);
  printf("place: %s\n", pinalign_place_to_string(pa->place));

exit:
  return;
}

void pins_dump(pin_array *ps, int indent)
{
  int i;

  do_indent(indent);
  printf("pins: (%d)\n", ps ? ps->n : 0);

  if (!ps) goto exit;

  indent += 2;

  for (i = 0; i < ps->n; i++)
    pin_dump(ps->item[i], indent);

exit:
  return;
}

void pin_dump(pin *p, int indent)
{
  if (!p) goto exit;

  do_indent(indent);
  printf("pin\n");

  indent += 2;

  do_indent(indent);
  printf("name: %s\n", p->name ? p->name : "[empty]");

  do_indent(indent);
  printf("num: %u\n", p->num);

  do_indent(indent);
  printf("location: %s\n", pin_location_to_string(p->location));

  do_indent(indent);
  printf("direction: %s\n", pin_direction_to_string(p->direction));

  do_indent(indent);
  printf("label: %s\n", p->label ? p->label : "[empty]");

  do_indent(indent);
  printf("inverted_circle: %s\n", p->inverted_circle ? "TRUE" : "FALSE");

  do_indent(indent);
  printf("funcmap: %s\n", p->funcmap ? "TRUE" : "FALSE");

exit:
  return;
}

void shape_dump(shape *sh, int indent)
{
  if (!sh) goto exit;

  do_indent(indent);
  printf("shape\n");

  indent += 2;

  do_indent(indent);
  printf("type: %s\n", shape_type_to_string(sh->type));

  do_indent(indent);
  printf("filled: %s\n", sh->filled ? "TRUE" : "FALSE");

exit:
  return;
}

void slots_dump(slot_array *sls, int indent)
{
  int i;

  do_indent(indent);
  printf("slots: (%d)\n", sls ? sls->n : 0);

  if (!sls) goto exit;

  indent += 2;

  for (i = 0; i < sls->n; i++)
    slot_dump(sls->item[i], indent);

exit:
  return;
}

void slot_dump(slot *sl, int indent)
{
  if (!sl) goto exit;

  do_indent(indent);
  printf("slot\n");

  indent += 2;

  do_indent(indent);
  printf("name: %s\n", sl->name ? sl->name : "[empty]");

  shape_dump(&sl->shape, indent);

  do_indent(indent);
  printf("pin_alignment\n");

  indent += 2;

  do_indent(indent);
  printf("right: %s\n", pinalign_place_to_string(sl->pinalignment[0].place));

  do_indent(indent);
  printf("left: %s\n", pinalign_place_to_string(sl->pinalignment[1].place));

  do_indent(indent);
  printf("top: %s\n", pinalign_place_to_string(sl->pinalignment[2].place));

  do_indent(indent);
  printf("bottom: %s\n", pinalign_place_to_string(sl->pinalignment[3].place));

  indent -= 2;

  do_indent(indent);
  printf("label: %s\n", sl->label ? sl->label : "[empty]");

  attributes_dump(sl->attributes, indent);

  pins_dump(sl->pins, indent);

  do_indent(indent);
  printf("width: %d\n", sl->width);

  do_indent(indent);
  printf("height: %d\n", sl->height);

exit:
  return;
}

  /**
   *  @fn void pin_side_lists_dump(pin_side_lists *psl)
   *  @brief dumps contents of @p psl to @b stdout
   *
   *  @param psl - pointer to @a pin_side_lists structure
   *
   *  @par Returns
   *       Nothing.
   */

void pin_side_lists_dump(pin_side_lists *psl)
{
  int i;

  if (!psl) goto exit;

  printf("%d left side pins\n", psl->left ? psl->left->n : 0);
  printf("left side width = %d\n", psl->left_width);
  for (i = 0; psl->left && i < psl->left->n; i++)
    printf("  %d: %s\n", i, psl->left->item[i]->name);

  printf("%d right side pins\n", psl->right ? psl->right->n : 0);
  printf("right side width = %d\n", psl->right_width);
  for (i = 0; psl->right && i < psl->right->n; i++)
    printf("  %d: %s\n", i, psl->right->item[i]->name);

  printf("%d top side pins\n", psl->top ? psl->top->n : 0);
  printf("top side width = %d\n", psl->top_width);
  for (i = 0; psl->top && i < psl->top->n; i++)
    printf("  %d: %s\n", i, psl->top->item[i]->name);

  printf("%d bottom side pins\n", psl->bottom ? psl->bottom->n : 0);
  printf("bottom side width = %d\n", psl->bottom_width);
  for (i = 0; psl->bottom && i < psl->bottom->n; i++)
    printf("  %d: %s\n", i, psl->bottom->item[i]->name);

exit:
  return;
}

#endif // DEBUG
