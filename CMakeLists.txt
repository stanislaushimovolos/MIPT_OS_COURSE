cmake_minimum_required(VERSION 3.8)
project(Ovchinnikov)

set(CMAKE_CXX_STANDARD 11)

#set(SOURCE_FILES TriangleAndPoint/main.c)
set(SOURCE_FILES Matrix/main.c)
#set(SOURCE_FILES TriangleAndPoint/main.c)

set(GCC_COVERAGE_COMPILE_FLAGS "-lm")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
add_executable(Ovchinnikov ${SOURCE_FILES} )