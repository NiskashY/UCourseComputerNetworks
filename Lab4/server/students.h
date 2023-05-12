#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <array>
#include <algorithm>
#include <fstream>

#include <json/json.h>
#include <json/value.h>
#include <json/writer.h>

#define NAME_TO_STRING(x) #x
template <class T>
auto convertStringJson(const T& str) -> Json::Value {
    Json::Value fmt_str;
    std::stringstream input(str);
    input >> fmt_str;
    return fmt_str;
}

auto convertJsonString(const Json::Value& json) -> std::string {
    Json::StreamWriterBuilder builder;
    return Json::writeString(builder, json);
}

template <class T, class U>
static void Input(U hint, T& val) {
    std::cout << hint << ' ';
    std::cin >> val;
}

class Students {
public:
    struct Student {
        Student() = default;
        Student(const Json::Value& root) {
            name = root[NAME_TO_STRING(name)].asString();
            group_number = root[NAME_TO_STRING(group_number)].asInt();
            scholarship  = root[NAME_TO_STRING(scholarship)].asInt();
            for (int i = 0; i < kSubjectCount; ++i) {
                subjects[i] = root[NAME_TO_STRING(subjects)][i][subjects_names[i]].asInt();
            }
        }

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

    Students(const std::string& json_fmt_str) {
        if (!json_fmt_str.empty()) {
            auto root = convertStringJson(json_fmt_str);
            for (auto & item : root["students"]) {
                data.emplace_back(item);
            }
        }
    }

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
