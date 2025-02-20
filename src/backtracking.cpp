#include "backtracking.hpp"

// #include <chrono>
#include <iostream>
// #include <thread>

namespace backtracking {

bool isValid(int row, int col, int num, std::vector<std::vector<int>>& grid) {
    int boxRowStart = row - (row % 3);
    int boxColStart = col - (col % 3);

    for (int i = 0; i < SIZE; i++) {
        if (grid[row][i] == num || grid[i][col] == num) return false;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[boxRowStart + i][boxColStart + j] == num) return false;
        }
    }

    return true;
}

bool solver(int row, int col, std::vector<std::vector<int>>& grid) {
    if (row == SIZE) return true;
    if (col == SIZE) return solver(row + 1, 0, grid);
    if (grid[row][col] != EMPTY) return solver(row, col + 1, grid);

    for (int num = 1; num <= SIZE; num++) {
        if (isValid(row, col, num, grid)) {
            grid[row][col] = num;
            if (solver(row, col + 1, grid)) return true;
            grid[row][col] = EMPTY;
        }
    }

    return false;
}

void solve(std::vector<std::vector<int>>& grid) {
    // auto timeStart = std::chrono::high_resolution_clock::now();
    // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    if (!solver(0, 0, grid)) {
        std::cout << "No solution exists!\n";
    }
    // auto timeEnd = std::chrono::high_resolution_clock::now();

    // timeTaken = std::chrono::duration<double, std::milli>(timeEnd - timeStart).count();
}
}  // namespace backtracking