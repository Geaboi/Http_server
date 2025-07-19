#include "signal_handler.hpp"
#include <csignal>
#include <iostream>
#include <atomic>
#include <sys/types.h>
#include <unistd.h>
    
volatile std::sig_atomic_t stop_server = 0;

void handle_sigint(int) {
    stop_server = 1;
    std::cout << "Server stopping..." << std::endl;
}

void setup_signal_handler() {
    std::signal(SIGINT, handle_sigint);
}