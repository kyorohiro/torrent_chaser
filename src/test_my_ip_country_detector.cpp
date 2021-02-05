#include<iostream>
# define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <my_ip_country_detector.hpp>

std::string setting_ipv4_cvs_path = "./dat/IP2LOCATION-LITE-DB1.CSV";
std::string setting_ipv6_cvs_path = "./dat/IP2LOCATION-LITE-DB1.IPV6.CSV";

/*
BOOST_AUTO_TEST_CASE(test1)
{
    // load a pair informaton about ip and contry
    my_ip_country_detector::setupContext(setting_ipv4_cvs_path,setting_ipv6_cvs_path); 
    std::string country = my_ip_country_detector::findCountryFromIPv4("127.0.0.1");
    std::cout << country << std::endl;
    BOOST_CHECK( country== "-");
}
*/

BOOST_AUTO_TEST_CASE(test2)
{
    // load a pair informaton about ip and contry
    my_ip_country_detector::setup_context(setting_ipv4_cvs_path,setting_ipv6_cvs_path); 
    std::string country = my_ip_country_detector::find_dns_from_ip("2001:4860:4860::8888");
    std::cout << "'"<< country << "'"<< std::endl;
    BOOST_CHECK( country== "dns.google");
    country = my_ip_country_detector::find_dns_from_ip("8.8.8.8");
    std::cout << "'"<< country <<"'"<< std::endl;
    BOOST_CHECK( country== "dns.google");

}
