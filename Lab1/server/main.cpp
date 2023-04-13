#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

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

        std::cout << "Listening!" << std::endl;
    }

    ~Server() {
        close(socket_fd);
    }

    long long Parse(const char* request, int request_size, int& i) const {
        int x = 0;
        while (i + 1 < request_size && request[i] != ' ') {
            x *= 10;
            x += request[i] - '0';
            i += 1;
        }
        i += 1;
        return x;
    }

    int32_t GetNumber(const int& new_socket) const {
        int32_t getted_number = 0;
        read(new_socket, &getted_number, sizeof(getted_number));
        return (int32_t)ntohl(getted_number);   // convert from internet type order to host type
    }

    [[noreturn]] void run() {
        Combinatorics comb;

        while (true) {
            // accept wait until request from client is queued
            int new_socket = accept(socket_fd,  (sockaddr*)(&server_address), (socklen_t*)&address_length);

            if (new_socket < 0) {
                close(new_socket);
                throw std::runtime_error("Error while Accepting on socket");
            }

            // TODO: htonl -> to send
            //       ntohl -> to read

            // read first number 
            // read second number
            // send (first! + second!) % kMod

            auto received_m = GetNumber(new_socket);
            // TODO: is i need to inform client ??
            send(new_socket, "ok", strlen("ok"), 0);    // ensure client that information is accepted
            auto received_n = GetNumber(new_socket);

            int32_t response = 0;
            try {
                response = htonl(comb.getSumOfFactorials(received_n, received_m));
            } catch (std::out_of_range& e) {
                std::cerr << '[' << e.what() << "] ";
            }
           
            std::cout << "Request: " << received_m << ' ' << received_n << ' ';
            std::cout << "Response: " << ntohl(response) << std::endl;
            send(new_socket, &response, sizeof(response), 0);
        }
    }

private:
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
