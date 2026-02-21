cmake_minimum_required(VERSION 3.14)

project(
  enum_name
  VERSION 1.0
  LANGUAGES CXX)

option(ENUM_NAME_BUILD_DOCS "Build documentation" OFF)
option(ENUM_NAME_BUILD_TESTS "Build tests" OFF)
option(ENUM_NAME_BUILD_EXAMPLE "Build example" ON)
option(ENUM_NAME_FETCH_FMT "Fetch fmt library" OFF)
option(ENUM_NAME_INSTALL "Install the enum_name library" OFF)

# Define the library
add_library(enum_name INTERFACE)
add_library(mgutility::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Specify the include directories for the library
target_include_directories(
  enum_name
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mgutility/include>
            $<INSTALL_INTERFACE:include> $<INSTALL_INTERFACE:mgutility/include>)

# Set the C++ standard
target_compile_features(enum_name INTERFACE cxx_std_11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(STATUS "Compiler is Clang")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

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

if(${ENUM_NAME_INSTALL})
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

if(${ENUM_NAME_BUILD_TESTS})
  # Enable testing
  enable_testing()

  # FetchContent to get the Doctest testing framework
  include(FetchContent)
  FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11)
  FetchContent_MakeAvailable(doctest)

  # include tests
  add_subdirectory(tests)
endif()

if(ENUM_NAME_FETCH_FMT)
  include(FetchContent)

  FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG 10.2.1) # 10.2.1
  FetchContent_MakeAvailable(fmt)

endif(ENUM_NAME_FETCH_FMT)

if(${ENUM_NAME_BUILD_EXAMPLE})
  add_subdirectory(example)
endif()

if(${ENUM_NAME_BUILD_DOCS})
  add_subdirectory(doc)
endif()
