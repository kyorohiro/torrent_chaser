#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <my_target_list_reader.hpp>

namespace target_list_reader
{
    std::vector<std::string> magnetlink_list;
    std::vector<std::string> torrentfile_list;
    void loadTargetListFile(std::string target_file_path)
    {
        std::fstream target_list_fs(target_file_path);

        const std::string magnetlink_head = "magnet:";
        std::string line;
        std::regex re("[ \\r\\n\\t]");

        while (std::getline(target_list_fs, line))
        {
            line = std::regex_replace(line, re, "");
            if (line.size() == 0)
            {
                continue;
            }
            if (0 == line.compare(0, magnetlink_head.size(), magnetlink_head))
            {
                magnetlink_list.push_back(line);
            }
            else
            {
                torrentfile_list.push_back(line);
            }
        }
    }
    void showDebugLog()
    {
        for (std::string v : magnetlink_list)
        {
            std::cout << "m:" << v << std::endl;
        }
        for (std::string t : torrentfile_list)
        {
            std::cout << "t:" << t << std::endl;
        }
    }
} // namespace target_list_reader