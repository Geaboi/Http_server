#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <filesystem>

const int PORT = 8080;
namespace fs = std::filesystem;

std::string parse_method(const std::string& request_line) {
    std::istringstream stream(request_line);
    std::string method;
    stream >> method;
    return method;
}

std::string parse_path(const std::string& request_line) {
    std::istringstream stream(request_line);
    std::string method, path;
    stream >> method >> path;
    return path;
}

std::string get_extension(const std::string& path) {
    size_t dot_pos = path.find_last_of(".");
    return (dot_pos != std::string::npos) ? path.substr(dot_pos + 1) : "";
}

bool isValidPath(const std::string& path) {
    return fs::exists(path);
}

void send_message(int client_socket, const std::string& filepath, const std::string& header) {
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return;
    }

    send(client_socket, header.c_str(), header.length(), 0);

    char buffer[1024];
    while (file.read(buffer, sizeof(buffer)) || file.gcount()) {
        send(client_socket, buffer, file.gcount(), 0);
    }

    file.close();
}

void handleRequest(const std::string& method, const std::string& path, const std::string& ext, std::string& header, int client_socket) {
    if (method == "GET") {
        std::cout << "Handling GET request for path: " << path << std::endl;
        if (isValidPath(path)) {
            if (path == "/" || path.empty()) {
                std::string full_path = "./src/index.html";
                header += "Content-Type: text/html\r\n\r\n";
                send_message(client_socket, full_path, header);
            }
            //Implement other get Features.
            else {
                header += "Content-Type: application/octet-stream\r\n\r\n";
                send_message(client_socket, path, header);
            }
        } else {
            std::cout << "Path is invalid." << std::endl;
        }
    } else if (method == "POST") {
        //Connect with my in house database.
        std::cout << "Handling POST request for path: " << path << std::endl;
    } else {
        std::cout << "Unsupported method: " << method << std::endl;
    }
}

int main() {
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        std::cerr << "setsockopt failed" << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    memset(&address.sin_zero, '\0', sizeof(address.sin_zero));

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return -1;
    }

    while (true) {
        std::cout << "\n+++++++ Waiting for new connection ++++++++\n\n";
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Accept failed" << std::endl;
            return -1;
        }

        if (fork() == 0) {
            close(server_fd);

            char buffer[30000] = {0};
            valread = read(new_socket, buffer, 30000);
            std::string request(buffer);

            std::cout << "Received: " << request << std::endl;

            std::string method = parse_method(request);
            std::string path = parse_path(request);
            std::string ext = get_extension(path);

            std::string header = "HTTP/1.1 200 OK\r\n";
            handleRequest(method, path, ext, header, new_socket);

            std::cout << "------------------Message sent-------------------" << std::endl;
            exit(0);
        }

        close(new_socket);
    }

    close(server_fd);
    std::cout << "Server closed" << std::endl;
    return 0;
}
