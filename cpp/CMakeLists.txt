cmake_minimum_required(VERSION 3.15)
project(AlgoHarness LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(alg STATIC src/alg.cpp)
target_include_directories(alg PUBLIC include)

add_executable(run src/main.cpp)
target_link_libraries(run PRIVATE alg)


