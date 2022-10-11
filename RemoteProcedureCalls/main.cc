
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

//#include <sys/random.h> // getrandom
#include <stdlib.h> // exit()
#include <sys/socket.h>
#include <stdio.h> //perror()
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring> //memset()
#include <dirent.h> //dirfd()
#include <fcntl.h> //open()
#include <pthread.h> //threads
#include <unistd.h> // sleep()

#include "RemoteFileSystem.h"
#include "Server.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    char host [] = "localhost";
    pthread_t clthread, svthread;
    int sv2;
    sv2 = pthread_create(&svthread, NULL, svstart, NULL);

    unsigned long auth = token();
    struct timeval timeout;      
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    RemoteFileSystem filesystem(host, auth, &timeout);
    sleep(1); //sleep to allow server to start
    char mode [] = "r+";
    RemoteFileSystem::File* f = filesystem.open("text.txt", mode);
    char buf[1000];

    f->read(buf, 10);
    mode_t mod = 0666;
    filesystem.chmod("text.txt", mod);

    f->lseek(0, SEEK_END);
    char * wr  = (char*)"Return!";
    f->write(wr, strlen(wr));

    char* newname = (char*)"vias.txt";
    //filesystem.rename(filesystem.msg.pathname, newname);
    //filesystem.unlink(filesystem.msg.pathname);

    puts("The End!");
    return 0;
}