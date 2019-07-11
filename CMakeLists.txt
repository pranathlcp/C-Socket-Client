cmake_minimum_required(VERSION 3.14)
project(C_Client C)

set(CMAKE_C_STANDARD 99)

add_executable(C_Client client.c)
target_link_libraries(C_Client pthread)
