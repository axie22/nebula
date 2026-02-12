#include "Simulation.h"
#include "Node.h"
#include <raylib.h>

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int radius = 8.0f;

    InitWindow(screenWidth, screenHeight, "Nebula");
    SetTargetFPS(60);
    Simulation sim;
    sim.Init();
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        sim.Update(dt);

        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < sim.nodes.size(); ++i) {
            Color nodeColor = RAYWHITE;
            if (i == sim.draggedNodeId) {
                nodeColor = GREEN;
            }
            DrawCircleV(sim.nodes[i].position, radius, nodeColor);
        }

        DrawFPS(10, 10);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}