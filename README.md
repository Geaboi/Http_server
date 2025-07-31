# Threaded C++ HTTP Server

This project is a multithreaded HTTP server built in C++17. It uses a custom thread pool and handles basic HTTP GET requests for static files. The server supports graceful shutdown via `Ctrl+C` and includes basic routing for serving HTML and images.

---

## Features

- ✅ Multi-threaded request handling using a custom thread pool
- ✅ Basic HTTP GET support (serves `.html`, `.jpg`)
- ✅ Graceful shutdown on `Ctrl+C` using `SIGINT`
- ✅ Modular codebase using object-oriented design
- ✅ Auto-cleanup of threads and resources

---

## Getting Started

### Prerequisites

- C++17-compatible compiler (e.g., `g++`, `clang++`)
- POSIX system (Linux, macOS)

### Build

```bash
g++ main.cpp src/*.cpp -o server -std=c++17
Or use a Makefile or CMakeLists.txt.

Run
./server
The server listens on port 8080 by default.

Stop
Press Ctrl+C to trigger a graceful shutdown.

Example Routes
GET / → serves src/index.html

GET /image.jpg → serves src/image.jpg

To Do
 Add support for more HTTP methods (e.g., POST)

 Support MIME type detection

 Add logging middleware

 Add file upload support

 Add config file for port and paths

