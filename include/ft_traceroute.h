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
#include <netinet/udp.h>
#include <signal.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define PACKET_SIZE 60
#define MAX_PACKET_PER_HOP 10
#define DEFAULT_MAX_HOP 30
#define DEFAULT_STARTING_PORT 33434

struct packet {
	struct iphdr	iphdr;
	union {
		struct
		{
			struct udphdr	hdr;
			char payload[PACKET_SIZE - sizeof(struct udphdr)];
		} udp;
		struct
		{
			struct icmphdr	hdr;
			char payload[PACKET_SIZE - sizeof(struct icmphdr)];
		} icmp;
	};
};

struct packet_info {
	struct packet	send;
	struct packet	recieved;
	struct timeval  start;
	struct timeval  end;
	int				error;
};


struct hop {
	struct packet_info	packets[MAX_PACKET_PER_HOP];
	u_int16_t			packetNumber;
	int					sockId;
	struct addrinfo		*addr;
	uint16_t			port;
	uint8_t				ttl;

};

typedef struct hop hop_t;

struct option {
    char        *addr;
	uint8_t nbPacket;
	uint8_t maxHop;
	uint8_t ttl;
	uint16_t port;
	uint8_t isICMP;
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
char ft_strcmp(char *str1, char *str2);

// print.c
void printAddrInfo(struct addrinfo *info, u_int8_t maxHop);
void printHop(hop_t *hop);
void printUsage();

// header.c
void fill_ICMP_Header(struct packet *pkt, u_int16_t sequence);
void fill_UDP_Header(struct udphdr *udphdr, int port);
void fill_IP_Header(struct iphdr *header, uint32_t daddr, u_int8_t ttl, u_int8_t protocol);

//option.c
void getOption(option_t *option, int ac, char **av);


#endif