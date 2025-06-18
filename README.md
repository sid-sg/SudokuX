
## **Preview video** 

https://github.com/user-attachments/assets/1b4c6815-7c5c-4f23-a288-9c0984e81a56

# SudokuX: GUI Sudoku Generator & Solver

A high performance Sudoku Puzzle Generator and Solver with an interactive GUI — built using C++, ImGui, OpenGL, GLFW for native desktop and ported to the web using WebAssembly (WASM + Emscripten).

## ✨ Features

- **Sudoku Puzzle Generation** (with difficulty control: Easy to Impossible)
- **Multiple Solving Algorithms:**
  - Backtracking
  - Simulated Annealing
  - Knuth’s Dancing Links (DLX) (blazing fast)
- **User Play Mode** — Solve manually with real-time constraint validation 
- **Benchmark Mode** — Compare algorithm performance (execution time measured)
- **Interactive GUI** built using ImGui + OpenGL + GLFW
- **Web version** via WebAssembly
- **Timer, Restart, Clear Cell options** in User Mode

## 🚀 Build Instructions

### 📦 Native Desktop Build (Linux / Mac / Windows)

**Prerequisites:**
- CMake 3.10+
- C++17 compatible compiler (tested with Clang)
- OpenGL + GLFW + ImGui

**Steps:**
```bash
git clone https://github.com/sid-sg/SudokuX.git
cd SudokuX
mkdir build && cd build
cmake ..
make
./sudokuSolver
```

### 🌐 WebAssembly Build (WASM)

**Prerequisites:**
- Emscripten SDK (latest version)

**Setup & Build:**
```bash
source /path/to/emsdk/emsdk_env.sh # Replace with your actual path
emcmake cmake -B webAssembly
cmake --build webAssembly
```

**Run locally via emrun:**
```bash
emrun --no_browser --port 8000 webAssembly/sudokuSolver.html
```

Then open:  
👉 [http://localhost:8000/webAssembly/sudokuSolver.html](http://localhost:8000/webAssembly/sudokuSolver.html)
