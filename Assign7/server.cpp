//**********************************************************************************************************
//
//  Brayan Chacha Gonzalez
//  z1861700
//  CSCI 463, Profess9r John Winans 
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//	File: server.cpp									      
//**********************************************************************************************************
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * Message to the user, verifying the command-line. 
 *
 * This function prints an appropriate "Usage" error message and "Patter" to 
 * cerr and terminates the program if it doesn't meet the appropiate requirements. 
 *
 * @note This functions needs to read the appropiate commands from the command line 
 *       otherwise it will return a message to the user. Below is the available params.
 *          (l) The port number to which the server must listen
 * 				Default is 0
 * 
 *************************************************************************************************************/
static void usage()
{

    std::cerr << "  Usage: [-l server-port]                          " << std::endl;
    std::cerr << "          -l, Specify the server’s port  number    " << std::endl;
    exit(1);
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
static int safe_write(int fd,const char *buf, size_t buf_len)
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

    return buf_len;
}

/**
 * @brief checkSum - This function will print the sumcheck of the file 
 *
 * @param buf        buffer
 *
 * @return sum
 **********************************************************************************************/
static int checkSum(char *buf)
{
	uint16_t sum = 0;

    for (uint16_t i = 0; buf[i] != '0'; i++) 
    {
        sum += buf[i];
    }

    std::cout << "Sum: "<< sum << std::endl;

    return sum;
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
int main(int argc,char **argv)
{   
	int rval;

    int sock;

    int useropt;

    int msgsock;

	char buf[1024];

    int sockValue = 0;

	socklen_t fromlength;

	struct sockaddr_in server;

    // Setting the signal number to the sig pipe 
	signal(SIGPIPE, SIG_IGN);

	while ((useropt = getopt(argc, argv, "l:")) != -1) 
    {
		switch (useropt) 
		{
			case 'l':
            {
				sockValue = atoi(argv[2]);

				break;
			}

			default:
            {
				usage();
				break;
            }
		}
    }

    std::cout << "Socket has port #" << sockValue << std::endl;

    server.sin_family = AF_INET;

	server.sin_port = htons(sockValue);

    server.sin_addr.s_addr = INADDR_ANY;

    // Creating socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("opening stream socket");

		exit(1);
	}

    // Bind the socket to the address and port number specified in sockaddr
	if(bind(sock, (sockaddr*)&server, sizeof(server)))
    {
		perror("binding stream socket");

		exit(1);
	}

    // Set up the connection 
	listen(sock,5);

	do
    {
		fromlength = sizeof(server);

        // Extract the first connection on the queue
		msgsock = accept(sock, (struct sockaddr*)&server,&fromlength);

		if (msgsock == -1)
        {
			perror("accept");
		}
        else
        {
            // Converion of IPv4/IPv6 into a string
			inet_ntop(server.sin_family, &server.sin_addr, buf, sizeof(buf));

			printf("Accepted connection from '%s', port %d\n", buf, ntohs(server.sin_port));
			
			do
            {
				bzero(buf,sizeof(buf));

                // Reading from message socket
				if((rval = read(msgsock,buf,sizeof(buf)-1)) < 0)

					perror("reading stream message");

				if(rval == 0)

					printf("Ending connection\n");

				else
				{
					buf[rval] = '\0';

					safe_write(rval, buf, checkSum(buf));
				}

			} while(rval != 0);

            // Closing socket
			close(msgsock);
		}

	} while(true);
}