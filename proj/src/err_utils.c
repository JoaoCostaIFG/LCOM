#include <errno.h>
#include <inttypes.h>
#include <lcom/lcf.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/err_utils.h"
#include "include/ev_disp.h"

/* ERROR AND WARNING */
static void
verr(FILE* fp, const char* fmt, va_list ap)
{
  vfprintf(stderr, fmt, ap);
  vfprintf(fp, fmt, ap);

  if (fmt[0] && fmt[strlen(fmt) - 1] == ':') {
    fputc(' ', stderr);
    fprintf(fp, "failed\n");
    perror(NULL);
  }
  else {
    fputc('\n', stderr);
    fputc('\n', fp);
  }
}

void
warn(const char* fmt, ...)
{
  FILE* fp;
  if ((fp = fopen(LOG_FILE, "a")) == NULL)
    return;

  va_list ap;
  va_start(ap, fmt);

  fprintf(stderr, "WARNING: ");
  fprintf(fp, "WARNING: ");

  verr(fp, fmt, ap);
  fclose(fp);
  va_end(ap);
}

void
die(const char* fmt, ...)
{
  FILE* fp;
  if ((fp = fopen(LOG_FILE, "a")) == NULL)
    return;

  va_list ap;
  va_start(ap, fmt);

  fprintf(stderr, "FATAL ERROR: ");
  fprintf(fp, "FATAL ERROR: ");

  verr(fp, fmt, ap);
  fclose(fp);
  va_end(ap);

  // Tell event dispatcher to exit
  quit();
}

void
clrlogs(void)
{
  /* clear log file contents */
  fclose(fopen(LOG_FILE, "w+"));
}

void
greet(void)
{
  printf(GREET_MSG " %s\n", LOG_FILE);

  tickdelay(micros_to_ticks(GREET_LEN));
}

void byebye(void)
{
  printf(GOODBYE_MSG);
}
