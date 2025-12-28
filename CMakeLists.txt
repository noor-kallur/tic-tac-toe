cmake_minimum_required(VERSION 3.10)
project(TicTacToeApp)

# 1. Use modern C++
set(CMAKE_CXX_STANDARD 17)

# 2. Tell CMake where the ImGui toolbox is
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

# 3. List EVERY file needed to make the app (The "Shopping List")
add_executable(TicTacToeApp 
    main.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# 4. Tell the compiler where to look for "Header" files
target_include_directories(TicTacToeApp PRIVATE 
    ${IMGUI_DIR} 
    ${IMGUI_DIR}/backends
)

# 5. Link the system libraries (The "Heavy Lifting")
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
target_link_libraries(TicTacToeApp glfw OpenGL::GL)