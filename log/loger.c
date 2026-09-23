//
// Created by saad on 23/09/26.
//

#include "loger.h"
#include <stdio.h>
#include <stdlib.h>

void logg(const char *message, LOG_LEVEL log_level) {
  const char *levels[2] = {"[FATAL ERROR TERMINATING THE APP]", "[WARNING]"};
  fprintf(stderr, "%s %s\n", levels[log_level], message);

  if (log_level == LEVEL_FATAL) {
    exit(EXIT_FAILURE);
  }
}
