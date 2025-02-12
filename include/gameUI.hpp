#pragma once

#include "imgui.h"

enum class GameState {
    SizeSelection,  // main menu
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
};