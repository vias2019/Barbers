#include "Client.h"

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h> // exit()
#include <stdio.h> //perror()
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring> //memset()
#include <sys/socket.h> //getaddrinfo
#include <netdb.h> //getaddrinfo


#define MAXLINE 1024

Client::Client(char* host, int sec){
    serverInfo(host);

    timeout.tv_sec = sec;
    timeout.tv_usec = 0;
    sockfd = createSocketFD(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0){
      perror("Client: socket creation failed");
      exit(EXIT_FAILURE);
    }
    printf("Client: host %s\n", host);
    printf("Client: createSocketFD %d\n", sockfd);

}

Client::~Client(){
    close(sockfd);
}

//Create socket file descriptor
int Client::createSocketFD(int domain, int type, int protocol)
{
    if ((sockfd = socket(domain, type, protocol)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Client: Socket %d\n", sockfd);

    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
        perror("setsockopt failed\n");

    return sockfd;
}

// void Client::serverInfo(char* host){
//     memset(&server_address, 0, sizeof(server_address));

//     server_address.sin_family = AF_INET; // IPv4
//     server_address.sin_addr.s_addr = INADDR_ANY;
//     server_address.sin_port = htons(PORT);
//     printf("%d, %d, %d",server_address.sin_family, server_address.sin_addr.s_addr, server_address.sin_port);
// }

void Client::serverInfo(char* host)
{
  memset(&server_address, 0, sizeof(server_address));
struct addrinfo hints;
  bzero(&hints, sizeof(struct addrinfo));
  hints.ai_flags = 0;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  hints.ai_addrlen = 0;
  hints.ai_addr = NULL;
  hints.ai_canonname = NULL;
  hints.ai_next = NULL;
    struct addrinfo *addresses;

  int result = getaddrinfo(host, NULL, &hints, &addresses);
  if (result != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
    exit(EXIT_FAILURE);
  }

  int n;
  struct addrinfo *p;
  for (n = 0, p = addresses; p != NULL; n++, p = p->ai_next) {
    if (p->ai_addr->sa_family != AF_INET) {
      printf("address %d: family %d  is not AF_INET(%d)", n, p->ai_addr->sa_family, AF_INET);
    } else {
     
      struct sockaddr_in *addr = (struct sockaddr_in *)p->ai_addr;
      server_address.sin_addr.s_addr = addr->sin_addr.s_addr;
      uint32_t ip = addr->sin_addr.s_addr;
      printf("address %d: %d.%d.%d.%d\n", n,
             (ip & 0xff), ((ip >> 8) & 0xff), ((ip >> 16) & 0xff), ((ip >> 24) & 0xff));
    }
  }
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_port = htons(PORT);
    printf("Client's port: %d\n", server_address.sin_port);
}

ssize_t Client::send(const void *src, size_t srcSize)
{
    ssize_t res;
    res = sendto(sockfd, src, srcSize, 0, (const struct sockaddr *) &server_address, sizeof(server_address));
    printf("Client: Hello message sent.\n");
    return res;
}

int Client::receive(void *dst, size_t dstSize)
{   
  sleep(1);
    socklen_t len;
    int n = recvfrom(sockfd, dst, dstSize,
                     MSG_WAITALL, (struct sockaddr *) &server_address,
                     &len);
    if(n == -1) {
        perror("Client receive: ");
    } else {
        printf("Received from Server : %d bytes\n", n);
    }
    return n;
}



