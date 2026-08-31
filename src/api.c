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
 *  @file api.c
 *  @brief API for gensym
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <liblihata/dom.h>
#include <unistd.h>
#include <sys/param.h>
#include <getopt.h>

#include "parse.h"
#include "pin_side_lists.h"
#include "uuid.h"
#include "debug.h"

#include "api.h"

  /**
   * @typedef direction
   * @brief general orientation of symbol box (vertical or horizontal)
   */

typedef enum
{
  vertical,
  horizontal
} direction;

static symbol *gensym_boxsym_read(char *name);

static lht_node_t *lihata_top(symbol *sy, unsigned int slot_idx);
static void lihata_add_line(lht_node_t *node,
                            char *stroke,
                            int x1,
                            int y1,
                            int x2,
                            int y2);
static void lihata_add_arc(lht_node_t *node,
                           int oid,
                           char *stroke,
                           int cx,
                           int cy,
                           int r,
                           double sang,
                           double dang,
                           bool svalid,
                           int sx,
                           int sy,
                           bool evalid,
                           int ex,
                           int ey);
static void lihata_add_uuid(lht_node_t *node);
static void lihata_add_symbol_objects(symbol *sy,
                                      int slot_idx,
                                      lht_node_t *node);
static void lihata_add_symbol_attributes(symbol *sy,
                                         int slot_idx,
                                         lht_node_t *node);
static void lihata_add_symbol_name(symbol *sy,
                                   int slot_idx,
                                   lht_node_t *node);
static void lihata_add_symbol_outline(symbol *sy,
                                      int slot_idx,
                                      lht_node_t *node);
static void lihata_add_symbol_device_name(symbol *sy,
                                          int slot_idx,
                                          lht_node_t *node);
static void lihata_add_symbol_pins(symbol *sy, int slot_idx, lht_node_t *node);
static void lihata_add_symbol_pin(symbol *sy,
                                  lht_node_t *node,
                                  pin *p,
                                  int obj_num,
                                  double x,
                                  double y);

static char *find_device_name(symbol *sy, unsigned int slot_idx);
static int find_device_name_length(symbol *sy, unsigned int slot_idx);

static pin_side_lists *psl = NULL;        /** pin side lists used generally  */

  /**
   *  @fn int gensym(char *file_name, gensym_options *opts)
   *  @brief generates symbol file(s) from definitions in @i file_name
   *
   *  @param file_name - string containing name of symbol definition file
   *  @param opts      - pointer to options for symbol generation
   *
   *  @return 0 on success, -1 on error
   */

int gensym(char *file_name, gensym_options *opts)
{
  int rv = -1;
  symbol *sy = NULL;
  slot *sl = NULL;
  int slot_idx = 0;
  lht_doc_t *doc = NULL;
  unsigned int height;
  unsigned int width;
  int device_name_width = 0;
  int pin_name_width = 0;
  direction dir = vertical;
  bool generate_slot_files = false;
  char *output_name = NULL;
  char *t;

  if (!file_name) goto exit;

  sy = gensym_boxsym_read(file_name);
  if (!sy) goto exit;

    /*
     * Build output_name.  If it is not already in options, then use the
     * file_name passed in.  Remove extension from output_name, if one exists
     */

  if (!opts || !opts->output_name)
    output_name = strdup(file_name);
  if (!output_name) goto exit;

  if (opts && opts->generate_slot_files)
    generate_slot_files = true;

  t = output_name;
  while (*t) ++t;
  while (t >= output_name)
  {
    if (*t == '.') break;
    --t;
  }
  if (t >= output_name) *t = 0;

    // GLOBAL slot is always used first

  for (slot_idx = 0; slot_idx < sy->slots->n; ++slot_idx)
  {
    sl = slot_array_get(sy->slots, slot_idx);
    if (!sl) continue;

    psl = pin_side_lists_new_from_symbol(sy, slot_idx);
    if (!psl) continue;

    pin_name_width = psl->top_width + psl->bottom_width + 2;
    pin_name_width *= 1500;

    height = MAX(psl->left->n, psl->right->n);
    height *= 4000;
    height += pin_name_width;

    width = MAX(psl->top->n, psl->bottom->n) + 2;
    width *= 4000;

    if (height > width) width += 2000;  // space for device name
    else height += 2000;

    device_name_width = find_device_name_length(sy, slot_idx);
    device_name_width *= 1500;

    if (height > width) dir = vertical;
    else dir = horizontal;

    switch (dir)
    {
      case vertical:
        width += 1500;  // add in one row of text for device name
        if ((height - pin_name_width) < device_name_width)
          height += device_name_width - (height - pin_name_width);
        break;

      case horizontal:
        height += 1500;  // add in one row of text for device name
        if ((width - pin_name_width) < device_name_width)
          width += device_name_width - (width - pin_name_width);
        break;
    }

    slot_set_height(sl, height);
    slot_set_width(sl, width);

    doc = gensym_lihata_doc(sy, slot_idx);
    if (!doc) goto exit;

    sprintf(file_name,
            "%s%s%s.ry",
            output_name,
            slot_idx ? "-" : "",
            slot_idx ? sl->name : "");

    if (gensym_lihata_write(doc, file_name))
      fprintf(stderr, "Failed to write '%s'\n", file_name);

    lht_dom_uninit(doc);
    if (psl) pin_side_lists_free(psl);

    if (!generate_slot_files) break;
  }

  rv = 0;

exit:
  if (sy) symbol_free(sy);
  if (output_name) free(output_name);

  return rv;
}

  /**
   *  @fn lht_doc_t *gensym_lihata_doc(symbol *sy, unsigned int slot_idx)
   *  @brief generates lihata document from @p sy
   *
   *  @param sy - pointer to symbol structure
   *  @param slot_idx - the slot # for generated document.  0 is GLOBAL.
   *
   *  @return pointer to @a lht_doc_t on success, NULL on error
   */

lht_doc_t *gensym_lihata_doc(symbol *sy, unsigned int slot_idx)
{
  lht_doc_t *doc = NULL;
  lht_node_t *root = NULL;
  lht_node_t *top = NULL;

  if (!sy) goto exit;

  doc = lht_dom_init();
  if (!doc) goto exit;

    /*
     *  root node for any symbol
     */

  root = lht_dom_node_alloc(LHT_HASH, "cschem-group-v1");
  if (!root) goto exit;

  doc->root = root;
  doc->root->doc = doc;

    /*
     *  currently handles only one top level hash node
     */

  top = lihata_top(sy, slot_idx);
  if (lht_dom_hash_put(root, top)) goto exit;

exit:
  return doc;
}

  /**
   *  @fn int gensym_lihata_write(lht_doc_t *doc, char *name)
   *  @brief writes @p doc to file named @p name
   *
   *  @param doc - pointer to @a lht_doc_t structure
   *  @param name - string containing name of file to write
   *
   *  @return 0 on success, -1 on error
   */

int gensym_lihata_write(lht_doc_t *doc, char *name)
{
  FILE *f;
  lht_err_t err;
  lht_dom_export_style_t style;
  int rv = -1;

  if (!doc || !doc->root) goto exit;

  if (name) f = fopen(name, "w");
  else f = stdout;

  style = LHT_STY_EQ_TE;
  err = lht_dom_export_style(doc->root, f, "", style);
  if (err)
  {
    fprintf(stderr, "err=%d\n", err);
    goto exit;
  }

  if (f && (f != stdout)) fclose(f);

  rv = 0;

exit:
  return rv;
}

  /**
   *  @fn gensym_options *gensym_options_new(void)
   *  @brief creates an instance of @a gensym_options
   *
   *  @par Parameters
   *       None.
   *
   *  @return pointer to @a gensym_options on success, NULL on error
   */

gensym_options *gensym_options_new(void)
{
  gensym_options *opts = NULL;

  opts = malloc(sizeof(gensym_options));
  if (!opts) goto exit;

  memset(opts, 0, sizeof(gensym_options));

exit:
  return opts;
}

  /**
   *  @fn void gensym_options_free(gensym_options *opts)
   *  @brief deallocates all memory assocated with @p opts
   *
   *  @param opts      - pointer to @a gensym_options struct
   *
   *  @par Returns
   *       Nothing.
   */

void gensym_options_free(gensym_options *opts)
{
  if (!opts) return;
  if (opts->output_name) free(opts->output_name);
  free(opts);
}

  /**
   *  @fn char *gensym_options_get_output_name(gensym_options *opts)
   *  @brief returns output name from @p opts
   *
   *  @param opts      - pointer to options for symbol generation
   *
   *  @return string containing output name, or NULL on error
   */

char *gensym_options_get_output_name(gensym_options *opts)
{
  return opts ? opts->output_name : NULL;
}

  /**
   *  @fn void gensym_options_set_output_name(gensym_options *opts,
   *                                          char *output_name)
   *  @brief sets output name in @p opts
   *
   *  @param opts        - pointer to options for symbol generation
   *  @param output_name - string containing base output name of files
   *
   *  @par Returns
   *       Nothing.
   */

void gensym_options_set_output_name(gensym_options *opts, char *output_name)
{
  if (!opts || !output_name) goto exit;

  if (opts->output_name) free(opts->output_name);
  opts->output_name = NULL;
  if (output_name) opts->output_name = strdup(output_name);

exit:
  return;
}

  /**
   *  @fn bool gensym_options_get_generate_slot_files(gensym_options *opts)
   *  @brief returns slot generation flag from @p opts
   *
   *  @param opts      - pointer to options for symbol generation
   *
   *  @return true or false
   */

bool gensym_options_get_generate_slot_files(gensym_options *opts)
{
  return opts ? opts->generate_slot_files : false;
}

  /**
   *  @fn void gensym_options_set_generate_slot_files(gensym_options *opts,
   *                                                  bool generate_slot_files)
   *  @brief sets slot generation flag in @p opts
   *
   *  @param opts                - pointer to options for symbol generation
   *  @param generate_slot_files - true or false
   *
   *  @par Returns
   *       Nothing.
   */

void gensym_options_set_generate_slot_files(gensym_options *opts,
                                            bool generate_slot_files)
{
  if (!opts) return;
  opts->generate_slot_files = generate_slot_files;
}

  /**
   *  @fn symbol *gensym_boxsym_read(char *name)
   *  @brief reads and parses a @i boxsym definition file from @p name
   *
   *  @param name - string containing name of symbol definition file
   *
   *  @return pointer to @a symbol struct, or NULL on error
   */

static symbol *gensym_boxsym_read(char *name)
{
  symbol *sy = NULL;

  if (!name) goto exit;

  sy = parse_boxsym_file(name);
  if (!sy) goto exit;

exit:
  return sy;
}

  /**
   *  @fn lht_node_t *lihata_top(symbol *sy, unsigned int slot_idx)
   *  @brief creates an @a lht_node_t node from a top symbol in @p sy
   *
   *  @details the top symbol is chosen by the slot # in @p slot_idx
   *           '0' is the GLOBAL slot, which includes all subsequent slots
   *
   *  @param file_name - string containing name of symbol definition file
   *  @param opts      - pointer to options for symbol generation
   *
   *  @return 0 on success, -1 on error
   */

static lht_node_t *lihata_top(symbol *sy, unsigned int slot_idx)
{
  lht_node_t *top = NULL;

  if (!sy) goto exit;

  top = lht_dom_node_alloc(LHT_HASH, "group.1");
  if (!top) goto exit;

  lihata_add_uuid(top);
  lihata_add_symbol_objects(sy, slot_idx, top);
  lihata_add_symbol_attributes(sy, slot_idx, top);

exit:
  return top;
}

  /**
   *  @fn void lihata_add_line(lht_node_t *node,
   *                        char *stroke,
   *                        int x1,
   *                        int y1,
   *                        int x2,
   *                        int y2)
   *  @brief adds a visual line in @p node
   *
   *  @param node - pointer to @a lht_node_t struct
   *  @param stroke - string containing pen name
   *  @param x1 - x value of start of line
   *  @param y1 - y value of start of line
   *  @param x2 - x value of end of line
   *  @param y2 - y value of end of line
   *
   *  @par Returns
   *       Nothing.
   */

static void lihata_add_line(lht_node_t *node,
                            char *stroke,
                            int x1,
                            int y1,
                            int x2,
                            int y2)
{
  lht_node_t *ha_node;
  lht_node_t *te_node;
  char num[32];

  if (!node) goto exit;

  memset(num, 0, 32);

  ha_node = lht_dom_node_alloc(LHT_HASH, strdup("line"));
  if (!ha_node) goto exit;

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("x1"));
  if (!te_node) goto exit;
  sprintf(num, "%d", x1);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("y1"));
  if (!te_node) goto exit;
  sprintf(num, "%d", y1);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("x2"));
  if (!te_node) goto exit;
  sprintf(num, "%d", x2);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("y2"));
  if (!te_node) goto exit;
  sprintf(num, "%d", y2);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("stroke"));
  if (!te_node) goto exit;
  te_node->data.text.value = strdup(stroke);
  lht_dom_hash_put(ha_node, te_node);

  lht_dom_list_append(node, ha_node);

exit:
  return;
}

  /**
   *  @fn void lihata_add_arc(lht_node_t *node,
   *                          int oid,
   *                          char *stroke,
   *                          int cx,
   *                          int cy,
   *                          int r,
   *                          double sang,
   *                          double dang,
   *                          bool svalid,
   *                          int sx,
   *                          int sy,
   *                          bool evalid,
   *                          int ex,
   *                          int ey)
   *  @brief adds a visual arc in @p node
   *
   *  @param node - pointer to @a lht_node_t struct
   *  @param oid - id unique to all items in parent object
   *  @param stroke - string containing pen name
   *  @param cx - x value of center of arc
   *  @param cy - y value of center of arc
   *  @param r - radius of arc
   *  @param sang - start angle of arc
   *  @param dang - (+/-) angle delta of arc
   *  @param svalid - is sang value valid, true or false
   *  @param sx - x value of exact starting coordinate of arc
   *  @param sy - y value of exact starting coordinate of arc
   *  @param evalid - is dang value valid, true or false
   *  @param ex - x value of exact ending coordinate of arc
   *  @param ey - y value of exact ending coordinate of arc
   *
   *  @par Returns
   *       Nothing.
   */


static void lihata_add_arc(lht_node_t *node,
                           int oid,
                           char *stroke,
                           int cx,
                           int cy,
                           int r,
                           double sang,
                           double dang,
                           bool svalid,
                           int sx,
                           int sy,
                           bool evalid,
                           int ex,
                           int ey)
{
  lht_node_t *ha_node;
  lht_node_t *te_node;
  char num[32];

  if (!node) goto exit;

  memset(num, 0, 32);

  sprintf(num, "arc.%d", oid);
  ha_node = lht_dom_node_alloc(LHT_HASH, strdup(num));
  if (!ha_node) goto exit;

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("cx"));
  if (!te_node) goto exit;
  sprintf(num, "%d", cx);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("cy"));
  if (!te_node) goto exit;
  sprintf(num, "%d", cy);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("r"));
  if (!te_node) goto exit;
  sprintf(num, "%d", r);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("sang"));
  if (!te_node) goto exit;
  sprintf(num, "%g", sang);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("dang"));
  if (!te_node) goto exit;
  sprintf(num, "%g", dang);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  if (!svalid)
  {
    te_node = lht_dom_node_alloc(LHT_TEXT, strdup("sx"));
    if (!te_node) goto exit;
    sprintf(num, "%d", sx);
    te_node->data.text.value = strdup(num);
    lht_dom_hash_put(ha_node, te_node);

    te_node = lht_dom_node_alloc(LHT_TEXT, strdup("sy"));
    if (!te_node) goto exit;
    sprintf(num, "%d", sy);
    te_node->data.text.value = strdup(num);
    lht_dom_hash_put(ha_node, te_node);
  }

  if (!svalid)
  {
    te_node = lht_dom_node_alloc(LHT_TEXT, strdup("ex"));
    if (!te_node) goto exit;
    sprintf(num, "%d", ex);
    te_node->data.text.value = strdup(num);
    lht_dom_hash_put(ha_node, te_node);

    te_node = lht_dom_node_alloc(LHT_TEXT, strdup("ey"));
    if (!te_node) goto exit;
    sprintf(num, "%d", ey);
    te_node->data.text.value = strdup(num);
    lht_dom_hash_put(ha_node, te_node);
  }

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("stroke"));
  if (!te_node) goto exit;
  te_node->data.text.value = strdup(stroke);
  lht_dom_hash_put(ha_node, te_node);

  lht_dom_list_append(node, ha_node);

exit:
  return;
}

  /**
   *  @fn void lihata_add_uuid(symbol *sy, lht_node_t *node)
   *  @brief adds a UUID sub-node to @p node
   *
   *  @param node - pointer to parent @a lht_node_t of UUID node
   *
   *  @par Returns
   *       Nothing.
   */

static void lihata_add_uuid(lht_node_t *node)
{
  lht_node_t *uuid_node;
  char *uuid_txt;

  if (!node) goto exit;

  uuid_txt = new_uuid();
  if (!uuid_txt) goto exit;

  uuid_node = lht_dom_node_alloc(LHT_TEXT, "uuid");
  if (!uuid_node) goto exit;

  uuid_node->data.text.value = strdup(uuid_txt);

  lht_dom_hash_put(node, uuid_node);

exit:
  return;
}

  /**
   *  @fn void lihata_add_symbol_objects(symbol *sy,
   *                                     int slot_idx,
   *                                     lht_node_t *node)
   *  @brief adds name to @p node, and visual outline, device name, and pin
   *         nodes to @p node
   *
   *  @param sy - pointer to @a symbol structure
   *  @param slot_idx - which slot # to use for object generation
   *  @param node - pointer to parent @a lht_node_t of children nodes
   *
   *  @par Returns
   *       Nothing.
   */

static void lihata_add_symbol_objects(symbol *sy,
                                      int slot_idx,
                                      lht_node_t *node)
{
  lht_node_t *li_node;

  if (!sy || !node) goto exit;

  li_node = lht_dom_node_alloc(LHT_LIST, "objects");
  if (!li_node) goto exit;

  lihata_add_symbol_name(sy, slot_idx, li_node);
  lihata_add_symbol_outline(sy, slot_idx, li_node);
  lihata_add_symbol_device_name(sy, slot_idx, li_node);
  lihata_add_symbol_pins(sy, slot_idx, li_node);

  lht_dom_hash_put(node, li_node);

exit:
  return;
}

  /**
   *  @fn void lihata_add_symbol_attributes(symbol *sy,
   *                                        int slot_idx,
   *                                        lht_node_t *node)
   *  @brief adds attributes node to @p node
   *
   *  @param sy - pointer to @a symbol structure
   *  @param slot_idx - which slot # to use for object generation
   *  @param node - pointer to parent @a lht_node_t of attributes node
   *
   *  @par Returns
   *       Nothing.
   */

static void lihata_add_symbol_attributes(symbol *sy,
                                         int slot_idx,
                                         lht_node_t *node)
{
  lht_node_t *ha_node;
  lht_node_t *te_node;
  slot *sl = NULL;
  attribute_array *attrs;
  attribute *attr;
  int i;

  if (!sy || !node) goto exit;

  ha_node = lht_dom_node_alloc(LHT_HASH, "attrib");
  if (!ha_node) goto exit;

  lht_dom_hash_put(node, ha_node);

  sl = slot_array_get(sy->slots, slot_idx);
  if (!sl) goto exit;

  attrs = sl->attributes;
  if (!attrs) goto exit;

    /*
     * Add generator specific attributes
     */

  te_node = lht_dom_node_alloc(LHT_TEXT, "-symbol-generator");
  te_node->data.text.value = strdup("gensym");
  lht_dom_hash_put(ha_node, te_node);
  
  te_node = lht_dom_node_alloc(LHT_TEXT, "role");
  te_node->data.text.value = strdup("symbol");
  lht_dom_hash_put(ha_node, te_node);
  
  te_node = lht_dom_node_alloc(LHT_TEXT, "name");
  te_node->data.text.value = strdup(sy->refdes);
  lht_dom_hash_put(ha_node, te_node);
  
  for (i = 0; i < attrs->n; i++)
  {
    attr = attrs->item[i];
    te_node = lht_dom_node_alloc(LHT_TEXT, attr->name);
    te_node->data.text.value = strdup(attr->value);
    lht_dom_hash_put(ha_node, te_node);
  }

exit:
  return;
}

  /**
   *  @fn void lihata_add_symbol_name(symbol *sy,
   *                                  int slot_idx,
   *                                  lht_node_t *node)
   *  @brief adds name node to @p node
   *
   *  @param sy - pointer to @a symbol structure
   *  @param slot_idx - which slot # to use for object generation
   *  @param node - pointer to parent @a lht_node_t of name node
   *
   *  @par Returns
   *       Nothing.
   */

static void lihata_add_symbol_name(symbol *sy,
                                   int slot_idx,
                                   lht_node_t *node)
{
  lht_node_t *ha_node;
  lht_node_t *te_node;
  char num[32];

  if (!sy || !node) goto exit;

  memset(num, 0, 32);

    /*
     * symbol name object
     */

  ha_node = lht_dom_node_alloc(LHT_HASH, strdup("text.1"));
  if (!ha_node) goto exit;

  te_node = lht_dom_node_alloc(LHT_TEXT, "stroke");
  if (!te_node) goto exit;
  te_node->data.text.value = strdup("sym-primary");
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, "dyntext");
  if (!te_node) goto exit;
  te_node->data.text.value = strdup("1");
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, "floater");
  if (!te_node) goto exit;
  te_node->data.text.value = strdup("1");
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, "text");
  if (!te_node) goto exit;
  te_node->data.text.value = strdup("%../A.name%");
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, "rot");
  if (!te_node) goto exit;
  te_node->data.text.value = strdup("0");
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, "x1");
  if (!te_node) goto exit;
  sprintf(num, "%d", -8000);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, "y1");
  if (!te_node) goto exit;
  sprintf(num, "%d", -4000);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  lht_dom_list_append(node, ha_node);

exit:
  return;
}

  /**
   *  @fn void lihata_add_symbol_outline(symbol *sy,
   *                                     int slot_idx,
   *                                     lht_node_t *node)
   *  @brief adds visual outline node to @p node
   *
   *  @param sy - pointer to @a symbol structure
   *  @param slot_idx - which slot # to use for object generation
   *  @param node - pointer to parent @a lht_node_t of outline node
   *
   *  @par Returns
   *       Nothing.
   */

static void lihata_add_symbol_outline(symbol *sy,
                                      int slot_idx,
                                      lht_node_t *node)
{
  slot *sl = NULL;
  lht_node_t *ha_node;
  lht_node_t *li_node;
  lht_node_t *te_node;
  char num[32];

  if (!sy || !node) goto exit;

  sl = slot_array_get(sy->slots, slot_idx);
  if (!sl) goto exit;

  memset(num, 0, 32);

  ha_node = lht_dom_node_alloc(LHT_HASH, strdup("polygon.2"));
  if (!ha_node) goto exit;

  li_node = lht_dom_node_alloc(LHT_LIST, strdup("outline"));
  if (!li_node) goto exit;

    /*
     *  left line
     */

  lihata_add_line(li_node, "sym-decor", 0, 0, 0, sl->height);

    /*
     *  top line
     */

  lihata_add_line(li_node, "sym-decor", 0, sl->height, sl->width, sl->height);

    /*
     *  right line
     */

  lihata_add_line(li_node, "sym-decor", sl->width, sl->height, sl->width, 0);

    /*
     *  bottom line
     */

  lihata_add_line(li_node, "sym-decor", sl->width, 0, 0, 0);

  lht_dom_hash_put(ha_node, li_node);

    /*
     *  stroke setting for entire outline
     */

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("stroke"));
  if (!te_node) goto exit;
  te_node->data.text.value = strdup("sym-decor");
  lht_dom_hash_put(ha_node, te_node);

  lht_dom_list_append(node, ha_node);

exit:
  return;
}

  /**
   *  @fn void lihata_add_symbol_device_name(symbol *sy,
   *                                         int slot_idx,
   *                                         lht_node_t *node)
   *  @brief adds visual device name node to @p node
   *
   *  @param sy - pointer to @a symbol structure
   *  @param slot_idx - which slot # to use for object generation
   *  @param node - pointer to parent @a lht_node_t of device name node
   *
   *  @par Returns
   *       Nothing.
   */

static void lihata_add_symbol_device_name(symbol *sy,
                                          int slot_idx,
                                          lht_node_t *node)
{
  slot *sl = NULL;
  lht_node_t *ha_node;
  lht_node_t *te_node;
  char *device_name = NULL;
  int dn_len = 0;
  int dn_width = 0;
  int x, y;
  int rot;
  direction dir = vertical;
  char num[25];

  if (!sy || !node) goto exit;

  sl = slot_array_get(sy->slots, slot_idx);
  if (!sl) goto exit;

  memset(num, 0, 25);

  device_name = find_device_name(sy, slot_idx);
  if (!device_name) goto exit;

  dn_len = find_device_name_length(sy, slot_idx);
  if (!dn_len) goto exit;

  dn_width = dn_len * 1500;

  if (sl->height > sl->width)
    dir = vertical;
  else
    dir = horizontal;

  x = y = rot = 0;

  switch (dir)
  {
    case vertical:
      x = (sl->width / 2) + 2000;
      y = ((sl->height - dn_width) / 2) + 2000;
      rot = 90;
      break;

    case horizontal:
      x = ((sl->width - dn_width) / 2) + 2000;
      y = (sl->height / 2) + 2000;
      break;
  }

  ha_node = lht_dom_node_alloc(LHT_HASH, strdup("text.3"));
  if (!ha_node) goto exit;

    /*
     *  stroke setting device name
     */

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("stroke"));
  if (!te_node) goto exit;
  te_node->data.text.value = strdup("sym-decor");
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("text"));
  if (!te_node) goto exit;
  te_node->data.text.value = strdup(device_name);
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("halign"));
  if (!te_node) goto exit;
  te_node->data.text.value = strdup("center");
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("rot"));
  if (!te_node) goto exit;
  sprintf(num, "%d", rot);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("x1"));
  if (!te_node) goto exit;
  sprintf(num, "%d", x);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  te_node = lht_dom_node_alloc(LHT_TEXT, strdup("y1"));
  if (!te_node) goto exit;
  sprintf(num, "%d", y);
  te_node->data.text.value = strdup(num);
  lht_dom_hash_put(ha_node, te_node);

  lht_dom_list_append(node, ha_node);

exit:
  return;
}

  /**
   *  @fn void lihata_add_symbol_pins(symbol *sy,
   *                                  int slot_idx,
   *                                  lht_node_t *node)
   *  @brief adds visual pin nodes to @p node
   *
   *  @param sy - pointer to @a symbol structure
   *  @param slot_idx - which slot # to use for object generation
   *  @param node - pointer to parent @a lht_node_t of device name node
   *
   *  @par Returns
   *       Nothing.
   */

static void lihata_add_symbol_pins(symbol *sy, int slot_idx, lht_node_t *node)
{
  pinalign_place place = pinalign_place_start;
  slot *sl;
  double x, y;
  int obj_counter = 4;
  int i;
  double step = 4000.0;

  if (!sy || !node) goto exit;

  sl = slot_array_get(sy->slots, slot_idx);
  if (!sl) goto exit;

    // left side pins

  if (sl) place = sl->pinalignment[pinalign_side_left].place;

  x = y = 0;

  switch (place)
  {
    case pinalign_place_start:
      y = sl->height - 2000;
      y -= (psl->top_width + 1) * 2000;
      break;

    case pinalign_place_center:
      y = sl->height - (sl->height - ((psl->left->n - 1) * 4000)) / 2;
      break;
  }

  for (i = 0; i < psl->left->n; i++)
  {
    lihata_add_symbol_pin(sy,
                          node,
                          psl->left->item[i],
                          obj_counter,
                          x,
                          y);
    y -= step;
    ++obj_counter;
  }

    // right side pins

  if (sl) place = sl->pinalignment[pinalign_side_right].place;

  x = sl->width;

  switch (place)
  {
    case pinalign_place_start:
      y = sl->height - 2000;
      y -= (psl->top_width + 1) * 2000;
      break;

    case pinalign_place_center:
      y = sl->height - (sl->height - ((psl->right->n - 1) * 4000)) / 2;
      break;
  }

  for (i = 0; i < psl->right->n; i++)
  {
    lihata_add_symbol_pin(sy,
                          node,
                          psl->right->item[i],
                          obj_counter,
                          x,
                          y);
    y -= step;
    ++obj_counter;
  }

    // top side pins

  if (sl) place = sl->pinalignment[pinalign_side_top].place;

  y = sl->height;

  switch (place)
  {
    case pinalign_place_start:
      x = sl->width - 2000;
      break;

    case pinalign_place_center:
      x = sl->width - (sl->width - ((psl->top->n - 1) * 4000)) / 2;
      break;
  }

  for (i = 0; i < psl->top->n; i++)
  {
    lihata_add_symbol_pin(sy,
                          node,
                          psl->top->item[i],
                          obj_counter,
                          x,
                          y);
    x -= step;
    ++obj_counter;
  }

    // bottom side pins

  if (sl) place = sl->pinalignment[pinalign_side_bottom].place;

  y = 0;

  switch (place)
  {
    case pinalign_place_start:
      x = sl->width - 2000;
      break;

    case pinalign_place_center:
      x = sl->width - (sl->width - ((psl->bottom->n - 1) * 4000)) / 2;
      break;
  }

  for (i = 0; i < psl->bottom->n; i++)
  {
    lihata_add_symbol_pin(sy,
                          node,
                          psl->bottom->item[i],
                          obj_counter,
                          x,
                          y);
    x -= step;
    ++obj_counter;
  }

exit:
  return;
}

  /**
   *  @fn void lihata_add_symbol_pin(symbol *sy,
   *                                 lht_node_t *node,
   *                                 pin *p,
   *                                 int obj_num,
   *                                 double x,
   *                                 double y)
   *  @brief adds visual pin node to @p node
   *
   *  @param sy - pointer to @a symbol structure
   *  @param slot_idx - which slot # to use for object generation
   *  @param node - pointer to parent @a lht_node_t of device name node
   *  @param obj_num - object id unique in parent @p node
   *  @param x - x value of pin anchor coordinate
   *  @param y - y value of pin anchor coordinate
   *
   *  @par Returns
   *       Nothing.
   */

static void lihata_add_symbol_pin(symbol *sy,
                                  lht_node_t *node,
                                  pin *p,
                                  int obj_num,
                                  double x,
                                  double y)
{
  lht_node_t *group = NULL;
  lht_node_t *objects = NULL;
  lht_node_t *hash = NULL;
  lht_node_t *hash2 = NULL;
  lht_node_t *text = NULL;
  lht_node_t *list = NULL;
  char scratch[64];
  int cx, cy;
  
  if (!sy || !node || !p) goto exit;
  if (!p->name && !p->label) goto exit;

  memset(scratch, 0, 64);

  sprintf(scratch, "group.%d", obj_num);
  group = lht_dom_node_alloc(LHT_HASH, scratch);
  if (!group) goto exit;

  lihata_add_uuid(group);
  lht_dom_list_append(node, group);

  objects = lht_dom_node_alloc(LHT_LIST, "objects");
  lht_dom_hash_put(group, objects);

    /*
     * show pin
     */

  hash = lht_dom_node_alloc(LHT_HASH, "line.1");
  lht_dom_list_append(objects, hash);

  text = lht_dom_node_alloc(LHT_TEXT, "stroke");
  text->data.text.value = strdup("term-decor");
  lht_dom_hash_put(hash, text);

  cx = cy = 0;

  if (p->direction) cx += 1000;
  if (p->inverted_circle) cx += 1000;

  text = lht_dom_node_alloc(LHT_TEXT, "x1");
  sprintf(scratch, "%d", cx);
  text->data.text.value = strdup(scratch);
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "y1");
  sprintf(scratch, "%d", cy);
  text->data.text.value = strdup(scratch);
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "x2");
  text->data.text.value = strdup("4000");
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "y2");
  text->data.text.value = strdup("0");
  lht_dom_hash_put(hash, text);

    /*
     * device dynamic display name
     */

  hash = lht_dom_node_alloc(LHT_HASH, "text.2");
  lht_dom_list_append(objects, hash);

  text = lht_dom_node_alloc(LHT_TEXT, "text");
  text->data.text.value = strdup("%../a.display/name%");
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "stroke");
  text->data.text.value = strdup("term-primary");
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "x1");
  text->data.text.value = strdup("1000");
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "y1");
  text->data.text.value = strdup("0");
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "dyntext");
  text->data.text.value = strdup("1");
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "rot");
  text->data.text.value = strdup("0");
  lht_dom_hash_put(hash, text);

    /*
     * device label
     */

  hash = lht_dom_node_alloc(LHT_HASH, "text.3");
  lht_dom_list_append(objects, hash);

  if (p->funcmap)
  {
    text = lht_dom_node_alloc(LHT_TEXT, "text");
    text->data.text.value = strdup("%../a.funcmap/name%");
    lht_dom_hash_put(hash, text);

    text = lht_dom_node_alloc(LHT_TEXT, "dyntext");
    text->data.text.value = strdup("1");
    lht_dom_hash_put(hash, text);
  }
  else
  {
    text = lht_dom_node_alloc(LHT_TEXT, "text");
    text->data.text.value = strdup(p->label);
    lht_dom_hash_put(hash, text);
  }

  text = lht_dom_node_alloc(LHT_TEXT, "stroke");
  text->data.text.value = strdup("term-secondary");
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "x1");
  text->data.text.value = strdup("-500");
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "y1");
  text->data.text.value = strdup("-2000");
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "mirx");
  text->data.text.value = strdup("1");
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "rot");
  text->data.text.value = strdup("0");
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "mirx");
  switch (p->location)
  {
    case pin_location_top:
    case pin_location_right:
      text->data.text.value = strdup("0");
      break;

    case pin_location_bottom:
    case pin_location_left:
      text->data.text.value = strdup("1");
      break;
  }
  lht_dom_hash_put(group, text);

    /*
     * direction indicator (if any)
     */

  switch (p->direction)
  {
    case pin_direction_in:
      hash2 = lht_dom_node_alloc(LHT_HASH, "polygon.4");
      if (!hash2) break;

      list = lht_dom_node_alloc(LHT_LIST, strdup("outline"));
      if (!list) goto exit;

      lihata_add_line(list, "term-primary", 0, 0, 1000, 1000);
      lihata_add_line(list, "term-primary", 1000, 1000, 1000, -1000);
      lihata_add_line(list, "term-primary", 1000, -1000, 0, 0);

      lht_dom_hash_put(hash2, list);

      text = lht_dom_node_alloc(LHT_TEXT, "stroke");
      text->data.text.value = strdup("term-primary");
      lht_dom_hash_put(hash2, text);

      text = lht_dom_node_alloc(LHT_TEXT, "fill");
      text->data.text.value = strdup("term-primary");
      lht_dom_hash_put(hash2, text);

      lht_dom_list_append(objects, hash2);
      break;

    case pin_direction_out:
      hash2 = lht_dom_node_alloc(LHT_HASH, "polygon.4");
      if (!hash2) break;

      list = lht_dom_node_alloc(LHT_LIST, strdup("outline"));
      if (!list) goto exit;

      lihata_add_line(list, "term-primary", 0, 1000, 1000, 0);
      lihata_add_line(list, "term-primary", 1000, 0, 0, -1000);
      lihata_add_line(list, "term-primary", 0, -1000, 0, 1000);

      lht_dom_hash_put(hash2, list);

      text = lht_dom_node_alloc(LHT_TEXT, "stroke");
      text->data.text.value = strdup("term-primary");
      lht_dom_hash_put(hash2, text);

      text = lht_dom_node_alloc(LHT_TEXT, "fill");
      text->data.text.value = strdup("term-primary");
      lht_dom_hash_put(hash2, text);

      lht_dom_list_append(objects, hash2);
      break;

    default: break;
  }

    /*
     * inverse circle (if any)
     */

  if (p->inverted_circle)
  {
    cx = 500;
    cy = 0;

    if (p->direction) cx += 1000;

    lihata_add_arc(objects,
                   5,
                   "term-primary",
                   cx,
                   cy,
                   500,
                   0,
                   360,
                   true,
                   0,
                   0,
                   true,
                   0,
                   0);
  }

  text = lht_dom_node_alloc(LHT_TEXT, "miry");
  text->data.text.value = strdup("0");
  lht_dom_hash_put(group, text);

  text = lht_dom_node_alloc(LHT_TEXT, "y");
  sprintf(scratch, "%g", y);
  text->data.text.value = strdup(scratch);
  lht_dom_hash_put(group, text);

  text = lht_dom_node_alloc(LHT_TEXT, "x");
  sprintf(scratch, "%g", x);
  text->data.text.value = strdup(scratch);
  lht_dom_hash_put(group, text);

  text = lht_dom_node_alloc(LHT_TEXT, "rot");
  switch (p->location)
  {
    case pin_location_top: text->data.text.value = strdup("90"); break;
    case pin_location_bottom: text->data.text.value = strdup("-90"); break;

    case pin_location_right:
    case pin_location_left:
    default:
      text->data.text.value = strdup("0");
      break;
  }
  lht_dom_hash_put(group, text);

  hash = lht_dom_node_alloc(LHT_HASH, "attrib");
  lht_dom_hash_put(group, hash);

  text = lht_dom_node_alloc(LHT_TEXT, "role");
  text->data.text.value = strdup("terminal");
  lht_dom_hash_put(hash, text);
  
  text = lht_dom_node_alloc(LHT_TEXT, "pinnum");
  sprintf(scratch, "%d", p->num);
  text->data.text.value = strdup(scratch);
  lht_dom_hash_put(hash, text);

  text = lht_dom_node_alloc(LHT_TEXT, "name");
  text->data.text.value = strdup(p->name);
  lht_dom_hash_put(hash, text);
  
exit:
  return;
}

  /**
   *  @fn char *find_device_name(symbol *sy, unsigned int slot_idx)
   *  @brief returns device name for slot # in @p sy
   *
   *  @param sy       - pointer to @a symbol structure
   *  @param slot_idx - which slot # to use for object generation
   *
   *  @return string containing device name, or NULL on error
   */

static char *find_device_name(symbol *sy, unsigned int slot_idx)
{
  char *name = NULL;
  slot *sl;
  attribute_array *aa;
  attribute *a;

  if (!sy) goto exit;

  sl = slot_array_get(sy->slots, slot_idx);
  if (!sl) goto exit;

  aa = sl->attributes;
  if (!aa) goto exit;

  for (a = attribute_array_first(aa); a; a = attribute_array_next(aa))
  {
    if (!strcmp(a->name, "device"))
    {
      name = a->value;
      break;
    }
  }

exit:
  return name;
}

  /**
   *  @fn int find_device_name_length(symbol *sy, unsigned int slot_idx)
   *  @brief returns string length of device name for slot # in @p sy
   *
   *  @param sy       - pointer to @a symbol structure
   *  @param slot_idx - which slot # to use for object generation
   *
   *  @return integer length of device name, or 0 on error or no device name
   */

static int find_device_name_length(symbol *sy, unsigned int slot_idx)
{
  char *name = NULL;
  int len = 0;

  if (!sy) goto exit;

  name = find_device_name(sy, slot_idx);
  if (!name) goto exit;

  len = strlen(name);

exit:
  return len;
}

