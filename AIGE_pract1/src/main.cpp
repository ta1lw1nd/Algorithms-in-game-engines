#include <raylib.h>
#include <input.h>

#include "utils.h"
#include "profiler.h"
#include "log_manager.h"
#include "game_object.h"
#include "game_camera.h"
#include "enemy.h"

using namespace Engine;

int main() {
    LogManager eventLog(3.0f);
    bool toDrawLog = true;
    bool toDrawScopeMarkers = true;
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
        if (IsKeyPressed(keyMap.debugScopeMarkerToggle)) {
            toDrawScopeMarkers = !toDrawScopeMarkers;
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
        if (toDrawScopeMarkers) Profiler::Draw(screenWidth - 15.0f, 15.0f, "PROFILER");

        {
            ScopeMarker marker("End Drawing", "Render");
            EndDrawing();
        }
    }

    return 0;
}
