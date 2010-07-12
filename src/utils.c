#include "utils.h"

char *strcpy(char *target, const char *source)
{
	int i;

	for (i = 0; source[i] != '\0'; ++i)
		target[i] = source[i];
	target[i] = source[i];

	return target;
}

void* memcpy(void* dest, const void* src, int count)
{
	char* dst8 = (char*) dest;
	char* src8 = (char*) src;

	while (count--)
	{
		*dst8++ = *src8++;
	}
	return dest;
}

int strcmp(const char *s1, const char *s2)

{

	while ((*s1 && *s2) && (*s1++ == *s2++))
		;

	return *(--s1) - *(--s2);

}

int( strncmp)(const char *s1, const char *s2, size_t n)
{
	unsigned char uc1, uc2;

	if (n == 0)
		return 0;

	while (n-- > 0 && *s1 == *s2)
	{

		if (n == 0 || *s1 == '\0')
			return 0;
		s1++;
		s2++;
	}
	uc1 = (*(unsigned char *) s1);
	uc2 = (*(unsigned char *) s2);
	return ((uc1 < uc2) ? -1 : (uc1 > uc2));
}

