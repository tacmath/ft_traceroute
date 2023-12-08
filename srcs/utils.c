#include "ft_traceroute.h"

u_int16_t checksum(void *b, int len) {
    u_int16_t *buf;
	u_int32_t sum;
	u_int16_t result;

    buf = b;
    sum = 0;
	for ( sum = 0; len > 1; len -= 2 )
		sum += *buf++;
	if ( len == 1 )
		sum += *(u_int8_t*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}

void ft_bzero(void* buffer, size_t len) {
    for(size_t n = 0; n < len; n++)
        ((char*)buffer)[n] = 0;
}

float getTimeInterval(struct timeval t1, struct timeval t2) { //in ms
    return (t2.tv_sec - t1.tv_sec) * 1000.0f + (t2.tv_usec - t1.tv_usec) * 0.001f;
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

int	ft_atoi(const char *str)
{
	int negatif;
	int resultat;
	int n;

	resultat = 0;
	negatif = 0;
	n = 0;
	while (str[n] == '\t' || str[n] == ' ' || str[n] == '\r'
			|| str[n] == '\n' || str[n] == '\v' || str[n] == '\f')
		n++;
	negatif = (str[n] == '-');
    n += (str[n] == '-' || str[n] == '+');
	while (str[n] >= '0' && str[n] <= '9')
		resultat = resultat * 10 + (str[n++] - '0');
	return ((negatif == 1) ? -resultat : resultat);
}

struct addrinfo *getAddrInfo(char *addr) {
    int error;
    struct addrinfo hints;
    struct addrinfo *addrinfo;

    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_CANONNAME;
    error = getaddrinfo(addr, 0, &hints, &addrinfo);
    if (error) {
        printf("error %s\n", gai_strerror(error));
        return 0;
    }
    return addrinfo;
}

char* strIcmpType(uint8_t type) {
    switch (type) {
        case ICMP_ECHOREPLY:        return "Echo Reply";
        case ICMP_DEST_UNREACH:     return "Destination Unreachable";
        case ICMP_SOURCE_QUENCH:    return "Source Quench";
        case ICMP_REDIRECT:         return "Redirect (change route)";
        case ICMP_ECHO:             return "Echo Request";
        case ICMP_TIME_EXCEEDED:    return "Time to live exceeded";
        case ICMP_PARAMETERPROB:    return "Parameter Problem";
        case ICMP_TIMESTAMP:        return "Timestamp Request";
        case ICMP_TIMESTAMPREPLY:   return "Timestamp Reply";
        case ICMP_INFO_REQUEST:     return "Information Request";
        case ICMP_INFO_REPLY:       return "Information Reply";
        case ICMP_ADDRESS:          return "Address Mask Request";
        case ICMP_ADDRESSREPLY:     return "Address Mask Reply";
        default: return "Type not found";
    }
}

