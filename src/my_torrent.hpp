
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
            std::string unique_id;
            std::string type;
      };

      std::string make_magnet_link(std::vector<char> binary);
      void setup(std::string bind_address, int upload_max, int download_max);
      void listen();
      void add_magnetlink(std::string key, std::string magnetlink);
      void add_torrentfile(std::string key, std::string path);
      void extract_ip_list_from_log(std::string log);
      void remove_magnetlink(std::string key);
      void terminate();
} // namespace my_torrent

#endif
