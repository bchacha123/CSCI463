//**********************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Professir John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//  
//  File: hex.h                                                         Revised Final Copy 
//
//*************************************************************************************************
#ifndef HEX_H
#define HEX_H

#include <string>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

/**
 * @note This class hold the variables that get called in the hex.cpp. Two new methods were added
 *
 **************************************************************************************************/
class hex
{
    public:
        static std::string to_hex8(uint8_t i); 

        static std::string to_hex32(uint32_t i);

        static std::string to_hex0x32(uint32_t i);

        // These two methods are additional methods needed for assignment 4
        static std::string to_hex0x20(uint32_t i);

        static std::string to_hex0x12(uint32_t i);
        
};// End hex class
#endif