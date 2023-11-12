cmake_minimum_required(VERSION 3.25)
project(lab1)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab1 test/testrw.c private/query/query.c public/query/query.h public/data/data.h private/file/tableBlocks.c private/file/fileApi.c public/file/tableBlocks.h public/file/fileApi.h test/tests.c test/tests.h public/file/dataBlocks.h private/file/allocator.c public/file/allocator.h private/file/iterator.c public/file/iterator.h private/data/comparator.c public/data/comparator.h
        private/util/util.c
        public/util/util.h
        test/local.c
        test/local.h
        private/platformic/cutfile.c
        public/platformic/cutfile.h)

#include_directories(include)
