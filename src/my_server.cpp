#include <httplib.h>
#include <my_server.hpp>

#include <iostream>
#include <sstream>
#include <iterator>
//
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/algorithm/string.hpp>
#include <regex>
#include <nlohmann/json.hpp>

//
//
#include<my_ip_country_detector.hpp>
namespace my_server
{
    std::string password;
    std::string username;

    std::string decodeBase64(const std::string &val);
    std::string encodeBase64(const std::string &val);

    httplib::Server http_server;

    bool handleAuthCheck(const httplib::Request &req, httplib::Response &res)
    {
        std::string value = req.get_header_value("Authorization");
        const std::regex reA(".*Basic[ \t]*([a-zA-Z0-9+/]*)[=]*");
        const std::regex reB("(.*):(.*)");
        std::smatch m;
        if (value == "")
        {
            std::cerr << "my_server_auth err (1)" << std::endl;
            goto AERR;
        }
        std::regex_match(value, m, reA);
        if (m.size() < 1)
        {
            std::cerr << "my_server_auth err (2)" << std::endl;
            goto AERR;
        }
        value = decodeBase64(m[1].str());
        std::regex_match(value, m, reB);
        if (m.size() < 2)
        {
            std::cerr << "my_server_auth err (3)"  << value << std::endl;
            goto AERR;
        }
        if (m[1] != username || m[2] != password)
        {
            std::cerr << "my_server_auth err (4)"  << value << std::endl;
            goto AERR;
        }
        std::cout << username << ":" <<  password << std::endl;
        return true;
    AERR:
        res.set_header("WWW-Authenticate", "Basic realm=\"..\", charset=\"UTF-8\"");
        res.status = 401;
        return false;
    }

    void listen(std::string ip, int port, std::string _username, std::string _password) {
        username = _username;
        password = _password;
        //
        http_server.Get("/", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "p:/" << std::endl;
            if (!handleAuthCheck(req, res))
            {
                return;
            }
            std::fstream input("./dat/html/index.html");
            std::vector<char> source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
            
            res.set_content(source.data(), "text/html");
            res.status = 200;
        });

        // todo
        http_server.Post("/api/get_hostname_from_ip", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "p:/api/get_hostname_from_ip" << std::endl;
            if (!handleAuthCheck(req, res))
            {
                std::cout << "--" << std::endl;
                return;
            }
             std::cout << "-1" << std::endl;
            std::string b = req.body;
             std::cout << "-2" << std::endl;

             std::cout << b << std::endl;
             std::cout << "3-1" << std::endl;

            nlohmann::json inp= nlohmann::json::parse(b);
             std::cout << "-+" << std::endl;
            //j["ip"]
            nlohmann::json o;
             std::cout << inp["ip"] << std::endl;
            o["country"] = "JA";// my_ip_country_detector::findCountryFromIPv4("8.8.8.8");//inp["ip"]);
            std::cout << inp["ip"] << std::endl;
            std::cout << o["country"] << std::endl;
            //res.set_content(o.dump(), "text/json");
            res.status = 200;
        });
        http_server.listen(ip.c_str(), port);
    }

    //
    // ref
    //   - https://code-examples.net/en/q/6ba0e2
    //   - https://stackoverflow.com/questions/7053538/how-do-i-encode-a-string-to-base64-using-only-boost
    //
    std::string decodeBase64(const std::string &val)
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

    std::string encodeBase64(const std::string &val)
    {
        using base64_txt = boost::archive::iterators::base64_from_binary<
            boost::archive::iterators::transform_width<
                std::string::const_iterator, 6, 8>>;
        auto tmp = std::string(base64_txt(std::begin(val)), base64_txt(std::end(val)));
        return tmp.append((3 - val.size() % 3) % 3, '=');
    }
} // namespace my_server