//**********************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Profess9r John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//	File: main.cpp 													Revised Final Copy 2/12/2021
//
//**********************************************************************************************
#include "memory.h" // memory header file 
#include "hex.h"    // hex    header file        

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

/**
 * Message to User, verifying the command-line. 
 *
 * This function prints an appropriate "Usage" error message and "Patter" to 
 * cerr and terminates the program if it doesn't meet the appropiate requirements. 
 *
 * @note This functions needs to read the appropiate commands from the command line 
 *       otherwise it will return a message to the user. 
 **********************************************************************************************/
static void usage()
{
	cerr << "Usage: rv32i [-m hex-mem-size] infile" << endl;
	cerr << "    -m specify memory size (default = 0x100)" << endl;
	exit(1);
}

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
 *      Example: ./EXACUTABLE -m HEX-MEM-SIZE FILENAME.IN
 ***********************************************************************************************/
int main(int argc, char **argv)
{
	uint32_t memory_limit = 0x100;	// default memory size is 0x100

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

		default:
			usage();
		}
	}

	if (optind >= argc)
		usage();	// missing filename

	memory mem(memory_limit);
	mem.dump();

	if (!mem.load_file(argv[optind]))
		usage();

	// Displaying dump to user 
	mem.dump();

	// Displaying memory calling hex class
	cout << mem.get_size() << endl;
	cout << hex::to_hex32(mem.get8(0)) << endl;
	cout << hex::to_hex32(mem.get16(0)) << endl;
	cout << hex::to_hex32(mem.get32(0)) << endl;
	cout << hex::to_hex0x32(mem.get8(0)) << endl;
	cout << hex::to_hex0x32(mem.get16(0)) << endl;
	cout << hex::to_hex0x32(mem.get32(0)) << endl;
	cout << hex::to_hex8(mem.get8(0)) << endl;
	cout << hex::to_hex8(mem.get16(0)) << endl;
	cout << hex::to_hex8(mem.get32(0)) << endl;

	cout << hex::to_hex0x32(mem.get32(0x1000)) << endl;

	mem.set8(0x10, 0x12);
	mem.set16(0x14, 0x1234);
	mem.set32(0x18, 0x87654321);


	cout << hex::to_hex0x32(mem.get8_sx(0x0f)) << endl;
	cout << hex::to_hex0x32(mem.get8_sx(0x7f)) << endl;
	cout << hex::to_hex0x32(mem.get8_sx(0x80)) << endl;
	cout << hex::to_hex0x32(mem.get8_sx(0xe3)) << endl;

	cout << hex::to_hex0x32(mem.get16_sx(0xe0)) << endl;
	cout << hex::to_hex0x32(mem.get32_sx(0xe0)) << endl;

	mem.dump();

	return 0;
}