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
 *  @file parse.c
 *  @brief functions to parse gensym definition file
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "common.h"

#include "parse.h"

static symbol *parse_symbol(char *data);
static void parse_slot(symbol *sy, char *name, char *data);
static void parse_pin(slot *sl, char *name, char *data);
static char *read_file(char *file_name);
static char *line(char *buffer);
static int parts(char *s, char **p1, char **p2, char **p3);

  /**
   * @fn
   * @brief parse boxsym format file name in @p file_name
   *
   * @param file_name - string containing path to file to parse
   *
   * @return pointer to @a symbol struct
   */

symbol *parse_boxsym_file(char *file_name)
{
  char *data = NULL;
  symbol *sy = NULL;

  if (!file_name) goto exit;

  data = read_file(file_name);
  if (!data) goto exit;

  sy = parse_symbol(data);

exit:
  if (data) free(data);

  return sy;
}

static symbol *parse_symbol(char *data)
{
  char *l = NULL;
  char *p1, *p2, *p3;
  char *copy;
  symbol *sy = NULL;

  if (!data) goto exit;

  copy = strdup(data);
  if (!copy) goto exit;

  sy = symbol_new();

    /*
     * parse global values
     */

  for (l = line(data); l; l = line(NULL))
  {
    p1 = p2 = p3 = NULL;

    if (!strlen(l)) continue;

    parts(l, &p1, &p2, &p3);

    if (!strcmp(p1, "refdes"))
      symbol_set_refdes(sy, p2);
    else if (!strcmp(p1, "text_size_mult"))
      symbol_set_text_size_mult(sy, atof(p2));
  }

  parse_slot(sy, "[GLOBAL]", copy);

  free(copy);

exit:
  return sy;
}

static void parse_slot(symbol *sy, char *name, char *data)
{
  char *l = NULL;
  char *p1, *p2, *p3;
  slot *sl = NULL;
  slot_array *sls;
  attribute_array *aa = NULL;
  attribute *a = NULL;
  int idx;
  pinalign_side side;
  pinalign_place place;

  sls = symbol_get_slots(sy);
  if (!sls) goto exit;
  idx = sls->n;

  sl = slot_new();
  if (!sl) goto exit;
  slot_array_add(sls, sl);
  slot_free(sl);
  sl = sls->item[idx];
  slot_set_name(sl, name);

  aa = slot_get_attributes(sl);

  for (l = line(data); l; l = line(NULL))
  {
    p1 = p2 = p3 = NULL;

    if (!strlen(l)) continue;

    parts(l, &p1, &p2, &p3);

    if ((!strcmp(p1, "attr")) ||
        (!strcmp(p1, "attr_both")) ||
        (!strcmp(p1, "attr_invis")))
    {
      a = attribute_new();
      if (!a) continue;
      attribute_set_name(a, p2);
      attribute_set_value(a, p3);
      if (!strcmp(p1, "attr"))
        attribute_set_visibility(a, attribute_visibility_value);
      if (!strcmp(p1, "attr_both"))
        attribute_set_visibility(a, attribute_visibility_both);
      if (!strcmp(p1, "attr_invis"))
        attribute_set_visibility(a, attribute_visibility_none);
      attribute_array_add(aa, a);
      attribute_free(a);
    }
    else if (!strcmp(p1, "label"))
      slot_set_label(sl, p2);
    else if (!strcmp(p1, "shape"))
      shape_set_type(&sl->shape, shape_type_from_string(p2));
    else if (!strcmp(p1, "shape_fill"))
      shape_set_filled(&sl->shape, bool_from_string(p2));
    else if (!strcmp(p1, "pinalign"))
    {
      side = pinalign_side_from_string(p2);
      place = pinalign_place_from_string(p3);
      sl->pinalignment[side].side = side;
      sl->pinalignment[side].place = place;
    }
    else if (!strcmp(p1, "end") && !strcmp(p2, "slot"))
      break;
    else if (!strcmp(p1, "begin") && !strcmp(p2, "slot"))
      parse_slot(sy, p3, NULL);
    else if (!strcmp(p1, "begin") && !strcmp(p2, "pin"))
      parse_pin(sl, p3, NULL);
  }

exit:
  return;
}

static void parse_pin(slot *sl, char *name, char *data)
{
  char *l = NULL;
  char *p1, *p2, *p3;
  pin_array *pa = NULL;
  pin *p = NULL;
  int idx;

  pa = slot_get_pins(sl);
  if (!pa) goto exit;
  idx = pa->n;

  p = pin_new();
  if (!p) goto exit;
  pin_array_add(pa, p);
  pin_free(p);
  p = pa->item[idx];

  pin_set_name(p, name);

  for (l = line(data); l; l = line(NULL))
  {
    p1 = p2 = p3 = NULL;

    if (!strlen(l)) continue;

    parts(l, &p1, &p2, &p3);

    if (!strcmp(p1, "label"))
      pin_set_label(p, p2);
    else if (!strcmp(p1, "num"))
      pin_set_num(p, atoi(p2));
    else if (!strcmp(p1, "loc"))
      pin_set_location(p, pin_location_from_string(p2));
    else if (!strcmp(p1, "dir"))
      pin_set_direction(p, pin_direction_from_string(p2));
    else if (!strcmp(p1, "invcirc"))
      pin_set_inverted_circle(p, true);
    else if (!strcmp(p1, "funcmap"))
      pin_set_funcmap(p, true);
    else if (!strcmp(p1, "end") && !strcmp(p2, "pin"))
      break;
  }

exit:
  return;
}

static char *read_file(char *file_name)
{
  FILE *f = NULL;
  char *buffer = NULL;
  struct stat st;
  size_t len;
  size_t n_read;
  int i;

  if (!file_name) goto exit;

  memset(&st, 0, sizeof(struct stat));

  if (stat(file_name, &st)) goto exit;
  len = st.st_size;

  f = fopen(file_name, "r");
  if (!f) goto exit;

  buffer = malloc(len + 1);
  if (!buffer) goto exit;

  memset(buffer, 0, len + 1);

  n_read = fread(buffer, 1, len, f);
  if (n_read != len)
  {
    free(buffer);
    buffer = NULL;
    goto exit;
  }

  for (i = 0; i < len; i++)
  {
    switch (buffer[i])
    {
      case '\r':
      case '\n':
        buffer[i] = 127;
        break;

      default:
        break;
    }
  }

exit:
  if (f) fclose(f);

  return buffer;
}

static char *line(char *buffer)
{
  static char *p_buff = NULL;
  char *l = NULL;
  char *tmp;

  if (buffer) p_buff = buffer;
  if (!*p_buff) goto exit;

  l = p_buff;

    /* Mark end of line with NUL and scan to beginning of next line */

  while (*p_buff != 127) ++p_buff;
  while (*p_buff == 127)
  {
    *p_buff = 0;
    ++p_buff;
  }

    /* skip any leading whitespace */

  for (tmp = l; *tmp; tmp++)
  {
    switch (*tmp)
    {
      case ' ':
      case '\t':
        ++l;
        break;

      default: goto end_loop;
    }
  }
end_loop:

    /* If line has no length, jump to end */

  if (!strlen(l)) goto exit;

    /* If remainder of line is a comment, jump to end */

  if (*l == '#')
  {
    *l = 0;
    goto exit;
  }

    /* truncate at trailing comment */

  for (tmp = l + strlen(l) - 1; tmp >= l; tmp--)
  {
    if (*tmp == '#')
    {
      *tmp = 0;
      break;
    }
  }

    /* truncate at trailing whitespace */

  for (tmp = l + strlen(l) - 1; tmp >= l; tmp--)
  {
    if ((*tmp == ' ') || (*tmp == '\t')) *tmp = 0;
    else break;
  }

exit:
  return l;
}

static int parts(char *s, char **p1, char **p2, char **p3)
{
  int cnt = 0;

  if (!s) goto exit;

  *p1 = *p2 = *p3 = NULL;

  *p2 = *p1 = s;

  while (**p2)
  {
    if ((**p2 == ' ') || (**p2 == '\t'))
    {
      **p2 = 0;
      ++*p2;
      break;
    }

    ++*p2;
  }

  if (!**p2)
  {
    *p2 = NULL;
    goto exit;
  }

    // Eat whitespace
  while (**p2)
  {
    if ((**p2 == ' ') || (**p2 == '\t')) ++*p2;
    else break;
  }

  *p3 = *p2;

  while (**p3)
  {
    if ((**p3 == ' ') || (**p3 == '\t'))
    {
      **p3 = 0;
      ++*p3;
      break;
    }

    ++*p3;
  }

    // Eat whitespace
  while (**p3)
  {
    if ((**p3 == ' ') || (**p3 == '\t')) ++*p3;
    else break;
  }

  if (!**p3)
  {
    *p3 = NULL;
    goto exit;
  }

exit:
  return cnt;
}

