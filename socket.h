#ifndef CLASS_SOCKET_H
#define CLASS_SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <sstream>

#define BUFF_SIZE 1024

using namespace std;

class class_socket
{
      private:
	
      public:
	class_socket();
	
	int sock;
	struct hostent *server;
	int s_connect(string host,string port);
	int s_disconnect();
	string s_read();
	int s_write(string);
        int s_listen(int port);
};
 
#endif // CLASS_SOCKET_H

