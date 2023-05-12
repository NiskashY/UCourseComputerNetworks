#pragma once

#include <unordered_map>

// multithreading
#include <mutex>
#include <thread>

// custom files
#include "students.h"
#include "log.h"
#include "thread_info.h"

// socket programing
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
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

        log.ShowMessage("\nListening! Port:", PORT, "IpAddress: localhost");
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


    auto getClientIp() -> std::string {
        std::string client_readable_ip(INET_ADDRSTRLEN, '\0');

        inet_ntop(server_address.sin_family,
                  (sockaddr*)&server_address.sin_addr,
                  client_readable_ip.data(),
                  client_readable_ip.size());

        while (!client_readable_ip.empty() && client_readable_ip.back() == '\0') {
            client_readable_ip.pop_back();
        } 

        return client_readable_ip;
    }


    void ProcessNewClientConnection(int new_socket, int port) {
        auto client_readable_ip = getClientIp();

        log.ShowMessage("Connection established with: " + client_readable_ip);
        
        {
            std::lock_guard<std::mutex> lock(connects_mut);
            auto id = std::this_thread::get_id();
            connections_info[id] = ThreadInfo(id, port);
        }


        while (true) {
            try {
                auto received_mark = ReadJson(new_socket, client_readable_ip);
                Json::Value response;
                {
                    // TODO:
                    const std::lock_guard<std::mutex> lock(students_mut);
                    response = students.findWithoutMark(received_mark["mark"].asInt());
                }

                log.ShowMessage(Log::kReqSep, Log::kRequestMsg, received_mark.toStyledString(),
                                Log::kLineSep, Log::kResponseMsg, response.toStyledString(), Log::kReqSep);
                auto rsp_msg = convertJsonString(response);
                send(new_socket, rsp_msg.data(), rsp_msg.size(), 0);
            } catch (std::runtime_error& e) {
                // connection lost
                log.ShowMessage(e.what());
                break;
            }
        }

        {
            std::lock_guard<std::mutex> lock(connects_mut);
            connections_info.erase(std::this_thread::get_id());
        }
    }

    [[noreturn]] void ServerCommunication() {
        while (true) {
            int new_socket = accept(socket_fd, (sockaddr*)(&server_address), (socklen_t*)&address_length);
            if (new_socket < 0) {
                close(new_socket);
                throw std::runtime_error("Error while Accepting on socket");
            }

            std::thread new_client_connection(&Server::ProcessNewClientConnection, this, new_socket, htons(server_address.sin_port));
            new_client_connection.detach();
       }
    }

    auto ShowMenu(const auto& menu) -> std::string {
        for (auto item : menu) {
            std::cout << item << std::endl;
        }
        std::string choice;
        std::cout << "Make a choice: ";
        std::getline(std::cin, choice);
        return choice;
    }

    void ShowAppearance() {
        const std::vector<const char *> menu = {
                "1 - Show current students",
                "2 - Add one more student",
                "3 - Remove last student",
                "4 - Show active clients",  // TODO:
                "else - exit"
        };
        while (true) {
            system("clear");
            auto choice = ShowMenu(menu);

            if (choice.size() == 1 && 0 < choice[0] - '0' && choice[0] - '0' <= 3) {
                const std::lock_guard<std::mutex> lock(students_mut);
                switch(choice[0] - '0') {
                    case 1: {
                        int index = 0;
                        students.visitor([&]<class T>(T& item) {
                            std::cout << '\n' << ++index << ".\n" << item << '\n';
                        });
                        break;
                    }
                    case 2: {
                        students.addStudent(students.createStudent());
                        break;
                    }
                    case 3: {
                        students.removeLastStudent();
                        break;
                    }
                }
            } else if (choice == "4") {
                const std::lock_guard<std::mutex> lock(connects_mut);
                if (connections_info.empty()) {
                    std::cout << "No connections are available" << std::endl;
                }
                for (auto& [id, info] : connections_info) {
                    info.showInfo();
                }
            } else {
                break;
            }
            std::getline(std::cin, choice); // ~ system("pause");
        }
    }

    void run() {
        std::thread appearance_thread(&Server::ShowAppearance, this);
        std::thread server_communication_thread(&Server::ServerCommunication, this);

        appearance_thread.join();
        server_communication_thread.join();
    }

private: // required for messages

    Log log;

private: // variables
    const int PORT = 9984; 
    const int MAX_QUERIES = 3;

    sockaddr_in server_address{}; // server information
    const int address_length = sizeof(server_address);
    int socket_fd = 0;           // file descriptor


    std::mutex students_mut; // mutex for students
    std::mutex connects_mut; // mutex for connections
    
    std::unordered_map<std::thread::id, ThreadInfo> connections_info;
    Students students; 
    char buffer[50];    // 50 -> size of a buffer
};
