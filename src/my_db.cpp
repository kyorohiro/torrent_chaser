#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <my_db.hpp>
#include <memory>
//
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/magnet_uri.hpp>

#include <my_base_encode.hpp>
#include <cstdio>
#include <sstream>
#include <fstream>

//
namespace my_db
{
    //
    sqlite3 *_db;
    std::string _torrent_file_root_path;

    int callback(void *NotUsed, int argc, char **argv, char **azColName)
    {
        return 0;
    }
    void setup(std::string dbpath, std::string torrent_file_root_path)
    {
        _torrent_file_root_path = torrent_file_root_path;
        {
            int rc = sqlite3_open(dbpath.c_str(), &_db);
            if (rc != SQLITE_OK)
            {
                throw std::runtime_error("failed to open sqllite" + std::string(sqlite3_errmsg(_db)));
            }
        }

        //
        // FOUND_IP TABLE
        {
            std::string sql =
                "CREATE TABLE FOUND_IP("
                "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "IP CHAR(300),"
                "COUNTRY CHAR(10),"
                "DNS CHAR(300),"
                "UNIXTIME INT,"
                "NAME TEXT,"
                "INFO TEXT,"
                "TYPE TEXT,"
                "UNIQUE_ID TEXT"
                ")";
            char *zErrMsg = 0;
            int rc = sqlite3_exec(_db, sql.c_str(), callback, 0, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                // throw std::runtime_error("failed to open sqllite" + std::string(sqlite3_errmsg(db)));
            }
        }

        //
        // TARGET_INFO TABLE
        {
            std::string sql =
                "CREATE TABLE TARGET_INFO("
                "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                "UNIQUE_ID CHAR(40),"
                "TARGET TEXT UNIQUE,"
                "INFOHASH TEXT,"
                "NAME TEXT"
                ")";
            char *zErrMsg = 0;
            int rc = sqlite3_exec(_db, sql.c_str(), callback, 0, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                //throw std::runtime_error("failed to open sqllite" + std::string(sqlite3_errmsg(db)));
            }
        }
    }
    void insert_target_info(std::string unique_id, std::string magnetlink, std::string info_hash, std::string name);
    TargetInfo save_torrent_file(const char *binary, int size)
    {
        lt::torrent_info x(binary, size);
        std::string infohash_hex = my_base_encode::encode_hex(x.info_hash().to_string());
        std::fstream outfile;

        std::string unique_id = my_base_encode::generate_sha1_string(std::string(binary, size));
        std::string path = _torrent_file_root_path + "/" + unique_id + ".torrent";

        outfile.open(path, std::ios_base::out | std::ios_base::binary);
        outfile.write(binary, size);
        outfile.close();
        insert_target_info(unique_id, path, infohash_hex, x.name());

        TargetInfo info;
        info.unique_id = unique_id;
        info.target = path;
        info.infohash = infohash_hex;
        info.name = x.name();
        return info;
    }

    TargetInfo insert_magnetlink(std::string magnetlink)
    {
        auto link = lt::parse_magnet_uri(magnetlink);

        std::string unique_id = my_base_encode::generate_sha1_string(magnetlink);
        std::string path = _torrent_file_root_path + "/" + unique_id + ".magnetlink";
        std::fstream outfile;
        outfile.open(path, std::ios_base::out | std::ios_base::binary);
        outfile.write(magnetlink.c_str(), magnetlink.size());
        outfile.close();
        std::string infohash_hex = my_base_encode::encode_hex(link.info_hashes.get_best().to_string());

        insert_target_info(unique_id, path, infohash_hex, "");
        TargetInfo info;
        info.unique_id = unique_id;
        info.target = path;
        info.infohash = infohash_hex;
        info.name = "";
        return info;
    }

    void insert_target_info(std::string unique_id, std::string magnetlink, std::string info_hash, std::string name)
    {
        std::stringstream ss;
        ss << "INSERT INTO TARGET_INFO(UNIQUE_ID,TARGET,INFOHASH,NAME) VALUES ("
           << "'" << unique_id << "',"
           << "'" << magnetlink << "',"
           << "'" << info_hash << "',"
           << "'" << name << "'"
           << ");";

        std::string sql = ss.str();
        std::cout << sql << std::endl;
        char *zErrMsg = 0;
        int rc = sqlite3_exec(_db, sql.c_str(), callback, 0, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error(std::string(sqlite3_errmsg(_db)));
        }
    }

    void insert_found_ip(std::string name, std::string ip, std::string country, std::string dns,
                         unsigned long int unixtime, std::string info, 
                         std::string type, std::string unique_id)
    {
        std::stringstream ss;
        ss << "INSERT INTO FOUND_IP(IP,COUNTRY,DNS,UNIXTIME,NAME,INFO,TYPE,UNIQUE_ID) VALUES ("
           << "'" << ip << "',"
           << "'" << country << "',"
           << "'" << dns << "',"
           << "" << unixtime << ","
           << "'" << name << "',"
           << "'" << info << "',"
           << "'" << type << "',"          
           << "'" << unique_id << "'"          
           << ");";

        std::string sql = ss.str();
        std::cout << sql << std::endl;
        char *zErrMsg = 0;
        int rc = sqlite3_exec(_db, sql.c_str(), callback, 0, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error(std::string(sqlite3_errmsg(_db)));
        }
    }

    int callbackGetMagnetLink(void *context, int argc, char **argv, char **azColName)
    {
        int i;
        std::vector<std::shared_ptr<TargetInfo>> *targetInfos = (std::vector<std::shared_ptr<TargetInfo>> *)context;
        //std::cout << "callback" << *((std::string*)NotUsed)<<std::endl;

        std::cout << "-------------1z" << std::endl;
        auto info = std::make_shared<TargetInfo>();
        for (i = 0; i < argc; i++)
        {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
            if (std::string(azColName[i]) == "ID")
            {
                info->id = (argv[i] == NULL ? -1 : std::stoi(argv[i]));
            }
            else if (std::string(azColName[i]) == "TARGET")
            {
                info->target = (argv[i] == NULL ? "" : std::string(argv[i]));
            }
            else if (std::string(azColName[i]) == "INFOHASH")
            {
                info->infohash = (argv[i] == NULL ? "" : std::string(argv[i]));
            }
            else if (std::string(azColName[i]) == "NAME")
            {
                info->name = (argv[i] == NULL ? "" : std::string(argv[i]));
            }
            else if (std::string(azColName[i]) == "UNIQUE_ID")
            {
                info->unique_id = (argv[i] == NULL ? "UNIQUE_ID" : std::string(argv[i]));
            }
        }
        targetInfos->push_back(info);
        std::cout << "-------------2z" << info->infohash << std::endl;
        printf("\n");
        return 0;
    }

    TargetInfo remove_magnetlink(int id)
    {
        //
        std::vector<std::shared_ptr<TargetInfo>> targetInfos;
        {
            std::stringstream ss;
            ss << "SELECT * FROM TARGET_INFO WHERE id = " << id << ";";
            std::string sql = ss.str();
            char *zErrMsg = 0;
            int rc = sqlite3_exec(_db, sql.c_str(), callbackGetMagnetLink, &targetInfos, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                throw std::runtime_error(std::string(sqlite3_errmsg(_db)));
            }
        }
        if (targetInfos.size() == 0)
        {
            // not found
            return TargetInfo();
        }
        {
            std::stringstream ss;
            ss << "DELETE FROM TARGET_INFO WHERE id = " << id << ";";

            std::string sql = ss.str();
            std::cout << ">>" << sql << std::endl;
            char *zErrMsg = 0;
            int rc = sqlite3_exec(_db, sql.c_str(), callback, 0, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                std::cout << sqlite3_errmsg(_db) << std::endl;
                throw std::runtime_error(std::string(sqlite3_errmsg(_db)));
            }
        }
        {
            std::string target = targetInfos[0]->target;
            std::string unique_id = targetInfos[0]->unique_id;
            std::remove(target.c_str());
        }
        TargetInfo info;
        info.unique_id = targetInfos[0]->unique_id;
        info.target = targetInfos[0]->target;
        info.infohash = targetInfos[0]->infohash;
        info.name = targetInfos[0]->name;
        return info;
    }

    void get_target_info(std::vector<std::shared_ptr<TargetInfo>> &targetInfos)
    {
        std::string sql = "SELECT * FROM TARGET_INFO;";
        char *zErrMsg = 0;
        int rc = sqlite3_exec(_db, sql.c_str(), callbackGetMagnetLink, &targetInfos, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error(std::string(sqlite3_errmsg(_db)));
        }
    }

    int callbackGetPeerInfo(void *context, int argc, char **argv, char **azColName)
    {
        int i;
        std::vector<std::shared_ptr<FoundIp>> *targetInfos = (std::vector<std::shared_ptr<FoundIp>> *)context;
        //std::cout << "callback" << *((std::string*)NotUsed)<<std::endl;

        auto info = std::make_shared<FoundIp>();
        for (i = 0; i < argc; i++)
        {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
            if (std::string(azColName[i]) == "ID")
            {
                info->id = (argv[i] == NULL ? -1 : std::stoi(argv[i]));
            }
            else if (std::string(azColName[i]) == "DNS")
            {
                info->dns = (argv[i] == NULL ? "" : std::string(argv[i]));
            }
            else if (std::string(azColName[i]) == "UNIXTIME")
            {
                info->unixtime= (argv[i] == NULL ? -1 : std::stoi(argv[i]));
            }
            else if (std::string(azColName[i]) == "IP")
            {
                info->ip = (argv[i] == NULL ? "" : std::string(argv[i]));
            }
            else if (std::string(azColName[i]) == "COUNTRY")
            {
                info->country = (argv[i] == NULL ? "" : std::string(argv[i]));
            }
            else if (std::string(azColName[i]) == "NAME")
            {
                info->name = (argv[i] == NULL ? "" : std::string(argv[i]));
            }
            else if (std::string(azColName[i]) == "INFO")
            {
                info->info = (argv[i] == NULL ? "" : std::string(argv[i]));
            }
            else if (std::string(azColName[i]) == "TYPE")
            {
                info->type = (argv[i] == NULL ? "" : std::string(argv[i]));
            }
            else if (std::string(azColName[i]) == "UNIQUE_ID")
            {
                info->unique_id = (argv[i] == NULL ? "" : std::string(argv[i]));
            }
            
        }
        targetInfos->push_back(info);
        printf("\n");
        return 0;
    }

    void get_peer_info(std::vector<std::shared_ptr<FoundIp>> &targetInfos, int idmin, int limit, std::string country)
    {
        std::cout << idmin << "," << limit << "," << country << std::endl;
        std::stringstream ss;
        ss << "SELECT * FROM FOUND_IP WHERE ID >= " << idmin << " ";
        if(country.length() != 0) {
            ss << " AND COUNTRY = '"<< country << "' ";
        }
        ss << " LIMIT " << limit << " ";        
        ss << ";";
        std::string sql = ss.str();
        std::cout << sql << std::endl;

        char *zErrMsg = 0;
        int rc = sqlite3_exec(_db, sql.c_str(), callbackGetPeerInfo, &targetInfos, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error(std::string(sqlite3_errmsg(_db)));
        }
    }

} // namespace my_db