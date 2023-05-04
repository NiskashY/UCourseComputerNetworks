#include <iostream>
#include "tcp_server.h"

int main() {
    try {
        Server server;
        server.run();
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
