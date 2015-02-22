/*
 * Create the socket and initialize WTP connections.
 */
//#include "WTP_class.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <net/if.h>

#include "WTP_iterator.h"

#define PORT "3490"  // the port users will be connecting to
#define MAX_INPUT_SIZE 256
#define BACKLOG 10     // how many pending connections queue will hold

void initiate_server(int port);
void multi_io_select(int sockfd, struct sockaddr_in cli_addr);
void print_wtp_list();

void initiate_server(int port) {
	int sockfd, portno;
	struct sockaddr_in serv_addr, cli_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		perror("ERROR opening socket\n");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		perror("ERROR on binding");
	listen(sockfd, 5);
	multi_io_select(sockfd, cli_addr);
}

void multi_io_select(int sockfd, struct sockaddr_in cli_addr) {
	int fd_max, newsockfd, clilen, n;
	char buffer[MAX_INPUT_SIZE];
	/* Using select system call */
	fd_set master_fds;
	fd_set read_fds;

	/* clear the master and temp sets */
	FD_ZERO(&master_fds);
	FD_ZERO(&read_fds);

	FD_SET(sockfd, &master_fds);
	fd_max = sockfd;
	printf("Starting Select loop. fdmax = %d\n", fd_max);
	while (1) {
		read_fds = master_fds;
		if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("ERROR in Server select().");
		}

		for (int i = 0; i <= fd_max; ++i) {
			if (FD_ISSET(i, &read_fds)) {
				printf("In select loop: socket %d is ready to read.\n", i);
				if (i == sockfd) {
					clilen = sizeof(cli_addr);
					if ((newsockfd = accept(sockfd,
							(struct sockaddr *) &cli_addr,
							(unsigned int*) &clilen)) < 0) {
						perror("Error accept failed.");
					}

					FD_SET(newsockfd, &master_fds);
					if (newsockfd > fd_max)
						fd_max = newsockfd;
					printf("Connected with client socket number %d\n",
							newsockfd);
					printf("Connection from IP %d.%d.%d.%d, port %d\n",
							(ntohl(cli_addr.sin_addr.s_addr) >> 24) & 0xff, // High byte of address
							(ntohl(cli_addr.sin_addr.s_addr) >> 16) & 0xff, // . . .
							(ntohl(cli_addr.sin_addr.s_addr) >> 8) & 0xff, // . . .
							ntohl(cli_addr.sin_addr.s_addr) & 0xff, // Low byte of addr
							ntohs(cli_addr.sin_port));
					char *ipaddress;
					int portNum = ntohs(cli_addr.sin_port);
					inet_ntop( AF_INET, &cli_addr.sin_addr, ipaddress,
					INET_ADDRSTRLEN);

					/*struct ifreq ifr;
					 char *iface = "eth1";
					 ifr.ifr_addr.sa_family = AF_INET;
					 strcpy(ifr.ifr_name, iface);
					 ioctl(newsockfd, SIOCGIFHWADDR, &ifr);
					 char* macaddr;

					 int n = snprintf(macaddr, 999, "%02x%02x%02x%02x%02x%02x",
					 (unsigned char) ifr.ifr_hwaddr.sa_data[0],
					 (unsigned char) ifr.ifr_hwaddr.sa_data[1],
					 (unsigned char) ifr.ifr_hwaddr.sa_data[2],
					 (unsigned char) ifr.ifr_hwaddr.sa_data[3],
					 (unsigned char) ifr.ifr_hwaddr.sa_data[4],
					 (unsigned char) ifr.ifr_hwaddr.sa_data[5]);
					 printf("mac:%s\n", macaddr);*/

					printf("IP address:%s\n", ipaddress);
					printf("port:%d\n", portNum);

					struct WTP_connected *wtp;
					wtp = initiate_WTP(0, newsockfd, NULL,
							ipaddress, portNum, NULL);
					insertNode(wtp);
					displayList(g_WTP_head);
				} else {
					int flag = 0;
					bzero(buffer, MAX_INPUT_SIZE);
					if ((n = read(i, buffer, 255)) <= 0) {
						printf("No data on Socket : %d\n", i);
						if (n < 0) // some error has occured
							perror("Error in connection!");
						else
							perror("connection hung up!");
						//return -2;
						close(i);
						FD_CLR(i, &master_fds);
						// when client leaves, remove it from
						deleteNode(search_node_by_socket(i));
						printf("deleted node socket = %d\n", i);
						displayList(g_WTP_head);
					} else {
						printf("Client Socket number %d sent message: %s", i,
								buffer);
						if (!((buffer[0] == 'b') && (buffer[1] == 'y')
								&& (buffer[2] == 'e'))) {
							n = write(i, "OK Navneet Agrawal", 23);
							printf("Replied to Client %d\n", i);
						} else {
							n = write(i, "Goodbye Navneet Agrawal", 23);
							printf("Replied to Client %d\n", i);
							flag = -1;
							printf("Client said bye. Finishing..\n");
							close(i);
							FD_CLR(i, &master_fds);
							deleteNode(search_node_by_socket(i));
							printf("deleted node socket = %d\n", i);
							displayList(g_WTP_head);
						}
					}
				}
			}
		}
	}
}

static char *ethernet_mactoa(struct sockaddr *addr) {
	static char buff[256];

	unsigned char *ptr = (unsigned char *) addr->sa_data;

	sprintf(buff, "%02X:%02X:%02X:%02X:%02X:%02X", (ptr[0] & 0xff),
			(ptr[1] & 0xff), (ptr[2] & 0xff), (ptr[3] & 0xff), (ptr[4] & 0xff),
			(ptr[5] & 0xff));

	return (buff);

}
