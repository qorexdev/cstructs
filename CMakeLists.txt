cmake_minimum_required(VERSION 3.12)
project(cstructs C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)

# Demo executable
add_executable(demo examples/demo.c)

# Tests
enable_testing()
add_executable(test_all tests/test_all.c)
add_test(NAME AllTests COMMAND test_all)

# Install headers
install(FILES
    include/cstructs.h
    include/vec.h
    include/map.h
    include/list.h
    include/queue.h
    DESTINATION include/cstructs
)
