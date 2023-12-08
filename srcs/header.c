#include "ft_traceroute.h"

void fill_ICMP_Header(struct packet *pkt, u_int16_t sequence) {
    pkt->icmp.hdr.type = ICMP_ECHO;
    pkt->icmp.hdr.code = 0;
    pkt->icmp.hdr.checksum = 0;
    pkt->icmp.hdr.un.echo.id = 0;
    pkt->icmp.hdr.un.echo.sequence = sequence;
    pkt->icmp.hdr.checksum = checksum(&pkt->icmp.hdr, sizeof(struct packet) - sizeof(struct iphdr));
}

void fill_UDP_Header(struct udphdr *udphdr, int port) {
    udphdr->source = INADDR_ANY;
    udphdr->dest = htons(port);
    udphdr->len = htons(sizeof(struct packet) - sizeof(struct iphdr));
    udphdr->check = 0;
}

void fill_IP_Header(struct iphdr *header, uint32_t daddr, u_int8_t ttl, u_int8_t protocol) {
    header->version = IPVERSION;
    header->ihl = 5;
    header->tos = 0;
    header->tot_len = 0;
    header->id = getpid();
    header->frag_off = 0;
    header->ttl = ttl;
    header->protocol = protocol;
    header->check = 0;
    header->saddr = INADDR_ANY;
    header->daddr = daddr;
}