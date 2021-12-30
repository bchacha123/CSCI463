//**********************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Professor John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//  
//  File: memory.cpp                                                Revised Final Copy 2/12/2021
//
//**********************************************************************************************
#include "memory.h" // Including Header files  
#include "hex.h"


/**
 * @brief Implementing a rounding logic.
 *
 * @param s Allocates the bytes in the mem vector.
 **********************************************************************************************/
memory::memory(uint32_t s)
{
    // Rounding the length 
    size = (s + 15)&0xfffffff0; 

    // Allocating size bytes using the mem vector and initializing the bytes to 0xa5
    mem.resize(size, 0xa5);

}// End memory(uint32_t s)



/**
* @brief Cleaning garabage from the destructor. 
**********************************************************************************************/
memory::~memory()
{
    mem.clear();

}// End ~memory()




/**
 * @brief This function will check the address in your simulated memory.
 *
 * @param i Used to check the address, if false will be passed to another function to_hex0x32().
 *
 * @return true, if the addr is not in the simulated memory.
 * @return 0, print message to user displaying a WARNING.
 ********************************************************************************************/
bool memory::check_illegal(uint32_t i) const
{
    if (i <= size)
    {
        // Returning true if the address is NOT in the simulated memory
        return true; 
    }
    else 
    {
        // Displaying warning to user and calling to_hex0x32() function.
        std::cout << "WARNING: Address out of range: " << hex::to_hex0x32(i) << std::endl;
        
        return false;
    }

}// End Check_illegal



/**
 * @brief This function will return the size of the number of bytes in the simulated memory. 
 *
 * @return The number of bytes in the simulated memory.
  ********************************************************************************************/
 uint32_t memory::get_size() const 
 {
     return size;

 }// End get_size()




 /**
 * @brief This function will veifity the address and will call check_illegal().
 *
 * @param addr Used to checking if the address in the mem is in a valid range. 
 *
 * @return true, will return the value of the bytes from the simulated memory at that given addr.
 * @return 0, if addr is not in valid range. 
  **********************************************************************************************/
uint8_t memory::get8(uint32_t addr) const
{
    // Verifying address 
    if(check_illegal(addr))
    {
        // bytes from the simulated mem @ that addr
        return mem[addr];
    }
    else 
    {
        return 0;
    }

}// End get8()



 /**
 * This function will call get8() twice, grabbing 2 bytes and doing a combination with Little Endian. 
 * Little Endian: The LSB of the data is place at the byte with the lowest addr. The rest   
 *                of the data is placed in order in the next bytes in memory. 

 * @param addr Address being used for the conversion of Little Endian.  
 *
 * @return x Will return 16-bit value based on the appropriate conversions   
  **************************************************************************************************/
uint16_t memory::get16(uint32_t addr) const
{
    // Preparing unsigned short 16-bit holder.
    uint16_t x;

    // Calling get8() twice and doing Little Endian conversion.
    x = (get8(addr) | get8(addr + 1) << 8);

    return x;

}// End get16()


 /**
 * This function will call get16() twice, grabbing 2 bytes and doing a combination with Little Endian. 
 * Little Endian: The LSB of the data is place at the byte with the lowest addr. The rest   
 *                of the data is placed in order in the next bytes in memory. 

 * @param addr Address being used for the conversion of Little Endian.  
 *
 * @return x Will return 32-bit value based on the appropriate conversions   
  **************************************************************************************************/
uint32_t memory::get32(uint32_t addr) const 
{
    // Preparing unsigned short 32-bit holder.
    uint32_t x;

    // Calling get16() twice and doing Little Endian conversion.
    x = get16(addr) | get16(addr + 2) << 16;

    return x;

}// End get32()



 /**
 * @brief This function will call get8() to return a sign-extended value using a cast.
 *
 * @param addr the address returning a sign extended value. 
 *
 * @return x, Will return the sign extention value baesed on a 32-bit sign int.
  **********************************************************************************************/
int32_t memory::get8_sx(uint32_t addr) const
{
    // Preparing unsigned short 32-bit holder.
    uint32_t x;

    // Calling get8()
    x = get8(addr);

    return x;

}// End get8_sx()



 /**
 * @brief This function will call get16() to return a sign-extended value using a cast.
 *
 * @param addr the address returning a sign extended value. 
 *
 * @return x, Will return the sign extention value of the 16-bit value as a 32-bit sign int.
  **********************************************************************************************/
int32_t memory::get16_sx(uint32_t addr) const
{
    // Preparing unsigned short 32-bit holder.
    uint32_t x;

    // Calling get16()
    x = get16(addr);

    return x;

}// End get16_sx()



 /**
 * @brief This function will call get32() to return a sign-extended value using a cast.
 *
 * @param addr the address returning a sign extended value. 
 *
 * @return x, Will return the value as a 32-bit signed int.
  **********************************************************************************************/
int32_t memory::get32_sx(uint32_t addr) const
{
    // Preparing unsigned short 32-bit holder.
    uint32_t x;

    // Calling get32()
    x = get32(addr);

    return x; 

}// End get32_sx()


 /**
 * @brief This function will call check_illegal() to verify the arguments of the address.
 *
 * @param addr Address being passed to verify if the addr is valid. 
 ************************************************************************************************/
void memory::set8(uint32_t addr, uint8_t val)
{
    if(check_illegal(addr))
    {
        // Assuming the addr is valid, setting the bytes to the simulated mem at
        // the address to the given val.
        mem[addr] = val;
    }

}// End set8()



 /**
 * @brief This function will call set8() twice to store the given val in Little-Endian format. 
 *
 * @param addr Address given to simulate the memory.  
 ************************************************************************************************/
void memory::set16(uint32_t addr, uint16_t val)
{
    // Calling set8() function
    set8(addr + 1, val >> 8);

    // Calling set8 function once again to set it to Little Endian format.
    set8(addr, val);

    // Setting the bytes to the simulated mem @ that address to the given val.
    mem[addr] = val;

}// End set16()


/**
 * @brief This function will call set16() twice to store the given val in Little-Endian format. 
 *
 * @param addr Address given to simulate the memory.  
 ************************************************************************************************/
void memory::set32(uint32_t addr, uint32_t val)
{
    // Calling set16 funtionn
    set16(addr + 1, val >> 8);

    // Calling set16 function once again to set the Little Endian conversition.
    set16(addr + 2, val >> 16);

    // Setting the bytes to the simulated mem @ that address to the given val.
    mem[addr] = val;

}// End set32()


/**
 * @brief This is the dump function simulating the memory
 *
 * The dump will display the entire content of the simulated memory in hex with the corresponding ASCII.
 * It will be displayed in a specific format displacing the appropiate spaces and asterisk. 
 **********************************************************************************************************/
void memory::dump() const
{
    // Declaring an array of 17 elements
    char ascii[17];

    // Setting 16 of those elements
    ascii[16] = 0;

    // Loop that will print the dump with the according asterisks and semicolons.
    for(uint32_t i=0; i < size; i++)
    {
        // Format of the dump()
        if(i%16 == 0)
        {
            if(i != 0)
            std::cout << " *" << ascii << "*" << std::endl;
            std::cout << hex::to_hex32(i) << ":";
        }

        // Formatting dump lines
        uint8_t ch = get8(i);
        std::cout << (i%16 == 8?"  ":" ") << hex::to_hex8(ch);
        ascii[i%16] = isprint(ch)?ch:'.';

    }// End loop

    std::cout << " *" << ascii << "*" << std::endl;

}// End dump()


/**
 * @brief This function will read a file in binary mode and read its content in the simulated memory.
 * @param fname This param is used open the file in binary mode
 *
 * @return false, if the file can't be found.
 * @return false, if the address is not valid 
 * @return true,  if file loads accordingly 
 **/
bool memory::load_file(const std::string &fname)
{

    char x;
    //uint8_t i;
    int cnt = 0; 

    // Opening the file 
    std::ifstream infile(fname, std::ios::in|std::ios::binary);

    // If the file cannot be opened, printing message to User. 
    if (!infile)
    {
        std::cout << "Can't open file '" << fname << "' for reading!" << std::endl;

        // Return false if cant open file
        return false;

    }// End reading error loop

    // Reading file one-byte at time 
    while(infile.get(x))
    {
      if(check_illegal(cnt))
      {
        mem[cnt] = x;
      }

      // To big of a file 
      else
      {
        // Diplaying message to User 
        std::cerr << "Program too big!" << std::endl;

        return false;
      }

      cnt++;
    }

    infile >> std::noskipws;

    return true;

}// End load_file
