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
#include <regex>

//
#include<ctime>
#include<signal.h>
//
//
//
std::string setting_ipv4_cvs_path = "./res/ip2country/IP2LOCATION-LITE-DB1.CSV";
std::string setting_ipv6_cvs_path = "./res/ip2country/IP2LOCATION-LITE-DB1.IPV6.CSV";

// 
// thread
std::thread server_thread;
std::thread torrent_thread;

void my_sigint_handler(int s){
    printf("Caught signal %d\n",s);

    //
    // terminate server and torrent client
    my_server::terminate();
    my_torrent::terminate();

    //
    // detach thrad
    server_thread.detach();
    torrent_thread.detach();

    //
    sleep(3);
    exit(1);
}

void start_http_server_on_thread(int unused)
{
    my_server::listen("0.0.0.0", 8080, username, password);
}

void start_torrent_client_on_thread(int unused) {
    my_torrent::listen();
}

int main(int argc, char *argv[])
{

    // load a pair informaton about ip and contry
    my_ip_country_detector::setup_context(setting_ipv4_cvs_path, setting_ipv6_cvs_path);

    // setup table
    my_db::setup(".app.db", "./data");


    //
    // setup torrent
    my_torrent::setup("", -1, -1);

    std::vector<std::shared_ptr<my_db::TargetInfo>> target_info_list;
    my_db::get_target_info(target_info_list);
    
    std::regex re(".*\\.magnetlink");    
    for(auto l : target_info_list) {
        if(std::regex_match(l->target,re)){
            //
            {
                std::fstream input(l->target);
                std::vector<char> source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
                my_torrent::add_magnetlink(l->unique_id, std::string(source.data(),source.size()));
            }
        } else {
            my_torrent::add_torrentfile(l->unique_id,l->target);
        }
    }
    

    // start httpserver
    server_thread = std::thread (start_http_server_on_thread, 3);//NULL);

    // start torrent
    torrent_thread = std::thread (start_torrent_client_on_thread, 3);//NULL);

    std::string input_from_console;

    //
    // input setting from console
    // 
    signal (SIGINT, my_sigint_handler);
    while (true)
    { 
        std::cin >> input_from_console;
    }

//    unsigned long int sec = time(NULL);
    return 0;
}
