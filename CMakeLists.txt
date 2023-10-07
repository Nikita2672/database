cmake_minimum_required(VERSION 3.25)
project(lab1)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab1 data/data.c errors.c errors.h fileUtil.c test/testrw.c query/query.c query/query.h data/data.h file/tableBlocks.c file/fileApi.c file/tableBlocks.h file/fileApi.h test/tests.c test/tests.h file/dataBlocks.c file/dataBlocks.h file/allocator.c file/allocator.h file/iterator.c file/iterator.h data/comparator.c data/comparator.h)
