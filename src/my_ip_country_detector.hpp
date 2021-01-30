#ifndef _MY_IP_COUNTER_DETECTOR

#define _MY_IP_COUNTER_DETECTOR
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>
#include <arpa/inet.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/address_v6.hpp>


namespace my_ip_country_detector {

 
struct IpAndCountryInfo
{
    boost::multiprecision::uint128_t begin;
    boost::multiprecision::uint128_t end;
    std::string country_name;
};   

IpAndCountryInfo createIpAndCountryInfo(std::string line);
void setupContext(std::string filepathV4, std::string filepathV6);
std::string findCountryFromIPv4(boost::multiprecision::uint128_t ip);
std::string findCountryFromIPv6(boost::multiprecision::uint128_t ip);
std::string findCountryFromIPv4(std::string ip);
std::string findCountryFromIPv6(std::string ip);
std::string findCountryFromIP(std::string ip);
}


#endif // #ifndef _MY_IP_COUNTER_DETECTOR