cmake_minimum_required(VERSION 4.2)
project(parseurTest C)

set(CMAKE_C_STANDARD 23)
include(FetchContent)

FetchContent_Declare(
        alexer
        GIT_REPOSITORY https://github.com/tsoding/alexer.git
        GIT_TAG main
)

FetchContent_GetProperties(alexer)

if(NOT alexer_POPULATED)
    FetchContent_MakeAvailable(alexer)
endif()

add_library(alexer INTERFACE)

target_include_directories(alexer INTERFACE
        ${alexer_SOURCE_DIR}
)

add_executable(parseurTest
        parser/OBJ_parser.c
)

target_link_libraries(parseurTest PRIVATE alexer)

find_package(SDL3 REQUIRED)


add_executable(testWindows
        ./3DMotor/main.c
        ./3DMotor/3DMotor.c
        ./3DMotor/3DMotor.h
        ./3DMotor/fileReader.c
        ./3DMotor/fileReader.h
)

target_link_libraries(testWindows PRIVATE m)
target_link_libraries(testWindows PRIVATE SDL3::SDL3)
