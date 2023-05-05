#pragma once

#include <thread>
#include <iostream>

class ThreadInfo {
public:
    ThreadInfo() = default;

    ThreadInfo(std::thread::id _thread_id, int port_) : thread_id(_thread_id), port(port_) {
        client_start_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    void showInfo() const {
        std::cout << "Thread id: " << thread_id << ' '
                  << "Port: " << port << ' '
                  <<  "Time: " << std::ctime(&client_start_time);
    }

private:
    std::thread::id thread_id;
    std::time_t client_start_time{};
    int port = 0;
};
