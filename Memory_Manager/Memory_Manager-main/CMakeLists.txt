cmake_minimum_required(VERSION 3.21)
project(memory_manager )
# add_subdirectory(shared_lib)
set(CMAKE_BUILD_TYPE Debug)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/atomics)
# set(CMAKE_C_COMPILER "g++-9")


set(HEADER_FILES
${CMAKE_CURRENT_SOURCE_DIR}/src/allocator.h
${CMAKE_CURRENT_SOURCE_DIR}/src/deque.h
${CMAKE_CURRENT_SOURCE_DIR}/src/atomic.h
)



set(SOURCE_FILES
${CMAKE_CURRENT_SOURCE_DIR}/src/allocator.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/deque.c

)

add_library(mylib SHARED  ${HEADER_FILES} ${SOURCE_FILES} )
add_library(fake SHARED  ${CMAKE_CURRENT_SOURCE_DIR}/src/tpt.c )

target_link_libraries(mylib -ldl)
add_executable(test ${CMAKE_CURRENT_SOURCE_DIR}/src/test.c)
target_link_libraries(test mylib -lpthread )#-ldl -lm 
add_dependencies(test mylib)
