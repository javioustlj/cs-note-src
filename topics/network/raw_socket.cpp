#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

int main() {
    // 创建原始套接字
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0) {
        std::cerr << "Socket creation failed." << std::endl;
        return 1;
    }

    // 获取接口索引
    struct ifreq if_idx;
    std::memset(&if_idx, 0, sizeof(if_idx));
    std::strncpy(if_idx.ifr_name, "eth0", IFNAMSIZ - 1); // 请替换为你的硬件端口名
    if (ioctl(sock, SIOCGIFINDEX, &if_idx) < 0) {
        std::cerr << "Error getting interface index." << std::endl;
        close(sock);
        return 1;
    }

    // 获取硬件地址
    struct ifreq if_mac;
    std::memset(&if_mac, 0, sizeof(if_mac));
    std::strncpy(if_mac.ifr_name, "eth0", IFNAMSIZ - 1); // 请替换为你的硬件端口名
    if (ioctl(sock, SIOCGIFHWADDR, &if_mac) < 0) {
        std::cerr << "Error getting MAC address." << std::endl;
        close(sock);
        return 1;
    }

    // 构造以太网头部
    struct ether_header *eh = (struct ether_header *)malloc(ETH_HLEN);
    std::memset(eh, 0, ETH_HLEN);

    // 设置目的MAC地址为广播地址
    std::memcpy(eh->ether_dhost, "\xFF\xFF\xFF\xFF\xFF\xFF", ETH_ALEN);

    // 设置源MAC地址（从if_mac获取）
    std::memcpy(eh->ether_shost, if_mac.ifr_hwaddr.sa_data, ETH_ALEN);

    // 设置以太网类型为IPv4
    eh->ether_type = htons(ETH_P_IP);

    // 构造发送的帧
    char buffer[ETH_FRAME_LEN];
    std::memset(buffer, 0, ETH_FRAME_LEN);
    std::memcpy(buffer, eh, ETH_HLEN);

    // 构造socket地址结构
    struct sockaddr_ll sll;
    std::memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_idx.ifr_ifindex;
    sll.sll_halen = ETH_ALEN;

    // 发送广播包
    if (sendto(sock, buffer, ETH_HLEN, 0, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
        std::cerr << "Send failed." << std::endl;
    } else {
        std::cout << "Broadcast packet sent." << std::endl;
    }

    // 清理资源
    free(eh);
    close(sock);

    return 0;
}
