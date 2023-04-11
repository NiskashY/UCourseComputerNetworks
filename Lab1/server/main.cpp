#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

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

    [[noreturn]] void run() const {
        while (true) {
            // accept wait until request from client is queued
            int new_socket = accept(socket_fd,  (sockaddr*)(&server_address), (socklen_t*)&address_length);

            if (new_socket > 0) {
                close(new_socket);
                throw std::runtime_error("Error while Accepting on socket");
            }

            std::string hello_from_server{"Hello from server"};
            send(new_socket, hello_from_server.c_str(), hello_from_server.size(), 0); // 0 - no flags specified
            std::cout << "Hello message sent: Server -> Client" << std::endl;
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
