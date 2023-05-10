#pragma once
#include <fstream>
#include <chrono>

class Log {
public:
    Log() {
        out.open("logs.txt", std::ios::app);
    }
    explicit Log(const char* filename_) {
        out.open(filename_, std::ios::app);
    }

    template <class T>
    void ShowMessage(const T& message) {
        out << message;
        auto end = std::chrono::system_clock::now();
        auto end_time = std::chrono::system_clock::to_time_t(end);
        out << " | " << std::ctime(&end_time) << std::endl;
    }

    template <class T, class... Args>
    void ShowMessage(const T& message, Args... args) {
        out << message << ' ';
        ShowMessage(args...);
    }

public: // variables
    constexpr static auto const kReqSep      = "==============";
    constexpr static auto const kLineSep     = "--------------";
    constexpr static auto const kRequestMsg  = "\nRequest:";
    constexpr static auto const kResponseMsg = "\nResponse:";
private:
    std::ofstream out;
};