cmake_minimum_required(VERSION 3.0.0)
project(flat3 VERSION 0.1.0)

add_executable(main_test test.cpp)
add_dependencies(main_test glbind glfw)
target_include_directories(main_test PUBLIC {$CMAKE_CURRENT_LIST_DIR}/vendor/glfw/include)
target_link_libraries(main_test PUBLIC glbind glfw stb)

add_executable(vertex_test vertex_test.cpp)
add_dependencies(vertex_test glbind glfw)
target_include_directories(vertex_test PUBLIC {$CMAKE_CURRENT_LIST_DIR}/vendor/glfw/include)
target_link_libraries(vertex_test PUBLIC glbind glfw stb)

add_executable(texture_test texture_test.cpp)
add_dependencies(texture_test glbind glfw)
target_include_directories(texture_test PUBLIC {$CMAKE_CURRENT_LIST_DIR}/vendor/glfw/include)
target_link_libraries(texture_test PUBLIC glbind glfw stb)

add_executable(frame_test frame_test.cpp)
add_dependencies(frame_test glbind glfw)
target_include_directories(frame_test PUBLIC {$CMAKE_CURRENT_LIST_DIR}/vendor/glfw/include)
target_link_libraries(frame_test PUBLIC glbind glfw stb)

add_test(NAME vertex_test COMMAND vertex_test)
add_test(NAME texture_test COMMAND texture_test)
add_test(NAME frame_test COMMAND frame_test)