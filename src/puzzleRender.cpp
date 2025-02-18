#include "puzzleRender.hpp"

#include <string>

#include "imgui.h"

void renderPuzzleForAlgo(const std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& givens) {
    const float cellSize = 50.0f;
    static int selectedRow = -1, selectedCol = -1;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            std::string label = (grid[i][j] != 0 ? std::to_string(grid[i][j]) : " ") + "##" + std::to_string(i) + "_" + std::to_string(j);

            bool isGiven = givens[i][j];

            if (isGiven) {
                ImGui::PushStyleColor(ImGuiCol_Button, RGBA(0, 191, 255, 0.8));  // Dark Blue
            }

            if (ImGui::Button(label.c_str(), ImVec2(cellSize, cellSize))) {
                selectedRow = i;
                selectedCol = j;
            }

            if (isGiven) {
                ImGui::PopStyleColor();
            }

            if ((j + 1) % 3 == 0 && j < 8) {
                ImGui::SameLine(0.0f, 15.0f);  // column spacing
            } else if (j < 8) {
                ImGui::SameLine();
            }
        }

        if ((i + 1) % 3 == 0 && i < 8) {  // row spacing
            ImGui::Spacing();
            ImGui::Spacing();
        }
    }
}


void renderPuzzleForUser(const std::vector<std::vector<int>>& grid, const std::vector<std::vector<bool>>& givens) {
    const float cellSize = 50.0f;
    static int selectedRow = -1, selectedCol = -1;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            std::string label = (grid[i][j] != 0 ? std::to_string(grid[i][j]) : " ") + "##" + std::to_string(i) + "_" + std::to_string(j);

            bool isSelected = (selectedRow == i && selectedCol == j);

            if (isSelected) {
                ImGui::PushStyleColor(ImGuiCol_Button, RGBA(0, 84, 251, 0.8));  // Dark Blue
            }

            if (ImGui::Button(label.c_str(), ImVec2(cellSize, cellSize))) {
                selectedRow = i;
                selectedCol = j;
            }

            if (isSelected) {
                ImGui::PopStyleColor();
            }

            if ((j + 1) % 3 == 0 && j < 8) {
                ImGui::SameLine(0.0f, 15.0f);  // column spacing
            } else if (j < 8) {
                ImGui::SameLine();
            }
        }

        if ((i + 1) % 3 == 0 && i < 8) {  // row spacing
            ImGui::Spacing();
            ImGui::Spacing();
        }
    }
}