/*
 * AC.c
 *
 * This is a WLAN controller which will be a server
 * for WTP clients. The controller will create a linux
 * socket secured by OpenSSL. Multiple clients are
 * handled by multiple processes. Inter-process
 * communication will make sure that all clients can
 * communicate with AC main daemon.
 *
 *  Created on: Feb 16, 2015
 *      Author: navneet
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    if (argc > 1 && *(argv[1]) == '-') {
        usage(); exit(1);
    }

    int listenPort = 1234;
    if (argc > 1)
        listenPort = atoi(argv[1]);


    // Create a socket
    int s0 = socket(AF_INET, SOCK_STREAM, 0);
    if (s0 < 0) {
        perror("Cannot create a socket"); exit(1);
    }

    // Fill in the address structure containing self address
    struct sockaddr_in myaddr;
    memset(&myaddr, 0, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(listenPort);        // Port to listen
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind a socket to the address
    int res = bind(s0, (struct sockaddr*) &myaddr, sizeof(myaddr));
    if (res < 0) {
        perror("Cannot bind a socket"); exit(1);
    }

    // Set the "LINGER" timeout to zero, to close the listen socket
    // immediately at program termination.
    struct linger linger_opt = { 1, 0 }; // Linger active, timeout 0
    setsockopt(s0, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(linger_opt));

    // Now, listen for a connection
    res = listen(s0, 10);    // "1" is the maximal length of the queue
    if (res < 0) {
        perror("Cannot listen"); exit(1);
    }

//Gaurang... starts here...
    fd_set master_fds; // For select loop
    fd_set read_fds;
    int fdmax;
    int s1;
    int i,j;
    char buffer[1024];
    /* add the listener to the master_fds set */
    FD_SET(s0, &master_fds);

    /* keep track of the biggest file descriptor */
    fdmax = s0; /* so far, it's this one*/

    // Accept a connection (the "accept" command waits for a connection with
    // no timeout limit...)
    struct sockaddr_in peeraddr;
    socklen_t peeraddr_len;

   for (;;)
   {
    	read_fds = master_fds;
	if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
	{
		perror("select() error");
	}

		for (i=0; i<=fdmax; i++)
		{
			if (FD_ISSET (i, &read_fds))
				if (i==s0) //Got a new incomming connection
				{
						s1 = accept(s0, (struct sockaddr*) &peeraddr, &peeraddr_len);
					    	if (s1 < 0) {
							perror("Cannot accept"); exit(1);
					    	}
					    	// A connection is accepted. The new socket "s1" is created
					    	// for data input/output. The peeraddr structure is filled in with
					    	// the address of connected entity, print it.
					    	printf(
							"Connection from IP %d.%d.%d.%d, port %d\n",
							(ntohl(peeraddr.sin_addr.s_addr) >> 24) & 0xff, // High byte of address
							(ntohl(peeraddr.sin_addr.s_addr) >> 16) & 0xff, // . . .
							(ntohl(peeraddr.sin_addr.s_addr) >> 8) & 0xff,  // . . .
							ntohl(peeraddr.sin_addr.s_addr) & 0xff,         // Low byte of addr
							ntohs(peeraddr.sin_port)
					    	);
					    	write(s1, "Hello!\r\n", 8);
						FD_SET(s1, &master_fds); /* add to master_fds set */
		    				if(s1 > fdmax)
		    				{ /* keep track of the maximum */
							fdmax = s1;
		    				}
				}

				else //Data from existing Clinet
				{
	    				if((res = read(s1, buffer, 1024)) <= 0)
	    				{
	    					/* got error or connection closed by client */
	    					if(res == 0)
						{
	     						/* connection closed */
	     						printf("Socket %d hung up\n", i);
	    					}
						else
	    						perror("recv() error lol!");
	    					/* close it... */
	    					close(i);
	    					/* remove from master set */
	    					FD_CLR(i, &master_fds);
	    				}
	    				else
	    				{
						/* we got some data from a client*/
	    					for(j = 0; j <= fdmax; j++)
	    					{
	    						/* send to everyone! */
	    						if(FD_ISSET(j, &master_fds))
	    						{
		   						/* except the listener and ourselves */
		   						//if(j != listener && j != i)
		   						if(j!= s0 && j==i)
								{
									       	buffer[res] = 0;
									    	//uint32_t display;
									    	//memcpy(&display, buffer, res);
									    	//display = ntohl(display);
									    	//printf("Received %d bytes:\n%d", res, display);
									    	printf("Received %d bytes: %s\n", res, buffer);
								}


							}
						}
					}
				}
		}
  }

    	return 0;

}
