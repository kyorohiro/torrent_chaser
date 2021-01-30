#include <my_torrent.hpp>
#include <vector>
#include <string>
#include <libtorrent/magnet_uri.hpp>
#include <iostream>

namespace my_torrent
{
    std::string make_magnet_link(std::vector<char> b)
    {
        lt::error_code ec;
        lt::torrent_info torrent_info(b.data(), b.size(), ec);
        if (ec)
        {
            std::cerr << "wrong magnet link " << std::endl;
            throw std::invalid_argument("wrong torrent file");
        }
        return lt::make_magnet_uri(torrent_info);
    }

} // namespace my_torrent