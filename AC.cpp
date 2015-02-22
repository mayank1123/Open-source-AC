/* Created by : Navneet Agrawal - CS641
 * Dated : 09/08/2014
 * Programming Assignment 1
 * File : Server4.c
 *  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "AC_Server.h"

#define MAX_INPUT_SIZE 256
#define AC_DISCOVERY_PORT 3462

int main(int argc, char *argv[]) {
	initiate_server(AC_DISCOVERY_PORT);
	return 0;
}
