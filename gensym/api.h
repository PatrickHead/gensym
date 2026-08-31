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
 *  @file api.h
 *  @brief API for gensym
 */

#ifndef API_H
#define API_H

#include <libminuid.h>
#include <liblihata/dom.h>

#include "symbol.h"

  /**
   * @typedef gensym_options
   * @brief creates a type for @a gensym_options
   */

typedef struct gensym_options gensym_options;

  /**
   * @struct gensym_options
   * @brief gensym options
   */

struct gensym_options
{
  char *output_name;         /** alternate base name for output files */
  bool generate_slot_files;  /** flag, controls slot file generation  */
};

int gensym(char *file_name, gensym_options *opts);
lht_doc_t *gensym_lihata_doc(symbol *sy, unsigned int slot_idx);
int gensym_lihata_write(lht_doc_t *doc, char *name);

gensym_options *gensym_options_new(void);
void gensym_options_free(gensym_options *opts);
char *gensym_options_get_output_name(gensym_options *opts);
void gensym_options_set_output_name(gensym_options *opts, char *output_name);
bool gensym_options_get_generate_slot_files(gensym_options *opts);
void gensym_options_set_generate_slot_files(gensym_options *opts,
                                            bool generate_slot_files);

#endif // API_H
