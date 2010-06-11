#ifndef AUG_STRINGS_H
#define AUG_STRINGS_H

#include <cstring>
#include <string>
#include <cctype>

#ifdef _MSC_VER
#ifdef strdup
#undef strdup
#endif
#define strdup _strdup
#endif

#ifndef ssize_t
#define ssize_t int
#endif

#define bcmp memcmp
#define bcopy(a,b,c) memmove(b,a,c)
#define bzero(a,b) memset(a,0,b)
inline int strncasecmp(const char* a, const char* b, size_t l)
{
	for(size_t i = 0; i < l; i++)
	{
		if(toupper(a[i]) != toupper(b[i]))
		{
			return (toupper(a[i]) < toupper(b[i]))?-1:1;
		}
	}
	return 0;
}
inline int strcasecmp(const char* a, const char* b)
{
	return strncasecmp(a,b,std::min(std::strlen(a),std::strlen(b)) + 1);
}

#endif