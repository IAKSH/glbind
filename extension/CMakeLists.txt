cmake_minimum_required(VERSION 3.0.0)
project(flat3 VERSION 0.1.0)

add_library(glbind_ext)
add_dependencies(glbind_ext glad glm)

target_sources(glbind_ext PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/camera.hpp
    ${CMAKE_CURRENT_LIST_DIR}/camera.cpp
    ${CMAKE_CURRENT_LIST_DIR}/image.hpp
    ${CMAKE_CURRENT_LIST_DIR}/image.cpp
)

target_include_directories(glbind_ext PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(glbind_ext glm)