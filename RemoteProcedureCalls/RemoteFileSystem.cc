#include "RemoteFileSystem.h"
#include "Server.h"

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h> //read(), write(), lseek()

#include <ostream> // for cout
#include <iostream> // for cout
#include <random> //random


using namespace std;

RemoteFileSystem::File::File(RemoteFileSystem* filesystem, const char *pathname, char *mode){
  fs = filesystem;
  strcpy(this->pathname, pathname);
  strcpy(this->mode, mode);
}

RemoteFileSystem::~RemoteFileSystem(){
}

// RemoteFileSystem::File* RemoteFileSystem::open(const char *pathname, char *mode){
// //update message struct
// //send_to
//   strcpy(msg.pathname, pathname);
//   strcpy(msg.mode, mode);
//   msg.task = Message::Task::OPEN;
//   ssize_t s = cl.send(&msg, sizeof(msg));
//   Message reply;
//   int r = cl.receive(&reply, sizeof(reply));
//   File* f = new File(this, pathname, mode);
//   return f;
// }

RemoteFileSystem::File* RemoteFileSystem::open(const char *pathname, char *mode){
//update message struct
//send_to
//printf("Sockfd 1 = %d\n", cl.sockfd);
  File* f = new File(this, pathname, mode);
  printf("FS-open: %p\n", this);
  strcpy(msg.pathname, pathname);
  strcpy(msg.mode, mode);
  msg.task = Message::Task::OPEN;
  ssize_t s = cl.send(&msg, sizeof(msg));
  //printf("Sockfd 2 = %d\n", cl.sockfd);

 // Message reply;
  int r = cl.receive(&reply, sizeof(reply));
  //printf("Sockfd 3 = %d\n", cl.sockfd);

  return f;
}

ssize_t RemoteFileSystem::File::read(void *buf, size_t count){
  //write to msg
  printf("Sockfd Read 1 = %d\n", fs->cl.sockfd);
printf("FS -read: %p\n", fs);
  fs->msg.task = Message::Task::READ;
  fs->msg.count = count;
  strcpy(fs->msg.pathname, pathname);
printf("Sockfd Read 2 = %d\n", fs->cl.sockfd);
  //send
  ssize_t s = fs->cl.send(&this->fs->msg, sizeof(msg));
  puts ("Read() func sent to server");
  printf("Sockfd Read 3 = %d\n", fs->cl.sockfd);

  //receive
  //save received data
  int r = fs->cl.receive(&this->fs->reply, sizeof(reply));
  puts("Read() func returned from server");
  printf("RFS 72 %s", (char*) this->fs->reply.buf);
  return 0;
}

ssize_t RemoteFileSystem::File::write(void *buf, size_t count){
  //write to msg
  fs->msg.task = Message::Task::WRITE;
  fs->msg.count = count;
  strcpy(fs->msg.pathname, pathname);
  memcpy(fs->msg.data, buf, min(sizeof(fs->msg.data), sizeof(buf)));
printf("Print data %s\n", fs->msg.data);
  //send
  ssize_t s = fs->cl.send(&this->fs->msg, sizeof(msg));

  //receive
  //save received data
  int r = fs->cl.receive(&this->fs->reply, sizeof(reply));
  return 0;
}

off_t RemoteFileSystem::File::lseek(off_t offset, int whence){
  //write to msg
  fs->msg.task = Message::Task::LSEEK;
  fs->msg.offset = offset;
  fs->msg.whence = whence;
  strcpy(fs->msg.pathname, pathname);

  //send
  ssize_t s = fs->cl.send(&this->fs->msg, sizeof(msg));

  //receive
  //save received data
  int r = fs->cl.receive(&this->fs->reply, sizeof(reply));
  return 0;
}

unsigned long  token(){
  unsigned long r = 0;
  srand((int) time(0));
  for (int i=0; i<64; i++) {
    r = r*2 + rand()%2;
  }
return r;
}

RemoteFileSystem::RemoteFileSystem(char *host, unsigned long auth_token, struct timeval *timeout): cl(host, timeout->tv_sec)
{
  msg.auth = auth_token;
}

int RemoteFileSystem::chmod(const char *pathname, mode_t mod){
//write to msg
  msg.task = Message::Task::CHMOD;
  msg.mod =  mod;
  strcpy(msg.pathname, pathname);

  //send
  ssize_t s = cl.send(&msg, sizeof(msg));

  //receive
  //save received data
  int r = cl.receive(&reply, sizeof(reply));
return 0;
}

int RemoteFileSystem::unlink(const char *pathname){  //deletes
  msg.task = Message::Task::UNLINK;
  strcpy(msg.pathname, pathname);

  //send
  ssize_t s = cl.send(&msg, sizeof(msg));

  //receive
  //save received data
  int r = cl.receive(&reply, sizeof(reply));
return 0;
}

int RemoteFileSystem::rename(const char *oldpath, const char *newpath){
  msg.task = Message::Task::RENAME;
  strcpy(msg.newpathname,newpath);
  strcpy(msg.pathname, oldpath);

  //send
  ssize_t s = cl.send(&msg, sizeof(msg));

  //receive
  //save received data
  int r = cl.receive(&reply, sizeof(reply));
  strcpy(msg.pathname, newpath);
return 0;
}

