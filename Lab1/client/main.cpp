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

    void sendMessage(const std::string& message) const {
        send(client_fd, message.c_str(), message.size(), 0);
    }

    void sendMessage(int m, int n) {
        // create message
        static char ensure[2];
        static int32_t received_answer = 0;  // variable that stores sum of factorials 
       
        m = htonl(m), n = htonl(n);

        send(client_fd, &m, sizeof(m), 0);
        read(client_fd, ensure, 2);
        send(client_fd, &n, sizeof(n), 0);
        
        read(client_fd, &received_answer, sizeof(received_answer));

        received_answer = ntohl(received_answer);
        std::cout << received_answer << std::endl;
        return;
    }

private:
    const int kPort = 0;

    const std::string kIpAddress;
    sockaddr_in server_address{}; // server information
    int client_fd = 0;            // socket information
};


int main() {
    const std::string& kMessage = "Input two non-negative integers -> get sum of factorials\nelse -> quit";
    const std::string& kInputMsg = "Input: ";

    std::cout << kMessage << std::endl;
    while (true) {
        std::cout << std::endl << kInputMsg;
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
