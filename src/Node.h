#pragma once
#include <raylib.h>

struct Node {
    // Physics properties
    Vector2 position;
    Vector2 velocity = { 0.0f, 0.0f };
    Vector2 force = { 0.0f, 0.0f };
    float mass = 10.0f;

    // File properties
    int id;
    std::string name;
    int indegree = 0;
    int outdegree = 0;

    Node(int id, std::string name, float x, float y) : id(id), position{x, y}, name(name) {}
};