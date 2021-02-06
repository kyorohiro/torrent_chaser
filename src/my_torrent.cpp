#include <my_torrent.hpp>
#include <vector>
#include <string>
#include <libtorrent/magnet_uri.hpp>
#include <iostream>

//
#include <libtorrent/session.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/alert.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/error_code.hpp>
#include <libtorrent/peer_connection_handle.hpp>

//
//
#include <regex>
#include <sstream>

//
#include <my_ip_country_detector.hpp>
#include <my_base_encode.hpp>
#include <my_db.hpp>
#include <ctime>

namespace my_torrent
{
    //
    int _upload_max = -1;
    int _download_max = -1;
    bool _interrupted = false;

    //
    std::shared_ptr<lt::session> _session;
    std::map<std::string, lt::torrent_handle> _torrent_handle_map = {};

    //std::map<std::string, int> alreadyConnected = {};

    //
    bool _put_ip(std::string key, std::string ip, int port, std::string type, std::string unique_id);

    struct my_peer_plugin : lt::peer_plugin
    {
        std::string _ip;
        int _port;
        std::string _key;
        std::string _name;
        bool _haveFlagToDB = false;
        bool _connectFlagToDB = false;
        my_peer_plugin(std::string key, std::string name, std::string ip, int port)
        {
            _key = key;
            _ip = ip;
            _port = port;
            _name = name;
            _haveFlagToDB = false;
            _connectFlagToDB = false;
        }

        void add_handshake(lt::entry &e) override
        {
        }

        void _set_have_to_db()
        {
            if (!_haveFlagToDB)
            {
                _haveFlagToDB = true;
                _put_ip(_name, _ip, _port, "have", _key);
            }
        }
        void _set_connected_to_db()
        {
            //std::cout << "DEBUG(1)"<<_name<<","<< _ip<<","<< _port<<","<< "C"<<","<< _key <<std::endl;
            if (!_connectFlagToDB)
            {
                //std::cout << "DEBUG(2)"<<_name<<","<< _ip<<","<< _port<<","<< "C"<<","<< _key <<std::endl;
                _connectFlagToDB = true;
                _put_ip(_name, _ip, _port, "connected", _key);
            }
        }
        bool on_have(lt::piece_index_t i) override
        {
            std::cout << "[plugin] on_have " << _key << ":" << _ip << ":" << _port << " on_have " << i << std::endl;
            _set_have_to_db();
            return false;
        }

        bool on_dont_have(lt::piece_index_t i) override
        {
            return false;
        }
        bool on_bitfield(lt::bitfield const &bitfield) override
        {
            std::cout << "[plugin] on_bitfield " << _key << ":" << _ip << ":" << _port << " on_bitfield " << bitfield.count() << "/" << bitfield.size() << std::endl;
            if (bitfield.count() > 0)
            {
                _set_have_to_db();
            }

            return false;
        }
        bool on_have_all() override
        {
            std::cout << "[plugin] on_have_all " << _key << ":" << _ip << ":" << _port << " on_have_all " << std::endl;
            _set_have_to_db();
            return false;
        }
        bool on_have_none() override
        {
            return false;
        }
        void on_disconnect(lt::error_code const &e) override
        {
        }

        void on_connected() override
        {
            std::cout << "[plugin] on_connected " << _key << ":" << _ip << ":" << _port << " on_connect " << std::endl;
            _set_connected_to_db();
        }
    };

    struct my_torrent_plugin : lt::torrent_plugin
    {
        std::string _key;
        std::string _name;
        my_torrent_plugin(std::string key)
        {
            _key = key;
        }

        std::shared_ptr<lt::peer_plugin> new_connection(lt::peer_connection_handle const &pch) override
        {
            std::cout << "[plugin][new_connecton]" << pch.remote().address().to_string() << ":" << pch.local_endpoint().port() << std::endl;
            return std::make_shared<my_peer_plugin>(_key, _name, pch.remote().address().to_string(), pch.local_endpoint().port());
        }

        void on_add_peer(lt::tcp::endpoint const &endpoint, lt::peer_source_flags_t flag1, lt::add_peer_flags_t flag2) override
        {
            std::string type = "other";
            if (lt::peer_info::dht == flag1)
            {
                type = "dht";
            }
            else if (lt::peer_info::tracker == flag1)
            {
                type = "tracker";
            }
            else if (lt::peer_info::lsd == flag1)
            {
                type = "lsd";
            }
            //
            // found ip
            std::cout << "[plugin][on_add_peer][" << type << "]" << endpoint.address().to_string() << ":" << endpoint.port() << std::endl;
            auto h = _torrent_handle_map[_key];
            _name = h.status().name;

            _put_ip(h.status().name, endpoint.address().to_string(), endpoint.port(), type, _key);
            //h.name();
        }
    };

    struct plugin_creator
    {
        std::string _key;
        plugin_creator(std::string key)
        {
            _key = key;
        }

        std::shared_ptr<lt::torrent_plugin>
        operator()(lt::torrent_handle const &h, lt::client_data_t)
        {
            return std::make_shared<my_torrent_plugin>(_key);
        }
    };

    bool _put_ip(std::string key, std::string ip, int port, std::string type, std::string unique_id)
    {

        std::stringstream ss;
        ss << ip << ":" << port;
        std::string ip_with_port = ss.str();
        bool alreadyHas = false;
        if (!alreadyHas)
        {
            std::string country = "-";
            std::string dns = "";
            auto ip_info = std::make_shared<IpInfo>();

            ip_info->ip_address = ip_with_port;
            try
            {
                ip_info->country = my_ip_country_detector::find_country_from_ip(ip);
            }
            catch (std::exception e)
            {
            }

            try
            {
                ip_info->domain = my_ip_country_detector::find_dns_from_ip(ip);
            }
            catch (std::exception e)
            {
            }
            ip_info->type = type;
            ip_info->unique_id = unique_id;
            //
            // save to
            if (!my_db::alreadtExist(ip, port, type))
            {
                my_db::insert_found_ip(key, ip, port, ip_info->country, ip_info->domain, time(nullptr), "", type, unique_id);
            }
        }

        return true;
    }
    void terminate()
    {
        _session->pause();
        // std::this_thread::yield();
        _interrupted = true;
    }

    //
    // make a magnet link from torrentfile
    //
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
    void setup(std::string bind_address, int upload_max, int download_max)
    {
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
        // setting
    }
    void remove_magnetlink(std::string key)
    {
        lt::torrent_handle torrent_handle = _torrent_handle_map[key];
        _session->remove_torrent(torrent_handle);
        _torrent_handle_map.erase(key);
    }

    void add_magnetlink(std::string key, std::string magnetlink)
    {
        lt::add_torrent_params torrent_params;
        torrent_params.save_path = ".data"; // save in this dir
        lt::error_code ec;
        lt::parse_magnet_uri(magnetlink, torrent_params, ec);
        if (ec)
        {
            std::cerr << "wrong magnet link " << magnetlink << std::endl;
            throw std::invalid_argument("wrong magnet link \"" + magnetlink + "\"");
        }
        //torrent_params = lt::parse_magnet_uri(m);
        plugin_creator creator(key);
        torrent_params.extensions.push_back(creator);
        lt::torrent_handle h = _session->add_torrent(torrent_params);
        _torrent_handle_map[key] = h;
        //
        if (_upload_max >= 0)
        {
            h.set_upload_limit(_upload_max);
        }
        if (_download_max >= 0)
        {
            h.set_download_limit(_download_max);
        }
    }
    void add_torrentfile(std::string key, std::string path)
    {
        lt::add_torrent_params torrent_params;
        torrent_params.save_path = ".data"; // save in this dir
        lt::error_code ec;
        torrent_params.ti = std::make_shared<lt::torrent_info>(path);

        if (ec)
        {
            std::cerr << "wrong torrentfile " << path << std::endl;
            throw std::invalid_argument("wrong torrentfile \"" + path + "\"");
        }
        plugin_creator creator(key);
        torrent_params.extensions.push_back(creator);
        lt::torrent_handle h = _session->add_torrent(torrent_params);
        _torrent_handle_map[key] = h;
        //

        if (_upload_max >= 0)
        {
            h.set_upload_limit(_upload_max);
        }
        if (_download_max >= 0)
        {
            h.set_download_limit(_download_max);
        }
    }

    void listen()
    {
        while (_interrupted == false)
        {
            std::vector<lt::alert *> alerts;
            _session->pop_alerts(&alerts);

            lt::state_update_alert *st;
            for (lt::alert *a : alerts)
            {
                // log for develop
                std::cout << "[" << a->type() << "](" << a->what() << ") " << a->message() << std::endl;
            }
            if (0 == alerts.size())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }
    }

} // namespace my_torrent
