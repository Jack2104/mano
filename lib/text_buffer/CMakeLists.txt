project(text_buffer)

add_library(
    ${PROJECT_NAME}
    src/TextBuffer.cpp
    src/TextMetadata.cpp
)
add_library(lib::text_buffer ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
