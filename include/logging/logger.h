#ifndef LOGGER_H
#define LOGGER_H


// terminal colors
#define KNORMAL  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGREEN  "\x1B[32m"
#define KYELLOW  "\x1B[33m"
#define KBLUE  "\x1B[34m"
#define KMAGENTA  "\x1B[35m"
#define KCYAN  "\x1B[36m"
#define KWHITE  "\x1B[37m"

// bolds
#define COLOR_BOLD  "\e[1m"
#define COLOR_OFF   "\e[m"

// tags
#define INFO_TAG "INFO"
#define ERROR_TAG "error"
#define WARNING_TAG "warning"


void logger(const char* tag, const char* message);

#endif
