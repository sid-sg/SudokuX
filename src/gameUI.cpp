#include "gameUI.hpp"

#include <chrono>

#include "generatePuzzle.hpp"
#include "puzzleRender.hpp"

GUI::GUI() : io(ImGui::GetIO()), solverRunning(false), game_started(false), game_solving(false), game_solved(false), selected_difficulty(0), selected_mode(0), selected_algo(ALGO_ALL), timeTaken(0), window_flags(0) {
    grid = std::vector<std::vector<int>>(SIZE, std::vector<int>(SIZE, EMPTY));
    givens = std::vector<std::vector<bool>>(SIZE, std::vector<bool>(SIZE, true));

    (void)this->io;
    this->io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    mainFont = this->io.Fonts->AddFontFromFileTTF("../assets/font/BebasNeue-Regular.ttf", 24.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
    headingFont = this->io.Fonts->AddFontFromFileTTF("../assets/font/Bangers-Regular.ttf", 100.0f, nullptr, io.Fonts->GetGlyphRangesDefault());

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

void GUI::solvePuzzleByAlgo() {
    if (solverRunning.load()) return;

    solverRunning.store(true);

    game_solving = true;
    game_solved = false;
    timeTaken = 0;

    if (solverThread && solverThread->joinable()) {
        solverThread->join();
    }

    solverThread = std::make_unique<std::thread>([this]() {
        auto start = std::chrono::high_resolution_clock::now();

        switch (selected_algo) {
            case ALGO_ALL:
                break;
            case ALGO_BACKTRACKING:
                backtracking::solve(grid);
                break;
            default:
                break;
        }

        auto end = std::chrono::high_resolution_clock::now();
        timeTaken = std::chrono::duration<double, std::milli>(end - start).count();

        game_solved = true;
        game_solving = false;
        solverRunning.store(false);
    });
}

void GUI::renderTime() {
    if (timeTaken >= 1000) {
        double seconds = timeTaken / 1000.0;
        ImGui::Text("Time taken: %.2f s", seconds);
    } else {
        ImGui::Text("Time taken: %.4f ms", timeTaken);
    }
}

bool GUI::Spinner(const char* label, float radius, int thickness, const ImU32& color) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size(radius * 2, (radius + style.FramePadding.y) * 2);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id)) return false;

    // Render the spinner path
    window->DrawList->PathClear();

    int num_segments = 30;
    int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

    const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
    const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

    const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

    for (int i = 0; i < num_segments; i++) {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius, centre.y + ImSin(a + g.Time * 8) * radius));
    }

    window->DrawList->PathStroke(color, false, thickness);
    return true;
}

void GUI::renderUI() {
    ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.8f , io.DisplaySize.y * 0.95f));

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
                Spinner("##spinner", 20.0f, 4, ImGui::GetColorU32(ImVec4(1, 1, 1, 1)));  // White spinner

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
