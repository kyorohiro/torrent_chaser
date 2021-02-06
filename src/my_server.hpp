#ifndef _MY_SERVER
#define _MY_SERVER

#include <iostream>

namespace my_server
{
    void listen(std::string ip, int port, std::string username, std::string password, std::string target_file_path, std::string downloaded_file_path);

    void terminate();
} // namespace my_server

#endif
