//**********************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Professir John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//  File: cpu_single_hart.h                   Revised Final Copy Tue, 23 Mar 2021 16:30:38 +0000
//**********************************************************************************************
#ifndef CPU_SINGLE_HART_H
#define CPU_SINGLE_HART_H

#include "rv32i_hart.h"
/**
* @note This is a subclass of rv32i_hart that is used to represent a CPU with a single hart.
*/
class cpu_single_hart : public rv32i_hart
{
    public:

    cpu_single_hart(memory &mem) : rv32i_hart(mem) { }

    void run(uint64_t exec_limit);

};//End cpu_single_hart 

#endif