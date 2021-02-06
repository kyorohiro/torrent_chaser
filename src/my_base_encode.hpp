#include<string>

namespace my_base_encode
{
    std::string decode_base64(const std::string &val);
    std::string encode_base64(const std::string &val);
    std::string encode_hex(const std::string &val);
    std::string generate_sha1_string(std::string v);
    std::string encode_url(std::string s, bool handled_space, bool handled_2f);
    std::string decode_url(std::string s);
} // namespace base_encode
