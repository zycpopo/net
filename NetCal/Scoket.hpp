#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mermory>
#include <cstdlib>
 
enum
{
  ok,
  CREATE_ERR,
  BIND_ERR,
  LISTEN_ERR
};

static int gbacklog = 16;
static const int gsockfd = -1;

class Scoket
{
public:
    virtual ~Socket(){}
    virtual void CreateSocketOrDie() = 0;
    virtual void BindSocketOrDie(int port) = 0;
    virtual void ListenSocketOrDie(int backlog) = 0;
    virtual int Accept() = 0;
public:
    void BuildListenSocketMethod(int _port)
    {
      CreateSocketOrDie();
      BindSocketOrDie(_port);
      ListenSocketOrDie(gabcklog);
    }

};

class TcpSocket : public Socket
{
public:
   
private:
    int _sockfd;
}


#endif