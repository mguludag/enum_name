cmake_minimum_required(VERSION 3.14)
project(
  enum_name_example
  VERSION 0.1
  LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG 10.2.1) # 10.2.1
FetchContent_MakeAvailable(fmt)

add_compile_definitions(ENUM_NAME_USE_FMT)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} mgutility::enum_name fmt::fmt)
