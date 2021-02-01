#include<string>

namespace my_base_encode
{
    std::string decode_base64(const std::string &val);
    std::string encode_base64(const std::string &val);
    std::string encode_hex(const std::string &val);
    std::string generate_sha1_string(std::string v);
} // namespace base_encode
