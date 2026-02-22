cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(CodeNebula VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(PLATFORM "Web" CACHE STRING "Force Raylib to Web platform" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Skip raylib examples" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "Skip raylib games" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)

# 2. Download nlohmann_json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# ImGui + rlImGui Library
set(IMGUI_DIR "vendor/imgui")
set(RLIMGUI_DIR "vendor/rlImGui")

add_library(ImGuiLib STATIC
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"
    "${RLIMGUI_DIR}/rlImGui.cpp"
    "${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp"
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

target_link_libraries(CodeNebula PRIVATE ImGuiLib nlohmann_json)

if (EMSCRIPTEN)
    # Tell Emscripten to output a playable .html file instead of just raw WASM
    set_target_properties(CodeNebula PROPERTIES SUFFIX ".html")
    
    # Give Emscripten extra memory and enable fetch
    target_link_options(CodeNebula PRIVATE -sFETCH=1 -sALLOW_MEMORY_GROWTH=1 -sUSE_GLFW=3)
    target_compile_definitions(CodeNebula PRIVATE ENDPOINT_URL="http://localhost:8000")
endif()