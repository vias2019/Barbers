#pragma once
#include <unistd.h>

using namespace std;

#define PORT 8078

class Message {
public:

    enum Task {OPEN, READ, WRITE, LSEEK,CHMOD, UNLINK, RENAME};
    Task task;
    size_t count;
    off_t offset;
    int whence;
    char pathname [255];
    char oldname [255];
    char newpathname [255];
    char mode [8];
    mode_t mod;
    int sequence;
    unsigned long auth = 0;
    char errormsg[30];
    char clientId [100];
    char data [1000];


};
class Response{
    public:
    char errormsg[50];
    char buf[1000];

};