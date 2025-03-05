#include "sys/socket.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
static const char pkt_data[] =
        "\x3c\xfd\xfe\x9e\x7f\x71\xec\xb1\xd7\x98\x3a\xc0\x08\x00\x45\x00"
        "\x00\x2e\x00\x00\x00\x00\x40\x11\x88\x97\x05\x08\x07\x08\xc8\x14"
        "\x1e\x04\x10\x92\x10\x92\x00\x1a\x6d\xa3\x34\x33\x1f\x69\x40\x6b"
        "\x54\x59\xb6\x14\x2d\x11\x44\xbf\xaf\xd9\xbe\xaa";
int main(int argc, char **argv)
{
    int n;
    int ret = 0;
    int sock;
    char buf[2048];
    struct ifreq ifreq;
    struct sockaddr_ll saddr;

    // create socket
    if((sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        ret = errno;
        goto error_exit;
    }

    // bind tap0
    snprintf(ifreq.ifr_name, sizeof(ifreq.ifr_name), "tap0");
    if (ioctl(sock, SIOCGIFINDEX, &ifreq)) {
        ret = errno;
        goto error_exit;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sll_family = AF_PACKET;
    saddr.sll_protocol = htons(ETH_P_ALL);
    saddr.sll_ifindex = ifreq.ifr_ifindex;
    saddr.sll_pkttype = PACKET_HOST;

    if(bind(sock, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
        ret = errno;
        goto error_exit;
    }

    // // // recv data
    // while(1) {
    //     n = recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL);
    //     printf("%d bytes recieved\n", n);
    // }

	sendto(sock, pkt_data,sizeof(pkt_data),0,NULL,0);

error_exit:
    if (ret) {
        printf("error: %s (%d)\n", strerror(ret), ret);
    }
    close(sock);
    return ret;
}
