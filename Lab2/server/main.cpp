#include <iostream>
#include <cstring>
#include <cassert>
#include <unordered_map>
#include <algorithm>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

class UDPServer { // UDP server
public:
    UDPServer() {
        socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
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

        std::cout << "UDP server is running! Port: " << PORT << " IpAddress: localhost" << std::endl;
    }

    ~UDPServer() {
        close(socket_fd);
    }

  
    std::string CreateResponse(const char* buffer, const auto& kKeyWord) {
        std::unordered_map<int, int> symbol_to_cnt;
        // count number of each sumbol
        for (int i = 0; buffer[i]; ++i) {
            auto symbol = std::tolower(buffer[i]);

            if (kKeyWord.find(symbol) != std::string::npos) {
                symbol_to_cnt[symbol] += 1;
            }
        }

        // form response
        bool is_all_contained = std::all_of(kKeyWord.begin(), kKeyWord.end(), [&](auto& item) {
            return symbol_to_cnt.contains(item);
        });

        std::string response{(is_all_contained ? "YES" : "NO")}; 
        response += ' ';
        for (auto& item : kKeyWord) {
            if (symbol_to_cnt[item] != -1) {
                response += item;
                response += '|' + std::to_string(symbol_to_cnt[item]) + ' ';
                symbol_to_cnt[item] = -1;   // indicate, that we already used this item
            }
        }
        response.shrink_to_fit();

        return response;
    }

    [[noreturn]] void run() {
        /*
        TODO: 1. server accept size of data
              2. allocate memory for data
              3. recvfrom to accept data
              4. from response 
              5. sendto(client_socket)
         */
        uint32_t msg_size = 0;
        const std::string kKeyWord{"windows"};

        while (true) {
            // Get requested message size

            recvfrom(socket_fd, &msg_size, sizeof(msg_size), 0,
                     (sockaddr*)&client_address, &address_length);
            msg_size = ntohl(msg_size);

            char buffer[msg_size + 1];
            buffer[msg_size] = '\0';
            
            // read to buffer of size msg_size data
            recvfrom(socket_fd, buffer, sizeof(buffer), 0,
                     (sockaddr*)&client_address, &address_length);

            std::string response(CreateResponse(buffer, kKeyWord));

            // need to write data into udp-socket
            // send to client size of the response msg
            msg_size = htonl(response.size());
            assert(sendto(socket_fd, &msg_size, sizeof(msg_size), 0,
                   (sockaddr*)&client_address, address_length
            ) != -1);
            
            sendto(socket_fd, response.c_str(), response.size(), 0,
                   (sockaddr*)&client_address, address_length
            );

            ShowMessage(kRequestMsg, buffer, kResponseMsg, response);
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
    const int PORT = 9984; 

    sockaddr_in server_address{}; // server information
    sockaddr_in client_address{}; // client infromation
    socklen_t address_length = sizeof(server_address);

    int socket_fd = 0;            // file descriptor
                                 
    const std::string kRequestMsg{"Request:"};
    const std::string kResponseMsg{"Response:"};
};

int main() {
    try {
        UDPServer server;
        server.run();
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}