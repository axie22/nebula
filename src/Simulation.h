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
        void Update(float dt);

    private:
        Vector2 center = { 400.0f, 300.0f };
        float CalculateDistance(Node p1, Node p2);
        const float MIN_POS = 100.0f; 
        const float MAX_POS = 500.0f;
        const int NUMBER_OF_NODES = 30;
};

#endif