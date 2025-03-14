#ifndef SHARED_H
#define SHARED_H

//#include "pch.h"

// Linux
#include <unistd.h>     // access
#include <arpa/inet.h>  // sockaddr_in, inet_pton
#include <execinfo.h>   // backtrace
#include <dlfcn.h>      // dlsym
#include <sys/time.h>   // gettimeofday
#include <linux/limits.h>   // PATH_MAX
#include <signal.h>     // signal

// C++
#include <array>
#include <cmath>        // fabs, sqrt
#include <cstdarg>      // va_start, va_end
#include <cstdio>       // printf, snprintf
#include <cstring>      // strcasecmp, strlen, strcmp, strerror, strncpy...
#include <filesystem>
#include <map>          // make_tuple, get, array
#include <sstream>      // ostringstream
#include <string>
#include <vector>

#include "types.h"
#include "functions.h"

extern uintptr_t resume_addr_Jump_Check;
extern uintptr_t resume_addr_Jump_Check_2;

extern customPlayerState_t customPlayerState[MAX_CLIENTS];

__attribute__ ((naked)) void hook_Jump_Check_Naked();
__attribute__ ((naked)) void hook_Jump_Check_Naked_2();

xfunction_t Scr_GetCustomFunction(const char **fname, qboolean *fdev);
xmethod_t Scr_GetCustomMethod(const char **fname, qboolean *fdev);

void Scr_CodeCallback_Error(qboolean terminal, qboolean emit, const char *internal_function, char *message);

void free_sqlite_db_stores_and_tasks();

#endif
