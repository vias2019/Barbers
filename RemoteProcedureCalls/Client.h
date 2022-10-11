
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h> // exit()
#include <sys/socket.h>
#include <stdio.h> //perror()
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring> //memset()
#include "Message.h"

#define MAXLINE 1024

class Client{
public:
    Client(char* host, int sec);
    ~Client();
    int createSocketFD(int domain, int type, int protocol);
    void serverInfo(char* host);
    int receive(void *dst, size_t dstSize);
    ssize_t send(const void *src, size_t srcSize);
    struct sockaddr_in server_address;
    char buffer[MAXLINE];
    int sockfd;
    struct timeval timeout;


private:
    

    
};
