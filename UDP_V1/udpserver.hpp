#pragma once

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include "Loger.hpp"

static const int gdefaultsockfd = -1;

class UdpServer
{
  public:
    UdpServer(uint16_t port)
      :_port(port),
       _sockfd(gdefaultsockfd),
       _isrunning(false)
    {}

    void Init()
    { 
      // 创建 socket
    _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_sockfd < 0)
    {
        LOG_FORMAT(LogLevel::FATAL, "Create socket error: %s", strerror(errno));
        exit(1);  // 直接退出，不返回
    }
    LOG_FORMAT(LogLevel::INFO, "Create socket success: %d", _sockfd);

    // 绑定地址
    struct sockaddr_in local;
    bzero(&local, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(_port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(_sockfd, (struct sockaddr*)&local, sizeof(local)) < 0)
    {
        LOG_FORMAT(LogLevel::FATAL, "Bind socket error: %s", strerror(errno));
        close(_sockfd);
        exit(2);
    }
    LOG_FORMAT(LogLevel::INFO, "Bind socket success: port %d", _port);
    }

    void Start()
    {
      _isrunning=true;
      char buffer[1024];
      struct sockaddr_in peer;
      socklen_t len=sizeof(peer);

      while(_isrunning)
      {
        ssize_t n=recvfrom(_sockfd,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&peer,&len);
        if(n>0)
        {
          buffer[n]='\0';
          std::string client_ip=inet_ntoa(peer.sin_addr);
          uint16_t client_port=ntohs(peer.sin_port);

          LOG(LogLevel::DEBUG) << "[" << client_ip << ":" << client_port << "]# " << buffer;

          std::string echo = "server echo#";
          echo +=buffer;
          sendto(_sockfd,echo.c_str(),echo.size(),0,(struct sockaddr*)&peer,len);

        }
      }
    }

    void Stop(){_isrunning=false;}

    ~UdpServer()
    {
      if(_sockfd!=-1)
      {
        close(_sockfd);
        _sockfd=-1;
      }
    }
  private:
    std::string _ip;
    uint16_t _port;
    int _sockfd;

    bool _isrunning;
};
