#pragma once

#include <vector>

#include "constants.hpp"

namespace backtracking {

bool isValid(int row, int col, int num, std::vector<std::vector<int>>& grid);

bool solver(int row, int col, std::vector<std::vector<int>>& grid);

void solve(std::vector<std::vector<int>>& grid, double& timeTaken);

}  // namespace backtracking
