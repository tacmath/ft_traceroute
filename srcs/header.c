#include "ft_traceroute.h"
/*
void fill_ICMP_Header(struct packet *pkt, u_int16_t sequence) {
    pkt->icmphdr.type = ICMP_ECHO;
    pkt->icmphdr.code = 0;
    pkt->icmphdr.checksum = 0;
    pkt->icmphdr.un.echo.id = 0;
    pkt->icmphdr.un.echo.sequence = sequence;
    pkt->icmphdr.checksum = checksum(&pkt->icmphdr, sizeof(struct packet) - sizeof(struct iphdr));
}*/

void fill_UDP_Header(struct packet *pkt, int port) {
    pkt->udphdr.source = INADDR_ANY;
    pkt->udphdr.dest = htons(port);
    pkt->udphdr.len = htons(sizeof(struct packet) - sizeof(struct iphdr));
    pkt->udphdr.check = 0;
}

void fill_IP_Header(struct iphdr *header, uint32_t daddr, u_int8_t ttl) {
    header->version = IPVERSION;
    header->ihl = 5;
    header->tos = 0;
    header->tot_len = 0;
    header->id = 0;
    header->frag_off = 0;
    header->ttl = ttl;
    header->protocol = IPPROTO_UDP;
    header->check = 0;
    header->saddr = INADDR_ANY;
    header->daddr = daddr;
}