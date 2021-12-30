//********************************************************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Professir John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//  File: registerfile.h                                          			    Revised Final Copy Tue, 23 Mar 2021 15:43:12 +0000
//********************************************************************************************************************************
#ifndef REGISTERFILE_H
#define REGISTERFILE_H

#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <string>
#include <vector>

/**
 * @note The purpose of this class is to store the state of the general-purpose registers
 *       of one RISC-V hart. RISC-V -> 32 registers, each is unique except x0 (ALWAYS zero)
 *******************************************************************************************/
class registerfile
{
    public:
        // Constructor 
        registerfile();

        // This function will initilize register x0 to zero and everything else to 0xf0f0f0f0.
        void reset();

        // This function will assigning registers r to the given val
        void set(uint32_t r, int32_t val);

        // This funciton will return the value of register r
        int32_t get(uint32_t r) const;

        // This function will return a dump of the registers. 
        void dump(const std::string &hdr) const;

    private: 
        // A vector of bytes representing the registers 
        std::vector<int32_t> registers;       

}; //End registerfile class 

#endif