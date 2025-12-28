#include "imgui.h"              // The core ImGui logic
#include "imgui_impl_glfw.h"    // ImGui talking to the window
#include "imgui_impl_opengl3.h" // ImGui talking to the graphics card
#include <GLFW/glfw3.h>         // The actual window creator
#include <queue>

#include <iostream>
#define GAME_TITLE "Tic Tac Toe"

int checkWinner(int a[9], int m[8][3]){
    for(int i = 0; i < 8; i++)
    {
        if ( (a[m[i][0]] != 0) &&
            (a[m[i][0]] == a[m[i][1]]) &&
            (a[m[i][0]] == a[m[i][2]]) )
        {
            return a[m[i][0]];
        }
        
    }
    return 0;

}
int main()
{
    if(glfwInit() == false) return -1;

    GLFWwindow* window = glfwCreateWindow(600, 500, GAME_TITLE, NULL, NULL);

    if(window == NULL){
        std::cout<<"Failed to crate a window"<<std::endl;
        glfwTerminate();
        return -1;
    }

    // connecting the window to GUI
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // cap it at 60 FPS

    // init imgui
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    bool turn = false;
    int board[9]= {0};
    std::queue<int> x_q;
    std::queue<int> o_q;


    int winCondition[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Horizontals
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // Verticals
        {0, 4, 8}, {2, 4, 6}             // Diagonals
    };
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // 1. START THE FRAME (Must happen before ANY ImGui calls)
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 2. DEFINE THE UI
        ImGui::SetNextWindowSize(ImVec2(350, 480), ImGuiCond_FirstUseEver);
        ImGui::Begin("Game Board");

        int ret = checkWinner(board, winCondition);

        if (ret != 0) {
            ImGui::SetWindowFontScale(2.0f);
            ImGui::Text("Player %s Won!", (ret == 1 ? "X" : "O"));
            ImGui::SetWindowFontScale(1.0f);

            if (ImGui::Button("Play Again?")) {
                for (int i = 0; i < 9; i++) board[i] = 0; // Fixed: i < 9
                turn = false;
            }
        }
        else {
    
            ImGui::Text("Turn: Player %s", (turn ? "X" : "O"));
        }

        ImGui::Separator();
        
        // Draw the 3x3 Grid
        ImGui::SetWindowFontScale(3.0f);
        for (int i = 0; i < 9; i++) { // Fixed: i < 9
            const char* label = " ";
            ImVec4 textColor = ImVec4(1, 1, 1, 1);

            if (board[i] == 1) {
                label = "X";
                textColor = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
            } else if (board[i] == 2) {
                label = "O";
                textColor = ImVec4(0.2f, 0.6f, 1.0f, 1.0f);
            }

            char buttonId[32];
            sprintf(buttonId, "%s###btn%d", label, i);
            // Only allow moves if the square is empty and no one has won yet
            if (board[i] == 1 && x_q.size() == 3 && x_q.front() == i) {
                textColor = ImVec4(1.0f, 0.5f, 0.5f, 0.5f); // Dim red for X
            } 
            else if (board[i] == 2 && o_q.size() == 3 && o_q.front() == i) {
                textColor = ImVec4(0.5f, 0.8f, 1.0f, 0.5f); // Dim blue for O
            }
            ImGui::PushStyleColor(ImGuiCol_Text, textColor);
            if (ImGui::Button(buttonId, ImVec2(100, 100))) {

                if (board[i] == 0 && ret == 0) {
                    if(turn==true)
                    {
                        board[i]=1;
                        x_q.push(i);
                        // delete the last x or o
                        if(x_q.size() > 3)
                        {
                            std::cout<<"x pop"<<std::endl;
                            board[x_q.front()] = 0;
                            x_q.pop();
                        }
                    }
                    else if(turn==false){
                        board[i]=2;
                        o_q.push(i);
                        if(o_q.size() > 3)
                        {
                            std::cout<<"o pop"<<std::endl;
                            board[o_q.front()] = 0;
                            o_q.pop();
                        }
                    }
                    turn = !turn;
                }
            }
            ImGui::PopStyleColor();

            // Wrap to new line every 3 buttons
            if ((i + 1) % 3 != 0) {
                ImGui::SameLine();
            }
        }
        ImGui::SetWindowFontScale(1.0f);
        ImGui::End();

        // 3. RENDERING
        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
    }
        
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;

}
