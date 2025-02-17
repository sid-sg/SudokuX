#include "gameUI.hpp"

// #include <unordered_map>

#include "generatePuzzle.hpp"
#include "puzzleRender.hpp"

GUI::GUI() : io(ImGui::GetIO()), game_started(false), show_difficulty_menu(false), selected_difficulty(0), selected_mode(0), selected_size(9), window_flags(0) {
    grid = std::vector<std::vector<int>>(SIZE, std::vector<int>(SIZE, EMPTY));

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
    digHoles(grid, selected_difficulty);
    // for (int r = 0; r < SIZE; r++) {
    //     for (int c = 0; c < SIZE; c++) {
    //         std::cout << grid[r][c] << " ";
    //     }
    //     std::cout << "\n";
    // }
    // std::cout << "-------------------\n";
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

    switch (gameState) {
            // case GameState::SizeSelection:

            //     ImGui::Text("Choose sudoku board dimensions:");
            //     for (int i = 0; i < IM_ARRAYSIZE(dimensions); i++) {
            //         ImGui::RadioButton(dimensions[i], &selected_size, dimensionValue[i]);
            //     }

            //     ImGui::Text("%d", selected_size);

            //     if (ImGui::Button("Next ->")) {
            //         gameState = GameState::DifficultySelection;
            //     }
            //     break;

        case GameState::DifficultySelection:

            ImGui::Text("Select Difficulty:");
            ImGui::RadioButton("Easy", &selected_difficulty, 0);
            ImGui::RadioButton("Medium", &selected_difficulty, 1);
            ImGui::RadioButton("Hard", &selected_difficulty, 2);
            ImGui::RadioButton("Insane", &selected_difficulty, 3);

            if (ImGui::Button("Next ->")) {
                gameState = GameState::WhoPlaysSelection;
            }
            // if (ImGui::Button("Back")) {
            //     gameState = GameState::SizeSelection;
            // }
            break;
        case GameState::WhoPlaysSelection:

            ImGui::Text("Select Mode:");
            ImGui::RadioButton("I'll Play", &selected_mode, 0);
            ImGui::RadioButton("Computer Solve", &selected_mode, 1);

            ImGui::Spacing();
            if (ImGui::Button("Begin Puzzle")) {
                gameState = GameState::PlayingMode;
                game_started = true;
            }

            // Back button
            ImGui::SameLine();

            if (ImGui::Button("Back")) {
                gameState = GameState::DifficultySelection;
            }

            break;
        case GameState::PlayingMode:

            ImGui::Text("Game Started!");

            if (game_started) {
                generatePuzzle();
                // for (int r = 0; r < SIZE; r++) {
                //     for (int c = 0; c < SIZE; c++) {
                //         std::cout << grid[r][c] << " ";
                //     }
                //     std::cout << "\n";
                // }
                game_started = false;
            }

            renderPuzzle(grid);  

            if (ImGui::Button("Return to Menu")) {
                // gameState = GameState::SizeSelection;
                gameState = GameState::DifficultySelection;
                game_started = false;
            }

            break;
        default:
            // gameState = GameState::SizeSelection;
            gameState = GameState::DifficultySelection;
            break;
    }

    ImGui::End();
}