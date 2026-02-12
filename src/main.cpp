#include "Simulation.h"
#include "Node.h"
#include <raylib.h>

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int radius = 8.0f;

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
        float dt = GetFrameTime();
        sim.Update(dt);
        
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