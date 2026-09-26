#include "log_manager.h"

namespace Engine {

    void LogManager::AddLog(const std::string& message) {
        log.push_back({ message, logDuration });
    }

    void LogManager::Update(float deltaTime) {
        for (size_t i = 0; i < log.size();) {
            log[i].timeRemaining -= deltaTime;
            if (log[i].timeRemaining <= 0.0f) {
                log.erase(log.begin() + i);
            } else {
                ++i;
            }
        }
    }

    void LogManager::Draw(float startX, float startY, const std::string& title) {
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

    void LogManager::Clear() {
        log.clear();
    }

}
