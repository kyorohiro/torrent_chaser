#include <string>

const std::string username = "<YOUT USERNAME>";
const std::string password = "<YOUR PASSWORD>";

//
// https://dev.to/kyorohiro/memo-get-country-name-from-ip-address-at-ip2locationlite-2172
const std::string setting_ipv4_cvs_path = "./res/ip2country/IP2LOCATION-LITE-DB1.CSV";
const std::string setting_ipv6_cvs_path = "./res/ip2country/IP2LOCATION-LITE-DB1.IPV6.CSV";

const std::string sqlite_db_path = ".app3.db";
const std::string store_folder_path = "./data";
const std::string downloaded_folder_path = "./.data";

const int upload_max = -1;
const int download_max = -1;

const int the_range_of_time = 3 * 60 * 60; // Number of searches after the specified number of seconds

const std::string http_server_ip = "0.0.0.0";
const int http_server_port = 8080;

const std::string torrent_bind_address = ""; // "0.0.0.0:6881"