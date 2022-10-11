//#include "RemoteFileSystem.h"

#include <sys/stat.h>
#include <fcntl.h> // open
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h> // exit()
#include <sys/socket.h>
#include <stdio.h> //perror()
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>  //memset()
#include <ostream>  //cout
#include <iostream> //cout
#include <vector>   //vector
#include <string> //string


#define MAXLINE 1024

#include "Server.h"
#include "Message.h"

Server::~Server()
{
    //upon shut down the socket free up sooner
    const int on = 1;
    setsockopt(sockfd2, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(int));
    //close(sockfd); 
}

int Server::createSocketFD(int domain, int type, int protocol)
{
    if ((sockfd2 = socket(domain, type, protocol)) < 0)
    {
        perror("Server: socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Server: Socket %d\n", sockfd2);
    return sockfd2;
}

void Server::serverInfo()
{
    memset(&server_address, 0, sizeof(server_address));
    memset(&cliaddr, 0, sizeof(cliaddr));

    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
;}

int Server::bindServer(){

    int res = bind(sockfd2, (struct sockaddr *)&server_address, sizeof(server_address));
    if (res < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    return res;
}

int Server::openFileMode(char *pathname, char *mode)
{
    int res;
    if (!strcmp(mode, "r"))
    {
        res = open(pathname, O_RDONLY);
        if (res ==-1){
            perror ("OpenFileMode");
        } else {
        cout << "Server: Success - file open in " << mode << " mode" << endl;
        }
    }
    else if (!strcmp(mode, "r+") || !strcmp(mode, "w+"))
    {
        res = open(pathname, O_RDWR);
        cout << "Server: Success - file open in " << mode << " mode" << endl;
    }
    else if (!strcmp(mode, "w"))
    {
        res = open(pathname, O_WRONLY);
        cout << "Server: Success - file open in " << mode << " mode" << endl;
    }
    return res;
}

int Server::opensv(char *pathname, char *mode, Response * res){

    int fd = openFileMode(pathname, mode);

    if (fd == -1)
    {
        char err[50] = "Error: File is not open";
        strcpy(res->errormsg, err);
        return -1;
    }
    openfd[pathname] = fd; //fd is unique number
    return fd;
}

bool Server::searchMap(char* pathname, int* fd){
    auto got = openfd.find(pathname);
    if ( got == openfd.end() )
    {
        return false;
    }
    *fd = got->second;
    return true;
}

ssize_t Server::readsv(char* pathname, size_t bufsize, Response * res){
    int fd;
    bool temp = searchMap(pathname, &fd);
    //Response* res = (Response*)buf;
    if(temp){
        ssize_t result = read(fd, res->buf, bufsize);
        if(result == -1){
            strcpy(res->errormsg, "Error: The file wasn't read");
            close(fd);
            return result;
        } else {
            return result;
        }
    } else{
        strcpy(res->errormsg, "Error: The file is not open, cannot be read");
        return -1;
    }
}

ssize_t Server::writesv(char* pathname, void * dst, size_t bufsize, Response * res){
    int fd;
    bool temp = searchMap(pathname, &fd);
    
    if(temp){
        ssize_t result = write(fd, dst, bufsize);
        if(result == -1){
            perror("Server writesv");
            strcpy(res->errormsg, "Error: The file wasn't written, check file permissions (chmod)");
            close(fd);
            return result;
        } else {
            return result;
        }
        
    } else{
        strcpy(res->errormsg, "Error: The file is not open; cannot be written");
        return -1;
    }
}

off_t Server::lseeksv(char* pathname, off_t offset, int whence,Response * res){
    int fd;
    bool temp = searchMap(pathname, &fd);
    if(temp){
        off_t res = lseek(fd, offset, whence);
        return res;
    } else{
        strcpy(res->errormsg, "Error: The file is not open; cannot be lseeked");
        return -1;
    }
}

int Server::switchTask(Message &dst, Response &res){
if (dst.auth != 0)
    {
        ssize_t r;
        off_t t;
        int temp;
        void * dt = dst.data;
        switch (dst.task)
        {
        case Message::Task::OPEN:
            opensv(dst.pathname, dst.mode, &res);
            break;
        case Message::Task::READ:
            r = readsv(dst.pathname, min(dst.count, sizeof(res.buf)), &res);
            break;
        case Message::Task::WRITE:
            r = writesv(dst.pathname, dt, dst.count, &res);
            break;
        case Message::Task::LSEEK:
            t = lseeksv(dst.pathname, dst.offset, dst.whence, &res);
            break;
        case Message::Task::CHMOD:
            temp = chmod(dst.pathname, dst.mod);
            break;
        case Message::Task::UNLINK:
            temp = unlink(dst.pathname);
            break;
        case Message::Task::RENAME:
            temp = rename(dst.pathname, dst.newpathname);
            break;
        default:
            cout << "Error: Unknown User Request" << endl;
        }
    }
    else
    {
        char error[50] = "Error: Not authorized user request";
        cout << error << endl;
        strcpy(res.errormsg, error);
        return -1;
    }
    return 1;
}

int Server::receive(Message &dst, size_t dstSize)
{
    socklen_t len = sizeof(cliaddr); //len is value/resuslt

    int n = recvfrom(sockfd2, &dst, sizeof(dst),
                     MSG_WAITALL, (struct sockaddr *)&cliaddr,
                     &len);
    printf("Received from Client : %d\n", n);
    return n;
}

ssize_t Server::send(const void *src, size_t srcSize, const struct sockaddr *cliaddr)
{
    ssize_t res;
    int len = sizeof(*cliaddr);
    res = sendto(sockfd2, src, srcSize, 0, cliaddr, (socklen_t)len);
    if(res == -1) {
        perror("Server: ");
    }
    return res;
}
//int domain, int type, int protocol, const struct sockaddr *cliaddr
void* svstart(void* arg){
    Server sv;
    int domain = AF_INET;
    int type = SOCK_DGRAM;
    int protocol = 0;

    int temp;
    temp = sv.createSocketFD(domain, type, protocol);
    sv.serverInfo();
    temp = sv.bindServer();
    while(true){
        Message msg;
        Response res;

        int r = sv.receive(msg, sizeof(msg));

        int sw = sv.switchTask(msg, res);

        ssize_t s = sv.send(&res, sizeof(res), &sv.cliaddr);
    }
}
