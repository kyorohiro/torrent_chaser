#include <iostream>
#include <arpa/inet.h>

#include <my_ip_country_detector.hpp>
#include <my_target_list_reader.hpp>
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
std::string setting_ipv4_cvs_path = "./dat/ip2country/IP2LOCATION-LITE-DB1.CSV";
std::string setting_ipv6_cvs_path = "./dat/ip2country/IP2LOCATION-LITE-DB1.IPV6.CSV";
std::string setting_output_path = "./result";

std::string magnetLinkListPath = "./dat/target_list.txt";


void start_http_server_on_thread(int unused)
{
    my_server::listen("0.0.0.0", 8080, username, password);
}


int main(int argc, char *argv[])
{

    // load a pair informaton about ip and contry
    my_ip_country_detector::setup_context(setting_ipv4_cvs_path, setting_ipv6_cvs_path);

    // load a target metainfo ,torrent and magnetlink
    target_list_reader::loadTargetListFile(magnetLinkListPath);

    // start torrent client
    target_list_reader::showDebugLog();

    // setup table
    my_db::setupDB(".app.db");

    // start httpserver
    
    std::thread server_thread(start_http_server_on_thread, 3);//NULL);

    //
    //
    std::string input;
    // setup(std::string bind_address, int upload_max, int download_max)
    my_torrent::setup("", -1, -1);
    std::vector<std::shared_ptr<my_db::TargetInfo>> target_info_list;
    my_db::getMagnetLink(target_info_list);
    for(auto l : target_info_list) {
        my_torrent::add_magnetlink(l->target);
    }
    my_torrent::listen();
    /*
    while (1)
    {
        std::getline(std::cin, input);
        std::cout << "[p]: " <<input << std::endl;
        if (std::cin.eof())
        {
            //std::cin.clear(); // reset cin state
            break;
        }
    }*/
    return 0;
}
