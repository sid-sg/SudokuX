#pragma once

#include <vector>

#include "constants.hpp"
#include "gameUI.hpp"

void renderPuzzleForUser(std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& givens, bool& puzzleSolved, float& elapsedTime);

void renderInputGrid(std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& givens);

void renderClearButton(std::vector<std::vector<int>>& grid);

void renderRestartButton(std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& givens);


/*


#pragma once

#include <vector>

#include "constants.hpp"
#include "gameUI.hpp"

class UserGame {
   private:
    GUI& gui;
    std::vector<std::vector<int>> grid;
    std::vector<std::vector<bool>> givens;
    int selectedRow;
    int selectedCol;

   public:
    UserGame(GUI& guiInstance) : gui(guiInstance), grid(guiInstance.getGrid()), givens(guiInstance.getGivens()), selectedRow(-1), selectedCol(-1){}

    void renderPuzzle();
    void renderInputGrid();
    void renderClearButton();
};

*/