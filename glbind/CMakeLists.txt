cmake_minimum_required(VERSION 3.0.0)
project(flat3 VERSION 0.1.0)

add_library(glbind INTERFACE)
add_dependencies(glbind glad glm)

target_sources(glbind 
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/frame.hpp
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/scope.hpp
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/shader.hpp
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/texture.hpp
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/vertex.hpp
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/primitive.hpp
)

target_include_directories(glbind INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(glbind INTERFACE glad glm)