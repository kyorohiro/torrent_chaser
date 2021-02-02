#include <httplib.h>
#include <my_server.hpp>

#include <iostream>
#include <sstream>
#include <iterator>
#include <exception>
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
#include <my_ip_country_detector.hpp>
#include <my_db.hpp>
#include <my_torrent.hpp>


namespace my_server
{
    std::string password;
    std::string username;

    std::string decodeBase64(const std::string &val);
    std::string encodeBase64(const std::string &val);

    httplib::Server _http_server;

    void save() {
        auto ip_list_map = my_torrent::ipinfo_list_map;
        for(auto i = ip_list_map .begin();i != ip_list_map.end();i++) {
            for(auto l : i->second) {
                std::string name = i->first;
                std::string ip =l->ip_address;
                std::string country = l->country;
                std::string domain = l->domain;
                //
                //my_db::
            }            
        }
    }

    void terminate() {
        _http_server.stop();
    }
    //
    // Basic Authorization
    //
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
            std::cerr << "my_server_auth err (3)" << value << std::endl;
            goto AERR;
        }
        if (m[1] != username || m[2] != password)
        {
            std::cerr << "my_server_auth err (4)" << value << std::endl;
            goto AERR;
        }
        std::cout << username << ":" << password << std::endl;
        return true;
    AERR:
        res.set_header("WWW-Authenticate", "Basic realm=\"..\", charset=\"UTF-8\"");
        res.status = 401;
        return false;
    }

    void _static_file_handle(std::string filepath, const httplib::Request &req, httplib::Response &res)
    {
        std::cout << "[request]:" << filepath << std::endl;
        if (!handleAuthCheck(req, res))
        {
            return;
        }
        std::fstream input(filepath);
        std::vector<char> source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
        res.set_content(std::string(source.data(), source.size()), "text/html");
        res.status = 200;
    }

    void listen(std::string ip, int port, std::string _username, std::string _password)
    {
        username = _username;
        password = _password;
        //
        // static file
        //
        _http_server.Get("/", [](const httplib::Request &req, httplib::Response &res) {
            _static_file_handle("./res/html/index.html", req, res);
        });

        _http_server.Get("/create_magnetlink", [](const httplib::Request &req, httplib::Response &res) {
            _static_file_handle("./res/html/create_magnetlink.html", req, res);
        });

        _http_server.Get("/magnetlink", [](const httplib::Request &req, httplib::Response &res) {
            _static_file_handle("./res/html/magnetlink.html", req, res);
        });

        _http_server.Get("/ip_check", [](const httplib::Request &req, httplib::Response &res) {
            _static_file_handle("./res/html/ip_check.html", req, res);
        });

        _http_server.Get("/current_ip_info", [](const httplib::Request &req, httplib::Response &res) {
            _static_file_handle("./res/html/current_ip_info.html", req, res);
        });

        //
        // api other
        //
        _http_server.Post("/api/get_info_from_ip", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "p:/api/get_info_from_ip" << std::endl;
            if (!handleAuthCheck(req, res))
            {
                return;
            }
            std::string b = req.body;
            nlohmann::json inp = nlohmann::json::parse(b);
            nlohmann::json o;
            o["country"] = my_ip_country_detector::find_country_from_ip(inp["ip"].get<std::string>());
            o["domain"] = my_ip_country_detector::find_dns_from_ip(inp["ip"].get<std::string>());
            res.set_content(o.dump(), "text/json");
            res.status = 200;
        });

        _http_server.Post("/api/current_ip/list", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "p:/api/current_ip/list" << std::endl;
            if (!handleAuthCheck(req, res))
            {
                return;
            }
            
            nlohmann::json o;
            auto ip_list_map = my_torrent::ipinfo_list_map;
            for(auto i = ip_list_map .begin();i != ip_list_map.end();i++) {
                o[i->first] = {};
                for(auto l : i->second) {
                    o[i->first].push_back({
                        {"ip", l->ip_address},
                        {"country", l->country},
                        {"domain", l->domain}
                    });
                }
                
            }
            res.set_content(o.dump(), "text/json");
            res.status = 200;
        });

        //
        // api magnetlink
        //
        _http_server.Post("/api/magnetlink/add", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "p:/api/add_magnet_link" << std::endl;
            if (!handleAuthCheck(req, res))
            {
                return;
            }
            std::string b = req.body;
            nlohmann::json inp = nlohmann::json::parse(b);
            nlohmann::json o;
            my_db::TargetInfo target_info = my_db::insert_magnetlink(inp["magnetlink"].get<std::string>());
            my_torrent::add_magnetlink(target_info.unique_id, inp["magnetlink"].get<std::string>());
            res.set_content(o.dump(), "text/json");
            res.status = 200;
        });

        _http_server.Post("/api/torrentfile/add", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "call /api/torrentfile/add" << std::endl;
            if (!handleAuthCheck(req, res))
            {
                return;
            }

            //
            // get torrentfile binary
            const char *buffer = req.body.c_str();
            //std::vector<char> xx(buffer, buffer + req.body.length());

            //
            // save 
            my_db::TargetInfo target_info = my_db::save_torrent_file(buffer, req.body.length());
            my_torrent::add_torrentfile(target_info.unique_id, target_info.target);

            //
            //
            nlohmann::json o;
            res.set_content(o.dump(), "text/json");
            res.status = 200;
        });

        _http_server.Post("/api/magnetlink/remove", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "call /api/magnetlink/remove" << std::endl;
            if (!handleAuthCheck(req, res))
            {
                return;
            }
            std::string b = req.body;
            nlohmann::json inp = nlohmann::json::parse(b);
            std::cout << inp.dump() << std::endl;
            nlohmann::json o;
            //
            auto target_info = my_db::remove_magnetlink(inp["id"].get<int>());
            my_torrent::remove_magnetlink(target_info.unique_id);
            res.set_content(o.dump(), "text/json");
            res.status = 200;
        });

        _http_server.Post("/api/magnetlink/create", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "call /api/magnetlink/create" << std::endl;
            if (!handleAuthCheck(req, res))
            {
                return;
            }
            const char *buffer = req.body.c_str();
            std::vector<char> xx(buffer, buffer + req.body.length());
            std::string link = my_torrent::make_magnet_link(xx);
            nlohmann::json o;
            o["magnetlink"] = link;
            res.set_content(o.dump(), "text/json");
            res.status = 200;
        });

        _http_server.Post("/api/magnetlink/list", [](const httplib::Request &req, httplib::Response &res) {
            if (!handleAuthCheck(req, res))
            {
                return;
            }
            try
            {
                std::vector<std::shared_ptr<my_db::TargetInfo>> targetInfos;

                my_db::get_magnetlink(targetInfos);
                nlohmann::json o;

                o["list"] = nlohmann::json::array();

                for (auto i : targetInfos)
                {
                    std::cout << "infohash : "<<i->infohash <<std::endl;
                    o["list"].push_back({
                        {"id", i->id},
                        {"infohash", i->infohash},
                        {"name", i->name},
                        {"target", i->target},
                    });
                }
                res.set_content(o.dump(), "text/json");
                res.status = 200;
            }
            catch (std::exception e)
            {
                std::cout << e.what() << std::endl;
            }
            //my_db::insertMagnetlink(inp["magnetlink"].get<std::string>());
        });
        _http_server.listen(ip.c_str(), port);
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