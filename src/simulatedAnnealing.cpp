#include "simulatedAnnealing.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <unordered_set>
#include <vector>

#include "constants.hpp"

namespace simulatedAnnealing {

int calculateScore(std::vector<std::vector<int>>& grid) {
    int score = 0;

    for (int i = 0; i < SIZE; i++) {
        std::array<bool, SIZE + 1> rowCheck = {};
        std::array<bool, SIZE + 1> colCheck = {};

        for (int j = 0; j < SIZE; j++) {
            rowCheck[grid[i][j]] = true;
            colCheck[grid[j][i]] = true;
        }

        score += SIZE - std::count(rowCheck.begin(), rowCheck.end(), true);
        score += SIZE - std::count(colCheck.begin(), colCheck.end(), true);
    }

    return score;
}

void randomizeGrid(std::vector<std::vector<int>>& grid) {
    std::unordered_set<int> givenNumbers;
    auto rng = std::mt19937(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    for (int boxRow = 0; boxRow < 3; boxRow++) {
        for (int boxCol = 0; boxCol < 3; boxCol++) {
            givenNumbers.clear();
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int element = grid[boxRow * 3 + i][boxCol * 3 + j];
                    if (element != EMPTY) {
                        givenNumbers.emplace(element);
                    }
                }
            }

            std::vector<int> values;
            for (int i = 1; i <= SIZE; i++) {
                if (givenNumbers.count(i) == 0) {
                    values.emplace_back(i);
                }
            }

            std::shuffle(values.begin(), values.end(), rng);

            int index = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int element = grid[boxRow * 3 + i][boxCol * 3 + j];
                    if (element == EMPTY) {
                        grid[boxRow * 3 + i][boxCol * 3 + j] = values[index++];
                    }
                }
            }
        }
    }
}

void swapCells(std::vector<std::vector<int>>& grid, std::vector<std::vector<bool>>& givens) {
    int boxRow = rand() % 3;
    int boxCol = rand() % 3;

    int row1, col1, row2, col2;

    do {
        row1 = boxRow * 3 + rand() % 3;
        col1 = boxCol * 3 + rand() % 3;
    } while (givens[row1][col1] != 0);

    do {
        row2 = boxRow * 3 + rand() % 3;
        col2 = boxCol * 3 + rand() % 3;
    } while (givens[row2][col2] != 0);

    std::swap(grid[row1][col1], grid[row2][col2]);
}

void solve(std::vector<std::vector<int>>& grid, std::vector<std::vector<bool>>& givens) {
    randomizeGrid(grid);
    int currentScore = calculateScore(grid);
    auto bestState = grid;
    int bestScore = currentScore;
    double temperature = INITIAL_TEMPERATURE;

    std::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> probDist(0.0, 1.0);

    for (int i = 0; i < MAX_ITERATIONS; ++i) {
        auto nextState = grid;
        swapCells(nextState, givens);
        int nextScore = calculateScore(nextState);
        int delta = currentScore - nextScore; 

        if (delta > 0 || exp(delta / temperature) > probDist(rng)) {
            grid = nextState;
            currentScore = nextScore;

            if (currentScore < bestScore) { 
                bestScore = currentScore;
                bestState = grid;
            }
        }

        if (currentScore == 0) break;  // Perfect solution found

        temperature *= COOLING_RATE;
    }

    grid = bestState;  // Restore best found state
}


}  // namespace simulatedAnnealing