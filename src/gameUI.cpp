#include "gameUI.hpp"

// #include <unordered_map>

#include "generatePuzzle.hpp"
#include "puzzleRender.hpp"

GUI::GUI() : io(ImGui::GetIO()), game_started(false), game_solving(false), game_solved(false), selected_difficulty(0), selected_mode(0), selected_algo(ALGO_ALL), window_flags(0) {
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
    // for (int r = 0; r < SIZE; r++) {
    //     for (int c = 0; c < SIZE; c++) {
    //         std::cout << grid[r][c] << " ";
    //     }
    //     std::cout << "\n";
    // }
    // std::cout << "-------------------\n";
    if (!fillGrid(grid)) {
        std::cout << "Failed to generate complete grid!\n";
    }
    // for (int r = 0; r < SIZE; r++) {
    //     for (int c = 0; c < SIZE; c++) {
    //         std::cout << grid[r][c] << " ";
    //     }
    //     std::cout << "\n";
    // }
    // std::cout << "-------------------\n";
    digHoles(grid, givens, selected_difficulty);
    // for (int r = 0; r < SIZE; r++) {
    //     for (int c = 0; c < SIZE; c++) {
    //         std::cout << grid[r][c] << " ";
    //     }
    //     std::cout << "\n";
    // }
    // std::cout << "-------------------\n";
}

void GUI::solvePuzzleByAlgo() {
    switch (selected_algo) {
        case ALGO_ALL:
            // solver::backtracking(grid);
            // solver::bfs(grid);
            // solver::dfs(grid);
            break;
        case ALGO_BACKTRACKING:
            backtracking::solve(grid);
            game_solved = true;
            game_solving = false;
            break;
        case ALGO_BFS:
            // bfs::solve(grid);
            break;
        case ALGO_DFS:
            // dfs::solve(grid);
            break;
        default:
            break;
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

    for (int i = 0; i < 5; i++) {
        ImGui::Spacing();
    }
    ImGui::Separator();

    for (int i = 0; i < 10; i++) {
        ImGui::Spacing();
    }

    // static const char* dimensions[] = {"4x4", "9x9", "16x16", "25x25"};
    // std::unordered_map<int, int> dimensionValue = {{0,4}, {1, 9}, {2, 16}, {3, 25}};

    GameState prevState = GameState::DifficultySelection;

    switch (gameState) {
        case GameState::DifficultySelection:
            ImGui::Text("Select Difficulty:");
            ImGui::RadioButton("Easy", &selected_difficulty, 0);
            ImGui::RadioButton("Medium", &selected_difficulty, 1);
            ImGui::RadioButton("Hard", &selected_difficulty, 2);
            ImGui::RadioButton("Evil", &selected_difficulty, 3);
            ImGui::RadioButton("Impossible", &selected_difficulty, 4);

            if (ImGui::Button("Next ->")) {
                prevState = gameState;
                gameState = GameState::WhoPlaysSelection;
            }
            break;

        case GameState::WhoPlaysSelection:
            ImGui::Text("Select Mode:");
            ImGui::RadioButton("I'll Play", &selected_mode, 0);
            ImGui::RadioButton("Computer Solve", &selected_mode, 1);

            ImGui::Spacing();
            if (selected_mode == 0) {
                if (ImGui::Button("Start game")) {
                    prevState = gameState;
                    gameState = GameState::PlayingMode;
                    game_started = true;
                }
            } else {
                if (ImGui::Button("Next ->")) {
                    prevState = gameState;
                    gameState = GameState::AlgoSelection;
                }
            }

            if (ImGui::Button("Back")) {
                gameState = prevState;
            }
            break;

        case GameState::AlgoSelection:
            ImGui::Text("Select Solving Algorithm:");

            ImGui::RadioButton("All algos for benchmarking", &selected_algo, ALGO_ALL);
            ImGui::RadioButton("Backtracking", &selected_algo, ALGO_BACKTRACKING);
            ImGui::RadioButton("BFS", &selected_algo, ALGO_BFS);
            ImGui::RadioButton("DFS", &selected_algo, ALGO_DFS);

            ImGui::Spacing();
            if (ImGui::Button("Next ->")) {
                prevState = gameState;
                gameState = GameState::PlayingMode;
                game_started = true;
                game_solved = false;
                game_solving = false;
            }

            if (ImGui::Button("Back")) {
                gameState = prevState;
            }
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
                    game_solved = false;
                    game_solving = false;
                }

                renderPuzzleForAlgo(grid, givens);

                if (ImGui::Button("Solve")) {
                    prevState = gameState;
                    gameState = GameState::AlgoSolving;
                    game_solving = true;
                    game_solved = false;
                }
            }
            if (ImGui::Button("Return to Menu")) {
                prevState = gameState;
                gameState = GameState::DifficultySelection;
                game_started = false;
            }
            break;
        case GameState::AlgoSolving:
            if (game_solved) {
                ImGui::Text("Solved!");
                renderPuzzleForAlgo(grid, givens);
            } else if (game_solving) {
                ImGui::Text("Solving...");
                solvePuzzleByAlgo();
                game_solved = true;
                game_solving = false;
            }

            if (ImGui::Button("Return to Menu")) {
                gameState = GameState::DifficultySelection;
                game_started = false;
                game_solved = false;
                game_solving = false;
            }
            break;

        default:
            gameState = GameState::DifficultySelection;
            break;
    }

    ImGui::End();
}