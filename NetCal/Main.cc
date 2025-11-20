#include "Socket.hpp"
#include <memory>

int main()
{
  EnableConsoleLogStrategy();
  std::unique_ptr<Scket> tsock = std::make_unique<TcpSocket>();
  tscok->BuildListenSocketMethod(8080);

  while(true)
  {
    sleep(1);
  }

  return 0;

}