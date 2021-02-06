#ifndef _MY_DB
#define _MY_DB

#include <iostream>
#include <vector>
#include <memory>

//
// Stock peer's ip infomation and target metainfo on SQLITE DB
//
namespace my_db {

    struct TargetInfo {
        int id;
        std::string unique_id;
        std::string target;
        std::string infohash;
        std::string name;
    };

    struct FoundIp {
        int id;
        std::string ip;
        int port;
        std::string country;
        std::string dns;
        unsigned long int unixtime;
        std::string name;
        std::string info;
        std::string type;
        std::string unique_id;
    };

    void setup(std::string dbpath, std::string torrent_file_root_path, int the_range_of_time);
    TargetInfo  insert_magnetlink(std::string magnetlink);
    void get_target_info(std::vector<std::shared_ptr<TargetInfo>> &targetInfos);
    void get_peer_info(std::vector<std::shared_ptr<FoundIp>> &targetInfos, int idmin, int limit, std::string country, std::string ip, int from_sec_to_now);

    TargetInfo remove_magnetlink(int id);    
    TargetInfo  save_torrent_file(const char *binary, int size);
    void insert_found_ip(std::string name,std::string ip, int port, std::string country,std::string dns,
            unsigned long int unixtime,std::string info, std::string type, std::string unique_id);      
     bool alreadtExist(std::string ip, int port, std::string type);
}

#endif
