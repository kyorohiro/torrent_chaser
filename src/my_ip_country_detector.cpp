#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <vector>
#include <arpa/inet.h>
#include <my_ip_country_detector.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <regex>
#include <boost/asio/ip/address.hpp>

//
// Get country name from ip address
//
// data format ref https://lite.ip2location.com/database/ip-country
namespace my_ip_country_detector
{
    std::vector<IpAndCountryInfo> contextV4;
    std::vector<IpAndCountryInfo> contextV6;

    void loadDataFromCVS(std::string filepath, std::vector<IpAndCountryInfo> &output);

    IpAndCountryInfo find(std::vector<IpAndCountryInfo> ips, boost::multiprecision::uint128_t ip);

    void setup_context(std::string filepathV4, std::string filepathV6)
    {
        loadDataFromCVS(filepathV4, contextV4);
        loadDataFromCVS(filepathV6, contextV6);
    }

    std::string find_country_from_ip(std::string ip)
    {
        boost::asio::ip::address a(boost::asio::ip::address::from_string(ip));

        if (a.is_v4())
        {
            return find_country_from_ipv4(ip);
        }
        else
        {
            return find_country_from_ipv6(ip);
        }
    }

    std::string find_dns_from_ip(std::string ip)
    {
        boost::asio::ip::address a(boost::asio::ip::address::from_string(ip));
        if (a.is_v4())
        {
            // v4
            struct sockaddr_in sa; /* input */
            socklen_t len;         /* input */
            char hbuf[NI_MAXHOST];
            memset(&sa, 0, sizeof(struct sockaddr_in));
            sa.sin_family = AF_INET;
            sa.sin_addr.s_addr = inet_addr(ip.c_str());
            len = sizeof(struct sockaddr_in);
            if (getnameinfo((struct sockaddr *)&sa, len, hbuf, sizeof(hbuf),
                            NULL, 0, NI_NAMEREQD))
            {
                return "";
            }
            else
            {
                printf("host=%s\n", hbuf);
                return std::string(hbuf);
            }
        }
        else
        {
            struct sockaddr_in6 sa6;
            char hbuf[NI_MAXHOST];
            memset(&sa6, 0, sizeof(struct sockaddr_in6));

            sa6.sin6_family = AF_INET6;
            in6_addr addr6;
            int s = inet_pton(AF_INET6, ip.c_str(), &addr6);
            sa6.sin6_addr = addr6;

            int len = sizeof(struct sockaddr_in6);
            if (getnameinfo((struct sockaddr *)&sa6, len, hbuf, sizeof(hbuf),
                            NULL, 0, NI_NAMEREQD))
            {
                return "";
            }
            else
            {
                return std::string(hbuf);
            }
        }
    }
    std::string find_country_from_ipv6(std::string ip)
    {
        boost::asio::ip::address_v6 v6 = boost::asio::ip::address_v6::from_string(ip);
        boost::multiprecision::uint128_t val{};
        for (uint8_t b : v6.to_bytes())
        {
            (val <<= 8) |= b;
        }
        return find_country_from_ipv6(val);
    }
    std::string find_country_from_ipv4(std::string ip)
    {
        //std::cout << "(9) <<" << ip << std::endl;
        boost::asio::ip::address_v4 i = boost::asio::ip::address_v4::from_string(ip.c_str());
        //std::cout << "(9) <<" << i.to_ulong() << std::endl;

        //std::cout << boost::multiprecision::uint128_t(i.to_ulong()) << std::endl;
        return find_country_from_ipv4(boost::multiprecision::uint128_t(i.to_ulong()));
    }

    std::string find_country_from_ipv4(boost::multiprecision::uint128_t ip)
    {
        return find(contextV4, ip).country_name;
    }

    std::string find_country_from_ipv6(boost::multiprecision::uint128_t ip)
    {
        return find(contextV6, ip).country_name;
    }

    void loadDataFromCVS(std::string filepath, std::vector<IpAndCountryInfo> &output)
    {
        std::fstream f(filepath);
        std::string l;

        while (std::getline(f, l))
        {
            //std::cout << l << std::endl;
            auto v = create_ip_and_country_info(l);
            output.push_back(v);
        }
    }

    IpAndCountryInfo create_ip_and_country_info(std::string line)
    {
        IpAndCountryInfo ret;
        std::stringstream ss;
        ss << line;
        std::string begin_src;
        std::string end_src;

        std::getline(ss, begin_src, ',');
        std::getline(ss, end_src, ',');
        std::getline(ss, ret.country_name, ',');
        if (ret.country_name.length() > 0 && ret.country_name[0] == '"')
        {
            ret.country_name = ret.country_name.substr(1, ret.country_name.length() - 2);
        }
        if (begin_src.length() > 0 && begin_src[0] == '"')
        {
            begin_src = begin_src.substr(1, begin_src.length() - 2);
        }
        ret.begin = boost::multiprecision::uint128_t(begin_src);

        if (end_src.length() > 0 && end_src[0] == '"')
        {
            end_src = end_src.substr(1, end_src.length() - 2);
        }
        ret.end = boost::multiprecision::uint128_t(end_src);

        return ret;
    }

    IpAndCountryInfo find(std::vector<IpAndCountryInfo> ips, boost::multiprecision::uint128_t ip)
    {
        int min = 0;
        int max = ips.size() - 1;
        //std::cout << "(19) <<" << ip << "m:" << min << "s:" << max << std::endl;
        //std::cout << "::" << ips[min].end << std::endl;
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
                //std::cout << ":f:" << i.country_name << "::" << std::endl;
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
            //std::cout << "::" << index << "::" << i.begin << "<" << ip << "<" << i.end << "::" << std::endl;
        } while (prev_index != index);

        //for(ipinfo i : ips) {
        //    if(i.begin <= ip && ip<=i.end) {
        //        return i;
        //    }
        //}
        throw std::invalid_argument("ipinfo::find");
    }

} // namespace my_ip_country_detector
