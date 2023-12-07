#include "ft_traceroute.h"

void printAddrInfo(struct addrinfo *info) {
    char ipbuff[INET_ADDRSTRLEN];
    struct sockaddr_in *sockaddr;

    sockaddr = (struct sockaddr_in*)info->ai_addr;
    inet_ntop(AF_INET, &sockaddr->sin_addr, ipbuff, sizeof(ipbuff));

    printf("ft_traceroute to %s (%s), %d hops max, %d byte packets\n", info->ai_canonname, ipbuff, MAX_HOP,PACKET_SIZE);
}

void printPacket(struct packet *packet, float delatTime) {
    u_int8_t    ttl;
    struct sockaddr_in sockaddr;
    char ipbuff[INET_ADDRSTRLEN];
	char hostName[NI_MAXHOST];
    int hostNameFound;

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = packet->iphdr.saddr;
	hostNameFound = !getnameinfo((struct sockaddr *) &sockaddr, sizeof(struct sockaddr_in), hostName, sizeof(hostName), NULL, 0, NI_NAMEREQD);
    inet_ntop(AF_INET, &packet->iphdr.saddr, ipbuff, sizeof(ipbuff));
    ttl = packet->iphdr.ttl;
    printf("%-45s (%s)  %.3f ms ttl=%d saddr=%d", (hostNameFound ? hostName : ipbuff), ipbuff, delatTime, ttl, packet->iphdr.saddr);
}

void printUsage() {
    printf("\nUsage\n  ft_traceroute [options] <destination>\n\nOptions:");
    printf("  <destination>     dns name or ip address\n");
    printf("  --help            print usage\n");
}