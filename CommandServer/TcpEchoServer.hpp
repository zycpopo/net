#ifndef __TCP_ECHO_SERVER_HPP__
#define __TCP_ECHO_SERVER_HPP__

#include "Logger.hpp"
#include "Comm.hpp"
#include "Mutex.hpp"
#include "Thread.hpp"
#include "InetAddr.hpp"
#include "Thread.hpp"
#include "ThreadPool.hpp"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <string>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>

using task_t = std::function<void()>;

static const int gdefaultfd=-1;
static const int gbacklog = 8;
static const int gport = 8080;

class TcpEchoServer
{
private:
  void HandlerIO(int sockfd,InetAddr client)
  {
    char buffer[1024];
    while(true)
    {
      buffer[0]=0;
      ssize_t n=read(sockfd,buffer,sizeof(buffer)-1);
      if(n>0)
      {
        buffer[n]=0;
        std::string echo_string = "server echo# ";
        echo_string +=buffer;
        LOG(LogLevel::DEBUG)<<client.ToString() <<" say: "<< buffer;
        write(sockfd,echo_string.c_str(),echo_string.size());
      }
      else if(n==0)
      {
        LOG(LogLevel::INFO)<<"client "
                           << client.ToString()<<" quit,me too,close fd: "<<sockfd;
        break;
      }
      else
      {
        LOG(LogLevel::WARNING) << "read client "
                               << client.ToString() << " error, sockfd : " << sockfd;
        break;
      }
    }

    close(sockfd);
  }
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
    InetAddr local(_port);
    if(bind(_listensockfd, local.Addr(), local.Length())!=0)
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

  //借助同一权限区域的类的对象指针解决静态成员访问不了类内部函数
  //(因为静态成员没有this指针，访问不了对象的成员)
  class ThreadData
  {
    public:
        ThreadData(int sockfd,TcpEchoServer *self,const InetAddr &addr)
          :_sockfd(sockfd),_self(self),_addr(addr)
        {}
    public:
        int _sockfd;
        TcpEchoServer *_self;
        InetAddr _addr;
  };

  static void *Routine(void *args)
  {
      ThreadData *td=static_cast<ThreadData*>(args);
      pthread_detach(pthread_self());
      td->_self->HandlerIO(td->_sockfd,td->_addr);

      delete td;
      return nullptr;
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
      InetAddr clientaddr(peer);

      LOG(LogLevel::INFO) << "获取新连接成功： "<< sockfd
          << " client addr: "<<clientaddr.ToString();

      //version-1 单线程(只允许一个客户端通信)仅测试
      //HandlerIO(sockfd,clientaddr);

      //version-2 多进程
      // pid_t id=fork();
      // if(id<0)
      // {
      //   LOG(LogLevel::FATAL)<<"资源不足,创建子进程失败";
      //   exit(FORK_ERR);
      // }
      // else if(id==0)
      // {
      //   //子进程
      //   close(_listensockfd);//关闭子进程_listendockfd,防止误改变，这样子关闭不影响父进程的——listensockfd
      //   if(fork()>0)
      //       exit(OK);
        
      //   //孙子进程
      //   HandlerIO(sockfd , clientaddr);
      //   exit(OK);
      // }
      // else
      // {
      //   close(sockfd);//父进程用不上，还防止误用；关闭无用fd，规避fd泄露
      //   //父进程
      //   pid_t rid = waitpid(id,nullptr,0);
      //   (void)rid;
      // }

      //version-3 多线程
      // pthread_t tid;
      // ThreadData *td=new ThreadData(sockfd,this,clientaddr);  
      // pthread_create(&tid,nullptr,Routine,(void*)td);

      //version-4 线程池
      ThreadPool<task_t>::GetInstance()->Enqueue([this, sockfd, clientaddr](){
                this->HandlerIO(sockfd, clientaddr);
            });

    }
  }

  ~TcpEchoServer()
  {}
private:
  int _listensockfd;//监听fd
  uint16_t _port;
};




#endif