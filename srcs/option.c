#include "ft_traceroute.h"

static void fillDefaultOption(option_t *option) {
    ft_bzero(option, sizeof(option_t));
    option->nbPacket = 3;
    option->port = DEFAULT_STARTING_PORT;
    option->maxHop = 30;
}

static int limitOption(int value, int min, int max, int def, char *optionName) {
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
                    if (!ft_strcmp(av[n], "--icmp"))
                        option->isICMP = 1;
                    break;
            }
        }
        else
            option->addr = av[n];
    }
}