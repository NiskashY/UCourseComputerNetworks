#include <iostream>
#include <json/value.h>
#include <stdexcept>

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

        std::cout << "Listening! Port: " << PORT << " IpAddress: localhost" << std::endl;
    }

    ~Server() {
        close(socket_fd);
    }


    Json::Value convertStringJson(const std::string& str) const {
        Json::Value fmt_str;
        std::stringstream input(str);
        input >> fmt_str;
        return fmt_str;
    }

    Json::Value ReadJson(const int& new_socket, const std::string& client_readable_ip) const {
        // read size than read string
        uint32_t msg_size = 0; 
        if (!read(new_socket, &msg_size, sizeof(msg_size))) {
            throw std::runtime_error("connection lost with: " + client_readable_ip);
        }
        msg_size = ntohl(msg_size);

        std::string received_msg(msg_size + 1, '\0');
        if (!read(new_socket, received_msg.data(), received_msg.size())) {
            throw std::runtime_error("connection lost with: " + client_readable_ip);
        }
        return convertStringJson(received_msg);
    }


    std::string convertJsonString(const Json::Value& json) {
        Json::StreamWriterBuilder builder;
        return Json::writeString(builder, json);
    }


    [[noreturn]] void run() {
        Combinatorics comb;

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

            std::cout << "Connection established with: " + client_readable_ip << std::endl;

            while (true) {
                try {
                    auto received_mark = ReadJson(new_socket, client_readable_ip);
                    Json::Value response; response["students"] = "[]";

                    ShowMessage("\n--------\n", kRequestMsg, received_mark.toStyledString());
                    
                    auto rsp_msg = convertJsonString(response);
                    uint32_t msg_sz = htonl(rsp_msg.size());
                    send(new_socket, &msg_sz, sizeof(msg_sz), 0);
                    send(new_socket, rsp_msg.data(), msg_sz, 0);
                } catch (std::runtime_error& e) {
                    // connection lost
                    std::cout << e.what() << std::endl;
                    break;
                }
            }
        }
    }

private: // functions
    template <class T>
    void ShowMessage(const T& message) {
        std::cout << message << std::endl;
    }

    template <class T, class... Args>
    void ShowMessage(const T& message, Args... args) {
        std::cout << message << ' ';  
        ShowMessage(args...);
    }

private: // variables 
    const int PORT = 9984;      // TODO: возможность задать автоматически
    const int MAX_QUERIES = 3;

    sockaddr_in server_address{}; // server information
    const int address_length = sizeof(server_address);
    int socket_fd = 0;           // file descriptor
    const std::string kRequestMsg = "Request:";
    const std::string kResponseMsg = "Response:";

    Students students;
};


int main() {
    try {
        Server server;
        server.run();
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
