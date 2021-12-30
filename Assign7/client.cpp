//**********************************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Profess9r John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//	File: client.cpp									      
//**********************************************************************************************************
#include <string>
#include <netdb.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <limits>

/**
 * Message to the user, verifying the command-line. 
 *
 * This function prints an appropriate "Usage" error message and "Patter" to 
 * cerr and terminates the program if it doesn't meet the appropiate requirements. 
 *
 * @note This functions needs to read the appropiate commands from the command line 
 *       otherwise it will return a message to the user. Below is the available params.
 *          (s) Specifying the server's IPv4 address.
 *         #### Server Port Number to which the client must connect. 
 * 				Default is 127.0.0.1
 * 
 *************************************************************************************************************/
static void usage()
{
    std::cerr << " Usage: [-s server-ip] server-port                                   " << std::endl;
    std::cerr << "         -s, Specify the server’s IPv4 number in dotted-quad format. " << std::endl;
    std::cerr << "             The server port number 								   " << std::endl;
    std::cerr << "             Default: 127.0.0.1                                      " << std::endl;
	exit(1);
}

/**
 * @brief load_file - This function will read input from cin and send it to the server 
 *
 * @param tid 		  Buffer
 *
 * @return true, assuming it read the input file 
 *******************************************************************************************/
bool load_file(char *buf)
{
	char load;

	int i = 0;

	while (std::cin.get(load))
	{
	   buf[i] = load;

	   i++;

	} return true;
}

/**
 * @brief safe_write - This function will loop to make sure that all the data was written 
 * 					   and nothing was left behind.
 *
 * @param fd      file descriptor 
 * @param buf     buffer
 * @param buf_len buffer length 
 *
 * @return buf_len
 **********************************************************************************************/
static ssize_t safe_write(int fd, char *buf, size_t buf_len)
{
	while (buf_len > 0)
    {
        ssize_t wlen = write(fileno(stdout), buf, buf_len);

        if(wlen == -1) 
			return -1; 

		// Reducing the remainding number of bytes send.
        buf_len -= wlen;

		// Moving the buffer pointer.
        buf += wlen;
    }

	// All bytes have been sent.
    return buf_len;
}

/**
 * @brief print_response - This function will read the given socket fd and print it until EOF
 *
 * @param fd      file descriptor 
 *
 * @return 0
 **********************************************************************************************/
static int print_response(int fd)
{
 	char buf[1024];

 	int rval = 1;

	// This while loop will propagate until it gets to EOF
 	while(rval>0)
	 {

		 if((rval = read(fileno(stdin), buf, sizeof(buf)-1)) < 0)
		 {
 			perror("reading stream message");

 			return -1;
 		 }
 		 else if(rval > 0)
		 {
			//EOF
 			buf[rval] = '\0';
			
 		 }

 	}
 	return 0;
 }

/**
 * This is the main function that thrives the entire program
 *
 * This function will verify the commands the user enter and if incorrect 
 * it will call the usage function. 
 *
 * @param argc Argument counter, the number of arguments passed to the program 
 * @param argv Argument vector, each string will be one of the arguments that will pass to the prog. 
 *
 * @return 0
 *
 * @bug This only works when the user enter enters the correct commands       
 *
 ********************************************************************************************************/
int main(int argc,char *argv[])
{
	int opt;

	int sock;

	char buf[1048];

	load_file(buf);

	struct sockaddr_in server;

	// Default IPv4 
	const char *ip_addr = "127.0.0.1";

	while ((opt = getopt(argc, argv, "s:")) != -1) 
    {
		switch (opt) 
		{
			case 's':
			{
				ip_addr = argv[2];
				break;
			}
			default:
			{
				usage();
				break;
			}
		}
    }

	// Creating socket
	sock = socket(AF_INET, SOCK_STREAM,0);
	if(sock < 0)
	{
		perror("opening stream socket");

		exit(1);
	}

	// Connect socket using specified by the command line 
	server.sin_family = AF_INET;

	std::cout << "Connection from " << ip_addr << std::endl;

	if(inet_pton(AF_INET, ip_addr, &server.sin_addr) <= 0)
	{
		fprintf(stderr, "%s : invalied address format\n", ip_addr);

		// Calling usage(), incorrect address format
		usage();

		exit(2); 
	}

	if(argc < 3)
	{
		server.sin_port = htons(atoi(argv[1]));
	}
	else
	{
		server.sin_port = htons(atoi(argv[3]));
	}

	// Calling connect, connecting the fd by the socket to the server
	if(connect(sock,(sockaddr*)&server, sizeof(server)) < 0)
	{
	    perror("Connecting Stream Socket");

	    exit(1);

	}
	std::cout << argv[2] << std::endl;

	// Passing the buf to safe_write for partial data
	if (safe_write(sock, buf, sizeof(buf)) < 0)
	{
	    perror("Writing on stream socket");

		// Letting the server know that the request phase of the transaction has complited 
	    shutdown(sock, SHUT_WR);

	    print_response(sock);

	    close(sock);
	}
	
	return 0;
}
