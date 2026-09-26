#pragma once
#include <raylib.h>
#include <string>
#include <vector>

namespace Engine {

    struct LogElement {
        std::string message;
        float timeRemaining;
    };

    // On-screen fading log used for gameplay/debug event messages.
    class LogManager {
        private:
            std::vector<LogElement> log;
            float logDuration;
            int fontSize;
            Color logColor;

        public:
            LogManager(float duration = 3.0f, int size = 18, Color color = WHITE)
                : logDuration(duration), fontSize(size), logColor(color) {}

            void AddLog(const std::string& message);
            void Update(float deltaTime);
            void Draw(float startX, float startY, const std::string& title = "EVENT LOG");
            void Clear();
    };

}
