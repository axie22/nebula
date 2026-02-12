#pragma once
#include <vector>
#include <cmath>
#include "Node.h"

class Simulation {
public:
    std::vector<Node> nodes;

    void Init() {
        const float MIN_POS = 100.0f; 
        const float MAX_POS = 500.0f;

        for (int i = 0; i < 30; ++i) {
            Node newNode;
            newNode.position.x = (float)GetRandomValue((int)MIN_POS, (int)MAX_POS);
            newNode.position.y = (float)GetRandomValue((int)MIN_POS, (int)MAX_POS);
            newNode.velocity = {0, 0};
            newNode.force = {0, 0};
            newNode.id = i; 
            nodes.push_back(newNode);
        }
    }

    void Update(float dt) {
        for (auto& node : nodes) {
            node.force = {0.0f, 0.0f};
        }

        for (int i = 0; i < nodes.size(); ++i) {
            for (int j = i + 1; j < nodes.size(); ++j) {
                float dx = nodes[i].position.x - nodes[j].position.x;
                float dy = nodes[i].position.y - nodes[j].position.y;

                float distSq = dx*dx + dy*dy;
                float distance = std::sqrt(distSq);

                if (distance < 1.0f) distance = 1.0f;

                // Calculate Force Magnitude (Coulomb's Law: F = k / r)
                float forceMagnitude = 2000.0f / distance;

                // Create the Force Vector (Direction * Magnitude)
                float fx = (dx / distance) * forceMagnitude;
                float fy = (dy / distance) * forceMagnitude;

                // Apply to both (Newton's 3rd Law)
                nodes[i].force.x += fx;
                nodes[i].force.y += fy;
                
                nodes[j].force.x -= fx;
                nodes[j].force.y -= fy;
            }
        }

        Vector2 center = { 400.0f, 300.0f };

        for (auto& node : nodes) {
            float dirX = center.x - node.position.x;
            float dirY = center.y - node.position.y;

            node.force.x += dirX * 0.5f;
            node.force.y += dirY * 0.5f;
        }


        for (auto& node : nodes) {
            node.velocity.x += node.force.x * dt;
            node.velocity.y += node.force.y * dt;

            node.velocity.x *= 0.95f; 
            node.velocity.y *= 0.95f;

            node.position.x += node.velocity.x * dt;
            node.position.y += node.velocity.y * dt;
        }
    }
};