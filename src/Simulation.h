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
};

#endif