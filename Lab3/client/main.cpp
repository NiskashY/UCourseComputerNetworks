#include <iostream>
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

    std::string convertJsonString(const Json::Value& json) {
        Json::StreamWriterBuilder builder;
        return Json::writeString(builder, json);
    }

    auto getListOfStudent(int needed_mark) {
        // just need to get reponse from server
        Json::Value prepared_msg;
        prepared_msg["mark"] = needed_mark; 

        // send block
        const auto msg_to_sent{convertJsonString(prepared_msg)};
        uint32_t requested_msg_size = htonl(msg_to_sent.size());
        send(client_fd, &requested_msg_size, sizeof(requested_msg_size), 0);
        send(client_fd, msg_to_sent.data(), msg_to_sent.size(), 0); 

        // read block
        uint32_t received_msg_size = 0;
        read(client_fd, &received_msg_size, sizeof(received_msg_size)); 
        received_msg_size = ntohl(received_msg_size);

        std::string received_students_list(received_msg_size, '\0');
        read(client_fd, received_students_list.data(), received_students_list.size());  // unformated json list 

        Json::Value fmt_json_students_list;
        std::stringstream input(received_students_list);
        input >> fmt_json_students_list;
        return fmt_json_students_list["students"];
    }

private:
    const int kPort = 0;
    
    const std::string kIpAddress;
    sockaddr_in server_address{}; // server information
    int client_fd = 0;            // socket information
};


int main() {
    const std::string& kMessage = "Input [1 - 10] -> get list of students without this mark\nelse -> quit";
    const std::string& kInputMsg = "Input: ";

    Client client;
    std::cout << kMessage << std::endl;
    while (true) {
        std::cout << std::endl << kInputMsg;
        std::string request;
        std::getline(std::cin, request);
        std::stringstream input(request);

        int parsed_request = 0;
        input >> parsed_request;

        char c = 0;
        if (parsed_request > 10 || parsed_request < 1 || input.fail() || input.get(c)) {
            std::cout << "Bye!" << std::endl;
            break;
        } else {
            try {
                auto response = client.getListOfStudent(parsed_request);
                for (auto& item : response) {
                    std::cout << item.toStyledString();
                }
            } catch (std::runtime_error& e) {
                std::cout << e.what() << std::endl;
            } catch (Json::RuntimeError& e) {
                std::cout << e.what() << std::endl;
            }
        }
    }

    return 0;
}
