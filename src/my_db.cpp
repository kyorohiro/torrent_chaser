#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <my_db.hpp>
#include <memory>
//
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/magnet_uri.hpp>

#include <my_base_encode.hpp>

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
                "INFO TEXT"
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
    void save_torrent_file(const char *binary, int size)
    {
        lt::torrent_info x(binary, size);
        std::string infohash_hex = my_base_encode::encode_hex(x.info_hash().to_string());
        std::fstream outfile;

        std::string unique_id = my_base_encode::generate_sha1_string(std::string(binary,size));
        std::string path = _torrent_file_root_path+"/"+unique_id+".torrent";

        outfile.open(path,  std::ios_base::out | std::ios_base::binary);
        outfile.write(binary, size);
        outfile.close();
        insert_target_info(unique_id ,path, infohash_hex, x.name());
    }

    void insert_magnetlink(std::string magnetlink)
    {
        auto link =lt::parse_magnet_uri(magnetlink);

        std::string unique_id = my_base_encode::generate_sha1_string(magnetlink);
        std::string path = _torrent_file_root_path+"/"+unique_id+".magnetlink";
        std::fstream outfile;
        outfile.open(path,  std::ios_base::out | std::ios_base::binary);
        outfile.write(magnetlink.c_str(), magnetlink.size());
        outfile.close();
        std::string infohash_hex = my_base_encode::encode_hex(link.info_hashes.get_best().to_string());

       insert_target_info(unique_id,path, infohash_hex,"");
    }

    void insert_target_info(std::string unique_id, std::string magnetlink, std::string info_hash, std::string name)
    {
        std::stringstream ss;
        ss << "INSERT INTO TARGET_INFO(UNIQUE_ID,TARGET,INFOHASH,NAME) VALUES ("
           << "'" << unique_id << "',"
           << "'" << magnetlink << "',"
           <<  "'" << info_hash << "',"
           <<  "'" << name << "'"
           <<");";

        std::string sql = ss.str();
        std::cout << sql << std::endl;
        char *zErrMsg = 0;
        int rc = sqlite3_exec(_db, sql.c_str(), callback, 0, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error(std::string(sqlite3_errmsg(_db)));
        }
    }

    void remove_magnetlink(int id)
    {
        std::stringstream ss;
        ss << "DELETE  FROM TARGET_INFO WHERE id = " << id << ";";

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
            else if (std::string(azColName[i]) == "UNIQUE_ID" )
            {
                info->unique_id = (argv[i] == NULL ? "UNIQUE_ID" : std::string(argv[i]));
            }

        }
        targetInfos->push_back(info);
        std::cout << "-------------2z"<<info->infohash << std::endl;
        printf("\n");
        return 0;
    }
    void get_magnetlink(std::vector<std::shared_ptr<TargetInfo>> &targetInfos)
    {
        std::string sql = "SELECT * FROM TARGET_INFO;";
        char *zErrMsg = 0;
        int rc = sqlite3_exec(_db, sql.c_str(), callbackGetMagnetLink, &targetInfos, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error(std::string(sqlite3_errmsg(_db)));
        }
    }
} // namespace my_db