#include "Simulation.h"

void Simulation::Init() {
    // Initialize in random positions
    nodes.reserve(NUMBER_OF_NODES);
    edges.reserve(NUMBER_OF_NODES + 1);

    for (int i = 0; i < NUMBER_OF_NODES; ++i) {
        float x = center.x + (float)GetRandomValue((int)SPAWN_LOWER, (int)SPAWN_UPPER);
        float y = center.y + (float)GetRandomValue((int)SPAWN_LOWER, (int)SPAWN_UPPER);
        nodes.emplace_back(i, "dummy", x, y);
    }

    for (int i = 0; i < nodes.size() - 1; ++i) {
        edges.emplace_back(i, i + 1);
    }
    edges.emplace_back((int)nodes.size() - 1, 0);
}

float Simulation::CalculateNodeDistance(const Node& p1, const Node& p2) {
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

int Simulation::CheckMouseNodeDistance(Vector2 worldMouse) {
    int node = -1;
    for (int i = 0; i < nodes.size(); ++i) {
        if (CalculateMouseDistance(worldMouse, nodes[i]) < NODE_RADIUS) {
            node = i;
            break;
        }
    }
    return node;
}

void Simulation::CenterNodes() {
    for (auto& node : nodes) {
        float dirX = center.x - node.position.x;
        float dirY = center.y - node.position.y;

        node.force.x += dirX * 0.5f;
        node.force.y += dirY * 0.5f;
    }
}

void Simulation::UpdateDraggedNode(Vector2 worldMouse) {
    nodes[draggedNodeId].position = worldMouse;
    nodes[draggedNodeId].force = { 0.0f, 0.0f };
    nodes[draggedNodeId].velocity = { 0.0f, 0.0f };
}

void Simulation::ResetForce() {
    for (auto& node : nodes) {
        node.force = {0.0f, 0.0f};
    }
}

void Simulation::ApplyRepulsionForces() {
    for (int i = 0; i < nodes.size(); ++i) {
        for (int j = i + 1; j < nodes.size(); ++j) {
            float dx = nodes[i].position.x - nodes[j].position.x;
            float dy = nodes[i].position.y - nodes[j].position.y;
            float distance = CalculateNodeDistance(nodes[i], nodes[j]);

            if (distance < 1.0f) distance = 1.0f;

            // Calculate Force Magnitude (Coulomb's Law: F = k / r)
            float forceMagnitude = REPULSION_FORCE / distance;

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
}

void Simulation::UpdateEdges() {
    for (const auto& edge : edges) {
        Node& a = nodes[edge.sourceId];
        Node& b = nodes[edge.targetId];

        Vector2 delta = { a.position.x - b.position.x, a.position.y - b.position.y };
        float dist = sqrt(delta.x*delta.x + delta.y*delta.y);
        
        if (dist == 0) continue;

        float displacement = dist - TARGET_LENGTH;
        float forceMag = SPRING_K * displacement; 

        float fx = (delta.x / dist) * forceMag;
        float fy = (delta.y / dist) * forceMag;

        a.force.x -= fx;
        a.force.y -= fy;
        b.force.x += fx;
        b.force.y += fy;
    }
}

void Simulation::UpdatePositions(float dt) {
    for (int i = 0; i < nodes.size(); ++i) {
        if (i == draggedNodeId) continue;
        Node& node = nodes[i];

        node.velocity.x += node.force.x * dt;
        node.velocity.y += node.force.y * dt;

        node.velocity.x *= 0.95f; 
        node.velocity.y *= 0.95f;

        node.position.x += node.velocity.x * dt;
        node.position.y += node.velocity.y * dt;
    }
}

void Simulation::ResetGraph() {
    hoverNodeId = -1;
    draggedNodeId = -1;
    selectedNodeId = -1;
    nodes.clear();
    edges.clear();
}

void Simulation::Update(float dt, Vector2 worldMouse) {
    hoverNodeId = CheckMouseNodeDistance(worldMouse);

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        int clickedNode = CheckMouseNodeDistance(worldMouse);
        draggedNodeId = clickedNode;
        if (clickedNode != -1) {
            selectedNodeId = clickedNode;
        }
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        draggedNodeId = -1;
    }

    ResetForce();

    ApplyRepulsionForces();

    CenterNodes();

    UpdateEdges();

    UpdatePositions(dt);

    if (draggedNodeId != -1) {
        UpdateDraggedNode(worldMouse);
    }
}
