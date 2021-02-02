#ifndef _MY_DB
#define _MY_DB

#include<iostream>
#include<vector>
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
        std::string country;
        std::string dns;
        unsigned long int unixtime;
        std::string name;
        std::string info;
    };

    void setup(std::string dbpath, std::string torrent_file_root_path);
    TargetInfo  insert_magnetlink(std::string magnetlink);
    void get_magnetlink(std::vector<std::shared_ptr<TargetInfo>> &targetInfos);
    TargetInfo remove_magnetlink(int id);    
    TargetInfo  save_torrent_file(const char *binary, int size);
    void insert_found_ip(std::string name,std::string ip,std::string country,std::string dns,
            unsigned long int unixtime,std::string info);
}

#endif
