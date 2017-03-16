/*

The project is developed as part of Computer Architecture class
Project Name: Functional/Pipeline Simulator for simpleRISC Processor

Developer's Name: Shivam Mittal / Abhishek Chowdhry
Developer's Email id: 2015csb1032@iitrpr.ac.in / 2015csb1002@iitrpr.ac.in
Date: 1st October 2016

*/

/* mySimpleSim.cpp
   Purpose of this file: implementation file for mySimpleSim
*/

#include "mySimpleSim.h"
#include <stdlib.h> 
#include <stdio.h>

// Register file
static unsigned int R[16];
// Program counter
static int PC;
// Flags
static int gt, eq;
// Memory
static unsigned char MEM[4000];

// Intermediate datapath and control path signals
// Intstruction encoding
static unsigned int instruction_word;
// Results of operand fetch unit
static unsigned int operand1, operand2, immediate, branchTarget;
// Results of ALU unit
static unsigned int branchPC, aluResult;
// Results of MEMORY unit
static unsigned int ldResult;

// Control signals
static int isSt, isLd, isBeq, isBgt, isRet, isImmediate, isWb, isUbranch, isCall;
static int isAdd, isSub, isCmp, isMul, isDiv, isMod, isLsl, isLsr, isAsr, isOr, isAnd, isNot, isMov;

// Calculated in ALU (branch unit)
static int isBranchTaken;

//Variable m to separate instruction memory and data memory
static int m;
// To know if we have reached the end of the program (We use an instruction 0xFFFFFFFF to know we have reached the end);
static int p_exit;

// Running our Simple RISC simulator
void run_simplesim() {
    // Run the 5 stages, the break condition is specified within the loop
    while (1) {
        // Instruction fetch unit
        fetch();
        // If we reach the end of the program (encounter the instruction 0xFFFFFFFF)
        if (p_exit == 1) {  
            break;
        }
        // Decode unit
        decode();
        // Execute unit
        execute();
        // Memory Unit
        mem();
        // Write Back unit
        write_back();

        printf("The state of registers after execution of the above instruction is:\n R0=%d \t R1=%d \t R2=%d \t R3=%d \n R4=%d \t R5=%d \t R6=%d \t R7=%d \n R8=%d \t R9=%d \t R10=%d \t R11=%d \n R12=%d \t R13=%d \t R14=%d \t R15=%d\n\n", R[0], R[1], R[2], R[3], R[4], R[5], R[6], R[7], R[8], R[9], R[10], R[11], R[12], R[13], R[14], R[15]);
    }

}

// It is used to set the reset values
// Reset all registers and memory content to 0
void reset_proc() {
    printf("Resetting processor \n");
    // Iterator variable
    int i;
    // Setting register to 0
    for (i = 0; i < 16; i++)
        R[i] = 0;

    // Setting flags to 0    
    gt = eq = 0;
    // PC is set to -4 instead of 0 , because in the first execution of IF stage, PC=PC+4.
    PC = -4;
    //Setting the memory to FF
    for (i = 0; i < 4000; i++)
        MEM[i] = 0xFF;
    // The number of instructions in the memory occupied initially is 0
    m = 0;
    // Set stack pointer to upper value of the memory
    R[14] = 4000;
    // First time branch is not taken
    isBranchTaken = 0;
    // Initial not exit condition
    p_exit = 0;

    printf("The state of registers after resetting the processor is:\n R0=%d \t R1=%d \t R2=%d \t R3=%d \n R4=%d \t R5=%d \t R6=%d \t R7=%d \n R8=%d \t R9=%d \t R10=%d \t R11=%d \n R12=%d \t R13=%d \t R14=%d \t R15=%d\n ", R[0], R[1], R[2], R[3], R[4], R[5], R[6], R[7], R[8], R[9], R[10], R[11], R[12], R[13], R[14], R[15]);
}

// Load_program_memory reads the input memory, and populates the instruction memory
void load_program_memory(char * file_name) {
    FILE * fp;
    unsigned int address, instruction;
    fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("Error opening input mem file\n");
        exit(1);
    }
    while (fscanf(fp, "%x %x", & address, & instruction) != EOF) {
        write_word(MEM, address, instruction);
        // Number of instructions
        m = m++;
    }
    fclose(fp);

}

// Writes the data memory in "data_out.mem" file
void write_data_memory() {

    FILE * fp;
    unsigned int i;
    fp = fopen("data_out.mem", "w");
    if (fp == NULL) {
        printf("Error opening dataout.mem file for writing\n");
        return;
    }

    for (i = 0; i < 4000; i = i + 4) {
        fprintf(fp, "%x %x\n", i, read_word(MEM, i));
    }
    fclose(fp);
}

// Reads from the instruction memory and updates the instruction register
void fetch() {

    printf("Instruction Fetch Unit\n");
    if (isBranchTaken) {
        printf("Branch is taken, so PC=branchPC\n");
        PC = branchPC;
    } else {
        printf("PC=PC+4\n");
        PC = PC + 4;
    }

    instruction_word = read_word(MEM, PC);
    printf("Executing Instruction : %x     from address : %x\n", instruction_word, PC);
    if (instruction_word == 0xFFFFFFFF) {
        p_exit = 1;
    }
}

// Calculating immediate
void calculateimmediate() {
    // Extracting modifier
    int modifier = (instruction_word & (0x00030000)) >> 16;
    // Extracting 16 bit immediate
    immediate = instruction_word & (0x0000FFFF);

    // Modifying immediate (Extending to 32 bit from 16 bit) according to modifier
    if (modifier == 0) {
        // Sign extension
        if ((immediate >> 15) == 1)
            immediate = 0xFFFF0000 | immediate;
    } else if (modifier == 1) {
        // Unsigned
    } else if (modifier == 2) {
        // Upper half extension
        immediate = (immediate << 16);
    }

}

// Calculating control signals
void calculatecontrol(unsigned int opcode, unsigned int I) {
    // Extracting op1, op2, op3, op4, op5
    int op1 = opcode & 0x00000001;
    int op2 = opcode & 0x00000002;
    int op3 = opcode & 0x00000004;
    int op4 = opcode & 0x00000008;
    int op5 = opcode & 0x00000010;

    isSt = (!op5) && op4 && op3 && op2 && op1;
    isLd = (!op5) && op4 && op3 && op2 && (!op1);
    isBeq = op5 && (!op4) && (!op3) && (!op2) && (!op1);
    isBgt = op5 && (!op4) && (!op3) && (!op2) && op1;
    isRet = op5 && (!op4) && op3 && (!op2) && (!op1);
    isImmediate = I;
    isWb = !(op5 || ((!op5) && op3 && op1 && (op4 || (!op2)))) || op5 && (!op4) && (!op3) && op2 && op1;
    isUbranch = op5 && (!op4) && (((!op3) && op2) || op3 && (!op2) && (!op1));
    isCall = op5 && (!op4) && (!op3) && op2 && op1;

    isAdd = ((!op5) && (!op4) && (!op3) && (!op2) && (!op1)) || ((!op5) && op4 && op3 && op2);
    isSub = (!op5) && (!op4) && (!op3) && (!op2) && op1;
    isCmp = (!op5) && (!op4) && op3 && (!op2) && op1;
    isMul = (!op5) && (!op4) && (!op3) && op2 && (!op1);
    isDiv = (!op5) && (!op4) && (!op3) && op2 && op1;
    isMod = (!op5) && (!op4) && op3 && (!op2) && (!op1);
    isLsl = (!op5) && op4 && (!op3) && op2 && (!op1);
    isLsr = (!op5) && op4 && (!op3) && op2 && op1;
    isAsr = (!op5) && op4 && op3 && (!op2) && (!op1);
    isOr = (!op5) && (!op4) && op3 && op2 && op1;
    isAnd = (!op5) && (!op4) && op3 && op2 && (!op1);
    isNot = (!op5) && op4 && (!op3) && (!op2) && (!op1);
    isMov = (!op5) && op4 && (!op3) && (!op2) && op1;
}

// DECODE UNIT
// Reads the instruction register, reads operand1, operand2 fromo register file
// Decides the operation to be performed in execute stage
void decode() {
    printf("DECODE UNIT\n");
    // Opcode and I
    unsigned int opcode, I;
    // Calculating immediate
    calculateimmediate();

    // Extracting branchTarget and multiplying by 4
    branchTarget = instruction_word & 0x07FFFFFF;
    branchTarget = branchTarget << 2;
    // Sign extension
    if (branchTarget >> 28 == 1) {
        branchTarget = branchTarget | 0xE0000000;
    }
    // Effective branchTarget
    branchTarget = branchTarget + PC;

    //Extracting opcode and I, and calculating control signals using that
    opcode = (0xF8000000 & instruction_word) >> 27;
    I = (0x04000000 & instruction_word) >> 26;
    calculatecontrol(opcode, I);

    // Choosing register number (for op1) from ra and rs1 on the basis of isRet
    if (isRet) {
        operand1 = 15;
    } else {
        operand1 = (instruction_word & 0x003C0000) >> 18;
    }
    operand1 = R[operand1];

    // Choosing register number (for op2) from rd and rs2 on the basis of isSt
    if (isSt) {
        operand2 = (instruction_word & 0x03C00000) >> 22;
    } else {
        operand2 = (instruction_word & 0x0003C000) >> 14;
    }
    operand2 = R[operand2];

    printf("Immediate is: %x, Branch Target is: %x, Operand1 = %x, Operand2= %x\n", immediate, branchTarget, operand1, operand2);
}

// EXECUTE UNIT
void execute() {

    printf("EXECUTE UNIT \n");

    // Branch Unit
    if (isRet) {
        branchPC = operand1;
    } else {
        branchPC = branchTarget;
    }
    // Calculating isBranchTaken
    isBranchTaken = isUbranch || (isBeq && eq) || (isBgt && gt);

    // ALU
    // Not overwriting operand2 in case the instruction is store, because it is used later (In MEMORY unit)
    // but for all other instructions, we have choosen between the two rs2 or immediate
    if (isImmediate && !isSt) {
        operand2 = immediate;
    }

    // If isAdd (it is either add, load or store)
    if (isAdd) {
        // If it isSt, then operand2 is not immediate
        if (isSt) {
            aluResult = operand1 + immediate;
        } else {
            aluResult = operand1 + operand2;
        }
    }

    if (isSub) {
        // Using 2s complement (As processor does :3)
        aluResult = operand1 + (~operand2) + 1;
    }

    if (isCmp) {
        // For sign comparisons
        int x = operand1, y = operand2;
        eq = gt = 0;
        if (x == y)
            eq = 1;
        if ((x + (~y) + 1) >> 31 == 0 && (x + (~y) + 1) != 0) {
            gt = 1;
        }
    }

    if (isMul) {
        aluResult = operand1 * operand2;
    }

    if (isDiv) {
        aluResult = operand1 / operand2;
    }

    if (isMod) {
        aluResult = operand1 % operand2;
    }

    if (isLsl) {
        aluResult = operand1 << operand2;
    }

    if (isLsr) {
        aluResult = operand1 >> operand2;
    }

    if (isAsr) {
        int x = 0, i;
        // Sign Extension
        if ((operand1 >> 31) == 1) {
            operand1 = (~operand1) + 1;
            operand1 = operand1 >> operand2;
            operand1 = (~operand1) + 1;
            aluResult = operand1;
        } else {
            aluResult = operand1 >> operand2;
        }
    }

    if (isOr) {
        aluResult = operand1 | operand2;
    }

    if (isNot) {
        aluResult = ~operand2;
    }

    if (isAnd) {
        aluResult = operand1 & operand2;
    }

    if (isMov) {
        aluResult = operand2;
    }

    printf("AluResult: %d\n", aluResult);

}

// MEMORY UNIT
void mem() {

    printf("MEMORY UNIT\n");

    // If isLd, then check whether it is valid data memory, not an instruction memory
    if (isLd) {
        if (aluResult <= (4 * m)) {
            printf("Tried to access instruction memory");
            exit(0);
        }
        // Load the result from the memory
        ldResult = read_word(MEM, aluResult);
        printf("Data (%d) loaded from memory address: %x\n", ldResult, aluResult);
    }
    // If the instructure is isSt, check whether it is valid data memory, not an instruction memory
    if (isSt) {
        if (aluResult <= 4 * m) {
            printf("Tried to access instruction memory");
            exit(0);
        }
        // Write the operand2 value in the memory at address aluResult
        write_word(MEM, aluResult, operand2);
        printf("Data (%d) stored in memory address: %x\n", operand2, aluResult);
    }
}

// Writes the results back to register file
void write_back() {
    printf("WRITEBACK UNIT\n");
    unsigned int result, rd;
    // Extracting rd
    rd = (instruction_word & 0x03C00000) >> 22;

    // Selecting which result to write
    if (isLd) {
        result = ldResult;
        printf("Written data (%d) into rd (%uth register) (For load instruction)\n", ldResult, rd);
    } else if (isCall) {
        result = PC + 4;
        printf("Savng PC+4 (%x) in ra(15th register)\n", PC + 4);
    } else {
        result = aluResult;
        printf("ALU result (%d) written into rd (%uth register)\n", aluResult, rd);
    }

    // If isWb, then write to the rdth register or 15th register if it is a call
    if (isWb) {
        if (isCall) {
            R[15] = result;
        } else {
            R[rd] = result;
        }
    }

}

// Reading data from a particular address in memory
int read_word(char * mem, unsigned int address) {
    int * data;
    data = (int * )(mem + address);
    return *data;
}

// Writing data at a particular adderss in memory
void write_word(char * mem, unsigned int address, unsigned int data) {
    int * data_p;
    data_p = (int * )(mem + address); * data_p = data;
}
