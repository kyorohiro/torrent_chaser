#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <my_db.hpp>
#include <memory>

namespace my_db
{
    sqlite3 *db;
    int callback(void *NotUsed, int argc, char **argv, char **azColName)
    {
        int i;
        //std::cout << "callback" << *((std::string*)NotUsed)<<std::endl;
        for (i = 0; i < argc; i++)
        {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
    }
    void setupDB(std::string dbpath)
    {
        {
            int rc = sqlite3_open(dbpath.c_str(), &db);
            if (rc != SQLITE_OK)
            {
                throw std::runtime_error("failed to open sqllite" + std::string(sqlite3_errmsg(db)));
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
            int rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
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
                "TARGET TEXT UNIQUE,"
                "INFOHASH TEXT,"
                "NAME TEXT"
                ")";
            char *zErrMsg = 0;
            int rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
            if (rc != SQLITE_OK)
            {
                //throw std::runtime_error("failed to open sqllite" + std::string(sqlite3_errmsg(db)));
            }
        }
    }
    void insertMagnetlink(std::string magnetlink)
    {
        std::stringstream ss;
        ss << "INSERT INTO TARGET_INFO(TARGET) VALUES ("<<"'"<<magnetlink<<"');";

        std::string sql = ss.str();
        char *zErrMsg = 0;
        int rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error(std::string(sqlite3_errmsg(db)));
        }
    }

    int callbackGetMagnetLink(void *context, int argc, char **argv, char **azColName)
    {
        int i;
        std::vector<std::shared_ptr<TargetInfo>> *targetInfos =(std::vector<std::shared_ptr<TargetInfo>>*) context;
        //std::cout << "callback" << *((std::string*)NotUsed)<<std::endl;

 std::cout << "-------------1z" << std::endl;
        auto info = std::make_shared<TargetInfo>();
        for (i = 0; i < argc; i++)
        {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
            if(std::string(azColName[i]) == "ID") {
                info->id = (argv[i] == NULL? -1: std::stoi(argv[i]));
            }
            if(std::string(azColName[i]) == "TARGET") {
                info->target = (argv[i] == NULL? "": std::string(argv[i]));
            }
            else if(std::string(azColName[i]) == "INFOHASH") {
                info->infohash = (argv[i] == NULL? "": std::string(argv[i]));
            }
            else if(std::string(azColName[i]) == "NAME") {
                info->name = (argv[i] == NULL? "": std::string(argv[i]));
            }
        }
        targetInfos->push_back(info);
         std::cout << "-------------2z" << std::endl;
        printf("\n");
        return 0;
    }
    void getMagnetLink(std::vector<std::shared_ptr<TargetInfo>> &targetInfos)
    {
        std::string sql = "SELECT * FROM TARGET_INFO;";
        char *zErrMsg = 0;
std::cout << "-------------3a" << std::endl;
        int rc = sqlite3_exec(db, sql.c_str(), callbackGetMagnetLink, &targetInfos, &zErrMsg);
        if (rc != SQLITE_OK)
        {
             std::cout << "-------------3b" << std::endl;

            throw std::runtime_error(std::string(sqlite3_errmsg(db)));
        }
         std::cout << "-------------3c" << std::endl;
    }
} // namespace my_db