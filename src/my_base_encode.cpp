#include <iostream>
#include <sstream>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/algorithm/string.hpp>
//
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/uuid/name_generator_sha1.hpp>

//
namespace my_base_encode
{
    //
    // ref
    //   - https://code-examples.net/en/q/6ba0e2
    //   - https://stackoverflow.com/questions/7053538/how-do-i-encode-a-string-to-base64-using-only-boost
    //
    std::string decode_base64(const std::string &val)
    {
        using namespace boost::archive::iterators;
        using base64_txt = boost::archive::iterators::transform_width<
            binary_from_base64<std::string::const_iterator>, 8, 6>;
        std::stringstream ss;

        std::copy(
            base64_txt(val.c_str()),
            base64_txt(val.c_str() + val.size()),
            ostream_iterator<char>(ss));

        return ss.str();
    }

    std::string encode_base64(const std::string &val)
    {
        using base64_txt = boost::archive::iterators::base64_from_binary<
            boost::archive::iterators::transform_width<
                std::string::const_iterator, 6, 8>>;
        auto tmp = std::string(base64_txt(std::begin(val)), base64_txt(std::end(val)));
        return tmp.append((3 - val.size() % 3) % 3, '=');
    }

    std::string encode_hex(const std::string &val)
    {
        static char const _hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        const char *v = val.c_str();
        int size = val.size();
        std::string ret(size * 2, ' ');
        for (int i = 0; i < size; i++)
        {
            ret[i * 2] = _hex[(val[i] >> 4) & 0xf];
            ret[i * 2 + 1] = _hex[(val[i]) & 0xf];
        }
        return ret;
    }

    std::string generate_sha1_string(std::string v)
    {
        boost::uuids::detail::sha1 sha1;
        boost::uuids::detail::sha1::digest_type digest;
        char sha1string[sizeof(digest) + 1];

        sha1.process_bytes(v.c_str(), v.size());
        sha1.get_digest(digest);

        char hash[20];
        for (int i = 0; i < 5; ++i)
        {
            const char *tmp = reinterpret_cast<char *>(digest);
            hash[i * 4] = tmp[i * 4 + 3];
            hash[i * 4 + 1] = tmp[i * 4 + 2];
            hash[i * 4 + 2] = tmp[i * 4 + 1];
            hash[i * 4 + 3] = tmp[i * 4];
        }

        return encode_hex(std::string(hash, 20));
    }
} // namespace my_base_encode