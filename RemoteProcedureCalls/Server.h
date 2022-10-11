#include "RemoteFileSystem.h"


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
#include <vector> // vector
#include <unordered_map> //unordered_map
#include <string> //string
#define MAXLINE 1024

class Server
{
public:
    ~Server();
    //create a socket file  descriptor
    int createSocketFD(int domain, int type, int protocol);
    void serverInfo();
    int bindServer();
    int receive(Message &dst, size_t msgSize);
    ssize_t send(const void *src, size_t srcSize, const struct sockaddr *cliaddr);
    int openFileMode(char* pathname, char *mode);
    int opensv(char* pathname, char* mode, Response * res);
    ssize_t readsv(char* pathname, size_t bufsize, Response * res);
    bool searchMap(char* pathname, int* fd);
    ssize_t writesv(char* pathname, void * dst, size_t bufsize, Response * res);
    off_t lseeksv(char* pathname, off_t offset, int whence, Response * res);
    int switchTask(Message &dst, Response &res);
    struct sockaddr_in server_address;
    struct sockaddr cliaddr;
    int sockfd2;

private:
    
    int fd_sv;
    char buffer[MAXLINE];
    char *scr;
    
    unordered_map <char*, int> openfd;
    bool searchFDinArray(int fd);
};
void* svstart(void* arg);