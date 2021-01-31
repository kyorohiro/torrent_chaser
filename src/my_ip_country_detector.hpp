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

//
// Get DOMAIN FROM IP
// Get CUNTRY FROM IP
//
namespace my_ip_country_detector
{

    struct IpAndCountryInfo
    {
        boost::multiprecision::uint128_t begin;
        boost::multiprecision::uint128_t end;
        std::string country_name;
    };

    IpAndCountryInfo create_ip_and_country_info(std::string line);
    void setup_context(std::string filepathV4, std::string filepathV6);
    std::string find_country_from_ipv4(boost::multiprecision::uint128_t ip);
    std::string find_country_from_ipv6(boost::multiprecision::uint128_t ip);
    std::string find_country_from_ipv4(std::string ip);
    std::string find_country_from_ipv6(std::string ip);
    std::string find_country_from_ip(std::string ip);
    std::string find_dns_from_ip(std::string ip);

} // namespace my_ip_country_detector

#endif // #ifndef _MY_IP_COUNTER_DETECTOR