#ifndef _UTILS_H_
#define _UTILS_H_
#include <stddef.h>

char * strcpy(char * target, const char * source);
void* memcpy(void* dest, const void* src, int count);
int strcmp(const char *s1, const char *s2);
int strncmp(const char * s1, const char * s2, size_t n);
int sprintf(char *out, const char *format, ...);

#endif
