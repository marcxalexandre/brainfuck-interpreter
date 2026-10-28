cmake_minimum_required(VERSION 3.10)
project(brainfuck LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(brainfuck main.c helper.c)
target_compile_options(brainfuck PRIVATE -Wall -Wextra -pedantic)


include(CTest)
if(BUILD_TESTING)
  add_test(NAME hello_world
           COMMAND brainfuck ${CMAKE_CURRENT_SOURCE_DIR}/hello_world.bf)
  set_tests_properties(hello_world PROPERTIES
                       PASS_REGULAR_EXPRESSION "Hello World!")
endif()
