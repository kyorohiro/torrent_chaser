#include<string>

namespace my_base_encode
{
    std::string decode64(const std::string &val);
    std::string encode64(const std::string &val);
    std::string encodeHex(const std::string &val);
} // namespace base_encode
