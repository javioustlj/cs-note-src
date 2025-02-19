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
#define DEST_MAC "\x10\x70\xfd\x12\x7d\x86"
#define CUSTOM_PROTOCOL_TYPE 0xABCD

void sendRawBroadcast() {

    int sockfd = socket(AF_PACKET, SOCK_RAW, htons(CUSTOM_PROTOCOL_TYPE));
    if (sockfd < 0) {
        perror("socket creation failed");
        return;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, "enp1s0", IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl SIOCGIFINDEX failed");
        close(sockfd);
        return;
    }

    struct ifreq if_mac;
    std::memset(&if_mac, 0, sizeof(if_mac));
    std::strncpy(if_mac.ifr_name, "enp1s0", IFNAMSIZ - 1); // 请替换为你的硬件端口名
    if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0) {
        std::cerr << "Error getting MAC address." << std::endl;
        close(sockfd);
        return;
    }

    printf("sa_data: ");
    for (int i = 0; i < sizeof(if_mac.ifr_hwaddr.sa_data); i++) {
        printf("%02x", (unsigned char)if_mac.ifr_hwaddr.sa_data[i]);
        if (i < sizeof(if_mac.ifr_hwaddr.sa_data) - 1) {
            printf(":");
        }
    }
    printf("\n");

    struct sockaddr_ll destAddr;
    memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sll_ifindex = ifr.ifr_ifindex;
    destAddr.sll_halen = ETH_ALEN;
    memcpy(destAddr.sll_addr, DEST_MAC, ETH_ALEN);

    printf("sll_addr: ");
    for (int i = 0; i < ETH_ALEN; i++) {
        printf("%02x", (unsigned char)destAddr.sll_addr[i]);
        if (i < ETH_ALEN - 1) {
            printf(":");
        }
    }
    printf("\n");

    char message[] = "Hello, custom protocol broadcast!";
    char buffer[BUFFER_SIZE];

    struct ether_header *eth_header = (struct ether_header *)buffer;
    memcpy(eth_header->ether_dhost, DEST_MAC, ETH_ALEN);  // 目标 MAC 地址

    std::memcpy(eth_header->ether_shost, if_mac.ifr_hwaddr.sa_data, ETH_ALEN);
    eth_header->ether_type = htons(CUSTOM_PROTOCOL_TYPE);  // 设置协议类型

    printf("ether_dhost: ");
    for (int i = 0; i < ETH_ALEN; i++) {
        printf("%02x", (unsigned char)eth_header->ether_dhost[i]);
        if (i < ETH_ALEN - 1) {
            printf(":");
        }
    }
    printf("\n");

    memcpy(buffer + sizeof(struct ether_header), message, strlen(message));

    ssize_t sentBytes = sendto(sockfd, buffer, strlen(message) + sizeof(struct ether_header), 0, (struct sockaddr*)&destAddr, sizeof(destAddr));
    if (sentBytes < 0) {
        perror("sendto failed");
    } else {
        std::cout << "Broadcast message sent: " << message << std::endl;
    }

    close(sockfd);
}

int main()
{
    sendRawBroadcast();

    return 0;
}
