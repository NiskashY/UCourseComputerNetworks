#include <iostream>
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

    [[noreturn]] void run() {
        Combinatorics comb;

        while (true) {
            // accept wait until request from client is queued
            int new_socket = accept(socket_fd,  (sockaddr*)(&server_address), (socklen_t*)&address_length);

            if (new_socket < 0) {
                close(new_socket);
                throw std::runtime_error("Error while Accepting on socket");
            }

            // TODO: read from socket -> compute;

            // first client send length of request (max 2 * 18 , second -> send

            const int MAX_NUMBER_SIZE = 20;
            char data_size[MAX_NUMBER_SIZE];
            read(new_socket, data_size, MAX_NUMBER_SIZE);
            send(new_socket, "ok", strlen("ok"), 0);    // ensure client that information is accepted

            int request_size = atoi(data_size) + 1;
            char request[request_size];
            read(new_socket, request, request_size);

            int i = 0;
            auto m = Parse(request, request_size, i);
            auto n = Parse(request, request_size, i);

            std::cout << "Request: " << m << ' ' << n << " | ";
            std::string response{};
            try {
                response = std::to_string(comb.getSumOfFactorials(n, m));
            } catch (std::out_of_range& e) {
                response = e.what();
            }

            send(new_socket, response.c_str(), response.size(), 0); // 0 - no flags specified
            std::cout << "Response: " << response << std::endl;
        }
    }

private:
    const int PORT = 9984;      // TODO: возможность задать автоматически
    const int MAX_QUERIES = 3;

    sockaddr_in server_address{}; // server information
    const int address_length = sizeof(server_address);
    int socket_fd = 0;           // file descriptor
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
