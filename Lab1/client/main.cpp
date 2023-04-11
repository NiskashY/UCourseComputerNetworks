#include <iostream>
#include <string.h>
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

    ~Client() {
        close(client_fd);
    }

    void sendMessage(const std::string& message) const {
        send(client_fd, message.c_str(), message.size(), 0);
        std::cout << "Hello message sent: Client -> Server" << '\n';
    }

private:
    const int kPort = 0;

    const std::string kIpAddress;
    sockaddr_in server_address{}; // server information
    int client_fd = 0;            // socket information
};


int main() {
    Client client;

    client.sendMessage("hello!");
}
