#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#include "Comm.hpp"
#include "InetAddr.hpp"

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " serverip serverport" << std::endl;
}

// ./tcp_client serverip serverport
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(0);
    }

    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        std::cerr << "create client sockfd error" << std::endl;
        exit(SOCKET_CREATE_ERR);
    }

    InetAddr server(serverport, serverip);
    if(connect(sockfd, server.Addr(), server.Length()) != 0)
    {
        std::cerr << "connect server error" << std::endl;
        exit(SOCKET_CONNECT_ERR);
    }

    std::cout << "connect " << server.ToString() << " success" << std::endl;
    

    while(true)
    {
        std::cout<<"Please Enter@ ";
        std::string line;
        std::getline(std::cin,line);

        ssize_t n=write(sockfd,line.c_str(),line.size());
        if(n>=0)
        {
            char buffer[1024];
            ssize_t m=read(sockfd,buffer,sizeof(buffer)-1);
            if(m > 0)
            {
                buffer[m]=0;
                std::cout << buffer << std::endl;
            }
        }
    }
    return 0;
}