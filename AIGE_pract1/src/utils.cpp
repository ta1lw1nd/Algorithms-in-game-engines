#include "utils.h"
#include <input.h>

Vector2 moveInput() {
    Vector2 input = { 0.0f, 0.0f };
    input.x = IsKeyDown(keyMap.moveRight) - IsKeyDown(keyMap.moveLeft);
    input.y = IsKeyDown(keyMap.moveDown) - IsKeyDown(keyMap.moveUp);
    return input;
}

float GetRandomFloat(float min, float max) {
    float scale = (float)GetRandomValue(0, 10000) / 10000.0f;
    return min + scale * (max - min);
}
