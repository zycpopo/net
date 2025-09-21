#include "udpserver.hpp"
#include <iostream>
#include <memory>

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " localport" << std::endl;
}

// ./udp_server serverport
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }

    uint16_t port =std::stoi(argv[1]);

    EnableConsoleLogStrategy();
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(port);
    usvr->Init();
    usvr->Start();


    return 0;
}