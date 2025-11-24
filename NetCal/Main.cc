#include "Socket.hpp"
#include "TcpServer.hpp"
#include <memory>

void Usage(std::string proc)
{
  std::cerr << "Usage: " << proc << " localport" << std::endl;
}

std::string Test(std::string &inbuffer)
{

}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    Usage(argv[0]);
    exit(0);
  }
  uint16_t serverport = std::stoi(argv[1]);
  EnableConsoleLogStrategy();
  std::unique_ptr<TcpServer> tsock = std::make_unique<TcpServer>(serverport,Test);

  tsock->Run();
  return 0;
}