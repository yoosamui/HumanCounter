
#ifndef XSYSTEM_H
#define XSYSTEM_H

#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */

#include <stdio.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <netdb.h>
    
#include <arpa/inet.h>
#include <sys/socket.h>    
#include <ifaddrs.h>
//#include <unistd.h>
#include <linux/if_link.h>
#include <sys/ioctl.h>
#include <net/if.h>


    struct ifaddrs *addrs, *tmp;
    struct ifreq ifr;

    class XSystem
    {
    public:

        int getAllInterfaces(vector<string>* v) {
            getifaddrs(&addrs);
            tmp = addrs;
            while (tmp) {
                if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET) {
                    (*v).push_back(tmp->ifa_name);
                }
                tmp = tmp->ifa_next;
            }
            freeifaddrs(addrs);
            return (*v).size();
        }

        int getMacAddress(string interface, vector<string>* v) {

            int sock, j, k;
            char *p, addr[32], mask[32], mac[32];

            sock = socket(PF_INET, SOCK_STREAM, 0);
            if (-1 == sock) {
                perror("socket() ");
                return 1;
            }

            strncpy(ifr.ifr_name, interface.c_str(), sizeof (ifr.ifr_name) - 1);
            ifr.ifr_name[sizeof (ifr.ifr_name) - 1] = '\0';

            if (-1 == ioctl(sock, SIOCGIFADDR, &ifr)) {
                perror("ioctl(SIOCGIFADDR) ");
                return 1;
            }

            p = inet_ntoa(((struct sockaddr_in *) (&ifr.ifr_addr))->sin_addr);
            strncpy(addr, p, sizeof (addr) - 1);
            addr[sizeof (addr) - 1] = '\0';


            if (-1 == ioctl(sock, SIOCGIFNETMASK, &ifr)) {
                perror("ioctl(SIOCGIFNETMASK) ");
                return 1;
            }
            p = inet_ntoa(((struct sockaddr_in *) (&ifr.ifr_netmask))->sin_addr);
            strncpy(mask, p, sizeof (mask) - 1);
            mask[sizeof (mask) - 1] = '\0';

            if (-1 == ioctl(sock, SIOCGIFHWADDR, &ifr)) {
                perror("ioctl(SIOCGIFHWADDR) ");
                return 1;
            }
            for (j = 0, k = 0; j < 6; j++) {
                k += snprintf(mac + k, sizeof (mac) - k - 1, j ? ":%02X" : "%02X",
                        (int) (unsigned int) (unsigned char) ifr.ifr_hwaddr.sa_data[j]);
            }
            mac[sizeof (mac) - 1] = '\0';


            (*v).push_back(ifr.ifr_name);
            (*v).push_back(addr);
            (*v).push_back(mask);
            (*v).push_back(mac);


            close(sock);
            return 0;


        }
    };

#endif /* XSYSTEM_H */

