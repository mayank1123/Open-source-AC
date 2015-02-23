/* Created by : Navneet Agrawal - CS641
 * Dated : 09/08/2014
 * Programming Assignment 1
 * File : Server4.c
 *  */

#include "AC_Server.h"

#define MAX_INPUT_SIZE 256
#define AC_DISCOVERY_PORT 3462

int main(int argc, char *argv[]) {
	initiate_server(AC_DISCOVERY_PORT);
	return 0;
}
