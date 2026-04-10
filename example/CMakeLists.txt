cmake_minimum_required(VERSION 3.14)
project(
  enum_name_example
  VERSION 0.1
  LANGUAGES CXX)


add_executable(${PROJECT_NAME} main.cpp)


target_link_libraries(${PROJECT_NAME} mgutility::enum_name)


if(ENUM_NAME_FETCH_FMT)
  add_compile_definitions(ENUM_NAME_USE_FMT)

  target_link_libraries(${PROJECT_NAME} fmt::fmt)
endif(ENUM_NAME_FETCH_FMT)
