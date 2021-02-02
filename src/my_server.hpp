#ifndef _MY_SERVER
#define _MY_SERVER

#include<iostream>

namespace my_server {
    void listen(std::string ip, int port, std::string _username, std::string _password);
    void terminate();
}


#endif

