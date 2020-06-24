// COMP1521 19t2 ... lab 03: where are the bits?
// watb.c: determine bit-field order

#include <stdio.h>
#include <stdlib.h>

struct _bit_fields {
	unsigned int a : 4;
	unsigned int b : 8;
	unsigned int c : 20;
};

int main (void)
{
    //initialise a b and c to 1
	struct _bit_fields x = {1,1,1};
    unsigned int *ptrX = (unsigned int *) &x;
    
    //shift mask 31 spots to the left to begin loop forwards 
	unsigned int mask = 1 << 31;
	
	int i = 0;
	while (i < 32) {
	    if ((*ptrX & mask) > 0) {
	        printf("1");    
        } else {
            printf("0");
            
        }
	    i++;
	    //bit shift by one to the right
	    mask = mask >> 1;
    }
    printf("\n");
	
	//thus, the layout is c (20 bits long), b (8 bits long) and a (4 bits long)
	

	return EXIT_SUCCESS;
}
