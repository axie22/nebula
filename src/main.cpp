#include "Simulation.h"
#include "Node.h"
#include "raymath.h"
#include <algorithm>
#include <raylib.h>

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
        for (int i = 0; i < sim.nodes.size(); ++i) {
            Color nodeColor = RAYWHITE;
            if (i == sim.draggedNodeId) {
                nodeColor = GREEN;
            }
            DrawCircleV(sim.nodes[i].position, radius, nodeColor);
        }

        DrawFPS(10, 10);
        EndMode2D();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}