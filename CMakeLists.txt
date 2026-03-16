cmake_minimum_required(VERSION 4.2.1)

project(mano VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 26)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(lib/ncpp)
add_subdirectory(lib/text_buffer)
add_subdirectory(lib/io_backend)
add_subdirectory(editor)
