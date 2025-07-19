#pragma once
#include "thread_pool.hpp"
#include <netinet/in.h>
#include <thread>
#include <vector>
#include <functional>

class HttpServer {
public:
    HttpServer(int port, size_t thread_count);
    bool start();
    ~HttpServer();

private:
    int server_fd;
    int port;
    struct sockaddr_in address;
    int addrlen;
    ThreadPool thread_pool;
};