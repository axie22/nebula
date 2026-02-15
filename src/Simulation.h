#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <cmath>
#include "Node.h"
#include "Edge.h"

class Simulation {
    public:
        std::vector<Node> nodes;
        std::vector<Edge> edges;
        int draggedNodeId = -1;

        void Init();
        void Update(float dt, Vector2 worldMouse);

    private:
        Vector2 center = { 400.0f, 300.0f };
        float CalculateNodeDistance(Node p1, Node p2);
        float CalculateMouseDistance(Vector2 mouse, Node p1);
        const float SPAWN_LOWER = -10.0f; 
        const float SPAWN_UPPER = 10.0f;
        const int NUMBER_OF_NODES = 10;
        const int NODE_RADIUS = 8.0;
        const float REPULSION_FORCE = 2000.0f;
        const float TARGET_LENGTH = 150.0f; // We want connected nodes to be this far apart
        const float SPRING_K = 0.02f;       // Stiffness (how hard it pulls)

        void CheckClickedNode(Vector2 worldMouse);
};

#endif