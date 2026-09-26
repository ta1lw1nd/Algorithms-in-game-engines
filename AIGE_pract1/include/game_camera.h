#pragma once
#include <raylib.h>
#include "log_manager.h"

namespace Engine {

    class GameCamera {
        private:
            Camera camera;
            float speed;
            Vector3 initialPosition;
        public:
            GameCamera(Vector3 initialPosition, float speed);

            void Update(Vector2 moveInput, float deltaTime, LogManager& eventLog);
            void MoveCamera(Vector2 moveInput, float deltaTime);
            void Reset(LogManager& eventLog);

            void Begin3D();
            void End3D();
    };

}
