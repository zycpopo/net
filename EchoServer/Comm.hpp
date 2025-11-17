#ifndef __COMM_HPP__
#define __COMM_HPP__

#include <iostream>

enum
{
  OK,
  SOCKET_CREATE_ERR,
  SOCKET_BIND_ERR,
  SOCKET_LISTEN_ERR,
  SOCKET_CONNECT_ERR,
  FORK_ERR
};

#endif