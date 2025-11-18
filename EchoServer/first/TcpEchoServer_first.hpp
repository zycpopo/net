#ifndef __TCP_ECHO_SERVER_HPP__
#define __TCP_ECHO_SERVER_HPP__

#include "Logger.hpp"
#include "Comm.hpp"
#include "Mutex.hpp"
#include "Thread.hpp"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>


static const int gdefaultfd=-1;
static const int gbacklog = 8;
static const int gport = 8080;

class TcpEchoServer
{
public:
  TcpEchoServer(uint16_t port = gport)
  : _listensockfd(gdefaultfd),
    _port(port)
  {}

  void Init()
  {
    //1.创建一个socket fd
    _listensockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(_listensockfd < 0 )
    {
      LOG(LogLevel::FATAL) << "create tcp socket error";
      exit(SOCKET_CREATE_ERR);
    }

    LOG(LogLevel::INFO)<<"create tcp socket success: "<<_listensockfd;

    //2.绑定socket fd
    struct sockaddr_in local;
    memset(&local,0,sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(_port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(_listensockfd, (struct sockaddr*)&local, sizeof(local))!=0)
    {
      LOG(LogLevel::FATAL)<<"bind socket error";
      exit(SOCKET_BIND_ERR);
    }
    LOG(LogLevel::INFO)<<"bind socket success: "<<_listensockfd;

    //3.设置socket监听
    if(listen(_listensockfd, gbacklog)!=0)
    {
      LOG(LogLevel::FATAL)<<"listen socket error";
      exit(SOCKET_LISTEN_ERR);
    }
    LOG(LogLevel::INFO)<<"Listen socket success: "<<_listensockfd;

  }

  void Start()
  {
    while(true)
    {
      struct sockaddr_in peer;
      socklen_t len = sizeof(peer);
      int sockfd = accept(_listensockfd,(struct sockaddr*)&peer,&len);
      if(sockfd < 0)
      {
        LOG(LogLevel::WARNING)<< "接受客户端失败";
        continue;
      }

      LOG(LogLevel::INFO) << "获取新连接成功： "<< sockfd;
    }
  }

  ~TcpEchoServer()
  {}
private:
  int _listensockfd;//监听fd
  uint16_t _port;
};




#endif