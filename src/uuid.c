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
 *  @file uuid.c
 *  @brief helper to create new UUIDs for lihata objects
 */

#include <stdbool.h>
#include <libminuid.h>

#include "uuid.h"

static minuid_session_t _minuid_session;  /** global uuid generation session */

static bool minuid_init_done = false;     /**
                                           * controls first time uuid
                                           * session intialized
                                           */

  /**
   *  @fn char *new_uuid(void)
   *  @brief generates new UUID for lihata objects
   *
   *  NOTE:  This function is NOT reentrant, as it returns a pointer to a
   *         static buffer.
   *
   *  @par Parameters
   *       None.
   *
   *  @return string with new UUID
   */

char *new_uuid(void)
{
  static minuid_str_t txt;
  minuid_bin_t bin;

  if (!minuid_init_done)
  {
    minuid_init(&_minuid_session);
    minuid_init_done = true;
  }

  minuid_gen(&_minuid_session, bin);
  minuid_bin2str(txt, bin);

  return txt;
}

