
#ifndef _MY_TORRENT
#define _MY_TORRENT

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace my_torrent
{

      struct IpInfo
      {
            std::string ip_address;
            std::string country;
            std::string domain;

      };
      extern std::map<std::string, std::vector<std::shared_ptr<IpInfo>>> ipinfo_list_map;
      //extern std::map<std::string, std::vector<std::string>> ip_list_map;

      std::string make_magnet_link(std::vector<char> binary);
      void setup(std::string bind_address, int upload_max, int download_max);
      void listen();
      void add_magnetlink(std::string magnetlink);
      void extract_ip_list_from_log(std::string log);
} // namespace my_torrent

#endif
