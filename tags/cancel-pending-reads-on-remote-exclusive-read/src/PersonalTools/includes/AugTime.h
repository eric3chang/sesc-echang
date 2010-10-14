#ifndef AUGTIME_H
#define AUGTIME_H

class timeval
{
public:
	long tv_sec;
	long tv_usec;
};

inline int gettimeofday(timeval* t, void*)
{
	t->tv_sec = t->tv_usec = 0;
	return 0;
}

#endif