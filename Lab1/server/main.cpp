#include <iostream>
#include <stdexcept>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "combinatorics.h"

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

    int32_t GetNumber(const int& new_socket, const std::string& client_readable_ip) const {
        int32_t getted_number = 0;
        int retval = read(new_socket, &getted_number, sizeof(getted_number));
        if (!retval) {
            throw std::runtime_error("connection lost with: " + client_readable_ip);
        }
        return (int32_t)ntohl(getted_number);   // convert from internet type order to host type
    }

    bool IsSocketConnected() const {
        int error = 0; socklen_t error_len = sizeof(int);
        int retval = getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &error_len);
        return !retval && !error;
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
                    auto received_m = GetNumber(new_socket, client_readable_ip);
                    auto received_n = GetNumber(new_socket, client_readable_ip);

                    int32_t response = 0;
                    try {
                        response = htonl(comb.getSumOfFactorials(received_n, received_m));
                    } catch (std::out_of_range &e) {
                        std::cerr << '[' << e.what() << "] ";
                    }

                    ShowMessage(kRequestMsg, received_m, received_n, kResponseMsg, ntohl(response));
                    send(new_socket, &response, sizeof(response), 0);
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
