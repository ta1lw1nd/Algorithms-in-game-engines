#pragma once
#include <raylib.h>
#include <vector>
#include <utility>

// Returns the movement input vector based on the current key bindings.
Vector2 moveInput();

// Returns a random float in the range [min, max].
float GetRandomFloat(float min, float max);

// Removes an element from a vector in O(1) by swapping it with the last
// element and popping the back. Does not preserve order.
template <typename T>
void swap_and_pop(std::vector<T>& vec, int index) {
    vec[index] = std::move(vec.back());
    vec.pop_back();
}
