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
#define MAX_INPUT_SIZE 256

int start_client(char* server_ip, int server_port);

int start_client(char* server_ip, int server_port) {
	int sockfd, portnum, n;
	struct sockaddr_in server_addr;

	char inputbuf[MAX_INPUT_SIZE];

	/* Create client socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "ERROR opening socket\n");
		return 1;
	}

	/* Fill in server address */
	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	if (!inet_aton(server_ip, &server_addr.sin_addr)) {
		fprintf(stderr, "ERROR invalid server IP address\n");
		return 1;
	}
	server_addr.sin_port = htons(server_port);

	/* Connect to server */
	if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))
			< 0) {
		perror("ERROR connecting\n");
		return 1;
	}
	printf("Connected to server\n");

	do {
		/* Ask user for message to send to server */
		printf("Please enter the message to the server: ");
		bzero(inputbuf, MAX_INPUT_SIZE);
		fgets(inputbuf, MAX_INPUT_SIZE - 1, stdin);

		/* Write to server */
		n = write(sockfd, inputbuf, strlen(inputbuf));
		if (n < 0) {
			fprintf(stderr, "ERROR writing to socket\n");
			return 1;
		}

		/* Read reply */
		bzero(inputbuf, MAX_INPUT_SIZE);
		n = read(sockfd, inputbuf, (MAX_INPUT_SIZE - 1));
		if (n < 0) {
			fprintf(stderr, "ERROR reading from socket\n");
			return 1;
		}
		printf("Server replied: %s\n", inputbuf);

		/* Check that reply is either OK or Goodbye */
		if (!((inputbuf[0] == 'G' && inputbuf[1] == 'o' && inputbuf[2] == 'o'
				&& inputbuf[3] == 'd' && inputbuf[4] == 'b'
				&& inputbuf[5] == 'y' && inputbuf[6] == 'e')
				|| (inputbuf[0] == 'O' && inputbuf[1] == 'K'))) {
			fprintf(stderr, "ERROR wrong reply from server\n");
			return 1;
		}

	} while (!(inputbuf[0] == 'G' && inputbuf[1] == 'o' && inputbuf[2] == 'o'
			&& inputbuf[3] == 'd' && inputbuf[4] == 'b' && inputbuf[5] == 'y'
			&& inputbuf[6] == 'e'));

	return 0;
}
