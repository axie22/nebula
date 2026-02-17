#include "Simulation.h"
#include "Node.h"
#include "Graph.h"

#include "imgui.h"
#include "rlImGui.h"


#include "raymath.h"
#include <algorithm>
#include <raylib.h>

static void DoControlMenu(Simulation& sim);


static void DoControlMenu(Simulation& sim)
{
	ImGui::Begin("Physics Constants");
    ImGui::SliderFloat("Repulsion Force", &sim.REPULSION_FORCE, 0.0f, 5000.0f);
    ImGui::SliderFloat("Target Length", &sim.TARGET_LENGTH, 10.0f, 500.0f);
    ImGui::SliderFloat("Spring K", &sim.SPRING_K, 0.0f, 2.0f);
    ImGui::End();
}



int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int radius = 8.0f;
    const float minZoom = 0.1f;
    const float maxZoom = 5.0f;
    const float zoomReductionSpeed = 10.0f;

    // Raylib setup
    InitWindow(screenWidth, screenHeight, "Nebula");
    SetTargetFPS(60);
    Camera2D camera = { 0 };
    camera.target = { screenWidth / 2, screenHeight / 2};
    camera.offset = { screenWidth / 2, screenHeight / 2};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    // Simulation setup
    Simulation sim;
    sim.Init();

    // ImGui setup
    rlImGuiSetup(true); 

    // Load Graph
    Graph loader;
    loader.LoadGraph(sim, "../graph.json");
    
    while (!WindowShouldClose()) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            camera.zoom += (wheel / zoomReductionSpeed);
            camera.zoom = std::clamp(camera.zoom, minZoom, maxZoom);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 delta = GetMouseDelta();
            Vector2 scaledDelta = Vector2Scale(delta, 1.0f / camera.zoom);
            camera.target = Vector2Subtract(camera.target, scaledDelta);
        }

        float dt = GetFrameTime();
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        sim.Update(dt, mouseWorldPos);
        
        BeginDrawing();
        BeginMode2D(camera);
        ClearBackground(BLACK);

        for (const auto& edge: sim.edges) {
            Vector2 start = sim.nodes[edge.sourceId].position;
            Vector2 end = sim.nodes[edge.targetId].position;
            DrawLineV(start, end, BLUE);
        }

        for (int i = 0; i < sim.nodes.size(); ++i) {
            Color nodeColor = RAYWHITE;
            if (i == sim.draggedNodeId) {
                nodeColor = GREEN;
            }
            DrawCircleV(sim.nodes[i].position, radius, nodeColor);
        }

        DrawFPS(10, 10);
        EndMode2D();

        rlImGuiBegin();
        DoControlMenu(sim);
        rlImGuiEnd();

        EndDrawing();
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}