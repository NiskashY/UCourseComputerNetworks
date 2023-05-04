#pragma once
// custom files
#include "combinatorics.h"
#include "students.h"

// socket programing
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

// json files
#include <json/json.h>
#include <json/writer.h>

class Server {
public:
    Server() {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            throw std::runtime_error("Cant create socket descriptor.");
        }

        // setup server information
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_port = htons(PORT);

        // connect socket with port
        if (bind(socket_fd, reinterpret_cast<sockaddr*>(&server_address), sizeof(server_address)) < 0) {
            throw std::runtime_error("Cant connect socket with port.");
        }

        // put server in a passive mode
        if (listen(socket_fd, MAX_QUERIES) < 0) {
            throw std::runtime_error("Cant listen to socket");
        }

        ShowMessage("Listening! Port:", PORT, "IpAddress: localhost");
    }

    ~Server() {
        close(socket_fd);
    }

    template <class T>
    auto convertStringJson(const T& str) const -> Json::Value {
        Json::Value fmt_str;
        std::stringstream input(str);
        input >> fmt_str;
        return fmt_str;
    }

    auto convertJsonString(const Json::Value& json) -> std::string {
        Json::StreamWriterBuilder builder;
        return Json::writeString(builder, json);
    }

    Json::Value ReadJson(const int& new_socket, const std::string& client_readable_ip) {
        size_t buf_size = sizeof(buffer);
        std::memset(buffer, '\0', buf_size);
        if (!read(new_socket, buffer, buf_size)) {
            throw std::runtime_error("connection lost with: " + client_readable_ip);
        }
        return convertStringJson(buffer);
    }

    [[noreturn]] void run() {
        Combinatorics comb;
        students.addStudent(students.createStudent());

        while (true) {
            int new_socket = accept(socket_fd, (sockaddr*)(&server_address), (socklen_t*)&address_length);
            if (new_socket < 0) {
                close(new_socket);
                throw std::runtime_error("Error while Accepting on socket");
            }

            std::string client_readable_ip(INET_ADDRSTRLEN, '\0');
            inet_ntop(server_address.sin_family,
                      (sockaddr*)&server_address.sin_addr,
                      client_readable_ip.data(),
                      client_readable_ip.size());

            ShowMessage("Connection established with: " + client_readable_ip);

            while (true) {
                try {
                    auto received_mark = ReadJson(new_socket, client_readable_ip);
                    auto response = students.findWithoutMark(received_mark["mark"].asInt());

                    ShowMessage(kReqSep, kRequestMsg, received_mark.toStyledString());
                    ShowMessage(kLineSep, kResponseMsg, response.toStyledString(), kReqSep);

                    auto rsp_msg = convertJsonString(response);
                    send(new_socket, rsp_msg.data(), rsp_msg.size(), 0);
                } catch (std::runtime_error& e) {
                    // connection lost
                    ShowMessage(e.what());
                    break;
                }
            }
        }
    }

private: // required for messages
    template <class T>
    void ShowMessage(const T& message) {
        std::cout << message << std::endl;
    }

    template <class T, class... Args>
    void ShowMessage(const T& message, Args... args) {
        std::cout << message << ' ';
        ShowMessage(args...);
    }

    const char * const kReqSep      = "\n==============\n";
    const char * const kLineSep     = "\n--------------\n";
    const char * const kRequestMsg  = "Request:\n";
    const char * const kResponseMsg = "Response:\n";

private: // variables
    const int PORT = 9984;      // TODO: возможность задать автоматически
    const int MAX_QUERIES = 3;

    sockaddr_in server_address{}; // server information
    const int address_length = sizeof(server_address);
    int socket_fd = 0;           // file descriptor

    Students students;   // TODO: хранить сразу в JsonCpp
    char buffer[50];    // 50 -> size of a buffer
};