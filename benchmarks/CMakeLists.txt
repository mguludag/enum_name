if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  message(FATAL_ERROR
    "Compile-time benchmarks support GCC and Clang compiler timing.")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(_enum_name_benchmark_time_option -ftime-trace)
  set(_enum_name_benchmark_constexpr_option -fconstexpr-steps=10000000)
else()
  set(_enum_name_benchmark_time_option -ftime-report)
  set(_enum_name_benchmark_constexpr_option -fconstexpr-ops-limit=1000000000)
endif()

if(CMAKE_VERSION VERSION_LESS 3.22)
  message(FATAL_ERROR "Compile-time benchmarks require CMake 3.22 or later.")
endif()

if(CMAKE_CXX_STANDARD)
  set(_enum_name_benchmark_cxx_standard "${CMAKE_CXX_STANDARD}")
else()
  set(_enum_name_benchmark_cxx_standard 20)
endif()
set(
  ENUM_NAME_BENCHMARK_CXX_STANDARD
  "${_enum_name_benchmark_cxx_standard}"
  CACHE STRING "C++ standard used by compile-time benchmarks (17 or later)")
unset(_enum_name_benchmark_cxx_standard)
if(ENUM_NAME_BENCHMARK_CXX_STANDARD LESS 17)
  message(FATAL_ERROR "ENUM_NAME_BENCHMARK_CXX_STANDARD must be 17 or later.")
endif()

set(ENUM_NAME_BENCHMARK_RANGE 2048 CACHE STRING
    "Enum range used by compile-time benchmarks")
set_property(CACHE ENUM_NAME_BENCHMARK_RANGE PROPERTY STRINGS
  32 64 128 256 512 1024 2048 4096)
# if(NOT "${ENUM_NAME_BENCHMARK_RANGE}" MATCHES "^(32|64|128|256|512|1024|2048|4096)$")
#   message(FATAL_ERROR
#     "ENUM_NAME_BENCHMARK_RANGE must be one of: 32, 64, 256, 512, 1024, 2048, 4096.")
# endif()
math(EXPR _enum_name_benchmark_max_value "${ENUM_NAME_BENCHMARK_RANGE} - 1")

set(ENUM_NAME_BENCHMARK_VALUE_COUNT "" CACHE STRING
    "Number of declared values in each benchmark enum (default: selected range)")
if("${ENUM_NAME_BENCHMARK_VALUE_COUNT}" STREQUAL "")
  set(_enum_name_benchmark_value_count "${ENUM_NAME_BENCHMARK_RANGE}")
else()
  set(_enum_name_benchmark_value_count "${ENUM_NAME_BENCHMARK_VALUE_COUNT}")
endif()
if(NOT "${_enum_name_benchmark_value_count}" MATCHES "^[1-9][0-9]*$")
  message(FATAL_ERROR
    "ENUM_NAME_BENCHMARK_VALUE_COUNT must be a positive integer.")
elseif(_enum_name_benchmark_value_count GREATER ENUM_NAME_BENCHMARK_RANGE)
  message(FATAL_ERROR
    "ENUM_NAME_BENCHMARK_VALUE_COUNT must not exceed ENUM_NAME_BENCHMARK_RANGE.")
endif()

set(ENUM_NAME_BENCHMARK_MIN_RANGE "" CACHE STRING
    "Minimum value searched by enum reflection libraries")
set(ENUM_NAME_BENCHMARK_MAX_RANGE "" CACHE STRING
    "Maximum value searched by enum reflection libraries (inclusive)")
if("${ENUM_NAME_BENCHMARK_MIN_RANGE}" STREQUAL "" AND
   "${ENUM_NAME_BENCHMARK_MAX_RANGE}" STREQUAL "")
  set(_enum_name_benchmark_min_range 0)
  set(_enum_name_benchmark_max_range "${_enum_name_benchmark_max_value}")
elseif("${ENUM_NAME_BENCHMARK_MIN_RANGE}" STREQUAL "" OR
       "${ENUM_NAME_BENCHMARK_MAX_RANGE}" STREQUAL "")
  message(FATAL_ERROR
    "Set both ENUM_NAME_BENCHMARK_MIN_RANGE and ENUM_NAME_BENCHMARK_MAX_RANGE.")
else()
  set(_enum_name_benchmark_min_range "${ENUM_NAME_BENCHMARK_MIN_RANGE}")
  set(_enum_name_benchmark_max_range "${ENUM_NAME_BENCHMARK_MAX_RANGE}")
endif()
if(NOT "${_enum_name_benchmark_min_range}" MATCHES "^-?[0-9]+$" OR
   NOT "${_enum_name_benchmark_max_range}" MATCHES "^-?[0-9]+$")
  message(FATAL_ERROR "Benchmark range bounds must be integers.")
endif()
math(EXPR _enum_name_benchmark_last_value
  "${_enum_name_benchmark_value_count} - 1")
if(_enum_name_benchmark_min_range GREATER 0 OR
   _enum_name_benchmark_max_range LESS _enum_name_benchmark_last_value)
  message(FATAL_ERROR
    "The configured search range must include all generated values.")
elseif(_enum_name_benchmark_min_range GREATER_EQUAL _enum_name_benchmark_max_range)
  message(FATAL_ERROR
    "ENUM_NAME_BENCHMARK_MAX_RANGE must be greater than MIN_RANGE.")
endif()
math(EXPR _enum_name_benchmark_max_exclusive
  "${_enum_name_benchmark_max_range} + 1")

set(ENUM_NAME_BENCHMARK_ENUM_COUNT 200 CACHE STRING
    "Number of enum types in each compile-time benchmark (1-1000)")
if(NOT "${ENUM_NAME_BENCHMARK_ENUM_COUNT}" MATCHES "^[1-9][0-9]*$")
  message(FATAL_ERROR
    "ENUM_NAME_BENCHMARK_ENUM_COUNT must be an integer from 1 to 1000.")
elseif(ENUM_NAME_BENCHMARK_ENUM_COUNT GREATER 1000)
  message(FATAL_ERROR
    "ENUM_NAME_BENCHMARK_ENUM_COUNT must be an integer from 1 to 1000.")
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/generate_benchmark.py")
set(_enum_name_benchmark_generated_dir
    "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${_enum_name_benchmark_generated_dir}")
execute_process(
  COMMAND
    "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/generate_benchmark.py"
    --range "${ENUM_NAME_BENCHMARK_RANGE}"
    --value-count "${_enum_name_benchmark_value_count}"
    --enum-count "${ENUM_NAME_BENCHMARK_ENUM_COUNT}"
    --output "${_enum_name_benchmark_generated_dir}/benchmark_enums.hpp"
  RESULT_VARIABLE _enum_name_benchmark_generator_result)
if(NOT _enum_name_benchmark_generator_result EQUAL 0)
  message(FATAL_ERROR "Failed to generate compile-time benchmark enums.")
endif()

include(FetchContent)

FetchContent_Declare(
  magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
  GIT_TAG v0.9.7
  GIT_SHALLOW TRUE)

FetchContent_Declare(
  enchantum
  GIT_REPOSITORY https://github.com/ZXShady/enchantum.git
  GIT_TAG 0.5.0
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(magic_enum)

function(add_enum_name_compile_time_benchmark
         target source library_definition include_dir)
  add_executable(${target} "${source}")
  target_include_directories(${target} PRIVATE
    "${include_dir}"
    "${_enum_name_benchmark_generated_dir}")
  target_compile_definitions(${target} PRIVATE
    "${library_definition}"
    "ENUM_NAME_BENCHMARK_MIN_RANGE=${_enum_name_benchmark_min_range}"
    "ENUM_NAME_BENCHMARK_MAX_RANGE=${_enum_name_benchmark_max_range}")
  target_compile_features(${target} PRIVATE cxx_std_17)
  set_target_properties(${target} PROPERTIES
    CXX_STANDARD "${ENUM_NAME_BENCHMARK_CXX_STANDARD}"
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)
  target_compile_options(${target} PRIVATE
    "${_enum_name_benchmark_time_option}"
    "${_enum_name_benchmark_constexpr_option}")
endfunction()

add_enum_name_compile_time_benchmark(
  enum_name_benchmark
  enum_name_benchmark.cpp
  ENUM_NAME_BENCHMARK_MGUTILITY
  "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(enum_name_benchmark PRIVATE mgutility::enum_name)
target_include_directories(enum_name_benchmark PRIVATE
  "${PROJECT_SOURCE_DIR}/mgutility/include")
target_compile_definitions(enum_name_benchmark PRIVATE
  "MGUTILITY_ENUM_RANGE_MIN=${_enum_name_benchmark_min_range}"
  "MGUTILITY_ENUM_RANGE_MAX=${_enum_name_benchmark_max_exclusive}")

string(LENGTH "V${_enum_name_benchmark_value_count}" _max_generated_name_length)
math(EXPR _enum_name_benchmark_blob_size
  "${_enum_name_benchmark_value_count} * ${_max_generated_name_length}")
if(_enum_name_benchmark_blob_size LESS 8192)
  set(_enum_name_benchmark_blob_size 8192)
endif()
target_compile_definitions(enum_name_benchmark PRIVATE
  "MGUTILITY_GLOBAL_ENUM_BLOB_SIZE=${_enum_name_benchmark_blob_size}")

add_enum_name_compile_time_benchmark(
  magic_enum_benchmark
  magic_enum_benchmark.cpp
  ENUM_NAME_BENCHMARK_MAGIC_ENUM
  "${magic_enum_SOURCE_DIR}/include")
target_compile_definitions(magic_enum_benchmark PRIVATE
  "MAGIC_ENUM_RANGE_MIN=${_enum_name_benchmark_min_range}"
  "MAGIC_ENUM_RANGE_MAX=${_enum_name_benchmark_max_range}")

if(ENUM_NAME_BENCHMARK_CXX_STANDARD GREATER_EQUAL 20)
  FetchContent_MakeAvailable(enchantum)
  add_enum_name_compile_time_benchmark(
    enchantum_benchmark
    enchantum_benchmark.cpp
    ENUM_NAME_BENCHMARK_ENCHANTUM
    "${enchantum_SOURCE_DIR}/include")
  target_compile_features(enchantum_benchmark PRIVATE cxx_std_20)
  target_compile_definitions(enchantum_benchmark PRIVATE
    "ENCHANTUM_MIN_RANGE=${_enum_name_benchmark_min_range}"
    "ENCHANTUM_MAX_RANGE=${_enum_name_benchmark_max_range}")
endif()
