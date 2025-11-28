cmake_minimum_required(VERSION 3.16)

project(NManager LANGUAGES C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})
add_definitions(${GTK4_CFLAGS_OTHER})

add_executable(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/src/Button.c
    ${CMAKE_SOURCE_DIR}/src/Directory.c
    ${CMAKE_SOURCE_DIR}/src/Entry.c
    ${CMAKE_SOURCE_DIR}/src/GlobalState.c
    ${CMAKE_SOURCE_DIR}/src/Main.c
    ${CMAKE_SOURCE_DIR}/src/Menu.c
)

target_link_libraries(${PROJECT_NAME} ${GTK4_LIBRARIES})