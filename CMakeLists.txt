cmake_minimum_required(VERSION 3.0)
project(my_muduo)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11") 
 
aux_source_directory(. SRC_ROOT) 

add_library(myMuduo SHARED  ${SRC_ROOT})