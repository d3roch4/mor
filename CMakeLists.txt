cmake_minimum_required(VERSION 2.8)

project(mor) # Mapeamento de Objeto Relacional

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

file(GLOB CPP_FILES mor/*.cpp)
file(GLOB HPP_FILES mor/*.h)

if(NOT TARGET d3util)
    add_subdirectory(../d3util d3util)
endif()

add_library(${PROJECT_NAME} ${CPP_FILES} ${HPP_FILES})
target_link_libraries(${PROJECT_NAME} d3util)
target_include_directories(${PROJECT_NAME} PUBLIC ./)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${HPP_FILES} DESTINATION include/${PROJECT_NAME})

