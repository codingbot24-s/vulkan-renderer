//
// Created by saad on 23/09/26.
//

#include "loger.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void logg(const char *message,LOG_LEVEL log_level, ...) {
  const char *levels[3] = {"[FATAL ERROR TERMINATING THE APP]", "[WARNING]",
                           "[DEBUG INFO]"};

  fprintf(stderr, "%s %s", levels[log_level], message);



  fprintf(stderr, "\n");
  if (log_level == LEVEL_FATAL) {
    exit(EXIT_FAILURE);
  }
}
