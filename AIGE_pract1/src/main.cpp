#include <raylib.h>
#include <raymath.h>
#include <input.h>
#include <vector>

namespace Engine {
    struct Transform {
        Vector3 position = { 0.0f, 0.0f, 0.0f };
        Quaternion rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
        Vector3 scale = { 1.0f, 1.0f, 1.0f };
    };

    class GameObject {
            protected:
                Transform transform;
                BoundingBox bounds;
            public:
                virtual void Draw() = 0;
                virtual void Update(float deltaTime) = 0;
                virtual ~GameObject() = default;

                Vector3 GetPosition() const { return transform.position; }
                void SetPosition(const Vector3& position) { transform.position = position; }

                Quaternion GetRotation() const { return transform.rotation; }
                void SetRotation(const Quaternion& rotation) { transform.rotation = rotation; }

                Vector3 GetScale() const { return transform.scale; }
                void SetScale(const Vector3& scale) { transform.scale = scale; }
        };

    class StaticCube : public GameObject {
        private:
            Color color;

        public:
            StaticCube(Vector3 position, Quaternion rotation, Vector3 scale, Color color) {
                transform.position = position;
                transform.rotation = rotation;
                transform.scale = scale;
                this->color = color;
            }

            void Draw() override {
                DrawCube(transform.position, transform.scale.x, transform.scale.y, transform.scale.z, color);
            }

            void Update(float deltaTime) override {

            }
    };

    class Platform {
        private:
            std::vector<StaticCube> cubes;
            int gridX;
            int gridZ;
            float cubeSize;
            Color color1;
            Color color2;
        public:
            Platform(int gridX, int gridZ, float cubeSize, Color color1, Color color2) : gridX(gridX), gridZ(gridZ), cubeSize(cubeSize), color1(color1), color2(color2) {
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

            void Draw() {
                for (auto& cube : cubes) {
                    cube.Draw();
                }
            }
    };

    class Target : public GameObject {
        public:
            Target(Vector3 position, Quaternion rotation, Vector3 scale)  {
                transform.position = position;
                transform.rotation = rotation;
                transform.scale = scale;
            }
            void Draw() override {
                DrawCube(transform.position, transform.scale.x, transform.scale.y, transform.scale.z, WHITE);
            }
            void Update(float deltaTime) override {
            }

            Vector3 GetPosition() {
                return transform.position;
            }
    };

    class GameCamera {
        private:
            Camera camera;
            float speed;
            Vector3 initialPosition;
        public:
            GameCamera(Vector3 initialPosition, float speed) : initialPosition(initialPosition), speed(speed) {
                camera.position = initialPosition;
                camera.target = { 0.0f, 0.0f, 0.0f };
                camera.up = { 0.0f, 1.0f, 0.0f };
                camera.fovy = 60.0f;
                camera.projection = CAMERA_PERSPECTIVE;
            }

            void Update(Vector2 moveInput, float deltaTime) {
                MoveCamera(moveInput, deltaTime);
                if(IsKeyPressed(keyMap.cameraResetPosition)) {
                    Reset();
                }
            }

            void MoveCamera(Vector2 moveInput, float deltaTime) {
                camera.position.x += moveInput.x * speed * deltaTime;
                camera.position.z += moveInput.y * speed * deltaTime;

                camera.target.x = camera.position.x - initialPosition.x;
                camera.target.z = camera.position.z - initialPosition.z;
            }

            void Reset() {
                camera.position = initialPosition;
                camera.target = { 0.0f, 0.0f, 0.0f };
            }

            void Begin3D() {
                BeginMode3D(camera);
            }

            void End3D() {
                EndMode3D();
            }
    };

    class Enemy : public GameObject {
        private:
            float speed;
            Color color;
            Vector3 targetPosition;
        public:
            Enemy(Vector3 spawnPosition, Vector3 targetPosition, float speed, Color color) : speed(speed), color(color), targetPosition(targetPosition) {
                transform.position = spawnPosition;

            }


            void Draw() override {
                DrawCube(transform.position, transform.scale.x, transform.scale.y, transform.scale.z, color);
            }

            void Update(float deltaTime) override {
                MoveTowardsTarget(targetPosition, deltaTime);
            }

            void MoveTowardsTarget(Vector3 targetPosition, float deltaTime) {
                Vector3 direction = targetPosition - transform.position;
                float distance = Vector3Length(direction);
                if (distance > 0) {
                    Vector3 normalizedDirection = Vector3Normalize(direction);
                    transform.position += normalizedDirection * speed * deltaTime;
                }
            }
    };

    class EnemyManager {
        private:
            Vector3 centerPosition;
            Vector3 targetPosition;
            std::vector<Enemy> enemies;
            float spawnPeriod;
            float spawnTimer;
            int maxEnemies;
            float spawnRadius;
            float minSpeed;
            float maxSpeed;
        public:
            EnemyManager(Vector3 centerPosition, Vector3 targetPosition, float spawnPeriod, int maxEnemies, float spawnRadius, float minSpeed, float maxSpeed) : centerPosition(centerPosition), targetPosition(targetPosition), spawnPeriod(spawnPeriod), maxEnemies(maxEnemies), spawnRadius(spawnRadius), minSpeed(minSpeed), maxSpeed(maxSpeed) {
                enemies.reserve(maxEnemies);
                spawnTimer = 0.0f;

                for (int i = 0; i < maxEnemies; ++i) {
                    enemies.push_back(Enemy(calculateSpawnPosition(), targetPosition, calculateSpeed(), calculateColor()));
                }
            }

                Color calculateColor() {
                    return { (unsigned char)GetRandomValue(0, 255), (unsigned char)GetRandomValue(0, 255), (unsigned char)GetRandomValue(0, 255), 255 };
            }

            float calculateSpeed() {
                return GetRandomValue(minSpeed, maxSpeed);
            }

            Vector3 calculateSpawnPosition() {
                float angle = GetRandomValue(0, 360);
                float distance = spawnRadius;
                return { centerPosition.x + distance * cos(angle), 1.0, centerPosition.z + distance * sin(angle) };
            }

            void Update(float deltaTime) {
                for (Enemy& enemy : enemies) {
                    enemy.Update(deltaTime);
                }
            }

            void Draw() {
                for (Enemy& enemy : enemies) {
                    enemy.Draw();
                }
            }

    };
}



Vector2 moveInput() {
    Vector2 input = { 0.0f, 0.0f };
    input.x = IsKeyDown(keyMap.moveRight) - IsKeyDown(keyMap.moveLeft);
    input.y = IsKeyDown(keyMap.moveDown) - IsKeyDown(keyMap.moveUp);
    return input;
}

using namespace Engine;

int main() {
    //Setup settings
    const unsigned int screenWidth = 1600;
    const unsigned int screenHeight = 900;
    int targetFPS = 180;

    InitWindow(screenWidth, screenHeight, "AIGE_pract1");
    SetTargetFPS(targetFPS);

    GameCamera camera({ 0.0f, 50.0f, 100.0f }, 100.0f);

    Platform platform(100, 100, 1.0f, {150, 150, 150, 255}, {175, 175, 175, 255});
    Target target({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 2.0f, 1.0f });

    EnemyManager enemyManager({ 0.0f, 0.0f, 0.0f }, target.GetPosition(), 0.1f, 100, 20.0f, 1.0f, 5.0f);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        enemyManager.Update(deltaTime);

        BeginDrawing();
        ClearBackground({40, 50, 70, 255});
        camera.Begin3D();
        camera.Update(moveInput(), deltaTime);

        enemyManager.Draw();

        platform.Draw();
        target.Draw();


        camera.End3D();
        EndDrawing();
    }

    return 0;
}
