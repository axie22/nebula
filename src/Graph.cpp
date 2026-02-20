#include "Graph.h"

using json = nlohmann::json;

float Graph::GetRandomPosition() {
    float rand = center.x + (float)GetRandomValue((int)SPAWN_LOWER, (int)SPAWN_UPPER);
    return rand;
}

void Graph::LoadGraphJSON(Simulation& sim, std::string data) {
    json json_data;
    try {
        json_data = json::parse(data);
    } catch (json::parse_error& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return;
    }

    if (!json_data.contains("nodes") || !json_data.contains("edges")) {
        std::cout << "Data: " << data;
        std::cerr << "Invalid JSON format: missing nodes or edges." << std::endl;
        return;
    }

    for (const auto& entry : json_data["nodes"]) {
        sim.nodes.emplace_back(entry["id"], entry["name"], GetRandomPosition(), GetRandomPosition());
    }

    for (const auto& entry : json_data["edges"]) {
        sim.edges.emplace_back(entry["source"], entry["target"]);

        sim.nodes[entry["source"]].outdegree++;
        sim.nodes[entry["target"]].indegree++;
    }
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

            sim.nodes[entry["source"]].outdegree++;
            sim.nodes[entry["target"]].indegree++;
        }
    } else {
        std::cerr << "File not found!" << std::endl;
        return;
    }    
}