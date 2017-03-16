/*

The project is developed as part of Computer Architecture class
Project Name: Functional/Pipeline Simulator for simpleRISC Processor

Developer's Name: Shivam Mittal / Abhishek Chowdhry
Developer's Email id: 2015csb1032@iitrpr.ac.in / 2015csb1002@iitrpr.ac.in
Date: 1st October 2016

*/

/* common.h
   Purpose of this file: Header file for commonn functions
*/


// Register file
extern unsigned int R[16];
// Program counter
extern int PC;
// Flags
extern int gt, eq;                  ////////////////////////
// Memory
extern char MEM[4000];

// Intermediate datapath and control path signals
// Intstruction encoding
extern unsigned int instruction_word; 
// Results of ALU unit
extern unsigned int branchPC;  

// Calculated in ALU (branch unit)
extern int isBranchTaken;

//Variable m to separate instruction memory and data memory
extern int m;
// To know if we have reached the end of the program (We use an instruction 0xFFFFFFFF to know we have reached the end);
extern int p_exit;

void reset_proc();
void load_program_memory(char* file_name);
void write_data_memory();

int read_word(char *mem, unsigned int address);
void write_word(char *mem, unsigned int address, unsigned int data);

