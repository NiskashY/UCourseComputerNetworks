#include <iostream>
#include "tcp_client.h"

void ShowStudent(const Json::Value& json_fmt_student) {
    const char* name = "name",
              * group_number = "group_number";
    const std::vector<const char*> subjects = {"Math", "Physics", "Biology"};

    std::cout << name << ": " << json_fmt_student[name] << '\n'; 
    std::cout << group_number << ": " << json_fmt_student[group_number] << '\n';
    std::cout << "Subject Marks: " << '\n';
    for (int i = 0; i < (int)subjects.size(); ++i) {
        std::cout << subjects[i] << ": "
                  << json_fmt_student["subjects"][i][subjects[i]] << '\n';
    }
}

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
                    for (int i = 0; i < (int)response.size(); ++i) {
                        ShowStudent(response[i]);
                        if (i + 1 != (int)response.size()) {
                            std::cout << '\n';
                        }
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
