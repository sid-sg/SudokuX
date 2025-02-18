#pragma once

#include <vector>

#include "gameUI.hpp"

void renderPuzzleForAlgo(const std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& givens);

void renderPuzzleForUser(const std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& givens);

