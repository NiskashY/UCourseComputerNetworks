#include <iostream>
#include <cstring>
#include <sstream>

#include <utility>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

class Client {
public:
    Client(const int& kPort_ = 9984,
           std::string kIpAddress_ = "127.0.0.1") : kPort(kPort_), kIpAddress(std::move(kIpAddress_)) {
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

    void sendMessage(const std::string& message) const {
        send(client_fd, message.c_str(), message.size(), 0);
        std::cout << "Hello message sent: Client -> Server" << '\n';
    }

    void sendMessage(int m, int n) const {
        // create message
        auto final_request = std::to_string(m), str_n = std::to_string(n);
        auto final_size = std::to_string(final_request.size() + str_n.size() + 1);
        final_request += " " + str_n;

        const int number_max_len = 20;
        char buffer[number_max_len];

        send(client_fd, final_size.c_str(), final_size.size(), 0);       // first
        read(client_fd, buffer, number_max_len);    // ensure that request is send
        send(client_fd, final_request.c_str(), final_request.size(), 0); // second


        memset(buffer, 0, number_max_len);
        // accept response
        read(client_fd, buffer, number_max_len);
        std::cout << buffer << std::endl;
    }

private:
    const int kPort = 0;

    char ensure[2] = "";
    const std::string kIpAddress;
    sockaddr_in server_address{}; // server information
    int client_fd = 0;            // socket information
};


int main() {
    const std::string& kMessage = "Input two non-negative integers -> get sum of factorials\nelse -> quit";

    std::cout << kMessage << std::endl;
    while (true) {
        std::string request;
        std::getline(std::cin, request);
        std::stringstream input(request);

        int m = 0, n = 0;
        input >> m >> n;

        char c = 0;
        if (m < 0 || n < 0 || input.fail() || input.get(c)) {
            std::cout << "Bye!" << std::endl;
            break;
        } else {
            try {
                Client client;
                client.sendMessage(m, n);
            } catch (std::runtime_error& e) {
                std::cout << e.what() << std::endl;
            }
        }
    }


    return 0;
}
