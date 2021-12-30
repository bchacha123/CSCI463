//**********************************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Profess9r John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//	File: main.cpp 									      Revised Final Copy Tue, 23 Mar 2021 17:06:43 +0000
//**********************************************************************************************************
#include <stdio.h>
#include <ctype.h>
#include <fstream>
#include <sstream>
#include <cassert>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

#include "memory.h" 		  // memory header file 
#include "registerfile.h"	  // Files added for assignment 5 -> register.cpp
#include "rv32i_hart.h"		  // Files added for assignment 5 -> rc32i_hart.cpp
#include "cpu_single_hart.h"  // Files added for assignment 5 -> cpu_single_hart.cpp

using namespace std;

/**
 * Message to User, verifying the command-line. 
 *
 * This function prints an appropriate "Usage" error message and "Patter" to 
 * cerr and terminates the program if it doesn't meet the appropiate requirements. 
 *
 * @note This functions needs to read the appropiate commands from the command line 
 *       otherwise it will return a message to the user. 
 *       New parameters added 
 *          (d) show disassembly before program execution
 *          (i) show instruction printing during execution
 *          (l) maximum number of instructions to exec
 *          (m) specify memory size (default = 0 x100)
 *          (r) r show register printing during execution
 *          (z) show a dump of the regs & memory after simulation" << endl;
 *************************************************************************************************************/
static void usage()
{
	cerr << "Usage: rv32i [-d ] [-i] [-r] [-z] [-l exec -limit ] [-m hex -mem -size ] infile" << endl;
	cerr << "    -d show disassembly before program execution" << endl;
	cerr << "    -i show instruction printing during execution" << endl;
	cerr << "    -l maximum number of instructions to exec" << endl;
	cerr << "    -m specify memory size (default = 0 x100)" << endl;
	cerr << "    -r show register printing during execution" << endl;
	cerr << "    -z show a dump of the regs & memory after simulation" << endl;
	exit(1);
}

/**
 * Disassemble, this function will decode and print each instruction in a loop.
 * 
 * @note This function will place the correct formatting for the user.
 *		 First  column: 32-bit addrs in hex. 
 * 		 Second column: 32-bit hex-full word based on what was fetched from column #1.
 *		 Third 	column: Instruction mnemonic 
 * 		 Fourth column: Depends on the given instruction (register, hexadimal literal, decimal literal)
 *
 *********************************************************************************************************/
static void disassemble(const memory &mem)
{
	for (u_int pc = 0; pc < mem.get_size();)
	{
     std::cout << hex::to_hex32(pc) << ": " << hex::to_hex32(mem.get32(pc)) << "  ";
	 std::cout << rv32i_decode::decode(pc,mem.get32(pc)) << std::endl;
     pc += 4; 
	}

}//End disassemble 

/**
 * This is the main function that thrives the entire program
 *
 * This function will verify the commands the user enter and if incorrect 
 * it will call the usage function. If succeeded correctly it will simulate a 
 * computer system memory. 
 *
 * @param argc Argument counter, the number of arguments passed to the program 
 * @param argv Argument vector, each string will be one of the arguments that will pass to the prog. 
 *
 * @return 0
 *
 * @bug This only works when the user enter enters the correct commands  
 *      Command Example to Terminal ./Exe_file_name -{PARAMETERS[dilmrz]} -m0x100 file_name.bin 2>&1 | head -n 1000 > all_output_name.run
 * 
 * 		Makefile needs -Wno-unused flag 
 *		g++ -g -Wno-unused -ansi -pedantic -Wall -Werror -Wextra -std=c++14 -c -o rv32i_decode.o rv32i_decode.cpp
 *
 ********************************************************************************************************************************************/
int main(int argc, char **argv)
{
    int64_t insn_limit = 0;
    
    uint32_t memory_limit = 0x100;

    bool show_instructions = false;

    bool show_registers = false;

    bool dflag = false;

    bool zflag = false;

    int opt;

    while ((opt = getopt(argc, argv, "m:dirzl:")) != -1) 
    {
        switch (opt) 
        {
            case 'm':
                memory_limit = std::stoul(optarg, nullptr, 16);  
                break;

            case 'l':
                insn_limit = std::stoul(optarg, nullptr, 10); 
                break;

            case 'r':
                show_registers = true; 
                break;

            case 'd':
                dflag = true; 
                break;

            case 'i':
                show_instructions = true;
                break;

            case 'z':
                zflag = true; 
                break;
    
            default: usage();
        }

    }//End while case statement

    memory mem(memory_limit);
	
    cpu_single_hart cpuSim(mem);

    cpuSim.reset();

    cpuSim.set_show_registers(show_registers);

    cpuSim.set_show_instructions(show_instructions); 

    if(optind >= argc)
	{ 
		usage(); // missing filename
	}

    if(!mem.load_file(argv[optind]))
	{ 
		usage(); 
	}

    if(dflag) 
	{ 
		disassemble(mem); 
	}

    // Running the program otherwise it will ask for user-input
    cpuSim.run(insn_limit);

    // Dump at bottom 
    if(zflag) 
	{ 
        cpuSim.dump(); mem.dump(); 
    }

    return 0;

}// End main()