#include "http_request_handler.hpp"
#include <netinet/in.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

namespace fs = std::filesystem;

HttpRequestHandler::HttpRequestHandler(int socket) : client_socket(socket) {}

void HttpRequestHandler::operator()() {
    struct timeval timeout = {5, 0};
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    char buffer[30000] = {0};
    long valread = read(client_socket, buffer, sizeof(buffer));
    if (valread <= 0) {
        close(client_socket);
        return;
    }

    std::string request(buffer);
    std::string method = parse_method(request);
    std::string path = parse_path(request);
    std::string ext = get_extension(path);
    std::string header = "HTTP/1.1 200 OK\r\nConnection: close\r\n";

    handle_request(method, path, ext, header);
    close(client_socket);
}

std::string HttpRequestHandler::parse_method(const std::string& request_line) {
    std::istringstream stream(request_line);
    std::string method;
    stream >> method;
    return method;
}

std::string HttpRequestHandler::parse_path(const std::string& request_line) {
    std::istringstream stream(request_line);
    std::string method, path;
    stream >> method >> path;
    return path;
}

std::string HttpRequestHandler::get_extension(const std::string& path) {
    size_t dot_pos = path.find_last_of(".");
    return (dot_pos != std::string::npos) ? path.substr(dot_pos + 1) : "";
}

bool HttpRequestHandler::is_valid_path(const std::string& path) {
    return fs::exists(path);
}

void HttpRequestHandler::send_file(const std::string& filepath, const std::string& header) {
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file.is_open()) return;

    send(client_socket, header.c_str(), header.length(), 0);
    char buffer[1024];
    while (file.read(buffer, sizeof(buffer)) || file.gcount()) {
        send(client_socket, buffer, file.gcount(), 0);
    }
}

void HttpRequestHandler::handle_request(const std::string& method, const std::string& path, const std::string& ext, std::string& header) {
    if (method == "GET") {
        std::cout << "Handling GET request for path: " << path << std::endl;
        if (path == "/" || path.empty()) {
            std::string full_path = "./src/index.html";
            header += "Content-Type: text/html\r\n\r\n";
            send_file(full_path, header);
        } else if ((ext == "jpg" || ext == "JPG") && is_valid_path("./src/image.jpg")) {
            std::string full_path = "./src/image.jpg";
            header += "Content-Type: image/jpeg\r\n\r\n";
            send_file(full_path, header);
        } else {
            std::string not_found = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
            send(client_socket, not_found.c_str(), not_found.length(), 0);
        }
    }
}
