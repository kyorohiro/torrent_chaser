#include <my_torrent.hpp>
#include <vector>
#include <string>
#include <libtorrent/magnet_uri.hpp>
#include <iostream>

//
#include<libtorrent/session.hpp>
#include<libtorrent/alert_types.hpp>
#include<libtorrent/alert.hpp>
#include<libtorrent/magnet_uri.hpp>
#include<libtorrent/error_code.hpp>

namespace my_torrent
{
    int _upload_max = -1;
    int _download_max = -1;
    std::shared_ptr<lt::session> _session;

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

    //
    // if bind_address is "" mean unset bind_address
    // if upload_max is -1 mean unset upload max
    // if download_max is -1 mean unset download max
    //
    void setup(std::string bind_address, int upload_max, int download_max) {
        //
        // create session
        //
        lt::settings_pack session_params;
        session_params.set_int(lt::settings_pack::alert_mask, lt::alert_category::all);
        if (bind_address.length() != 0)
        {
            session_params.set_str(lt::settings_pack::listen_interfaces, bind_address);
        }

        //session_params.set_bool(lt::settings_pack::enable_dht, false);
        _session = std::make_shared<lt::session>(session_params);

        //      
    }

    void add_magnetlink(std::string magnetlink) {
        lt::add_torrent_params torrent_params;
        torrent_params.save_path = ".data"; // save in this dir
        lt::error_code ec;
		lt::parse_magnet_uri(magnetlink,torrent_params, ec);
		if (ec){
            std::cerr << "wrong magnet link "  << magnetlink << std::endl;
            throw std::invalid_argument("wrong magnet link \""+ magnetlink+"\"");
        }
        //torrent_params = lt::parse_magnet_uri(m);
        lt::torrent_handle h = _session->add_torrent(torrent_params);
        if(_upload_max >= 0 ) {
            h.set_upload_limit(_upload_max);
        }
        if(_download_max >= 0) {
            h.set_download_limit(_download_max);
        }
    }
} // namespace my_torrent