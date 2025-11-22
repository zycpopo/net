#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <memory>
#include <cstdlib>

#include "InetAddr.hpp"
#include "Logger.hpp"
 
enum
{
  ok,
  CREATE_ERR,
  BIND_ERR,
  LISTEN_ERR
};

static int gbacklog = 16;
static const int gsockfd = -1;

class Socket
{
public:
    virtual ~Socket(){}
    virtual void CreateSocketOrDie() = 0;
    virtual void BindSocketOrDie(int port) = 0;
    virtual void ListenSocketOrDie(int backlog) = 0;
    virtual std::shared_ptr<Socket> Accept(InetAddr *clientaddr) = 0;
    virtual int Sockfd() = 0;
    virtual void Close() = 0;
    virtual ssize_t Recv(std::string *out) = 0;
    virtual ssize_t Send(const std::string &in) = 0;
    virtual bool Connect(InetAddr &peer) = 0;

public:
    void BuildListenSocketMethod(int _port)
    {
      CreateSocketOrDie();
      BindSocketOrDie(_port);
      ListenSocketOrDie(gbacklog);
    }
    void BuildClientSockerMethod()
    {
      CreateSocketOrDie();
    }

};

class TcpSocket : public Socket
{
public:
    TcpSocket(): _sockfd(gsockfd)
    {}
    TcpSocket(int sockfd):_sockfd(sockfd)
    {}

    void CreateSocketOrDie() override
    {
        _sockfd = socket(AF_INET,SOCK_STREAM,0);
        if(_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket error";
        }
        LOG(LogLevel::INFO) << "create socket success!";
    }

    void BindSocketOrDie(int port) override
    {
        InetAddr local(port);
        if (bind(_sockfd, local.Addr(), local.Length()) != 0)
        {
            LOG(LogLevel::FATAL) << "bind socket error!";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind socket success!";
    }

    void ListenSocketOrDie(int backlog) override
    {
        if(listen(_sockfd, backlog) != 0)
        {
            LOG(LogLevel::FATAL) << "listen socket error!";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::INFO) << "listen socket success!";
    }

    std::shared_ptr<Socket> Accept(InetAddr *clientaddr) override
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);

        int fd = accept(_sockfd, (struct sockaddr*)&peer, &len);
        if(fd < 0)
        {
            LOG(LogLevel::WARNING) << "accept socket error!";
            return nullptr;
        }
        LOG(LogLevel::INFO) << "accept socket success!";
        clientaddr->Init(peer);
        return std::make_shared<TcpSocket>(fd);
    }
    int Sockfd() override
    {
        return _sockfd;
    }
    void Close() override
    {
        if(_sockfd >= 0)
            close(_sockfd);
    }

    ssize_t Recv(std::string *out) override
    {
        char buffer[1024];
        ssize_t n = recv(_sockfd,buffer,sizeof(buffer)-1,0);
        if(n > 0)
        {
          buffer[n]=0;
          *out +=buffer;
        }
        return n;
    }

    ssize_t Send(const std::string &in)
    {
      return send(_sockfd,in.c_str(),in.size(),0);
    }

    bool Connect(InetAddr &peer) override
    {
      int n = connect(_sockfd,peer.Addr(),peer.Length());
      if(n >= 0)
      {
        return true;
      }
      else 
        return false;
    }

    ~TcpSocket(){}
private:
    int _sockfd;
};


#endif