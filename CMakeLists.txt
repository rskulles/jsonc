cmake_minimum_required(VERSION 3.29)
project(jsonc_test C)

set(CMAKE_C_STANDARD 11)

add_library(jsonc SHARED src/json.c)
add_executable(test_app main.c src/json.c)

target_include_directories(test_app PRIVATE include)
target_include_directories(jsonc PRIVATE include)
target_link_libraries(test_app  PRIVATE jsonc)
install(TARGETS test_app)