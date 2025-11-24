#pragma once

#include <memory>
#include <unistd.h>
#include <signal.h>
#include <functional>

#include "InetAddr.hpp"
#include "Socket.hpp"

using callback_t = std::function<std::string(std::string&)>;//规定服务器执行的任务类型

class TcpServer
{
public:
    TcpServer(int port,callback_t cb)
    : _port(port), _cb(cb),
      _listensocket(std::make_unique<TcpSocket>())
    {
      _listensocket->BuildListenSocketMethod(_port);
    }

    void HandlerRequest(std::shared_ptr<Socket> sockfd,InetAddr addr)
    {
      std::string inbuffer;
      while(true)
      {
        ssize_t n = sockfd->Recv(&inbuffer);
        if(n > 0)
        {
          LOG(LogLevel::DEBUG) << addr.ToString()<<"# "<<inbuffer;
          std::string send_str = _cb(inbuffer); 
          if(send_str.empty())
                    continue;
          sockfd->Send(inbuffer);
        }
        else if(n == 0)
        {
          LOG(LogLevel::DEBUG) << addr.ToString() << "quit, me too!";
          break;
        }
        else 
        {
          LOG(LogLevel::DEBUG) << addr.ToString() << "read error,quit!";
          break;
        }
      }
      sockfd->Close();
    }

    void Run()
    {
      signal(SIGCHLD,SIG_IGN);
      while(true)
      {
        InetAddr addr;
        auto sockfd = _listensocket->Accept(&addr);
        if(sockfd == nullptr)
            continue;
        LOG(LogLevel::DEBUG) << "获取一个新连接：" << addr.ToString()
            <<",sockfd: " <<sockfd->Sockfd();
        if(fork()==0)
        {
          _listensocket->Close();
          HandlerRequest(sockfd,addr);
          exit(0);
        }  
        sockfd->Close();
      }
    }

    ~TcpServer()
    {}
private:
    int _port;
    std::unique_ptr<Socket> _listensocket;
    callback_t _cb;
};