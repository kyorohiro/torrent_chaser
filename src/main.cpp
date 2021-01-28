#include<iostream>
#include<arpa/inet.h>

#include<my_ip_country_detector.hpp>
#include<my_target_list_reader.hpp>
#include<my_server.hpp>

//
#include<sstream>
//
//
//



//
//
//
std::string setting_ipv4_cvs_path = "./dat/IP2LOCATION-LITE-DB1.CSV";
std::string setting_ipv6_cvs_path = "./dat/IP2LOCATION-LITE-DB1.IPV6.CSV";
std::string setting_output_path = "./result";

std::string magnetLinkListPath = "./dat/target_list.txt";
std::string username = "yamada";
std::string password = "tarou"
;
const std::string html_index =
  "<head>"
  ""
  "</head>"
  "concatenated into just a single string. "
  "The disadvantage is that you have to quote "
  "each part, and newlines must be literal as "
  "usual.";




int main(int argc, char* argv[]) {

    // load a pair informaton about ip and contry
    my_ip_country_detector::setupContext(setting_ipv4_cvs_path,setting_ipv6_cvs_path); 

    // load a target metainfo ,torrent and magnetlink
    target_list_reader::loadTargetListFile(magnetLinkListPath);

    // start torrent client
    target_list_reader::showDebugLog();

    // start httpserver

    my_server::listen("0.0.0.0", 8080, username, password);

    return 0;
}


