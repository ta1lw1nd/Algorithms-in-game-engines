#include "game_camera.h"
#include <input.h>

namespace Engine {

    GameCamera::GameCamera(Vector3 initialPosition, float speed) : initialPosition(initialPosition), speed(speed) {
        camera.position = initialPosition;
        camera.target = { 0.0f, 0.0f, 0.0f };
        camera.up = { 0.0f, 1.0f, 0.0f };
        camera.fovy = 60.0f;
        camera.projection = CAMERA_PERSPECTIVE;
    }

    void GameCamera::Update(Vector2 moveInput, float deltaTime, LogManager& eventLog) {
        MoveCamera(moveInput, deltaTime);
        if (IsKeyPressed(keyMap.cameraResetPosition)) {
            Reset(eventLog);
        }
    }

    void GameCamera::MoveCamera(Vector2 moveInput, float deltaTime) {
        camera.position.x += moveInput.x * speed * deltaTime;
        camera.position.z += moveInput.y * speed * deltaTime;

        camera.target.x = camera.position.x - initialPosition.x;
        camera.target.z = camera.position.z - initialPosition.z;
    }

    void GameCamera::Reset(LogManager& eventLog) {
        eventLog.AddLog("Camera reset");
        camera.position = initialPosition;
        camera.target = { 0.0f, 0.0f, 0.0f };
    }

    void GameCamera::Begin3D() {
        BeginMode3D(camera);
    }

    void GameCamera::End3D() {
        EndMode3D();
    }

}
