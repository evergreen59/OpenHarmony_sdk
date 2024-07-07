#pragma once
#include <string.h>
// support basename function
#include <libgen.h>

#define get_current_dir_name() getcwd(0,0)
#ifndef __MUSL__
#define __bswap_16(x) __builtin_bswap16(x)
#define __bswap_32(x) __builtin_bswap32(x)
#define __bswap_64(x) __builtin_bswap64(x)
#endif

// inline void* mempcpy(void* dst, const void* src, size_t n) {
//     return (char*)memcpy(dst, src, n) + n;
// }
