#include "ft_traceroute.h"

int traceroute_runnig = 1;

void stop_loop(int sig) {
    (void)sig;
    traceroute_runnig = 0;
}

int create_socket(struct timeval timeout) {
    int sockId;
    int option;


    if ((sockId = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        dprintf(2, "ft_traceroute: Socket creation failed\n");
        return 0;
    }

    // set custom header to true
    option = 1;
    if (setsockopt(sockId, IPPROTO_IP, IP_HDRINCL, &option, sizeof(option))) {
        dprintf(2, "ft_traceroute: Failed to set socket option\n");
        return 0;
    }

	// setting timeout of recv setting
    if (setsockopt(sockId, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
        dprintf(2, "ft_traceroute: Failed to set socket option\n");
        return 0;
    }
    return (sockId);
}

void incrementHopTTL(hop_t *hop) {
    hop->ttl++;
    for(u_int16_t n = 0; n < hop->packetNumber; n++)
        hop->packets[n].send.iphdr.ttl = hop->ttl;
}

int initHop(hop_t *hop, struct addrinfo *addr, option_t *option) {
    struct timeval timeout;
    
    ft_bzero(hop, sizeof(hop_t));
    hop->packetNumber = option->nbPacket;
    hop->addr = addr;
    hop->ttl = option->ttl;
    hop->port = option->port;
    for(u_int16_t n = 0; n < option->nbPacket; n++)
        fill_IP_Header(&hop->packets[n].send.iphdr, (uint32_t)((struct sockaddr_in*)addr->ai_addr)->sin_addr.s_addr, hop->ttl, 
           option->isICMP ? IPPROTO_ICMP : IPPROTO_UDP);
    timeout.tv_sec = 0;
    timeout.tv_usec = 200000;
    if (!(hop->sockId = create_socket(timeout)))
		return 0;
    return 1;
}

void sendHopPackets(hop_t *hop) {
    for(u_int16_t sequence = 0; sequence < hop->packetNumber && traceroute_runnig; sequence++) {
        ft_bzero(&hop->packets[sequence].recieved, sizeof(struct packet));
        if (hop->packets[sequence].send.iphdr.protocol == IPPROTO_UDP)
            fill_UDP_Header(&hop->packets[sequence].send.udp.hdr, hop->port + sequence);
        else
            fill_ICMP_Header(&hop->packets[sequence].send, sequence);
        gettimeofday(&hop->packets[sequence].start, 0);
        sendto(hop->sockId, &hop->packets[sequence].send, sizeof(struct packet), 0, (struct sockaddr*)hop->addr->ai_addr, sizeof(*hop->addr->ai_addr));
        hop->packets[sequence].error = recvfrom(hop->sockId, &hop->packets[sequence].recieved, sizeof(struct packet), 0, INADDR_ANY, 0) < 0;
        gettimeofday(&hop->packets[sequence].end, 0);
    }
}

int isFinalAddressReatched(hop_t *hop) {
    u_int32_t finalAddr;

    finalAddr = (uint32_t)((struct sockaddr_in*)hop->addr->ai_addr)->sin_addr.s_addr;
    for (unsigned n = 0; n < hop->packetNumber; n++)
        if (hop->packets[n].recieved.iphdr.saddr == finalAddr)
            return 1;
    return 0;
}

int loop(struct addrinfo *addr, option_t *option) {
    hop_t   hop;
    

    if (!initHop(&hop, addr, option))
        return 0;
    
    while (traceroute_runnig && hop.ttl < option->maxHop && !isFinalAddressReatched(&hop)) {
        incrementHopTTL(&hop);
        sendHopPackets(&hop);
        printHop(&hop);
    }
    close(hop.sockId);
    return 1;
}

int main(int ac, char **av) {
    struct addrinfo *addrinfo;
    option_t        option;

    getOption(&option, ac, av);
    if (!option.addr) {
        dprintf(2, "Specify \"host\" missing argument.\n");
        return 1;
    }

    signal(SIGINT, &stop_loop);
    if (!(addrinfo = getAddrInfo(option.addr)))
        return 1;
    printAddrInfo(addrinfo, option.maxHop);
    if (!loop(addrinfo, &option))
        return 1;
    freeaddrinfo(addrinfo);
    return 0;
}
