#ifndef SIMULATION_H
#define SIMULATION_H

#include <vector>
#include <cmath>
#include "Node.h"

class Simulation {
    public:
        std::vector<Node> nodes;
        int draggedNodeId = -1;

        void Init();
        void Update(float dt, Vector2 worldMouse);

    private:
        Vector2 center = { 400.0f, 300.0f };
        float CalculateNodeDistance(Node p1, Node p2);
        float CalculateMouseDistance(Vector2 mouse, Node p1);
        const float MIN_POS = 100.0f; 
        const float MAX_POS = 500.0f;
        const int NUMBER_OF_NODES = 30;
        const int NODE_RADIUS = 8.0;

        void CheckClickedNode(Vector2 worldMouse);
};

#endif