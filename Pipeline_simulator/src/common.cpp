/*

The project is developed as part of Computer Architecture class
Project Name: Functional/Pipeline Simulator for simpleRISC Processor

Developer's Name: Shivam Mittal / Abhishek Chowdhry
Developer's Email id: 2015csb1032@iitrpr.ac.in / 2015csb1002@iitrpr.ac.in
Date: 1st October 2016

*/

/* common.cpp
   Purpose of this file: Common functions for both functional and pipeline simulator
*/

#include "common.h"
#include <stdlib.h> 
#include <stdio.h>

// Register file
unsigned int R[16];
// Program counter
int PC;
// Flags
int gt, eq;                  ////////////////////////
// Memory
char MEM[4000];

// Intermediate datapath and control path signals
// Intstruction encoding
unsigned int instruction_word; 
// Results of ALU unit
unsigned int branchPC;  

// Calculated in ALU (branch unit)
int isBranchTaken;

//Variable m to separate instruction memory and data memory
int m;
// To know if we have reached the end of the program (We use an instruction 0xFFFFFFFF to know we have reached the end);
int p_exit;



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
    
//    printf("Exit");
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
        m++;
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
