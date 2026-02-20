#include "Graph.h"
#include "Node.h"
#include "Simulation.h"

#include <httplib.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <nlohmann/json.hpp>
#include <raylib.h>
#include <raymath.h>
#include <rlImGui.h>

#include <algorithm>
#include <string>
#include <future>
#include <chrono>

using json = nlohmann::json;

struct AppContext {
    Camera2D camera;
    Simulation sim;
    Graph graph;
    std::string github_url;
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int radius = 8.0f;
    const float minZoom = 0.1f;
    const float maxZoom = 5.0f;
    const float zoomReductionSpeed = 10.0f;
};


static bool DrawUserInputBox(std::string& github_url);
static void DoControlMenu(Simulation& sim);
static void DoNodeInspector(Node& node);
static void DrawDirectedEdge(Vector2 start, Vector2 end, float nodeRadius, Color color);
void UpdateDrawFrame(void* arg);
std::string getGraph(std::string github_url, httplib::Client& cli);

static std::future<std::string> graphFuture;
static bool isFetching = false;
httplib::Client cli("localhost", 8000); 

static bool DrawUserInputBox(std::string& github_url) {
    std::string github_prefix = "https://github.com/";
    bool shouldFetch = false;
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::Begin("Github URL");
    ImGui::InputText("URL:", &github_url);
    
    if (ImGui::Button("Visualize") && github_url.starts_with(github_prefix)) {
        shouldFetch = true;
    }
    
    ImGui::End();
    return shouldFetch;
}

static void DoControlMenu(Simulation& sim)
{
    ImGui::SetNextWindowPos(ImVec2(10, 100), ImGuiCond_FirstUseEver);
	ImGui::Begin("Physics Constants");
    ImGui::SliderFloat("Repulsion Force", &sim.REPULSION_FORCE, 0.0f, 5000.0f);
    ImGui::SliderFloat("Target Length", &sim.TARGET_LENGTH, 10.0f, 500.0f);
    ImGui::SliderFloat("Spring K", &sim.SPRING_K, 0.0f, 2.0f);
    ImGui::End();
}

static void DoNodeInspector(Node& node) {
    ImGui::Begin("File Information");
    ImGui::Text("File Name: %s", node.name.c_str());
    ImGui::Text("Indegree: %d", node.indegree);
    ImGui::Text("Outdegree: %d", node.outdegree);
    ImGui::End();
}

void DrawDirectedEdge(Vector2 start, Vector2 end, float nodeRadius, Color color) {
    Vector2 delta = { end.x - start.x, end.y - start.y };
    float length = sqrt(delta.x*delta.x + delta.y*delta.y);

    if (length < nodeRadius * 2) return; 

    Vector2 direction = { delta.x / length, delta.y / length };

    // --- MATH SECTION ---
    Vector2 startSurface = { 
        start.x + (direction.x * nodeRadius), 
        start.y + (direction.y * nodeRadius) 
    };
    
    Vector2 endSurface = { 
        end.x - (direction.x * nodeRadius), 
        end.y - (direction.y * nodeRadius) 
    };

    DrawLineV(startSurface, endSurface, color);

    float arrowSize = 6.0f;
    float arrowAngle = 30.0f * DEG2RAD;
    float angle = atan2(delta.y, delta.x);

    Vector2 p1 = {
        endSurface.x - arrowSize * cos(angle - arrowAngle),
        endSurface.y - arrowSize * sin(angle - arrowAngle)
    };
    
    Vector2 p2 = {
        endSurface.x - arrowSize * cos(angle + arrowAngle),
        endSurface.y - arrowSize * sin(angle + arrowAngle)
    };

    DrawTriangle(endSurface, p2, p1, color);
}

std::string getGraph(std::string github_url, httplib::Client& cli) {
    json data = {
        {"url", github_url}
    };
    
    auto res = cli.Post("/process", data.dump(), "application/json");
    printf("Fetching URL: %s\n", github_url.c_str());
    
    if (res && res->status == 200) {
        return res->body; 
    } else {
        printf("HTTP Error or bad response!\n");
        return "{}";
    }
}

void UpdateDrawFrame(void *arg) {
    AppContext* ctx = static_cast<AppContext*>(arg);
    Camera2D& camera = ctx->camera;
    Simulation& sim = ctx->sim;
    Graph& loader = ctx->graph;
    std::string& github_url = ctx->github_url;

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        camera.zoom += (wheel / ctx->zoomReductionSpeed);
        camera.zoom = std::clamp(camera.zoom, ctx->minZoom, ctx->maxZoom);
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
        DrawCircleV(sim.nodes[i].position, ctx->radius, nodeColor);
    }

    for (const auto& edge: sim.edges) {
        Vector2 start = sim.nodes[edge.sourceId].position;
        Vector2 end = sim.nodes[edge.targetId].position;
        DrawDirectedEdge(start, end , sim.NODE_RADIUS, BLUE);
    }

    DrawFPS(5, 5);
    EndMode2D();

    rlImGuiBegin();

    if (DrawUserInputBox(github_url) && !isFetching) {
        graphFuture = std::async(std::launch::async, getGraph, github_url, std::ref(cli));
        isFetching = true;
    }

    if (isFetching) {
        ImGui::Text("Fetching repository data... Please wait.");
        if (graphFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            std::string result = graphFuture.get(); 
            isFetching = false;
            sim.ResetGraph();
            loader.LoadGraphJSON(sim, result);
        }
    }

    DoControlMenu(sim);
    if (sim.hoverNodeId != -1) {
        ImGui::SetTooltip("%s", sim.nodes[sim.hoverNodeId].name.c_str());
    }
    if (sim.selectedNodeId != -1) {
        DoNodeInspector(sim.nodes[sim.selectedNodeId]);
    }
    rlImGuiEnd();

    EndDrawing();
}



int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int radius = 8.0f;
    const float minZoom = 0.1f;
    const float maxZoom = 5.0f;
    const float zoomReductionSpeed = 10.0f;
    std::string github_url; 
    const char* raw_endpoint = std::getenv("ENDPOINT_URL");

    if (raw_endpoint == nullptr) {
        printf("Endpoint environment variable not set\n");
        return 1;
    }

    std::string endpoint(raw_endpoint);
    
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

    AppContext appContext;
    appContext.camera = camera;
    appContext.sim = sim;
    appContext.graph = loader;
    appContext.github_url = github_url;
    
    while (!WindowShouldClose()) {
        UpdateDrawFrame(&appContext);
    }

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}