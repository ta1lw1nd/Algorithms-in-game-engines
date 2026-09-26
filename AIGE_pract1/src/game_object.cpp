#include "game_object.h"

namespace Engine {

    StaticCube::StaticCube(Vector3 position, Quaternion rotation, Vector3 scale, Color color) {
        transform.position = position;
        transform.rotation = rotation;
        transform.scale = scale;
        this->color = color;
    }

    void StaticCube::Draw() {
        DrawCube(transform.position, transform.scale.x, transform.scale.y, transform.scale.z, color);
    }

    void StaticCube::Update(float deltaTime) {

    }

    Platform::Platform(int gridX, int gridZ, float cubeSize, Color color1, Color color2)
        : gridX(gridX), gridZ(gridZ), cubeSize(cubeSize), color1(color1), color2(color2) {
        cubes.reserve(gridX * gridZ);
        for (int i = 0; i < gridX; ++i) {
            for (int j = 0; j < gridZ; ++j) {
                cubes.push_back(StaticCube(
                    Vector3{i * cubeSize - gridX * cubeSize / 2, 0.0f, j * cubeSize - gridZ * cubeSize / 2},
                    Quaternion{0.0f, 0.0f, 0.0f, 1.0f},
                    Vector3{cubeSize, cubeSize, cubeSize},
                    (i + j) % 2 == 0 ? color1 : color2));
            }
        }
    }

    void Platform::Draw() {
        for (auto& cube : cubes) {
            cube.Draw();
        }
    }

    Target::Target(Vector3 position, Quaternion rotation, Vector3 scale) {
        transform.position = position;
        transform.rotation = rotation;
        transform.scale = scale;
    }

    void Target::Draw() {
        DrawCube(transform.position, transform.scale.x, transform.scale.y, transform.scale.z, WHITE);
    }

    void Target::Update(float deltaTime) {
    }

}
