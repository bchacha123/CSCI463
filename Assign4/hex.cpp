//**********************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Professir John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//  File: hex.cpp                                                   Revised Final Copy 3/2/2021
//
//**********************************************************************************************

#include "hex.h"            
#include "memory.h"
#include "rv32i_decode.h"

/**
 * @brief This function will return a string with exactly 2 hex digits 
 *
 * @param i Used to cast and represent 2 hex digits the 8 bits
 *
 * @return Will return a std::string
 **********************************************************************************************/
std::string hex::to_hex8(uint8_t i)
{
    std::ostringstream os;

    os << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(i);

    return os.str();

}//End to_hex8

/**
 * @brief This function will return a string with 8 hex digits 
 *
 * @param i Used to cast and represent the 32-bits 
 *
 * @return will return a std::string 
 **********************************************************************************************/
 std::string hex::to_hex32(uint32_t i)
 {
    std::ostringstream os;

    os << std::hex << std::setfill('0') << std::setw(8) << static_cast<uint32_t>(i);

    return os.str();

 }// End to_hex32

/**
 * @brief This function will return a string with the appropiate format
 *
 * @param i Used to represent the 32-bits 
 *
 * @return will return a string starting with 0x, following the 8 hex digits 
 **********************************************************************************************/
 std::string hex::to_hex0x32(uint32_t i)
 {
     return std::string("0x")+to_hex32(i);

 }// End to_hex0x32 

/**
 * @brief This function will return a string with the appropiate format
 *
 * @param i Used to represent the 32-bits 
 *
 * @return will return a string starting with 0x, followed by the 5 hex digits representing the 20 bits of i.
 *************************************************************************************************************/
std::string hex::to_hex0x20(uint32_t i)
{
     return std::string("0x")+ hex::to_hex32(i);

}// End to_hex0x20

/**
 * @brief This function will return a string with the appropiate format
 *
 * @param i Used to represent the 32-bits 
 *
 * @return will return a string starting with 0x, following the 3 hex digits representing the 12 bits of i.
 *************************************************************************************************************/
std::string hex::to_hex0x12(uint32_t i)
{
    return std::string("0x") + hex::to_hex0x20(i);

}// End to_hex0x12