#pragma once
#include <iostream>
#include <json/value.h>
#include <vector>
#include <array>
#include <algorithm>
#include <fstream>

#define NAME_TO_STRING(x) #x

template <class T, class U>
static void Input(U hint, T& val) {
    std::cout << hint << ' ';
    std::cin >> val;
}

class Students {
public:
    struct Student {
        Student() = default;

        constexpr static int kSubjectCount = 3;
        constexpr static std::string subjects_names[kSubjectCount] = {"Math", "Physics", "Biology"}; 

        std::string name;
        int group_number = 0;
        int scholarship = 0;

        std::array<int, kSubjectCount> subjects{};

        auto toJson() -> Json::Value {
            Json::Value root;
            root[NAME_TO_STRING(name)] = name;
            root[NAME_TO_STRING(group_number)] = group_number;
            root[NAME_TO_STRING(scholarship)] = scholarship;
            for (int i = 0; i < kSubjectCount; ++i) {
                root[NAME_TO_STRING(subjects)][i][subjects_names[i]] = subjects[i];
            }
            return root;
        }
    };

    Students() = default;

    Student createStudent() const {
        Student tmp;

        Input("Input name:", tmp.name); 
        Input("Input group:", tmp.group_number);
        Input("Input scholarship:", tmp.scholarship);
        std::cout << "Input subjects marks:\n";
        for (int i = 0; i < Student::kSubjectCount; ++i) {
            Input(Student::subjects_names[i] + ':', tmp.subjects[i]);
        }

        return tmp;
    }

    void addStudent(Student&& student) {
       data.emplace_back(student); 
    }

    void removeLastStudent() {
        if (!data.empty()) {
            data.pop_back();
        }
    }

    Json::Value findWithoutMark(int mark) {
        Json::Value root;
        int index = 0;
        for (auto& student : data) {
            auto& subjects = student.subjects;
            if (std::find(subjects.begin(), subjects.end(), mark) == subjects.end()) {
                root["students"][index++] = student.toJson();
            }
        }
        return root;
    }

    auto toJson() -> Json::Value {
        Json::Value root;
        for (int i = 0; i < (int)data.size(); ++i) {
            root["students"][i] = data[i].toJson();
        }
        return root;
    }

    template <class T>
    void visitor(T lambda) {
        if (data.empty()) {
            lambda("No one exists");
        }
        for (auto& item : data) {
            lambda(item);
        }
    }

private:
    std::vector<Student> data;
};

std::ostream& operator<<(std::ostream& out, const Students::Student& student) {
    out << NAME_TO_STRING(name) << ": " << student.name << '\n';
    out << NAME_TO_STRING(group_number) << ": " << student. group_number << '\n';
    for (int i = 0; i < Students::Student::kSubjectCount; ++i) {
        out << Students::Student::subjects_names[i] << ": " << student.subjects[i] << '\n';
    }
    return out;
}
