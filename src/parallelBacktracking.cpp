#include "parallelBacktracking.hpp"

#include <omp.h>

#include <iostream>
#include <vector>
#include <cmath>

namespace parallelBacktracking {

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

bool solver(int row, int col, std::vector<std::vector<int>>& grid, int depth = 0) {
    if (row == SIZE) return true;
    if (col == SIZE) return solver(row + 1, 0, grid, depth);
    if (grid[row][col] != EMPTY) return solver(row, col + 1, grid, depth);

    bool foundSolution = false;

#pragma omp taskgroup  // Ensures all spawned tasks finish before proceeding
    {
        for (int num = 1; num <= SIZE; num++) {
            if (isValid(row, col, num, grid)) {
                grid[row][col] = num;

                if (depth < sqrt(SIZE)) {  // Parallelize only the first few levels
#pragma omp task shared(foundSolution) firstprivate(grid)
                    {
                        if (solver(row, col + 1, grid, depth + 1)) {
                            foundSolution = true;
                        }
                    }
                } else {
                    if (solver(row, col + 1, grid, depth + 1)) {
                        foundSolution = true;
                    }
                }

                grid[row][col] = EMPTY;
            }
        }
    }

    return foundSolution;
}

void solve(std::vector<std::vector<int>>& grid) {
    bool solved = false;

#pragma omp parallel
    {
#pragma omp single nowait
        { solved = solver(0, 0, grid, 0); }
    }

    if (!solved) {
        std::cout << "No solution exists!\n";
    }
}

}  // namespace parallelBacktracking
