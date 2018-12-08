#include <stdio.h>
#include <stdlib.h>

typedef unsigned long long int uint64;
typedef unsigned short int uint16;

typedef struct{
	uint16 * vals;
	uint64 current;
	uint64 sizecap;
}buildastack;

int main(void){
	
	printf("%08x, %08x", 0, ~0);
	return 0;
	buildastack mystack;
	mystack.sizecap = 1024;
	mystack.current = 0;
	mystack.vals = (uint16 *)malloc(sizeof(uint16) * mystack.sizecap);
	
	while(mystack.sizecap < ((uint64)1 << 32)){
		if(mystack.current == mystack.sizecap){
			printf("%u\n", mystack.sizecap);
			mystack.sizecap += mystack.sizecap;
			mystack.vals = (uint16 *)realloc(mystack.vals, mystack.sizecap);
		}
		mystack.current++;
	}
	

	return 0;
}

