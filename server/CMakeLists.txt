cmake_minimum_required(VERSION 3.21)
project(server)

set(CMAKE_CXX_STANDARD 17)

add_executable(server server.cpp server.h user_server.h user_server.cpp user.h user.cpp user_file.h user_file.cpp helper.h helper.cpp main.cpp group.h group.cpp databases.cpp databases.h)
target_link_libraries(${PROJECT_NAME}
        pthread)