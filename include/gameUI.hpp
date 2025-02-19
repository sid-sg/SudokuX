#pragma once

#include <vector>
#include <thread>

#include "imgui.h"

#include "backtracking.hpp"

constexpr ImVec4 RGBA(int R, int G, int B, float A = 1.0f) { return ImVec4(R / 255.0f, G / 255.0f, B / 255.0f, A); }

constexpr int ALGO_ALL = 0;
constexpr int ALGO_BACKTRACKING = 1;
constexpr int ALGO_BFS = 2;
constexpr int ALGO_DFS = 3;

enum class GameState {
    // SizeSelection,  // main menu
    DifficultySelection,
    WhoPlaysSelection,
    AlgoSelection,
    PlayingMode,
    AlgoSolving,
};

// enum class SolvingAlgo{
//     Backtracking,
//     BFS,
//     DFS
// }

class GUI {
   private:
    ImGuiIO& io;
    ImGuiWindowFlags window_flags;
    ImFont* mainFont;
    ImFont* headingFont;

    std::vector<std::vector<int>> grid;
    std::vector<std::vector<bool>> givens;
    GameState gameState;
    double timeTaken;

    bool game_started;
    bool game_solving;
    bool game_solved;

    int selected_mode;
    int selected_difficulty;
    int selected_algo;

   public:
    GUI();

    ~GUI();



    void renderUI();

    void generatePuzzle();
    
    void solvePuzzleByAlgo();
};