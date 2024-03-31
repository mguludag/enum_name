cmake_minimum_required(VERSION 3.14)
project(
  enum_name_test
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(AFTER PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} test_enum_name.cpp)
target_link_libraries(${PROJECT_NAME} doctest)

add_test(NAME enum_name_test COMMAND enum_name_test)
