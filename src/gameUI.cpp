#include "gameUI.hpp"

#include <algorithm>
#include <array>
#include <chrono>

#include "generatePuzzle.hpp"
#include "puzzleRender.hpp"

GUI::GUI() : io(ImGui::GetIO()), solverRunning(false), game_started(false), game_solving(false), game_solved(false), selected_difficulty(0), selected_mode(0), selected_algo(ALGO_ALL), timeTaken(0), window_flags(0) {
    grid = std::vector<std::vector<int>>(SIZE, std::vector<int>(SIZE, EMPTY));
    givens = std::vector<std::vector<bool>>(SIZE, std::vector<bool>(SIZE, true));

    (void)this->io;
    this->io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    mainFont = this->io.Fonts->AddFontFromFileTTF("../assets/font/BebasNeue-Regular.ttf", 40.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
    headingFont = this->io.Fonts->AddFontFromFileTTF("../assets/font/Boldonse-Regular.ttf", 100.0f, nullptr, io.Fonts->GetGlyphRangesDefault());

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

        if (selected_algo == ALGO_ALL) {
            std::vector<std::pair<std::string, double>> algo_times;  // <algorithm name, time taken in milliseconds>
            std::vector<std::vector<int>> solved_grid;

            // Backtracking
            auto algo_start = std::chrono::high_resolution_clock::now();
            solved_grid = grid;
            backtracking::solve(solved_grid);
            auto algo_end = std::chrono::high_resolution_clock::now();
            algo_times.emplace_back("Backtracking", std::chrono::duration<double, std::milli>(algo_end - algo_start).count());

            // Simulated Annealing
            algo_start = std::chrono::high_resolution_clock::now();
            std::vector<std::vector<int>> sa_grid = grid;
            simulatedAnnealing::solve(sa_grid, givens);
            algo_end = std::chrono::high_resolution_clock::now();
            algo_times.emplace_back("Simulated Annealing", std::chrono::duration<double, std::milli>(algo_end - algo_start).count());

            // Dancing Links
            algo_start = std::chrono::high_resolution_clock::now();
            solved_grid = grid;
            DLX::solve(solved_grid);
            algo_end = std::chrono::high_resolution_clock::now();
            algo_times.emplace_back("Dancing Links", std::chrono::duration<double, std::milli>(algo_end - algo_start).count());

            sort(algo_times.begin(), algo_times.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

            timeResults = algo_times;
            grid = solved_grid;
        } else {
            auto algo_start = std::chrono::high_resolution_clock::now();

            // std::vector<std::vector<int>> temp_grid = grid;
            switch (selected_algo) {
                case ALGO_BACKTRACKING:
                    backtracking::solve(grid);
                    break;
                case ALGO_SIMULATED_ANNEALING:
                    simulatedAnnealing::solve(grid, givens);
                    break;
                case ALGO_DLX:
                    std::cout << "DLX solving started\n";
                    DLX::solve(grid);
                    std::cout << "DLX solving finished\n";

                    break;
                default:
                    break;
            }

            auto algo_end = std::chrono::high_resolution_clock::now();
            timeTaken = std::chrono::duration<double, std::milli>(algo_end - algo_start).count();

            // grid = temp_grid;  // Update grid with solution
        }

        game_solved = true;
        game_solving = false;
        solverRunning.store(false);

        // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
}

void GUI::renderTime() {
    ImGui::Spacing();
    ImGui::Spacing();

    if (selected_algo == ALGO_ALL) {
        ImGui::TextUnformatted("Time taken by each algorithm:");
        ImGui::Spacing();
        ImGui::Spacing();

        for (const auto& result : timeResults) {
            if (result.second >= 1000) {
                ImGui::Text("%s: %.2f seconds", result.first.c_str(), result.second / 1000.0);
            } else if (result.second >= 1.0) {
                ImGui::Text("%s: %.2f milliseconds", result.first.c_str(), result.second);
            } else if (result.second >= 0.001) {
                ImGui::Text("%s: %.2f microseconds", result.first.c_str(), result.second * 1000.0);
            } else {
                ImGui::Text("%s: %.2f nanoseconds", result.first.c_str(), result.second * 1000000.0);
            }
        }
    } else {
        if (timeTaken >= 1000) {
            ImGui::Text("Time taken: %.2f seconds", timeTaken / 1000.0);
        } else if (timeTaken >= 1.0) {
            ImGui::Text("Time taken: %.2f milliseconds", timeTaken);
        } else if (timeTaken >= 0.001) {
            ImGui::Text("Time taken: %.2f microseconds", timeTaken * 1000.0);
        } else {
            ImGui::Text("Time taken: %.3f nanoseconds", timeTaken * 1000000.0);
        }
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

template <typename F>
void CenterComponent(F&& componentFunc) {
    // Save the current cursor position
    float startY = ImGui::GetCursorPosY();

    // Begin invisible group to calculate width
    ImGui::BeginGroup();
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.0f);
    componentFunc();
    ImGui::PopStyleVar();
    ImGui::EndGroup();

    // Get width of the component we just rendered invisibly
    float componentWidth = ImGui::GetItemRectSize().x;

    // Calculate centered position
    float windowWidth = ImGui::GetWindowWidth();
    float posX = (windowWidth - componentWidth) * 0.5f;

    // Reset cursor position to before the invisible render
    ImGui::SetCursorPos(ImVec2(posX, startY));

    // Actually render the component
    componentFunc();
}

void GUI::renderUI() {
    if (windowSize.x != io.DisplaySize.x * 0.8f || windowSize.y != io.DisplaySize.y * 0.95f) {
        windowSize = ImVec2(io.DisplaySize.x * 0.8f, io.DisplaySize.y * 0.95f);
        center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);

        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    }

    ImGui::Begin("SudokuX", NULL, window_flags);

    CenterComponent([&]() {
        ImGui::PushFont(headingFont);
        ImGui::TextUnformatted("SudokuX");
        ImGui::PopFont();
    });

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    switch (gameState) {
        case GameState::DifficultySelection:
            grid.assign(SIZE, std::vector<int>(SIZE, EMPTY));
            givens.assign(SIZE, std::vector<bool>(SIZE, true));

            ImGui::TextUnformatted("Select Difficulty:");

            static constexpr int totalDifficulty = 5;
            static const std::array<std::string, totalDifficulty> difficultyLevels = {"Easy", "Medium", "Hard", "Evil", "Impossible"};

            for (int i = 0; i < totalDifficulty; ++i) {
                ImGui::RadioButton(difficultyLevels[i].c_str(), &selected_difficulty, i);
            }

            if (ImGui::Button("Next ->")) gameState = GameState::WhoPlaysSelection;
            break;

        case GameState::WhoPlaysSelection:
            ImGui::TextUnformatted("Select Mode:");
            ImGui::RadioButton("I'll Play", &selected_mode, 0);
            ImGui::RadioButton("Computer Solve", &selected_mode, 1);

            if (ImGui::Button("Next ->")) gameState = GameState::AlgoSelection;
            if (ImGui::Button("Back")) gameState = GameState::DifficultySelection;
            ImGui::SameLine();

            break;

        case GameState::AlgoSelection:
            ImGui::TextUnformatted("Select Solving Algorithm:");

            static constexpr int totalAlgos = 4;
            static const std::array<std::string, totalAlgos> solvingAlgos = {"All algos for benchmarking", "Backtracking", "Simulated Annealing", "Dancing Li40s"};
            for (int i = 0; i < totalAlgos; ++i) {
                ImGui::RadioButton(solvingAlgos[i].c_str(), &selected_algo, i);
            }

            if (ImGui::Button("Next ->")) {
                gameState = GameState::PlayingMode;
                game_started = true;
            }
            if (ImGui::Button("Back")) gameState = GameState::WhoPlaysSelection;
            ImGui::SameLine();
            break;

        case GameState::PlayingMode:
            if (selected_mode == 0) {
                ImGui::TextUnformatted("Game Started!");
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
                    solvePuzzleByAlgo();
                }
            }

            if (ImGui::Button("Return to Menu")) gameState = GameState::DifficultySelection;
            ImGui::SameLine();
            break;

        case GameState::AlgoSolving:
            if (game_solving) {
                ImGui::TextUnformatted("Solving... Please wait.");
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
