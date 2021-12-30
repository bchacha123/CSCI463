//********************************************************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Professir John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//  File: registerfile.cpp                                          			Revised Final Copy Tue, 23 Mar 2021 15:55:21 +0000
//********************************************************************************************************************************
#include "hex.h"
#include "memory.h"
#include "registerfile.h"   // File added for assignment 5 -> registerfile.h

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <vector>

/**
 * @brief Implementing the register file, and setting register x0 to zero. 
 **********************************************************************************************/
registerfile::registerfile()
{
    reset();
    
}// End registerfile implementation


/**
 * @brief Implementation that will reset the registers and set register 0
 **********************************************************************************************/
 void registerfile::reset()
 {
    // Setting registers
    registers = std::vector<int32_t>(32, 0xf0f0f0f0);

    // Setting register 0 
    registers[0] = 0; 

 }// End reset()


 /**
 * @brief      Implementation of assign the designated register to the appropiate value
 *
 * @param r    register 
 * @param val  value 
 **********************************************************************************************/
 void registerfile::set(uint32_t r, int32_t val)
 {
    // If the register is not zero or less than or equal to 32 return the val of the register.
     if(r > 0 && r <= 32) registers[r] = val;

 }//End set()



 /**
 * @brief   Implementation of the return value of the register
 *
 * @param r return the value of the given register 
 **********************************************************************************************/
int32_t registerfile::get(uint32_t r) const
 {

    // If the register is not sero or less than or equal to 32 return the val of the register.
    if(r > 0 && r <= 32) return registers[r];

	return 0;

 }// End get()


 /**
 * @brief     Dump of the registers 
 *
 * @param hdr A header that gets printed as the dump gets printed with the appropiate format
 **********************************************************************************************/
void registerfile::dump(const std::string &hdr) const 
{
    for(int32_t i = 0; i < 32; i += 1)
    {
		if(i%8 == 0) std::cout << hdr << std::setfill(' ') << std::setw(3) << std::right << ("x" + std::to_string(i)) << " ";

		else if(i%4 == 0) std::cout << " ";

		std::cout << hdr << hex::to_hex32(registers[i]) << " ";

		if(i%8 == 7 and i != 0)
        {
			std::cout << std::endl;
		}
	}

}// End dump()