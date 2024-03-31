cmake_minimum_required(VERSION 3.14)
project(
  enum_name_test
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} test_enum_name.cpp)

target_link_libraries(${PROJECT_NAME} mgutility::enum_name doctest::doctest)

add_test(NAME enum_name_test COMMAND enum_name_test)
