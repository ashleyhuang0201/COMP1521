// BigNum.c ... LARGE positive integer values

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "BigNum.h"

// Initialise a BigNum to N bytes, all zero
void initBigNum (BigNum *bn, int Nbytes)
{
    bn->bytes = malloc(Nbytes*sizeof(Byte));
    assert(bn->bytes != NULL);
    bn->nbytes = Nbytes;
    //initialise all to zero
    int i = 0;
    while (i < Nbytes) {
        bn->bytes[i] = 0;
        i++;
    }
	
}

// Add two BigNums and store result in a third BigNum
void addBigNums (BigNum bnA, BigNum bnB, BigNum *res)
{
    
    int counter = 0;
    //highbytes is the highest number of bytes the result can have
    int highbytes;
    if (bnA.nbytes >= bnB.nbytes) {
        highbytes = bnA.nbytes + 1;
    } else {
        highbytes = bnB.nbytes + 1;
    }
       
    if (highbytes > res->nbytes) {
        res->nbytes = highbytes;
        res->bytes = realloc(res->bytes, sizeof(Byte)*(res->nbytes));
        int i = 0;
        while (i < res->nbytes) {
            res->bytes[i] = 0;
            i++;
        }
    }
    
    int carryover = 0;
	while (counter < highbytes) {
	    int tempSum = carryover;
	    if (counter < bnA.nbytes) {
	        tempSum += bnA.bytes[counter];
        }
        
        if (counter < bnB.nbytes) {
            tempSum += bnB.bytes[counter];
        }
        
        carryover = 0;
        if (tempSum >= 10) {
            tempSum = tempSum - 10;
            carryover = 1;
            
        }
        res->bytes[counter] = tempSum;
        counter++;    
    }
    
}

// Set the value of a BigNum from a string of digits
// Returns 1 if it *was* a string of digits, 0 otherwise
int scanBigNum (char *s, BigNum *bn)
{
    assert(s != NULL && bn != NULL);
    
    bool numberEnd = false;
    char *start = s;
    //if number has started 0 included
    bool numberStart = false;
    //number has started, 0 discluded
    bool digitStart = false;
    bool validNumber = true;
    bool isZero = false;
    //finding the start of the number
    while (*start != '\0' && digitStart == false && validNumber == true) {
        if (isdigit(*start) || isspace(*start)) {
            //the digits have started and there is a space in between
            if (numberStart == true && isspace(*start)) {
                //checking if it is a 0 with spaces trailing
                //new pointer to check remaining validity
                char *spaceStart = start;
                while (*spaceStart != '\0' && validNumber == true) {
                    if (isspace(*spaceStart)) {
                        spaceStart++;
                        
                    } else {
                        validNumber = false;
                    }
                }
                isZero = true; 
            } else if(*start == '0' && numberStart == false){
                numberStart = true;
            
            } else if (*start != '0' && isspace(*start)==false) {  
            //its not equal to 0 so number starts
                numberStart = true;
                digitStart = true;  
            } else {
                start++;
            }
        //non-digit or space
        } else {
            validNumber = false;
        }   
    }
    int length = 0;
    //going through the number and finding its length
    while (*start != '\0' && validNumber == true && numberEnd == false) {
        if (isdigit(*start)) {
            start++;
            length++;
        } else if (isspace(*start)) {
            numberEnd = true;
        } else {
            validNumber = false;
        }
        
    }
    start--;
    char *endlocation = start;
    start++;
    //everything after the number ended has to be a space
    //otherwise, it is invalid
    
    while (*start != '\0' && validNumber == true) {
        if (isspace(*start)) {
            start++;
        } else {
            validNumber = false;
        }
    }
    //invalid string
    if (validNumber == false) {
        return 0;
    }
    //the whole number is 0
    if (isZero == true) {
        bn->bytes[0] = (Byte)0;
        return 1;
    }
        
        
    //if it requires more memory
    if (length > bn->nbytes) {
        bn->nbytes = length;
        bn->bytes = realloc(bn->bytes, sizeof(Byte)*(bn->nbytes));
        int i = 0;
    while (i < bn->nbytes) {
        bn->bytes[i] = 0;
        i++;
    }
        
    }
    
    //reading in backwards
    int counter = 0;
    while (counter <length) {
        
        bn->bytes[counter] = charToNum(*endlocation);
       
        endlocation--;
        counter++; 
        
    } 
    
  
    
    return 1;
     
}

// Display a BigNum in decimal format
void showBigNum (BigNum bn)
{
    int i = bn.nbytes - 1;
    //don't print out leading zeros
    while (bn.bytes[i] == 0 && i > 0) {
        i--;
    }
    
    while (i >= 0) {
        
        putchar(numToChar(bn.bytes[i]));
        i--;
    }

}

int charToNum (char a) {
    return a - '0';  
}

char numToChar (int a) {
    return a + '0';
}
    
        
