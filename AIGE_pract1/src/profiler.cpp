#include "profiler.h"

namespace Engine {

    void Profiler::BeginFrame() {
        displayData = std::move(profileData);
        profileData.clear();
        activeMarkers.clear();
    }

    int Profiler::BeginMarker(const std::string& name, const std::string& category) {
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

    void Profiler::EndMarker(int index, float duration) {
        if (!activeMarkers.empty()) {
            profileData[index].duration = duration;
            activeMarkers.pop_back();
        }
    }

    const std::vector<ProfileData>& Profiler::GetProfileData() {
        return profileData;
    }

    void Profiler::Draw(float endX, float startY, const std::string& title) {
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

    ScopeMarker::ScopeMarker(const std::string& name, const std::string& category) {
        startTime = std::chrono::high_resolution_clock::now();
        markerIndex = Profiler::BeginMarker(name, category);
    }

    ScopeMarker::~ScopeMarker() {
        auto endTime = std::chrono::high_resolution_clock::now();
        float duration = std::chrono::duration<float, std::milli>(endTime - startTime).count();
        Profiler::EndMarker(markerIndex, duration);
    }

}
