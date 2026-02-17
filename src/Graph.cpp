#include "Graph.h"

using json = nlohmann::json;

float Graph::GetRandomPosition() {
    float rand = center.x + (float)GetRandomValue((int)SPAWN_LOWER, (int)SPAWN_UPPER);
    return rand;
}

void Graph::LoadGraph(Simulation& sim, const std::string& filename) {
    std::ifstream file(filename);
    // read json file
    if (file.is_open()) {
        json data;
        file >> data;

        // clear old data
        sim.nodes.clear();
        sim.edges.clear();

        for (const auto& entry : data["nodes"]) {
            sim.nodes.emplace_back(entry["id"], entry["name"], GetRandomPosition(), GetRandomPosition());
        }

        for (const auto& entry : data["edges"]) {
            sim.edges.emplace_back(entry["source"], entry["target"]);
        }
    } else {
        std::cerr << "File not found!" << std::endl;
        return;
    }    
}