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
    hop->isICMP = option->isICMP;
    for(u_int16_t n = 0; n < option->nbPacket; n++) {
        fill_IP_Header(&hop->packets[n].send.iphdr, (uint32_t)((struct sockaddr_in*)addr->ai_addr)->sin_addr.s_addr, hop->ttl);
    }
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;
    if (!(hop->sockId = create_socket(timeout)))
		return 0;
    return 1;
}

void sendHopPackets(hop_t *hop) {
    for(u_int16_t sequence = 0; sequence < hop->packetNumber; sequence++) {
        ft_bzero(&hop->packets[sequence].recieved, sizeof(struct packet));
        fill_UDP_Header(&hop->packets[sequence].send, hop->port + sequence);
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

void fillDefaultOption(option_t *option) {
    ft_bzero(option, sizeof(option_t));
    option->nbPacket = 3;
    option->port = DEFAULT_STARTING_PORT;
    option->maxHop = 30;
}

int limitOption(int value, int min, int max, int def, char *optionName) {
    if (value < min || value > max) {
        dprintf(2, "L'option %s dois avoir une valeur entre %d et %d valeur par defaut utiliser\n", optionName, min, max);
        return def;
    }
    return value;
}

void getOption(option_t *option, int ac, char **av) {
    fillDefaultOption(option);
    for (int n = 1; n < ac; n++) {
        if (av[n][0] == '-') {
            switch (av[n][1]) {
                case 'q': option->nbPacket = limitOption(ft_atoi(&av[n][2]), 1, MAX_PACKET_PER_HOP, 3, "q"); break;
                case 'm': option->maxHop = limitOption(ft_atoi(&av[n][2]), 1, UINT8_MAX, DEFAULT_MAX_HOP, "m"); break;
                case 'f': option->ttl = limitOption(ft_atoi(&av[n][2]), 1, UINT8_MAX, 1, "f") - 1; break;
                case 'p': option->port = limitOption(ft_atoi(&av[n][2]), 1, UINT16_MAX - MAX_PACKET_PER_HOP, DEFAULT_STARTING_PORT, "p") - 1; break;
                case 'I': option->isICMP = 1; break;
                default: 
                    if (!ft_strcmp(av[n], "--help")) {
                        printUsage();
                        exit(0);
                    }
                    break;
            }
        }
        else
            option->addr = av[n];
    }
}

int main(int ac, char **av) {
    struct addrinfo *addrinfo;
    option_t        option;

    getOption(&option, ac, av);
    if (!option.addr) {
        dprintf(2, "Specify \"host\" missing argument.\n");
        return 1;
    }

    if (getuid() != 0) {
		dprintf(2, "ft_traceroute: Operation not permitted\n");
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
