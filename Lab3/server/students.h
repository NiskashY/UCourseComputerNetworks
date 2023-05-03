#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

class Students {
public:
    struct Student {
        const static int kSubjectCount = 3;
        std::string name;
        int group_number = 0;
        int scholarship = 0;

        std::array<int, kSubjectCount> subjects; 
    };

    Student createStudent() const {
        Student tmp;

        auto Input = [&]<class T>(auto hint, T& val) {
            std::cout << hint << ' ';
            std::cin >> val;
        };

        Input("Input name:", tmp.name); 
        Input("Input group:", tmp.group_number);
        Input("Input scholarship:", tmp.scholarship);
        std::cout << "Input subjects marks:";
        const char* subjects[] = {"Math", "Physics", "Biology"}; 
        for (int i = 0; i < Student::kSubjectCount; ++i) {
            Input(subjects[i], tmp.subjects[i]);
        }

        return tmp;
    }

    void addStudent(Student&& student) {
       data.emplace_back(student); 
    }

    void removeStudent(int index) {
        std::swap(data[index], data.back());
        data.pop_back();
    }

    std::vector<Student> findWithoutMark(int mark) {
        std::vector<Student> result;
        for (auto& student : data) {
            auto& subjects = student.subjects;
            if (std::find(subjects.begin(), subjects.end(), mark) == subjects.end()) {
                result.push_back(student);
            }
        }
        return result;
    }

private:
    std::vector<Student> data;
};
