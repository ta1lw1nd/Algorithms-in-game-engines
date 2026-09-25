#include <cstddef>
#include <iterator>
#include <raylib.h>
#include <raymath.h>
#include <input.h>
#include <chrono>
#include <string>
#include <vector>
#include <utility>

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

template <typename T>
void swap_and_pop(std::vector<T>& vec, int index) {
    vec[index] = std::move(vec.back());
    vec.pop_back();
}

namespace Engine {
    struct ProfileData {
            std::string name;
            std::string category;
            float duration;
            int depth;
            std::vector<int> children;
        };

    class Profiler {
        private:
            inline static std::vector<ProfileData> profileData;
            inline static std::vector<ProfileData> displayData;
            inline static std::vector<int> activeMarkers;

        public:
            static void BeginFrame() {
                displayData = std::move(profileData);
                profileData.clear();
                activeMarkers.clear();
            }

            static int BeginMarker(const std::string& name, const std::string& category) {
                int index = static_cast<int>(profileData.size());

                ProfileData data;
                data.name = name;
                data.category = category;
                data.duration = 0.0f;
                data.depth = static_cast<int>(activeMarkers.size());

                if (!activeMarkers.empty()) {
                    profileData[activeMarkers.back()].children.push_back(index);
                }

                profileData.push_back(data);
                activeMarkers.push_back(index);

                return index;
            }

            static void EndMarker(int index, float duration) {
                if (!activeMarkers.empty()) {
                    profileData[index].duration = duration;
                    activeMarkers.pop_back();
                }
            }

            static const std::vector<ProfileData>& GetProfileData() {
                return profileData;
            }

            static void Draw(float endX, float startY, const std::string& title = "PROFILER") {
                if (displayData.empty()) return;

                float frameDuration = displayData[0].duration;

                float padding = 12.0f;
                float lineSpacing = 6.0f;
                int fontSize = 18;
                float titleFontSize = fontSize + 4.0f;
                float headerHeight = titleFontSize + 10.0f;
                float indentSize = 14.0f;
                float columnGap = 20.0f;

                std::string fpsText = TextFormat("%d FPS", GetFPS());
                float titleWidth = MeasureText(title.c_str(), titleFontSize);
                float fpsWidth = MeasureText(fpsText.c_str(), fontSize);

                float maxNameWidth = titleWidth + columnGap + fpsWidth;
                float maxTimeWidth = 0.0f;
                float maxPercentWidth = 0.0f;

                for (const auto& data : displayData) {
                    float nameWidth = MeasureText(data.name.c_str(), fontSize) + data.depth * indentSize;
                    if (nameWidth > maxNameWidth) maxNameWidth = nameWidth;

                    float timeWidth = MeasureText(TextFormat("%.1f ms", data.duration), fontSize);
                    if (timeWidth > maxTimeWidth) maxTimeWidth = timeWidth;

                    float percent = frameDuration > 0.0f ? (data.duration / frameDuration) * 100.0f : 0.0f;
                    float percentWidth = MeasureText(TextFormat("%.0f%%", percent), fontSize);
                    if (percentWidth > maxPercentWidth) maxPercentWidth = percentWidth;
                }

                float panelWidth = padding + maxNameWidth + columnGap + maxTimeWidth + columnGap + maxPercentWidth + padding;
                float startX = endX - panelWidth;
                float lineHeight = fontSize + lineSpacing;
                float panelHeight = padding + headerHeight + (displayData.size() * lineHeight) + padding;

                Rectangle panelRect = { startX, startY, panelWidth, panelHeight };

                DrawRectangleRec(panelRect, ColorAlpha(DARKGRAY, 0.85f));
                DrawRectangle((int)startX, (int)startY, (int)panelWidth, (int)(headerHeight + padding / 2), ColorAlpha(BLACK, 0.4f));
                DrawRectangleLinesEx(panelRect, 2.0f, ColorAlpha(WHITE, 0.8f));

                DrawText(title.c_str(), (int)(startX + padding), (int)(startY + padding / 2), (int)titleFontSize, WHITE);
                DrawText(fpsText.c_str(), (int)(startX + panelWidth - padding - fpsWidth), (int)(startY + padding / 2), fontSize, WHITE);

                float lineY = startY + headerHeight + 2.0f;
                DrawLineEx({ startX + padding / 2, lineY }, { startX + panelWidth - padding / 2, lineY }, 1.5f, ColorAlpha(WHITE, 0.5f));

                float timeColumnX = startX + padding + maxNameWidth + columnGap;
                float percentColumnX = timeColumnX + maxTimeWidth + columnGap;
                float currentY = lineY + padding;

                for (const auto& data : displayData) {
                    float nameX = startX + padding + data.depth * indentSize;
                    float percent = frameDuration > 0.0f ? (data.duration / frameDuration) * 100.0f : 0.0f;

                    DrawText(data.name.c_str(), (int)nameX, (int)currentY, fontSize, WHITE);
                    DrawText(TextFormat("%.1f ms", data.duration), (int)timeColumnX, (int)currentY, fontSize, WHITE);
                    DrawText(TextFormat("%.0f%%", percent), (int)percentColumnX, (int)currentY, fontSize, WHITE);

                    currentY += lineHeight;
                }
            }
        };

        class ScopeMarker {
            private:
                std::chrono::high_resolution_clock::time_point startTime;
                int markerIndex;

            public:
                ScopeMarker(const std::string& name, const std::string& category) {
                    startTime = std::chrono::high_resolution_clock::now();
                    markerIndex = Profiler::BeginMarker(name, category);
                }

                ~ScopeMarker() {
                    auto endTime = std::chrono::high_resolution_clock::now();
                    float duration = std::chrono::duration<float, std::milli>(endTime - startTime).count();
                    Profiler::EndMarker(markerIndex, duration);
                }
        };

    struct LogElement {
        std::string message;
        float timeRemaining;
    };

    class LogManager {
        private:
            std::vector<LogElement> log;
            float logDuration;
            int fontSize;
            Color logColor;

        public:
            LogManager(float duration = 3.0f, int size = 18, Color color = WHITE)
                : logDuration(duration), fontSize(size), logColor(color) {}

            void AddLog(const std::string& message) {
                log.push_back({ message, logDuration });
            }

            void Update(float deltaTime) {
                for (size_t i = 0; i < log.size();) {
                    log[i].timeRemaining -= deltaTime;
                    if (log[i].timeRemaining <= 0.0f) {
                        log.erase(log.begin() + i);
                    } else {
                        ++i;
                    }
                }
            }

            void Draw(float startX, float startY, const std::string& title = "EVENT LOG") {
                if (log.empty()) return;

                float padding = 12.0f;
                float lineSpacing = 6.0f;
                float titleFontSize = fontSize + 4.0f;
                float headerHeight = titleFontSize + 10.0f;

                float maxTextWidth = MeasureText(title.c_str(), titleFontSize);

                for (const auto& elem : log) {
                    float textWidth = MeasureText(elem.message.c_str(), fontSize);
                    if (textWidth > maxTextWidth) {
                        maxTextWidth = textWidth;
                    }
                }

                float panelWidth = maxTextWidth + (padding * 2.0f);
                float lineHeight = fontSize + lineSpacing;
                float panelHeight = padding + headerHeight + (log.size() * lineHeight) + padding;

                Rectangle panelRect = { startX, startY, panelWidth, panelHeight };

                DrawRectangleRec(panelRect, ColorAlpha(DARKGRAY, 0.85f));
                DrawRectangle((int)startX, (int)startY, (int)panelWidth, (int)(headerHeight + padding / 2), ColorAlpha(BLACK, 0.4f));
                DrawRectangleLinesEx(panelRect, 2.0f, ColorAlpha(WHITE, 0.8f));

                DrawText(title.c_str(), (int)(startX + padding), (int)(startY + padding / 2), (int)titleFontSize, WHITE);

                float lineY = startY + headerHeight + 2.0f;
                DrawLineEx({ startX + padding / 2, lineY }, { startX + panelWidth - padding / 2, lineY }, 1.5f, ColorAlpha(WHITE, 0.5f));

                float currentY = lineY + padding;
                for (size_t i = 0; i < log.size(); ++i) {
                    float alpha = log[i].timeRemaining / logDuration;
                    if (alpha > 1.0f) alpha = 1.0f;

                    Color textColor = ColorAlpha(logColor, alpha);

                    DrawText(log[i].message.c_str(), (int)(startX + padding), (int)currentY, fontSize, textColor);

                    currentY += lineHeight;
                }
            }

            void Clear() {
                log.clear();
            }
    };

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

                Vector3 GetPosition() { return transform.position; }
                void SetPosition(const Vector3& position) { transform.position = position; }

                Quaternion GetRotation() { return transform.rotation; }
                void SetRotation(const Quaternion& rotation) { transform.rotation = rotation; }

                Vector3 GetScale() { return transform.scale; }
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

            void Update(Vector2 moveInput, float deltaTime, LogManager& eventLog) {
                MoveCamera(moveInput, deltaTime);
                if(IsKeyPressed(keyMap.cameraResetPosition)) {
                    Reset(eventLog);
                }
            }

            void MoveCamera(Vector2 moveInput, float deltaTime) {
                camera.position.x += moveInput.x * speed * deltaTime;
                camera.position.z += moveInput.y * speed * deltaTime;

                camera.target.x = camera.position.x - initialPosition.x;
                camera.target.z = camera.position.z - initialPosition.z;
            }

            void Reset(LogManager& eventLog) {
                eventLog.AddLog("Camera reset");
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
            bool reachedTarget = false;
        public:
            Enemy() : speed(0.0f), color(WHITE), targetPosition({ 0.0f, 0.0f, 0.0f }) {

            }

            void Spawn(Vector3 spawnPosition, Vector3 targetPosition, Quaternion rotation, Vector3 scale, float speed, Color color, LogManager& eventLog) {
                eventLog.AddLog("Enemy spawned");
                transform.position = spawnPosition;
                transform.rotation = rotation;
                transform.scale = scale;
                this->targetPosition = targetPosition;
                this->speed = speed;
                this->color = color;
                this->reachedTarget = false;
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
                if (distance > 0.5f) {
                    Vector3 normalizedDirection = Vector3Normalize(direction);
                    transform.position += normalizedDirection * speed * deltaTime;
                } else {
                    reachedTarget = true;
                }
            }

            bool HasReachedTarget() const {
                return reachedTarget;
            }

            void SetTargetPosition(Vector3 targetPosition) {
                this->targetPosition = targetPosition;
            }
    };

    class EnemyManager {
        private:
            Vector3 centerPosition;
            Vector3 targetPosition;
            std::vector<Enemy> pool;
            size_t activeEnemies;
            float spawnPeriod;
            float spawnTimer;
            size_t maxEnemies;
            float spawnRadius;
            float minSpeed;
            float maxSpeed;
        public:
            EnemyManager(Vector3 centerPosition, Vector3 targetPosition, float spawnPeriod, int maxEnemies, float spawnRadius, float minSpeed, float maxSpeed) : centerPosition(centerPosition), targetPosition(targetPosition), spawnPeriod(spawnPeriod), maxEnemies(maxEnemies), spawnRadius(spawnRadius), minSpeed(minSpeed), maxSpeed(maxSpeed) {
                pool.resize(maxEnemies);
                activeEnemies = 0;
                spawnTimer = 0.0f;
            }

            Color calculateColor() {
                    return { (unsigned char)GetRandomValue(0, 255), (unsigned char)GetRandomValue(0, 255), (unsigned char)GetRandomValue(0, 255), 255 };
            }

            float calculateSpeed() {
                return GetRandomFloat(minSpeed, maxSpeed);
            }

            Vector3 calculateSpawnPosition() {
                float angle = GetRandomFloat(0, 360) * DEG2RAD;
                float distance = spawnRadius;
                return { centerPosition.x + distance * cos(angle), 1.0, centerPosition.z + distance * sin(angle) };
            }

            void SpawnEnemy(LogManager& eventLog) {
                if (activeEnemies < maxEnemies && spawnTimer >= spawnPeriod) {
                    pool[activeEnemies].Spawn(calculateSpawnPosition(), targetPosition, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, calculateSpeed(), calculateColor(), eventLog);
                    activeEnemies++;
                    spawnTimer = 0.0f;
                }
            }

            void Update(float deltaTime, LogManager& eventLog) {
                spawnTimer += deltaTime;
                SpawnEnemy(eventLog);
                for(size_t i = 0; i < activeEnemies; i++) {
                    pool[i].SetTargetPosition(targetPosition);
                    pool[i].Update(deltaTime);

                    if(pool[i].HasReachedTarget()) {
                        eventLog.AddLog("Enemy reached target");
                        std::swap(pool[i], pool[activeEnemies - 1]);
                        activeEnemies--;
                        i--;
                    }
                }
            }

            void Draw() {
                for(size_t i = 0; i < activeEnemies; i++) {
                    pool[i].Draw();
                }
            }
    };

}


using namespace Engine;

int main() {
    LogManager eventLog(3.0f);
    bool toDrawLog = true;
    //Setup settings
    const unsigned int screenWidth = 1600;
    const unsigned int screenHeight = 900;
    int targetFPS = 5000;
    InitWindow(screenWidth, screenHeight, "AIGE_pract1");
    SetTargetFPS(targetFPS);

    eventLog.AddLog("Game window initialized successfully");

    GameCamera camera({ 0.0f, 10.0f, 15.0f }, 100.0f);

    Platform platform(100, 100, 1.0f, {150, 150, 150, 255}, {175, 175, 175, 255});
    Target target({ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 2.0f, 1.0f });

    EnemyManager enemyManager({ 0.0f, 0.0f, 0.0f }, target.GetPosition(), 0.1f, 100, 20.0f, 1.0f, 5.0f);

    while (!WindowShouldClose()) {
        Profiler::BeginFrame();
        ScopeMarker frameMarker("Frame", "Frame");

        float deltaTime = GetFrameTime();
        if (IsKeyPressed(keyMap.debugLogToggle)) {
            toDrawLog = !toDrawLog;
        }

        {
            ScopeMarker marker("Enemies", "Update");
            enemyManager.Update(deltaTime, eventLog);
        }
        eventLog.Update(deltaTime);

        BeginDrawing();
        ClearBackground({40, 50, 70, 255});
        camera.Begin3D();

        {
            ScopeMarker marker("Camera", "Update");
            camera.Update(moveInput(), deltaTime, eventLog);
        }

        {
            ScopeMarker marker("Draw", "Render");

            {
                ScopeMarker subMarker("Enemies", "Render");
                enemyManager.Draw();
            }
            {
                ScopeMarker subMarker("Platform", "Render");
                platform.Draw();
            }
            {
                ScopeMarker subMarker("Target", "Render");
                target.Draw();
            }
        }

        camera.End3D();
        //DrawText(TextFormat("FPS: %d", GetFPS()), screenWidth / 2, 5, 18, WHITE);
        if (toDrawLog) eventLog.Draw(15.0f, 15.0f, "SYSTEM LOG");
        Profiler::Draw(screenWidth - 15.0f, 15.0f, "PROFILER");

        {
            ScopeMarker marker("End Drawing", "Render");
            EndDrawing();
        }
    }

    return 0;
}
