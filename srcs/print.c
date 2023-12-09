#include "ft_traceroute.h"

extern int traceroute_runnig;

void printAddrInfo(struct addrinfo *info, u_int8_t maxHop) {
    char ipbuff[INET_ADDRSTRLEN];
    struct sockaddr_in *sockaddr;

    sockaddr = (struct sockaddr_in*)info->ai_addr;
    inet_ntop(AF_INET, &sockaddr->sin_addr, ipbuff, sizeof(ipbuff));

    printf("ft_traceroute to %s (%s), %d hops max, %d byte packets\n", info->ai_canonname, ipbuff, maxHop,PACKET_SIZE);
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

    if (!traceroute_runnig)
        return;
    printf("\r %d ", hop->ttl);
    for (unsigned n = 0; n < hop->packetNumber; n++) {
        delatTime = getTimeInterval(hop->packets[n].start, hop->packets[n].end);
        if (hop->packets[n].error)
            printf(" *");
        else if (hasUniqueAddr(hop, n))
            printPacket(&hop->packets[n].recieved, delatTime);
        else
            printf("  %.3f ms", delatTime);
    }
    printf("\n");
}


void printUsage() {
    printf("\nUsage\n  ft_traceroute [options] <destination>\n\nOptions:");
    printf("  <destination>     DNS name or ip address\n");
    printf("  -h  --help        Print usage\n");
    printf("  -f<first_ttl>     Start from the first_ttl hop (instead from 1)\n");
    printf("  -I  --icmp        Use ICMP ECHO for tracerouting\n");
    printf("  -m<max_ttl>       Set the max number of hops (max TTL to be reached). Default is 30\n");
    printf("  -p<port>          Set the destination port to use for UDP. Default is 33434\n");
    printf("  -q<nqueries>      Set the number of probes per each hop. Default is 3\n");
}