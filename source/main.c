#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef unsigned short int uint16;
typedef unsigned long long int uint64;

typedef struct{
	uint16 * vals;
	uint64 current;
	uint64 sizecap;
}buildastack;

buildastack stack;

typedef struct{
	union thing{
		char byte[(1 << 16) + (1 << 4)];	
		unsigned short int instr[(1 << 15) + (1 << 3)];
	};
}memory;

memory buffer;

size_t pc = 0;
size_t bytesloaded = 0;

void putregister(uint16 reg, uint16 val);
void steppc(uint16 advance);
void setpc(uint16 address);
void incrementpc(void);
void dojump(void);
void dumpregisters(void);
void dumpstack(void);
bool isregister(uint16 address);
bool stackmin(void);
bool stackmax(void);
void pushstack(uint16 val);
void growstack(void);
uint16 popstack(void);
uint16 getliteral(uint16 val);
uint16 getregister(uint16 reg);
uint16 getval(uint16 address);

int main(void){
	
	FILE * binary;
	binary = fopen("challenge.bin", "rb");
	if(binary == NULL){
		printf("COULD NOT LOAD BINARY\n");
		return 0;
	}
	
	stack.current = 0;
	stack.sizecap = 1024;
	stack.vals = (uint16 *)malloc(sizeof(uint16) * stack.sizecap);

	fseek(binary , 0L , SEEK_END);
	int F_SIZE = ftell( binary );
	rewind(binary);
	memset(buffer.byte, 0, F_SIZE);
	
	while(bytesloaded < F_SIZE){
		buffer.byte[bytesloaded] = getc(binary);
		bytesloaded++;
	}
	
	printf("%u bytes loaded\n\n", bytesloaded);
	
	bool halt = false;
	
	while(!halt){
		switch(buffer.instr[pc]){
			case 0:
				//HALT
				halt = true;
				break;
			case 1:
				//SET
				incrementpc();
				buffer.instr[buffer.instr[pc]] = getval(pc + 1);
				steppc(2);
				break;
			case 2:
				//PUSH
				incrementpc();
				if(stackmax()){
					growstack();
				}
				pushstack(getval(pc));
				incrementpc();
				break;
			case 3:
				//POP
				if(stackmin()){
					printf("CANT POP\n");
					halt = true;
					break;
				}
				incrementpc();
				buffer.instr[buffer.instr[pc]] = popstack();
				incrementpc();
				break;
			case 4:
				//EQ
				incrementpc();
				buffer.instr[buffer.instr[pc]] = (getval(pc+1) == getval(pc+2));
				steppc(3);
				break;
			case 5:
				//GT
				incrementpc();
				buffer.instr[buffer.instr[pc]] = (getval(pc+1) > getval(pc+2));
				steppc(3);
				break;
			case 6:
				//JMP
				incrementpc();
				dojump();
				break;
			case 7:
				//JT
				if(getval(pc+1)){
					steppc(2);
					dojump();
				}else{
					steppc(3);
				}
				break;
			case 8:
				//JF
				if(!getval(pc+1)){
					steppc(2);
					dojump();
				}else{
					steppc(3);
				}
				break;
			case 9:
				//ADD
				incrementpc();
				buffer.instr[buffer.instr[pc]] = getval(pc+1) + getval(pc+2);
				buffer.instr[buffer.instr[pc]] &= 0x7FFF;
				steppc(3);
				break;
			case 10:
				//MULT
				incrementpc();
				buffer.instr[buffer.instr[pc]] = getval(pc+1) * getval(pc+2);
				buffer.instr[buffer.instr[pc]] &= 0x7FFF;
				steppc(3);
				break;
			case 11:
				//MOD
				incrementpc();
				buffer.instr[buffer.instr[pc]] = getval(pc+1) % getval(pc+2);
				buffer.instr[buffer.instr[pc]] &= 0x7FFF;
				steppc(3);
				break;
			case 12:
				//AND
				incrementpc();
				buffer.instr[buffer.instr[pc]] = getval(pc+1) & getval(pc+2);
				buffer.instr[buffer.instr[pc]] &= 0x7FFF;
				steppc(3);
				break;
			case 13:
				//OR
				incrementpc();
				buffer.instr[buffer.instr[pc]] = getval(pc+1) | getval(pc+2);
				buffer.instr[buffer.instr[pc]] &= 0x7FFF;
				steppc(3);
				break;
			case 14:
				//NOT
				incrementpc();
				buffer.instr[buffer.instr[pc]] = ~(getval(pc+1));
				buffer.instr[buffer.instr[pc]] &= (uint16)0x7FFF;
				steppc(2);
				break;
			case 15:
				//RMEM
				incrementpc();
				if(isregister(buffer.instr[pc + 1])){
					buffer.instr[buffer.instr[pc]] = buffer.instr[buffer.instr[buffer.instr[pc + 1]]];
				}else{
					buffer.instr[buffer.instr[pc]] = buffer.instr[buffer.instr[pc + 1]];
				}
				steppc(2);
				break;
			case 16:
				//WMEM
				incrementpc();
				buffer.instr[getval(pc)] = getval(pc + 1);
				steppc(2);
				break;
			case 17:
				//CALL
				incrementpc();
				if(stack.current == stack.sizecap){
					stack.sizecap << 1;
					stack.vals = (uint16 *)realloc(stack.vals, stack.sizecap);
				}
				stack.vals[stack.current] = pc + 1;
				stack.current++;
				pc = getval(pc);
				break;
			case 18:
				//RET
				incrementpc();
				if(stack.current == 0){
					printf("CANT POP\n");
					halt = true;
					break;
				}
				stack.current--;
				pc = stack.vals[stack.current];
				break;
			case 19:
				//OUT
				incrementpc();
				if(isregister(buffer.instr[pc])){
					printf("%c", buffer.instr[buffer.instr[pc]]);
				}else{
					printf("%c", buffer.byte[(pc << 1)]);
				}
				incrementpc();
				break;	
			case 20:
				//IN
				incrementpc();
				buffer.instr[buffer.instr[pc]] = getchar();
				incrementpc();
				break;
			case 21:
				//NOP
				incrementpc();
				break;
			default:
				//INVALID
				printf("ENCOUNTERED INVALID OPCODE\n");
				halt = true;
				break;
		}
	}
	
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

void pushstack(uint16 val){
	stack.vals[stack.current] = val;
	stack.current++;
}

uint16 popstack(void){
	stack.current--;
	return stack.vals[stack.current];	
}

bool stackmin(void){
	return stack.current == 0;
}

bool stackmax(void){
	return stack.current == stack.sizecap;
}

void growstack(void){
	stack.sizecap << 1;
	stack.vals = (uint16 *)realloc(stack.vals, stack.sizecap);
}

