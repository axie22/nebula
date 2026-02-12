#include "Simulation.h"

void Simulation::Init() {
    // Initialize in random positions
    nodes.reserve(NUMBER_OF_NODES);

    for (int i = 0; i < NUMBER_OF_NODES; ++i) {
        float x = (float)GetRandomValue((int)MIN_POS, (int)MAX_POS);
        float y = (float)GetRandomValue((int)MIN_POS, (int)MAX_POS);
        nodes.emplace_back(i, x, y);
    }
}

float Simulation::CalculateNodeDistance(Node p1, Node p2) {
    float dx = p1.position.x - p2.position.x;
    float dy = p1.position.y - p2.position.y;

    float distSq = dx*dx + dy*dy;
    float distance = std::sqrt(distSq);
    return distance;
}

float Simulation::CalculateMouseDistance(Vector2 mouse, Node p1) {
    float dx = p1.position.x - mouse.x;
    float dy = p1.position.y - mouse.y;
    float distSq = dx * dx + dy + dy;
    return std::sqrt(distSq);
}

void Simulation::CheckClickedNode() {
    for (int i = 0; i < nodes.size(); ++i) {
        if (CalculateMouseDistance(GetMousePosition(), nodes[i]) < NODE_RADIUS) {
            draggedNodeId = i;
            break;
        }
    }
}

void Simulation::Update(float dt) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        CheckClickedNode();
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        draggedNodeId = -1;
    }

    for (auto& node : nodes) {
        node.force = {0.0f, 0.0f};
    }

    for (int i = 0; i < nodes.size(); ++i) {
        for (int j = i + 1; j < nodes.size(); ++j) {
            float dx = nodes[i].position.x - nodes[j].position.x;
            float dy = nodes[i].position.y - nodes[j].position.y;
            float distance = CalculateNodeDistance(nodes[i], nodes[j]);

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

    if (draggedNodeId != -1) {
        nodes[draggedNodeId].position = GetMousePosition();
        nodes[draggedNodeId].force = { 0.0f, 0.0f };
        nodes[draggedNodeId].velocity = { 0.0f, 0.0f };
    }
}
