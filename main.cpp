#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <queue>
#include <iostream>
#include <cmath> // For sinf

#define GAME_TITLE "Tic Tac Toe - Smooth"

int checkWinner(int a[9], int m[8][3]) {
    for (int i = 0; i < 8; i++) {
        if ((a[m[i][0]] != 0) && (a[m[i][0]] == a[m[i][1]]) && (a[m[i][0]] == a[m[i][2]]))
            return a[m[i][0]];
    }
    return 0;
}

int main() {
    if (glfwInit() == false) return -1;
    // 1. Set the initial size (Width, Height)
    GLFWwindow* window = glfwCreateWindow(450, 600, GAME_TITLE, NULL, NULL);

    // 2. To prevent the user from dragging the edges and breaking your layout:
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); 

    // 3. If you want to change the size AFTER the window is already open:
    glfwSetWindowSize(window, 500, 700);

    if (window == NULL) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // --- STEP 1: FIX PIXELATION ---
    // Load a real font. Change the path to a valid .ttf on your Linux system.
    // Standard Arch/Ubuntu path example:
    ImFont* mainFont = io.Fonts->AddFontFromFileTTF("../fonts/AdwaitaMono-Bold.ttf", 20.0f);
    if(mainFont == nullptr){
        std::cout<<"Font Failed"<<std::endl;
    }
    if (!mainFont) mainFont = io.Fonts->AddFontDefault(); 

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    bool turn = false;
    int board[9] = {0};
    
    // --- STEP 2: ANIMATION STATE ---
    float opacities[9] = {0.0f}; // Stores 0.0 (invisible) to 1.0 (fully drawn)
    float animSpeed = 5.0f;      // How fast the fade happens
    
    std::queue<int> x_q, o_q;
    int winCondition[8][3] = { {0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6} };

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        float dt = io.DeltaTime; // Time elapsed since last frame

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        // Forces the ImGui window to cover the entire GLFW window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2((float)display_w, (float)display_h));

        // Use these flags to hide the title bar and background for a "seamless" look
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | 
                                ImGuiWindowFlags_NoResize | 
                                ImGuiWindowFlags_NoMove | 
                                ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("Game Board", nullptr, flags);

        int ret = checkWinner(board, winCondition);

        // --- STEP 3: UPDATE OPACITIES (Lerping) ---
        for (int i = 0; i < 9; i++) {
            if (board[i] != 0) {
                if (opacities[i] < 1.0f) opacities[i] += dt * animSpeed;
            } else {
                if (opacities[i] > 0.0f) opacities[i] -= dt * animSpeed;
            }
        }

        if (ret != 0) {
            ImGui::Text("Player %s Won!", (ret == 1 ? "X" : "O"));
            if (ImGui::Button("Reset Game")) {
                for (int i = 0; i < 9; i++) board[i] = 0;
                while(!x_q.empty()) x_q.pop();
                while(!o_q.empty()) o_q.pop();
            }
        } else {
            ImGui::Text("Turn: Player %s", (turn ? "X" : "O"));
        }

        ImGui::Separator();
        
        // Pulse logic for the expiring piece
        float pulse = (sinf((float)ImGui::GetTime() * 6.0f) * 0.2f) + 0.7f;

        ImGui::PushFont(mainFont); // Switch to the high-quality font
        for (int i = 0; i < 9; i++) {
            const char* label = (board[i] == 1) ? "X" : (board[i] == 2) ? "O" : " ";
            
            // --- STEP 4: COLOR & ALPHA INTERPOLATION ---
            ImVec4 color = (board[i] == 1) ? ImVec4(1.0f, 0.3f, 0.3f, 1.0f) : ImVec4(0.0f, 1.0f, 0.0f, 1.0f);;
            float alpha = opacities[i];

            // Check if this is the oldest piece (next to vanish)
            bool isOldest = (board[i] == 1 && x_q.size() == 3 && x_q.front() == i) || 
                            (board[i] == 2 && o_q.size() == 3 && o_q.front() == i);
            
            if (isOldest) alpha *= pulse; // Apply pulse to alpha

            color.w = alpha; // Apply final animated alpha to color

            char buttonId[16];
            sprintf(buttonId, "###btn%d", i);

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            if (ImGui::Button(buttonId, ImVec2(110, 110))) {
                if (board[i] == 0 && ret == 0) {
                    if (turn) { board[i] = 1; x_q.push(i); if (x_q.size() > 3) { board[x_q.front()] = 0; x_q.pop(); } }
                    else      { board[i] = 2; o_q.push(i); if (o_q.size() > 3) { board[o_q.front()] = 0; o_q.pop(); } }
                    turn = !turn;
                }
            }
            
            // Re-render label on top of button for better centering control
            // (Button text can sometimes be offset at large scales)
            ImVec2 pos = ImGui::GetItemRectMin();
            ImVec2 size = ImGui::GetItemRectSize();
            ImGui::GetWindowDrawList()->AddText(mainFont, 70.0f, 
                ImVec2(pos.x + (size.x/2) - 20, pos.y + (size.y/2) - 35), 
                ImColor(color), label);

            ImGui::PopStyleColor();

            if ((i + 1) % 3 != 0) ImGui::SameLine();
        }
        ImGui::PopFont();
        ImGui::End();

        // --- RENDER ---
        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup...
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}