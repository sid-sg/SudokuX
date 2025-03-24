#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>

#include "backtracking.hpp"
#include "dlx.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "simulatedAnnealing.hpp"

constexpr ImVec4 RGBA(int R, int G, int B, float A = 1.0f) { return ImVec4(R / 255.0f, G / 255.0f, B / 255.0f, A); }

constexpr int ALGO_ALL = 0;
constexpr int ALGO_BACKTRACKING = 1;
constexpr int ALGO_SIMULATED_ANNEALING = 2;
constexpr int ALGO_DLX = 3;

enum class GameState {
    // SizeSelection,  // main menu
    DifficultySelection,
    WhoPlaysSelection,
    AlgoSelection,
    PlayingMode,
    AlgoSolving,
    AllAlgoMode,
    UserPlayingMode
};

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

    std::chrono::steady_clock::time_point startTime;
    double runningTime = 0;
    bool timerRunning = false;

    std::unique_ptr<std::thread> solverThread;
    std::atomic<bool> solverRunning;

    bool hasPrinted;  // for debbuging

   public:
    GUI();

    ~GUI();

    // UI functions

    bool Spinner(const char* label, float radius, int thickness, const ImU32& color);
    void renderTime();
    void renderUI();

    // puzzle functions

    void generatePuzzle();
    void solvePuzzleByAlgo();

    // state functions

    void stateDifficultySelection();
    void stateWhoPlaysSelection();
    void stateAlgoSelection();
    void statePlayingMode();
    void stateAlgoSolving();
    void stateUserPlayingMode();

    // getters

    // const std::vector<std::vector<int>>& getGrid() const { return grid; }
    // const std::vector<std::vector<bool>>& getGivens() const { return givens; }
};