cmake_minimum_required(VERSION 3.22.1)
project(llp C)


set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}   -Wall -Wextra -pedantic -Wmissing-prototypes -Wstrict-prototypes -fsanitize-address-use-after-scope -g")

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/**.c)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/**.h)

include_directories(include)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} PUBLIC ${HEADERS})

add_library(${PROJECT_NAME}_lib ${SOURCES})