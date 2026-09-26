#pragma once
#include <raylib.h>
#include <chrono>
#include <string>
#include <vector>

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
            static void BeginFrame();
            static int BeginMarker(const std::string& name, const std::string& category);
            static void EndMarker(int index, float duration);
            static const std::vector<ProfileData>& GetProfileData();
            static void Draw(float endX, float startY, const std::string& title = "PROFILER");
    };

    // RAII helper that times its own scope and records it with the Profiler.
    class ScopeMarker {
        private:
            std::chrono::high_resolution_clock::time_point startTime;
            int markerIndex;

        public:
            ScopeMarker(const std::string& name, const std::string& category);
            ~ScopeMarker();
    };

}
