#ifndef FT_TRACEROUTE_H
#define FT_TRACEROUTE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define PACKET_SIZE 60
#define MAX_PACKET_PER_HOP 10
#define MAX_HOP 30

struct packet {
	struct iphdr	iphdr;
	struct icmphdr	icmphdr;
	char payload[PACKET_SIZE - sizeof(struct icmphdr)];
};

struct packet_info {
	struct packet	send;
	struct packet	recieved;
	struct timeval  start;
	struct timeval  end;
};


struct hop {
	struct packet_info	packets[MAX_PACKET_PER_HOP];
	int					sockIds[MAX_PACKET_PER_HOP];
	u_int16_t			packetNumber;
	struct addrinfo		*addr;
	uint8_t				ttl;

};

typedef struct hop hop_t;

struct option {
    char        *addr;
    uint8_t help;
};

typedef struct option option_t;


// utils.c
u_int16_t checksum(void *b, int len);
void ft_bzero(void* buffer, size_t len);
float getTimeInterval(struct timeval t1, struct timeval t2);
int	ft_atoi(const char *str);
struct addrinfo *getAddrInfo(char *addr);
char* strIcmpType(uint8_t type);

// print.c
void printAddrInfo(struct addrinfo *info);
void printPacket(struct packet *packet, float delatTime);
void printUsage();

// header.c
void fill_ICMP_Header(struct packet *pkt, u_int16_t sequence);
void fill_IP_Header(struct iphdr *header, uint32_t daddr, u_int8_t ttl);


#endif