#include "http_server.hpp"
#include "http_request_handler.hpp"
#include "signal_handler.hpp"
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <sys/time.h>
#include <cerrno>

extern volatile std::sig_atomic_t stop_server;

HttpServer::HttpServer(int port, size_t thread_count)
    : port(port), addrlen(sizeof(address)), thread_pool(thread_count) {}

bool HttpServer::start() {
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return false;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    memset(&address.sin_zero, '\0', sizeof(address.sin_zero));

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return false;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "Listen failed" << std::endl;
        return false;
    }

    // Set timeout on accept()
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    std::cout << "Server is listening on port " << port << std::endl;
    while (!stop_server) {
        int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            if (stop_server) break;
            if (errno == EAGAIN || errno == EWOULDBLOCK) continue;  // Accept timeout
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue;
        }
        thread_pool.enqueue(HttpRequestHandler(new_socket));
    }

    std::cout << "Server is shutting down..." << std::endl;
    return true;
}

HttpServer::~HttpServer() {
    thread_pool.shutdown();
    close(server_fd);
}
