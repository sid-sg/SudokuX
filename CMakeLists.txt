cmake_minimum_required(VERSION 3.10)
project(Sudoku-Solver VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# WebAssembly-specific settings
if(EMSCRIPTEN)
    set(CMAKE_BUILD_TYPE Release)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/webAssembly")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -sASSERTIONS=0 -sERROR_ON_UNDEFINED_SYMBOLS=0 -s USE_GLFW=3 -s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s USE_PTHREADS=1 -pthread -s PTHREAD_POOL_SIZE=2 -s --shell-file ${CMAKE_SOURCE_DIR}/webAssembly/shell.html")
    
    # Preload font folder
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${CMAKE_SOURCE_DIR}/assets/font@assets/font")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -march=native -flto")
    set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld")
endif()

message(STATUS "Using C++ Compiler: ${CMAKE_CXX_COMPILER}")

if( NOT EMSCRIPTEN)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
endif()

# ImGui
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

# Executable
add_executable(sudokuSolver
    src/main.cpp
    src/gameUI.cpp
    src/generatePuzzle.cpp
    src/puzzleRender.cpp
    src/userGame.cpp
    src/backtracking.cpp
    src/simulatedAnnealing.cpp
    src/dlx.cpp
)

# Link libraries
if(EMSCRIPTEN)
    target_link_libraries(sudokuSolver PRIVATE imgui glfw)
else()
    target_link_libraries(sudokuSolver PRIVATE imgui glfw OpenGL::OpenGL)
endif()