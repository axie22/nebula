#include "Simulation.h"
#include "Node.h"
#include <raylib.h>

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int radius = 10.0f;

    InitWindow(screenWidth, screenHeight, "Nebula");
    SetTargetFPS(60);
    Simulation sim;
    sim.Init();
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        sim.Update(dt);

        BeginDrawing();
        ClearBackground(BLACK);

        for (auto& node: sim.nodes) {
            DrawCircleV(node.position, radius, RAYWHITE);
        }

        DrawFPS(10, 10);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}