#ifndef GRAPH_H
#define GRAPH_H

#include <fstream>
#include <vector>
#include <iostream>
#include <nlohmann/json.hpp>

#include "Simulation.h"

class Graph {
    public:
        void LoadGraph(Simulation& sim, const std::string& filename);
        void LoadGraphJSON(Simulation& sim, std::string data);

    private:
        float GetRandomPosition();
        Vector2 center = { 400.0f, 300.0f };
        static constexpr float SPAWN_LOWER = -10.0f; 
        static constexpr float SPAWN_UPPER = 10.0f;
};


#endif