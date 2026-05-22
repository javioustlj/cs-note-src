#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

#define BUFFER_SIZE 1024
#define BROADCAST_MAC "\xff\xff\xff\xff\xff\xff"
#define CUSTOM_PROTOCOL_TYPE 0xABCD

void receiveRawBroadcast()
{
    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(CUSTOM_PROTOCOL_TYPE));
    if (sockfd < 0) {
        perror("socket creation failed");
        return;
    }

    char buffer[BUFFER_SIZE];
    std::cout << "Waiting for messages..." << std::endl;
    while (true) {
        std::memset(buffer, 0, BUFFER_SIZE);
        ssize_t recvBytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, nullptr, nullptr);
        if (recvBytes < 0) {
            perror("recvfrom failed");
            continue;
        }
        else {
            std::cout << "Received other message: " << std::endl;
        }
        // 解析以太网头部
        struct ether_header *eth_header = (struct ether_header *)buffer;
        if (ntohs(eth_header->ether_type) == CUSTOM_PROTOCOL_TYPE) {
            char *message = buffer + sizeof(struct ether_header);
            std::cout << "Received broadcast message: " << message << std::endl;
        }
        else {
            std::cout << "Received non-custom protocol message." << std::endl;
        }
    }
    close(sockfd);
}

int main()
{
    receiveRawBroadcast();
    return 0;
}
