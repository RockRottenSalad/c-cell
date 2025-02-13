#ifndef UTILS_H
#define UTILS_H

#include<stdio.h>

typedef unsigned char byte;

#define RESET_COLOR         "\033[0m"

#define FG_BRIGHT_GRAY      "\033[0;90m"
#define FG_BRIGHT_RED       "\033[0;91m"
#define FG_BRIGHT_GREEN     "\033[0;92m"
#define FG_BRIGHT_YELLOW    "\033[0;93m"
#define FG_BRIGHT_BLUE      "\033[0;94m"
#define FG_BRIGHT_MAGENTA   "\033[0;95m"
#define FG_BRIGHT_CYAN      "\033[0;96m"
#define FG_BRIGHT_WHITE     "\033[0;97m"

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define __LINE_STRING__ STRINGIZE(__LINE__)

#define PANIC(...)\
    fprintf(stderr, FG_BRIGHT_WHITE "|PANIC: ["__FILE__":%d]| " RESET_COLOR, __LINE__ );\
    fprintf(stderr, __VA_ARGS__);\
    fprintf(stderr, "\n");\
    exit(-1)

#define FREE_ELSE_PANIC(PTR)\
    if(PTR == NULL) {\
        PANIC("Attempted to free NULL pointer");\
    }\
    free(PTR);\
    PTR = NULL

// Undef this to disable all logs
#ifdef LOGGING
#define STR_VALUE(ARG) #ARG

#define LOG(...)\
    fprintf(stderr, FG_BRIGHT_WHITE "|LOG: ["__FILE__":%d]| " RESET_COLOR, __LINE__ );\
    fprintf(stderr, __VA_ARGS__);\
    fprintf(stderr, "\n")

#else

#define LOG(...)\
 
#endif

#endif
