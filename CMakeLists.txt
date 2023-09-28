cmake_minimum_required(VERSION 3.25)
project(lab1)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab1 data/data.c errors.c errors.h fileUtil.c test/testrw.c query/query.c query/query.h data/data.h file/blocks.c file/fileApi.c file/blocks.h file/fileApi.h)
