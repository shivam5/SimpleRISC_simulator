/*

The project is developed as part of Computer Architecture class
Project Name: Functional/Pipeline Simulator for simpleRISC Processor

Developer's Name: Shivam Mittal / Abhishek Chowdhry
Developer's Email id: 2015csb1032@iitrpr.ac.in / 2015csb1002@iitrpr.ac.in
Date: 1st October 2016

*/

/* mySimpleSim.h
   Purpose of this file: Header file for mySimpleSim
*/

#include "common.h"
#include "pipelineregisters.h"


void p_run_simplesim();


//reads from the instruction memory and updates the instruction register
void p_fetch();
//reads the instruction register, reads operand1, operand2 from register file, decides the operation to be performed in execute stage
void p_decode();
//executes the ALU operation based on ALUop
void p_execute();
//perform the memory operation
void p_mem();
//writes the results back to register file
void p_write_back();

void copy(pipelineregisters &a, pipelineregisters &b);
//
void flush();
void calculate_print_state();
bool checkdatalock(unsigned int A, unsigned int b);

//Intermediate functions
// Calculating immediate
unsigned int p_calculateimmediate();
// Calculating control signals
void p_calculatecontrol(unsigned int, unsigned int);
