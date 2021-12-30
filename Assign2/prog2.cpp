//********************************************************************************************
//
//        Name: Brayan Chacha Gonzalez 
//         zID: z1861700
//  Instructor: John Winans, CSCI463 
//   Assign no. Assignment #2
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//********************************************************************************************

#include <iostream>
#include <fstream>
#include <iomanip> // Used for setfill, setw
#include <cstdint> // Standard Integer Representation Header File needed
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief Bitwise Operators & IEEE-754 Floating Point Number Decoding function 
 *
 * This function will read from the user zero or more 32 bit-hex values and print 
 * a detailed description of the floating point value based on IEEE-754.
 * 
 * IEEE-754 32 bit definition
 * 0 - 22  --> significand 
 * 23 - 30 --> exponent 
 * 31      --> sign
 *
 * @param x Obtaining what the user input, any ingeter/natural number or 32 bit-hex
 *
 * @return It will return a detailed description of the floating point value that is
 *         represented by extracting and showing the sign, exponent, and significant.
 ********************************************************************************************/
void printBinFloat(int32_t x)
{

  int32_t significand = (x & 0x007fffff);            /*! Bitmask for significand - Using the AND operator to determine the significand. 
                                                         Taking the LSB 2 bit with whatever hex value gets entered and 
                                                         using the AND operator with the corresponding significant flag bits. */


  int32_t exponent = ((x & 0x7f800000) >> 23) - 127; /*! Bitmask for Exponent - We are going to use another AND opereator 
                                                         to extract the exponent. From bit 23 to 30. */
  
  bool sign_bit = (x & 0x80000000);                  /*! Bool for sign bit - using a bool data type to determine the sign bit of the hex 
                                                         value (pos, neg), once again going to use an AND operator to extract bit 31 */

  // Left coulumn header, displaying the hex digits user entered 
  std::cout << "0x" << std::hex << x << " = ";

  // Declaring a unsign int that will hold the number 1 to the MSB 
  uint32_t one_bit = 0x80000000;

  // For loop, determining the sign 
  for(int i = 1; i <= 32; ++i)
  {
    std::cout << (x & one_bit ? '1': '0');

    // Groups of 4
    if(i % 4 == 0)
    {
      std::cout << " ";
    }

    // logical shift 
    one_bit = one_bit >> 1;

  }// End for loop for sign bit


  // Left coulumn header's 
  std::cout << std::endl;
  std::cout << "sign: "   << sign_bit           << std::endl;
  std::cout << " exp: 0x" << std::setfill ('0') << std::setw (8) << std::hex << exponent; 
  std::cout << " ("       << std::dec           << exponent      << ")"      << std::endl;
  std::cout << " sig: 0x" << std::setfill('0')  << std::setw(8)  << std::hex << significand << std::endl;



  /*! Positive (+) or Negative(-) infinity 

      Representing (+)
      Exponent  all 1's
      Signicand all 0's
      Sign bit      0

      Representing (-)
      Exponent  all 1's
      Signicand all 0's
      Sign bit      1

  */
  if (sign_bit == false)
  {

    std::cout << '+';

  }
  else 
  {

    std::cout << '-';

  } // End pos/neg loop


  if (exponent == (int32_t)0x00000080 && significand == 0x00000000)
  {

    std::cout << "inf" << std::endl;
    
  } 
  else if(exponent == (int32_t)0xffffff81 && significand == 0x00000000) 
  {

    std::cout << '0' << std::endl;

  } // End else if


  /*! Placing float value when the exponent is postive
      along with placing the binary point following the mandisa. 

      logical shift, to the significand. 9 to the left 
  */  
  else if (exponent >= 0)
  {
    int temp_digit = exponent + 23;

    std::cout << '1';

    significand = (significand << 9);

    for (int i = 0; i < temp_digit; i++)
    {
      if(i == (int)exponent)
      {

        std::cout << ".";

      }

      std::cout << (significand & 0x80000000 ? '1' : '0');

      significand = (significand << 1);

      if (i >= (int)exponent && i > 23) 
        {

         break;

        }
      }

		std::cout <<  std::endl;
    
  }// End else if 


  /*! Placing float value when the exponent is negative
      along with placing the binary point following the mandisa 
      with zeros. 

      logical shift, to the significand. 9 to the left 
  */  
  else if (exponent < 0)
  {
    std::cout << "0.";

    significand = (significand << 9);

    for (int i = -1; i > exponent; i--)
    {
      std::cout << '0';
    }

    std::cout << '1';

    for (int i = 0; i < 23; i++)
    {
      std::cout << (significand & 0x80000000 ? '1' : '0');

      significand = (significand << 1);
    }

    std::cout << std::endl;

  }// End else If 

}// End printBinFloat


/**
 * @brief This is my main function that executes the program. 
 * 
 * Main will obtain the user input in a while loop and will call another method 
 * called printBinFloat that will perform IEEE-754 Floating Point Number Operations.
 *
 * @return 0
 *
 * @bug This only works for integers/natural numbers or any hex value in a 32-bytes 
 ********************************************************************************/
int main()
{
  // Declaring unsigned 32-bit integer
  uint32_t x;

  // Obtaining user input, integers/natural #'s, hex 32-bytes  
  while(std::cin >> std::hex >> x)
  {

  // Calling printBinFloat 
  printBinFloat(x);

  }

  return 0;

}// End main