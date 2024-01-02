#include "../../include/logging/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<string.h>

void logger(const char* tag, const char* message) {
   time_t now;
   time(&now);
   char* time_now = ctime(&now);
   time_now[strlen(time_now) - 1] = 0;
   printf("%s " COLOR_BOLD "[%s]" COLOR_OFF ": %s\n", time_now, tag, message);
}