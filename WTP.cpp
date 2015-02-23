/*
 * WTP client side daemon
 *
 * created by Navneet Agrawal
 * Dated: Feb 22 2015
 *
 */

#include "WTP_client.h"
#define AC_SERVER_PORT 3462

int main(int argc, char *argv[]){
	const char* server_ip = "127.0.0.1";
	int server_port = AC_SERVER_PORT;
	start_client((char*) server_ip, server_port);
}
