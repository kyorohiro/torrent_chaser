#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>
#include <arpa/inet.h>
#include <my_ip_country_detector.hpp>
#include <boost/asio/ip/address_v6.hpp>
namespace my_ip_country_detector {

//
// data format ref https://lite.ip2location.com/database/ip-country
// "0","281470681743359","-","-"
// "281470681743360","281470698520575","-","-"
// "281470698520576","281470698520831","US","United States of America"
//  


std::vector<IpAndCountryInfo> contextV4;
std::vector<IpAndCountryInfo> contextV6;

void loadDataFromCVS(std::string filepath, std::vector<IpAndCountryInfo> &output);

IpAndCountryInfo find(std::vector<IpAndCountryInfo> ips, boost::multiprecision::uint128_t ip);

void setupContext(std::string filepathV4, std::string filepathV6){
    loadDataFromCVS(filepathV4, contextV4);
    loadDataFromCVS(filepathV6, contextV6);
}

std::string findCountryFromIPv4(std::string ip) {
    std::cout << "(9) <<" << ip << std::endl;
    boost::asio::ip::address_v4 i = boost::asio::ip::address_v4::from_string(ip.c_str());
    std::cout << "(9) <<" << i.to_ulong()<< std::endl;
 
    std::cout << boost::multiprecision::uint128_t(i.to_ulong()) << std::endl;
    return findCountryFromIPv4(boost::multiprecision::uint128_t(i.to_ulong()));
}

std::string findCountryFromIPv4(boost::multiprecision::uint128_t ip) {
    return find(contextV4, ip).country_name;
}

std::string findCountryFromIPv6(boost::multiprecision::uint128_t ip) {
    return find(contextV6, ip).country_name;
}

void loadDataFromCVS(std::string filepath, std::vector<IpAndCountryInfo> &output)
{
    std::fstream f(filepath);
    std::string l;

    while (std::getline(f, l))
    {
        //std::cout << l << std::endl;
        auto v = createIpAndCountryInfo(l);
        output.push_back(v);
    }
}

IpAndCountryInfo createIpAndCountryInfo(std::string line) {
    IpAndCountryInfo ret;
        std::stringstream ss;
        ss << line;
        std::string begin_src;
        std::string end_src;

        std::getline(ss, begin_src, ',');
        std::getline(ss, end_src, ',');
        std::getline(ss, ret.country_name, ',');

        if (begin_src.length() > 0 && begin_src[0] == '"')
        {
            begin_src = begin_src.substr(1, begin_src.length() - 2);
        }
        ret.begin = boost::multiprecision::uint128_t(begin_src);

        if (end_src.length() > 0 && end_src[0] == '"')
        {
            end_src = end_src.substr(1, end_src.length() - 2);
        }
        ret.end= boost::multiprecision::uint128_t(end_src);

        return ret;
    }
    
IpAndCountryInfo find(std::vector<IpAndCountryInfo> ips, boost::multiprecision::uint128_t ip)
{
    int min = 0;
    int max = ips.size() - 1;
    std::cout << "(19) <<" << ip <<"m:"<<min << "s:"<< max << std::endl;
    std::cout << "::" << ips[min].end<< std::endl;
    if (ips[min].begin <= ip && ip <= ips[min].end)
    {
        return ips[min];
    }
    if (ips[max].begin <= ip && ip <= ips[max].end)
    {
        return ips[max];
    }
    int index = -1;
    int prev_index = -1;
    do
    {
        prev_index = index;
        index = (max - min) / 2 + min;
        IpAndCountryInfo i = ips[index];
        if (i.begin <= ip && ip <= i.end)
        {
            std::cout << ":f:" << i.country_name<< "::"  << std::endl;
            return i;
        }
        if (ip < i.begin)
        {
            max = index;
        }
        else if (i.end < ip)
        {
            min = index;
        }
        std::cout << "::" << index<< "::"<<  i.begin << "<" <<ip << "<"<<i.end << "::"<< std::endl;
    } while (prev_index != index);

    //for(ipinfo i : ips) {
    //    if(i.begin <= ip && ip<=i.end) {
    //        return i;
    //    }
    //}
    throw std::invalid_argument("ipinfo::find");
}



}
