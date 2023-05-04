#pragma once

#include <cstring>
#include <json/value.h>
#include <sstream>

// socket programming
#include <utility>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

// json files
#include <json/json.h>
#include <json/writer.h>

class Client {
public:
    Client(const int& kPort_ = 9984,
           std::string kIpAddress_ = "127.0.0.1") : kPort(kPort_), kIpAddress(std::move(kIpAddress_))
    {
        client_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (client_fd < 0) {
            throw std::runtime_error("Cant create socket descriptor.");
        }

        server_address.sin_family = AF_INET;      // IPv4
        server_address.sin_port   = htons(kPort);

        if (inet_pton(AF_INET, kIpAddress.c_str(), &server_address.sin_addr) <= 0) {
            throw std::runtime_error("Cant convert IP address from text to binary form");
        }


        if (connect(client_fd, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
            throw std::runtime_error("Cant connect to the server with ip: " + kIpAddress);
        }
    }

    auto convertJsonString(const Json::Value& json) -> std::string {
        Json::StreamWriterBuilder builder;
        return Json::writeString(builder, json);
    }

    template<class T>
    auto convertStringJson(const T& str) const -> Json::Value{
        Json::Value fmt_str;
        std::stringstream input(str);
        input >> fmt_str;
        return fmt_str;
    }

    auto getListOfStudent(int needed_mark) -> Json::Value {
        // just need to get reponse from server
        Json::Value prepared_msg;
        prepared_msg["mark"] = needed_mark;

        // send block
        const auto msg_to_sent{convertJsonString(prepared_msg)};
        send(client_fd, msg_to_sent.data(), msg_to_sent.size(), 0);

        // read block
        std::memset(buffer, '\0', sizeof(buffer));
        read(client_fd, buffer, sizeof(buffer));  // unformated json list

        return convertStringJson(buffer)["students"];
    }

private:
    const int kPort = 0;

    char buffer[2049];
    const std::string kIpAddress;
    sockaddr_in server_address{}; // server information
    int client_fd = 0;            // socket information
};