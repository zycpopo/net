#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " serverip serverport" << std::endl;
}

// ./udp_client server_ip server_port
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(0);
    }

    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cout << "create socket errror" << std::endl;
        return 0;
    }
    // [srcip, srcport] [dstip, dstport]
    // client要不要：显示的bind自己的ip和端口？不要！！！
    // client 要不要 隐式bind IP和端口？
    // 为什么？client会在自己OS的帮助下，随机bind端口号

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(serverport);
    server.sin_addr.s_addr = inet_addr(serverip.c_str());

    while (true)
    {
        std::cout << "Please Enter@ ";
        std::string line;
        std::getline(std::cin, line);

        // 写
        sendto(sockfd, line.c_str(), line.size(), 0, (struct sockaddr*)&server, sizeof(server));

        //读
        struct sockaddr_in temp;
        socklen_t len = sizeof(temp);
        char buffer[1024];
        int m = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&temp, &len);
        if(m > 0)
        {
            buffer[m] = 0;
            std::cout << buffer << std::endl;
        }
    }

    return 0;
}