cmake_minimum_required(VERSION 3.21)
project(Server)

set(CMAKE_CXX_STANDARD 20)

add_executable(Server main.cpp combinatorics.h tcp_server.h)
#--------------JSONCPP------------------

set(JSON_INC_PATH /home/mint/GitRepos/vcpkg/packages/jsoncpp_x64-osx/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${JSON_INC_PATH})

set(JSON_LIB_PATH external/vcpkg/packages/jsoncpp_x64-osx/lib)
target_link_directories(${PROJECT_NAME} PUBLIC ${JSON_LIB_PATH})

#----------------------------------------

target_link_libraries(${PROJECT_NAME}
        jsoncpp
)
