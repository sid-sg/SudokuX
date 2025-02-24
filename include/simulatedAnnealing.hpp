#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

namespace simulatedAnnealing {

constexpr double INITIAL_TEMPERATURE = 1.0;
constexpr double COOLING_RATE = 0.99999;
constexpr int MAX_ITERATIONS = 1000000;

int calculateScore(std::vector<std::vector<int>>& grid);

void randomizeGrid(std::vector<std::vector<int>>& grid);

void swapCells(std::vector<std::vector<int>>& grid, std::vector<std::vector<bool>>& givens);

void solve(std::vector<std::vector<int>>& grid, std::vector<std::vector<bool>>& givens);
}  // namespace simulatedAnnealing
