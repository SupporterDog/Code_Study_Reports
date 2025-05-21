#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define DATASEG_ADDR 0x10000000
#define TEXTSEG_ADDR 0x00400000


int changeStoI(const char* str) {
	int base = 10;
	bool isHex = (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'));

	if (isHex) {
		base = 16;
		str += 2;
	}

	return (isHex) ? (int)strtol(str, NULL, base) : atoi(str);
}


void setArguments(int argc, char* argv[], int* isAtp, int isM[], int* isD, int* isP, int* isN) {
	if (!strcmp(argv[1], "-antp")) {
		*isAtp = 0;
	}
	else if (!strcmp(argv[1], "-atp")) {
		*isAtp = 1;
	}
	else {
		printf("It is not atp or antp");
		exit(1);
	}

	for (int i = 2; i < argc - 1; i++) {
		if (!strcmp(argv[i], "-m")) {
			char* arg = argv[++i];
			isM[0] = changeStoI(strtok(arg, ":"));
			isM[1] = changeStoI(strtok(NULL, ":"));
		}
		else if (!strcmp(argv[i], "-d")) {
			*isD = 1;
		}
		else if (!strcmp(argv[i], "-p")) {
			*isP = 1;
		}
		else if (!strcmp(argv[i], "-n")) {
			*isN = changeStoI(argv[++i]);
		}
	}
}


unsigned int registers[32];
unsigned int pc = TEXTSEG_ADDR;
int* textSegment;
int* dataSegment;


int printRegs() {
	printf("Current register values :\n");
	printf("PC: 0x%x\n", pc);
	printf("Registers:\n");
	for (int i = 0; i < 32; i++) {
		printf("R%d: 0x%x\n", i, registers[i]);
	}
	printf("\n");
}

void writeBTM(unsigned int addr, unsigned int rt) {
	unsigned int data = registers[rt] & 0xFF;
	unsigned int offset = addr & 0x03;
	unsigned int mask = 0xFF << (offset * 8);
	unsigned int clearedData = dataSegment[addr / 4] & ~mask;
	dataSegment[addr / 4] = clearedData | (data << (offset * 8));
}


int loadBFM(unsigned int addr) {
	unsigned int offset = addr % 4;
	unsigned int word = dataSegment[addr >> 2];

	return (word >> ((3 - offset) * 8)) & 0xFF;
}

void printMemoryContent(int* dataSegment, int* textSegment, int dataSize, int textSize, int* mRange) {
	int start = mRange[0];
	int end = mRange[1];
	printf("Memory content [0x%x..0x%x] :\n", mRange[0], mRange[1]);
	printf("-------------------------------------------\n");
	for (unsigned int addr = start; addr <= end; addr += 4) {
		if (TEXTSEG_ADDR <= addr && addr < TEXTSEG_ADDR + textSize) {
			printf("0x%x: 0x%x\n", addr, textSegment[(addr - TEXTSEG_ADDR) >> 2]);
		}
		else if (DATASEG_ADDR <= addr && addr < DATASEG_ADDR + dataSize) {
			printf("0x%x: 0x%x\n", addr, dataSegment[(addr - DATASEG_ADDR) >> 2]);
		}
		else {
			printf("0x%x: 0x0\n", addr);
		}
	}
}


void printNullMemoryContent(int* memoryRange) {
	int start = memoryRange[0];
	int end = memoryRange[1];

	printf("Memory content [0x%x..0x%x] :\n", memoryRange[0], memoryRange[1]);
	printf("-------------------------------------------\n");
	for (unsigned int addr = start; addr <= end; addr += 4) {
		printf("0x%x: 0x0\n", addr);
	}
}

void printPipelineState(int textSegmentSize, int* pipeline) {
	printf("Current pipeline PC state :\n{");

	for (int i = 0; i < 5; i++) {
		if (i != 0) printf("|");
		if (pipeline[i] && pipeline[i] >= TEXTSEG_ADDR && pipeline[i] < TEXTSEG_ADDR + textSegmentSize) {
			printf("%#x", pipeline[i]);
		}
	}

	printf("}\n");
}

typedef struct IFID_StateRegister {
	int val, stall, pc, NPC, instr;
} IFID;

typedef struct IDEX_StateRegister {
	int val, stall, pc, NPC, op, rs, rt, rd, simm, uimm, control;
} IDEX;

typedef struct EXMEM_StateRegister {
	int val, pc, ALUOUT, rt, BRtarget, REGDst, control;
} EXMEM;

typedef struct MEMWB_StateRegister {
	int val, pc, ALUOUT, MEMOUT, REGDst, control;
} MEMWB;

char* loadFile(const char* filename, int* fileSize) {
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Error opening file.\n");
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	*fileSize = ftell(fp);
	rewind(fp);

	if (*fileSize == 0) {
		fclose(fp);
		return NULL;
	}

	char* buf = (char*)malloc(sizeof(char) * (*fileSize));
	if (buf == NULL) {
		printf("Memory allocation error.\n");
		fclose(fp);
		exit(1);
	}
	memset(buf, 0, *fileSize);

	fread(buf, 1, *fileSize, fp);
	fclose(fp);

	return buf;
}

void handleEmptyFile(int fileSize, int isM[]) {
	if (!fileSize || fileSize == 1) {
		printf("==== Completion cycle : %d ====\n\n", 0);
		printf("Current pipeline PC state :\n");
		printf("{||||}\n\n");
		printRegs();
		if (isM[0] != -1) {
			printNullMemoryContent(isM);
		}
		exit(0);
	}
}


int main(int argc, char* argv[]) {

	IFID NOW_IFID = { 0, };
	IDEX NOW_IDEX = { 0, };
	EXMEM NOW_EXMEM = { 0, };
	MEMWB NOW_MEMWB = { 0, };

	int isAtp = 1;
	int isM[2] = { -1, -1 };
	int isD = 0;
	int isP = 0;
	int isN = -1;

	for (int i = 0; i < 32; i++) { registers[i] = 0; }
	int cycles = 0;
	int pipeline_output[5] = { 0, 0, 0, 0, 0 };

	setArguments(argc, argv, &isAtp, isM, &isD, &isP, &isN);
	int fileSize;
	char* buf = loadFile(argv[argc - 1], &fileSize);
	handleEmptyFile(fileSize, isM);

	int sizeof_textSegment = changeStoI(strtok(buf, "\n"));
	int sizeof_dataSegment = changeStoI(strtok(NULL, "\n"));


	textSegment = (int*)malloc(sizeof_textSegment);
	dataSegment = (int*)malloc(sizeof_dataSegment);

	for (int i = 0; i < sizeof_textSegment / 4; i++) {
		textSegment[i] = changeStoI(strtok(NULL, "\n"));
	}
	for (int i = 0; i < sizeof_dataSegment / 4; i++) {
		dataSegment[i] = changeStoI(strtok(NULL, "\n"));
	}

	while (1) {
		

		for (int i = 0; i < 5; i++) { pipeline_output[i] = 0; }
		unsigned int jumptarget = 0;
		unsigned int ID_BRtarget = 0;
		unsigned int MEM_Taken = 0;

		if (NOW_MEMWB.val) {

			pipeline_output[4] = NOW_MEMWB.pc;
			if (NOW_MEMWB.control == 3) { 
				registers[NOW_MEMWB.REGDst] = NOW_MEMWB.MEMOUT;
			}
			else if (NOW_MEMWB.control == 1) { 
				registers[NOW_MEMWB.REGDst] = NOW_MEMWB.ALUOUT;
			}

			isN -= 1;
		}

		IFID NEXT_IFID = { 0, };
		IDEX NEXT_IDEX = { 0, };
		EXMEM NEXT_EXMEM = { 0, };
		MEMWB NEXT_MEMWB = { 0, };

		if (NOW_EXMEM.val) {
			int rt_value = registers[NOW_EXMEM.rt];
			if ((NOW_MEMWB.control & 1) && (NOW_MEMWB.REGDst != 0)) {
				if (NOW_MEMWB.REGDst == NOW_EXMEM.rt) {
					rt_value = registers[NOW_MEMWB.REGDst];
				}
			}


			pipeline_output[3] = NOW_EXMEM.pc;
			if (NOW_EXMEM.control == 1) {
				dataSegment[NOW_EXMEM.ALUOUT / 4] = registers[NOW_EXMEM.rt];
			}
			else if (NOW_EXMEM.control == 5) { 
				writeBTM(NOW_EXMEM.ALUOUT, NOW_EXMEM.rt);
			}
			else if (NOW_EXMEM.control == 98) { 
				NEXT_MEMWB.MEMOUT = dataSegment[NOW_EXMEM.ALUOUT / 4];
			}
			else if (NOW_EXMEM.control == 102) { 
				NEXT_MEMWB.MEMOUT = loadBFM(NOW_EXMEM.ALUOUT);
			}
			else if (NOW_EXMEM.control == 16) { 
				if (!NOW_EXMEM.ALUOUT) {
					MEM_Taken = 1;
				}
			}
			else if (NOW_EXMEM.control == 8) { 
				if (NOW_EXMEM.ALUOUT) {
					MEM_Taken = 1;
				}
			}

			NEXT_MEMWB.pc = NOW_EXMEM.pc;
			NEXT_MEMWB.ALUOUT = NOW_EXMEM.ALUOUT;
			NEXT_MEMWB.REGDst = NOW_EXMEM.REGDst;
			NEXT_MEMWB.control = NOW_EXMEM.control >> 5;
			NEXT_MEMWB.val = 1;



			if (NOW_EXMEM.control == 16 || NOW_EXMEM.control == 8) {
				if ((isAtp && !MEM_Taken) || (!isAtp && MEM_Taken)) {
					pc = (isAtp) ? NOW_EXMEM.pc + 4 : NOW_EXMEM.BRtarget;

					NEXT_EXMEM.val = 0;
					NEXT_EXMEM.pc = 0;
					NEXT_IDEX.val = 0;
					NEXT_IDEX.pc = 0;
					NEXT_IFID.val = 0;
					NEXT_IFID.pc = 0;

					NOW_EXMEM = NEXT_EXMEM;
					NOW_IDEX = NEXT_IDEX;
					NOW_IFID = NEXT_IFID;

					NOW_MEMWB = NEXT_MEMWB;


					printf("==== Cycle %d ====\n\n", ++cycles);
					if (isP) {
						printPipelineState(sizeof_textSegment, pipeline_output);
						printf("\n");
					}

					if (isD) {
						printRegs();
						if (isM[0] != -1) {
							printMemoryContent(dataSegment, textSegment, sizeof_dataSegment, sizeof_textSegment, isM);
						}
					}

					continue;
				}
			}

		}

		if (NOW_IDEX.val) {

			pipeline_output[2] = NOW_IDEX.pc;

			int rs_value = registers[NOW_IDEX.rs];
			int rt_value = registers[NOW_IDEX.rt];
			int rd_value = registers[NOW_IDEX.rd];


			if ((NOW_EXMEM.control & 32) && (NOW_EXMEM.REGDst != 0)) {
				if (NOW_EXMEM.REGDst == NOW_IDEX.rs) {
					rs_value = NOW_EXMEM.ALUOUT;
				}
				if (NOW_EXMEM.REGDst == NOW_IDEX.rt) {
					rt_value = NOW_EXMEM.ALUOUT;
				}
			}

			if ((NOW_MEMWB.control & 1) && (NOW_MEMWB.REGDst != 0) && (NOW_EXMEM.REGDst != NOW_EXMEM.REGDst)) {
				if (NOW_MEMWB.REGDst == NOW_IDEX.rs) {
					rs_value = NOW_MEMWB.ALUOUT; 
				}
				if (NOW_MEMWB.REGDst == NOW_IDEX.rt) {
					rt_value = NOW_MEMWB.ALUOUT;
				}
			}

			if (NOW_IDEX.op == 0) {
				NEXT_EXMEM.REGDst = NOW_IDEX.rd;
				int shamt = (NOW_IDEX.uimm >> 6) & 31;
				int funct = NOW_IDEX.uimm & 63;

				if (funct == 0x21) { 
					NEXT_EXMEM.ALUOUT = (rs_value + rt_value) & 0xffffffff;
				}
				else if (funct == 0x24) {
					NEXT_EXMEM.ALUOUT = rs_value & rt_value;
				}
				else if (funct == 0x2b) { 
					NEXT_EXMEM.ALUOUT = (rs_value < rt_value) ? 1 : 0;
				}
				else if (funct == 0) {
					NEXT_EXMEM.ALUOUT = rt_value << shamt;
				}
				else if (funct == 2) { 
					NEXT_EXMEM.ALUOUT = rt_value >> shamt;
				}
				else if (funct == 0x25) { 
					NEXT_EXMEM.ALUOUT = rs_value | rt_value;
				}
				else if (funct == 0x27) { 
					NEXT_EXMEM.ALUOUT = ~(rs_value | rt_value);
				}
				else if (funct == 0x23) { 
					NEXT_EXMEM.ALUOUT = rs_value - rt_value;
				}
				else if (funct == 8) { 
					NEXT_EXMEM.ALUOUT = rs_value;
				}
			}
			else if (NOW_IDEX.op == 3) {
				NEXT_EXMEM.ALUOUT = NOW_IDEX.NPC;
				NEXT_EXMEM.REGDst = 31;
			}
			else {
				NEXT_EXMEM.REGDst = NOW_IDEX.rt;

				if (NOW_IDEX.op == 9) { 
					NEXT_EXMEM.ALUOUT = rs_value + NOW_IDEX.simm;
				}
				else if (NOW_IDEX.op == 0xc) {
					NEXT_EXMEM.ALUOUT = rs_value & NOW_IDEX.uimm;
				}
				else if (NOW_IDEX.op == 0xf) { 
					NEXT_EXMEM.ALUOUT = NOW_IDEX.uimm << 16;
				}
				else if (NOW_IDEX.op == 0xd) {
					NEXT_EXMEM.ALUOUT = rs_value | NOW_IDEX.uimm;
				}
				else if (NOW_IDEX.op == 0xb) { 
					NEXT_EXMEM.ALUOUT = (rs_value < NOW_IDEX.simm) ? 1 : 0;
				}
				else if (NOW_IDEX.op == 4 || NOW_IDEX.op == 5) { 
					NEXT_EXMEM.BRtarget = NOW_IDEX.NPC + (NOW_IDEX.simm * 4);
					NEXT_EXMEM.ALUOUT = rs_value - rt_value;
				}
				else if (NOW_IDEX.op == 0x23 || NOW_IDEX.op == 0x20) { 
					NEXT_EXMEM.ALUOUT = rs_value + NOW_IDEX.simm - DATASEG_ADDR;
				}
				else if (NOW_IDEX.op == 0x2b || NOW_IDEX.op == 0x28) { 
					NEXT_EXMEM.ALUOUT = rs_value + NOW_IDEX.uimm - DATASEG_ADDR;
				}
			}


			NEXT_EXMEM.pc = NOW_IDEX.pc;
			NEXT_EXMEM.rt = NOW_IDEX.rt;
			NEXT_EXMEM.control = NOW_IDEX.control;
			NEXT_EXMEM.val = 1;


		}

		
		if (NOW_IFID.val) {
			pipeline_output[1] = NOW_IFID.pc;
			int instruction = NOW_IFID.instr;
			int op = (instruction >> 26) & 63;
			unsigned int rs = (instruction >> 21) & 31;
			unsigned int rt = (instruction >> 16) & 31;
			unsigned int rd = (instruction >> 11) & 31;

			unsigned int uimm = instruction & (int)(pow(2, 16) - 1);
			int simm = (uimm >> 15) ? 0xffff0000 | uimm : uimm;


			if ((NOW_IDEX.val) && (NOW_IDEX.control & 2) && ((NOW_IDEX.rt == rs) || (NOW_IDEX.rt == rt))) {
				pipeline_output[1] = 0;
				pipeline_output[0] = NOW_IFID.pc;

				NEXT_IDEX.val = 0;
				NEXT_IDEX.pc = 0;
				NOW_IDEX = NEXT_IDEX;

				NOW_EXMEM = NEXT_EXMEM;
				NOW_MEMWB = NEXT_MEMWB;

				printf("==== Cycle %d ====\n\n", ++cycles);
				if (isP) {
					printPipelineState(sizeof_textSegment, pipeline_output);
					printf("\n");
				}

				if (isD) {
					printRegs();
					if (isM[0] != -1) {
						printMemoryContent(dataSegment, textSegment, sizeof_dataSegment, sizeof_textSegment, isM);
					}
				}


				continue;
			}

			if (op == 0) { 
				NEXT_IDEX.control = 32; 
				int funct = instruction & 63;
				if (funct == 8) { 
					NEXT_IDEX.control = 0;
					jumptarget = instruction & (int)(pow(2, 26) - 1);
					jumptarget = registers[rs];
				}
			}
			else if (op == 2) { 
				NEXT_IDEX.control = 0;
				jumptarget = instruction & (int)(pow(2, 26) - 1);
				jumptarget = (pc & (15 << 28)) | (jumptarget * 4);
			}
			else if (op == 3) { 
				NEXT_IDEX.control = 32; 
				jumptarget = instruction & (int)(pow(2, 26) - 1);
				jumptarget = (pc & (15 << 28)) | (jumptarget * 4);
			}
			else if (op == 9 || op == 0xc || op == 0xf || op == 0xd || op == 0xb) { 
				NEXT_IDEX.control = 32; 
			}
			else if (op == 4) { 
				NEXT_IDEX.control = 16;
				ID_BRtarget = NOW_IFID.NPC + (simm * 4);
			}
			else if (op == 5) { 
				NEXT_IDEX.control = 8;
				ID_BRtarget = NOW_IFID.NPC + (simm * 4);
			}
			else if (op == 0x2b) { 
				NEXT_IDEX.control = 1;
			}
			else if (op == 0x28) { 
				NEXT_IDEX.control = 5;
			}
			else if (op == 0x23) { 
				NEXT_IDEX.control = 98;
			}
			else if (op == 0x20) { 
				NEXT_IDEX.control = 102;
			}


			NEXT_IDEX.pc = NOW_IFID.pc;
			NEXT_IDEX.NPC = NOW_IFID.NPC;

			NEXT_IDEX.op = op;
			NEXT_IDEX.rs = rs;
			NEXT_IDEX.rt = rt;
			NEXT_IDEX.rd = rd;
			NEXT_IDEX.uimm = uimm;
			NEXT_IDEX.simm = simm;

			NEXT_IDEX.val = 1;

		}


		
		if (sizeof_textSegment > (pc - TEXTSEG_ADDR) && pc >= TEXTSEG_ADDR) {
			pipeline_output[0] = pc;
			NEXT_IFID.pc = pc;
			NEXT_IFID.NPC = pc + 4;
			NEXT_IFID.instr = textSegment[(pc - TEXTSEG_ADDR) / 4];
			NEXT_IFID.val = 1;

			pc = pc + 4;
		}

		if (jumptarget) {
			pc = jumptarget;
			pipeline_output[0] = 0;
			NEXT_IFID.val = 0;
			NEXT_IFID.pc = 0;
		}
		if (ID_BRtarget && isAtp) {
			pc = ID_BRtarget;
			pipeline_output[0] = 0;
			NEXT_IFID.val = 0;
			NEXT_IFID.pc = 0;
		}


		
		NOW_IFID = NEXT_IFID;
		NOW_IDEX = NEXT_IDEX;
		NOW_EXMEM = NEXT_EXMEM;
		NOW_MEMWB = NEXT_MEMWB;

		if (isN == 0 || (sizeof_textSegment <= (pc - TEXTSEG_ADDR) && !(NOW_IFID.val) && !(NOW_IDEX.val) && !(NOW_EXMEM.val) && !(NOW_MEMWB.val))) { break; }

		
		printf("===== Cycle %d =====\n", ++cycles);
		if (isP) {
			printPipelineState(sizeof_textSegment, pipeline_output);
			printf("\n");
		}

		if (isD) {
			printRegs();
			if (isM[0] != -1) {
				printMemoryContent(dataSegment, textSegment, sizeof_dataSegment, sizeof_textSegment, isM);
			}
		}

		

	}

	printf("===== Completion cycle : %d =====\n\n", ++cycles);
	printPipelineState(sizeof_textSegment, pipeline_output);
	printf("\n");
	printRegs();


	free(dataSegment);
	free(textSegment);

	return 0;
}
