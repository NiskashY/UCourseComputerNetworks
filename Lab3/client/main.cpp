#include <iostream>
#include "tcp_client.h"

int main() {
    const std::string& kMessage = "Input [1 - 10] -> get list of students without this mark\nelse -> quit";
    const std::string& kInputMsg = "Input: ";

    Client client;
    std::cout << kMessage << std::endl;
    while (true) {
        std::cout << std::endl << kInputMsg;
        std::string request;
        std::getline(std::cin, request);
        std::stringstream input(request);

        int parsed_request = 0;
        input >> parsed_request;

        char c = 0;
        if (parsed_request > 10 || parsed_request < 1 || input.fail() || input.get(c)) {
            std::cout << "Bye!" << std::endl;
            break;
        } else {
            try {
                auto response = client.getListOfStudent(parsed_request);
                if (response.isNull()) {
                    std::cout << "no students in the database!" << std::endl;
                } else {
                    for (auto &item: response) {
                        std::cout << item.toStyledString(); // TODO: compact
                    }
                }
            } catch (std::runtime_error& e) {
                std::cout << e.what() << std::endl;
            } catch (Json::RuntimeError& e) {
                std::cout << e.what() << std::endl;
            }
        }
    }

    return 0;
}
