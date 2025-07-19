#include <iostream>
#include <string>
#include "http_server.hpp"
#include <csignal>
#include <sys/time.h>
#include "signal_handler.hpp"

constexpr int PORT = 8080;


int main() {
    setup_signal_handler();  
    HttpServer server(PORT, std::thread::hardware_concurrency());
    if (!server.start()) {
        std::cerr << "Failed to start server." << std::endl;
        return 1;
    }
    return 0;
}