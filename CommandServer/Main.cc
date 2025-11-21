#include "CommandServer.hpp"
#include "Command.hpp"
#include <memory>

void Usage(std::string proc)
{
  std::cerr << "Usage: " << proc << "localport" << std::endl;
}

int main(int argc, char *argv[])
{
  // uint16_t serverport = 8080;

  if (argc != 2)
  {
    Usage(argv[0]);
    exit(0);
  }
  uint16_t serverport = std::stoi(argv[1]);

  EnableConsoleLogStrategy();

  Command cmdobj;
  std::unique_ptr<CommandServer> tsvr = std::make_unique<CommandServer>(
    [&cmdobj](const std::string &cmd) -> std::string
    {
      return cmdobj.Exec(cmd);
    },serverport);

  tsvr->Init();
  tsvr->Start();

  return 0;
}
