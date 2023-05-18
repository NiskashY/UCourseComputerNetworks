#include <iostream>
#include <vector>

#include "../server/file_handler.h"
#include "../server/students.h"

auto ShowMenu(const auto& menu) -> std::string {
    for (auto item : menu) {
        std::cout << item << std::endl;
    }
    std::string choice;
    std::cout << "Make a choice: ";

    do {
        std::getline(std::cin, choice);
    } while (choice.empty());

    return choice;
}

auto ProcessRequest(int req) -> void {
    static constexpr auto kFilePath = "/home/mint/GitRepos/UCourseComputerNetworks/Lab4/server/build/students.txt";

    FileHandler handler(kFilePath);
    Students students(handler.read()); // TODO: file is locked, after multiple requests

    if (req == 1) {
        int index = 0;
        students.visitor([&]<class T>(T& item) {
            std::cout << ++index << ".\n" << item << '\n';
        });
    } else if (req == 2) {
        students.addStudent(students.createStudent());
    } else {
        students.removeLastStudent();
    }

    handler.rewriteFile(convertJsonString(students.toJson()));
}

int main() {

    const std::vector<const char *> menu = {
            "1 - Show current students",
            "2 - Add one more student",
            "3 - Remove last student",
            "else - exit"
    };
    
    // file locker -> read to students
    while (true) {
        system("clear");
        auto choice = ShowMenu(menu);

        int choiceNumber = choice[0] - '0';
        if (0 < choiceNumber && choiceNumber < (int)menu.size()) {
            ProcessRequest(choiceNumber);
        } else {
            std::cout << std::endl << "Bye" << std::endl;
            break;
        }

        std::cout << "To continue press Enter:";
        std::getline(std::cin, choice); // ~ system("pause");
    }

    return 0;
}
