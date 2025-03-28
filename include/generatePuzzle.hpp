#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <random>
#include <vector>

#include "constants.hpp"

// ------------------------
// Backtracker to generate complete grid
// ------------------------

bool isValid(std::vector<std::vector<int>>& grid, int row, int col, int num);

bool fillGrid(std::vector<std::vector<int>>& grid);

// ------------------------
// Uniqueness checking
// ------------------------

bool solveUnique(std::vector<std::vector<int>>& grid, int& count);

bool hasUniqueSolution(std::vector<std::vector<int>> grid);

std::vector<std::pair<int, int>> generateSequence();

// ------------------------
// puzzle generator using Digging Holes to make holes in the complete grid
// ------------------------

struct DifficultyMetrics {
    int targetGivens;
    int rowColLB;  // row/col lower bound
};

DifficultyMetrics getDifficultyMetrics(int difficulty);

int countGivens(const std::vector<std::vector<int>>& grid);

int countRowGivens(const std::vector<std::vector<int>>& grid, int row);

int countColGivens(const std::vector<std::vector<int>>& grid, int col);

void getImpossibleGrid(std::vector<std::vector<int>>& grid, std::vector<std::vector<bool>>& givens);

void digHoles(std::vector<std::vector<int>>& grid, std::vector<std::vector<bool>>& givens, int difficulty);