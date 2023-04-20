#include <iostream>
#include <cstring>
#include <sstream>

#include <stdexcept>
#include <utility>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>


// appearance
#define CLEAR_LINE() (std::cout << "\033[2K")
#define GO_UP_LINE() (std::cout << "\033[1A")

class UDPClient {
public:
    UDPClient(const int& kPort_ = 9984,
           std::string kIpAddress_ = "127.0.0.1") : kPort(kPort_), kIpAddress(std::move(kIpAddress_))
    {
        client_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (client_fd < 0) {
            throw std::runtime_error("Cant create socket descriptor.");
        }

        server_address.sin_family = AF_INET;      // IPv4
        server_address.sin_port   = htons(kPort);
        address_size = (socklen_t*)sizeof(server_address);

        if (inet_pton(AF_INET, kIpAddress.c_str(), &server_address.sin_addr) <= 0) {
            throw std::runtime_error("Cant convert IP address from text to binary form");
        }
         
        timeval tv {0, 100000};
        if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
            throw std::runtime_error("Cant set timeout for socket");
        }   
    }


    std::string getResponse(const std::string& message) const {
        // send size => send msg
        uint32_t msg_size = htonl(message.size());

        static std::string kServerUnavailabaleMsg = "Response: Time Limit exceeded";

        sendto(client_fd, &msg_size, sizeof(msg_size), 0,
               (sockaddr*)&server_address, sizeof(server_address)
        );

        sendto(client_fd, message.c_str(), message.size(), 0,
               (sockaddr*)&server_address, sizeof(server_address)
        );

        msg_size = 0;
        // get msg size from server => accepted response
        recvfrom(client_fd, &msg_size, sizeof(msg_size), MSG_WAITALL,
                     (sockaddr*)&server_address, address_size);
        
        msg_size = ntohl(msg_size);
        std::string response(msg_size, '\0');

        recvfrom(client_fd, response.data(), msg_size, MSG_WAITALL,
                     (sockaddr*)&server_address, address_size);

        return response.empty() ? kServerUnavailabaleMsg : response;
    }

private:
    const int kPort = 0;
    const std::string kIpAddress;
    int client_fd = 0;            // socket information

    sockaddr_in server_address{}; // server information
    socklen_t* address_size{};
};


int main() {
    UDPClient client;

    const std::string& kMessage = "Input line // If line == '!q'-> exit";
    const std::string& kInputMsg = "Input: ";
    const std::string& kQuitMsg = "!q";

    std::cout << kMessage << std::endl;
    while (true) {
        std::string request;
        std::cout << std::endl;
        do {
            std::cout << std::endl << kInputMsg;
            std::getline(std::cin, request);
            if (request.empty()) {
                GO_UP_LINE();
                GO_UP_LINE();
                CLEAR_LINE();
            }
        } while (request.empty());
        
        if (request == kQuitMsg) {
            std::cout << "Bye!" << std::endl;
            break;
        }

        std::cout << client.getResponse(request) << std::endl;
    }

    return 0;
}
