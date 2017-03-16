/*

The project is developed as part of Computer Architecture class
Project Name: Functional/Pipeline Simulator for simpleRISC Processor

Developer's Name: Shivam Mittal / Abhishek Chowdhry
Developer's Email id: 2015csb1032@iitrpr.ac.in / 2015csb1002@iitrpr.ac.in
Date: 1st October 2016

*/

/* pipeline.cpp
   Purpose of this file: implementation file for pipeline
*/


#include "pipeline.h"
#include <stdlib.h> 
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

pipelineregisters IF_OF;
pipelineregisters OF_EX;
pipelineregisters EX_MA;
pipelineregisters MA_RW;
bool isStall=false;
unsigned int no_of_cycles=0;

void calculate_print_state()
{
    cout<<"\nIF\tOF\tEX\tMA\tWB\n";
    cout<<"I"<<(PC/4)+2<<"\t";
    if(IF_OF.PC==-1)
    {
        cout<<"-\t";
    }
    else
    {
        cout<<"I";
        cout<<(IF_OF.PC/4)+1<<"\t";
    }
    if(OF_EX.PC==-1)
    {
        cout<<"-\t";
    }
    else
    {
        cout<<"I";
        cout<<(OF_EX.PC/4)+1<<"\t";
    }
    if(EX_MA.PC==-1)
    {
        cout<<"-\t";
    }
    else
    {
        cout<<"I";
        cout<<(EX_MA.PC/4)+1<<"\t";
    }
    if(MA_RW.PC==-1)
    {
        cout<<"-\t";
    }
    else
    {
        cout<<"I";
        cout<<(MA_RW.PC/4)+1<<"\t";
    }
}

void copy(pipelineregisters &ob1, pipelineregisters &ob2){
     ob2.PC=ob1.PC;
     ob2.instruction_word=ob1.instruction_word;
     ob2.a=ob1.a;
     ob2.b=ob1.b;
     ob2.operand2=ob1.operand2;
     ob2.branchTarget=ob1.branchTarget;
     ob2.aluResult=ob1.aluResult;
     ob2.ldResult=ob1.ldResult;
     ob2.isSt=ob1.isSt;
     ob2.isLd=ob1.isLd;
     ob2.isBeq=ob1.isBeq;
     ob2.isBgt=ob1.isBgt;
     ob2.isRet=ob1.isRet;
     ob2.isImmediate=ob1.isImmediate;
     ob2.isWb=ob1.isWb;
     ob2.isUbranch=ob1.isUbranch;
     ob2.isCall=ob1.isCall;
     ob2.isAdd=ob1.isAdd;
     ob2.isSub=ob1.isSub;
     ob2.isCmp=ob1.isCmp;
     ob2.isMul=ob1.isMul;
     ob2.isDiv=ob1.isDiv;
     ob2.isMod=ob1.isMod;
     ob2.isLsl=ob1.isLsl;
     ob2.isLsr=ob1.isLsr;
     ob2.isAsr=ob1.isAsr;     
     ob2.isOr=ob1.isOr;
     ob2.isAnd=ob1.isAnd;
     ob2.isNot=ob1.isNot;
     ob2.isMov=ob1.isMov;
}


bool checkdatalock(unsigned int A, unsigned int B){
    unsigned int Aopcode, Bopcode, AI;
    
    Aopcode = (0xF8000000 & A) >> 27;
    Bopcode = (0xF8000000 & B) >> 27;

   // cout<<"Aopcode = "<<Aopcode<<endl;
   //     cout<<"Bopcode = "<<Bopcode<<endl;

    AI = (0x04000000 & A) >> 26;

    //if A.opcode == nop,b,beq,bgt,call
    if (Aopcode== 0xD || Aopcode== 0x12 || Aopcode== 0x10 || Aopcode== 0x11 || Aopcode== 0x13 )
        return false;
            
    // if B.opcode == nop, cmp, st, b, beq, bgt, ret
    if (Bopcode== 0xD || Bopcode== 0x5 || Bopcode== 0xF || Bopcode== 0x12 || Bopcode== 0x10 || Bopcode== 0x11 || Bopcode== 0x14 )
        return false;

    
    unsigned int src1, src2, dest;
    src1 = (A & 0x003C0000) >> 18;
    src2 = (A & 0x0003C000) >> 14;
    
    if (Aopcode == 0xF)
        src2= (A & 0x03C00000) >> 22;
    
    if (Aopcode == 0x14)
        src1 = R[15];
    
    dest = (B & 0x03C00000) >> 22;
    
    if (Bopcode == 0x13)
        dest=R[15];
        
    bool hasSrc2=true;
    
    if (Aopcode != 0xF){
        if (AI == 1)
            hasSrc2=false;
    }
    
    if (src1 == dest)
        return true;
    else if ( (hasSrc2==true) && (src2==dest))
        return true;    
    
    return false;
}

// Running our Simple RISC simulator
void p_run_simplesim() {
    // Run the 5 stages, the break condition is specified within the loop
    while (1) {

        cout<<"-----------------------------------------------------------------\n";    
        no_of_cycles++;
        cout<<endl<<endl<<endl<<endl<<endl<<endl;
        cout<<"-----------------------------------------------------------------\n";
        cout<<"Cycle no : "<<no_of_cycles<<endl;
        
        cout<<"STATE: \n";
        calculate_print_state();
        isStall=false;
        isStall=(checkdatalock(IF_OF.instruction_word,OF_EX.instruction_word))||(checkdatalock(IF_OF.instruction_word,EX_MA.instruction_word))||(checkdatalock(IF_OF.instruction_word,MA_RW.instruction_word));
        
            
        // Write Back unit
        p_write_back();

        // Memory Unit
        p_mem();
        
        // Execute unit
        p_execute();
        if(isBranchTaken)
        {
            OF_EX.instruction_word=0x68000000;
            OF_EX.isSt=OF_EX.isLd=OF_EX.isBeq=OF_EX.isBgt=OF_EX.isRet=OF_EX.isImmediate=OF_EX.isWb=OF_EX.isUbranch=OF_EX.isCall=0;
                            OF_EX.isAdd=OF_EX.isSub=OF_EX.isCmp=OF_EX.isMul=OF_EX.isDiv=OF_EX.isMod=OF_EX.isLsl=OF_EX.isLsr=OF_EX.isAsr=OF_EX.isOr=OF_EX.isAnd=OF_EX.isNot=OF_EX.isMov=0;    
            OF_EX.PC=-1;
            
            IF_OF.instruction_word=0x68000000;
            IF_OF.isSt=IF_OF.isLd=IF_OF.isBeq=IF_OF.isBgt=IF_OF.isRet=IF_OF.isImmediate=IF_OF.isWb=IF_OF.isUbranch=IF_OF.isCall=0;
                            IF_OF.isAdd=IF_OF.isSub=IF_OF.isCmp=IF_OF.isMul=IF_OF.isDiv=IF_OF.isMod=IF_OF.isLsl=IF_OF.isLsr=IF_OF.isAsr=IF_OF.isOr=IF_OF.isAnd=IF_OF.isNot=IF_OF.isMov=0; 
            IF_OF.PC=-1;       
        }
        if(!isStall)
        {
            // Decode unit
            p_decode();
            
            // Instruction fetch unit
            p_fetch();
            // If we reach the end of the program (encounter the instruction 0xFFFFFFFF)
            if (p_exit == 1) {  
                flush();
            }
        }
        else
        {
        
            cout<<"\nThe instructions in DECODE and FETCH units have been stalled, hence not displayed in this cycle.\n";
            OF_EX.instruction_word=0x68000000;
            OF_EX.isSt=OF_EX.isLd=OF_EX.isBeq=OF_EX.isBgt=OF_EX.isRet=OF_EX.isImmediate=OF_EX.isWb=OF_EX.isUbranch=OF_EX.isCall=0;
                            OF_EX.isAdd=OF_EX.isSub=OF_EX.isCmp=OF_EX.isMul=OF_EX.isDiv=OF_EX.isMod=OF_EX.isLsl=OF_EX.isLsr=OF_EX.isAsr=OF_EX.isOr=OF_EX.isAnd=OF_EX.isNot=OF_EX.isMov=0;           
            OF_EX.PC=-1;
        }



    }

}

//Function to flush the pipeline
void flush(){
    //p_write_back();
    //p_mem();
    //p_execute();
    //p_decode();
            no_of_cycles++;
        cout<<"-----------------------------------------------------------------\n";
        cout<<endl<<endl<<endl<<endl<<endl<<endl;
        cout<<"-----------------------------------------------------------------\n";
        cout<<"Cycle no : "<<no_of_cycles<<endl;
        cout<<"\nIF\tOF\tEX\tMA\tWB\n";
        cout<<"-\t-\t";
    if(OF_EX.PC==-1)
    {
        cout<<"-\t";
    }
    else
    {
        cout<<"I";
        cout<<(OF_EX.PC/4)+1<<"\t";
    }
    if(EX_MA.PC==-1)
    {
        cout<<"-\t";
    }
    else
    {
        cout<<"I";
        cout<<(EX_MA.PC/4)+1<<"\t";
    }
    if(MA_RW.PC==-1)
    {
        cout<<"-\t";
    }
    else
    {
        cout<<"I";
        cout<<(MA_RW.PC/4)+1<<"\t";
    }
        
        
    p_write_back();
    p_mem();
    p_execute();
    
            no_of_cycles++;
        cout<<"-----------------------------------------------------------------\n";
        cout<<endl<<endl<<endl<<endl<<endl<<endl;
        cout<<"-----------------------------------------------------------------\n";
        cout<<"Cycle no : "<<no_of_cycles<<endl;
        
        cout<<"\nIF\tOF\tEX\tMA\tWB\n";
        cout<<"-\t-\t-\t";
    if(EX_MA.PC==-1)
    {
        cout<<"-\t";
    }
    else
    {
        cout<<"I";
        cout<<(EX_MA.PC/4)+1<<"\t";
    }
    if(MA_RW.PC==-1)
    {
        cout<<"-\t";
    }
    else
    {
        cout<<"I";
        cout<<(MA_RW.PC/4)+1<<"\t";
    }
        
    p_write_back();
    p_mem();
    
        cout<<"-----------------------------------------------------------------\n";
        cout<<endl<<endl<<endl<<endl<<endl<<endl;
        cout<<"Cycle to flush the 0xFFFFFFFF instruction, not counted in the total nubmer of cycles\n ";
        cout<<"\nIF\tOF\tEX\tMA\tWB\n";
        cout<<"-\t-\t-\t-\t";
    if(MA_RW.PC==-1)
    {
        cout<<"-\t";
    }
    else
    {
        cout<<"I";
        cout<<(MA_RW.PC/4)+1<<"\t";
    }

    p_write_back();
        cout<<"-----------------------------------------------------------------\n";
            
    cout<<"\nNUMBER OF CYCLES: ";
    cout<<no_of_cycles<<endl;
            cout<<"-----------------------------------------------------------------\n";
    cout<<"\n";
    exit(0);
}

// Reads from the instruction memory and updates the instruction register
void p_fetch() {

    printf("\nINSTRUCTION FETCH UNIT\n");
    cout<<"PC Calculation :\n";
    if (isBranchTaken) {
        printf("Branch is taken, so PC=branchPC\n");
        PC = branchPC;
    } else {
        printf("PC=PC+4\n");
        PC = PC + 4;
    }

    instruction_word = read_word(MEM, PC);
    printf("Instruction : %x     from address : %x\n", instruction_word, PC);
    if (instruction_word == 0xFFFFFFFF) {
        if(OF_EX.isUbranch || OF_EX.isBeq || OF_EX.isBgt)
        {
            
        }
        else 
        {
            p_exit=1;
        }
    }
    
    IF_OF.PC=PC;
    IF_OF.instruction_word=instruction_word;
    
    cout<<"After executing this unit, IF-OF pipeline register : \n";
    cout<<"IF_OF->PC= "<<IF_OF.PC<<"\nIF_OF->instruction_word= "<<IF_OF.instruction_word<<endl;
}

// Calculating immediate
unsigned int p_calculateimmediate() {
    // Extracting modifier
    int modifier = (IF_OF.instruction_word & (0x00030000)) >> 16;
    // Extracting 16 bit immediate
    unsigned int immediate = IF_OF.instruction_word & (0x0000FFFF);

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
    
    return immediate;

}

// Calculating control signals
void p_calculatecontrol(unsigned int opcode, unsigned int I) {
    // Extracting op1, op2, op3, op4, op5
    int op1 = opcode & 0x00000001;
    int op2 = opcode & 0x00000002;
    int op3 = opcode & 0x00000004;
    int op4 = opcode & 0x00000008;
    int op5 = opcode & 0x00000010;

    OF_EX.isSt = (!op5) && op4 && op3 && op2 && op1;
    OF_EX.isLd = (!op5) && op4 && op3 && op2 && (!op1);
    OF_EX.isBeq = op5 && (!op4) && (!op3) && (!op2) && (!op1);
    OF_EX.isBgt = op5 && (!op4) && (!op3) && (!op2) && op1;
    OF_EX.isRet = op5 && (!op4) && op3 && (!op2) && (!op1);
    OF_EX.isImmediate = I;
    OF_EX.isWb = !(op5 || ((!op5) && op3 && op1 && (op4 || (!op2)))) || op5 && (!op4) && (!op3) && op2 && op1;
    OF_EX.isUbranch = op5 && (!op4) && (((!op3) && op2) || op3 && (!op2) && (!op1));
    OF_EX.isCall = op5 && (!op4) && (!op3) && op2 && op1;

    OF_EX.isAdd = ((!op5) && (!op4) && (!op3) && (!op2) && (!op1)) || ((!op5) && op4 && op3 && op2);
    OF_EX.isSub = (!op5) && (!op4) && (!op3) && (!op2) && op1;
    OF_EX.isCmp = (!op5) && (!op4) && op3 && (!op2) && op1;
    OF_EX.isMul = (!op5) && (!op4) && (!op3) && op2 && (!op1);
    OF_EX.isDiv = (!op5) && (!op4) && (!op3) && op2 && op1;
    OF_EX.isMod = (!op5) && (!op4) && op3 && (!op2) && (!op1);
    OF_EX.isLsl = (!op5) && op4 && (!op3) && op2 && (!op1);
    OF_EX.isLsr = (!op5) && op4 && (!op3) && op2 && op1;
    OF_EX.isAsr = (!op5) && op4 && op3 && (!op2) && (!op1);
    OF_EX.isOr = (!op5) && (!op4) && op3 && op2 && op1;
    OF_EX.isAnd = (!op5) && (!op4) && op3 && op2 && (!op1);
    OF_EX.isNot = (!op5) && op4 && (!op3) && (!op2) && (!op1);
    OF_EX.isMov = (!op5) && op4 && (!op3) && (!op2) && op1;
}

// DECODE UNIT
// Reads the instruction register, reads operand1, operand2 fromo register file
// Decides the operation to be performed in execute stage
void p_decode() {
    copy(IF_OF, OF_EX);
    
    
    printf("\nDECODE UNIT\n");
    printf("Instruction : %x     from address : %x\n", IF_OF.instruction_word, IF_OF.PC);
    // Opcode and I
    unsigned int opcode, I, immediate, operand1, operand2;
    // Calculating immediate
    immediate = p_calculateimmediate();

    // Extracting branchTarget and multiplying by 4
    OF_EX.branchTarget = IF_OF.instruction_word & 0x07FFFFFF;
    OF_EX.branchTarget = OF_EX.branchTarget << 2;
    // Sign extension
    if (OF_EX.branchTarget >> 28 == 1) {
        OF_EX.branchTarget = OF_EX.branchTarget | 0xE0000000;
    }
    // Effective branchTarget
    OF_EX.branchTarget = OF_EX.branchTarget + IF_OF.PC;

    //Extracting opcode and I, and calculating control signals using that
    opcode = (0xF8000000 & IF_OF.instruction_word) >> 27;
    I = (0x04000000 & IF_OF.instruction_word) >> 26;
    p_calculatecontrol(opcode, I);

    // Choosing register number (for op1) from ra and rs1 on the basis of isRet
    if (OF_EX.isRet) {
        operand1 = 15;
    } else {
        operand1 = (IF_OF.instruction_word & 0x003C0000) >> 18;
    }
    operand1 = R[operand1];

    // Choosing register number (for op2) from rd and rs2 on the basis of isSt
    if (OF_EX.isSt) {
        operand2 = (IF_OF.instruction_word & 0x03C00000) >> 22;
    } else {
        operand2 = (IF_OF.instruction_word & 0x0003C000) >> 14;
    }
    operand2 = R[operand2];
    
    OF_EX.a=operand1;
    OF_EX.operand2=operand2;
    
    if(OF_EX.isImmediate)
        OF_EX.b=immediate;
    else
        OF_EX.b=operand2;
    
        cout<<"After executing this unit, OF-EX pipeline register : \n";
    cout<<"OF_EX->PC= "<<OF_EX.PC<<"\nOF_EX->instruction_word= "<<OF_EX.instruction_word<<endl;
    cout<<"OF_EX->A= "<<OF_EX.a<<"\nOF_EX->B= "<<OF_EX.b<<endl;    
    cout<<"OF_EX->operand2= "<<OF_EX.operand2<<"\nControl signals are also stored "<<endl;
}

// EXECUTE UNIT
void p_execute() {

    unsigned int aluResult;
    copy(OF_EX,EX_MA);
    //unsigned int branchPC,isBranchTaken;
    printf("\nEXECUTE UNIT \n");
    printf("Instruction : %x     from address : %x\n", OF_EX.instruction_word, OF_EX.PC);
        
    cout<<OF_EX.a<<endl;
    cout<<OF_EX.b<<endl;
    cout<<OF_EX.operand2<<endl;

    // Branch Unit
    if (OF_EX.isRet) {
        branchPC = OF_EX.a;
    } else {
        branchPC = OF_EX.branchTarget;
    }
    // Calculating isBranchTaken
    isBranchTaken = OF_EX.isUbranch || (OF_EX.isBeq && eq) || (OF_EX.isBgt && gt);

    // ALU
    // Not overwriting operand2 in case the instruction is store, because it is used later (In MEMORY unit)
    // but for all other instructions, we have choosen between the two rs2 or immediate
    /*
    if (OF_EX.isImmediate && ! OF_EX.isSt) {
        operand2 = immediate;
    }
    */

    // If isAdd (it is either add, load or store)
    if (OF_EX.isAdd) {
        // If it isSt, then operand2 is not immediate
        /*
        if (isSt) {
            aluResult = operand1 + immediate;
        } else {
            aluResult = operand1 + operand2;
        }
        */
        aluResult=OF_EX.a+OF_EX.b;
    }

    if (OF_EX.isSub) {
        // Using 2s complement (As processor does :3)
        aluResult = OF_EX.a + (~(OF_EX.b)) + 1;
    }

    if (OF_EX.isCmp) {
        // For sign comparisons
        int x = OF_EX.a, y = OF_EX.b;
        eq = gt = 0;
        if (x == y)
            eq = 1;
        if ((x + (~y) + 1) >> 31 == 0 && (x + (~y) + 1) != 0) {
            gt = 1;
        }
    }

    if (OF_EX.isMul) {
        aluResult = OF_EX.a * OF_EX.b;
    }

    if (OF_EX.isDiv) {
        aluResult = OF_EX.a / OF_EX.b;
    }

    if (OF_EX.isMod) {
        aluResult = OF_EX.a % OF_EX.b;
    }

    if (OF_EX.isLsl) {
        aluResult = OF_EX.a << OF_EX.b;
    }

    if (OF_EX.isLsr) {
        aluResult = OF_EX.a >> OF_EX.b;
    }

    if (OF_EX.isAsr) {
        int x = 0, i;
        // Sign Extension
        if ((OF_EX.a >> 31) == 1) {
            OF_EX.a = (~(OF_EX.a)) + 1;
            OF_EX.a = OF_EX.a >> OF_EX.b;
            OF_EX.a = (~(OF_EX.a)) + 1;
            aluResult = OF_EX.a;
        } else {
            aluResult = OF_EX.a >> OF_EX.b;
        }
    }

    if (OF_EX.isOr) {
        aluResult = (OF_EX.a) | (OF_EX.b);
    }

    if (OF_EX.isNot) {
        aluResult = ~(OF_EX.b);
    }

    if (OF_EX.isAnd) {
        aluResult = (OF_EX.a) & (OF_EX.b);
    }

    if (OF_EX.isMov) {
        aluResult = OF_EX.b;
    }
    
    EX_MA.aluResult = aluResult;
    
            cout<<"After executing this unit, EX-MA pipeline register : \n";
    cout<<"EX_MA->PC= "<<EX_MA.PC<<"\nEX_MA->instruction_word= "<<EX_MA.instruction_word<<endl;
    cout<<"EX_MA->aluResult= "<<EX_MA.aluResult<<endl;    
    cout<<"EX_MA->operand2= "<<EX_MA.operand2<<"\nControl signals are also stored "<<endl;

}

// MEMORY UNIT
void p_mem() {

    copy(EX_MA, MA_RW);
    printf("\nMEMORY UNIT\n");
    printf("Instruction : %x     from address : %x\n", EX_MA.instruction_word, EX_MA.PC);
    // If isLd, then check whether it is valid data memory, not an instruction memory
    if (EX_MA.isLd) {
    // Change made, check for error
        if (EX_MA.aluResult < (4 * m)) {
            printf("Tried to access instruction memory\n");
            exit(0);
        }
        
        // Load the result from the memory
        MA_RW.ldResult = read_word(MEM, EX_MA.aluResult);
        printf("Data (%d) loaded from memory address: %x\n", MA_RW.ldResult, EX_MA.aluResult);
    }
    // If the instructure is isSt, check whether it is valid data memory, not an instruction memory
    if (EX_MA.isSt) {
    // Change made, check for error
        if (EX_MA.aluResult < 4 * m) {
            printf("Tried to access instruction memory\n");
            exit(0);
        }
        
        // Write the operand2 value in the memory at address aluResult
        write_word(MEM, EX_MA.aluResult, EX_MA.operand2);
        printf("Data (%d) stored in memory address: %x\n", EX_MA.operand2, EX_MA.aluResult);
    }
    
                cout<<"After executing this unit, MA-RW pipeline register : \n";
    cout<<"MA_RW->PC= "<<MA_RW.PC<<"\nMA_RW->instruction_word= "<<MA_RW.instruction_word<<endl;
    cout<<"MA_RW->aluResult= "<<MA_RW.aluResult<<"MA_RW->ldResult= "<<MA_RW.ldResult<<endl;    
    cout<<"MA_RW->operand2= "<<MA_RW.operand2<<"\nControl signals are also stored "<<endl;
}

// Writes the results back to register file
void p_write_back() {
    printf("\nWRITEBACK UNIT\n");
    printf("Instruction : %x     from address : %x\n", MA_RW.instruction_word, MA_RW.PC); 
    unsigned int result, rd;
    // Extracting rd
    rd = (MA_RW.instruction_word & 0x03C00000) >> 22;

    // Selecting which result to write
    if (MA_RW.isLd) {
        result = MA_RW.ldResult;
        printf("Written data (%d) into rd (%uth register) (For load instruction)\n", MA_RW.ldResult, rd);
    } else if (MA_RW.isCall) {
        result = MA_RW.PC + 4;
        printf("Saving PC+4 (%x) in ra(15th register)\n", MA_RW.PC + 4);
    } else {
        result = MA_RW.aluResult;
        printf("ALU result (%d) written into rd (%uth register)\n", MA_RW.aluResult, rd);
    }

    // If isWb, then write to the rdth register or 15th register if it is a call
    if (MA_RW.isWb) {
        if (MA_RW.isCall) {
            R[15] = result;
        } else {
            R[rd] = result;
        }
    }
                    printf("The state of registers after write_back of the above instruction is:\n\t\t R0=%d \n\t\t R1=%d \n\t\t R2=%d \n\t\t R3=%d \n\t\t R4=%d \n\t\t R5=%d \n\t\t R6=%d \n\t\t R7=%d \n\t\t R8=%d \n\t\t R9=%d \n\t\t R10=%d \n\t\t R11=%d \n\t\t R12=%d \n\t\t R13=%d \n\t\t R14=%d \n\t\t R15=%d\n\t\t\n", R[0], R[1], R[2], R[3], R[4], R[5], R[6], R[7], R[8], R[9], R[10], R[11], R[12], R[13], R[14], R[15]);


}
