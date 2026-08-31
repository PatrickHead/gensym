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
 *  @file debug.h
 *  @brief functions used to help develop and debug
 */

#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
#include "common.h"
#include "symbol.h"
#include "pin_side_lists.h"

void symbol_dump(symbol *sy, int indent);
void attributes_dump(attribute_array *as, int indent);
void attribute_dump(attribute *a, int indent);
void pinalign_dump(pinalign *pa, int indent);
void pins_dump(pin_array *ps, int indent);
void pin_dump(pin *p, int indent);
void shape_dump(shape *sh, int indent);
void slots_dump(slot_array *sls, int indent);
void slot_dump(slot *sl, int indent);
void pin_side_lists_dump(pin_side_lists *psl);
char *bool_to_string(bool b);
bool bool_from_string(char *s);
#endif // DEBUG

#endif // DEBUG_H
