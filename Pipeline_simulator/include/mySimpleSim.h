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


// Results of operand fetch unit
static unsigned int operand1, operand2, immediate, branchTarget;
// Results of ALU unit
static unsigned int aluResult;
// Results of MEMORY unit
static unsigned int ldResult;

// Control signals
static int isSt, isLd, isBeq, isBgt, isRet, isImmediate, isWb, isUbranch, isCall;
static int isAdd, isSub, isCmp, isMul, isDiv, isMod, isLsl, isLsr, isAsr, isOr, isAnd, isNot, isMov;



void run_simplesim();


//reads from the instruction memory and updates the instruction register
void fetch();
//reads the instruction register, reads operand1, operand2 from register file, decides the operation to be performed in execute stage
void decode();
//executes the ALU operation based on ALUop
void execute();
//perform the memory operation
void mem();
//writes the results back to register file
void write_back();

//Intermediate functions
// Calculating immediate
void calculateimmediate();
// Calculating control signals
void calculatecontrol(unsigned int, unsigned int);
