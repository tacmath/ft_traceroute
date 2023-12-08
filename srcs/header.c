#include "ft_traceroute.h"

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
    header->id = getpid();
    header->frag_off = 0;
    header->ttl = ttl;
    header->protocol = IPPROTO_UDP;
    header->check = 0;
    header->saddr = INADDR_ANY;
    header->daddr = daddr;
}