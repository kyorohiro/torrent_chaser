
#ifndef _MY_TORRENT
#define _MY_TORRENT

#include<string>
#include<vector>
namespace my_torrent {
      std::string make_magnet_link(std::vector<char> binary);
      void setup(std::string bind_address, int upload_max, int download_max);
      void listen();
}

#endif
