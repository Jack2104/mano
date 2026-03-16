project(editor)

add_executable(${PROJECT_NAME} main.cpp editor.cpp)

target_link_libraries(${PROJECT_NAME}
    lib::ncpp
    lib::text_buffer
    lib::io_backend
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})