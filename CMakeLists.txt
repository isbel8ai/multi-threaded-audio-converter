cmake_minimum_required(VERSION 3.6)
project(cinemo_test)

set(CMAKE_CXX_STANDARD 17)

add_executable(cinemo_test src/main.cpp)
target_link_libraries(${PROJECT_NAME}  stdc++fs mp3lame)