cmake_minimum_required(VERSION 3.15)
project(cmake_tutorial
    VERSION 1.0.0
    DESCRIPTION "realtime library test"
    LANGUAGES C)



set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Wall -Wextra -pedantic -g3)

add_library(realtime STATIC
    src/utils/queue.c
    src/utils/dictionary.c
    src/utils/linked_list.c
)

target_include_directories(realtime PUBLIC include)



find_package(PkgConfig REQUIRED)
pkg_check_modules(CRITERION REQUIRED criterion)

add_executable(test 
    test/test.c
)

target_link_libraries(test ${CRITERION_LIBRARIES} realtime)
target_include_directories(test PRIVATE
    ${CRITERION_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
    





