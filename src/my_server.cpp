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
#include <my_base_encode.hpp>
#include <boost/filesystem.hpp>
namespace my_server
{
    std::string password;
    std::string username;

    httplib::Server _http_server;
void file_manager(std::string base_url_path, std::string base_file_path, const httplib::Request &req, httplib::Response &res);
    void terminate()
    {
        _http_server.stop();
        //std::this_thread::yield();
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
        value = my_base_encode::decode_base64(m[1].str());
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
    try
    {
        if (!handleAuthCheck(req, res))
        {
            return;
        }
        std::fstream input(filepath);
        std::vector<char> source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
        res.set_content(std::string(source.data(), source.size()), "text/html");
        res.status = 200;
    }
    catch (...)
    {
        std::cerr << "error at _static_file_handle" << filepath << std::endl;
        res.status = 501;
    }

    int is_regular_file(const char *path);

    void listen(std::string ip, int port, std::string _username, std::string _password)
    {
        username = _username;
        password = _password;
        //
        // static file
        //
        _http_server.Get("/", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "[request]: "
                      << "/" << std::endl;
            _static_file_handle("./res/html/index.html", req, res);
        });

        _http_server.Get("/create_magnetlink", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "[request]: "
                      << "/create_magnetlink" << std::endl;
            _static_file_handle("./res/html/create_magnetlink.html", req, res);
        });

        _http_server.Get("/magnetlink", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "[request]: "
                      << "/magnetlink" << std::endl;
            _static_file_handle("./res/html/magnetlink.html", req, res);
        });

        _http_server.Get("/ip_check", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "[request]: "
                      << "/ip_check" << std::endl;
            _static_file_handle("./res/html/ip_check.html", req, res);
        });

        _http_server.Get("/history_ip_info", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "[request]: "
                      << "/history_ip_info" << std::endl;
            _static_file_handle("./res/html/history_ip_info.html", req, res);
        });

        //
        // api other
        //
        _http_server.Post("/api/get_info_from_ip", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "[request]: /api/get_info_from_ip" << std::endl;
            try
            {
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
            }
            catch (...)
            {
                std::cerr << "error at /api/get_info_from_ip " << req.body << std::endl;
                res.status = 501;
            }
        });

        _http_server.Post("/api/history_ip/list", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "[request]: /api/history_ip/list" << std::endl;
            if (!handleAuthCheck(req, res))
            {
                return;
            }

            try
            {
                std::vector<std::shared_ptr<my_db::FoundIp>> targetInfos;
                std::string b = req.body;
                nlohmann::json inp = nlohmann::json::parse(b);
                int idmin = inp["idmin"].get<int>();
                int limit = inp["limit"].get<int>();
                std::string country = inp["country"].get<std::string>();
                std::string ip = inp["ip"].get<std::string>();
                int from_sec_to_now = inp["from_sec_to_now"].get<int>();

                get_peer_info(targetInfos, idmin, limit, country, ip, from_sec_to_now);

                nlohmann::json o;
                for (auto l : targetInfos)
                {
                    o["history"].push_back({{"ip", l->ip},
                                            {"port", l->port},
                                            {"country", l->country},
                                            {"domain", l->dns},
                                            {"name", l->name},
                                            {"unixtime", l->unixtime},
                                            {"id", l->id},
                                            {"info", l->info},
                                            {"type", l->type},
                                            {"unique_id", l->unique_id}

                    });
                }
                res.set_content(o.dump(), "text/json");

                res.status = 200;
            }
            catch (...)
            {
                std::cerr << "error at /api/history_ip/list" << req.body << std::endl;
                res.status = 501;
            }
        });

        //
        // api magnetlink
        //
        _http_server.Post("/api/magnetlink/add", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "[request]: /api/add_magnet_link" << std::endl;
            try
            {
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
            }
            catch (...)
            {
                std::cerr << "error at /api/magnetlink/add" << req.body << std::endl;
                res.status = 501;
            }
        });

        _http_server.Post("/api/torrentfile/add", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "[request]: /api/torrentfile/add" << std::endl;
            try
            {
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
            }
            catch (...)
            {
                std::cerr << "error at /api/torrentfile/add" << req.body << std::endl;
                res.status = 501;
            }
        });

        _http_server.Post("/api/magnetlink/remove", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "[request]: /api/magnetlink/remove" << std::endl;
            try
            {
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
            }
            catch (...)
            {
                std::cerr << "error at /api/magnetlink/remove" << req.body << std::endl;
                res.status = 501;
            }
        });

        _http_server.Post("/api/magnetlink/create", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "[request]: /api/magnetlink/create" << std::endl;
            try
            {
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
            }
            catch (...)
            {
                std::cerr << "error at /api/magnetlink/create" << req.body << std::endl;
                res.status = 501;
            }
        });

        _http_server.Post("/api/magnetlink/list", [](const httplib::Request &req, httplib::Response &res) {
            std::cout << "[request]: /api/magnetlink/list" << std::endl;

            try
            {
                if (!handleAuthCheck(req, res))
                {
                    return;
                }
                std::vector<std::shared_ptr<my_db::TargetInfo>> targetInfos;

                my_db::get_target_info(targetInfos);
                nlohmann::json o;

                o["list"] = nlohmann::json::array();

                for (auto i : targetInfos)
                {
                    std::cout << "infohash : " << i->infohash << std::endl;
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
            catch (...)
            {
                std::cerr << "error at /api/magnetlink/list" << req.body << std::endl;
                res.status = 501;
            }
        });

        //
        // direct file tree
        //
        _http_server.Get("/downloaded/.*", [](const httplib::Request &req, httplib::Response &res) {
            file_manager("downloaded", "./.data", req, res);
        });
        _http_server.Get("/target/.*", [](const httplib::Request &req, httplib::Response &res) {
            file_manager("target", "./data", req, res);
        });
        //

        _http_server.listen(ip.c_str(), port);
    }

    int is_regular_file(const char *path)
    {
        struct stat statbuf;
        if (stat(path, &statbuf) != 0)
            return 0;
        return S_ISDIR(statbuf.st_mode);
    }

    // base_url_path = mm
    // base_file_path := ./.data
    void file_manager(std::string base_url_path, std::string base_file_path, const httplib::Request &req, httplib::Response &res)
    {
        if (!handleAuthCheck(req, res))
        {
            return;
        }
        std::regex re("\\/" + base_url_path + "\\/(.*)");
        std::smatch smatch;
        if (!std::regex_match(req.path, smatch, re))
        {
            res.set_content("wrong path " + req.path, "text/html");
            return;
        }
        std::string path = smatch[1].str();

        if (path.length() != 0 && path[0] == '/')
        {
            res.set_content("wrong path " + req.path, "text/html");
            return;
        }
        std::regex re2(".*\\.\\.\\/.*");
        if (std::regex_match(path, re2))
        {
            res.set_content("wrong path " + req.path, "text/html");
            return;
        }
        namespace fs = boost::filesystem;

        fs::path p(base_file_path + "/" + path);
        //
        if (fs::is_regular_file(p))
        {
            const std::string path = p.string();
            res.set_chunked_content_provider(
                "application/octet-stream", //
                [path](size_t s, httplib::DataSink &sink) {
                    int buffer_size = 1024 * 256;
                    char *buffer = new char[buffer_size];
                    std::ifstream input(path);
                    while (!input.eof())
                    {
                        input.read(buffer, buffer_size);
                        int length = input.gcount();
                        if (length == 0)
                        {
                            break;
                        }
                        sink.write(buffer, input.gcount());
                    }
                    sink.done();
                    delete buffer;
                    return true;
                });
        }
        else
        {
            fs::directory_iterator itr(p);
            fs::directory_iterator end_itr;

            std::stringstream ss;
            std::regex re3(base_file_path + "/(.*)");
            std::smatch smatch3;
            for (; itr != end_itr; ++itr)
            {
                ss << itr->path().c_str() << "<br>";
                std::string filename = itr->path().filename().string();
                ss << "<a href=\""
                   << "/" + base_url_path + "/" << my_base_encode::encode_url(path + (path.size() == 0 ? "" : "/") + filename, false, true) << "\">" << filename << "</a><br>";
            }
            res.set_content(ss.str(), "text/html");
        }
        return;
    }
} // namespace my_server