#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short int uint16;
typedef unsigned long long int uint64;

typedef struct{
	uint16 * vals;
	uint64 current;
	uint64 sizecap;
}buildastack;

typedef struct{
	union thing{
		char byte[(1 << 16) + (1 << 4)];	
		unsigned short int instr[(1 << 15) + (1 << 3)];
	};
}memory;

memory buffer;
size_t pc = 0;
size_t bytesloaded = 0;
inputtimes = 0;
char input[8] = {'d', 'o', 'o', 'r', 'w', 'a', 'y', '\n'};
void dojump(void);
bool isregister(uint16 address);
uint16 getliteral(uint16 val);
uint16 getregister(uint16 reg);
void putregister(uint16 reg, uint16 val);
void incrementpc(void);
void steppc(uint16 advance);
void setpc(uint16 address);
uint16 getval(uint16 address);
void dumpregisters(void);
void dumpstack(void);
buildastack stack;

int main(void){
	
	
	uint16 temp;
	
	FILE * binary;
	binary = fopen("challenge.bin", "rb");
	if(binary == NULL){
		printf("you suck ass\n");
		return 0;
	}
	stack.current = 0;
	stack.sizecap = 1024;
	stack.vals = (uint16 *)malloc(sizeof(uint16) * stack.sizecap);

	fseek( binary , 0L , SEEK_END);
	int F_SIZE = ftell( binary );
	rewind( binary );
	memset(buffer.byte, 0, F_SIZE);
	while(bytesloaded < F_SIZE){
		buffer.byte[bytesloaded] = getc(binary);
		bytesloaded++;
	}
	
	bool error = false;
	bool halt = false;
	
	while(!halt){
		switch(buffer.instr[pc]){
			case 0:
				//HALT
				printf("HALT %x\n", pc * 2);
				halt = true;
				break;
			case 1:
				//SET
				//printf("SET\n");
				pc++;
				buffer.instr[buffer.instr[pc]] = getval(pc + 1);
				pc+=2;
				break;
			case 2:
				//PUSH
				//printf("PUSH\n");
				pc++;
				if(stack.current == stack.sizecap){
					stack.sizecap << 1;
					stack.vals = (uint16 *)realloc(stack.vals, stack.sizecap);
				}
				stack.vals[stack.current] = getval(pc);
				stack.current++;
				pc++;
				break;
			case 3:
				//POP
				//printf("POP\n");
				if(stack.current == 0){
					printf("CANT POP\n");
					pc++;pc++;
					break;
				}
				pc++;
				stack.current--;
				buffer.instr[buffer.instr[pc]] = stack.vals[stack.current];
				pc++;
				break;
			case 4:
				//EQ
				//printf("%hx: EQ\n", pc << 1);
				pc++;
				buffer.instr[buffer.instr[pc]] = (getval(pc+1) == getval(pc+2));
				pc+=3;
				break;
			case 5:
				//GT
				//printf("GT\n");
				pc++;
				buffer.instr[buffer.instr[pc]] = (getval(pc+1) > getval(pc+2));
				pc+=3;
				break;
			case 6:
				//JMP
				//printf("JMP %x\n", pc * 2);
				incrementpc();
				dojump();
				break;
			case 7:
				//JT
				//printf("%x: JT ", pc * 2);
				if(getval(pc+1)){
					steppc(2);
					dojump();
				}else{
					steppc(3);
				}
				//printf("%x\n", pc * 2);
				break;
			case 8:
				//JF
				//printf("%x: JF ", pc * 2);
				if(!getval(pc+1)){
					steppc(2);
					dojump();
				}else{
					steppc(3);
				}
				//printf("%x\n", pc * 2);
				break;
			case 9:
				//ADD
				//printf("ADD\n");
				pc++;
				buffer.instr[buffer.instr[pc]] = getval(pc+1) + getval(pc+2);
				buffer.instr[buffer.instr[pc]] &= 0x7FFF;
				pc+=3;
				break;
			case 10:
				//MULT
				//printf("MULT\n");
				pc++;
				buffer.instr[buffer.instr[pc]] = getval(pc+1) * getval(pc+2);
				buffer.instr[buffer.instr[pc]] &= 0x7FFF;
				pc+=3;
				break;
			case 11:
				//printf("MOD\n");
				pc++;
				buffer.instr[buffer.instr[pc]] = getval(pc+1) % getval(pc+2);
				buffer.instr[buffer.instr[pc]] &= 0x7FFF;
				pc+=3;
				break;
			case 12:
				//printf("AND\n");
				pc++;
				buffer.instr[buffer.instr[pc]] = getval(pc+1) & getval(pc+2);
				buffer.instr[buffer.instr[pc]] &= 0x7FFF;
				pc+=3;
				break;
			case 13:
				//printf("OR\n");
				pc++;
				buffer.instr[buffer.instr[pc]] = getval(pc+1) | getval(pc+2);
				buffer.instr[buffer.instr[pc]] &= 0x7FFF;
				pc+=3;
				break;
			case 14:
				//printf("NOT\n");
				pc++;
				buffer.instr[buffer.instr[pc]] = ~(getval(pc+1));
				buffer.instr[buffer.instr[pc]] &= (uint16)0x7FFF;
				pc+=2;
				break;
			case 15:
				//printf("%x: RMEM\n", pc << 1);
				pc++;
				if(isregister(buffer.instr[pc + 1])){
					buffer.instr[buffer.instr[pc]] = buffer.instr[buffer.instr[buffer.instr[pc + 1]]];
				}else{
					buffer.instr[buffer.instr[pc]] = buffer.instr[buffer.instr[pc + 1]];
				}
				pc+=2;
				break;
			case 16:
				//printf("WMEM\n");
				pc++;
				buffer.instr[getval(pc)] = getval(pc + 1);
				pc+=2;
				break;
			case 17:
				//printf("CALL\n");
				pc++;
				if(stack.current == stack.sizecap){
					stack.sizecap << 1;
					stack.vals = (uint16 *)realloc(stack.vals, stack.sizecap);
				}
				stack.vals[stack.current] = pc + 1;
				stack.current++;
				pc = getval(pc);
				break;
			case 18:
				//printf("RET\n");
				pc++;
				if(stack.current == 0){
					printf("CANT POP\n");
					pc++;
					break;
				}
				stack.current--;
				pc = stack.vals[stack.current];
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
				//IN
				pc++;
				buffer.instr[buffer.instr[pc]] = getchar();
				inputtimes++;
				pc++;
				break;
			case 21:
				//printf("NOP\n");
				//NOP
				pc++;
				break;
			default:
				printf("LIT VAL\n");
				pc++;
				break;
		}
	}
	//dumpmem();
	printf("EXECUTION HALT");
	return 0;
}

bool isregister(uint16 address){
	address -= (1 << 15);
	if(address >= 0 && address <= 7){
		return true; 
	}else{
		return false;
	}
}

uint16 getval(uint16 address){
	if(isregister(buffer.instr[address])){
		return getregister(address);
	}else{
		return getliteral(address);
	}
}

uint16 getliteral(uint16 val){
	return buffer.instr[val];
}

uint16 getregister(uint16 reg){
	return buffer.instr[buffer.instr[reg]];
}

void putregister(uint16 reg, uint16 val){
	buffer.instr[buffer.instr[reg]] = val;
}

void incrementpc(void){
	pc++;
}

void steppc(uint16 advance){
	pc += advance;
}

void setpc(uint16 address){
	pc = address;
}

void dojump(void){
	if(isregister(buffer.instr[pc])){
		pc = getregister(pc);
	}else{
		pc = getliteral(pc);
	}
}

void dumpregisters(void){
	for(size_t iter = 0; iter < 8; iter++){
		printf("register %i: %u\n", iter, buffer.instr[(1 << 15) + iter]);
	}
}

void dumpstack(void){
	for(int iter = stack.current; iter >= 0; iter--){
		printf("at %i: %hu\n", iter, stack.vals[iter]);
	}
}

void dumpmem(void){
	for(size_t iter = 0; iter < bytesloaded; iter++){
		printf("%02hhx\t", (unsigned char)buffer.byte[iter]);
		if(!(iter % 16)){
			printf("\n");
		}
	}
}

