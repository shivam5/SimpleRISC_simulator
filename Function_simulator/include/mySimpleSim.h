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


void run_simplesim();
void reset_proc();
void load_program_memory(char* file_name);
void write_data_memory();



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

int read_word(char *mem, unsigned int address);
void write_word(char *mem, unsigned int address, unsigned int data);

