//**********************************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Profess9r John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//	File: reduce.cpp 									      
//**********************************************************************************************************
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include <sstream>
#include <iomanip>
#include <thread>
#include <vector>
#include <random>
#include <mutex>


constexpr int rows = 1000;		/// < the number of rows in the work matrix
constexpr int cols = 100;		/// < the number of cols in the work matrix

std::mutex stdout_lock ; 		/// < for serializing access to stdout
std::mutex counter_lock ; 		/// < for dynamic balancing only

volatile int counter = rows; 	/// < for dynamic balancing only
std::vector <int> tcount; 		/// < count of rows summed for each thread
std::vector <uint64_t> sum; 	/// < the calculated sum from each thread

int work[rows][cols]; 			/// < the matrix to be summed

/**
 * Message to the user, verifying the command-line. 
 *
 * This function prints an appropriate "Usage" error message and "Patter" to 
 * cerr and terminates the program if it doesn't meet the appropiate requirements. 
 *
 * @note This functions needs to read the appropiate commands from the command line 
 *       otherwise it will return a message to the user. Below is the available params.
 *          (d) Dynamic Threading
 *          (t) Number of Threads 
 * 
 *************************************************************************************************************/
static void usage()
{

    std::cerr << "     Usage: reduce [-d] [-t number_of_threas] 	     " 	<< std::endl;
    std::cerr << "            -d Dynamic Threading   					 "  << std::endl;
    std::cerr << "            -t Number of Threads 						 " 	<< std::endl;
	std::cerr << "	       Default is 2 Threads 					     " 	<< std::endl;

    exit(1);

}// End Usage()

/**
 * @brief Static Sum - This function will sum the rows of the matrix using static load balancing. 
 *
 * @param tid			This is the thread ID passed from main()
 * @param num_threads	Number of threads 
 *
 * @return Thread status (start/end) and counter along with the sum of the rows/columns in the matrix
 *************************************************************************************************************/
void sum_static(int tid, int num_threads)
{
	
	// Taking the lock to access stdout 
	stdout_lock.lock();

	// Printing threads to user 
	std::cout << "Thread " << tid << " starting" << std::endl;

	// Unlocking 
	stdout_lock.unlock();

	int static_count = tid;

	// This while loop does the work for summing the rows of the matrix
	while(static_count < rows)
	{
		for(int i = 0; i < cols; i++)

			sum[tid] += work[static_count][i];

		static_count += num_threads;

		++tcount[tid];

	}// End while loop

	// Locking once again to access stdout
	stdout_lock.lock();

	// Printing thread count and sum of matrix
	std::cout << "Thread " << tid << " ending tcount=" << tcount[tid] << " sum=" << sum[tid] << std::endl;
	
	// Unlock 
	stdout_lock.unlock();

} // End stum_static 


/**
 * @brief Dynamic Sum   This function will sum the rows of the matrix using static load balancing. 
 *
 * @param tid			This is the thread ID passed from main()
 *
 * @return Thread status (start/end) and counter along with the sum of the rows/columns in the matrix
 *************************************************************************************************************/
void sum_dynamic(int tid)
{
	// Taking the lock to access stdout 
	stdout_lock.lock();

	// Printing threads to user 
	std::cout << "Thread " << tid << " starting" << std::endl;

	// Unlocking 
	stdout_lock.unlock();

	// Flag for while loop, "is it Done?"
	bool done = false;

	// This while loop is where we will take the counter as atomic and
	// placing the lock's making it ONE operation and then doing an if loop 
	// that does the work.
	while(!done)
	{
		// A copy of the counter, having that lock. 
		int count_copy;

		// One big atomic operation
		counter_lock.lock();
		{
			if(counter > 0) 
				
				--counter;

			else done = true;

				// Taking the copy of the counter so it can be used knowing it has a lock
				count_copy = counter;

		}
		counter_lock.unlock(); // Unlocking

		// This loop will do the work of summing the rows of the matrix
		if(!done)
		{
			++tcount[tid];

			for(int i = 0; i < cols; i++)
				sum[tid] += work[count_copy][i];
		}

	} // End while(!done)

	// Locking once again to access stdout
	stdout_lock.lock();

	// Printing Thread count and sum of matrix
	std ::cout << "Thread " << tid << " ending tcount=" << tcount[tid] << " sum=" << sum[tid] << std ::endl;

	// Unlock 
	stdout_lock.unlock();

}// End sum_dynamic


/**
 * This is the main function that thrives the entire program
 *
 * This function will verify the commands the user enter and if incorrect 
 * it will call the usage function. If succeeded correctly it will simulate the contents of a 2D
 * martix in a multithreaded application that uses static or dynamic load balancing base on what 
 * the user has enter to the command-line. 
 *
 * @param argc Argument counter, the number of arguments passed to the program 
 * @param argv Argument vector, each string will be one of the arguments that will pass to the prog. 
 *
 * @return 0
 *
 * @bug This only works when the user enter enters the correct commands  
 *      Command Example to Terminal ./reduce [-d] [-t][number of threads]
 *
 ********************************************************************************************************/
int main(int argc, char **argv)
{
	srand(0x1234);

	bool dynamicSum = false;

	bool hconcurrency = false;

	// Default size, 2 threads
	int thread_limit = 2; 

	int opt;

	// Total number of rows all threads together
	uint64_t total_work = 0;

	// Total sum of threads all together
	uint64_t total_sum = 0;

	while ((opt = getopt(argc, argv, "t:d")) != -1) 
    {
		switch (opt) 
		{
			case 'd':
			{
				dynamicSum = true;

				hconcurrency = false;
			} 
			break;

			case 't':
			{	
				hconcurrency = false;

				std::istringstream iss(optarg);

                iss >> std::dec >> thread_limit;

                if(thread_limit > (int)std::thread::hardware_concurrency()) 
                	
					thread_limit = (int)std::thread::hardware_concurrency();

			} 
			break;
			
			default: /* ’?’ */
			usage(); 
		}

    }// End While(opt)

	// Creating a vector of pointers to threads
	std::vector<std::thread*> threads;

	// Making the tcount vector have one element for each thread & initalizing them to 0
	tcount.resize(thread_limit,0);
	
	// Making the sum vector have one element for each thread & initalizing them to 0
	sum.resize(thread_limit,0);

	// Printing the supported cores
    if(!hconcurrency) 
	{ 
		std::cout << std::thread::hardware_concurrency() << " concurrent threads supported."<< std::endl;
 
	}

	// Load the matrix with rand() values
	for(int i=0; i < rows ; i++)
	{
		for(int j=0; j < cols; j++)
		{
			work[i][j] = rand();
		}

	} 

	// This for loop will call the designated subroutines 
	for(int i = 0; i < thread_limit; i++)
	{
		if(dynamicSum)
		{
			// Calling sum_dynamic and passing ID
			threads.push_back(new std::thread(sum_dynamic,i));
		}
		else 
		{	// Calling sum_static and passing ID along with thread limit (2)
			threads.push_back(new std::thread(sum_static,i,thread_limit));
		}
	}

	// This for loop will go through all the elements in the vector and will do a join along with sums
	for (int i = 0; i < thread_limit; ++i)
	{
		threads.at(i)->join();

		delete threads.at(i);

		total_sum += sum.at(i);

		total_work += tcount.at(i);

	} 

	// Printing stats of total work and total sums
	std::cout << "main() exiting, total_work=" << total_work << " gross_sum=" << total_sum << std::endl;

	return 0;
}