#include "generatePuzzle.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

// ------------------------
// Backtracker to generate complete grid
// ------------------------

bool isValid(std::vector<std::vector<int>>& grid, int row, int col, int num) {
    int boxSize = sqrt(SIZE);
    for (int i = 0; i < SIZE; i++) {
        if (grid[row][i] == num || grid[i][col] == num) return false;
    }
    int startRow = (row / boxSize) * boxSize;
    int startCol = (col / boxSize) * boxSize;
    for (int i = 0; i < boxSize; i++) {
        for (int j = 0; j < boxSize; j++) {
            if (grid[startRow + i][startCol + j] == num) return false;
        }
    }
    return true;
}

bool fillGrid(std::vector<std::vector<int>>& grid) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (grid[row][col] == EMPTY) {
                for (int num = 1; num <= SIZE; num++) {
                    if (isValid(grid, row, col, num)) {
                        grid[row][col] = num;
                        if (fillGrid(grid)) return true;
                        grid[row][col] = EMPTY;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

// ------------------------
// Uniqueness Checking
// ------------------------

// Count solutions using DFS and stop if more than one solution is found
bool solveUnique(std::vector<std::vector<int>>& grid, int& count) {
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            if (grid[row][col] == EMPTY) {
                for (int num = 1; num <= SIZE; num++) {
                    if (isValid(grid, row, col, num)) {
                        grid[row][col] = num;
                        if (solveUnique(grid, count)) return true;
                        grid[row][col] = EMPTY;
                    }
                }
                return false;
            }
        }
    }
    count++;
    return count > 1;
}

// check if puzzle has exactly one solution
bool hasUniqueSolution(std::vector<std::vector<int>> grid) {
    int count = 0;
    solveUnique(grid, count);
    return (count == 1);
}

std::vector<std::pair<int, int>> generateSequence() {
    std::vector<std::pair<int, int>> seq;
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            seq.push_back({r, c});
        }
    }
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::shuffle(seq.begin(), seq.end(), rng);
    return seq;
}

// ------------------------
// puzzle generator using Digging Holes to make holes in the complete grid
// ------------------------

DifficultyMetrics getDifficultyMetrics(int difficulty) {
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist;

    int givenLow, givenHigh, rowColLB;

    switch (difficulty) {
        case 0:
            givenLow = 36, givenHigh = 49, rowColLB = 4;
            break;
        case 1:
            givenLow = 32, givenHigh = 35, rowColLB = 3;
            break;
        case 2:
            givenLow = 28, givenHigh = 31, rowColLB = 2;
            break;
        case 3:
            givenLow = 22, givenHigh = 27, rowColLB = 0;
            break;
        default:
            givenLow = 36, givenHigh = 49, rowColLB = 4;
            break;
    }

    dist = std::uniform_int_distribution<int>(givenLow, givenHigh);
    return {dist(rng), rowColLB};
}

// count givens in full grid
int countGivens(const std::vector<std::vector<int>>& grid) {
    int count = 0;
    for (auto& row : grid)
        for (int num : row)
            if (num != EMPTY) count++;
    return count;
}

// count givens in row
int countRowGivens(const std::vector<std::vector<int>>& grid, int row) {
    int count = 0;
    for (int num : grid[row])
        if (num != EMPTY) count++;
    return count;
}

// count givens in column
int countColGivens(const std::vector<std::vector<int>>& grid, int col) {
    int count = 0;
    for (int row = 0; row < SIZE; row++) {
        if (grid[row][col] != EMPTY) count++;
    }
    return count;
}

void getImpossibleGrid(std::vector<std::vector<int>>& grid, std::vector<std::vector<bool>>& givens) {
    grid = {
        {8, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 3, 0, 0, 0, 6, 0, 0, 0},
        {7, 0, 0, 9, 0, 0, 0, 2, 0},
        {0, 5, 0, 0, 0, 0, 7, 0, 0},
        {0, 0, 0, 4, 0, 5, 0, 7, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 3},
        {0, 1, 0, 0, 0, 0, 0, 6, 8},
        {0, 0, 8, 5, 0, 0, 0, 1, 0},
        {9, 0, 0, 0, 0, 0, 4, 0, 0}
    };

    givens = {
        {1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 0, 0, 0},
        {1, 0, 0, 1, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 1, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 1},
        {0, 1, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 1, 1, 0, 0, 0, 1, 0},
        {1, 0, 0, 0, 0, 0, 1, 0, 0}
    };
}


void digHoles(std::vector<std::vector<int>>& grid, std::vector<std::vector<bool>>& givens, int difficulty) {

    if(difficulty == 4){
        getImpossibleGrid(grid, givens);
        return;
    }
    
    DifficultyMetrics metrics = getDifficultyMetrics(difficulty);
    int currentGivens = countGivens(grid);
    int cellsToRemove = currentGivens - metrics.targetGivens;

    std::cout << "Initial Givens: " << currentGivens << " | Target: " << metrics.targetGivens << std::endl;

    std::vector<std::pair<int, int>> seq = generateSequence();
    std::vector<std::vector<bool>> tried(SIZE, std::vector<bool>(SIZE, false));

    for (auto [row, col] : seq) {
        if (cellsToRemove <= 0) break;
        if (grid[row][col] == EMPTY || tried[row][col]) continue;

        if (countRowGivens(grid, row) - 1 < metrics.rowColLB) continue;
        if (countColGivens(grid, col) - 1 < metrics.rowColLB) continue;

        int backup = grid[row][col];
        grid[row][col] = EMPTY;
        givens[row][col] = false;

        if (!hasUniqueSolution(grid)) {
            grid[row][col] = backup;
            givens[row][col] = true;
        } else {
            cellsToRemove--;
        }
        tried[row][col] = true;
    }

    if (countGivens(grid) > metrics.targetGivens) {
        for (auto [row, col] : seq) {
            if (countGivens(grid) <= metrics.targetGivens) break;
            if (grid[row][col] == EMPTY || tried[row][col]) continue;

            int backup = grid[row][col];
            grid[row][col] = EMPTY;
            givens[row][col] = false;

            if (!hasUniqueSolution(grid)) {
                grid[row][col] = backup;
                givens[row][col] = true;
            }
        }
    }

    std::cout << "Final Givens: " << countGivens(grid) << "\n";
}
