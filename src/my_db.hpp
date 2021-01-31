#ifndef _MY_DB
#define _MY_DB

#include<iostream>
#include<vector>
#include <memory>

namespace my_db {
    struct TargetInfo {
        int id;
        std::string target;
        std::string infohash;
        std::string name;
    };
    void setup(std::string dbpath);
    void insert_magnetlink(std::string magnetlink);
    void get_magnetlink(std::vector<std::shared_ptr<TargetInfo>> &targetInfos);
    void remove_magnetlink(int id);
}

#endif
