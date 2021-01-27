#ifndef _MY_TARGET_LIST_READER
#define _MY_TARGET_LIST_READER

#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include<regex>

namespace target_list_reader {

extern std::vector<std::string> magnetlink_list;
extern std::vector<std::string> torrentfile_list;

void loadTargetListFile(std::string target_file_path);
void showDebugLog();
}

#endif //#ifndef _MY_TARGET_LIST_READER

