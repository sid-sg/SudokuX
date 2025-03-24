#include "userGame.hpp"

#include <string>

#include "backtracking.hpp"
#include "imgui.h"

static int selectedRow = -1, selectedCol = -1;
static int selectedValue = -1;

bool isPuzzleSolved(std::vector<std::vector<int>>& grid) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] == 0 || !backtracking::isValid(i, j, grid[i][j], grid)) {
                return false;  
            }
        }
    }
    return true;
}

void renderPuzzleForUser(std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& givens, bool& puzzleSolved, float& elapsedTime) {
    const float cellSize = 50.0f;
    float totalWidth = cellSize * 9 + 6 * ImGui::GetStyle().ItemSpacing.x + 2 * 15.0f;
    float windowWidth = ImGui::GetWindowWidth();

    if (!puzzleSolved && isPuzzleSolved(grid)) {
        puzzleSolved = true;
        elapsedTime = ImGui::GetTime();
    }

    for (int i = 0; i < 9; i++) {
        float posX = (windowWidth - totalWidth) * 0.5f;
        ImGui::SetCursorPosX(posX);

        for (int j = 0; j < 9; j++) {
            std::string label = (grid[i][j] != 0 ? std::to_string(grid[i][j]) : " ") + "##" + std::to_string(i) + "_" + std::to_string(j);

            bool isGiven = givens[i][j];
            bool isSelected = (selectedRow == i && selectedCol == j);
            bool isInvalid = false;

            if (!puzzleSolved) {
                if (!isGiven && grid[i][j] != 0) {
                    int temp = grid[i][j];
                    grid[i][j] = 0;
                    isInvalid = !backtracking::isValid(i, j, temp, grid);
                    grid[i][j] = temp;
                }
            }

            if (puzzleSolved || isGiven) {
                ImGui::PushStyleColor(ImGuiCol_Button, RGBA(0, 191, 255, 0.8));  // Light Blue
            } else if (isSelected && !isInvalid) {
                ImGui::PushStyleColor(ImGuiCol_Button, RGBA(0, 84, 251, 0.8));  // Dark Blue
            } else if (isInvalid) {
                ImGui::PushStyleColor(ImGuiCol_Button, RGBA(255, 0, 0, 0.8));  // Red
            }

            if (ImGui::Button(label.c_str(), ImVec2(cellSize, cellSize))) {
                selectedRow = i;
                selectedCol = j;
            }

            if (isGiven || isSelected || isInvalid || puzzleSolved) {
                ImGui::PopStyleColor();
            }

            if ((j + 1) % 3 == 0 && j < 8) {
                ImGui::SameLine(0.0f, 15.0f);
            } else if (j < 8) {
                ImGui::SameLine();
            }
        }

        if ((i + 1) % 3 == 0 && i < 8) {
            ImGui::Spacing();
            ImGui::Spacing();
        }
    }

    if (puzzleSolved) {
        ImGui::Spacing();
        ImGui::Text("Puzzle Solved Successfully in %.2f seconds!", elapsedTime);
    }
}

void renderInputGrid(std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& givens) {
    const float cellSize = 40.0f;

    float totalWidth = (cellSize * 3) + (ImGui::GetStyle().ItemSpacing.x * 2);  // 3 cells per row + spacing
    float windowWidth = ImGui::GetWindowWidth();
    float posX = (windowWidth - totalWidth) * 0.5f;

    for (int i = 1; i <= 9; i++) {
        if (i % 3 == 1) {
            ImGui::SetCursorPosX(posX);
        }

        std::string label = std::to_string(i);
        if (ImGui::Button(label.c_str(), ImVec2(cellSize, cellSize))) {
            if (selectedRow != -1 && selectedCol != -1 && givens[selectedRow][selectedCol] == 0) {
                grid[selectedRow][selectedCol] = i;
                selectedRow = -1;
                selectedCol = -1;
            }
        }

        if (i % 3 != 0) {
            ImGui::SameLine();
        }
    }
}

void renderClearButton(std::vector<std::vector<int>>& grid) {
    if (ImGui::Button("Clear Cell", ImVec2(150, 50))) {
        if (selectedRow != -1 && selectedCol != -1) {
            grid[selectedRow][selectedCol] = 0;
        }
    }
    ImGui::SameLine();
}

void renderRestartButton(std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& givens) {
    if (ImGui::Button("Restart", ImVec2(100, 50))) {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (givens[i][j] == false) {
                    grid[i][j] = 0;
                }
            }
        }
    }
    ImGui::SameLine();
}

/*

#include <string>

#include "imgui.h"
#include "userGame.hpp"



void UserGame::renderPuzzle() {
    const float cellSize = 50.0f;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            std::string label = (grid[i][j] != 0 ? std::to_string(grid[i][j]) : " ") + "##" + std::to_string(i) + "_" + std::to_string(j);

            bool isGiven = givens[i][j];
            if (isGiven) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.75f, 1.0f, 0.8f));  // Light Blue
            }

            bool isSelected = (selectedRow == i && selectedCol == j);
            if (isSelected && !isGiven) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.33f, 0.98f, 0.8f));  // Dark Blue
            }

            if (ImGui::Button(label.c_str(), ImVec2(cellSize, cellSize))) {
                selectedRow = i;
                selectedCol = j;
            }

            if (isGiven || (isSelected && !isGiven)) {
                ImGui::PopStyleColor();
            }

            if ((j + 1) % 3 == 0 && j < 8) {
                ImGui::SameLine(0.0f, 15.0f);  // Column spacing
            } else if (j < 8) {
                ImGui::SameLine();
            }
        }

        if ((i + 1) % 3 == 0 && i < 8) {
            ImGui::Spacing();
            ImGui::Spacing();
        }
    }
}

void UserGame::renderInputGrid() {
    const float cellSize = 40.0f;

    for (int i = 1; i <= 9; i++) {
        std::string label = std::to_string(i);

        if (ImGui::Button(label.c_str(), ImVec2(cellSize, cellSize))) {
            if (selectedRow != -1 && selectedCol != -1 && grid[selectedRow][selectedCol] == 0 && !givens[selectedRow][selectedCol]) {
                grid[selectedRow][selectedCol] = i;
                selectedRow = -1;
                selectedCol = -1;
            }
        }

        if (i % 3 != 0) {
            ImGui::SameLine();
        }
    }
}

void UserGame::renderClearButton() {
    if (selectedRow != -1 && selectedCol != -1 && !givens[selectedRow][selectedCol]) {
        if (ImGui::Button("Clear", ImVec2(100, 50))) {
            grid[selectedRow][selectedCol] = 0;
            selectedRow = -1;
            selectedCol = -1;
        }
    }
}

*/