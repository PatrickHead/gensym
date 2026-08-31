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
 *  @file lihata_helpers.c
 *  @brief helper functions for creating lihata document
 */

#include <stdbool.h>
#include <string.h>
#include <libminuid.h>
#include <liblihata/dom.h>

#include "uuid.h"
#include "lihata_helpers.h"

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

void lihata_add_line(lht_node_t *node,
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


void lihata_add_arc(lht_node_t *node,
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

void lihata_add_uuid(lht_node_t *node)
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

