#include <stdio.h>
#include <stdbool.h>

typedef unsigned short int uint16;
typedef struct{
	union thing{
		char byte[(1 << 16) + (1 << 4)];	
		unsigned short int instr[(1 << 15) + (1 << 3)];
	};
}memory;

bool isregister(uint16 address);

int main(void){
	
	memory buffer;
	uint16 temp;
	size_t bytesloaded = 0;
	size_t pc = 0;
	FILE * binary;
	binary = fopen("challenge.bin", "r");
	if(binary == NULL){
		printf("you suck\n");
		return 0;
	}
	while((buffer.byte[bytesloaded] = getc(binary)) != EOF){
		bytesloaded++;
		
	}
	printf("bytes loaded: %u\n\n", bytesloaded);
	
	bool error = false;
	bool halt = false;
	while(!halt){
		switch(buffer.instr[pc]){
			case 0:
				printf("HALT\n");
				//HALT
				halt = true;
				break;
			case 1:
				//SET
				printf("SET\n");
				pc++;
				if(isregister(buffer.instr[pc])){
					pc++;
					if(isregister(buffer.instr[pc])){
						buffer.instr[pc - 1] = buffer.instr[buffer.instr[pc]];
					}else{
						error = true;
						break;
					}
				}else{
					error = true;
					break;
				}
				break;
			case 2:
				//PUSH
				printf("PUSH\n");
				pc++;pc++;
				break;
			case 3:
				//POP
				printf("POP\n");
				pc++;pc++;
				break;
			case 4:
				//EQ
				printf("EQ\n");
				pc+=4;
				break;
			case 5:
				//GT
				printf("GT\n");
				pc+=4;
				break;
			case 6:
				//JMP
				printf("JMP\n");
				pc++;
				if(isregister(buffer.instr[pc])){
					pc = buffer.instr[buffer.instr[pc]];
				}else{
					pc = buffer.instr[pc];
				}
				
				break;
			case 7:
				//JT
				printf("JT\n");
				pc+=3;
				break;
			case 8:
				//JF
				printf("JF\n");
				pc+=3;
				break;
			case 9:
				//ADD
				printf("ADD\n");
				pc+=4;
				break;
			case 10:
				printf("MULT\n");
				pc+=4;
				break;
			case 11:
				printf("MOD\n");
				pc+=4;
				break;
			case 12:
				printf("AND\n");
				pc+=4;
				break;
			case 13:
				printf("OR\n");
				pc+=4;
				break;
			case 14:
				printf("NOT\n");
				pc+=3;
				break;
			case 15:
				printf("RMEM\n");
				pc+=3;
				break;
			case 16:
				printf("WMEM\n");
				pc+=3;
				break;
			case 17:
				printf("CALL\n");
				pc+=2;
				break;
			case 18:
				printf("RET\n");
				pc++;
				break;
			case 19:
				//OUT
				pc++;
				if(isregister(buffer.instr[pc])){
					temp = buffer.instr[buffer.instr[pc]];
					printf("%c", temp);
				}else{
					printf("%c", buffer.byte[(pc << 1)]);
				}
				pc++;
				break;	
			case 20:
				printf("IN\n");
				pc+=2;
				break;
			case 21:
				printf("NOP\n");
				//NOP
				pc++;
				break;
			default:
				printf("LITERAL VALUE?????");
				break;
		}	
	}
	
	return 0;
}

bool isregister(uint16 address){
	if((address > ((1 << 15) - 1)) && (address < ((1 << 15) + 6))){
		return true; 
	}else{
		return false;
	}
}

