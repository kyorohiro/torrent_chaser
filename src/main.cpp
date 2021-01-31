#include <iostream>
#include <arpa/inet.h>

#include <my_ip_country_detector.hpp>
#include <my_server.hpp>
#include <my_db.hpp>
#include <my_torrent.hpp>
//
#include <sstream>
#include <thread>
#include <my_config.h>

//
//
//
std::string setting_ipv4_cvs_path = "./res/ip2country/IP2LOCATION-LITE-DB1.CSV";
std::string setting_ipv6_cvs_path = "./res/ip2country/IP2LOCATION-LITE-DB1.IPV6.CSV";


void start_http_server_on_thread(int unused)
{
    my_server::listen("0.0.0.0", 8080, username, password);
}


int main(int argc, char *argv[])
{

    // load a pair informaton about ip and contry
    my_ip_country_detector::setup_context(setting_ipv4_cvs_path, setting_ipv6_cvs_path);

    // setup table
    my_db::setup(".app.db", "./data");


    // start httpserver
    std::thread server_thread(start_http_server_on_thread, 3);//NULL);

    /*
    //
    // start torrent
    my_torrent::setup("", -1, -1);

    std::vector<std::shared_ptr<my_db::TargetInfo>> target_info_list;
    my_db::get_magnetlink(target_info_list);
    for(auto l : target_info_list) {
        my_torrent::add_magnetlink(l->target);
    }

    my_torrent::listen();
    */
    server_thread.join();

    return 0;
}
