cmake_minimum_required(VERSION 3.21)
project(client)

set(CMAKE_CXX_STANDARD 17)

add_executable(client client.cpp client.h main.cpp)
target_link_libraries(${PROJECT_NAME}
        pthread)

