#include "gameUI.hpp"

#include <chrono>

#include "generatePuzzle.hpp"
#include "puzzleRender.hpp"

GUI::GUI() : io(ImGui::GetIO()), game_started(false), game_solving(false), game_solved(false), selected_difficulty(0), selected_mode(0), selected_algo(ALGO_ALL), timeTaken(0), window_flags(0) {
    grid = std::vector<std::vector<int>>(SIZE, std::vector<int>(SIZE, EMPTY));
    givens = std::vector<std::vector<bool>>(SIZE, std::vector<bool>(SIZE, true));

    (void)this->io;
    this->io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    mainFont = this->io.Fonts->AddFontFromFileTTF("../font/BebasNeue-Regular.ttf", 24.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
    headingFont = this->io.Fonts->AddFontFromFileTTF("../font/Bangers-Regular.ttf", 100.0f, nullptr, io.Fonts->GetGlyphRangesDefault());

    this->io.FontDefault = mainFont;

    static bool no_titlebar = true;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_move = true;
    static bool no_resize = true;
    static bool no_close = true;

    if (no_titlebar) window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu) window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_move) window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize) window_flags |= ImGuiWindowFlags_NoResize;
}

GUI::~GUI() {}

void GUI::generatePuzzle() {
    if (!fillGrid(grid)) {
        std::cout << "Failed to generate complete grid!\n";
    }
    digHoles(grid, givens, selected_difficulty);
}

// void GUI::solvePuzzleByAlgo() {
// double timeTaken = 0;

// std::thread([this]() {
//     switch (selected_algo) {
//         case ALGO_ALL:
//             break;
//         case ALGO_BACKTRACKING:
//             game_solving = true;
//             game_solved = false;

//             // Run solver in a separate thread
//             backtracking::solve(grid, timeTaken);
//             game_solved = true;
//             game_solving = false;

//             break;
//         default:
//             break;
//     }
// }).detach();
// }

void GUI::solvePuzzleByAlgo() {
    game_solving = true;
    game_solved = false;
    timeTaken = 0;

    std::thread([this]() {
        auto start = std::chrono::high_resolution_clock::now();

        backtracking::solve(grid);

        auto end = std::chrono::high_resolution_clock::now();
        timeTaken = std::chrono::duration<double, std::milli>(end - start).count();
        game_solved = true;
        game_solving = false;
    }).detach();
}

void GUI::renderTime() {
    if (timeTaken >= 1000) {
        double seconds = timeTaken / 1000.0;
        ImGui::Text("Time taken: %.2f s", seconds);
    } else {
        ImGui::Text("Time taken: %.4f ms", timeTaken);
    }
}

void GUI::renderUI() {
    ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(1400, 900));

    ImGui::Begin("Sudoku-X", NULL, window_flags);

    ImGui::PushFont(headingFont);
    ImGui::Text("Sudoku-X");
    ImGui::PopFont();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    switch (gameState) {
        case GameState::DifficultySelection:
            ImGui::Text("Select Difficulty:");
            ImGui::RadioButton("Easy", &selected_difficulty, 0);
            ImGui::RadioButton("Medium", &selected_difficulty, 1);
            ImGui::RadioButton("Hard", &selected_difficulty, 2);
            ImGui::RadioButton("Evil", &selected_difficulty, 3);
            ImGui::RadioButton("Impossible", &selected_difficulty, 4);

            if (ImGui::Button("Next ->")) gameState = GameState::WhoPlaysSelection;
            break;

        case GameState::WhoPlaysSelection:
            ImGui::Text("Select Mode:");
            ImGui::RadioButton("I'll Play", &selected_mode, 0);
            ImGui::RadioButton("Computer Solve", &selected_mode, 1);

            if (ImGui::Button("Next ->")) gameState = GameState::AlgoSelection;
            if (ImGui::Button("Back")) gameState = GameState::DifficultySelection;
            break;

        case GameState::AlgoSelection:
            ImGui::Text("Select Solving Algorithm:");
            ImGui::RadioButton("All algos for benchmarking", &selected_algo, ALGO_ALL);
            ImGui::RadioButton("Backtracking", &selected_algo, ALGO_BACKTRACKING);

            if (ImGui::Button("Next ->")) {
                gameState = GameState::PlayingMode;
                game_started = true;
            }
            if (ImGui::Button("Back")) gameState = GameState::WhoPlaysSelection;
            break;

        case GameState::PlayingMode:
            if (selected_mode == 0) {
                ImGui::Text("Game Started!");
                if (game_started) {
                    generatePuzzle();
                    game_started = false;
                }
                renderPuzzleForUser(grid, givens);
            } else {
                if (game_started) {
                    generatePuzzle();
                    game_started = false;
                }

                renderPuzzleForAlgo(grid, givens);

                if (ImGui::Button("Solve")) {
                    gameState = GameState::AlgoSolving;
                    solvePuzzleByAlgo();  // Trigger solver
                }
            }

            if (ImGui::Button("Return to Menu")) gameState = GameState::DifficultySelection;
            break;

        case GameState::AlgoSolving:
            if (game_solving) {
                ImGui::Text("Solving... Please wait.");
            } else if (game_solved) {
                renderPuzzleForAlgo(grid, givens);
                renderTime();
            }

            if (ImGui::Button("Return to Menu")) gameState = GameState::DifficultySelection;
            break;

        default:
            gameState = GameState::DifficultySelection;
            break;
    }

    ImGui::End();
}
