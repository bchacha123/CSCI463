//**********************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Professir John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//  File: cpu_single_hart.h                  Revised Final Copy Tue, 23 Mar 2021 16:31:54 +0000
//**********************************************************************************************
#include <sstream>
#include <iostream>     
#include <iomanip>      
#include <cassert>

#include "cpu_single_hart.h"
#include "registerfile.h"

/**
* @note This function will return the memory at the given instructions along with the number of instructions that were executed. 
* 
* @param exec_limit returning the exec limit and calling halted when the number of instructions have been executed.
***********************************************************************************************************************************/
void cpu_single_hart::run(uint64_t exec_limit)
{

    regs.set(2, mem.get_size());

    // Counter for the number of instructions executed
    uint32_t cnt = 0;

    while (!rv32i_hart::is_halted())
    {
        cnt++;

        //If exec_limit if zero tick is being called
        if (exec_limit == 0 || rv32i_hart::insn_counter < exec_limit)
        {
            rv32i_hart::tick();
        }
        else
        {   
            //Calling halt 
            rv32i_hart::halt = true;
        }

    }

    std::cout << "Execution terminated. Reason: EBREAK instruction" << std::endl;

    std::cout << cnt << " instructions executed" << std::endl;
}