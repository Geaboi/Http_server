#pragma once
#include <string>

class HttpRequestHandler {
public:
    explicit HttpRequestHandler(int socket);
    void operator()();

private:
    int client_socket;
    static std::string parse_method(const std::string& request_line);
    static std::string parse_path(const std::string& request_line);
    static std::string get_extension(const std::string& path);
    static bool is_valid_path(const std::string& path);
    void send_file(const std::string& filepath, const std::string& header);
    void handle_request(const std::string& method, const std::string& path, const std::string& ext, std::string& header);
};
