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

#include "pipelineregisters.h"

#include <iostream> 
#include <stdio.h>

using namespace std;

pipelineregisters::pipelineregisters(){
    PC=-1;
    instruction_word=0x68000000;
    a=0;
    b=0;
    operand2=0;
    branchTarget=0;
    aluResult=0;
    ldResult=0;
    isSt=isLd=isBeq=isBgt=isRet=isImmediate=isWb=isUbranch=isCall=0;
    isAdd=isSub=isCmp=isMul=isDiv=isMod=isLsl=isLsr=isAsr=isOr=isAnd=isNot=isMov=0;

}
