cmake_minimum_required(VERSION 3.14)
project(enum_name_example VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
        DocTest
        GIT_REPOSITORY "https://github.com/onqtam/doctest"
        GIT_TAG "v2.4.11"
)

FetchContent_MakeAvailable(DocTest)

add_executable(${PROJECT_NAME} example/main.cpp)
target_include_directories(${PROJECT_NAME} AFTER PUBLIC ${CMAKE_SOURCE_DIR}/include)

enable_testing()
add_subdirectory(test)