//**********************************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Profess9r John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//	File: main.cpp 																Revised Final Copy 3/2/2021
//
//**********************************************************************************************************
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include "hex.h"    		// hex    header file   
#include "memory.h" 		// memory header file 
#include "rv32i_decode.h"	// rv32i  header file      

using namespace std;

/**
 * Message to User, verifying the command-line. 
 *
 * This function prints an appropriate "Usage" error message and "Patter" to 
 * cerr and terminates the program if it doesn't meet the appropiate requirements. 
 *
 * @note This functions needs to read the appropiate commands from the command line 
 *       otherwise it will return a message to the user. 
 *********************************************************************************************************/
static void usage()
{
	cerr << "Usage: rv32i [-m hex-mem-size] infile" << endl;
	cerr << "    -m specify memory size (default = 0x100)" << endl;
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
 *      Example: ./EXACUTABLE  -mHEX-MEM-SIZE    DIRECTORY_OF_TESTINGFILES/NAME_OF_FILE  
 * 
 * 		Makefile needs -Wno-unused flag 
 *		g++ -g -Wno-unused -ansi -pedantic -Wall -Werror -Wextra -std=c++14 -c -o rv32i_decode.o rv32i_decode.cpp
 *
 ******************************************************************************************************************/
int main(int argc, char **argv)
{
	uint32_t memory_limit = 0x100;	// default memory size = 256 bytes

	int opt;
	while ((opt = getopt(argc, argv, "m:")) != -1)
	{
		switch(opt)
		{
		case 'm':
			{
				std::istringstream iss(optarg);
				iss >> std::hex >> memory_limit;
			}
			break;

		default: /* ’?’ */
			usage();
		}
	}

	if (optind >= argc)
		usage();	// missing filename

	memory mem(memory_limit);

	if (!mem.load_file(argv[optind]))
		usage();

	disassemble(mem);
	mem.dump();

	return 0;
}