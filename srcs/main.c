#include "ft_traceroute.h"

int traceroute_runnig = 1;

void stop_loop(int sig) {
    (void)sig;
    traceroute_runnig = 0;
}

int create_socket(struct timeval timeout) {
    int sockId;
    int option;


    if ((sockId = socket(PF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
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

int initHop(hop_t *hop, struct addrinfo *addr, u_int16_t packetNumber) {
    struct timeval timeout;
    
    ft_bzero(hop, sizeof(hop_t));
    hop->packetNumber = packetNumber;
    hop->addr = addr;
    hop->ttl = 0;
    timeout.tv_sec = 0;
    timeout.tv_usec = 50000;
    for(u_int16_t n = 0; n < packetNumber; n++) {
        fill_IP_Header(&hop->packets[n].send.iphdr, (uint32_t)((struct sockaddr_in*)addr->ai_addr)->sin_addr.s_addr, hop->ttl);
            if (!(hop->sockIds[n] = create_socket(timeout)))
		        return 0;
    }
    return 1;
}

void sendHopPackets(hop_t *hop) {
    for(u_int16_t sequence = 0; sequence < hop->packetNumber; sequence++) {
        fill_UDP_Header(&hop->packets[sequence].send);
        gettimeofday(&hop->packets[sequence].start, 0);
        sendto(hop->sockIds[0], &hop->packets[sequence].send, sizeof(struct packet), 0, (struct sockaddr*)hop->addr->ai_addr, sizeof(*hop->addr->ai_addr));
    }
}

void recieveHopPackets(hop_t *hop) {
    /*struct timeval timeout;
    int ret = 0;
    fd_set fds;*/
    

    /*for (unsigned n = 0; n < hop->packetNumber; n++) {
        FD_ZERO(&fds[n]);
        FD_SET(hop->sockIds[n], &fds[n]);
    }*/
    
    /*while (ret < hop->packetNumber) {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        FD_ZERO(&fds);
        for (unsigned n = 0; n < hop->packetNumber; n++)
            FD_SET(hop->sockIds[n], &fds);
        ret = select(hop->sockIds[hop->packetNumber - 1] + 1, &fds, 0, 0, &timeout);
    }*/

    for(u_int16_t sequence = 0; sequence < hop->packetNumber; sequence++) {
        ft_bzero(&hop->packets[sequence].recieved, sizeof(struct packet));
        hop->packets[sequence].error = recvfrom(hop->sockIds[0], &hop->packets[sequence].recieved, sizeof(struct packet), 0, INADDR_ANY, 0) < 0;
        gettimeofday(&hop->packets[sequence].end, 0);
    }
}

void freeHop(hop_t *hop) {
    for (unsigned n = 0; n < hop->packetNumber; n++)
        close(hop->sockIds[0]);
}

int loop(struct addrinfo *addr) {
    hop_t   hop;
    u_int32_t finalAddr;

    if (!initHop(&hop, addr, 3))
        return 0;
    finalAddr = (uint32_t)((struct sockaddr_in*)addr->ai_addr)->sin_addr.s_addr;
    while (traceroute_runnig && hop.ttl <= MAX_HOP && finalAddr != hop.packets->recieved.iphdr.saddr) {
        incrementHopTTL(&hop);
        sendHopPackets(&hop);
        recieveHopPackets(&hop);
        printHop(&hop);
    }
    freeHop(&hop);
    return 1;
}

void fillDefaultOption(option_t *option) {
    ft_bzero(option, sizeof(option_t));
}

int limitOption(int value, int min, int max, int def, char *optionName) {
    if (value < min || value > max) {
        dprintf(2, "L'option %s dois avoir une valeur entre %d et %d valeur par defaut utiliser\n", optionName, min, max);
        return def;
    }
    return value;
}

char ft_strcmp(char *str1, char *str2) {
    unsigned int n;

    if (!str1 || !str2)
        return (str1 != str2);
    n = 0;
    while(str1[n] == str2[n] && str1[n])
        n++;
    return str1[n] - str2[n];
}

void getOption(option_t *option, int ac, char **av) {
    fillDefaultOption(option);
    for (int n = 1; n < ac; n++) {
        if (av[n][0] == '-') {
            switch (av[n][1]) {
                
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

 //   signal(SIGINT, &stop_loop);
    if (!(addrinfo = getAddrInfo(option.addr)))
        return 1;
    printAddrInfo(addrinfo);
    if (!loop(addrinfo))
        return 1;
    freeaddrinfo(addrinfo);
    return 0;
}
