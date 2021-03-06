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
#include <ctime>
#include <signal.h>
//
//
//

//
// thread
std::thread server_thread;
std::thread torrent_thread;

void my_sigint_handler(int s)
{
    std::cout << "Caught signal " << s << std::endl;
    ;
    std::cout << "S Terminate http servdf" << s << std::endl;
    ;
    my_server::terminate();
    std::cout << "E Terminate http server" << s << std::endl;
    ;
    std::cout << "S Terminate torrent" << s << std::endl;
    ;
    my_torrent::terminate();
    std::cout << "E Terminate torrent" << s << std::endl;
    ;
}

void start_http_server_on_thread(int unused)
{
    my_server::listen(http_server_ip, http_server_port, username, password, store_folder_path, downloaded_folder_path);
}

void start_torrent_client_on_thread(int unused)
{
    my_torrent::listen();
}

int main(int argc, char *argv[])
{

    // load a pair informaton about ip and contry
    my_ip_country_detector::setup_context(setting_ipv4_cvs_path, setting_ipv6_cvs_path);

    //
    // setup table
    my_db::setup(sqlite_db_path, store_folder_path, the_range_of_time);

    //
    // setup torrent
    my_torrent::setup(torrent_bind_address, upload_max, download_max, downloaded_folder_path);

    //
    // load target info(torrent file & magnet link path) from database
    std::vector<std::shared_ptr<my_db::TargetInfo>> target_info_list;
    my_db::get_target_info(target_info_list);

    //
    // set
    std::regex re(".*\\.magnetlink");
    for (auto l : target_info_list)
    {
        if (std::regex_match(l->target, re))
        {
            //
            {
                std::fstream input(l->target);
                std::vector<char> source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
                my_torrent::add_magnetlink(l->unique_id, std::string(source.data(), source.size()));
            }
        }
        else
        {
            my_torrent::add_torrentfile(l->unique_id, l->target);
        }
    }

    // start httpserver
    server_thread = std::thread(start_http_server_on_thread, 3); //NULL);

    // start torrent
    torrent_thread = std::thread(start_torrent_client_on_thread, 3); //NULL);

    //
    // wait
    signal(SIGINT, my_sigint_handler);
    server_thread.join();
    torrent_thread.join();
    return 0;
}
