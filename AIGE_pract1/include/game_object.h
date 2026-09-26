#pragma once
#include <raylib.h>
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
            StaticCube(Vector3 position, Quaternion rotation, Vector3 scale, Color color);

            void Draw() override;
            void Update(float deltaTime) override;
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
            Platform(int gridX, int gridZ, float cubeSize, Color color1, Color color2);

            void Draw();
    };

    class Target : public GameObject {
        public:
            Target(Vector3 position, Quaternion rotation, Vector3 scale);

            void Draw() override;
            void Update(float deltaTime) override;
    };

}
