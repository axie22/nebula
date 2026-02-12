#pragma once
#include <raylib.h>

struct Node {
    Vector2 position;
    Vector2 velocity;
    Vector2 force;
    float mass = 10.0f;
    int id;
};