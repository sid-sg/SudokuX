cmake_minimum_required(VERSION 3.10)
project(Sudoku-Solver VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC 
    ${PROJECT_SOURCE_DIR}/imgui
    ${PROJECT_SOURCE_DIR}/imgui/backends
)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(sudokuSolver
    src/main.cpp
)

target_link_libraries(sudokuSolver
    PRIVATE
    imgui
    OpenGL::OpenGL
    glfw
)