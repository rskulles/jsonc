cmake_minimum_required(VERSION 3.29)
project(jsonc_test C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_library(jsonc SHARED src/json.c)

set_target_properties(jsonc PROPERTIES
        LINKER_LANGUAGE C
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
target_include_directories(jsonc PRIVATE include)

add_executable(test_app main.c)
target_link_libraries(test_app  PRIVATE jsonc)
set_target_properties(test_app PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(test_app PRIVATE include)
