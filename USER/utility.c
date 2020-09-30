#include "utility.h"



struct Note MK_NOTATION(double x,size_t size){
	struct Note res;
	int temp = 1;

	res.value = 0.0;
	res.E     = 0;
    
    if(x == 0.0)
        return res;

	while(--size)
		temp *= 10;

	if(x < temp){
		while(x < temp){
			x *= 10;
			res.E--;
		}
	}else{
		while(x > temp){
			x /= 10;
			res.E++;
		}
	}
    res.value = x;
	return res;
}

double DE_NOTATION(struct Note* p){
    int    E     = p->E;
    double value = p->value;
    
    if(E > 0)
        while(E--)
            value*=10;
    else
        while(E++)
            value/=10;
    return value;
}

