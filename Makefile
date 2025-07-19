Make:
	g++ main.cpp http_server.cpp thread_pool.cpp http_request_handler.cpp signal_handler.cpp -o main -std=c++17

run: Make
	./mainmake