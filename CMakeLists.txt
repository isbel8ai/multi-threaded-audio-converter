cmake_minimum_required(VERSION 3.6)
project(cinemo_test CXX)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXE_LINKER_FLAGS -static)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
set(CMAKE_EXE_LINK_DYNAMIC_C_FLAGS)       # remove -Wl,-Bdynamic
set(CMAKE_EXE_LINK_DYNAMIC_CXX_FLAGS)

set(CMAKE_SHARED_LIBRARY_C_FLAGS)         # remove -fPIC
set(CMAKE_SHARED_LIBRARY_CXX_FLAGS)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)    # remove -rdynamic
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)

file(GLOB SOURCES "src/*.cpp")

add_executable(cinemo_test ${SOURCES})
target_link_libraries(${PROJECT_NAME} mp3lame pthread)

install (TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/bin)