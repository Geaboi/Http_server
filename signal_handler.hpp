#pragma once
#include <csignal>

extern volatile std::sig_atomic_t stop_server;

void setup_signal_handler();