#pragma once
#include <raylib.h>

struct Node {
    Vector2 position;
    Vector2 velocity = { 0.0f, 0.0f };
    Vector2 force = { 0.0f, 0.0f };
    float mass = 10.0f;
    int id;

    Node(int id, float x, float y) : id(id), position{x, y} {}
};