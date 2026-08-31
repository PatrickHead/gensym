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
 *  @file gensym.c
 *  @brief entry point for @b gensym application
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <libminuid.h>
#include <liblihata/dom.h>
#include <unistd.h>
#include <sys/param.h>
#include <getopt.h>

#include "api.h"
#include "debug.h"
#include "config.h"

#define GENSYM_VERSION VERSION

static void usage(void);

  /**
   *  @fn int main(int argc, char **argv)
   *  @brief command entry point
   *
   *  @param argc - number of command line arguments
   *  @param argv - array of argument strings
   *
   *  @return integer exit status
   */

int main(int argc, char **argv)
{
  int i;
  int c;
  gensym_options *gs_opts = NULL;

  gs_opts = gensym_options_new();
  if (!gs_opts) goto exit;

  while ((c = getopt(argc, argv, "ho:s")) != EOF)
  {
    switch (c)
    {
      case 'h':
        usage();
        return 0;

      case 'o':
        gs_opts->output_name = strdup(optarg);
        break;

      case 's':
        gs_opts->generate_slot_files = true;
        break;

      case '?':
      default:
        usage();
        return 1;
    }
  }

  if (argc <= optind)
  {
    usage();
    return 1;
  }

  for (i = optind; i < argc; i++)
    if (gensym(argv[i], gs_opts)) break;

exit:
  if (gs_opts) gensym_options_free(gs_opts);

  return 0;
}

  /**
   *  @fn void usage(void)
   *  @brief display usage message for application
   *
   *  @par Parameters
   *       None.
   *
   *  @par Returns
   *       Nothing.
   */

static void usage(void)
{
  printf("\n");
  printf("gensym v%s\n", GENSYM_VERSION);
  printf("\n");
  printf("usage:\n");
  printf("  gensym -h\n");
  printf("  gensym [-s] [-o <output name>] <file name> ...\n");
  printf("\n");
  printf("    -s generate file(s) for each symbol slot\n");
  printf("    -o use <output name> for output file(s)\n");
  printf("\n");
  printf("    <file name> name of boxsym(.bs) definition file\n");
  printf("\n");
}

