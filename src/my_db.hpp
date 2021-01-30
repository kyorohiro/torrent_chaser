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
    void setupDB(std::string dbpath);
    void insertMagnetlink(std::string magnetlink);
    void getMagnetLink(std::vector<std::shared_ptr<TargetInfo>> &targetInfos);
}