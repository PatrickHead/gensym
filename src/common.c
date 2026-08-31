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
 *  @file common.c
 *  @brief functions common to project
 */

#include <stdio.h>
#include <string.h>

#include "common.h"

  /**
   * @fn char *bool_to_string(bool b)
   * @brief convert bool value to string
   *
   * @param b - boolean value
   *
   * @return "yes" for true, "no" for false
   */

char *bool_to_string(bool b)
{
  if (b) return "yes";
  return "no";
}

  /**
   * @fn bool bool_from_string(char *s)
   * @brief convert string to bool value
   *
   * @param s - string representation of boolean value
   *
   * @return @ bool value
   */

bool bool_from_string(char *s)
{
  bool filled = false;

  if (!s) goto exit;

  if (!strcmp(s, "yes")) filled = true;

exit:
  return filled;
}

  /**
   * @fn void do_indent(int indent)
   * @brief emits @p indent spaces on @a stdout
   *
   * @param indent - number of spaces to emit
   *
   * @par Returns
   *    Nothing.
   */

void do_indent(int indent)
{
  int i;

  if (indent < 0) return;
  for (i = 0; i < indent; i++) fputc(' ', stdout);
}

