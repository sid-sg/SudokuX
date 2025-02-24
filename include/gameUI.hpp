#pragma once

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include "backtracking.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "simulatedAnnealing.hpp"

constexpr ImVec4 RGBA(int R, int G, int B, float A = 1.0f) { return ImVec4(R / 255.0f, G / 255.0f, B / 255.0f, A); }

constexpr int ALGO_ALL = 0;
constexpr int ALGO_BACKTRACKING = 1;
constexpr int ALGO_SIMULATED_ANNEALING = 2;
constexpr int ALGO_GENETIC = 3;

enum class GameState {
    // SizeSelection,  // main menu
    DifficultySelection,
    WhoPlaysSelection,
    AlgoSelection,
    PlayingMode,
    AlgoSolving,
    AllAlgoMode
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

    ImVec2 windowSize;
    ImVec2 center;

    std::vector<std::vector<int>> grid;
    std::vector<std::vector<bool>> givens;
    GameState gameState;
    double timeTaken;
    std::vector<std::pair<std::string, double>> timeResults;

    bool game_started;
    bool game_solving;
    bool game_solved;

    int selected_mode;
    int selected_difficulty;
    int selected_algo;

    std::unique_ptr<std::thread> solverThread;
    std::atomic<bool> solverRunning;

   public:
    GUI();

    ~GUI();

    bool Spinner(const char* label, float radius, int thickness, const ImU32& color);

    void renderUI();

    void generatePuzzle();

    void solvePuzzleByAlgo();

    void renderTime();
};