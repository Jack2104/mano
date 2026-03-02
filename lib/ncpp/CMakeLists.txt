project(ncpp)

add_library(
    ${PROJECT_NAME}
    src/ncurses_utilities.cpp
)
add_library(lib::ncpp ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses)
target_link_libraries(${PROJECT_NAME} PRIVATE Curses)

message("${CURSES_LIBRARIES}")
message("${CURSES_INCLUDE_DIR}")