cmake_minimum_required(VERSION 3.0.0)
project(flat3 VERSION 0.1.0)

add_subdirectory(glfw)
add_subdirectory(glm)

add_library(glad)
target_sources(glad
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/glad/include/KHR/khrplatform.h
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/glad/include/glad/glad.h
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/glad/src/glad.c
)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_LIST_DIR}/glad/include/)

add_library(stb INTERFACE)
target_sources(stb
    INTERFACE ${CMAKE_CURRENT_LIST_DIR}/stb/stb_image.h
)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_LIST_DIR}/stb/)