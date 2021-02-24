cmake_minimum_required(VERSION 2.8)

project(mor) # Mapeamento de Objeto Relacional

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
option(BUILD_EXAMPLE "Build example code" OFF)

file(GLOB CPP_FILES mor/*.cpp)
file(GLOB HPP_FILES mor/*.h)

set(Boost_USE_STATIC_LIBS   ON)
#find_package(Boost COMPONENTS system)
include_directories(./ )

add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${HPP_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ./ )
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} Boost::boost Boost::system)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${HPP_FILES} DESTINATION include/${PROJECT_NAME})


if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()
