cmake_minimum_required(VERSION 3.14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(CodeNebula VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)
find_package(nlohmann_json REQUIRED)

# ImGui + rlImGui Library
set(IMGUI_DIR "vendor/imgui")
set(RLIMGUI_DIR "vendor/rlImGui")

# Create a library target named "ImGuiLib"
add_library(ImGuiLib STATIC
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${RLIMGUI_DIR}/rlImGui.cpp"
)

target_include_directories(ImGuiLib PUBLIC 
    "${IMGUI_DIR}" 
    "${RLIMGUI_DIR}"
)

target_link_libraries(ImGuiLib PUBLIC raylib)

add_executable(CodeNebula 
    src/main.cpp 
    src/Simulation.cpp
    src/Graph.cpp
)

target_link_libraries(CodeNebula PRIVATE ImGuiLib nlohmann_json::nlohmann_json)