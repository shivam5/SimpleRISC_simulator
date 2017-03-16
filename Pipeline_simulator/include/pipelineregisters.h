/*

The project is developed as part of Computer Architecture class
Project Name: Functional/Pipeline Simulator for simpleRISC Processor

Developer's Name: Shivam Mittal / Abhishek Chowdhry
Developer's Email id: 2015csb1032@iitrpr.ac.in / 2015csb1002@iitrpr.ac.in
Date: 1st October 2016

*/

/* pipeline.h
   Purpose of this file: Header file for pipeline
*/

#include "common.h"

class pipelineregisters{

public : 
pipelineregisters();
// Program counter
int PC;
// Flags
//extern int gt, eq;                  ////////////////////////

// Intermediate datapath and control path signals
// Intstruction encoding
unsigned int instruction_word; 
// Results of ALU unit      
////////////////////////////extern unsigned int branchPC;  

// Calculated in ALU (branch unit)
////////////////////////////extern int isBranchTaken;

// Results of operand fetch unit
unsigned int a,b, operand2;          /////////////////////////
unsigned int branchTarget;//,immediate;
// Results of ALU unit
unsigned int aluResult;
// Results of MEMORY unit
unsigned int ldResult;

// Control signals
int isSt, isLd, isBeq, isBgt, isRet, isImmediate, isWb, isUbranch, isCall;
int isAdd, isSub, isCmp, isMul, isDiv, isMod, isLsl, isLsr, isAsr, isOr, isAnd, isNot, isMov;

protected:

private:

};
