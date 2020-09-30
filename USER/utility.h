
#ifndef _UTILITY_H
#define _UTILITY_H

#include <stdint.h>
#include <stddef.h>


struct Note{
	double value;
	int    E;
};
struct Note MK_NOTATION(double x,size_t size);
double DE_NOTATION(struct Note* p);


#endif

