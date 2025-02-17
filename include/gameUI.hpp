#pragma once

#include <vector>

#include "imgui.h"

constexpr ImVec4 RGBA(int R, int G, int B, float A = 1.0f) { return ImVec4(R / 255.0f, G / 255.0f, B / 255.0f, A); }

enum class GameState {
    // SizeSelection,  // main menu
    DifficultySelection,
    WhoPlaysSelection,
    PlayingMode
};

class GUI {
   private:
    ImGuiIO& io;
    ImGuiWindowFlags window_flags;
    ImFont* mainFont;
    ImFont* headingFont;

    std::vector<std::vector<int>> grid;
    GameState gameState;
    bool game_started;
    bool show_difficulty_menu;
    int selected_difficulty;
    int selected_mode;
    int selected_size;

   public:
    GUI();

    ~GUI();

    void renderUI();

    void generatePuzzle();
};