#include "ft_traceroute.h"

void printAddrInfo(struct addrinfo *info) {
    char ipbuff[INET_ADDRSTRLEN];
    struct sockaddr_in *sockaddr;

    sockaddr = (struct sockaddr_in*)info->ai_addr;
    inet_ntop(AF_INET, &sockaddr->sin_addr, ipbuff, sizeof(ipbuff));

    printf("ft_traceroute to %s (%s), %d hops max, %d byte packets\n", info->ai_canonname, ipbuff, MAX_HOP,PACKET_SIZE);
}

static void printPacket(struct packet *packet, float delatTime) {
    struct sockaddr_in sockaddr;
    char ipbuff[INET_ADDRSTRLEN];
	char hostName[NI_MAXHOST];
    int hostNameFound;

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = packet->iphdr.saddr;
	hostNameFound = !getnameinfo((struct sockaddr *) &sockaddr, sizeof(struct sockaddr_in), hostName, sizeof(hostName), NULL, 0, NI_NAMEREQD);
    inet_ntop(AF_INET, &packet->iphdr.saddr, ipbuff, sizeof(ipbuff));
    printf(" %s (%s)  %.3f ms", (hostNameFound ? hostName : ipbuff), ipbuff, delatTime);
}

static int hasUniqueAddr(hop_t *hop, unsigned packetIndex) {
    u_int32_t current = hop->packets[packetIndex].recieved.iphdr.saddr;
    for (unsigned n = 0; n < packetIndex; n++)
        if (hop->packets[n].recieved.iphdr.saddr == current)
            return 0;
    return 1;
}

void printHop(hop_t *hop) {
    float delatTime;

    printf("\r %d ", hop->ttl);
    for (unsigned n = 0; n < hop->packetNumber; n++) {
        delatTime = getTimeInterval(hop->packets[n].start, hop->packets[n].end);
        if (hop->packets[n].error)
            printf(" *");
        else if (hasUniqueAddr(hop, n))
            printPacket(&hop->packets[n].recieved, delatTime);
        else
            printf(" %.3f ms", delatTime);
    }
    printf("\n");
}


void printUsage() {
    printf("\nUsage\n  ft_traceroute [options] <destination>\n\nOptions:");
    printf("  <destination>     dns name or ip address\n");
    printf("  --help            print usage\n");
}