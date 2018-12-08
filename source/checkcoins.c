#include <stdio.h>
#include <stdbool.h>

void main(void){
	
	int coin[5] = {2,7,9,3,5};
	int temp;
	bool quit = false;
	for(size_t iter = 0; iter < 5; iter++){
		for(size_t iter2 = 0; iter2 < 5; iter2++){
			for(size_t iter3 = 0; iter3 < 5; iter3++){
				for(size_t iter4 = 0; iter4 < 5; iter4++){
					for(size_t iter5 = 0; iter5 < 5; iter5++){
						temp = coin[iter] + coin[iter2] * (coin[iter3] * coin[iter3]) + (coin[iter4] * coin[iter4] * coin[iter4]) - coin[iter5];
						if(temp == 399){
							printf("%i %i %i %i %i \n", coin[iter], coin[iter2], coin[iter3], coin[iter4], coin[iter5]);
						}
					}
				}
			}
		}
	}
out:
	printf("done\n");
	
	
}
