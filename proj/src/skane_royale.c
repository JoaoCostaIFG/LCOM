#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "include/ev_disp.h"
#include "include/err_utils.h"
#include "include/serial.h"
#include "include/utils.h"

/**
 * @mainpage  FEUP MIEIC LCOM's course final project documentation (2019/2020)
 *
 * @section  intro_sec  Skane Royale Edition
 *
 * @subsection  step1  Authors: Joao de Jesus Costa & Joao Lucas Silva Martins
 *
 * @image  html  report_cover.png
 */

static int
print_usage()
{
  printf(
    "Usage: <resources path - string> <mode - hex>\n");
  return 1;
}

int(proj_main_loop)(int argc, char* argv[])
{
  /* greet player */
  greet();

  /* read configs */
  if (argc != 0) {
    uint16_t mode;
    char path[256];

    switch (argc) {
      case 2: // video mode
        if (sscanf(argv[1], "%hx", &mode) != 1) {
          printf("%s: invalid mode (%s).\n", __func__, argv[2]);
          return print_usage();
        }
        set_videomode(mode);

      case 1: // resources path
        if (sscanf(argv[0], "%s", path) != 1) {
          printf("%s: invalid path (%s).\n", __func__, argv[1]);
          return print_usage();
        }
        set_resourcepath(path);

        break;
      default:
        break;
    }
  }

  /* start game */
  init();

  /* end game */
  quit();
  return 0;
  /* c: */
}

int
main(int argc, char* argv[])
{
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}
