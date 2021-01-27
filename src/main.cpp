#include<iostream>
#include<ip_country_detector.hpp>
#include<arpa/inet.h>

std::string setting_ipv4_cvs_path = "./dat/IP2LOCATION-LITE-DB1.CSV";
std::string setting_ipv6_cvs_path = "./dat/IP2LOCATION-LITE-DB1.IPV6.CSV";
std::string setting_output_path = "./result";

std::string magnetLinkListPath = "./dat/target_list.txt";

int main(int argc, char* argv[]) {
    ip_country_detector::setupContext(setting_ipv4_cvs_path,setting_ipv6_cvs_path);    
    return 0;
}

int compare_ipv6(struct in6_addr *ipA, struct in6_addr *ipB)
{
    int i = 0;
    for(i = 0; i < 16; ++i) // Don't use magic number, here just for example
    {
        if (ipA->s6_addr[i] < ipB->s6_addr[i])
            return -1;
        else if (ipA->s6_addr[i] > ipB->s6_addr[i])
            return 1;
    }
    return 0;
}
