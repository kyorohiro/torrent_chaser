#include<iostream>
#include<arpa/inet.h>

#include<my_ip_country_detector.hpp>
#include<my_target_list_reader.hpp>
#include<my_server.hpp>
#include<my_db.hpp>
//
#include<sstream>


//
//
//
std::string setting_ipv4_cvs_path = "./dat/ip2country/IP2LOCATION-LITE-DB1.CSV";
std::string setting_ipv6_cvs_path = "./dat/ip2country/IP2LOCATION-LITE-DB1.IPV6.CSV";
std::string setting_output_path = "./result";

std::string magnetLinkListPath = "./dat/target_list.txt";
std::string username = "yamada";
std::string password = "tarou";



int main(int argc, char* argv[]) {

    // load a pair informaton about ip and contry
    my_ip_country_detector::setupContext(setting_ipv4_cvs_path,setting_ipv6_cvs_path); 

    // load a target metainfo ,torrent and magnetlink
    target_list_reader::loadTargetListFile(magnetLinkListPath);

    // start torrent client
    target_list_reader::showDebugLog();

    // setup table
    my_db::setupDB(".app.db");

    // start httpserver
    my_server::listen("0.0.0.0", 8080, username, password);

    return 0;
}


