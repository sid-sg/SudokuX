#include <stdio.h>

#include "gameUI.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>  // Will drag system OpenGL headers

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static void glfw_error_callback(int error, const char* description) { fprintf(stderr, "GLFW Error %d: %s\n", error, description); }

// Global variables for emscripten main loop
GLFWwindow* g_window = nullptr;
GUI* g_gui = nullptr;
ImVec4 g_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

#ifndef __EMSCRIPTEN__
void setWindowIcon(GLFWwindow* window) {
    int width, height, channels;
    unsigned char* pixels = stbi_load("/home/sid/repos/sudoku/assets/icon/image.jpg", &width, &height, &channels, 4);
    if (pixels) {
        GLFWimage icon;
        icon.width = width;
        icon.height = height;
        icon.pixels = pixels;

        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(pixels);
    } else {
        printf("Failed to load icon: %s\n", stbi_failure_reason());
    }
}
#endif

// Main loop function for emscripten
#ifdef __EMSCRIPTEN__
void main_loop_iteration() {
    if (!g_window || !g_gui)
        return;

    glfwPollEvents();
    if (glfwGetWindowAttrib(g_window, GLFW_ICONIFIED) != 0) {
        return;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    g_gui->renderUI();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(g_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(g_clear_color.x * g_clear_color.w, g_clear_color.y * g_clear_color.w, g_clear_color.z * g_clear_color.w, g_clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(g_window);
}
#endif

int main(int, char**) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    g_window = glfwCreateWindow(1280, 720, "SudokuX", nullptr, nullptr);
    if (g_window == nullptr) return 1;
    glfwMakeContextCurrent(g_window);
    glfwSwapInterval(1);  // Enable vsync

    #ifndef __EMSCRIPTEN__
    setWindowIcon(g_window);
    #endif

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // Disable loading imgui.ini in WebAssembly builds
    #ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
    
    // For WebAssembly, we need to make sure we don't try to load fonts from the filesystem
    // Setup font manually with the default font
    io.Fonts->AddFontDefault();
    #endif

    g_gui = new GUI();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(g_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    g_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
#ifdef __EMSCRIPTEN__
    // Set the main loop for Emscripten
    emscripten_set_main_loop(main_loop_iteration, 0, true);
#else
    while (!glfwWindowShouldClose(g_window))
    {
        glfwPollEvents();
        if (glfwGetWindowAttrib(g_window, GLFW_ICONIFIED) != 0) {
            // Sleep to avoid high CPU usage when minimized
            // Note: ImGui_ImplGlfw_Sleep isn't a standard ImGui function, you might need to implement it
            // or replace with a platform-specific sleep function
            glfwWaitEvents();
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        g_gui->renderUI();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(g_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(g_clear_color.x * g_clear_color.w, g_clear_color.y * g_clear_color.w, g_clear_color.z * g_clear_color.w, g_clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(g_window);
    }
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    delete g_gui;
    glfwDestroyWindow(g_window);
    glfwTerminate();

    return 0;
}