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
        int hoverNodeId = -1;

        static constexpr float NUMBER_OF_NODES = 100;
        float REPULSION_FORCE = 2000.0f;
        float TARGET_LENGTH = 150.0f; // We want connected nodes to be this far apart
        float SPRING_K = 0.02f;       // Stiffness (how hard it pulls)

        void Init();
        void Update(float dt, Vector2 worldMouse);

        

    private:
        Vector2 center = { 400.0f, 300.0f };
        float CalculateNodeDistance(const Node& p1, const Node& p2);
        float CalculateMouseDistance(Vector2 mouse, Node p1);
        static constexpr float SPAWN_LOWER = -10.0f; 
        static constexpr float SPAWN_UPPER = 10.0f;
        
        static constexpr int NODE_RADIUS = 8.0;

        int CheckMouseNodeDistance(Vector2 worldMouse);
        void ResetForce();
        void CenterNodes();
        void UpdateEdges();
        void ApplyRepulsionForces();
        void UpdatePositions(float dt);
        void UpdateDraggedNode(Vector2 worldMouse);
};

#endif