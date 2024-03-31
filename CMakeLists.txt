cmake_minimum_required(VERSION 3.14)

project(
  enum_name
  VERSION 1.0
  LANGUAGES CXX)

# Define the library
add_library(enum_name INTERFACE)
add_library(mgutility::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Specify the include directories for the library
target_include_directories(
  enum_name INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                      $<INSTALL_INTERFACE:include>)

# Set the C++ standard
target_compile_features(enum_name INTERFACE cxx_std_11)

if(CMAKE_SYSTEM_NAME STREQUAL Linux)
  include(GNUInstallDirs)
  set(include_install_dir ${CMAKE_INSTALL_INCLUDEDIR})
  set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
else()
  set(include_install_dir "include")
  set(config_install_dir "lib/cmake/${PROJECT_NAME}")
endif()

# Create the package configuration files
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/enum_nameConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

configure_package_config_file(
  ${CMAKE_CURRENT_LIST_DIR}/cmake/enum_nameConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/enum_nameConfig.cmake"
  INSTALL_DESTINATION lib/cmake/enum_name)

if(NOT ${ENUM_NAME_NO_INSTALL})
  # Install the library
  install(
    TARGETS enum_name
    EXPORT enum_nameTargets
    INCLUDES
    DESTINATION include)

  install(
    EXPORT enum_nameTargets
    FILE enum_nameTargets.cmake
    NAMESPACE mgutility::
    DESTINATION lib/cmake/enum_name)

  install(DIRECTORY include/ DESTINATION include)

  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/enum_nameConfig.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/enum_nameConfigVersion.cmake"
          DESTINATION lib/cmake/enum_name)

  export(
    EXPORT enum_nameTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/enum_nameTargets.cmake"
    NAMESPACE mgutility::)
endif()

# Add example executable
add_executable(example_enum_name example/main.cpp)

# Link the example executable with the library
target_link_libraries(example_enum_name PRIVATE mgutility::enum_name)

if(NOT ${ENUM_NAME_NO_TESTS})
  # Enable testing
  enable_testing()

  # FetchContent to get the Doctest testing framework
  include(FetchContent)
  FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11)
  FetchContent_MakeAvailable(doctest)

  # Add test executable
  add_executable(test_enum_name tests/test_enum_name.cpp)

  # Link the test executable with the library and Doctest
  target_link_libraries(test_enum_name PRIVATE mgutility::enum_name
                                               doctest::doctest)

  # Add tests
  add_test(NAME test_enum_name COMMAND test_enum_name)
endif()

if(${ENUM_NAME_BUILD_DOCS})
  add_subdirectory(doc)
endif()
